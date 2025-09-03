#pragma once
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

class NetClient : public QObject {
    Q_OBJECT
public:
    explicit NetClient(QObject* parent = nullptr);
    static NetClient& instance();

    void insertPatient(const QJsonObject& p);
    void getAll();
    void getByName(const QString& name);
    void getByPhone(const QString& phone);

public slots:
    void onReadyRead(const QJsonObject &content);

signals:
    void patientInserted(bool ok);
    void allPatientsReceived(const QJsonArray &data);
    void patientByNameReceived(const QJsonObject &data);
    void patientByPhoneReceived(const QJsonObject &data);

private:
    void send(const QJsonObject& obj);
};
