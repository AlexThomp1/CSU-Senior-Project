QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settings.cpp

HEADERS += \
    mainwindow.h \
    settings.h

FORMS += \
    mainwindow.ui \
    settings.ui

RC_ICONS = images\image_converter_icon.ico

# For OpenCV & FreeImage
INCLUDEPATH += E:\DOWNLOADS\opencv\build\include
INCLUDEPATH += E:\DOWNLOADS\FreeImageBinaries\Dist\x64

LIBS += E:\DOWNLOADS\opencv\build\install\x64\mingw\bin\libopencv_core460.dll
LIBS += E:\DOWNLOADS\opencv\build\install\x64\mingw\bin\libopencv_highgui460.dll
LIBS += E:\DOWNLOADS\opencv\build\install\x64\mingw\bin\libopencv_imgcodecs460.dll
LIBS += E:\DOWNLOADS\opencv\build\install\x64\mingw\bin\libopencv_imgproc460.dll
LIBS += E:\DOWNLOADS\opencv\build\install\x64\mingw\bin\libopencv_features2d460.dll
LIBS += E:\DOWNLOADS\opencv\build\install\x64\mingw\bin\libopencv_calib3d460.dll
LIBS += -LE:/DOWNLOADS/FreeImageBinaries/Dist/x64 -lFreeImage

# more correct variant, how set includepath and libs for mingw
# add system variable: OPENCV_SDK_DIR=D:/opencv/opencv-build/install
# read http://doc.qt.io/qt-5/qmake-variable-reference.html#libs

#INCLUDEPATH += $$(OPENCV_SDK_DIR)/include

#LIBS += -L$$(OPENCV_SDK_DIR)/x86/mingw/lib \
#        -lopencv_core320        \
#        -lopencv_highgui320     \
#        -lopencv_imgcodecs320   \
#        -lopencv_imgproc320     \
#        -lopencv_features2d320  \
#        -lopencv_calib3d320

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

DISTFILES +=
