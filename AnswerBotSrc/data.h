#ifndef DATA_H
#define DATA_H

#include <QMap>
#include <QByteArray>

class Data
{
public:
    int codesCount;
    QMap<QByteArray, QByteArray> countries;
    QMap<QByteArray, QByteArray> capitals;
    QMap<QByteArray, QByteArray> codes;
    Data();
};

#endif // DATA_H
