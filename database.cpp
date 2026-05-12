#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Database::Database() {}

Database::~Database()
{
    closeDatabase();
}

bool Database::saveUser(int age, double weight, double height) {
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users (age, weight, height) VALUES (:age, :weight, :height)");
    query.bindValue(":age", age);
    query.bindValue(":weight", weight);
    query.bindValue(":height", height);

    if (!query.exec()) {
        qDebug() << "Ошибка сохранения пользователя:" << query.lastError().text();
        return false;
    }
    qDebug() << "Пользователь сохранён в базу.";
    return true;
}
bool Database::openDatabase(const QString &path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open()) {
        qDebug() << "Ошибка открытия базы данных:" << m_db.lastError().text();
        return false;
    }
    qDebug() << "База данных открыта:" << path;
    return true;
}

void Database::createTables()
{
    QSqlQuery query(m_db);

    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "age INTEGER, "
               "weight REAL, "
               "height REAL"
               ")");

    // таблица продуктов
    query.exec("CREATE TABLE IF NOT EXISTS products ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, "
               "price REAL, "
               "calories REAL, "
               "protein REAL, "
               "fat REAL, "
               "carbs REAL"
               ")");

    // таблица бюджета
    query.exec("CREATE TABLE IF NOT EXISTS budget ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "goal_name TEXT, "
               "goal_amount REAL, "
               "saved REAL DEFAULT 0"
               ")");

    qDebug() << "Таблицы созданы.";
}

void Database::closeDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "База данных закрыта.";
    }
}

void Database::seedProducts()
{
    QSqlQuery query(m_db);
    // очистка старых данных
    query.exec("DELETE FROM products");

    // стартовый набор продуктов
    query.exec("INSERT INTO products (name, price, calories, protein, fat, carbs) VALUES ('Гречка', 25.0, 340.0, 12.0, 3.0, 68.0)");
    query.exec("INSERT INTO products (name, price, calories, protein, fat, carbs) VALUES ('Куриное филе', 45.0, 110.0, 23.0, 2.0, 0.0)");
    query.exec("INSERT INTO products (name, price, calories, protein, fat, carbs) VALUES ('Молоко', 20.0, 60.0, 3.0, 3.5, 4.8)");
    query.exec("INSERT INTO products (name, price, calories, protein, fat, carbs) VALUES ('Яйца', 10.0, 155.0, 13.0, 11.0, 1.0)");
    query.exec("INSERT INTO products (name, price, calories, protein, fat, carbs) VALUES ('Рис', 15.0, 330.0, 7.0, 1.0, 74.0)");
    query.exec("INSERT INTO products (name, price, calories, protein, fat, carbs) VALUES ('Макароны', 12.0, 350.0, 12.0, 1.0, 72.0)");
    query.exec("INSERT INTO products (name, price, calories, protein, fat, carbs) VALUES ('Хлеб', 8.0, 265.0, 8.0, 3.0, 49.0)");
    query.exec("INSERT INTO products (name, price, calories, protein, fat, carbs) VALUES ('Творог', 35.0, 110.0, 18.0, 2.0, 3.0)");
    query.exec("INSERT INTO products (name, price, calories, protein, fat, carbs) VALUES ('Бананы', 60.0, 95.0, 1.5, 0.5, 21.0)");

    qDebug() << "Продукты добавлены в базу.";
}

QVector<QVariantMap> Database::getProducts()
{
    QVector<QVariantMap> products;
    QSqlQuery query(m_db);
    query.exec("SELECT name, price, calories, protein, fat, carbs FROM products");
    while (query.next()) {
        QVariantMap product;
        product["name"] = query.value(0).toString();
        product["price"] = query.value(1).toDouble();
        product["calories"] = query.value(2).toDouble();
        product["protein"] = query.value(3).toDouble();
        product["fat"] = query.value(4).toDouble();
        product["carbs"] = query.value(5).toDouble();
        products.append(product);
    }
    return products;
}

QVariantMap Database::getLastUser()
{
    QVariantMap user;
    QSqlQuery query(m_db);
    query.exec("SELECT age, weight, height FROM users ORDER BY id DESC LIMIT 1");
    if (query.next()) {
        user["age"] = query.value(0).toInt();
        user["weight"] = query.value(1).toDouble();
        user["height"] = query.value(2).toDouble();
    }
    return user;
}