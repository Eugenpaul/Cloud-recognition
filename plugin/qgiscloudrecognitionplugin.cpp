#include "qgiscloudrecognitionplugin.h"

QgsCloudRecognitionPlugin::QgsCloudRecognitionPlugin(QgisInterface* iface): mIface(iface), mAction(0)
{
}
QgsCloudRecognitionPlugin::~QgsCloudRecognitionPlugin()
{
}
void QgsCloudRecognitionPlugin::initGui()
{
  mAction = new QAction(tr("&CR plugin"), this);
  hdfedit = new QListWidget();
  imgedit = new QLineEdit();
  hdfedit->setSortingEnabled(true);
  connect(mAction, SIGNAL(activated()), this, SLOT(MainWindow()));
  mIface->addToolBarIcon(mAction);
  mIface->addPluginToMenu(tr("&CR plugin"), mAction);
}
void QgsCloudRecognitionPlugin::unload()
{
  mIface->removeToolBarIcon(mAction);
  mIface->removePluginMenu(tr("&Convert to point"), mAction);
  delete mAction;
}



void QgsCloudRecognitionPlugin::MainWindow()
{
  showWindow();
  
  
  
  //
  
  
  
  
  
  
}

void QgsCloudRecognitionPlugin::Process()
{
  QString im = imgedit->text();
  QString imm = im + ".mask.png";
  QString p1 = hdfedit->item(0)->text();
  QString ph = hdfedit->item(1)->text();
  QString pq = hdfedit->item(2)->text();
  
  process(p1, ph, pq, im);
  
  QFileInfo myFileInfo(im);
  QString myBaseNameQString = myFileInfo.baseName();
  mIface->addRasterLayer(im, myBaseNameQString);
  QFileInfo myFileInfo2(imm);
  myBaseNameQString = myFileInfo2.baseName();
  mIface->addRasterLayer(im, myBaseNameQString);
  
}
void QgsCloudRecognitionPlugin::openHDF()
{
  path = QFileDialog::getOpenFileName();
  if(!path.isNull())
  {
    hdfedit->addItem(path);
  }
}

void QgsCloudRecognitionPlugin::saveimage()
{
  path = QFileDialog::getSaveFileName();
  if(!path.isNull())
  {
    imgedit->setText(path);
  }
}


void QgsCloudRecognitionPlugin::deletefromlist()
{
  hdfedit->removeItemWidget(hdfedit->currentItem());
}

void QgsCloudRecognitionPlugin::showWindow()
{
  
  
  QWidget *window = new QWidget();
  window->resize(600, 500);
  window->show();
  window->setWindowTitle(
    QApplication::translate("crplugin", "Cloud recognition"));
    window->activateWindow();
    QLabel *hdflabel = new QLabel;
    QLabel *imglabel = new QLabel;
    QGridLayout *layout = new QGridLayout;
    QPushButton *addbutton = new QPushButton;
    QPushButton *delbutton = new QPushButton;
    QPushButton *setbutton = new QPushButton;
    QPushButton *procbutton = new QPushButton;
    addbutton->setText("Add");
    delbutton->setText("Delete");
    setbutton->setText("Set");
    procbutton->setText("Run");
    hdflabel->setText("hdf files");
    imglabel->setText("png result");
    layout->addWidget(hdflabel);
    layout->addWidget(hdfedit);
    layout->addWidget(addbutton);
    layout->addWidget(delbutton);
    layout->addWidget(imglabel);
    layout->addWidget(imgedit);
    layout->addWidget(setbutton);
    layout->addWidget(procbutton);
    window->setLayout(layout);
    QString path;
    QObject::connect(addbutton, SIGNAL(clicked()), this, SLOT(openHDF()));
    QObject::connect(delbutton, SIGNAL(clicked()), this, SLOT(deletefromlist()));
    QObject::connect(setbutton, SIGNAL(clicked()), this, SLOT(saveimage()));
    QObject::connect(procbutton, SIGNAL(clicked()), this, SLOT(Process()));
}



QGISEXTERN QgisPlugin* classFactory(QgisInterface* iface)
{
  return new QgsCloudRecognitionPlugin(iface);
}
QGISEXTERN QString name()
{
  return "Cloud recognition plugin";
}
QGISEXTERN QString description()
{
  return "A plugin that opens mod02.*.hdf files, creates cloud mask, and saves it to png";
}
QGISEXTERN QString version()
{
  return "0.1a";
}

QGISEXTERN int type()
{
  return QgisPlugin::UI;
}

QGISEXTERN void unload(QgisPlugin* theQgsCloudRecognitionPluginPointer)
{
  delete theQgsCloudRecognitionPluginPointer;
}
