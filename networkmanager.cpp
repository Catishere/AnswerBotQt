#include "networkmanager.h"

NetworkManager::NetworkManager(IAnswerer* answerer)
{
    this->setParent(answerer);
    manager = new QNetworkAccessManager();
    connect(manager, &QNetworkAccessManager::finished,
            this, &NetworkManager::finished);

    connect(this, &NetworkManager::parsed,
            answerer, &IAnswerer::networkAnswer);
}

NetworkManager::~NetworkManager()
{
    delete manager;
}

int NetworkManager::fromGoogle(QByteArray question)
{
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(question)));
    connect(reply, &QNetworkReply::errorOccurred,
            this, &NetworkManager::error);
    connect(reply, &QNetworkReply::sslErrors,
            this, &NetworkManager::sslErrors);
    return 0;
}

void NetworkManager::error(QNetworkReply::NetworkError err)
{
    qDebug() << err;
}

void NetworkManager::sslErrors(const QList<QSslError> &errors)
{
    qDebug() << errors;
}

void NetworkManager::finished(QNetworkReply *reply)
{
    qDebug() << "finished";
    emit parsed(reply->readAll().mid(50, 20));
}
