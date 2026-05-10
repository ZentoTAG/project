#include "mainwindow.h"
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
    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
