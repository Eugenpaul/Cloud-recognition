#include "interface.h"
#define MASKDEPTH 	10



using namespace std;

bool process(char *path1, char *pathh, char *pathq, char *imgpath, int rewrite_imgpath, char *imgmaskpath, int rewrite_imgmaskpath, ProgressViewer *progress)
{
    int i, j;
    arrays a;
    a.radiance1 = NULL;
    a.radianceh = NULL;
    a.radianceq = NULL;
    a.radiance2 = NULL;

    a.brightness1 = NULL;
    a.brightnessh = NULL;
    a.brightnessq = NULL;
    a.brightness2 = NULL;

    a.cloudmask = NULL;


    progress->setValue(10);

    //create_database_if_not_exists("localhost", "root", "root");
    readradiance(path1, pathh, pathq, &a, progress, 30);
    progress->setValue(40);
    a.cloudmask = (char **)malloc(a.heightq*sizeof(char *));
    for (i = 0; i < a.heightq; i++)
    {
      a.cloudmask[i] = (char *)malloc(a.widthq*sizeof(char));
    }
    progress->setValue(45);
    fprintf(stderr, "CHECK progress 45\n");
    getbrightness(&a, progress, 15);
    progress->setValue(60);
    fprintf(stderr, "CHECK progress 60\n");
    detect(&a, progress, 15);
    progress->setValue(75);
    fprintf(stderr, "CHECK progress 80\n");

    savepicture(imgpath, imgmaskpath, &a, progress, 15);
    progress->setValue(90);
    if (a.radianceq)
    {
        for (i = 0; i < 2; i++)
        {
            if (a.radianceq[i])
            {
                for (j = 0; j < a.heightq; j++)
                {
                    if (a.radianceq[i][j])
                    {
                        free(a.radianceq[i][j]);
                    }
                }
                free(a.radianceq[i]);
            }
        }
        free (a.radianceq);
    }
    if (a.radianceh)
    {
        for (i = 0; i < 5; i++)
        {
            if (a.radianceh[i])
            {
                for (j = 0; j < a.heighth; j++)
                {
                    if (a.radianceh[i][j])
                    {
                        free(a.radianceh[i][j]);
                    }
                }
                free(a.radianceh[i]);
            }
        }
        free (a.radianceh);
    }
    if (a.radiance1)
    {
        for (i = 0; i < 15; i++)
        {
            if (a.radiance1[i])
            {
                for (j = 0; j < a.height1; j++)
                {
                    if (a.radiance1[i][j])
                    {
                        free(a.radiance1[i][j]);
                    }
                }
                free(a.radiance1[i]);
            }
        }
        free (a.radiance1);
    }
    if (a.radiance2)
    {
        for (i = 0; i < 16; i++)
        {
            if (a.radiance2[i])
            {
                for (j = 0; j < a.height2; j++)
                {
                    if (a.radiance2[i][j])
                    {
                        free(a.radiance2[i][j]);
                    }
                }
                free(a.radiance2[i]);
            }
        }
        free (a.radiance2);
    }
    int depth;
    if (a.brightness1)
    {
        for (j = 0; j < BRIGHTNESS1NUMBER; j++)
        {
            depth = brightness1essential[j];
            if (a.brightness1[depth])
            {
                for (i = 0; i < a.height1; i++)
                {
                    if (a.brightness1[depth][i])
                    {
                        free(a.brightness1[depth][i]);
                    }
                }
                free(a.brightness1[depth]);
            }
        }
        free(a.brightness1);
    }
    if (a.brightness2)
    {
        for (j = 0; j < BRIGHTNESS2NUMBER; j++)
        {
            depth = brightness2essential[j];
            if (a.brightness2[depth])
            {
                for (i = 0; i < a.height2; i++)
                {
                    if (a.brightness2[depth][i])
                    {
                        free(a.brightness2[depth][i]);
                    }
                }
                free(a.brightness2[depth]);
            }
        }
        free(a.brightness2);
    }

    if (a.cloudmask)
    {
        for (i = 0; i < a.heightq; i++)
        {
            if (a.cloudmask[i])
            {
                free(a.cloudmask[i]);
            }
        }
        free(a.cloudmask);
    }
    if (a.longitude)
    {
        for (i = 0; i < a.height1; i++)
        {
            free(a.longitude[i]);
        }
        free(a.longitude);
    }
    if (a.latitude)
    {
        for (i = 0; i < a.height1; i++)
        {
            free(a.latitude[i]);
        }
        free(a.latitude);
    }
    fprintf(stderr, "CHECK progress-setValue(90)\n");
    progress->setValue(100);
    fprintf(stderr, "CHECK progress 100\n");


    return true;
}
