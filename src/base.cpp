#include "../inc/base.h"

#define NAME_INDEX 1
#define PATH1_INDEX 2
#define PATHH_INDEX 3
#define PATHQ_INDEX 4
#define IMG_INDEX 5
#define IMGMASK_INDEX 6

#define NAME_LENGTH 128
#define SQL_QUERY_LENGTH 1024

char *server = "localhost";
char *modis_user = "modisuser";
char *modis_user_password = "modispassword";
char *modis_database = "modisdatabase";
char *modis_table = "modistable";

bool mysql_is_available()
{
    MYSQL *conn;
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return false;
    }
    mysql_close(conn);
    return true;
}

bool modis_database_table_exists()
{
    MYSQL *conn;
    char mysql_string[SQL_QUERY_LENGTH];
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return false;
    }
    if (mysql_real_connect(conn, server, modis_user, modis_user_password, modis_database, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        mysql_close(conn);
        return false;
    }
    sprintf(mysql_string, "select * from %s", modis_table);
    if (mysql_query(conn, mysql_string))
    {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    mysql_close(conn);
    return true;
}

int create_database_if_not_exists(char *root_user, char *root_password)
{
    MYSQL *conn;
    char createstring[SQL_QUERY_LENGTH];

    conn = mysql_init(NULL);

    if (conn == NULL)
    {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return 1;
    }

    if (mysql_real_connect(conn, server, root_user, root_password, NULL, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    sprintf(createstring, "create database if not exists %s", modis_database);
    printf("%s\n", createstring);
    if (mysql_query(conn, createstring))
    {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    /*sprintf(createstring, "create user \'%s\'@\'%s\' identified by \'%s\'", modis_user, server, modis_user_password);
    if (mysql_query(conn, createstring))
    {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        mysql_close(conn);
        return 1;
    }*/
    sprintf(createstring, "create table if not exists %s.%s (id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(100),"
                          "path1 VARCHAR(%d), pathh VARCHAR(%d), pathq VARCHAR(%d), imgpath VARCHAR(%d), maskpath VARCHAR(%d))",
                          modis_database, modis_table, PATH_LENGTH, PATH_LENGTH, PATH_LENGTH, PATH_LENGTH, PATH_LENGTH, PATH_LENGTH);

    printf("%s\n", createstring);
    if (mysql_query(conn, createstring))
    {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    sprintf(createstring, "grant usage on %s.* to \'%s\'@\'%s\' identified by \'%s\'", modis_database, modis_user, server, modis_user_password);
    printf("%s\n", createstring);
    if (mysql_query(conn, createstring))
    {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    sprintf(createstring, "grant all privileges on %s.* to \'%s\'@\'%s\'", modis_database, modis_user, server);
    printf("%s\n", createstring);
    if (mysql_query(conn, createstring))
    {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    mysql_close(conn);

    return 0;
}

int selectpaths(char *name, char **path1, char **pathh, char **pathq, char **img, char **imgmask)
{
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  conn = mysql_init(NULL);
  char *selectstring = (char *)calloc(2048, 1);

    // Connect to database
    if(!mysql_real_connect(conn, server, modis_user, modis_user_password, modis_database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_free_result(res);
        mysql_close(conn);
	return 1;
    }

    // sql query
    sprintf(selectstring, "select * from %s where name=\'%s\'\0", modis_table, name);
    printf("%s\n", selectstring);
    if(mysql_query(conn, selectstring))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
	free(selectstring);
        mysql_free_result(res);
        mysql_close(conn);
	return 1;
    }


    res = mysql_use_result(conn);
    if ((row = mysql_fetch_row(res)) == NULL)
    {
      mysql_free_result(res);
      mysql_close(conn);
      return 1;
    }

    if (row[PATH1_INDEX])
    {
        *path1 = (char *)calloc(1, PATH_LENGTH);
        strcpy(*path1, row[PATH1_INDEX]);
    }
    else
    {
        *path1 = NULL;
    }

    if (row[PATHH_INDEX])
    {
        *pathh = (char *)calloc(1, PATH_LENGTH);
        strcpy(*pathh, row[PATHH_INDEX]);
    }
    else
    {
        *pathh = NULL;
    }

    if (row[PATHQ_INDEX])
    {
        *pathq = (char *)calloc(1, PATH_LENGTH);
        strcpy(*pathq, row[PATHQ_INDEX]);
    }
    else
    {
        *pathq = NULL;
    }

    if (row[IMG_INDEX] != NULL)
    {
      *img = (char *)calloc(1, PATH_LENGTH);
      strcpy(*img, row[IMG_INDEX]);
    }
    else
    {
      *img = NULL;
    }

    if (row[IMGMASK_INDEX] != NULL)
    {
      *imgmask = (char *)calloc(1, PATH_LENGTH);
      strcpy(*imgmask, row[IMGMASK_INDEX]);
    }
    else
    {
      *imgmask = NULL;
    }

    if ((row[PATH1_INDEX] == NULL)||(row[PATHH_INDEX] == NULL)||(row[PATHQ_INDEX] == NULL))
    {
      mysql_free_result(res);
      mysql_close(conn);
      return 1;
    }
    mysql_free_result(res);
    mysql_close(conn);
    printf("check the very finish\n");
    return 0;
}

int selectfrombase(char *what, char ***result, int *n)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    conn = mysql_init(NULL);
    int i = 0;
    char selectstring[2048];
    fprintf(stderr, "CHECK selectfrombase\n");
    // Connect to modis_database
    if(!mysql_real_connect(conn, server, modis_user, modis_user_password, modis_database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_free_result(res);
        mysql_close(conn);
        return 1;
    }

    // sql query
    sprintf(selectstring, "select * from %s where name like \'%%%s%%\'\0",modis_table, what);
    printf("%s\n", selectstring);
    if(mysql_query(conn, selectstring))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_free_result(res);
        mysql_close(conn);
	return 1;
    }


    res = mysql_store_result(conn);
    *n = mysql_num_rows(res);

    fprintf(stderr, "CHECK mysql_num_rows: %d\n", *n);

    //res = mysql_use_result(conn);

    printf("check after res = mysql_use_result(conn)\n");
    //output table name
    /*printf("Mysql Tables in mysql database: \n");
    while((row = mysql_fetch_row(res)) !=NULL)
        printf("%-3s%-30s%-30s\n", row[0], row[1], row[2]);*/

    if (*n != 0)
    {
        if (!(*result = (char **)calloc(*n, sizeof(char *))))
        {
            mysql_free_result(res);
            mysql_close(conn);
            return 1;
        }


        for (i = 0; i < *n; i++)
        {
            result[0][i] = (char *)malloc(NAME_LENGTH);
        }
        i = 1;

        if ((row = mysql_fetch_row(res)) == NULL)
        {
            mysql_free_result(res);
            mysql_close(conn);
            return 1;
        }
        else
        {
            printf("check firstcpy\n");
            strcpy(result[0][0], row[NAME_INDEX]);
        }
        while((i < *n) && ((row = mysql_fetch_row(res)) != NULL))
        {
            printf("check next: %s\n", row[NAME_INDEX]);
            strcpy(result[0][i], row[NAME_INDEX]);
            i++;
        }
        printf("check finish\n");
    }
    mysql_free_result(res);
    mysql_close(conn);
    printf("check the very finish\n");
    return 0;
}

int addfiletodatabase(char *path)
{
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  conn = mysql_init(NULL);
  char *last;
  char *prelast;
  char *slash;
  char *lastslash;
  char *dot;
  char *start;
  char *end;
  char *prefix = (char *)calloc(1,NAME_LENGTH);
  char *name = (char *)calloc(1,NAME_LENGTH);
  bool hdfmode = false;
  bool pngmode = false;
  slash = strstr(path, "/");
  while (slash != NULL)
  {
    last = slash;
    slash = strstr(last + 1, "/");
  }
  lastslash = last;
  dot = strstr(last + 1, ".");
  if (dot == NULL)
  {
      return 1;
  }

  strncpy(prefix, last + 1, dot - last);
  printf("check prefix = %s\n", prefix);

  start = dot + 1;
  while (dot != NULL)
  {
    prelast = last;
    last = dot;
    dot = strstr(last + 1, ".");
  }
  end = last;

  if ((start == last) || (start == prelast))
  {
      return 1;
  }
  fprintf(stderr, "CHECK extension: %s\n", last);
  if (strcmp(last, ".hdf") == 0)
  {
      hdfmode = true;
  }
  else if (strcmp(last, ".png") == 0)
  {
      pngmode = true;
  }
    char *selectstring = (char *)calloc(2048, 1);
    if (pngmode)
    {
        strncpy(name, lastslash + 1, prelast - lastslash - 1);
        strcpy(prefix, prelast);
        fprintf(stderr, "CHECK png name: %s\n", name);
        fprintf(stderr, "CHECK png postfix: %s\n", prefix);
    }
    else if (hdfmode)
    {
        strncpy(name, start, end - start);
        printf("check name = %s\n", name);
    }
    if(!mysql_real_connect(conn, server, modis_user, modis_user_password, modis_database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return 1;
    }
    sprintf(selectstring, "select * from %s where name=\'%s\'", modis_table, name);
    printf("%s\n", selectstring);
    if(mysql_query(conn, selectstring))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        free(selectstring);
        return 1;
    }

    res = mysql_use_result(conn);

    if((row = mysql_fetch_row(res)) ==NULL)
    {
        sprintf(selectstring, "insert into %s (name) values(\'%s\');\0", modis_table, name);
        printf("%s\n", selectstring);
        if(mysql_query(conn, selectstring))
        {
            fprintf(stderr, "%s\n", mysql_error(conn));
            mysql_free_result(res);
            free(selectstring);
            return 1;
        }
    }

    mysql_free_result(res);
    if (strcmp(prefix, "MOD021KM.") == 0)
    {
        sprintf(selectstring, "UPDATE %s SET path1 = \'%s\' WHERE name = \'%s\'\0", modis_table, path, name);
        printf("%s\n", selectstring);
        if(mysql_query(conn, selectstring))
        {
            fprintf(stderr, "%s\n", mysql_error(conn));
            free(selectstring);
            return 1;
        }
    }
    else if (strcmp(prefix, "MOD02HKM.") == 0)
    {
        sprintf(selectstring, "UPDATE %s SET pathh = \'%s\' WHERE name = \'%s\'\0", modis_table, path, name);
        printf("%s\n", selectstring);
        if(mysql_query(conn, selectstring))
        {
            fprintf(stderr, "%s\n", mysql_error(conn));
            free(selectstring);
            return 1;
        }
    }
    else if (strcmp(prefix, "MOD02QKM.") == 0)
    {
        sprintf(selectstring, "UPDATE %s SET pathq = \'%s\' WHERE name = \'%s\'\0", modis_table, path, name);
        printf("%s\n", selectstring);
        if(mysql_query(conn, selectstring))
        {
            fprintf(stderr, "%s\n", mysql_error(conn));
            free(selectstring);
            return 1;
        }
    }
    else if (strcmp(prefix, ".surface.png") == 0)
    {
        sprintf(selectstring, "UPDATE %s SET imgpath = \'%s\' WHERE name = \'%s\'\0", modis_table, path, name);
        printf("%s\n", selectstring);
        if(mysql_query(conn, selectstring))
        {
             fprintf(stderr, "%s\n", mysql_error(conn));
             free(selectstring);
             return 1;
        }
    }
    else if (strcmp(prefix, ".mask.png") == 0)
    {
        sprintf(selectstring, "UPDATE %s SET maskpath = \'%s\' WHERE name = \'%s\'\0", modis_table, path, name);
        printf("%s\n", selectstring);
        if(mysql_query(conn, selectstring))
        {
            fprintf(stderr, "%s\n", mysql_error(conn));
            free(selectstring);
            return 1;
        }
    }
    else
    {
        return 1;
    }
    if (selectstring)
    {
        free(selectstring);
    }
    mysql_close(conn);
    return 0;
}