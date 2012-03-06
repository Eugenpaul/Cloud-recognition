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
  hdfbase = new QListWidget();
  searchedit = new QLineEdit();
  progressbar = new QProgressBar();
  progressbar->setMinimum(0);
  progressbar->setMaximum(100);
  progressbar->setTextVisible(false);

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
}

void QgsCloudRecognitionPlugin::rewriteQuestion(QWidget *w, char *type, char *name, char *path, bool *rewrite)
{
    //QMessageBox::StandardButton reply;
    int reply;
    char question[2048];
    sprintf(question, "File of type %s for %s already exists:\n \'%s\'\n Do you want to create a new one in the folder specified and replace it the database", type, name, path);
    QMessageBox msgBox;

    msgBox.addButton(QMessageBox::Yes);

    msgBox.addButton(QMessageBox::No);

    msgBox.setText(question);

    reply = msgBox.exec();
    /*reply = QMessageBox::question(w, tr("QMessageBox::question()"),
                                    tr(question),
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);*/
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
    QString qdestination_dir = imgedit->text();
    char *destination_dir = NULL;
    QMessageBox msgBox;
    char msgText[MAX_MESSAGE_LENGTH];
    ProgressViewer *progress = new ProgressViewer();
    QObject::connect(progress, SIGNAL(valueChanged(int)),
                     progressbar, SLOT(setValue(int)));
    char *path1 = NULL, *pathh = NULL, *pathq = NULL, *img = NULL, *imgmask = NULL;
    int i;
    char *name = NULL;
    if (imgedit->text() == "")
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
    }
    if (hdfedit->count() != 0)
    {
        for (i = 0; i < hdfedit->count(); i++)
        {
            //QString item_text("<FONT COLOR=blue>" + hdfedit->item(i)->text() + "</FONT>");
            hdfedit->item(i)->setText(hdfedit->item(i)->text());
            if (name)
            {
                free(name);
                name = NULL;
            }
            if (!qstring_to_string(hdfedit->item(i)->text(), &name, hdfedit->item(i)->text().size()))
            {
                sprintf(msgText, "Error: cannot convert QString to char *\n");
                msgBox.setText(msgText);
                msgBox.exec();
                fprintf(stderr, "Error: cannot conver QString to char *\n");
                return;
            }

            if (selectpaths(name, &path1, &pathh, &pathq, &img, &imgmask))
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
            }
            else
            {
                  int destination_dir_length = strlen(destination_dir);
                  bool add_slash = false;
                  bool rewrite = false;
                  if ((destination_dir[destination_dir_length - 1] != '/') && (destination_dir[destination_dir_length - 1] != '\\'))
                  {
                      add_slash = true;
                  }

                      if (img != NULL)
                      {
                          if (!file_or_directory_exists(img))
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
                          strcat(img, ".surface.png");
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
                          if (!file_or_directory_exists(imgmask))
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
                          strcat(imgmask, ".mask.png");
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
                      if (addfiletodatabase(img))
                      {
                          sprintf(msgText, "Error: cannot add file to the database:\n\'%s\'\n", img);
                          msgBox.setText(msgText);
                          msgBox.exec();
                          fprintf(stderr, "Error: cannot add file to database:\n\'%s\'\n", img);
                      }
                      if (addfiletodatabase(imgmask))
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
    if (hdfbase->count())
    {
        if (hdfbase->currentItem())
        {
            for (i = 0; i < hdfedit->count(); i++)
            {
                if (hdfedit->item(i)->text() == hdfbase->currentItem()->text())
                {
                    same_found = true;
                }
            }
            if (!same_found)
            {
                hdfedit->addItem(hdfbase->currentItem()->text());
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
    for (i = 0; i < hdfbase->count(); i++)
    {
        for (j = 0; j < hdfedit->count(); j++)
        {
            if (hdfedit->item(j)->text() == hdfbase->item(i)->text())
            {
                same_found = true;
            }
        }
        if (!same_found)
        {
            hdfedit->addItem(hdfbase->item(i)->text());
        }
    }
    return;
}

void QgsCloudRecognitionPlugin::saveimage()
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
        imgedit->setText(dialog->selectedFile());
    }
    return;
}

void QgsCloudRecognitionPlugin::createDB()
{
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
    return;
}

void QgsCloudRecognitionPlugin::openImages(QListWidgetItem *item)
{
    QMessageBox msgBox;
    char msgText[MAX_MESSAGE_LENGTH];
    char *name;
    char *path1, *pathh, *pathq, *img, *imgmask;
    if (!mysql_is_available())
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
    }

    if (selectpaths(name, &path1, &pathh, &pathq, &img, &imgmask))
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
    }
    if ((img != NULL) && (imgmask != NULL))
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
    }
}

