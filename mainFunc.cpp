#include "mainFunc.h"
#include <QDebug>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QImageWriter>
#include <QBuffer>
#include "ipp.h"


mainFunc::mainFunc()
{
    QString dirname = "C:/Progs/bmpFilter/IN";
    QDir dir(dirname);
    QString filename = "";

    if(!dir.isEmpty())
    {
        dir.setSorting(QDir::Time);                           // будем начинать с тех, которые пришли первыми (снизу папки)
        QFileInfoList list = dir.entryInfoList();
        QFileInfo fileInfo = list.at(2);             // не считая . и .. , которые всегда в начале папки
        filename = fileInfo.fileName();

        // ---- bmp -> bin --------------

        QImage img;

        img.load(dirname + "/" + filename);

        Ipp8u* imgMass = ippsMalloc_8u(img.width() * img.height());
        Ipp8u* imgMassOUT = ippsMalloc_8u(img.width() * img.height());

        const uchar *data = img.constBits();
        const int size = (img.width() * img.height());

        ippsCopy_8u(data, imgMass, size);

        int width = img.width();
        int height = img.height();
        int len = width * height;
        int CmlxLen = len/2;

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

            QImage new_image((Ipp8u*)imgMassOUT, width, height, QImage::Format_Grayscale8, nullptr, nullptr);
            new_image.save("C:/Progs/bmpFilter/OUT/" + filename, "BMP");
            img.save("C:/Progs/bmpFilter/processed/" + filename);
            img.detach();
            QFile fileDelete(dirname + "/" + filename);
            fileDelete.remove();
            fileDelete.close();


            ippsFree(pBuffer);
            ippiFree(imgMass);
            ippiFree(imgMassOUT);
            printf("Exit status %d (%s)\n", (int)status, ippGetStatusString(status));
            qDebug() << status << endl;
            }

            //------ ко второй версии фильтра ----------------------------------
            Ipp32f* imgMass32 = ippsMalloc_32f(img.width() * img.height());
            Ipp32fc* imgMass32fc = ippsMalloc_32fc((img.width() * img.height())/2);
            Ipp32fc* IPPimg_flt = ippsMalloc_32fc((img.width() * img.height())/2);
            ippsConvert_8u32f(imgMass, imgMass32, len);
            for(int y = 0; y < len/2; y++)
            {
              imgMass32fc[y].re = imgMass32[2*y];
              imgMass32fc[y].im = imgMass32[2*y+1];
            }

            if(false)
            {
             IppStatus st;

             //Медианный фильтр
             IppiSize ROI_size, mask_size;
             ROI_size.width = img.width();
             ROI_size.height = img.height();
             int bufSize = 0;

             mask_size.width = 13;
             mask_size.height = 5;

             ippiFilterMedianBorderGetBufferSize (ROI_size, mask_size, ipp32f, 1, &bufSize);
             Ipp8u* pMedian_Buf = ippsMalloc_8u (bufSize);

             Ipp32f* pBuf_m1 = ippsMalloc_32f (CmlxLen);
             Ipp32f* pBuf_m2 = ippsMalloc_32f (CmlxLen);
             Ipp32f* pBuf_m3 = ippsMalloc_32f (CmlxLen);
             Ipp32f* pBuf_m4 = ippsMalloc_32f (CmlxLen);

             st = ippsCplxToReal_32fc (imgMass32fc, pBuf_m1, pBuf_m2, CmlxLen);

            // st = ippiFilterMedianBorder_32f_C1R (pBuf_m1, width*sizeof(Ipp32f), pBuf_m3, width*sizeof(Ipp32f), ROI_size, mask_size, ippBorderRepl, 0, pMedian_Buf);
             st = ippiFilterMedianBorder_32f_C1R (pBuf_m1, width, pBuf_m3, width, ROI_size, mask_size, ippBorderRepl, 0, pMedian_Buf);
            // st = ippiFilterMedianBorder_32f_C1R (pBuf_m2, width*sizeof(Ipp32f), pBuf_m4, width*sizeof(Ipp32f), ROI_size, mask_size, ippBorderRepl, 0, pMedian_Buf);
             st = ippiFilterMedianBorder_32f_C1R (pBuf_m2, width, pBuf_m4, width, ROI_size, mask_size, ippBorderRepl, 0, pMedian_Buf);

             ippsRealToCplx_32f (pBuf_m3, pBuf_m4, IPPimg_flt, CmlxLen);

             for(int y = 0; y < len/2; y++)
             {
               imgMass32[2*y]   = imgMass32fc[y].re;
               imgMass32[2*y+1] = imgMass32fc[y].im;
             }

             Ipp32f pMin = 0;
             Ipp32f pMax = 0;
             ippsMinMax_32f(imgMass32, len, &pMin, &pMax);                               // При необходимости доработь SF
                //qDebug() << pMin << "       " << pMax << pMax - pMin << endl;

             ippsConvert_32f8u_Sfs(imgMass32,imgMassOUT, len, ippRndNear, -7);

             QImage new_image((Ipp8u*)imgMass32, width, height, QImage::Format_Grayscale8, nullptr, nullptr);
             new_image.save("new_img.bmp", "BMP");
            }
            //--------------------------------------------------------------------------

  qDebug() << "OK" << endl;

    }



}
