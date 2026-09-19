#include "Database.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>

#include <sqlite3.h>

/**
 * @brief Creates a database manager.
 */
Database::Database()
    : m_database(nullptr)
{
}

/**
 * @brief Closes the SQLite database connection.
 */
Database::~Database()
{
    if (m_database != nullptr) {

        sqlite3_close(
            static_cast<sqlite3 *>(m_database)
        );

        m_database = nullptr;
    }
}

/**
 * @brief Returns the path where Clipo stores its database.
 */
QString Database::databasePath() const
{
    const QString dataDirectory =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
        );

    return dataDirectory + "/clipo.db";
}

/**
 * @brief Initializes SQLite and creates the history table.
 */
bool Database::initialize()
{
    const QString path = databasePath();

    QFileInfo databaseInfo(path);

    QDir directory = databaseInfo.dir();

    if (!directory.exists()) {

        if (!directory.mkpath(".")) {

            qWarning()
                << "Failed to create database directory:"
                << directory.absolutePath();

            return false;
        }
    }

    sqlite3 *database = nullptr;

    const int result =
        sqlite3_open(
            path.toUtf8().constData(),
            &database
        );

    if (result != SQLITE_OK) {

        qWarning()
            << "Failed to open SQLite database:"
            << sqlite3_errmsg(database);

        if (database != nullptr) {
            sqlite3_close(database);
        }

        return false;
    }

    m_database = database;

    /*
     * Clipboard history table.
     *
     * timestamp represents the most recent time the item
     * was copied. This allows duplicate clipboard items
     * to be moved to the top without creating duplicates.
     */
    const char *createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS clipboard_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            content TEXT NOT NULL UNIQUE,
            timestamp INTEGER NOT NULL
        );
    )";

    char *errorMessage = nullptr;

    const int tableResult =
        sqlite3_exec(
            database,
            createTableSQL,
            nullptr,
            nullptr,
            &errorMessage
        );

    if (tableResult != SQLITE_OK) {

        qWarning()
            << "Failed to create clipboard_history table:"
            << errorMessage;

        sqlite3_free(errorMessage);

        return false;
    }

    qDebug()
        << "Clipo database initialized:"
        << path;

    return true;
}

/**
 * @brief Adds a clipboard item to history.
 *
 * SQLite's INSERT ... ON CONFLICT behavior is used so that
 * duplicate clipboard content does not create multiple rows.
 *
 * Instead, the existing item's timestamp is updated, which
 * effectively moves it to the top of the history.
 */
bool Database::addEntry(const QString &content)
{
    if (m_database == nullptr) {

        qWarning()
            << "Database is not initialized.";

        return false;
    }

    if (content.isEmpty()) {
        return false;
    }

    sqlite3 *database =
        static_cast<sqlite3 *>(m_database);

    const char *sql = R"(
        INSERT INTO clipboard_history
            (content, timestamp)
        VALUES
            (?, strftime('%s', 'now'))

        ON CONFLICT(content)
        DO UPDATE SET
            timestamp = excluded.timestamp;
    )";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK) {

        qWarning()
            << "Failed to prepare INSERT statement:"
            << sqlite3_errmsg(database);

        return false;
    }

    const QByteArray utf8Content =
        content.toUtf8();

    sqlite3_bind_text(
        statement,
        1,
        utf8Content.constData(),
        utf8Content.size(),
        SQLITE_TRANSIENT
    );

    const int result =
        sqlite3_step(statement);

    sqlite3_finalize(statement);

    if (result != SQLITE_DONE) {

        qWarning()
            << "Failed to store clipboard entry:"
            << sqlite3_errmsg(database);

        return false;
    }

    return true;
}

/**
 * @brief Retrieves clipboard history.
 */
std::vector<ClipboardEntry>
Database::getHistory(int limit)
{
    std::vector<ClipboardEntry> entries;

    if (m_database == nullptr) {
        return entries;
    }

    sqlite3 *database =
        static_cast<sqlite3 *>(m_database);

    const char *sql = R"(
        SELECT id, content, timestamp
        FROM clipboard_history
        ORDER BY timestamp DESC, id DESC
        LIMIT ?;
    )";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK) {

        qWarning()
            << "Failed to prepare SELECT statement:"
            << sqlite3_errmsg(database);

        return entries;
    }

    sqlite3_bind_int(
        statement,
        1,
        limit
    );

    while (sqlite3_step(statement) == SQLITE_ROW) {

        ClipboardEntry entry;

        entry.id =
            sqlite3_column_int(statement, 0);

        const char *text =
            reinterpret_cast<const char *>(
                sqlite3_column_text(statement, 1)
            );

        entry.content =
            QString::fromUtf8(
                text ? text : ""
            );

        entry.timestamp =
            sqlite3_column_int64(statement, 2);

        entries.push_back(entry);
    }

    sqlite3_finalize(statement);

    return entries;
}

/**
 * @brief Deletes a single clipboard entry.
 */
bool Database::deleteEntry(int id)
{
    if (m_database == nullptr) {
        return false;
    }

    sqlite3 *database =
        static_cast<sqlite3 *>(m_database);

    const char *sql = R"(
        DELETE FROM clipboard_history
        WHERE id = ?;
    )";

    sqlite3_stmt *statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK) {

        return false;
    }

    sqlite3_bind_int(
        statement,
        1,
        id
    );

    const int result =
        sqlite3_step(statement);

    sqlite3_finalize(statement);

    return result == SQLITE_DONE;
}

/**
 * @brief Removes all clipboard history.
 */
bool Database::clearHistory()
{
    if (m_database == nullptr) {
        return false;
    }

    sqlite3 *database =
        static_cast<sqlite3 *>(m_database);

    const char *sql =
        "DELETE FROM clipboard_history;";

    char *errorMessage = nullptr;

    const int result =
        sqlite3_exec(
            database,
            sql,
            nullptr,
            nullptr,
            &errorMessage
        );

    if (result != SQLITE_OK) {

        qWarning()
            << "Failed to clear clipboard history:"
            << errorMessage;

        sqlite3_free(errorMessage);

        return false;
    }

    return true;
}