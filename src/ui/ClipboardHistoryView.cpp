#include "ClipboardHistoryView.h"

#include "../database/Database.h"

#include <QAbstractItemView>
#include <QListWidgetItem>
#include <QSize>

#include <vector>


/**
 * @brief Creates the clipboard history view.
 */
ClipboardHistoryView::ClipboardHistoryView(
    Database &database,
    QWidget *parent
)
    : QListWidget(parent),
      m_database(database),
      m_currentSearch()
{
    /*
     * ---------------------------------------------------------
     * Selection
     * ---------------------------------------------------------
     *
     * Only one clipboard item can be selected at a time.
     */
    setSelectionMode(
        QAbstractItemView::SingleSelection
    );


    /*
     * ---------------------------------------------------------
     * Scrolling
     * ---------------------------------------------------------
     *
     * Clipboard content should never create a horizontal
     * scrollbar.
     */
    setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );


    /*
     * Allow long clipboard content to wrap when an item
     * is expanded.
     */
    setWordWrap(
        true
    );


    /*
     * Elide long compact previews rather than creating a
     * horizontal scrolling area.
     */
    setTextElideMode(
        Qt::ElideRight
    );


    /*
     * Allow items to have different heights.
     *
     * Compact items and expanded items have different heights.
     */
    setUniformItemSizes(
        false
    );


    /*
     * Clicking a clipboard item expands it.
     */
    connect(
        this,
        &QListWidget::itemClicked,
        this,
        &ClipboardHistoryView::expandItem
    );

    // Double-clicking a clipboard item requests a paste.
    connect(
        this,
        &QListWidget::itemDoubleClicked,
        this,
        [this](QListWidgetItem *item)
        {
            // Ignore invalid items.
            if (item == nullptr) {
                return;
            }

            // Ignore the "No clipboard history yet" item.
            if (!item->data(Qt::UserRole).isValid()) {
                return;
            }

            // Tell PopupWindow to paste the currently selected item.
            emit itemDoubleClicked();
        }
    );


    /*
     * Load the initial history.
     */
    loadHistory();
}


/**
 * @brief Reloads clipboard history.
 */
void ClipboardHistoryView::refresh()
{
    /*
     * Reload everything from SQLite.
     *
     * loadHistory() will re-apply the current search query.
     */
    loadHistory();
}


/**
 * @brief Creates a compact preview of clipboard content.
 */
QString ClipboardHistoryView::createPreview(
    const QString &content
) const
{
    /*
     * Start with the complete clipboard content.
     */
    QString preview = content;


    /*
     * Replace newlines with a visible separator.
     *
     * This keeps normal history entries compact.
     */
    preview.replace(
        '\n',
        " ↵ "
    );

    preview.replace(
        '\r',
        ""
    );


    /*
     * Collapse unnecessary whitespace.
     */
    preview = preview.simplified();


    /*
     * Limit the visible preview.
     *
     * The original clipboard content remains stored inside
     * QListWidgetItem::UserRole + 1.
     */
    constexpr int maxPreviewLength = 100;

    if (preview.length() > maxPreviewLength) {

        preview =
            preview.left(maxPreviewLength)
            + "...";
    }


    return preview;
}


/**
 * @brief Loads clipboard history from SQLite.
 */
