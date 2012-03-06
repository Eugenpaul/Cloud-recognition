#include "../inc/addition.h"

#define MAX_PATH_LENGTH 255
#define DEFAULT_DIR_MODE 0777

/* some supporting functions */

bool qstring_to_string(QString qstring, char **result, int length)
{
    QByteArray tempstr;
    tempstr = qstring.toLatin1();
    if (!(*result = (char *)malloc(length + 1)))
    {
        return false;
    }
    strncpy(*result, tempstr.data(), length);
    result[0][length] = '\0';
    fprintf(stderr, "CHECK result of qstring_to_string: %s\n", *result);
    return true;
}

bool file_or_directory_exists(char *file_or_directory)
{
    struct stat st;
    if(stat(file_or_directory,&st) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
    return true;
}

bool force_create_directory(char *directory)
{
    int i;
    int path_len = strlen(directory);
    char *temp_path = NULL;
    bool create_mode = false;
    if (!(temp_path = (char *)malloc(MAX_PATH_LENGTH)))
    {
        return false;
    }
    for (i = 0; i < path_len; i++)
    {
        if (((directory[i] == '/') || (directory[i] == '\\')) && (i != 0) && (i != 1))
        {
            if (!create_mode)
            {
                strncpy(temp_path, directory, i);
                temp_path[i] = '\0';
                if (!file_or_directory_exists(temp_path))
                {
                    create_mode = true;
                    printf("CHECK trying to create path: %s\n", temp_path);
                    if (mkdir(temp_path, DEFAULT_DIR_MODE))
                    {
                        return false;
                    }
                }
            }
            else
            {
                strncpy(temp_path, directory, i);
                temp_path[i] = '\0';
                printf("CHECK trying to create path: %s\n", temp_path);
                if (mkdir(temp_path, DEFAULT_DIR_MODE))
                {
                    return false;
                }
            }
        }
    }
    if (temp_path)
    {
        free(temp_path);
    }
    if (!file_or_directory_exists(directory))
    {
        if (mkdir(directory, DEFAULT_DIR_MODE))
        {
            return false;
        }
    }
    return true;
}

