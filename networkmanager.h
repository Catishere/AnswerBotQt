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
    int fromGoogle(QByteArray question);
public slots:
    void error(QNetworkReply::NetworkError);
    void sslErrors(const QList<QSslError> &errors);
    void finished(QNetworkReply *reply);
signals:
    void parsed(QByteArray answer);
};

#endif // NETWORKMANAGER_H
