#ifndef QGSPOINTCONVERTERPLUGIN_H
#define QGSPOINTCONVERTERPLUGIN_H
#include "qgisplugin.h"
#include "qgsgeometry.h"
#include "qgsrasterlayer.h"
#include "qgisinterface.h"
#include "qgsgeometry.h"
#include "qgsvectordataprovider.h"
#include "qgsvectorlayer.h"
#include <qgsrasterbandstats.h> 
#include <qgsrasterpyramid.h>
#include <QObject>
#include <QListWidget>
#include <QTextStream>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QtGui>
#include "interface.h"
#ifdef WIN32
#define QGISEXTERN extern "C" __declspec( dllexport )
#else
#define QGISEXTERN extern "C"
#endif
class QgsCloudRecognitionPlugin: public QObject, public QgisPlugin
{
  Q_OBJECT
public:
  QgsCloudRecognitionPlugin(QgisInterface* iface);
  ~QgsCloudRecognitionPlugin();
  void initGui();
  void unload();
  
public slots:
  void MainWindow();
  void openHDF();
  void deletefromlist();
  void saveimage();
  void Process();
private:
  QgisInterface *mIface;
  QAction *mAction;
  QWidget *window;
  QString path;
  
  QListWidget *hdfedit;
  QLineEdit *imgedit;
  void showWindow();
};  
  #endif
  