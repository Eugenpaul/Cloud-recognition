#ifndef QGSPOINTCONVERTERPLUGIN_H
#define QGSPOINTCONVERTERPLUGIN_H


//#include <qgsrasterpyramid.h>
#include <QObject>
#include <QListWidget>
#include <QTextStream>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QtGui>
#include <QFileInfo>


#ifdef WIN32
#define QGISEXTERN extern "C" __declspec( dllexport )
#else
#define QGISEXTERN extern "C"
#endif

#include "qgisplugin.h"



//#include "qgis.h"

#include "qgsvectorlayer.h"
#include "qgsvectordataprovider.h"
#include <qgsrasterbandstats.h>
#include "qgsgeometry.h"
//#include "qgsrasterlayer.h"
#include <qgsapplication.h>
#include "qgisinterface.h"
#include "interface.h"
#include "../inc/files.h"
#include "ui_qgscloudrecognitionbase.h"

class QgsCloudRecognitionPlugin: public QObject, public QgisPlugin
{
  Q_OBJECT
public:
  QgsCloudRecognitionPlugin(QgisInterface* iface);
  ~QgsCloudRecognitionPlugin();
  void initGui();
  void unload();

public slots:
  void mainWindow();
  void addAll();
  void addGroupName();
  void addFilesToDataList();
  void browseDestination();
  void clearList();
  void deletefromlist();
  void saveimage();
  void searchInData();
  void openImages(QListWidgetItem *item);
  void processData();
  void rewriteQuestion(QWidget *w, QString type, QString name, QString path, bool *rewrite);
private:
  QgisInterface *mIface;
  QAction *mAction;
  QWidget *window;
  QString mDataFile;
  Ui::QgsCloudRecognitionForm *mMainWidget;
};
  #endif
