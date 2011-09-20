QGIS_DIR = /usr/include/qgis/
PROJECT_ROOT = ../
TEMPLATE = lib
CONFIG = qt
QT += xml qt3support
unix:LIBS += -L/$$QGIS_DIR/lib -lqgis_core -lqgis_gui -lpng 
INCLUDEPATH += $$QGIS_DIR/src/ui $$QGIS_DIR/src/plugins /usr/include/gdal $$QGIS_DIR/src/* $$QGIS_DIR/src/gui $$QGIS_DIR/src/raster $$QGIS_DIR/src/core $$QGIS_DIR/src/core/raster $$QGIS_DIR $$PROJECT_ROOT/inc PROJECT_ROOT/src/*.cpp
SOURCES = qgiscloudrecognitionplugin.cpp interface.cpp $$PROJECT_ROOT/src/convert.cpp $$PROJECT_ROOT/src/detection.cpp $$PROJECT_ROOT/src/pic.cpp $$PROJECT_ROOT/src/readhdf.cpp
HEADERS = qgiscloudrecognitionplugin.h interface.h $$PROJECT_ROOT/inc/convert.h $$PROJECT_ROOT/inc/detection.h $$PROJECT_ROOT/inc/pic.h $$PROJECT_ROOT/inc/readhdf.h
DEST = cloudrecognitionplugin.so
DEFINES += GUI_EXPORT= CORE_EXPORT=
