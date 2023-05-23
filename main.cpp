#include <QCoreApplication>
#include <QDebug>
#include "mainFunc.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    mainFunc();

    return a.exec();
}
