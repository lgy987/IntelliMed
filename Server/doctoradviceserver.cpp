#include "doctoradviceserver.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

DoctorAdviceServer::DoctorAdviceServer(QObject *parent)
    : QTcpServer(parent) {}

bool DoctorAdviceServer::start() {
    if (!db_.open()) return false;
    db_.ensureSchema();
    db_.seed();
    return true;
}

// This function allows the main server to forward a JSON request
QJsonObject DoctorAdviceServer::handleRequest(const QJsonObject &request) {
    QJsonObject response;

    // reuse your existing handler logic by mocking a "fake socket"
    handleJson(nullptr, request);  // handleJson will produce output only via sendJson
    // Since sendJson expects QTcpSocket*, we can also create a helper that returns QJsonObject
    // Better: implement a new handleJsonInternal function:

    QString type = request.value("type").toString();

    if (type == "login") {
        const QString id = request.value("id").toString();
        QString name;
        const QString role = db_.queryUserRole(id, &name);
        bool ok = !role.isEmpty();

        response.insert("type", "login_result");
        response.insert("ok", ok);
        response.insert("id", id);
        response.insert("name", name);
        response.insert("role", role);
    } else if (type == "create_order") {
        const QString patientId = request.value("patient_id").toString();
        const QString doctorId  = request.value("doctor_id").toString();
        const QString dept      = request.value("dept").toString();
        const QString content   = request.value("content").toString();
        QString err;
        bool ok = db_.createOrder(patientId, doctorId, dept, content, &err);

        response.insert("type", "create_order_result");
        response.insert("ok", ok);
        response.insert("error", ok ? "" : err);
    } else if (type == "list_orders") {
        const QString patientId = request.value("patient_id").toString();
        const QString keyword   = request.value("keyword").toString();
        const QString dept      = request.value("dept").toString();
        const QString dateFrom  = request.value("date_from").toString();
        const QString dateTo    = request.value("date_to").toString();
        int page = request.value("page").toInt();
        int pageSize = request.value("page_size").toInt();
        if (page < 1) page = 1;
        if (pageSize < 1) pageSize = 10;

        int total = db_.countOrders(patientId, keyword, dept, dateFrom, dateTo);
        const QString arrJson = db_.listOrdersAsJson(patientId, keyword, dept, dateFrom, dateTo, page, pageSize);
        QJsonDocument arrDoc = QJsonDocument::fromJson(arrJson.toUtf8());

        response.insert("type", "orders");
        response.insert("patient_id", patientId);
        response.insert("total", total);
        response.insert("page", page);
        response.insert("page_size", pageSize);
        response.insert("items", arrDoc.array());
    } else if (type == "update_order") {
        int id = request.value("id").toInt();
        const QString dept = request.value("dept").toString();
        const QString content = request.value("content").toString();
        QString err;
        bool ok = db_.updateOrder(id, dept, content, &err);

        response.insert("type", "update_order_result");
        response.insert("ok", ok);
        response.insert("id", id);
        response.insert("error", ok ? "" : err);
    } else if (type == "delete_order") {
        int id = request.value("id").toInt();
        QString err;
        bool ok = db_.deleteOrder(id, &err);

        response.insert("type", "delete_order_result");
        response.insert("ok", ok);
        response.insert("id", id);
        response.insert("error", ok ? "" : err);
    } else {
        response.insert("type", "error");
        response.insert("message", "unknown_type");
    }

    return response;
}

