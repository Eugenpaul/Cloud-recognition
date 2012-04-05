#include "qgiscloudrecognitionplugin.h"

static const QString sName = QObject::tr( "Cloud Recognition" );
static const QString sDescription = QObject::tr( "A plugin that opens mod02.*.hdf files, creates cloud mask, and saves it to tiff" );
static const QString sCategory = QObject::tr( "Plugins" );
static const QString sPluginVersion = QObject::tr( "Version 0.2a" );
static const QgisPlugin::PLUGINTYPE sPluginType = QgisPlugin::RENDERER;

QgsCloudRecognitionPlugin::QgsCloudRecognitionPlugin(QgisInterface* iface):QgisPlugin( sName, sDescription, /*sCategory,*/ sPluginVersion, sPluginType ), mIface(iface), mAction(0)
{
  mDataFile = QDir().homePath() + "/.qgis/cloudrecognitionplugindata"; //TODO use settings
}

QgsCloudRecognitionPlugin::~QgsCloudRecognitionPlugin()
{
}

void QgsCloudRecognitionPlugin::initGui()
{
  mAction = new QAction(tr("&CR plugin"), this);
  connect(mAction, SIGNAL(activated()), this, SLOT(MainWindow()));
  mIface->addToolBarIcon(mAction);
  mIface->addPluginToMenu(tr("&CR plugin"), mAction);
}

void QgsCloudRecognitionPlugin::unload()
{
  mIface->removeToolBarIcon(mAction);
  mIface->removePluginMenu(tr("&CR plugin"), mAction);
  delete mAction;
}

void QgsCloudRecognitionPlugin::MainWindow()
{
    showWindow();
}

