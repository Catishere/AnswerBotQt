#include "bot.h"

Bot::Bot(QObject *parent) : QObject(parent)
{
    Reader *reader = new Reader(this);
    connect(reader, &Reader::resultReady, this, &Bot::handleResults);
    connect(reader, &Reader::finished, reader, &QObject::deleteLater);
    reader->start();
}

void Bot::handleResults(const QString &result)
{

}
