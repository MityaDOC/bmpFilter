#include <QCoreApplication>
#include <QDebug>
#include "mainFunc.h"
#include "windows.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SetConsoleOutputCP(1251); // Русская локализация
    SetConsoleCP(1251);

    mainFunc();

    return a.exec();
}