void QgsCloudRecognitionPlugin::selectfromdb()
{
    char *searchstring = NULL;
    QMessageBox msgBox;
    char msgText[MAX_MESSAGE_LENGTH];
    char **result = NULL;
    int n = 0;
    int i = 0;
    QString string;
    if (!mysql_is_available())
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
    if (!qstring_to_string(searchedit->text(), &searchstring, searchedit->text().size()))
    {
        sprintf(msgText, "Error: cannot convert QString to char*\n");
        msgBox.setText(msgText);
        msgBox.exec();
        fprintf(stderr, "Error: cannot convert QString to char*\n");
        return;
    }

    fprintf(stderr, "CHECK searchstring: \"%s\"\n", searchstring);
    if(selectfrombase(searchstring, &result, &n))
    {
        return;
    }
    else
    {
        printf("check before clear\n");
        hdfbase->clear();
        printf("check after clear, n = %d\n", n);
        for (i = 0; i < n; i++)
        {
            printf("check result: %s\n", result[i]);

            //string = QString(QLatin1String(result[i]));
            hdfbase->addItem(QLatin1String(result[i]));
        }
        if (result)
        {
            for (i = 0; i < n; i++)
            {
                if (result[i])
                {
                    fprintf(stderr, "CHECK free: %d\n", i);
                    free(result[i]);
                    fprintf(stderr, "CHECK after free %d\n" ,i);
                }
            }
            fprintf(stderr, "CHECK free result\n");
            free(result);
            fprintf(stderr, "CHECK after free result\n");
        }
    }
    return;
}

void QgsCloudRecognitionPlugin::addtodatabase()
{
    QFileDialog *dialog = new QFileDialog();
    dialog->setFileMode(QFileDialog::ExistingFiles);
    QStringList filters = QStringList();
    filters << "any suitable files (MOD*.hdf *.surface.png *.mask.png)"
            << "hdf MOD files (MOD*.hdf)"
            << "result images (*.surface.png *.mask.png)"
            << "any files (*)";
    //dialog->setNameFilter(trUtf8("hdf MOD files (MOD*.hdf), png result files (*.png)"));

    dialog->setNameFilterDetailsVisible(true);
    dialog->setNameFilters(filters);
    QStringList file_names;
    QMessageBox msgBox;
    char msgText[MAX_MESSAGE_LENGTH];
    char *pathstring = NULL;
    int i;
    if (!mysql_is_available())
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
    if (dialog->exec())
    {
        file_names = dialog->selectedFiles();
    }
    if (!file_names.isEmpty())
    {
        int successfully_added_num = 0;
        for (i = 0; i < file_names.size(); i++)
        {
            if (!qstring_to_string(file_names[i], &pathstring, file_names[i].size()))
            {
                sprintf(msgText, "Error: cannot convert QString to char*\n");
                msgBox.setText(msgText);
                msgBox.exec();
                fprintf(stderr, "Error: cannot convert QString to char*\n");
                return;
            }
            if (addfiletodatabase(pathstring))
            {
                sprintf(msgText, "Error: cannot add file to the database:\n\'%s\'\n", pathstring);
                msgBox.setText(msgText);
                msgBox.exec();
                fprintf(stderr, "Error: cannot add file to database:\n\'%s\'\n", pathstring);
            }
            else
            {
                successfully_added_num ++;
            }
            free(pathstring);
            pathstring = NULL;
        }
        sprintf(msgText, "%d file%s ha%s been added\n", successfully_added_num,
                                                        successfully_added_num != 1? "s":"",
                                                        successfully_added_num != 1? "ve":"s"
               );
        msgBox.setText(msgText);
        msgBox.exec();
    }
    return;
}


void QgsCloudRecognitionPlugin::deletefromlist()
{
    if (hdfedit->currentItem())
    {
        delete hdfedit->currentItem();
    }
    return;
}

void QgsCloudRecognitionPlugin::clearlist()
{
    hdfedit->clear();
    return;
}

void QgsCloudRecognitionPlugin::dbConfigWindow()
{
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

    return;
}

