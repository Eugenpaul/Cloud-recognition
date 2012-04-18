cmake_minimum_required(VERSION 2.4)

########################################################
# Files

SET (CLOUDRECOGNITION_SRCS
     qgiscloudrecognitionplugin.cpp
     ../src/convert.cpp
     ../src/detection.cpp
     ../src/pic.cpp
     ../src/files.cpp
     ../src/readhdf.cpp
     interface.cpp
)

SET (CLOUDRECOGNITION_MOC_HDRS
     qgiscloudrecognitionplugin.h
     ../inc/convert.h
     ../inc/detection.h
     ../inc/pic.h
     ../inc/files.h
     ../inc/readhdf.h
     interface.h
)

SET (CLOUDRECOGNITION_UIS qgscloudrecognitionbase.ui)
SET (QGIS_DIR /home/ugnpaul/git/Quantum-GIS/)
SET (QGIS_PLUGIN_DIR /home/ugnpaul/git/Quantum-GIS/plugins)
add_definitions( -DCORE_EXPORT= )
add_definitions( -DGUI_EXPORT= )

########################################################
# Build
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtXml REQUIRED)
INCLUDE(${QT_USE_FILE})

QT4_WRAP_UI (CLOUDRECOGNITION_UIS_H  ${CLOUDRECOGNITION_UIS})

QT4_WRAP_CPP (CLOUDRECOGNITION_MOC_SRCS  ${CLOUDRECOGNITION_MOC_HDRS})

ADD_LIBRARY (cloudrecognitionplugin MODULE ${CLOUDRECOGNITION_SRCS} ${CLOUDRECOGNITION_MOC_SRCS} ${CLOUDRECOGNITION_UIS_H})



INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_BINARY_DIR}
     ../inc/
     ../src/
     ${GDAL_INCLUDE_DIR}
     ${GEOS_INCLUDE_DIR}
     ${QGIS_DIR}/src/ui
     ${QGIS_DIR}/src/plugins
     ${QGIS_DIR}/src/
     ${QGIS_DIR}/src/gui
     #${QGIS_DIR}/src/raster
     ${QGIS_DIR}/src/core
     ${QGIS_DIR}/src/core/raster
     ${QGIS_DIR}/
)


TARGET_LINK_LIBRARIES(cloudrecognitionplugin
  qgis_core
  qgis_gui
)


########################################################
# Install

INSTALL(TARGETS cloudrecognitionplugin
  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR})