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
    auto r = searchManager->get(req);
    connect(r, &QNetworkReply::errorOccurred,
            this, &NetworkManager::queryError);
}

void NetworkManager::fromGoogleTranslated(QByteArray question)
{
    translateOnly = false;
    QNetworkRequest req(QUrl(TRANSLATE_URL + question));
    req.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    req.setRawHeader("Cookie", "CONSENT=YES+cb.20210919-19-p0.en+FX+947");
    transManager->get(req);
}

void NetworkManager::translate(const QByteArray &q)
{
    translateOnly = true;
    QByteArray url = constructTranslationUrl(q);
    QNetworkRequest req(QUrl(url.data()));
    req.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    req.setRawHeader("Cookie", "CONSENT=YES+cb.20210919-19-p0.en+FX+947");
    transManager->get(req);
}

void NetworkManager::finishedTranslate(QNetworkReply *reply)
{
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    QString s = json.array().at(0).toArray().at(0).toArray().at(0).toString();
    qDebug() << "> Translate: " << s;
    if (translateOnly)
        emit parsed(s.toUtf8());
    else
        fromGoogle(s.toUtf8().toPercentEncoding());
}

void NetworkManager::queryError(QNetworkReply::NetworkError error)
{
    qDebug() << "ERROR: Network request error '" << error << "' occured";
}

void NetworkManager::finishedGoogle(QNetworkReply *reply)
{
    QByteArray ba = reply->readAll();
    QFile file("text.html");
    file.open(QIODevice::WriteOnly);
    file.write(ba);

    QList<QByteArrayView> selectors = {
        "<div class=\"Z0LcW XcVN5d",
        "<div class=\"zCubwf",
        "<div jsname=\"T3JDGc\" class=\"FozYP",
        "<div class=\"uoFCfc",
        "<div class=\"ayqGOc",
        "<div class=\"dDoNo",
        "<pre class=\"tw-data-text tw-text-large XcVN5d tw-ta\" data-placeholder=\"Translation\" id=\"tw-target-text", // translate
        "<div class=\"lr-fy-ev lr-fy-ov lr-fy-eq-elem", // math
        "<div class=\"gsrt vk_bk FzvWSb", // time
        "<span class=\"wob_t TVtOme", //weather
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

QByteArray NetworkManager::constructTranslationUrl(const QByteArray &text)
{
    QString sl = text.left(2);
    QString tl = text.mid(2, 2);
    QString q = text.right(text.size() - 5);
    return QString().asprintf("https://translate.googleapis.com/translate_a/single?"
                              "client=gtx&ie=UTF-8&oe=UTF-8&sl=%s&tl=%s&dt=qca&dt=t&q=%s",
                              sl.toUtf8().data(),
                              tl.toUtf8().data(),
                              q.toUtf8().data()
                              ).toUtf8();
}
