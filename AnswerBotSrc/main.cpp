#include <QCoreApplication>
#include "reader.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Reader reader;

    return a.exec();
}
