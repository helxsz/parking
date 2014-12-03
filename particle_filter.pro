#-------------------------------------------------
#
# Project created by QtCreator 2014-06-08T01:54:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = particle_filter
TEMPLATE = app
QT += network

SOURCES += main.cpp\
    tracking/object_tracker.cpp \
    tracking/cv_onlinemil.cpp \
    tracking/cv_onlineboosting.cpp \
    Player.cpp \
    detector/detector.cpp \
    parkingapp.cpp \
    Settings.cpp \
    detector/edgedetectors.cpp \
    detector/hogdetector.cpp \
    utility/Camera.cpp \
    utility/QtOpenCV.cpp \
    utility/UDirectory.cpp \
    utility/UFile.cpp \
    webdialog.cpp

HEADERS  += \
    tracking/object_tracker.h \
    tracking/cv_onlinemil.h \
    tracking/cv_onlineboosting.h \
    player.h \
    Settings.h \
    detector/detector.h \
    parkingapp.h \
    detector/edgedetectors.h \
    detector/hogdetector.h \
    utility/QtOpenCV.cpp \
    utility/Camera.h \
    utility/UDirectory.h \
    utility/UFile.h \
    webdialog.h\

FORMS    += \
    parkingapp.ui \
    webdialog.ui

#   /opt/opencv3/include/opencv \

INCLUDEPATH += /usr/local/include/opencv \
               /usr/local/include \
               /usr/local/include/eigen3 \
               /usr/include/gsl \
               /opt/bgslibrary3 \
               /opt/pbd/include
               /opt/armadillo/include/armadillo_bits
#        -lopencv_calib3d \
#        -lopencv_features2d \
#        -lopencv_flann \
#        -lopencv_videostab \
#        -lopencv_nonfree \
#         -lopencv_legacy \
#        -lopencv_ml \
#        -lopencv_objdetect \
#        -lopencv_photo \
#        -lopencv_stitching \
#        -lopencv_superres \
#        -lopencv_ts \
#
#         -lopencv_gpu \
#        -lopencv_contrib \

#    -L/usr/local/lib \    -L/opt/opencv3/lib \
LIBS += -L/usr/local/lib \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lopencv_video \
        -lopencv_objdetect \
        -lopencv_ml \
        -lopencv_calib3d \
        -lopencv_features2d \
        -lopencv_flann \
        -lopencv_videostab \
        -lopencv_nonfree \
        -lopencv_legacy \
#        -lopencv_ml \
#        -lopencv_objdetect \
        -lopencv_photo \
        -lopencv_stitching \
        -lopencv_superres \
        -lopencv_ts \
        -lopencv_gpu \
        -lopencv_contrib \
        -lmatio \
        -lboost_filesystem \
        -lboost_context \
        -lboost_date_time \
        -lboost_log \
        -L/usr/lib \
        -lgsl \
        -lgslcblas \
        -L/opt/bgslibrary3/build \
        -lbgs \
        -L/opt/pbd/build/src \
        -lPartsBasedDetector \
        -L/opt/armadillo/build \
        -larmadillo

OTHER_FILES += \
    config/FrameDifferenceBGS.xml \
    config/VideoCapture.xml \
    config/PreProcessor.xml \
    config/FrameProcessor.xml \
    config/test.json \
    model/car.xml
