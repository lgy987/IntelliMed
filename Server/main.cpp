#include <QCoreApplication>
#include "server.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool initDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("users.db"); // file will be created in app directory

    if (!db.open()) {
        qCritical() << "Failed to open database!";
        return false;
    }

    QSqlQuery query;
    // Create users table if it doesn't exist
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "username TEXT UNIQUE,"
                    "password TEXT,"
                    "email TEXT UNIQUE,"
                    "token TEXT)")) {
        qCritical() << "Failed to create users table:" << query.lastError();
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    initDatabase();
    Server server;

    return a.exec();
}
