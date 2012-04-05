/********************************************************************************
** Form generated from reading UI file 'qgscloudrecognitionbase.ui'
**
** Created: Sun Apr 1 10:17:01 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QGSCLOUDRECOGNITIONBASE_H
#define UI_QGSCLOUDRECOGNITIONBASE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QgsCloudRecognitionForm
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *addButton;
    QPushButton *addAllButton;
    QPushButton *deleteButton;
    QPushButton *clearButton;
    QPushButton *browseButton;
    QProgressBar *progressBar;
    QPushButton *searchButton;
    QLineEdit *searchLineEdit;
    QLineEdit *destinationDirLineEdit;
    QLabel *destinationDirLabel;
    QPushButton *addFilesButton;
    QPushButton *closeButton;
    QPushButton *runButton;
    QLabel *searchResultsLabel;
    QLabel *selectedDataLabel;
    QListWidget *selectedDataListWidget;
    QListWidget *searchResultsListWidget;

    void setupUi(QWidget *QgsCloudRecognitionForm)
    {
        if (QgsCloudRecognitionForm->objectName().isEmpty())
            QgsCloudRecognitionForm->setObjectName(QString::fromUtf8("QgsCloudRecognitionForm"));
        QgsCloudRecognitionForm->resize(585, 404);
        gridLayout_2 = new QGridLayout(QgsCloudRecognitionForm);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        addButton = new QPushButton(QgsCloudRecognitionForm);
        addButton->setObjectName(QString::fromUtf8("addButton"));

        verticalLayout->addWidget(addButton);

        addAllButton = new QPushButton(QgsCloudRecognitionForm);
        addAllButton->setObjectName(QString::fromUtf8("addAllButton"));

        verticalLayout->addWidget(addAllButton);

        deleteButton = new QPushButton(QgsCloudRecognitionForm);
        deleteButton->setObjectName(QString::fromUtf8("deleteButton"));

        verticalLayout->addWidget(deleteButton);

        clearButton = new QPushButton(QgsCloudRecognitionForm);
        clearButton->setObjectName(QString::fromUtf8("clearButton"));

        verticalLayout->addWidget(clearButton);


        gridLayout->addLayout(verticalLayout, 3, 2, 1, 1);

        browseButton = new QPushButton(QgsCloudRecognitionForm);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));

        gridLayout->addWidget(browseButton, 5, 4, 1, 1);

        progressBar = new QProgressBar(QgsCloudRecognitionForm);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(0);
        progressBar->setTextVisible(false);

        gridLayout->addWidget(progressBar, 7, 0, 1, 5);

        searchButton = new QPushButton(QgsCloudRecognitionForm);
        searchButton->setObjectName(QString::fromUtf8("searchButton"));

        gridLayout->addWidget(searchButton, 0, 0, 1, 1);

        searchLineEdit = new QLineEdit(QgsCloudRecognitionForm);
        searchLineEdit->setObjectName(QString::fromUtf8("searchLineEdit"));

        gridLayout->addWidget(searchLineEdit, 0, 1, 1, 3);

        destinationDirLineEdit = new QLineEdit(QgsCloudRecognitionForm);
        destinationDirLineEdit->setObjectName(QString::fromUtf8("destinationDirLineEdit"));

        gridLayout->addWidget(destinationDirLineEdit, 5, 1, 1, 3);

        destinationDirLabel = new QLabel(QgsCloudRecognitionForm);
        destinationDirLabel->setObjectName(QString::fromUtf8("destinationDirLabel"));

        gridLayout->addWidget(destinationDirLabel, 5, 0, 1, 1);

        addFilesButton = new QPushButton(QgsCloudRecognitionForm);
        addFilesButton->setObjectName(QString::fromUtf8("addFilesButton"));

        gridLayout->addWidget(addFilesButton, 0, 4, 1, 1);

        closeButton = new QPushButton(QgsCloudRecognitionForm);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));

        gridLayout->addWidget(closeButton, 6, 3, 1, 2);

        runButton = new QPushButton(QgsCloudRecognitionForm);
        runButton->setObjectName(QString::fromUtf8("runButton"));

        gridLayout->addWidget(runButton, 6, 0, 1, 3);

        searchResultsLabel = new QLabel(QgsCloudRecognitionForm);
        searchResultsLabel->setObjectName(QString::fromUtf8("searchResultsLabel"));

        gridLayout->addWidget(searchResultsLabel, 2, 0, 1, 1);

        selectedDataLabel = new QLabel(QgsCloudRecognitionForm);
        selectedDataLabel->setObjectName(QString::fromUtf8("selectedDataLabel"));

        gridLayout->addWidget(selectedDataLabel, 2, 3, 1, 1);

        selectedDataListWidget = new QListWidget(QgsCloudRecognitionForm);
        selectedDataListWidget->setObjectName(QString::fromUtf8("selectedDataListWidget"));

        gridLayout->addWidget(selectedDataListWidget, 3, 3, 1, 2);

        searchResultsListWidget = new QListWidget(QgsCloudRecognitionForm);
        searchResultsListWidget->setObjectName(QString::fromUtf8("searchResultsListWidget"));

        gridLayout->addWidget(searchResultsListWidget, 3, 0, 1, 2);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(QgsCloudRecognitionForm);

        QMetaObject::connectSlotsByName(QgsCloudRecognitionForm);
    } // setupUi

    void retranslateUi(QWidget *QgsCloudRecognitionForm)
    {
        QgsCloudRecognitionForm->setWindowTitle(QApplication::translate("QgsCloudRecognitionForm", "Form", 0, QApplication::UnicodeUTF8));
        addButton->setText(QApplication::translate("QgsCloudRecognitionForm", "Add >", 0, QApplication::UnicodeUTF8));
        addAllButton->setText(QApplication::translate("QgsCloudRecognitionForm", "Add all >>", 0, QApplication::UnicodeUTF8));
        deleteButton->setText(QApplication::translate("QgsCloudRecognitionForm", "Delete <", 0, QApplication::UnicodeUTF8));
        clearButton->setText(QApplication::translate("QgsCloudRecognitionForm", "Clear <<", 0, QApplication::UnicodeUTF8));
        browseButton->setText(QApplication::translate("QgsCloudRecognitionForm", "Browse...", 0, QApplication::UnicodeUTF8));
        searchButton->setText(QApplication::translate("QgsCloudRecognitionForm", "Search", 0, QApplication::UnicodeUTF8));
        destinationDirLabel->setText(QApplication::translate("QgsCloudRecognitionForm", "Destination folder", 0, QApplication::UnicodeUTF8));
        addFilesButton->setText(QApplication::translate("QgsCloudRecognitionForm", "Add to list...", 0, QApplication::UnicodeUTF8));
        closeButton->setText(QApplication::translate("QgsCloudRecognitionForm", "Close", 0, QApplication::UnicodeUTF8));
        runButton->setText(QApplication::translate("QgsCloudRecognitionForm", "Run", 0, QApplication::UnicodeUTF8));
        searchResultsLabel->setText(QApplication::translate("QgsCloudRecognitionForm", "Available data", 0, QApplication::UnicodeUTF8));
        selectedDataLabel->setText(QApplication::translate("QgsCloudRecognitionForm", "Selected data", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QgsCloudRecognitionForm: public Ui_QgsCloudRecognitionForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QGSCLOUDRECOGNITIONBASE_H
