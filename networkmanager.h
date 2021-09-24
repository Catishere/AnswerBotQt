#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QtConcurrent>
#include <QNetworkReply>
#include <QJsonDocument>

#define TRANSLATE_URL "https://translate.google.bg/translate_a/single?"     \
            "client=gtx&sl=bg&tl=en&hl=en&dt=qca&dt=t&q="

#define USER_AGENT "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/" \
            "537.36 (KHTML, like Gecko) Chrome/93.0.4577.82 Safari/537.36"

class NetworkManager : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager *searchManager;
    QNetworkAccessManager *transManager;
    QByteArray find(QByteArray &ba, const char * start,
                    int offset, const char * end, bool isClean = true);
public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();
    void fromGoogle(QByteArray question);
    void fromGoogleTranslated(QByteArray question);
public slots:
    void finishedGoogle(QNetworkReply *reply);
    void finishedTranslate(QNetworkReply *reply);
signals:
    void parsed(QByteArray answer);
};

#endif // NETWORKMANAGER_H
