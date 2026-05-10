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

    // Таблица продуктов
    query.exec("CREATE TABLE IF NOT EXISTS products ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, "
               "price REAL, "
               "calories REAL, "
               "protein REAL, "
               "fat REAL, "
               "carbs REAL"
               ")");

    // Таблица бюджета
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