void QgsCloudRecognitionPlugin::showWindow()
{
    QWidget *window = new QWidget();
    window->resize(600, 400);
    QRect rect = QApplication::desktop()->geometry();
    window->move (rect.center() - window->rect().center());
    window->show();
    window->setWindowTitle(QApplication::translate("crplugin", "Cloud recognition"));
    window->activateWindow();
    QLabel *hdflabel = new QLabel;
    QLabel *searchresultlabel = new QLabel;
    QLabel *imglabel = new QLabel;
    QGridLayout *layout = new QGridLayout;
    QGridLayout *vertical_layout = new QGridLayout;

    QPushButton *addbutton = new QPushButton;
    QPushButton *delbutton = new QPushButton;
    QPushButton *addallbutton = new QPushButton;
    QPushButton *clearbutton = new QPushButton;
    QPushButton *setbutton = new QPushButton;
    QPushButton *procbutton = new QPushButton;
    QPushButton *cancelbutton = new QPushButton;

    QPushButton *searchbutton = new QPushButton;
    QPushButton *dbconfig = new QPushButton;
    QPushButton *addtodb = new QPushButton;
    QPushButton *add = new QPushButton;

    imgedit->setText(getenv("HOME"));

    searchbutton->setText("Search");
    addtodb->setText("Add to DB...");
    dbconfig->setText("DB config...");
    add->setText("Add");

    addbutton->setText("Add >");
    delbutton->setText("Del <");
    addallbutton->setText("Add all >>");
    clearbutton->setText("Clear <<");
    setbutton->setText("Browse...");
    procbutton->setText("Run");
    hdflabel->setText("Data to process");
    searchresultlabel->setText("Search result");
    imglabel->setText("Destination folder:");
    cancelbutton->setText("Cancel");

    layout->addWidget(searchbutton, 0, 0, 1, 1);
    layout->addWidget(searchedit, 0, 1, 1, 2);
    layout->addWidget(addtodb, 0, 3, 1, 1);
    layout->addWidget(dbconfig, 0, 4, 1, 1);

    layout->addWidget(searchresultlabel, 1, 0, 1, 1);
    layout->addWidget(hdflabel, 1, 3, 1, 1);

    layout->addWidget(hdfbase, 2, 0, 1, 2);
    layout->addWidget(hdfedit, 2, 3, 1, 2);

    vertical_layout->addWidget(addbutton, 0, 0);
    vertical_layout->addWidget(addallbutton, 1, 0);
    vertical_layout->addWidget(delbutton, 3, 0);
    vertical_layout->addWidget(clearbutton, 4, 0);

    layout->addLayout(vertical_layout, 2, 2);


    layout->addWidget(imglabel, 3, 0, 1, 1);
    layout->addWidget(imgedit, 3, 1, 1, 3);

    layout->addWidget(setbutton, 3, 4, 1, 1);
    layout->addWidget(procbutton, 4, 0, 1, 3);
    layout->addWidget(cancelbutton, 4, 3, 1, 2);

    layout->addWidget(progressbar, 5, 0, 1, 5);

    window->setLayout(layout);
    QString path;
    QObject::connect(searchbutton, SIGNAL(clicked()), this, SLOT(selectfromdb()));
    QObject::connect(searchedit, SIGNAL(returnPressed()), searchbutton, SIGNAL(clicked()));
    QObject::connect(addbutton, SIGNAL(clicked()), this, SLOT(addGroupName()));
    QObject::connect(addallbutton, SIGNAL(clicked()), this, SLOT(addAll()));
    QObject::connect(delbutton, SIGNAL(clicked()), this, SLOT(deletefromlist()));
    QObject::connect(clearbutton, SIGNAL(clicked()), this, SLOT(clearlist()));

    QObject::connect(addtodb, SIGNAL(clicked()), this, SLOT(addtodatabase()));
    QObject::connect(dbconfig, SIGNAL(clicked()), this, SLOT(dbConfigWindow()));

    QObject::connect(hdfedit,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
           this,SLOT(openImages(QListWidgetItem*)));
    QObject::connect(hdfbase,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
           this,SLOT(openImages(QListWidgetItem*)));

    QObject::connect(setbutton, SIGNAL(clicked()), this, SLOT(saveimage()));
    QObject::connect(procbutton, SIGNAL(clicked()), this, SLOT(Process()));
    QObject::connect(cancelbutton, SIGNAL(clicked()), window, SLOT(close()));
    return;
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
