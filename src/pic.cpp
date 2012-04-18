/*
 * pic.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eugenpaul
 */

#include "pic.h"
#include <gdal/cpl_string.h>
#include <gdal/ogr_spatialref.h>
#include "gdal/gdal_priv.h"
/*#include "gdalwarper.h"
      #include "cpl_string.h"
      #include "ogr_spatialref.h"
      #include "ogr_api.h"*/

#define DIGIT_WIDTH 30
#define DIGIT_HEIGHT 45

#define DIGIT_MATRIX_WIDTH 6
#define DIGIT_MATRIX_HEIGHT 7

const char zero[DIGIT_MATRIX_HEIGHT][DIGIT_MATRIX_WIDTH] =
                    {
                      { 0, 1, 1, 1, 0, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 0, 1, 1, 1, 0, 0 }
                     };

const char one[DIGIT_MATRIX_HEIGHT][DIGIT_MATRIX_WIDTH] =
                    {
                      { 0, 0, 1, 0, 0, 0 },
                      { 0, 1, 1, 0, 0, 0 },
                      { 0, 0, 1, 0, 0, 0 },
                      { 0, 0, 1, 0, 0, 0 },
                      { 0, 0, 1, 0, 0, 0 },
                      { 0, 0, 1, 0, 0, 0 },
                      { 1, 1, 1, 1, 1, 0 }
                     };
const char two[DIGIT_MATRIX_HEIGHT][DIGIT_MATRIX_WIDTH] =
                    {
                      { 0, 1, 1, 1, 0, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 0, 0, 0, 0, 1, 0 },
                      { 0, 0, 0, 1, 0, 0 },
                      { 0, 0, 1, 0, 0, 0 },
                      { 0, 1, 0, 0, 0, 0 },
                      { 1, 1, 1, 1, 1, 0 }
                     };
const char three[DIGIT_MATRIX_HEIGHT][DIGIT_MATRIX_WIDTH] =
                    {
                      { 0, 1, 1, 1, 0, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 0, 0, 0, 0, 1, 0 },
                      { 0, 0, 1, 1, 0, 0 },
                      { 0, 0, 0, 0, 1, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 0, 1, 1, 1, 0, 0 }
                     };
const char four[DIGIT_MATRIX_HEIGHT][DIGIT_MATRIX_WIDTH] =
                    {
                      { 0, 0, 0, 1, 0, 0 },
                      { 0, 0, 1, 1, 0, 0 },
                      { 0, 1, 0, 1, 0, 0 },
                      { 1, 0, 0, 1, 0, 0 },
                      { 1, 1, 1, 1, 1, 0 },
                      { 0, 0, 0, 1, 0, 0 },
                      { 0, 0, 0, 1, 0, 0 }
                     };
const char five[DIGIT_MATRIX_HEIGHT][DIGIT_MATRIX_WIDTH] =
                    {
                      { 1, 1, 1, 1, 1, 0 },
                      { 1, 0, 0, 0, 0, 0 },
                      { 1, 1, 1, 1, 0, 0 },
                      { 0, 0, 0, 0, 1, 0 },
                      { 0, 0, 0, 0, 1, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 0, 1, 1, 1, 0, 0 }
                     };
const char six[DIGIT_MATRIX_HEIGHT][DIGIT_MATRIX_WIDTH] =
                    {
                      { 0, 1, 1, 1, 0, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 1, 0, 0, 0, 0, 0 },
                      { 1, 1, 1, 1, 0, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 0, 1, 1, 1, 0, 0 }
                     };
const char seven[DIGIT_MATRIX_HEIGHT][DIGIT_MATRIX_WIDTH] =
                    {
                      { 1, 1, 1, 1, 1, 0 },
                      { 0, 0, 0, 0, 1, 0 },
                      { 0, 0, 0, 1, 0, 0 },
                      { 0, 0, 1, 0, 0, 0 },
                      { 0, 0, 1, 0, 0, 0 },
                      { 0, 0, 1, 0, 0, 0 },
                      { 0, 0, 1, 0, 0, 0 }
                     };
