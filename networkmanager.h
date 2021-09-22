#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "ianswerer.h"

class NetworkManager : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager *manager;
public:
    explicit NetworkManager(IAnswerer * answerer);
    ~NetworkManager();
    void fromGoogle(QByteArray question);
public slots:
    void finished(QNetworkReply *reply);
signals:
    void parsed(QByteArray answer);
};

#endif // NETWORKMANAGER_H
