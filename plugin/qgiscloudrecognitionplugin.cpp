#include "qgiscloudrecognitionplugin.h"

#define WINDOW_INITIAL_HEIGHT 400
#define WINDOW_INITIAL_WIDTH 600

static const QString sName = QObject::tr( "Cloud Recognition" );
static const QString sDescription = QObject::tr( "A plugin that opens mod02.*.hdf files, creates cloud mask, and saves it to tiff" );
static const QString sCategory = QObject::tr( "Plugins" );
static const QString sPluginVersion = QObject::tr( "Version 0.2a" );
static const QgisPlugin::PLUGINTYPE sPluginType = QgisPlugin::RENDERER;

QgsCloudRecognitionPlugin::QgsCloudRecognitionPlugin(QgisInterface* iface):QgisPlugin( sName, sDescription, sCategory, sPluginVersion, sPluginType ), mIface(iface), mAction(0)
{
  //mDataFile = QDir().homePath() + "/.qgis/cloudrecognitionplugindata";
  QString userPluginPath = QgsApplication::qgisUserDbFilePath() + "/plugins/cloudrecognitionplugin";
  mDataFile = userPluginPath + "/cloudrecognitionplugindata";
  QString systemPluginPath = QgsApplication::prefixPath() + "/plugins/cloudrecognitionplugin";
  QString localeFullName;
  bool overrideLocale = QSettings().value( "locale/overrideFlag", QVariant(false)).toBool();
  if (!overrideLocale)
  {
    localeFullName = QLocale::system().name();
  }
  else
  {
    localeFullName = QSettings().value( "locale/userLocale", QVariant("")).toString();
  }
  QString translationPath;
  if (QFileInfo(userPluginPath).exists())
  {
      translationPath = userPluginPath + "/i18n/cloudrecognition_" + localeFullName + ".qm";
  }
  else
  {
      translationPath = systemPluginPath + "/i18n/cloudrecognition_" + localeFullName + ".qm";
  }
  QTranslator *translator;
  QString localePath = translationPath;
  if (QFileInfo( localePath ).exists())
  {
    translator = new QTranslator();
    translator->load( localePath );
    QCoreApplication::installTranslator( translator );
  }

}

QgsCloudRecognitionPlugin::~QgsCloudRecognitionPlugin()
{
}

void QgsCloudRecognitionPlugin::initGui()
{
  mAction = new QAction(tr("&CR plugin"), this);
  connect(mAction, SIGNAL(activated()), this, SLOT(mainWindow()));
  mIface->addToolBarIcon(mAction);
  mIface->addPluginToRasterMenu(tr("&CR plugin"), mAction);
}

void QgsCloudRecognitionPlugin::unload()
{
  mIface->removeToolBarIcon(mAction);
  mIface->removePluginMenu(tr("&CR plugin"), mAction);
  delete mAction;
}

void QgsCloudRecognitionPlugin::rewriteQuestion(QWidget *w, QString type, QString name, QString path, bool *rewrite)
{
  int reply;
  QString question = QString(tr("File of type %1 for %2 already exists:\n \'%3\'\n Do you want to create a new one in the folder specified and replace it in the list")).arg(type, name, path);
  QMessageBox msgBox;

  msgBox.addButton(QMessageBox::Yes);
  msgBox.addButton(QMessageBox::No);
  msgBox.setText(question);

  reply = msgBox.exec();

  if (reply == QMessageBox::Yes)
  {
    *rewrite = true;
  }
  else
  {
    *rewrite = false;
  }
  return;
}