void ClipboardHistoryView::loadHistory()
{
    /*
     * Remove the current visual entries.
     */
    clear();


    /*
     * Ask the database for the newest 50 clipboard entries.
     */
    const std::vector<ClipboardEntry> history =
        m_database.getHistory(50);


    /*
     * ---------------------------------------------------------
     * Empty database
     * ---------------------------------------------------------
     */
    if (history.empty()) {

        auto *item =
            new QListWidgetItem(
                "No clipboard history yet."
            );


        /*
         * Make the empty-state item non-selectable.
         */
        item->setFlags(
            Qt::NoItemFlags
        );


        /*
         * Add it to the list.
         */
        addItem(
            item
        );

        return;
    }


    /*
     * ---------------------------------------------------------
     * Add database entries
     * ---------------------------------------------------------
     */
    for (const ClipboardEntry &entry : history) {

        /*
         * Create the short preview shown normally.
         */
        const QString preview =
            createPreview(
                entry.content
            );


        /*
         * Create the visible list item.
         */
        auto *item =
            new QListWidgetItem(
                preview
            );


        /*
         * Normal clipboard entries are compact.
         */
        item->setSizeHint(
            QSize(
                -1,
                40
            )
        );


        /*
         * Store the database ID.
         *
         * UserRole will later allow us to implement:
         *
         *     Delete
         *     Pin
         *     Update
         */
        item->setData(
            Qt::UserRole,
            entry.id
        );


        /*
         * Store the complete original clipboard content.
         *
         * The visible text is only a preview.
         *
         * Later the complete content will be used by the
         * paste operation.
         */
        item->setData(
            Qt::UserRole + 1,
            entry.content
        );


        /*
         * Add the item to the list.
         */
        addItem(
            item
        );
    }


    /*
     * Select the newest clipboard entry.
     */
    setCurrentRow(
        0
    );


    /*
     * Re-apply the search query after refreshing.
     *
     * This is important when a new clipboard item is added
     * while the user is currently searching.
     */
    if (!m_currentSearch.isEmpty()) {

        filter(
            m_currentSearch
        );
    }
}


/**
 * @brief Filters clipboard history.
 */
void ClipboardHistoryView::filter(
    const QString &searchText
)
{
    /*
     * Save the current search query.
     */
    m_currentSearch =
        searchText.trimmed();


    /*
     * ---------------------------------------------------------
     * Empty search
     * ---------------------------------------------------------
     *
     * Show every real clipboard item again.
     */
    if (m_currentSearch.isEmpty()) {

        for (
            int index = 0;
            index < count();
            ++index
        ) {

            QListWidgetItem *item =
                this->item(index);


            /*
             * Empty-state item has no UserRole data.
             */
            if (
                !item->data(
                    Qt::UserRole
                ).isValid()
            ) {

                item->setHidden(
                    false
                );

                continue;
            }


            /*
             * Show the clipboard item.
             */
            item->setHidden(
                false
            );


            /*
             * Restore compact preview.
             */
            restoreItem(
                item
            );
        }


        /*
         * Select the first visible item.
         */
        for (
            int index = 0;
            index < count();
            ++index
        ) {

            QListWidgetItem *item =
                this->item(index);


            if (!item->isHidden()) {

                setCurrentItem(
                    item
                );

                break;
            }
        }

        return;
    }


    /*
     * ---------------------------------------------------------
     * Search existing history
     * ---------------------------------------------------------
     */
    for (
        int index = 0;
        index < count();
        ++index
    ) {

        QListWidgetItem *item =
            this->item(index);


        /*
         * Empty-state item should never be shown while
         * searching.
         */
        if (
            !item->data(
                Qt::UserRole
            ).isValid()
        ) {

            item->setHidden(
                true
            );

            continue;
        }


        /*
         * Retrieve the complete clipboard content.
         */
        const QString fullText =
            item->data(
                Qt::UserRole + 1
            ).toString();


        /*
         * Case-insensitive search.
         *
         * Example:
         *
         *     Docker
         *
         * matches:
         *
         *     docker ps
         */
        const bool matches =
            fullText.contains(
                m_currentSearch,
                Qt::CaseInsensitive
            );


        /*
         * Hide entries that don't match.
         */
        item->setHidden(
            !matches
        );


        /*
         * Matching entries return to their compact form.
         */
        if (matches) {

            restoreItem(
                item
            );
        }
    }


    /*
     * Select the first matching result.
     */
    for (
        int index = 0;
        index < count();
        ++index
    ) {

        QListWidgetItem *item =
            this->item(index);


        if (!item->isHidden()) {

            setCurrentItem(
                item
            );

            return;
        }
    }


    /*
     * No results.
     *
     * We intentionally do not create a fake list item here.
     * A proper "No results" UI can be added later.
     */
}


/**
 * @brief Restores an item to compact form.
 */
