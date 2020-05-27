#include "mqttsimulator.h"

MqttSimulator::MqttSimulator(QTextBrowser **log, QObject *parent)
    :
    QObject(parent), log(log)
{
    client.setKeepAlive(60);
    client.setPort(8883);

    connect(&client, &QMqttClient::connected, this, &MqttSimulator::on_connect);
}

MqttSimulator::~MqttSimulator()
{
    disconnect(&client, &QMqttClient::connected, this, &MqttSimulator::on_connect);
}

void MqttSimulator::on_connect()
{
    (*log)->append("Connected!\n");
    (*log)->append("Status: " + QString::number(client.state()) + "\n");

    QJsonDocument payload = QJsonDocument::fromJson(data.toUtf8());
    QMqttTopicName topic = QMqttTopicName("/devices/ambulance0/events");

    (*log)->append("Sending data: " + payload.toJson() + "...");

    client.publish(topic, payload.toJson(), 1);
    (*log)->append("sent!\n");

    client.disconnectFromHost();
    (*log)->append("Disconnected...\n");
}

void MqttSimulator::publish_data(QByteArray jwt, QString data, QString root_ca)
{
    sslConf.setCaCertificates(QSslCertificate::fromPath(root_ca));

    this->data = data;

    client.setPassword(jwt);
    client.connectToHostEncrypted(sslConf);
}

QByteArray MqttSimulator::createJWT(QString interpreter, QStringList arguments)
{
    script.start(interpreter, arguments);
    script.waitForFinished(-1);

    return script.readAllStandardOutput();
}

void MqttSimulator::setClient(QString id)
{
    client.setClientId(id);
}

void MqttSimulator::setHost(QString name)
{
    client.setHostname(name);
}
