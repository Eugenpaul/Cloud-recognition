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
//#include <qgsrasterpyramid.h>
#include <QObject>
#include <QListWidget>
#include <QTextStream>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QtGui>
#include "interface.h"
#include "../inc/base.h"
#include "../inc/addition.h"

#ifdef WIN32
#define QGISEXTERN extern "C" __declspec( dllexport )
#else
#define QGISEXTERN extern "C"
#endif

#define MAX_MESSAGE_LENGTH 2048
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
  void addAll();
  void addGroupName();
  void addtodatabase();
  void clearlist();
  void createDB();
  void dbConfigWindow();
  void deletefromlist();
  void saveimage();
  void selectfromdb();
  void openImages(QListWidgetItem *item);
  void Process();
  void rewriteQuestion(QWidget *w, char *type, char *name, char *path, bool *rewrite);
private:
  QgisInterface *mIface;
  QAction *mAction;
  QWidget *window;
  QString path;
  QProgressBar *progressbar;
  QListWidget *hdfbase;
  QListWidget *hdfedit;
  QLineEdit *imgedit;
  QLineEdit *searchedit;
  QLineEdit *user_edit;
  QLineEdit *password_edit;
  QLabel *db_status;
  void showWindow();
};
  #endif
