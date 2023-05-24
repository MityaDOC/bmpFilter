#include "mainFunc.h"
#include <QDebug>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QImageWriter>
#include <QBuffer>
#include <QDateTime>
#include <QElapsedTimer>
#include <QFileSystemWatcher>
#include "ipp.h"


mainFunc::mainFunc()
{
    //------------ preparation -----------------

    QString dirnameIN  = "C:/Progs/bmpFilter/IN";           // !Введите адрес рабочей папки!
    QDir dirIN(dirnameIN);
    if(!dirIN.exists())
    {
        qCritical() << "Отсутсвует входная папка!";
        return;
    }
    QString dirnameOUT = "C:/Progs/bmpFilter/OUT";          // Введите адрес папки для обработанных файлов
    QDir dirOUT(dirnameOUT);
    if(!dirOUT.exists())
    {
        dirOUT.mkdir(dirnameOUT);
    }
    QString dirnameDONE = "C:/Progs/bmpFilter/processed";   // Введите адрес папки для исходных файлов
    QDir dirDONE(dirnameDONE);
    if(!dirDONE.exists())
    {
        dirDONE.mkdir(dirnameDONE);
    }
    QFile fileLOG("C:/Progs/bmpFilter/log.txt");            // Введите адрес файла для логов
    if(!fileLOG.exists())
    {
        fileLOG.open(QIODevice::WriteOnly | QIODevice::Append);
    }
    QFile fileMETRICS("C:/Progs/bmpFilter/metrics.txt");    // Введите адрес папки для метрик
    if(!fileMETRICS.exists())
    {
        fileMETRICS.open(QIODevice::WriteOnly | QIODevice::Append);
    }

    //---------------------------------------------

    int numFiles = 0;
    int another_numFiles = 0; // for comparation
    qint64 totalTime = 0;
    QElapsedTimer timer;
    timer.start();

    QString filename = "";

    QTextStream outLOG(&fileLOG);
    QTextStream outMETRICS(&fileMETRICS);

    outLOG << QTime::currentTime().toString("hh:mm:ss.zzz") << "    " << "Monitoring folder " << dirnameIN << " ..."<< endl << endl;

    while(true)  // бесконечный цикл обработки
    {

            while(!dirIN.isEmpty())
            {
                dirIN.setSorting(QDir::Time);                           // будем начинать с тех, которые пришли первыми (снизу папки)
                QFileInfoList list = dirIN.entryInfoList();
                QFileInfo fileInfo = list.at(2);                        // не считая . и .. , которые всегда в начале папки
                filename = fileInfo.fileName();                         // имя текущего файла для обработки

                outLOG << QTime::currentTime().toString("hh:mm:ss.zzz") << "     " << "файл " + filename + " начал обрабатываться." << endl;

                // ---- bmp -> bin --------------

                QImage img;

                img.load(dirnameIN + "/" + filename);

                Ipp8u* imgMass = ippsMalloc_8u(img.width() * img.height());
                Ipp8u* imgMassOUT = ippsMalloc_8u(img.width() * img.height());

                const uchar *data = img.constBits();
                const int size = (img.width() * img.height());

                ippsCopy_8u(data, imgMass, size);

                int width = img.width();
                int height = img.height();
                //int len = width * height;
                //int CmlxLen = len/2;

                //---------------------------------

                    if(true)
                    {

                    IppStatus status = ippStsNoErr;

                    int srcStep = img.width(), dstStep = img.width();         /* Steps, in bytes, through the source/destination images */
                    IppiSize roiSize = { width, height }; /* Size of source/destination ROI in pixels */
                    Ipp8u *pBuffer = NULL;                /* Pointer to the work buffer */
                    int iTmpBufSize = 0;                  /* Common work buffer size */
                    IppiBorderType borderType = ippBorderRepl;
                    Ipp8u borderValue = 254;
                    IppiSize maskSize = {5,5};
                    int numChannels = 1;

                    status = ippiFilterBoxBorderGetBufferSize(roiSize, maskSize, ipp8u, numChannels, &iTmpBufSize);

                    pBuffer = ippsMalloc_8u(iTmpBufSize);

                    status = ippiFilterBoxBorder_8u_C1R(imgMass, srcStep, imgMassOUT, dstStep, roiSize, maskSize, borderType, &borderValue, pBuffer);

                    //------------ Упаковка файлов ------
                    QImage new_image((Ipp8u*)imgMassOUT, width, height, QImage::Format_Grayscale8, nullptr, nullptr);
                    new_image.save(dirnameOUT + "/" + filename);
                    img.save(dirnameDONE + "/" + filename);
                    img.detach();
                    QFile fileDelete(dirnameIN + "/" + filename);
                    fileDelete.remove();
                    fileDelete.close();

                    outLOG << QTime::currentTime().toString("hh:mm:ss.zzz") << "     " << "результаты обработки " + filename + " помещены в соответствующие директории." << endl;
                    //-----------------------------------

                    ippsFree(pBuffer);
                    ippiFree(imgMass);
                    ippiFree(imgMassOUT);
                    //printf("Exit status %d (%s)\n", (int)status, ippGetStatusString(status));
                    //qDebug() << status << endl;
                    }

                    numFiles++;
                    totalTime += timer.elapsed();
                    timer.restart();
                    outLOG << QTime::currentTime().toString("hh:mm:ss.zzz") << "     " << "файл " + filename + " закончил обрабатываться." << endl;

            }

                    // При необходимости добавить тайм-айт или блок отслеживания НОВЫХ изменений в файле!

            if(numFiles != another_numFiles)  // обновляем метрику после каждой новой порции обработанных файлов
            {
            double averageFilesPerMinute = (static_cast<double>(numFiles) / static_cast<double>(totalTime))*(1000.0 * 60.0);

            outMETRICS << QTime::currentTime().toString("hh:mm:ss.zzz") << "\n"
            << "Processed " << numFiles << " files in " << totalTime << " milliseconds." << "\n"
            << "Average processing time per file is " << (static_cast<double>(totalTime))/(static_cast<double>(numFiles)) << " milliseconds." << "\n"
            << "Average number of files processed per minute is " << averageFilesPerMinute << endl;

            another_numFiles = numFiles;
            }

    }


    // Malloc free and closing files

    fileLOG.close();
    fileMETRICS.close();
    //-------------------------------

}
