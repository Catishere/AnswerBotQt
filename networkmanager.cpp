#include "networkmanager.h"

NetworkManager::NetworkManager(IAnswerer* answerer)
{
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

void NetworkManager::fromGoogle(QByteArray question)
{
    manager->get(QNetworkRequest(QUrl(question)));
}

void NetworkManager::finished(QNetworkReply *reply)
{
    emit parsed(reply->readAll().mid(50, 20));
}