const char eight[DIGIT_MATRIX_HEIGHT][DIGIT_MATRIX_WIDTH] =
                    {
                      { 0, 1, 1, 1, 0, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 0, 1, 1, 1, 0, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 0, 1, 1, 1, 0, 0 }
                     };
const char nine[DIGIT_MATRIX_HEIGHT][DIGIT_MATRIX_WIDTH] =
                    {
                      { 0, 1, 1, 1, 0, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 0, 1, 1, 1, 1, 0 },
                      { 0, 0, 0, 0, 1, 0 },
                      { 1, 0, 0, 0, 1, 0 },
                      { 0, 1, 1, 1, 0, 0 }
                     };
//using namespace std;

double abs(double value)
{
    return value > 0? value: -value;
}


#if 0
void print_number_png(int value, int xfirst, int yfirst, int xlast, int ylast, png::image<png::rgb_pixel> &image)
{
    int digit = 0;
    int i, j, ki, kj, ii, jj;
    ki = DIGIT_WIDTH/DIGIT_MATRIX_WIDTH;
    kj = DIGIT_HEIGHT/DIGIT_MATRIX_HEIGHT;
    while (value != 0)
    {
        digit = value%10;
        value = value/10;
        switch (digit)
        {
#define DIGITPNG(name)                                                                     \
                for (i = DIGIT_MATRIX_WIDTH; i > 0; i--)                                \
                {                                                                       \
                    for (j = DIGIT_MATRIX_HEIGHT; j > 0; j--)                           \
                    {                                                                   \
                        if (name[DIGIT_MATRIX_HEIGHT - j][DIGIT_MATRIX_WIDTH - i])      \
                        {                                                               \
                            for (ii = i * ki; (ii > (i - 1) * ki) && (ii > 0); ii--)    \
                            {                                                           \
                                for (jj = j * kj; (jj > (j - 1) * kj) && (jj > 0); jj--)                                                \
                                {                                                                                                       \
                                    image[xlast - jj > 0? xlast - jj: 0][ylast - ii > 0? ylast - ii: 0] = png::rgb_pixel(0, 255, 0);    \
                                }                                                                                                       \
                            }                                                           \
                        }                                                               \
                        else                                                            \
                          for (ii = i * ki; (ii > (i - 1) * ki) && (ii > 0); ii--)      \
                            {                                                           \
                                for (jj = j * kj; (jj > (j - 1) * kj) && (jj > 0); jj--)                                                \
                                {                                                                                                       \
                                    image[xlast - jj > 0? xlast - jj: 0][ylast - ii > 0? ylast - ii: 0] = png::rgb_pixel(0, 0, 0);      \
                                }                                                                                                       \
                            }                                                           \
                    }                                                                   \
                }
            case 0:
            {
                DIGITPNG(zero);
                break;
            }
            case 1:
            {
                DIGITPNG(one);
                break;
            }
            case 2:
            {
                DIGITPNG(two);
                break;
            }
            case 3:
            {
                DIGITPNG(three);
                break;
            }
            case 4:
            {
                DIGITPNG(four);
                break;
            }
            case 5:
            {
                DIGITPNG(five);
                break;
            }
            case 6:
            {
                DIGITPNG(six);
                break;
            }
            case 7:
            {
                DIGITPNG(seven);
                break;
            }
            case 8:
            {
                DIGITPNG(eight);
                break;
            }
            case 9:
            {
                DIGITPNG(nine);
                break;
            }
            default:
            {
                DIGITPNG(zero);
                break;
            }
        }
        ylast -= DIGIT_WIDTH;
    }
}
#endif

