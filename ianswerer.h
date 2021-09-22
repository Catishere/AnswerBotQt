#ifndef IANSWERER_H
#define IANSWERER_H

#include <QObject>

class IAnswerer : public QObject
{
    Q_OBJECT
public slots:
    virtual void networkAnswer(QByteArray answer) = 0;
};

#endif // IANSWERER_H
