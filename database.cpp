#include "database.h"
#include "magnitparser.h"
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

    // таблица бюджета
    query.exec("CREATE TABLE IF NOT EXISTS budget ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "goal_name TEXT, "
               "goal_amount REAL, "
               "saved REAL DEFAULT 0"
               ")");

    query.exec("CREATE TABLE IF NOT EXISTS products ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, "
               "category TEXT, "
               "price REAL, "
               "calories REAL, "
               "protein REAL, "
               "fat REAL, "
               "carbs REAL, "
               "sugar REAL DEFAULT 0, "
               "salt REAL DEFAULT 0, "
               "tags TEXT"
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
    query.exec("DELETE FROM products");

    query.exec("INSERT INTO products "
               "(name, category, price, calories, protein, fat, carbs, sugar, salt, tags) VALUES "

               "('Куриное филе', 'protein', 45, 110, 23, 2, 0, 0, 0.1, 'gluten_free,lactose_free'),"
               "('Яйца', 'protein', 10, 155, 13, 11, 1, 0, 0.2, 'gluten_free,lactose_free'),"
               "('Творог', 'dairy', 35, 110, 18, 5, 3, 3, 0.1, 'gluten_free'),"
               "('Тунец', 'protein', 80, 130, 28, 1, 0, 0, 0.3, 'gluten_free,lactose_free'),"

               "('Рис', 'carbs', 15, 330, 7, 1, 74, 0, 0.01, 'gluten_free,vegan'),"
               "('Гречка', 'carbs', 25, 340, 12, 3, 68, 0, 0.01, 'gluten_free,vegan'),"
               "('Овсянка', 'carbs', 20, 370, 13, 7, 62, 1, 0.01, 'vegan'),"
               "('Картофель', 'carbs', 12, 77, 2, 0.4, 17, 1, 0.01, 'gluten_free,vegan'),"

               "('Яблоки', 'fruits', 40, 52, 0.3, 0.2, 14, 10, 0.01, 'gluten_free,vegan'),"
               "('Бананы', 'fruits', 60, 95, 1.5, 0.5, 21, 17, 0.01, 'gluten_free,vegan'),"

               "('Брокколи', 'vegetables', 50, 34, 3, 0.4, 7, 1.5, 0.02, 'gluten_free,vegan'),"
               "('Морковь', 'vegetables', 20, 41, 1, 0.2, 10, 5, 0.05, 'gluten_free,vegan'),"

               "('Молоко', 'dairy', 20, 60, 3, 3.5, 4.8, 5, 0.1, 'gluten_free'),"
               "('Сыр', 'dairy', 50, 360, 25, 28, 1, 0.5, 1.2, 'gluten_free'),"

               "('Орехи', 'fats', 90, 600, 20, 50, 15, 4, 0.01, 'gluten_free,vegan'),"
               "('Оливковое масло', 'fats', 70, 900, 0, 100, 0, 0, 0, 'gluten_free,vegan,lactose_free')"
               );
    qDebug() << "Продукты с тегами добавлены.";
}

QVector<QVariantMap> Database::getProducts()
{
    QVector<QVariantMap> products;
    QSqlQuery query(m_db);
query.exec("SELECT name, category, price, calories, protein, fat, carbs, sugar, salt, tags FROM products");
    while (query.next()) {
        QVariantMap product;
        product["name"]     = query.value(0).toString();
        product["category"] = query.value(1).toString();
        product["price"]    = query.value(2).toDouble();
        product["calories"] = query.value(3).toDouble();
        product["protein"]  = query.value(4).toDouble();
        product["fat"]      = query.value(5).toDouble();
        product["carbs"]    = query.value(6).toDouble();
        product["sugar"]    = query.value(7).toDouble();
        product["salt"]     = query.value(8).toDouble();
        product["tags"]     = query.value(9).toString();
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

bool Database::insertProducts(const QVector<QVariantMap> &products)
{
    QSqlQuery query(m_db);

    for (const auto &p : products)
    {
        query.prepare(
            "INSERT INTO products "
            "(name, category, price, calories, protein, fat, carbs, sugar, salt, tags) "
            "VALUES "
            "(:name, :category, :price, :calories, :protein, :fat, :carbs, :sugar, :salt, :tags)"
            );

        query.bindValue(":name", p["name"]);
        query.bindValue(":category", p["category"]);
        query.bindValue(":price", p["price"]);
        query.bindValue(":calories", p["calories"]);
        query.bindValue(":protein", p["protein"]);
        query.bindValue(":fat", p["fat"]);
        query.bindValue(":carbs", p["carbs"]);
        query.bindValue(":sugar", p["sugar"]);
        query.bindValue(":salt", p["salt"]);
        query.bindValue(":tags", p["tags"]);

        if (!query.exec())
        {
            qDebug() << "Ошибка вставки:"
                     << query.lastError().text();
        }
    }

    return true;
}