#include "../inc/files.h"

#define FILE_TYPES_NUMBER 5
#define NAME_INDEX 0
#define PATH1_INDEX 1
#define PATHH_INDEX 2
#define PATHQ_INDEX 3
#define IMG_INDEX 4
#define IMGMASK_INDEX 5

bool fileExists(QString &path)
{
  return QFile(path).exists();
}

bool directoryExists(QString &path)
{
  return QDir(path).exists();
}

bool createDirectory(QString &path)
{
  return QDir().mkpath(path);
}

bool getPathsByName(QString &name, QString &pathKM, QString &pathHKM, QString &pathQKM, QString &pathMap, QString &pathMask, QString &dataFile)
{
  QFile file(dataFile);
  QTextStream in(&file);
  QString line;
  bool found;

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return false;
  }

  found = false;
  while(( !file.atEnd() ) && ( !found ))
  {
    line = file.readLine();
    line.resize(line.size() - 1);
    if (line == name)
    {
      found = true;
      pathKM = file.readLine();
      pathKM.resize(pathKM.size() - 1);
      pathHKM = file.readLine();
      pathHKM.resize(pathHKM.size() - 1);
      pathQKM = file.readLine();
      pathQKM.resize(pathQKM.size() - 1);
      pathMap = file.readLine();
      pathMap.resize(pathMap.size() - 1);
      pathMask = file.readLine();
      pathMask.resize(pathMask.size() - 1);
    }
    else
    {
      for (int i = 0; (i < FILE_TYPES_NUMBER) && (!file.atEnd()); i++)
      {
        file.readLine();
      }
    }
  }
  file.close();
  if (!found)
  {
    pathKM = "";
    pathHKM = "";
    pathQKM = "";
    pathMap = "";
    pathMask = "";
  }

  return true;
}

bool searchRequest(QString &searchWord, QStringList &result, QString &dataFile)
{
  QFile file(dataFile);
  QTextStream in(&file);
  QString line;
  bool caseSensitive = true;

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return false;
  }

  result.clear();
  QByteArray bline;
  while ( !file.atEnd() )
  {
    line = file.readLine();
    bline = searchWord.toUtf8();
    if (line.contains(bline.data(), Qt::CaseInsensitive))
    {
      line.resize(line.size() - 1);
      result << QString(line);
    }
    for (int i = 0; (i < FILE_TYPES_NUMBER) && (!file.atEnd()); i++)
    {
      file.readLine();
    }
  }

  file.close();
  return true;
}

bool addFile(QString &path, QString &dataFile)
{
  int slash = path.lastIndexOf('/');
  int firstDot = path.indexOf('.', slash);
  int lastDot;
  QStringRef prefix;
  QStringRef name;
  QStringRef extension;
  QString finalprefix;
  QString finalname;

  bool hdfmode = false;
  bool tiffmode = false;
  bool found = false;
  int dataIndex = 0;

  if (slash == -1)
  {
    return false;
  }
  if (firstDot == -1)
  {
    return false;
  }
  lastDot = path.lastIndexOf('.');
  if (lastDot == -1)
  {
    return false;
  }
  prefix = path.midRef(slash + 1, firstDot - slash);
  name = path.midRef(firstDot + 1, lastDot - firstDot - 1);
  extension = path.midRef(lastDot);

  if (extension == ".hdf")
  {
      hdfmode = true;
  }
  else if (extension == ".tiff")
  {
      tiffmode = true;
  }
  if (tiffmode)
  {
    lastDot = name.lastIndexOf('.');
    if (lastDot == -1)
    {
      return false;
    }
    finalprefix = name.toString().midRef(lastDot).toString() + extension.toString();
    finalname = prefix.toString() + name.toString().midRef(0, lastDot).toString();
  }
  else
  {
    finalprefix = prefix.toString();
    finalname = name.toString();
  }

  if ("MOD021KM." == finalprefix)
  {
    dataIndex = PATH1_INDEX;
  }
  else if ("MOD02HKM." == finalprefix)
  {
    dataIndex = PATHH_INDEX;
  }
  else if ("MOD02QKM." == finalprefix)
  {
    dataIndex = PATHQ_INDEX;
  }
  else if (".surface.tiff" == finalprefix)
  {
    dataIndex = IMG_INDEX;
  }
  else if (".mask.tiff" == finalprefix)
  {
    dataIndex = IMGMASK_INDEX;
  }
  else
  {
    return false;
  }

  QFile file(dataFile);
  QFile ofile(dataFile + ".tmp");

  file.open(QIODevice::ReadOnly | QIODevice::Text);
  ofile.open(QIODevice::WriteOnly | QIODevice::Text);

  QTextStream in(&file);
  QTextStream out(&ofile);

  QString line;
  QByteArray bline;

  found = false;
  while(( !file.atEnd() ))
  {
    line = file.readLine();
    if ((!found) && (line == finalname + "\n"))
    {
      out << line;
      found = true;
      for (int i = 1; i <= FILE_TYPES_NUMBER; i++)
      {
        line = file.readLine();
        if (i == dataIndex)
        {
          //bline = path.to();
          out << path + "\n";
          //out << std::endl;
        }
        else
        {
          out << line;
        }
      }
    }
    else
    {
      out << line;
      for (int i = 0; (i < FILE_TYPES_NUMBER) && (!file.atEnd()); i++)
      {
        line = file.readLine();
        out << line;
      }
    }
  }
  if (!found)
  {
    out << finalname + "\n";
    for (int i = 1; i <= FILE_TYPES_NUMBER; i++)
    {
      if (i == dataIndex)
      {
        out << path;
      }
      out << "\n";
    }
  }
  file.close();
  ofile.close();

  file.remove();
  ofile.rename(dataFile);

  return true;
}