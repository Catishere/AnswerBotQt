#include "networkmanager.h"

NetworkManager::NetworkManager(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager();
    connect(manager, &QNetworkAccessManager::finished,
            this, &NetworkManager::finished);
}

NetworkManager::~NetworkManager()
{
    delete manager;
}

void NetworkManager::fromGoogle(QByteArray question)
{
    QNetworkRequest req(QUrl("https://www.google.com/search?q=" + question));
    //QNetworkRequest translate(QUrl(TRANSLATE_URL));
    req.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    req.setRawHeader("Cookie", "CONSENT=YES+cb.20210919-19-p0.en+FX+947");
    manager->get(req);
}

void NetworkManager::finished(QNetworkReply *reply)
{
    QByteArray ba = reply->readAll();
    QFile file("text.html");
    file.open(QIODevice::WriteOnly);
    file.write(ba);
    file.close();
    QByteArray res = find(ba, "Z0LcW XcVN5d", 14, "<");
    if (res.isEmpty())
        res = find(ba, "<div class=\"zCubwf\"", 0, "", false);
    emit parsed(res);
}

QByteArray NetworkManager::find(QByteArray &ba, const char *start,
                                int offset, const char *end, bool isClean)
{
    qsizetype start_index = ba.indexOf(start) + offset;
    qsizetype end_index;

    if (!isClean) {
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
    } else {
        end_index = ba.indexOf(end, start_index);
    }

    if (start_index == offset - 1 || end_index < 0)
        return QByteArray();

    QByteArray res = ba.mid(start_index, end_index - start_index);
    if (isClean)
        return res;
    else
        return QString(res).remove(QRegularExpression("<.+?>")).toUtf8();
}
