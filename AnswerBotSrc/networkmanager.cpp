#include "networkmanager.h"

NetworkManager::NetworkManager(QObject *parent) : QObject(parent)
{
    searchManager = new QNetworkAccessManager();
    transManager = new QNetworkAccessManager();
    connect(searchManager, &QNetworkAccessManager::finished,
            this, &NetworkManager::finishedGoogle);
    connect(transManager, &QNetworkAccessManager::finished,
            this, &NetworkManager::finishedTranslate);
}

NetworkManager::~NetworkManager()
{
    delete searchManager;
    delete transManager;
}

void NetworkManager::fromGoogle(QByteArray question)
{
    QNetworkRequest req(QUrl("https://www.google.com/search?q=" + question));
    req.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    req.setRawHeader("Cookie", "CONSENT=YES+cb.20210919-19-p0.en+FX+947");
    searchManager->get(req);
}

void NetworkManager::fromGoogleTranslated(QByteArray question)
{
    QNetworkRequest req(QUrl(TRANSLATE_URL + question));
    req.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    req.setRawHeader("Cookie", "CONSENT=YES+cb.20210919-19-p0.en+FX+947");
    transManager->get(req);
}

void NetworkManager::finishedTranslate(QNetworkReply *reply)
{
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    QString s = json.array().at(0).toArray().at(0).toArray().at(0).toString();
    qDebug() << "> Translate: " << s;
    fromGoogle(s.toUtf8().toPercentEncoding());
}

void NetworkManager::finishedGoogle(QNetworkReply *reply)
{
    QByteArray ba = reply->readAll();

    QList<QByteArrayView> selectors = {
        "<div class=\"Z0LcW XcVN5d",
        "<div class=\"zCubwf",
        "<div jsname=\"T3JDGc\" class=\"FozYP",
        "<div class=\"uoFCfc",
        "<div class=\"ayqGOc",
        "<div class=\"dDoNo"
    };

    for (auto &selector : selectors) {
        QByteArray res = find(ba, selector);
        if (!res.isEmpty() && res.size() < 100) {
            emit parsed(res);
            return;
        }
    }
    emit parsed("Google miss");
}

QByteArray NetworkManager::find(QByteArray &ba, const QByteArrayView &selector)
{
    qsizetype start_index = ba.indexOf(selector);
    qsizetype end_index;

    qsizetype temp_index = start_index;
    int tag_count = 0;
    while (true) {
        qsizetype tag_index = ba.indexOf('<', temp_index);
        if (tag_index >= 0) {
            if (ba.at(tag_index + 1) == '/')
                tag_count--;
            else
                tag_count++;
            if (tag_count == 0) {
                end_index = tag_index;
                break;
            }
            temp_index = tag_index + 1;
        } else {
            end_index = -1;
            break;
        }
    }

    if (start_index < 0 || end_index < 0)
        return QByteArray();

    QByteArray res = ba.mid(start_index, end_index - start_index);
    return QString(res).remove(QRegularExpression("<.+?>")).toUtf8();
}
