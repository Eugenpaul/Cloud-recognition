#ifndef ADDITION_H_
#define ADDITION_H_

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <QObject>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool file_or_directory_exists(char *file_or_directory);
bool force_create_directory(char *directory);
bool qstring_to_string(QString qstring, char **result, int length);

#endif