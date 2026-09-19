#ifndef CLIPO_DATABASE_H
#define CLIPO_DATABASE_H

#include <QString>
#include <vector>

/**
 * @brief Represents a single clipboard history entry.
 *
 * This structure contains the information required by the UI
 * and database layer to represent one clipboard item.
 */
struct ClipboardEntry
{
    int id;
    QString content;
    long long timestamp;
};

/**
 * @brief Provides persistent storage for Clipo clipboard history.
 *
 * The Database class is responsible only for persistence.
 * Clipboard monitoring and UI behavior are intentionally kept
 * outside this class.
 *
 * SQLite is used as the local storage engine because it is:
 *
 * - Lightweight
 * - Serverless
 * - Fast for local applications
 * - Cross-platform
 * - Easy to distribute with Clipo
 */
class Database
{
public:

    /**
     * @brief Creates a database manager.
     */
    Database();

    /**
     * @brief Closes the database connection.
     */
    ~Database();

    /**
     * @brief Initializes the SQLite database.
     *
     * Creates the database file and required tables if they
     * do not already exist.
     *
     * @return true if initialization succeeds.
     */
    bool initialize();

    /**
     * @brief Adds a clipboard item to history.
     *
     * If the same content already exists, a new duplicate is
     * not created. Instead, the existing item is moved to the
     * top of the history.
     *
     * @param content Clipboard text.
     *
     * @return true if the operation succeeds.
     */
    bool addEntry(const QString &content);

    /**
     * @brief Returns clipboard history.
     *
     * Results are ordered from newest to oldest.
     *
     * @param limit Maximum number of entries to return.
     *
     * @return Clipboard history entries.
     */
    std::vector<ClipboardEntry> getHistory(int limit = 50);

    /**
     * @brief Deletes one clipboard history entry.
     *
     * @param id Database ID of the entry.
     *
     * @return true if the entry was deleted successfully.
     */
    bool deleteEntry(int id);

    /**
     * @brief Deletes all clipboard history.
     *
     * @return true if the history was cleared successfully.
     */
    bool clearHistory();

private:

    /**
     * @brief Returns the location of the SQLite database.
     */
    QString databasePath() const;

    /**
     * @brief SQLite database handle.
     *
     * Stored as void* in the header so that SQLite's implementation
     * details do not leak into other parts of the application.
     */
    void *m_database;
};

#endif // CLIPO_DATABASE_H