void QgsCloudRecognitionPlugin::processData()
{
  QString qDestinationDir = mMainWidget->destinationDirLineEdit->text();
  QMessageBox msgBox;
  QString failedList;
  ProgressViewer *progress = new ProgressViewer();
  QObject::connect(progress, SIGNAL(valueChanged(int)),
                   mMainWidget->progressBar, SLOT(setValue(int)));
  QString qName, qPathKM, qPathH, qPathQ, qImg, qImgMask;
  int i;

  if (mMainWidget->destinationDirLineEdit->text().isEmpty())
  {
    msgBox.setText(tr("Destination folder is not specified\n"));
    msgBox.exec();
    return;
  }
  if (!directoryExists(qDestinationDir))
  {
    if (!createDirectory(qDestinationDir))
    {
      msgBox.setText(tr("Cannot create destination folder: ") + qDestinationDir);
      msgBox.exec();
      return;
    }
  }
  if (mMainWidget->selectedDataListWidget->count() != 0)
  {
    for (i = 0; i < mMainWidget->selectedDataListWidget->count(); i++)
    {
      mMainWidget->selectedDataListWidget->item(i)->setText(mMainWidget->selectedDataListWidget->item(i)->text());

      qName = mMainWidget->selectedDataListWidget->item(i)->text();
      if (!getPathsByName(qName, qPathKM, qPathH, qPathQ, qImg, qImgMask, mDataFile))
      {
        msgBox.setText(tr("Cannot get needed info from the data list"));
        return;
      }

      if (qPathKM.isEmpty() || qPathH.isEmpty() || qPathQ.isEmpty())
      {
        msgBox.setText(tr("Not enough files for processing\n"));
        return;
      }
      else
      {
        bool rewrite = false;
        if ((qDestinationDir[qDestinationDir.size() - 1] != '/') && (qDestinationDir[qDestinationDir.size() - 1] != '\\'))
        {
          qDestinationDir += "/";
        }

        if (!qImg.isEmpty())
        {
          if (!fileExists(qImg))
          {
            qImg = "";
          }
          else
          {
            rewriteQuestion(window, ".surface.", qName, qImg, &rewrite);
            if (rewrite)
            {
              qImg = "";
            }
          }
        }
        if (qImg.isEmpty())
        {
          qImg = qDestinationDir + qName + ".surface.tiff";
        }
        else
        {
          qImg = "";
        }

        if (!qImgMask.isEmpty())
        {
          if (!fileExists(qImgMask))
          {
            qImgMask = "";
          }
          else
          {
            rewriteQuestion(window, ".mask.", qName, qImgMask, &rewrite);
            if (rewrite)
            {
              qImgMask = "";
            }
          }
        }
        if (qImgMask.isEmpty())
        {
          qImgMask = qDestinationDir + qName + ".mask.tiff";
        }
        else
        {
          qImgMask = "";
        }

        progress->setValue(5);

        if (!qImgMask.isEmpty() || !qImg.isEmpty())
        {
          if (!process(qPathKM, qPathH, qPathQ, qImg, qImgMask, progress))
          {
            msgBox.setText(tr("Error while processing ") + qName);
            msgBox.exec();
          }
          else
          {
            if (!addFile(qImg, mDataFile))
            {
              failedList += qImg + "\n";
            }
            if (!addFile(qImgMask, mDataFile))
            {
              failedList += qImgMask + "\n";
            }
          }
          progress->setValue(100);
        }
      }
    }
    if (!failedList.isEmpty())
    {
      msgBox.setText(tr("Error: cannot add the following paths to the data list:\n") + failedList);
      msgBox.exec();
    }
  }
  else
  {
    msgBox.setText(tr("To process data you should select the necessary names from the left list"));
    msgBox.exec();
    return;
  }

  msgBox.setText(tr("Done\n"));
  msgBox.exec();
  progress->setValue(0);
  return;

}

void QgsCloudRecognitionPlugin::addGroupName()
{
  int i = 0;
  bool same_found = false;
  if (mMainWidget->searchResultsListWidget->count())
  {
    if (mMainWidget->searchResultsListWidget->currentItem())
    {
      for (i = 0; i < mMainWidget->selectedDataListWidget->count(); i++)
      {
        if (mMainWidget->selectedDataListWidget->item(i)->text() == mMainWidget->searchResultsListWidget->currentItem()->text())
        {
          same_found = true;
        }
      }
      if (!same_found)
      {
        mMainWidget->selectedDataListWidget->addItem(mMainWidget->searchResultsListWidget->currentItem()->text());
      }
    }
  }
  return;
}

