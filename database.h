#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>

class Database {
public:
    Database();
    ~Database();

    bool openDatabase(const QString &path);
    void createTables();
    void closeDatabase();
    bool saveUser(int age, double weight, double height);
    void seedProducts(); // заполнение таблицы продуктов начальными данными

    QVector<QVariantMap> getProducts(); // возщвратвсех продуктов из бд
    QVariantMap getLastUser();

private:
    QSqlDatabase m_db;
};

#endif // DATABASE_H