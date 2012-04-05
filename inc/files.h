#ifndef _FILES_H
#define _FILES_H

#include <iostream>
#include <QString>
#include <QStringRef>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QTextStream>

bool fileExists(QString &path);
bool directoryExists(QString &path);
bool createDirectory(QString &path);
bool getPathsByName(QString &name, QString &pathKM, QString &pathHKM, QString &pathQKM, QString &pathMap, QString &pathMask, QString &dataFile);
bool searchRequest(QString &searchWord, QStringList &result, QString &dataFile);
bool addFile(QString &path, QString &dataFile);

#endif