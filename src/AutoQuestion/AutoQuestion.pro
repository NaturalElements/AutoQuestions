
TEMPLATE = app
TARGET = AutoQuestion
DESTDIR = ../../binLinux
QT += core widgets gui printsupport
CONFIG += debug_and_release
DEFINES += _UNICODE QT_WIDGETS_LIB QT_PRINTSUPPORT_LIB
INCLUDEPATH += . \
    ./GeneratedFiles
DEPENDPATH += .
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

CONFIG(debug, debug|release){
	INCLUDEPATH += ./GeneratedFiles/debug 
	MOC_DIR += ./GeneratedFiles/debug 
	OBJECTS_DIR += debug
}else{
	INCLUDEPATH += ./GeneratedFiles/release 
	MOC_DIR += ./GeneratedFiles/release 
	OBJECTS_DIR += release
}

include(AutoQuestion.pri)