void QgsCloudRecognitionPlugin::addAll()
{
  int i = 0;
  int j = 0;
  bool same_found = false;
  for (i = 0; i < mMainWidget->searchResultsListWidget->count(); i++)
  {
    for (j = 0; j < mMainWidget->selectedDataListWidget->count(); j++)
    {
      if (mMainWidget->selectedDataListWidget->item(j)->text() == mMainWidget->searchResultsListWidget->item(i)->text())
      {
        same_found = true;
      }
    }
    if (!same_found)
    {
      mMainWidget->selectedDataListWidget->addItem(mMainWidget->searchResultsListWidget->item(i)->text());
    }
  }
  return;
}

void QgsCloudRecognitionPlugin::browseDestination()
{
  QFileDialog *dialog = new QFileDialog();
  dialog->setFileMode(QFileDialog::Directory);
  dialog->setOption(QFileDialog::ShowDirsOnly, true);
  if (dialog->exec())
  {
    mMainWidget->destinationDirLineEdit->setText(dialog->selectedFiles()[0]);
  }
  return;
}

void QgsCloudRecognitionPlugin::openImages(QListWidgetItem *item)
{
  QMessageBox msgBox;
  QString qPathKM, qPathH, qPathQ, qImg, qImgMask;
  QString qName = item->text();
  if (!getPathsByName(qName, qPathKM, qPathH, qPathQ, qImg, qImgMask, mDataFile))
  {
    return;
  }

  if (qImg.isEmpty() && qImgMask.isEmpty())
  {
    msgBox.setText(tr("Raster is not generated yet"));
    msgBox.exec();
    return;
  }
  else if (qImg.isEmpty())
  {
    msgBox.setText(tr("Surface raster is not generated"));
    msgBox.exec();
  }
  else if (qImg.isEmpty())
  {
    msgBox.setText(tr("Mask raster is not generated"));
    msgBox.exec();
  }

  QString myBaseNameQString;
  if (!qImg.isEmpty())
  {
    if (fileExists(qImg))
    {
      QFileInfo myFileInfo(qImg);
      myBaseNameQString = myFileInfo.completeBaseName();
      mIface->addRasterLayer(qImg, myBaseNameQString);
    }
    else
    {
      msgBox.setText(tr("File doesn't exist:\n") + qImg);
      msgBox.exec();
    }
  }

  if (!qImgMask.isEmpty())
  {
    if (fileExists(qImgMask))
    {
      QFileInfo myFileInfo(qImgMask);
      myBaseNameQString = myFileInfo.completeBaseName();
      mIface->addRasterLayer(qImgMask, myBaseNameQString);
    }
    else
    {
      msgBox.setText(tr("File doesn't exist:\n") + qImgMask);
      msgBox.exec();
    }
  }
  return;
}

void QgsCloudRecognitionPlugin::searchInData()
{
    QMessageBox msgBox;
    int n = 0;
    int i = 0;
    QString string;
    if (!fileExists(mDataFile))
    {
      return;
    }

    QStringList result = QStringList();
    QString searchString = mMainWidget->searchLineEdit->text();
    if (!searchRequest(searchString, result, mDataFile))
    {
        return;
    }
    else
    {
      mMainWidget->searchResultsListWidget->clear();
      if (!result.isEmpty())
      {
        for (i = 0; i < result.size(); i++)
        {
            mMainWidget->searchResultsListWidget->addItem(result.at(i));
        }
      }
    }
    return;
}