void print_number(int value, int xfirst, int yfirst, int xlast, int ylast, GByte *imgRaster, arrays *a)
{
    int digit = 0;
    int i, j, ki, kj, ii, jj;
    ki = DIGIT_WIDTH/DIGIT_MATRIX_WIDTH;
    kj = DIGIT_HEIGHT/DIGIT_MATRIX_HEIGHT;
    while (value != 0)
    {
        digit = value%10;
        value = value/10;
        switch (digit)
        {
#define DIGIT(name)                                                                     \
                for (i = DIGIT_MATRIX_WIDTH; i > 0; i--)                                \
                {                                                                       \
                    for (j = DIGIT_MATRIX_HEIGHT; j > 0; j--)                           \
                    {                                                                   \
                        if (name[DIGIT_MATRIX_HEIGHT - j][DIGIT_MATRIX_WIDTH - i])      \
                        {                                                               \
                            for (ii = i * ki; (ii > (i - 1) * ki) && (ii > 0); ii--)    \
                            {                                                           \
                                for (jj = j * kj; (jj > (j - 1) * kj) && (jj > 0); jj--)                                                  \
                                {                                                                                                         \
                                    imgRaster[3*((xlast - jj > 0? xlast - jj: 0)*a->widthq + (ylast - ii > 0? ylast - ii: 0))] = 0;       \
                                    imgRaster[3*((xlast - jj > 0? xlast - jj: 0)*a->widthq + (ylast - ii > 0? ylast - ii: 0)) + 1] = 255; \
                                    imgRaster[3*((xlast - jj > 0? xlast - jj: 0)*a->widthq + (ylast - ii > 0? ylast - ii: 0)) + 2] = 0;   \
                                }                                                                                                         \
                            }                                                           \
                        }                                                               \
                        else                                                            \
                          for (ii = i * ki; (ii > (i - 1) * ki) && (ii > 0); ii--)      \
                            {                                                           \
                                for (jj = j * kj; (jj > (j - 1) * kj) && (jj > 0); jj--)                                                  \
                                {                                                                                                         \
                                    imgRaster[3*((xlast - jj > 0? xlast - jj: 0)*a->widthq + (ylast - ii > 0? ylast - ii: 0))] = 0;       \
                                    imgRaster[3*((xlast - jj > 0? xlast - jj: 0)*a->widthq + (ylast - ii > 0? ylast - ii: 0)) + 1] = 0;   \
                                    imgRaster[3*((xlast - jj > 0? xlast - jj: 0)*a->widthq + (ylast - ii > 0? ylast - ii: 0)) + 2] = 0;   \
                                }                                                                                                         \
                            }                                                           \
                    }                                                                   \
                }
            case 0:
            {
                DIGIT(zero);
                break;
            }
            case 1:
            {
                DIGIT(one);
                break;
            }
            case 2:
            {
                DIGIT(two);
                break;
            }
            case 3:
            {
                DIGIT(three);
                break;
            }
            case 4:
            {
                DIGIT(four);
                break;
            }
            case 5:
            {
                DIGIT(five);
                break;
            }
            case 6:
            {
                DIGIT(six);
                break;
            }
            case 7:
            {
                DIGIT(seven);
                break;
            }
            case 8:
            {
                DIGIT(eight);
                break;
            }
            case 9:
            {
                DIGIT(nine);
                break;
            }
            default:
            {
                DIGIT(zero);
                break;
            }
        }
        ylast -= DIGIT_WIDTH;
    }
}

