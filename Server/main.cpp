#include <QCoreApplication>
#include "server.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "ai.h"

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
                    "token TEXT,"
                    "token_expiry INTEGER)")) {
        qCritical() << "Failed to create users table:" << query.lastError();
        return false;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS doctors ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "username TEXT UNIQUE,"
                    "password TEXT,"
                    "email TEXT UNIQUE,"
                    "token TEXT,"
                    "token_expiry INTEGER)")) {
        qCritical() << "Failed to create doctors table:" << query.lastError();
        return false;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS personal_info ("
                    "id INTEGER PRIMARY KEY,"               // same as users.id
                    "name TEXT,"                    // real name
                    "id_number TEXT,"               // ID number
                    "phone_number TEXT,"                     // optional phone number, nullable
                    "FOREIGN KEY(id) REFERENCES users(id) ON DELETE CASCADE)")) {
        qCritical() << "Failed to create personal_info table:" << query.lastError();
        return false;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS doctor_personal_info ("
                    "id INTEGER PRIMARY KEY,"
                    "name TEXT,"
                    "department TEXT,"
                    "title TEXT,"
                    "description TEXT,"
                    "FOREIGN KEY(id) REFERENCES doctors(id) ON DELETE CASCADE)")) {
        qCritical() << "Failed to create personal_info table:" << query.lastError();
        return false;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS session ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "patientId INTEGER UNIQUE,"
                    "doctorId INTEGER,"
                    "time TEXT,"
                    "FOREIGN KEY(patientId) REFERENCES users(id) ON DELETE CASCADE,"
                    "FOREIGN KEY(doctorId) REFERENCES doctors(id) ON DELETE CASCADE)")) {
        qCritical() << "Failed to create session table:" << query.lastError();
        return false;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS session_old ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "patientId INTEGER,"
                    "doctorId INTEGER,"
                    "time TEXT,"
                    "FOREIGN KEY(patientId) REFERENCES users(id) ON DELETE CASCADE,"
                    "FOREIGN KEY(doctorId) REFERENCES doctors(id) ON DELETE CASCADE)")) {
        qCritical() << "Failed to create session table:" << query.lastError();
        return false;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS messages ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "doctor_id INTEGER NOT NULL,"
                    "patient_id INTEGER NOT NULL,"
                    "sender_type TEXT CHECK(sender_type IN ('patient','doctor')) NOT NULL,"
                    "message TEXT NOT NULL,"
                    "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "FOREIGN KEY(doctor_id) REFERENCES doctors(id),"
                    "FOREIGN KEY(patient_id) REFERENCES users(id))")) {
        qCritical() << "Failed to create messages table:" << query.lastError();
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    initDatabase();
    Server server;

    Ai::callAI("你好");

    return a.exec();
}
