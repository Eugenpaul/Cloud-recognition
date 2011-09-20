/****************************************************************************
** Meta object code from reading C++ file 'qgiscloudrecognitionplugin.h'
**
** Created: Fri Sep 16 02:38:11 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qgiscloudrecognitionplugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qgiscloudrecognitionplugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QgsCloudRecognitionPlugin[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x0a,
      40,   26,   26,   26, 0x0a,
      50,   26,   26,   26, 0x0a,
      67,   26,   26,   26, 0x0a,
      79,   26,   26,   26, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QgsCloudRecognitionPlugin[] = {
    "QgsCloudRecognitionPlugin\0\0MainWindow()\0"
    "openHDF()\0deletefromlist()\0saveimage()\0"
    "Process()\0"
};

const QMetaObject QgsCloudRecognitionPlugin::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QgsCloudRecognitionPlugin,
      qt_meta_data_QgsCloudRecognitionPlugin, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QgsCloudRecognitionPlugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QgsCloudRecognitionPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QgsCloudRecognitionPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QgsCloudRecognitionPlugin))
        return static_cast<void*>(const_cast< QgsCloudRecognitionPlugin*>(this));
    if (!strcmp(_clname, "QgisPlugin"))
        return static_cast< QgisPlugin*>(const_cast< QgsCloudRecognitionPlugin*>(this));
    return QObject::qt_metacast(_clname);
}

int QgsCloudRecognitionPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: MainWindow(); break;
        case 1: openHDF(); break;
        case 2: deletefromlist(); break;
        case 3: saveimage(); break;
        case 4: Process(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
