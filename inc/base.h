#ifndef DATABASE_H_
#define DATABASE_H_
#include<stdio.h>
#include<mysql/mysql.h>
#include<stdlib.h>
#include<string.h>

#define PATH_LENGTH 255

bool mysql_is_available();
bool modis_database_table_exists();
int selectfrombase(char *what, char ***result, int *n);
int addfiletodatabase(char *path);
int selectpaths(char *name, char **path1, char **pathh, char **pathq, char **img, char **imgmask);
int create_database_if_not_exists(char *root_user, char *root_password);
#endif

