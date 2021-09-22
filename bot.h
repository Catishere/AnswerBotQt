#ifndef BOT_H
#define BOT_H

#include "reader.h"

#include <QObject>

class Bot : public QObject
{
    Q_OBJECT

    Reader *reader;
public:
    explicit Bot(QObject *parent = nullptr);

public slots:
    void handleResults(const QString &result);

signals:

};

#endif // BOT_H
