#include "convert.h"
#include "detection.h"
#include "pic.h"
#include "readhdf.h"
#include "structures.h"
#include <QObject>
#include <QTextStream>
#include <QProgressBar>
#include <QWidget>

#define LEAVE_ORIGINAL                  1
#define MOVE_ORIGINAL                   2
#define REWRITE_ORIGINAL                3

#define CREATE_NEW                      4
#define CREATE_NEW_AND_DELETE_ORIGINAL  5

bool process(QString qPathKM, QString qPathH, QString qPathQ, QString qImgPath, QString qImgMaskPath, ProgressViewer *progress);