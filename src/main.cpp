#include <QApplication>
#include <QClipboard>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QClipboard *clipboard = QApplication::clipboard();

    QObject::connect(
        clipboard,
        &QClipboard::dataChanged,
        []()
        {
            const QString text =
                QApplication::clipboard()->text();

            qDebug() << "Clipboard changed:";
            qDebug() << text;
        }
    );

    qDebug() << "Clipo started.";

    return app.exec();
}