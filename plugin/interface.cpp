#include "interface.h"
#define MASKDEPTH 	10



using namespace std;

bool process(QString ipath1, QString ipathh, QString ipathq, QString iimgpath)
{
  QByteArray tempstr1, tempstrh, tempstrq, tempstri;
  
  char *path1, *pathh, *pathq, *imgpath;
  
  tempstr1 = ipath1.toLatin1();
  path1 = tempstr1.data();
  tempstrh = ipathh.toLatin1();
  pathh = tempstrh.data();
  tempstrq = ipathq.toLatin1();
  pathq = tempstrq.data();
  tempstri = iimgpath.toLatin1();
  imgpath = tempstri.data();
  
  unsigned short ***radiance1 = NULL;
  unsigned short ***radianceh = NULL;
  unsigned short ***radianceq = NULL;
  unsigned short ***radiance2 = NULL;
  
  float ***brightness1 = NULL;
  float ***brightnessh = NULL;
  float ***brightnessq = NULL;
  float ***brightness2 = NULL;
  
  float32 *offsets;
  float32 *scales;
  
  bool ***cloudmask;
  int *height;
  int *width;
  ofstream log;
  
  
  log.open("newlog.txt");
  log << "start" << endl;
  log.flush();
  height = (int *)malloc(sizeof(int)*5);
  width = (int *)malloc(sizeof(int)*5);
  cloudmask = (bool ***)malloc(sizeof(bool **)*MASKDEPTH);
  readradiance(path1, pathh, pathq, &radiance1, &radianceh, &radianceq, &radiance2, &offsets, &scales, height, width, log);
  log << "read success " << height[0] << " " << width[0] << " " << height[1] << " " << width[1] << " " << height[2] << " " << width[2] << endl;
  log.flush();
  for (int depth = 0; depth < MASKDEPTH; depth++)
  {	
    
    cloudmask[depth] = (bool **)malloc(height[0]*sizeof(bool *));
    for (int i = 0; i < height[0]; i++)
    {
      cloudmask[depth][i] = (bool *)malloc(width[0]*sizeof(bool));
    }
  }
  log << height[0] << ", " << height[1] << ", " << height[2] << ", " << height[3] << endl;
  log.flush();
  getbrightness(radiance1, radianceh, radianceq, radiance2, height, width, &brightness1, &brightnessh, &brightnessq, &brightness2, scales, offsets, log);
  log << "detecting" << endl;
  log.flush();
  detect(radiance1, radianceh, radianceq, radiance2, brightness1, brightnessh, brightnessq, brightness2, height, width, scales, offsets, log, cloudmask);
  char mskpath[255];
  sprintf(mskpath, "%s%s", imgpath, ".mask.png");
  log << "saving" << endl;
  log.flush();
  savepicture(imgpath,mskpath, radiance1, radianceh, radianceq, cloudmask, height, width, offsets, scales, log);
  log << "free" << endl;
  log.flush();
  for (int i = 0; i < 15; i++)
    for (int j = 0; j < height[2]; j++)
      delete [] radiance1[i][j];
  free(radiance1);
  log << "free" << endl;
  log.flush();
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < height[1]; j++)
      delete [] radianceh[i][j];
  
  free(radianceh);
  log << "free" << endl;
  log.flush();
  
  for (int i = 0; i< 2; i++)
    for (int j = 0; j < height[0]; j++)
      delete [] radianceq[i][j];
  free(radianceq);
  log << "free" << endl;
  log.flush();
  for (int i = 0; i< 15; i++)
    for (int j = 0; j < height[3]; j++)
      delete [] radiance2[i][j];
  free(radiance2);
  log << "free" << endl;
  log.flush();
  
  for (int i = 0; i < 15; i++)
    for (int j = 0; j < height[2]; j++)
      delete [] brightness1[i][j];
  free(brightness1);
  log << "free" << endl;
  log.flush();
  
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < height[1]; j++)
      delete [] brightnessh[i][j];
  free(brightnessh);
  log << "free" << endl;
  log.flush();
  
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < height[0]; j++)
      delete [] brightnessq[i][j];
  free(brightnessq);
  log << "free" << endl;
  log.flush();
  
  for (int i = 0; i < 15; i++)
    for (int j = 0; j < height[3]; j++)
      delete [] brightness2[i][j];
  free(brightness2);
  log << "free" << endl;
  log.flush();
  
  delete [] offsets;
  delete [] scales;
  
  for (int i = 0; i < MASKDEPTH; i ++)
    for (int j = 0; j < height[0]; j++)
      delete [] cloudmask[i][j];
  free(cloudmask);
  log << "free" << endl;
  log.flush();
  delete [] height;
  delete [] width;
  log.close();
  return true;
}