bool createGDALRaster(char *filenamemap, char *filenamemask,  arrays *a, ProgressViewer *progress, int progresspart)
{
    const char *pszFormat = "GTiff";
    GDALDriver *poDriver;
    char **papszMetadata;
    GDALDataset *poDstDS;
    char **papszOptions = NULL;
    double adfGeoTransform[6] = { 444720, 30, 0, 3751320, 0, -30 };
    OGRSpatialReference oSRS;
    char *pszSRS_WKT = NULL;
    GDALRasterBand *poBand;
    GByte *imgRaster;

    char modifiedfilename[255];
    bool detected = false;
    int i; int j;
    int progress_steps_number = 12;
    int progress_bit = progresspart/progress_steps_number;

    int hi = 0;
    int hj = 0;
    int kmi = 0;
    int kmj = 0;
    int kmii = 0;
    int kmjj = 0;
    int r, g, b;
#if 0
    double          dfMinX=0.0, dfMinY=0.0, dfMaxX=0.0, dfMaxY=0.0;
    char                **papszTO = NULL;
    GDALDatasetH        hDstDS;
    int bCreateOutput = 0;

    char *pszSRS = SanitizeSRS("EPSG:4326");
    papszTO = CSLSetNameValue( papszTO, "DST_SRS", pszSRS );
    CPLFree( pszSRS );
    papszTO = CSLSetNameValue( papszTO, "METHOD", "GEOLOC_ARRAY" );
    dfMinX = CPLAtofM("62");
    dfMinY = CPLAtofM("42");
    dfMaxX = CPLAtofM("108");
    dfMaxY = CPLAtofM("66");
    bCreateOutput = TRUE;
    hDstDS = GDALWarpCreateOutput( papszSrcFiles, pszDstFilename,pszFormat,
                                            papszTO, &papszCreateOptions,
                                            eOutputType, &hUniqueTransformArg,
                                            &hUniqueSrcDS);
#endif
    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);

    if( poDriver == NULL )
        return false;

    papszMetadata = poDriver->GetMetadata();
    if( !CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
    {
       fprintf(stderr, "Driver %s doesn't support Create() method\n", pszFormat);
       return false;
    }
    imgRaster = (GByte *)malloc(a->widthq*a->heightq*3*sizeof(GByte));

    if (filenamemap)
    {
        poDstDS = poDriver->Create( filenamemap, a->widthq, a->heightq, 3, GDT_Byte, papszOptions );


        poDstDS->SetGeoTransform( adfGeoTransform );
        //GDALColorTable *poCT= new GDALColorTable(GPI_RGB);
        oSRS.SetUTM( 11, TRUE );
        oSRS.SetWellKnownGeogCS( "NAD27" );
        oSRS.exportToWkt( &pszSRS_WKT );
        poDstDS->SetProjection( pszSRS_WKT );
        CPLFree( pszSRS_WKT );
        //poBand = poDstDS->GetRasterBand(1);
        //poBand->SetColorTable(poCT);
        for (i = 0; i < a->heightq; i++)
            {
                    hi = int(a->heighth/(double)a->heightq*i);

                    for (j = 0 ; j < a->widthq; j++)
                    {
                            hj = int(a->widthh/(double)a->widthq*j);


                            r = ((a->radianceq[0][i][j] - a->offsetsq[0])*a->scalesq[0]*255)/(150);
                            g = ((a->radianceh[1][hi][hj] - a->offsetsh[1])*a->scalesh[1]*255)/(150);
                            b = ((a->radianceh[0][hi][hj] - a->offsetsh[0])*a->scalesh[0]*255)/(150);

                            if (r > 255) r = 255;
                            if (g > 255) g = 255;
                            if (b > 255) b = 255;
                            imgRaster[3*(i*a->widthq + j)] = r;
                            imgRaster[3*(i*a->widthq + j) + 1] = g;
                            imgRaster[3*(i*a->widthq + j) + 2] = b;
                    }
            }
            progress->setValue(progress->Value() + progress_bit);
            for (i = 0; i < a->height1; i++)
            {
                    kmi = int(a->heightq/(double)a->height1*i);
                    for (j = 0; j < a->width1; j++)
                    {
                            if ((abs(a->latitude[i][j] - int(a->latitude[i][j])) < 0.03) ||
                                (abs(a->longitude[i][j] - int(a->longitude[i][j])) < 0.03))
                            {
                                    kmj = int(a->widthq/(double)a->width1*j);
                                    for (kmii = (kmi - 2 < 0? 0: kmi - 2); kmii < (kmi + 3 >= a->heightq? a->heightq - 1: kmi + 3); kmii++)
                                    {
                                        for (kmjj = (kmj - 2 < 0? 0: kmj - 2); kmjj < (kmj + 3 >= a->widthq? a->widthq - 1: kmj + 3); kmjj++)
                                        {
                                            imgRaster[3*(kmii*a->widthq + kmjj)] = 0;
                                            imgRaster[3*(kmii*a->widthq + kmjj) + 1] = 0;
                                            imgRaster[3*(kmii*a->widthq + kmjj) + 2] = 0;
                                        }
                                    }
                            }
                    }
            }

            for (i = 0; i < a->height1; i ++)
            {
                kmi = int(a->heightq/(double)a->height1*i);
                for (j = 0; j < a->width1; j++)
                {
                    if ((abs(a->longitude[i][j] - int(a->longitude[i][j])) < 0.03) &&
                        (abs(a->latitude[i][j] - int(a->latitude[i][j])) > 0.49) && (abs(a->latitude[i][j] - int(a->latitude[i][j])) < 0.51))
                            {
                                    kmj = int(a->widthq/(double)a->width1*j);
                                    /*for (kmii = (kmi - 50 < 0? 0: kmi - 50); kmii < (kmi + 51 >= a->heightq? a->heightq - 1: kmi + 51); kmii++)
                                    {
                                        for (kmjj = (kmj - 50 < 0? 0: kmj - 50); kmjj < (kmj + 51 >= a->widthq? a->widthq - 1: kmj + 51); kmjj++)
                                        {
                                            image[kmii][kmjj] = png::rgb_pixel(255, 0, 0);
                                        }
                                    }*/
                                    print_number(int(a->longitude[i][j]),
                                                  kmi - 40 < 0? 0: kmi - 40,
                                                  kmj - 40 < 0? 0: kmj - 40,
                                                  kmi + 41 >= a->heightq? a->heightq - 1: kmi + 41,
                                                  kmj + 41 >= a->widthq? a->widthq - 1: kmj + 41,
                                                  imgRaster, a);
                            }
                }
            }
            for (j = 0; j < a->width1; j ++)
            {
                kmj = int(a->widthq/(double)a->width1*j);
                for (i = 0; i < a->height1; i++)
                {
                    if ((abs(a->latitude[i][j] - int(a->latitude[i][j])) < 0.03) &&
                        (abs(a->longitude[i][j] - int(a->longitude[i][j])) > 0.49) && (abs(a->longitude[i][j] - int(a->longitude[i][j])) < 0.51))
                            {
                                    kmi = int(a->heightq/(double)a->height1*i);
                                    /*for (kmii = (kmi - 50 < 0? 0: kmi - 50); kmii < (kmi + 51 >= a->heightq? a->heightq - 1: kmi + 51); kmii++)
                                    {
                                        for (kmjj = (kmj - 50 < 0? 0: kmj - 50); kmjj < (kmj + 51 >= a->widthq? a->widthq - 1: kmj + 51); kmjj++)
                                        {
                                            image[kmii][kmjj] = png::rgb_pixel(0, 0, 255);
                                        }
                                    }*/
                                    print_number(int(a->latitude[i][j]),
                                                  kmi - 40 < 0? 0: kmi - 40,
                                                  kmj - 40 < 0? 0: kmj - 40,
                                                  kmi + 41 >= a->heightq? a->heightq - 1: kmi + 41,
                                                  kmj + 41 >= a->widthq? a->widthq - 1: kmj + 41,
                                                  imgRaster, a);
                            }
                }
            }

            progress->setValue(progress->Value() + progress_bit);
            progress->setValue(progress->Value() + progress_bit);
            progress->setValue(progress->Value() + progress_bit*3);
            poDstDS->RasterIO( GF_Write, 0, 0, a->widthq, a->heightq,
                              imgRaster, a->widthq, a->heightq, GDT_Byte, 3, NULL, 3, 3 * a->widthq, 1);

            GDALClose( (GDALDatasetH) poDstDS );
    }
    if (filenamemask)
        {
            poDstDS = poDriver->Create( filenamemask, a->widthq, a->heightq, 3, GDT_Byte, papszOptions );
            poDstDS->SetGeoTransform( adfGeoTransform );
            oSRS.SetUTM( 11, TRUE );
            oSRS.SetWellKnownGeogCS( "NAD27" );
            oSRS.exportToWkt( &pszSRS_WKT );
            poDstDS->SetProjection( pszSRS_WKT );
            CPLFree( pszSRS_WKT );
            r = 55;
            g = 55;
            b = 55;
            int k = 0;
            int pixelmask[7];
            for (i = 0; i < a->heightq; i++)
            {
                    for (j = 0; j < a->widthq; j++)
                    {
                            for (k = 0; k < 7; k++)
                            {
                                pixelmask[k] = a->cloudmask[i][j] % 2;
                                a->cloudmask[i][j] /= 2;
                            }
                            r = 55;
                            g = 55;
                            b = 55;
                            detected = false;
                            if (pixelmask[0])
                            {
                                    r += 50;
                                    detected = true;
                                    //image[i][j] = png::rgb_pixel(255, 70, 70);
                            }
                            if (pixelmask[1])
                            {
                                    b += 50;
                                    detected = true;
                                    //image[i][j] = png::rgb_pixel(70, 70, 255);
                            }


                            if (pixelmask[3]) //nearby
                            {
                                    //g = 100;
                                    //r = 250;
                                    //b = 100;
                                    //detected = true;
                            }
                            if (pixelmask[4])
                            {

                                    r += 100;

                                    detected = true;
                            }
                            if (pixelmask[2])
                            {
                                    g += 50;
                                    detected = false;
                                    //image[i][j] = png::rgb_pixel(70, 255, 70);
                            }

                            if (pixelmask[5])
                            {
                                    /*g += 100;
                                    detected = true;*/
                            }

                            if (pixelmask[6])
                            {
                                    b += 100;
                                    detected = true;
                            }

                            if (detected)
                            {
                                    imgRaster[3*(i*a->widthq + j)] = r;
                                    imgRaster[3*(i*a->widthq + j) + 1] = g;
                                    imgRaster[3*(i*a->widthq + j) + 2] = b;
                            }
                            else
                            {
                                    imgRaster[3*(i*a->widthq + j)] = 0;
                                    imgRaster[3*(i*a->widthq + j) + 1] = 0;
                                    imgRaster[3*(i*a->widthq + j) + 2] = 0;
                            }

                    }
            }

            progress->setValue(progress->Value() + progress_bit);
            poDstDS->RasterIO( GF_Write, 0, 0, a->widthq, a->heightq,
                              imgRaster, a->widthq, a->heightq, GDT_Byte, 3, NULL, 3, 3 * a->widthq, 1);

            GDALClose( (GDALDatasetH) poDstDS );
        }
        progress->setValue(progress->Value() + progress_bit*3);
    free(imgRaster);
    return true;
}