void ClipboardHistoryView::restoreItem(
    QListWidgetItem *item
)
{
    if (item == nullptr) {
        return;
    }


    /*
     * Get the complete clipboard content.
     */
    const QString fullText =
        item->data(
            Qt::UserRole + 1
        ).toString();


    /*
     * Restore the short preview.
     */
    item->setText(
        createPreview(
            fullText
        )
    );


    /*
     * Restore compact height.
     */
    item->setSizeHint(
        QSize(
            -1,
            40
        )
    );
}


/**
 * @brief Expands the clicked clipboard item.
 */
void ClipboardHistoryView::expandItem(
    QListWidgetItem *item
)
{
    if (item == nullptr) {
        return;
    }


    /*
     * ---------------------------------------------------------
     * Collapse all other items
     * ---------------------------------------------------------
     */
    for (
        int index = 0;
        index < count();
        ++index
    ) {

        QListWidgetItem *currentItem =
            this->item(index);


        if (currentItem == item) {
            continue;
        }


        /*
         * Ignore hidden search results.
         */
        if (currentItem->isHidden()) {
            continue;
        }


        /*
         * Ignore the empty-state item.
         */
        if (
            !currentItem->data(
                Qt::UserRole
            ).isValid()
        ) {

            continue;
        }


        restoreItem(
            currentItem
        );
    }


    /*
     * ---------------------------------------------------------
     * Expand selected item
     * ---------------------------------------------------------
     */
    const QString fullText =
        item->data(
            Qt::UserRole + 1
        ).toString();


    /*
     * Display the complete clipboard content.
     *
     * Word wrapping prevents horizontal scrolling.
     */
    item->setText(
        fullText
    );


    /*
     * Give the expanded item additional space.
     *
     * We'll make this dynamic later.
     */
    item->setSizeHint(
        QSize(
            -1,
            90
        )
    );
}


/**
 * @brief Returns the complete clipboard content of the
 * currently selected history item.
 */
QString ClipboardHistoryView::selectedContent() const
{
    /*
     * Get the currently selected item.
     */
    const QListWidgetItem *item =
        currentItem();


    /*
     * No item is selected.
     */
    if (item == nullptr) {
        return QString();
    }


    /*
     * Retrieve the original clipboard content.
     *
     * UserRole + 1 contains the complete text.
     */
    return item->data(
        Qt::UserRole + 1
    ).toString();
}

/**
 * @brief Selects the next visible clipboard item.
 *
 * Hidden items are skipped. This is important when the user
 * has an active search query.
 */
void ClipboardHistoryView::selectNext()
{
    const int currentRowIndex =
    this->currentRow();


    /*
     * Start searching immediately after the current item.
     */
    for (
        int row = currentRowIndex + 1;
        row < count();
        ++row
    ) {

        QListWidgetItem *item =
            this->item(row);


        /*
         * Skip hidden search results.
         */
        if (item->isHidden()) {
            continue;
        }


        /*
         * Skip the "No clipboard history yet" item.
         */
        if (
            !item->data(
                Qt::UserRole
            ).isValid()
        ) {
            continue;
        }


        /*
         * Select the next valid item.
         */
        setCurrentRow(
            row
        );

        return;
    }
}

/**
 * @brief Selects the previous visible clipboard item.
 *
 * Hidden items are skipped so keyboard navigation works
 * correctly while searching.
 */
void ClipboardHistoryView::selectPrevious()
{
    const int currentRowIndex =
        this->currentRow();

    /*
     * Start searching immediately before the current item.
     */
    for (
        int row = currentRowIndex - 1;
        row >= 0;
        --row
    ) {
        QListWidgetItem *item =
            this->item(row);

        /*
         * Skip hidden items.
         */
        if (item->isHidden()) {
            continue;
        }

        /*
         * Skip the empty-state item.
         */
        if (
            !item->data(
                Qt::UserRole
            ).isValid()
        ) {
            continue;
        }

        /*
         * Select the previous valid item.
         */
        setCurrentRow(row);

        return;
    }
}