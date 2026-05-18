#include "mainwindow.h"
#include "magnitparser.h"
#include "database.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Database db;
    if (!db.openDatabase("products.db")) {
        qDebug() << "Не удалось открыть базу данных!";
        return -1;
    }
    db.createTables();
    // db.seedProducts();
    MagnitParser parser;

    QVector<QVariantMap> parsed =
        parser.loadProducts();
    qDebug() << "PARSED COUNT =" << parsed.size();
    db.insertProducts(parsed);
    qDebug() << "Спарсили:" << parsed.size();
    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