#if 0
bool savepicture(char *filenamemap, char *filenamemask,  arrays *a, ProgressViewer *progress, int progresspart)
{
	char modifiedfilename[255];
	bool detected = false;
	int i; int j;
        int progress_steps_number = 12;
        int progress_bit = progresspart/progress_steps_number;

	png::image<png::rgb_pixel> image(a->widthq,a->heightq);
	image.resize(a->widthq, a->heightq);

	int hi = 0;
	int hj = 0;
	int kmi = 0;
	int kmj = 0;
        int kmii = 0;
        int kmjj = 0;
	int r, g, b;


        if (filenamemap)
        {
            for (i = 0; i < a->heightq; i++)
            {
                    hi = int(a->heighth/(double)a->heightq*i);

                    for (j = 0 ; j < a->widthq; j++)
                    {
                            hj = int(a->widthh/(double)a->widthq*j);


                            r = ((a->radianceq[0][i][j] - a->offsetsq[0])*a->scalesq[0]*255)/(150);
                            g = ((a->radianceh[1][hi][hj] - a->offsetsh[1])*a->scalesh[1]*255)/(150);
                            b = ((a->radianceh[0][hi][hj] - a->offsetsh[0])*a->scalesh[0]*255)/(150);

                            if (r > 255) r = 255;
                            if (g > 255) g = 255;
                            if (b > 255) b = 255;
                            image[i][j] = png::rgb_pixel(r, g, b);
                    }
            }
            progress->setValue(progress->Value() + progress_bit);
            for (i = 0; i < a->height1; i++)
            {
                    kmi = int(a->heightq/(double)a->height1*i);
                    for (j = 0; j < a->width1; j++)
                    {
                            if ((abs(a->latitude[i][j] - int(a->latitude[i][j])) < 0.03) ||
                                (abs(a->longitude[i][j] - int(a->longitude[i][j])) < 0.03))
                            {
                                    kmj = int(a->widthq/(double)a->width1*j);
                                    for (kmii = (kmi - 2 < 0? 0: kmi - 2); kmii < (kmi + 3 >= a->heightq? a->heightq - 1: kmi + 3); kmii++)
                                    {
                                        for (kmjj = (kmj - 2 < 0? 0: kmj - 2); kmjj < (kmj + 3 >= a->widthq? a->widthq - 1: kmj + 3); kmjj++)
                                        {
                                            image[kmii][kmjj] = png::rgb_pixel(0, 0, 0);
                                        }
                                    }
                            }
                    }
            }

            for (i = 0; i < a->height1; i ++)
            {
                kmi = int(a->heightq/(double)a->height1*i);
                for (j = 0; j < a->width1; j++)
                {
                    if ((abs(a->longitude[i][j] - int(a->longitude[i][j])) < 0.03) &&
                        (abs(a->latitude[i][j] - int(a->latitude[i][j])) > 0.49) && (abs(a->latitude[i][j] - int(a->latitude[i][j])) < 0.51))
                            {
                                    kmj = int(a->widthq/(double)a->width1*j);
                                    /*for (kmii = (kmi - 50 < 0? 0: kmi - 50); kmii < (kmi + 51 >= a->heightq? a->heightq - 1: kmi + 51); kmii++)
                                    {
                                        for (kmjj = (kmj - 50 < 0? 0: kmj - 50); kmjj < (kmj + 51 >= a->widthq? a->widthq - 1: kmj + 51); kmjj++)
                                        {
                                            image[kmii][kmjj] = png::rgb_pixel(255, 0, 0);
                                        }
                                    }*/
                                    print_number_png(int(a->longitude[i][j]),
                                                  kmi - 40 < 0? 0: kmi - 40,
                                                  kmj - 40 < 0? 0: kmj - 40,
                                                  kmi + 41 >= a->heightq? a->heightq - 1: kmi + 41,
                                                  kmj + 41 >= a->widthq? a->widthq - 1: kmj + 41,
                                                  image);
                            }
                }
            }
            for (j = 0; j < a->width1; j ++)
            {
                kmj = int(a->widthq/(double)a->width1*j);
                for (i = 0; i < a->height1; i++)
                {
                    if ((abs(a->latitude[i][j] - int(a->latitude[i][j])) < 0.03) &&
                        (abs(a->longitude[i][j] - int(a->longitude[i][j])) > 0.49) && (abs(a->longitude[i][j] - int(a->longitude[i][j])) < 0.51))
                            {
                                    kmi = int(a->heightq/(double)a->height1*i);
                                    /*for (kmii = (kmi - 50 < 0? 0: kmi - 50); kmii < (kmi + 51 >= a->heightq? a->heightq - 1: kmi + 51); kmii++)
                                    {
                                        for (kmjj = (kmj - 50 < 0? 0: kmj - 50); kmjj < (kmj + 51 >= a->widthq? a->widthq - 1: kmj + 51); kmjj++)
                                        {
                                            image[kmii][kmjj] = png::rgb_pixel(0, 0, 255);
                                        }
                                    }*/
                                    print_number_png(int(a->latitude[i][j]),
                                                  kmi - 40 < 0? 0: kmi - 40,
                                                  kmj - 40 < 0? 0: kmj - 40,
                                                  kmi + 41 >= a->heightq? a->heightq - 1: kmi + 41,
                                                  kmj + 41 >= a->widthq? a->widthq - 1: kmj + 41,
                                                  image);
                            }
                }
            }

            progress->setValue(progress->Value() + progress_bit);
            progress->setValue(progress->Value() + progress_bit);
            image.write(filenamemap);
        }
        progress->setValue(progress->Value() + progress_bit*3);

	/*bool up, down, left, right;
	for (i = 1; i < heightq - 1; i++)
	{
		for (j = 1; j < widthq - 1; j++)
		{
			if ((!cloudmask[0][i][j])&&(!cloudmask[1][i][j])&&(!cloudmask[2][i][j])&&(!cloudmask[4][i][j])&&(!cloudmask[5][i][j])&&(!cloudmask[7][i][j]))
			{
				up = cloudmask[0][i-1][j]||cloudmask[1][i-1][j]||cloudmask[2][i-1][j]||cloudmask[3][i-1][j]||cloudmask[4][i-1][j]||cloudmask[5][i-1][j]||cloudmask[6][i-1][j]||cloudmask[7][i-1][j];
				down = cloudmask[0][i+1][j]||cloudmask[1][i+1][j]||cloudmask[2][i+1][j]||cloudmask[3][i+1][j]||cloudmask[4][i+1][j]||cloudmask[5][i+1][j]||cloudmask[6][i+1][j]||cloudmask[7][i+1][j];
				left = cloudmask[0][i][j-1]||cloudmask[1][i][j-1]||cloudmask[2][i][j-1]||cloudmask[3][i][j-1]||cloudmask[4][i][j-1]||cloudmask[5][i][j-1]||cloudmask[6][i][j-1]||cloudmask[7][i][j-1];
				right = cloudmask[0][i][j+1]||cloudmask[1][i][j+1]||cloudmask[2][i][j+1]||cloudmask[3][i][j+1]||cloudmask[4][i][j+1]||cloudmask[5][i][j+1]||cloudmask[6][i][j+1]||cloudmask[7][i][j+1];
				if(up||down||left||right)
				{
					r = 250;
					g = 0;
					b = 0;
					image[i][j] = png::rgb_pixel(r,g,b);
				}
			}
		}
	}
	sprintf(modifiedfilename, "%s%s", filename, "borders.png");
	image.write(modifiedfilename);
*/
        if (filenamemask)
        {
            r = 55;
            g = 55;
            b = 55;
            int k = 0;
            int pixelmask[7];
            for (i = 0; i < a->heightq; i++)
            {
                    for (j = 0; j < a->widthq; j++)
                    {
                            for (k = 0; k < 7; k++)
                            {
                                pixelmask[k] = a->cloudmask[i][j] % 2;
                                a->cloudmask[i][j] /= 2;
                            }
                            r = 55;
                            g = 55;
                            b = 55;
                            detected = false;
                            if (pixelmask[0])
                            {
                                    r += 50;
                                    detected = true;
                                    //image[i][j] = png::rgb_pixel(255, 70, 70);
                            }
                            if (pixelmask[1])
                            {
                                    b += 50;
                                    detected = true;
                                    //image[i][j] = png::rgb_pixel(70, 70, 255);
                            }


                            if (pixelmask[3]) //nearby
                            {
                                    //g = 100;
                                    //r = 250;
                                    //b = 100;
                                    //detected = true;
                            }
                            if (pixelmask[4])
                            {

                                    r += 100;

                                    detected = true;
                            }
                            if (pixelmask[2])
                            {
                                    g += 50;
                                    detected = false;
                                    //image[i][j] = png::rgb_pixel(70, 255, 70);
                            }

                            if (pixelmask[5])
                            {
                                    /*g += 100;
                                    detected = true;*/
                            }

                            if (pixelmask[6])
                            {
                                    b += 100;
                                    detected = true;
                            }

                            if (detected)
                            {
                                    image[i][j] = png::rgb_pixel(r, g, b);
                            }
                            else
                            {
                                    image[i][j] = png::rgb_pixel(0, 0, 0);
                            }

                    }
            }

            progress->setValue(progress->Value() + progress_bit);
            //sprintf(modifiedfilename, "%s%s", filename, "mask.png");
            image.write(filenamemask);
        }
	progress->setValue(progress->Value() + progress_bit*3);

	return true;
}
#endif