void DoctorAdviceServer::handleJson(QTcpSocket *s, const QJsonObject &obj) {
    const QString type = obj.value("type").toString();

    if (type == "login") {
        const QString id = obj.value("id").toString();
        QString name;
        const QString role = db_.queryUserRole(id, &name);
        bool ok = !role.isEmpty();

        QJsonObject out;
        out.insert("type", "login_result");
        out.insert("ok", ok);
        out.insert("id", id);
        out.insert("name", name);
        out.insert("role", role);
        sendJson(s, out);
        return;
    }

    if (type == "create_order") {
        const QString patientId = obj.value("patient_id").toString();
        const QString doctorId  = obj.value("doctor_id").toString();
        const QString dept      = obj.value("dept").toString();
        const QString content   = obj.value("content").toString();
        QString err;
        bool ok = db_.createOrder(patientId, doctorId, dept, content, &err);

        QJsonObject out;
        out.insert("type", "create_order_result");
        out.insert("ok", ok);
        out.insert("error", ok ? "" : err);
        sendJson(s, out);
        return;
    }

    if (type == "list_orders") {
        const QString patientId = obj.value("patient_id").toString();
        const QString keyword   = obj.value("keyword").toString();
        const QString dept      = obj.value("dept").toString();
        const QString dateFrom  = obj.value("date_from").toString();
        const QString dateTo    = obj.value("date_to").toString();
        int page = obj.value("page").toInt();
        int pageSize = obj.value("page_size").toInt();
        if (page < 1) page = 1;
        if (pageSize < 1) pageSize = 10;

        int total = db_.countOrders(patientId, keyword, dept, dateFrom, dateTo);
        const QString arrJson = db_.listOrdersAsJson(patientId, keyword, dept, dateFrom, dateTo, page, pageSize);
        QJsonDocument arrDoc = QJsonDocument::fromJson(arrJson.toUtf8());

        QJsonObject out;
        out.insert("type", "orders");
        out.insert("patient_id", patientId);
        out.insert("total", total);
        out.insert("page", page);
        out.insert("page_size", pageSize);
        out.insert("items", arrDoc.array());
        sendJson(s, out);
        return;
    }

    if (type == "update_order") {
        int id = obj.value("id").toInt();
        const QString dept = obj.value("dept").toString();
        const QString content = obj.value("content").toString();
        QString err;
        bool ok = db_.updateOrder(id, dept, content, &err);

        QJsonObject out;
        out.insert("type", "update_order_result");
        out.insert("ok", ok);
        out.insert("id", id);
        out.insert("error", ok ? "" : err);
        sendJson(s, out);
        return;
    }

    if (type == "delete_order") {
        int id = obj.value("id").toInt();
        QString err;
        bool ok = db_.deleteOrder(id, &err);

        QJsonObject out;
        out.insert("type", "delete_order_result");
        out.insert("ok", ok);
        out.insert("id", id);
        out.insert("error", ok ? "" : err);
        sendJson(s, out);
        return;
    }

    QJsonObject out;
    out.insert("type", "error");
    out.insert("message", "unknown_type");
    sendJson(s, out);
}

void DoctorAdviceServer::sendJson(QTcpSocket *s, const QJsonObject &obj) {
    QJsonDocument doc(obj);
    QByteArray line = doc.toJson(QJsonDocument::Compact);
    line.append('\n');
    s->write(line);
    s->flush();
}

void DoctorAdviceServer::incomingConnection(qintptr handle) {
    QTcpSocket *s = new QTcpSocket(this);
    if (!s->setSocketDescriptor(handle)) {
        s->deleteLater();
        return;
    }
    Conn *c = new Conn;
    c->sock = s;
    conns_[s] = c;

    connect(s, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(s, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void DoctorAdviceServer::onReadyRead() {
    QTcpSocket *s = qobject_cast<QTcpSocket*>(sender());
    if (!s) return;
    Conn *c = conns_.value(s);
    if (!c) return;

    c->buffer.append(s->readAll());

    while (true) {
        int idx = c->buffer.indexOf('\n');
        if (idx < 0) break;
        QByteArray line = c->buffer.left(idx);
        c->buffer.remove(0, idx + 1);

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
            QJsonObject o;
            o.insert("type", "error");
            o.insert("message", "invalid_json");
            sendJson(s, o);
            continue;
        }
        handleJson(s, doc.object());
    }
}

void DoctorAdviceServer::onDisconnected() {
    QTcpSocket *s = qobject_cast<QTcpSocket*>(sender());
    if (!s) return;
    Conn *c = conns_.take(s);
    if (c) delete c;
    s->deleteLater();
}