void QgsCloudRecognitionPlugin::rewriteQuestion(QWidget *w, char *type, char *name, char *path, bool *rewrite)
{
    int reply;
    char question[2048];
    sprintf(question, "File of type %s for %s already exists:\n \'%s\'\n Do you want to create a new one in the folder specified and replace it the database", type, name, path);
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

void QgsCloudRecognitionPlugin::Process()
{
    QString qdestination_dir = mMainWidget->destinationDirLineEdit->text();
    char *destination_dir = NULL;
    QMessageBox msgBox;
    char msgText[MAX_MESSAGE_LENGTH];
    ProgressViewer *progress = new ProgressViewer();
    QObject::connect(progress, SIGNAL(valueChanged(int)),
                     mMainWidget->progressBar, SLOT(setValue(int)));
    char *path1 = NULL, *pathh = NULL, *pathq = NULL, *img = NULL, *imgmask = NULL;
    int i;
    char *name = NULL;

    if (mMainWidget->destinationDirLineEdit->text() == "")
    {
        sprintf(msgText, "Destination folder is not specified\n");
        msgBox.setText(msgText);
        msgBox.exec();
        fprintf(stderr, "Error: destination folder is not specified\n");
        return;
    }

    if (!qstring_to_string(qdestination_dir, &destination_dir, qdestination_dir.size()))
    {
        sprintf(msgText, "Error: cannot convert QString to char*\n");
        msgBox.setText(msgText);
        msgBox.exec();
        fprintf(stderr, "Error: cannot convert QString to char*\n");
        return;
    }
  /*
    if (!file_or_directory_exists(destination_dir))
    {
        if (!force_create_directory(destination_dir))
        {
            sprintf(msgText, "Error: cannot create folder \"%s\"\n", destination_dir);
            msgBox.setText(msgText);
            msgBox.exec();
            fprintf(stderr, "Error: cannot create folder \"%s\"\n", destination_dir);
            return;
        }
    }*/

  if (!directoryExists(qdestination_dir))
  {
    if (!createDirectory(qdestination_dir))
    {
      //ERROR
      return;
    }
  }
  if (mMainWidget->selectedDataListWidget->count() != 0)
  {
    for (i = 0; i < mMainWidget->selectedDataListWidget->count(); i++)
    {
      //QString item_text("<FONT COLOR=blue>" + mMainWidget->selectedDataListWidget->item(i)->text() + "</FONT>");
      mMainWidget->selectedDataListWidget->item(i)->setText(mMainWidget->selectedDataListWidget->item(i)->text());
      if (name)
      {
        free(name);
        name = NULL;
      }
            if (!qstring_to_string(mMainWidget->selectedDataListWidget->item(i)->text(), &name, mMainWidget->selectedDataListWidget->item(i)->text().size()))
            {
                sprintf(msgText, "Error: cannot convert QString to char *\n");
                msgBox.setText(msgText);
                msgBox.exec();
                fprintf(stderr, "Error: cannot conver QString to char *\n");
                return;
            }
            QString qname, qpath1, qpathh, qpathq, qimg, qimgmask;
            qname = mMainWidget->selectedDataListWidget->item(i)->text();
            if (!getPathsByName(qname, qpath1, qpathh, qpathq, qimg, qimgmask, mDataFile))
            {
              //ERROR
              return;
            }

            if (qpath1.isEmpty() || qpathh.isEmpty() || qpathq.isEmpty())
            {
              fprintf(stderr, "CHECK ERROR: files not found\n");
              return;
            }
            /*if (selectpaths(name, &path1, &pathh, &pathq, &img, &imgmask))
            {

                sprintf(msgText, "Some files are not in the database:\n"
                          "MOD021KM path = %s%s\nMOD02HKM path = %s%s\nMOD02QKM path = %s%s",
                          path1, path1 == NULL ? ": empty record" : "",
                          pathh, pathh == NULL ? ": empty record" : "",
                          pathq, pathq == NULL ? ": empty record" : ""
                          );
                msgBox.setText(msgText);
                msgBox.exec();
                fprintf(stderr, "Error: some files are not in the database\n");
            }*/
            else
            {
                  if (!qstring_to_string(qpath1, &path1, qpath1.size())
                    || !qstring_to_string(qpathh, &pathh, qpathh.size())
                    || !qstring_to_string(qpathq, &pathq, qpathq.size())
                    || !qstring_to_string(qimg, &img, qimg.size())
                    || !qstring_to_string(qimgmask, &imgmask, qimgmask.size()))
                  {
                      sprintf(msgText, "Error: cannot convert QString to char *\n");
                      msgBox.setText(msgText);
                      msgBox.exec();
                      fprintf(stderr, "Error: cannot conver QString to char *\n");
                      return;
                  }
                  if (qimg.isEmpty())
                  {
                    if (img)
                    {
                      free(img);
                    }
                    img = NULL;
                  }
                  if (qimgmask.isEmpty())
                  {
                    if (imgmask)
                    {
                      free(imgmask);
                    }
                    imgmask = NULL;
                  }
                  int destination_dir_length = strlen(destination_dir);
                  bool add_slash = false;
                  bool rewrite = false;
                  if ((destination_dir[destination_dir_length - 1] != '/') && (destination_dir[destination_dir_length - 1] != '\\'))
                  {
                      add_slash = true;
                  }

                      if (!qimg.isEmpty())
                      {
                          if (!fileExists(qimg))
                          {
                              free(img);
                              img = NULL;
                          }
                          else
                          {
                              rewriteQuestion(window, ".surface.", name, img, &rewrite);
                              if (rewrite)
                              {
                                  free(img);
                                  img = NULL;
                              }
                          }
                      }
                      if (img == NULL)
                      {
                          if (!(img = (char *)malloc(PATH_LENGTH)))
                          {
                              sprintf(msgText, "Error while processing \"%s\": cannot allocate memory for imgpath\n", name);
                              msgBox.setText(msgText);
                              msgBox.exec();
                              fprintf(stderr, "Error while processing \"%s\": cannot allocate memory for imgpath\n", name);
                              return;
                          }
                          strcpy(img, destination_dir);
                          if (add_slash)
                          {
                              strcat(img, "/");
                          }
                          strcat(img, name);
                          strcat(img, ".surface.tiff");
                          strcat(img, "\0");
                          fprintf(stderr, "CHECK img: %s\n", img);
                      }
                      else
                      {
                          free(img);
                          img = NULL;
                      }

                      if (imgmask != NULL)
                      {
                          if (!fileExists(qimgmask))
                          {
                              free(imgmask);
                              imgmask = NULL;
                          }
                          else
                          {
                              rewriteQuestion(window, ".mask.", name, imgmask, &rewrite);
                              if (rewrite)
                              {
                                  free(imgmask);
                                  imgmask = NULL;
                              }
                          }
                      }
                      if (imgmask == NULL)
                      {
                          if (!(imgmask = (char *)malloc(PATH_LENGTH)))
                          {
                              sprintf(msgText, "Error while processing \"%s\": cannot allocate memory for imgmaskpath\n", name);
                              msgBox.setText(msgText);
                              msgBox.exec();
                              fprintf(stderr, "Error while processing \"%s\": cannot allocate memory for imgmaskpath\n", name);
                              return;
                          }
                          strcpy(imgmask, destination_dir);
                          if (add_slash)
                          {
                              strcat(imgmask, "/");
                          }
                          strcat(imgmask, name);
                          strcat(imgmask, ".mask.tiff");
                          strcat(imgmask, "\0");
                          fprintf(stderr, "CHECK imgmask: %s\n", imgmask);
                      }
                      else
                      {
                          free(imgmask);
                          imgmask = NULL;
                      }

                  progress->setValue(5);
                  if ((imgmask != NULL)||(img != NULL))
                  if (!process(path1, pathh, pathq, img, 0, imgmask, 0, progress))
                  {
                      sprintf(msgText, "Error while processing \"%s\"\n", name);
                      msgBox.setText(msgText);
                      msgBox.exec();
                      fprintf(stderr, "Error while processing \"%s\"\n", name);
                  }
                  else
                  {
                      qimg = QString(img);
                      if (!addFile(qimg, mDataFile))
                      {
                          sprintf(msgText, "Error: cannot add file to the database:\n\'%s\'\n", img);
                          msgBox.setText(msgText);
                          msgBox.exec();
                          fprintf(stderr, "Error: cannot add file to database:\n\'%s\'\n", img);
                      }
                      qimgmask = QString(imgmask);
                      if (!addFile(qimgmask, mDataFile))
                      {
                          sprintf(msgText, "Error: cannot add file to the database:\n\'%s\'\n", imgmask);
                          msgBox.setText(msgText);
                          msgBox.exec();
                          fprintf(stderr, "Error: cannot add file to database:\n\'%s\'\n", imgmask);
                      }
                  }
                  progress->setValue(100);

            //      QFileInfo myFileInfo(im);
            //      QString myBaseNameQString = myFileInfo.baseName();
            //      mIface->addRasterLayer(im, myBaseNameQString);
            //      QFileInfo myFileInfo2(imm);
            //      myBaseNameQString = myFileInfo2.baseName();
            //      mIface->addRasterLayer(imm, myBaseNameQString);

            }
        }
    }
    else
    {
        sprintf(msgText, "To process data you should select the necessary names from database");
                  msgBox.setText(msgText);
                  msgBox.exec();
        return;
    }
    if (destination_dir)
    {
        free(destination_dir);
        destination_dir = NULL;
    }
    if (name)
    {
         free(name);
         name = NULL;
    }
    sprintf(msgText, "Done\n");
    msgBox.setText(msgText);
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
  QMessageBox msgBox;
  char msgText[MAX_MESSAGE_LENGTH];
  char *pathstring;
  int i;
  if (dialog->exec())
  {
    mMainWidget->destinationDirLineEdit->setText(dialog->selectedFile());
  }
  return;
}

void QgsCloudRecognitionPlugin::createDB()
{
#if 0
    QMessageBox msgBox;
    char msgText[MAX_MESSAGE_LENGTH];
    char *root_user;
    char *root_password;
    if ((!qstring_to_string(user_edit->text(), &root_user, user_edit->text().size())) ||
        (!qstring_to_string(password_edit->text(), &root_password, password_edit->text().size())))
    {
        sprintf(msgText, "Error: cannot convert QString to char*\n");
        msgBox.setText(msgText);
        msgBox.exec();
        fprintf(stderr, "Error: cannot convert QString to char*\n");
        return;
    }
    if (create_database_if_not_exists(root_user, root_password))
    {
        sprintf(msgText, "Error: cannot create database\n");
        msgBox.setText(msgText);
        msgBox.exec();
        fprintf(stderr, "Error: cannot create database\n");
        return;
    }
    if (root_user)
    {
        free(root_user);
    }
    if (root_password)
    {
        free(root_password);
    }
    if (modis_database_table_exists())
    {
        db_status->setText("Database is OK\n"
                           "Database \'modisdatabase\' and user \'modisuser\'@\'localhost\' with password \'modispassword\' have been created"
        );
        sprintf(msgText, "Database created successfully\n");
        msgBox.setText(msgText);
        msgBox.exec();
    }
#endif
    return;
}

void QgsCloudRecognitionPlugin::openImages(QListWidgetItem *item)
{
    QMessageBox msgBox;
    char msgText[MAX_MESSAGE_LENGTH];
    char *name;
    char *path1, *pathh, *pathq, *img, *imgmask;
    /*if (!mysql_is_available())
    {
        sprintf(msgText, "MySQL is not available\nPlease, check that you have installed mysql-client and mysql-server");
        msgBox.setText(msgText);
        msgBox.exec();
        fprintf(stderr, "Error: cannot connect to mysql\n");
        return;
    }
    if (!modis_database_table_exists())
    {
        sprintf(msgText, "Database or modis_user doesn't exist\nPlease, go to \"DB config...\" and create them");
        msgBox.setText(msgText);
        msgBox.exec();
        fprintf(stderr, "Error: modis_database or modis_user doesn't exist\n");
        return;
    }
    if (!qstring_to_string(item->text(), &name, item->text().size()))
    {
        sprintf(msgText, "Error: cannot convert QString to char *\n");
        msgBox.setText(msgText);
        msgBox.exec();
        fprintf(stderr, "Error: cannot conver QString to char *\n");
        return;
    }*/
    QString qpath1, qpathh, qpathq, qimg, qimgmask;
    QString qname = item->text();
    if (!getPathsByName(qname, qpath1, qpathh, qpathq, qimg, qimgmask, mDataFile))
    {
        return;
    }
    if (qpath1.isEmpty() || qpathh.isEmpty() || qpathq.isEmpty())
    {
        fprintf(stderr, "CHECK ERROR: path not found\n");
        //return;
    }
    /*if (selectpaths(name, &path1, &pathh, &pathq, &img, &imgmask))
    {

        sprintf(msgText, "Some files are not in the database:\n"
                          "MOD021KM path = %s%s\nMOD02HKM path = %s%s\nMOD02QKM path = %s%s",
                          path1, path1 == NULL ? ": empty record" : "",
                          pathh, pathh == NULL ? ": empty record" : "",
                          pathq, pathq == NULL ? ": empty record" : ""
                          );
        msgBox.setText(msgText);
        msgBox.exec();
        fprintf(stderr, "Error: some files are not in the database\n");
    }*/
    if (qimg.isEmpty() || qimgmask.isEmpty())
    {
        fprintf(stderr, "CHECK ERROR: img not found\n");
        return;
    }

    if (fileExists(qimg) && fileExists(qimgmask))
    {
             QFileInfo myFileInfo(qimg);
             QString myBaseNameQString = myFileInfo.completeBaseName();
             mIface->addRasterLayer(qimg, myBaseNameQString);
             QFileInfo myFileInfo2(qimgmask);
             myBaseNameQString = myFileInfo2.completeBaseName();
             mIface->addRasterLayer(qimgmask, myBaseNameQString);
    }
    /*if ((img != NULL) && (imgmask != NULL))
    {
        if (!file_or_directory_exists(img))
        {
            sprintf(msgText, "File doesn't exist:\n %s", img);
            msgBox.setText(msgText);
            msgBox.exec();
            fprintf(stderr, "Error: some files are not in the database\n");
        }
        else
        {
             QFileInfo myFileInfo(img);
             QString myBaseNameQString = myFileInfo.completeBaseName();
             mIface->addRasterLayer(img, myBaseNameQString);
             if (!file_or_directory_exists(imgmask))
              {
                  sprintf(msgText, "File doesn't exist:\n %s", imgmask);
                  msgBox.setText(msgText);
                  msgBox.exec();
                  fprintf(stderr, "Error: some files are not in the database\n");
              }
              else
              {
                  QFileInfo myFileInfo2(imgmask);
                  myBaseNameQString = myFileInfo2.completeBaseName();
                  mIface->addRasterLayer(imgmask, myBaseNameQString);
              }
        }
    }
    else
    {
        sprintf(msgText, "Some files are not in the database:\n"
                          "image path = %s%s\nimage mask path = %s%s",
                          img, img == NULL ? ": empty record" : "",
                          imgmask, imgmask == NULL ? ": empty record" : ""
                          );
        msgBox.setText(msgText);
        msgBox.exec();
        fprintf(stderr, "Error: some files are not in the database\n");
    }*/
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
    //dialog->setNameFilter(trUtf8("hdf MOD files (MOD*.hdf), .tiff result files (*.tiff)"));
    openDialog->setNameFilterDetailsVisible(true);
    openDialog->setNameFilters(filters);
    QStringList fileNames;
    QMessageBox msgBox;
    char msgText[MAX_MESSAGE_LENGTH];
    //char *pathstring = NULL;
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

void QgsCloudRecognitionPlugin::clearlist()
{
    mMainWidget->selectedDataListWidget->clear();
    return;
}

void QgsCloudRecognitionPlugin::dbConfigWindow()
{
#if 0
    QMessageBox msgBox;
    char msgText[MAX_MESSAGE_LENGTH];
    if (!mysql_is_available())
    {
        sprintf(msgText, "MySQL is not available\nPlease, check that you have installed mysql-client and mysql-server");
        msgBox.setText(msgText);
        msgBox.exec();
        fprintf(stderr, "Error: cannot connect to mysql\n");
        return;
    }
    QWidget *config_window = new QWidget();
    config_window->setAttribute(Qt::WA_DeleteOnClose);
    QRect rect = QApplication::desktop()->geometry();

    QGridLayout *layout = new QGridLayout();

    db_status = new QLabel();
    QLabel *user_label = new QLabel();
    QLabel *password_label = new QLabel();

    user_edit = new QLineEdit();
    password_edit = new QLineEdit();

    QPushButton *create_button = new QPushButton();
    QPushButton *cancel_button = new QPushButton();

    password_edit->setEchoMode(QLineEdit::Password);
    db_status->setWordWrap(true);
    if (!modis_database_table_exists())
    {
        db_status->setText("Database doesn't exist, create it by inserting login and password of mysql user with create privileges\n"
                           "Database \'modisdatabase\' and user \'modisuser\'@\'localhost\' with password \'modispassword\' will be created"
        );
        user_edit->setEnabled(true);
        password_edit->setEnabled(true);
    }
    else
    {
        db_status->setText("Database is OK\n"
                           "Database \'modisdatabase\' and user \'modisuser\'@\'localhost\' with password \'modispassword\' are created"
        );
        user_edit->setEnabled(false);
        password_edit->setEnabled(false);
    }

    user_label->setText("User:");
    password_label->setText("Password:");
    create_button->setText("Create");
    cancel_button->setText("Close");

    config_window->resize(300, 200);
    config_window->move (rect.center() - config_window->rect().center());
    config_window->show();
    config_window->setWindowTitle(QApplication::translate("db config", "Database configuration"));

    layout->addWidget(db_status, 0, 0, 1, 2);
    layout->addWidget(user_label, 2, 0);
    layout->addWidget(user_edit, 2, 1);
    layout->addWidget(password_label, 3, 0);
    layout->addWidget(password_edit, 3, 1);
    layout->addWidget(create_button, 4, 0);
    layout->addWidget(cancel_button, 4, 1);

    config_window->setLayout(layout);

    QObject::connect(create_button, SIGNAL(clicked()), this, SLOT(createDB()));
    QObject::connect(cancel_button, SIGNAL(clicked()), config_window, SLOT(close()));
#endif
    return;
}

void QgsCloudRecognitionPlugin::showWindow()
{
    QWidget *window = new QWidget();
    mMainWidget = new Ui::QgsCloudRecognitionForm();
    mMainWidget->setupUi(window);
//    mMainWidget.progressBar->setTextVisible(false);

    window->resize(600, 400);
    QRect rect = QApplication::desktop()->geometry();
    window->move (rect.center() - window->rect().center());
    window->show();
    window->setWindowTitle(QApplication::translate("crplugin", "Cloud recognition"));
    window->activateWindow();

    QString path;
    QObject::connect(mMainWidget->searchButton, SIGNAL(clicked()), this, SLOT(searchInData()));
    QObject::connect(mMainWidget->searchLineEdit, SIGNAL(returnPressed()), mMainWidget->searchButton, SIGNAL(clicked()));
    QObject::connect(mMainWidget->addButton, SIGNAL(clicked()), this, SLOT(addGroupName()));
    QObject::connect(mMainWidget->addAllButton, SIGNAL(clicked()), this, SLOT(addAll()));
    QObject::connect(mMainWidget->deleteButton, SIGNAL(clicked()), this, SLOT(deletefromlist()));
    QObject::connect(mMainWidget->clearButton, SIGNAL(clicked()), this, SLOT(clearlist()));

    QObject::connect(mMainWidget->addFilesButton, SIGNAL(clicked()), this, SLOT(addFilesToDataList()));
    //QObject::connect(dbconfig, SIGNAL(clicked()), this, SLOT(dbConfigWindow()));

    QObject::connect(mMainWidget->selectedDataListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
           this,SLOT(openImages(QListWidgetItem*)));
    QObject::connect(mMainWidget->searchResultsListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
           this,SLOT(openImages(QListWidgetItem*)));

    QObject::connect(mMainWidget->browseButton, SIGNAL(clicked()), this, SLOT(browseDestination()));
    QObject::connect(mMainWidget->runButton, SIGNAL(clicked()), this, SLOT(Process()));
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
