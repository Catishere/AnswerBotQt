#ifndef ANSWERER_H
#define ANSWERER_H

#include <QObject>
#include <QtNetwork>
#include <QByteArray>
#include <QSettings>
#include <QRegularExpression>
#include <QFile>
#include <QMap>

#include "networkmanager.h"
#include "tinyexpr.h"

#define RELOAD 0b01
#define LAUNCH 0b10
#define RELOAD_LAUNCH (RELOAD | LAUNCH)

class Answerer : public QObject
{
    Q_OBJECT
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
    explicit Answerer(QSettings &settings, QObject *parent = nullptr);
    QByteArray answer(QByteArray q);
    int writeConfig(QByteArray cfg);
    int makeWrite(QByteArray& expr);

    void setAlivePrisoners(const QString &newAlivePrisoners);
    void setTotalPrisoners(const QString &newTotalPrisoners);
    void setJbDay(const QString &newJbDay);
    void translate(const QByteArray &q);
public slots:
    void networkAnswer(QByteArray answer);

signals:
    void interruptLoop();
    void executeIngame(int);

};

#endif // ANSWERER_H
