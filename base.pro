QT -= gui
QT += widgets

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mainFunc.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


# для IPP
INCLUDEPATH += "C:\Program Files (x86)\Intel\oneAPI\ipp\latest\include"

LIBS += -lUser32
LIBS  += -L"C:\Program Files (x86)\Intel\oneAPI\ipp\latest\lib\intel64" -lippdcmt -lippcvmt -lippimt -lippccmt -lippvmmt -lippsmt -lippcoremt

HEADERS += \
    mainFunc.h