void QgsCloudRecognitionPlugin::addFilesToDataList()
{
    QFileDialog *openDialog = new QFileDialog();
    openDialog->setFileMode(QFileDialog::ExistingFiles);
    QStringList filters = QStringList();
    filters << "any suitable files (MOD*.hdf *.surface.tiff *.mask.tiff)"
            << "hdf MOD files (MOD*.hdf)"
            << "result images (*.surface.tiff *.mask.tiff)"
            << "any files (*)";
    openDialog->setNameFilterDetailsVisible(true);
    openDialog->setNameFilters(filters);
    QStringList fileNames;
    QMessageBox msgBox;
    if (openDialog->exec())
    {
        fileNames = openDialog->selectedFiles();
    }
    if (!fileNames.isEmpty())
    {
        int successfullyAddedNum = 0;
        for (int i = 0; i < fileNames.size(); i++)
        {
          if (!addFile(fileNames[i], mDataFile))
          {
            msgBox.setText("Error: cannot add file : \n\'" + fileNames[i] + "\'");
            msgBox.exec();
          }
          else
          {
            successfullyAddedNum ++;
          }
        }
        /*sprintf(msgText, "%d file%s ha%s been added\n", successfully_added_num,
                                                        successfully_added_num != 1? "s":"",
                                                        successfully_added_num != 1? "ve":"s"
               );*/
        msgBox.setText(QString("files added: %1 ").arg(successfullyAddedNum));
        msgBox.exec();
    }
    return;
}


void QgsCloudRecognitionPlugin::deletefromlist()
{
  if (mMainWidget->selectedDataListWidget->currentItem())
  {
    delete mMainWidget->selectedDataListWidget->currentItem();
  }
  return;
}

void QgsCloudRecognitionPlugin::clearList()
{
  mMainWidget->selectedDataListWidget->clear();
  return;
}

void QgsCloudRecognitionPlugin::mainWindow()
{
  QWidget *window = new QWidget();
  mMainWidget = new Ui::QgsCloudRecognitionForm();
  mMainWidget->setupUi(window);
  window->resize(WINDOW_INITIAL_WIDTH, WINDOW_INITIAL_HEIGHT);
  QRect rect = QApplication::desktop()->geometry();
  window->move (rect.center() - window->rect().center());
  window->show();
  window->setWindowTitle(QApplication::translate("crplugin", "Cloud recognition"));
  window->activateWindow();

  QObject::connect(mMainWidget->searchButton, SIGNAL(clicked()), this, SLOT(searchInData()));
  QObject::connect(mMainWidget->searchLineEdit, SIGNAL(returnPressed()), mMainWidget->searchButton, SIGNAL(clicked()));
  QObject::connect(mMainWidget->addButton, SIGNAL(clicked()), this, SLOT(addGroupName()));
  QObject::connect(mMainWidget->addAllButton, SIGNAL(clicked()), this, SLOT(addAll()));
  QObject::connect(mMainWidget->deleteButton, SIGNAL(clicked()), this, SLOT(deletefromlist()));
  QObject::connect(mMainWidget->clearButton, SIGNAL(clicked()), this, SLOT(clearList()));
  QObject::connect(mMainWidget->addFilesButton, SIGNAL(clicked()), this, SLOT(addFilesToDataList()));
  QObject::connect(mMainWidget->selectedDataListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                   this,SLOT(openImages(QListWidgetItem*)));
  QObject::connect(mMainWidget->searchResultsListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                   this,SLOT(openImages(QListWidgetItem*)));
  QObject::connect(mMainWidget->browseButton, SIGNAL(clicked()), this, SLOT(browseDestination()));
  QObject::connect(mMainWidget->runButton, SIGNAL(clicked()), this, SLOT(processData()));
  QObject::connect(mMainWidget->closeButton, SIGNAL(clicked()), window, SLOT(close()));
  return;
}



QGISEXTERN QgisPlugin* classFactory(QgisInterface* iface)
{
  return new QgsCloudRecognitionPlugin(iface);
}

QGISEXTERN QString name()
{
  return sName;
}

QGISEXTERN QString description()
{
  return sDescription;
}

QGISEXTERN QString version()
{
  return sPluginVersion;
}

QGISEXTERN QString category()
{
  return sCategory;
}

QGISEXTERN int type()
{
  return sPluginType;
}

QGISEXTERN void unload(QgisPlugin* theQgsCloudRecognitionPluginPointer)
{
  delete theQgsCloudRecognitionPluginPointer;
}
