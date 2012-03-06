#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <QObject>
#include "hdf/mfhdf.h"

class ProgressViewer : public QObject
{
    Q_OBJECT

    public:
        ProgressViewer() { value = 0; }
        int Value() const { return value; }

    public slots:
        void setValue(int newvalue)
        {
            if (value != newvalue)
            {
                value = newvalue;
                emit valueChanged(value);
            }
        }

    signals:
        void valueChanged(int newValue);

    private:
        int value;
};

typedef struct arrays_s
{
    unsigned short ***radiance1;
    unsigned short ***radianceh;
    unsigned short ***radianceq;
    unsigned short ***radiance2;

    float ***brightness1;
    float ***brightnessh;
    float ***brightnessq;
    float ***brightness2;

    float32 **longitude;
    float32 **latitude;

    float32 *offsets1;
    float32 *scales1;

    float32 *offsetsh;
    float32 *scalesh;

    float32 *offsetsq;
    float32 *scalesq;

    float32 *offsets2;
    float32 *scales2;

    char **cloudmask;

    int height1;
    int width1;

    int heighth;
    int widthh;

    int heightq;
    int widthq;

    int height2;
    int width2;

}arrays;

#endif