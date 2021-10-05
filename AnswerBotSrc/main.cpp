#include <QCoreApplication>
#include "reader.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Reader reader(&a);

    return a.exec();
}
