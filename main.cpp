#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <iterator>
#include <stdio.h>
#include <direct.h>
#include <QDir>


void func1(){

    QString filename = "C:/Progs/bmpFilter/IN"; // ВЫБОР ФАЙЛА
    QDir dir(filename);
    QStringList images = dir.entryList(QStringList() << "*.bmp", QDir::Files);

    qDebug() << images[0] << endl;

    /*
    if (filename.isEmpty())  //создает диалоговое окно выбора одного файла. Этот метод возвращает
                        // значение типа QString, содержащее имя и путь выбранного файла
        {
        return;
        }

    QFile file(filename);
    if(!file.open(QIODevice::ReadWrite)) // rules
    {
        return;
    }

    file.close();
    */
    qDebug() << "ok" << endl;
}



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    func1();

    return a.exec();
}
