#ifndef ANSWERER_H
#define ANSWERER_H

#include <QtNetwork>
#include <QByteArray>
#include <QSettings>
#include <QRegularExpression>
#include <QFile>
#include <QMap>

#include "networkmanager.h"
#include "tinyexpr.h"
#include "ianswerer.h"

class Answerer : IAnswerer
{
    QMap<QByteArray, QByteArray> countries;
    QMap<QByteArray, QByteArray> capitals;
    QSettings & settings;
    NetworkManager netManager;
    QString alivePrisoners;
    QString totalPrisoners;
    QString jbDay;
    QByteArray answerCapitals(QByteArray &question) const;
    QByteArray makeConfig(QByteArray &answer) const;
public:
    Answerer(QSettings &settings);
    QByteArray answer(QByteArray q);
    int writeConfig(QByteArray cfg);
    int makeWrite(QByteArray& expr);

    void setAlivePrisoners(const QString &newAlivePrisoners);
    void setTotalPrisoners(const QString &newTotalPrisoners);
    void setJbDay(const QString &newJbDay);
public slots:
    void networkAnswer(QByteArray answer);
};

#endif // ANSWERER_H
