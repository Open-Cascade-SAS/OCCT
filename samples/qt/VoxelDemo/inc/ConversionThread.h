#ifndef _CONVERSIONTHREAD_H_
#define _CONVERSIONTHREAD_H_

#include <QThread.h>
//#include <Voxel_Converter.hxx>
#include <Voxel_FastConverter.hxx>

class ConversionThread : public QThread
{

public:

    ConversionThread();
    ~ConversionThread();

//    void setConverter(Voxel_Converter*     converter);
    void setConverter(Voxel_FastConverter* converter);

    void setVolumicValue(const int value);
    void setScanSide(const int side);
    void setThreadIndex(const int ithread);

    int* getProgress();

protected:

    void run();

private:

//    Voxel_Converter*     myConverter;
    Voxel_FastConverter* myFastConverter;

    int myVolumicValue;
    int myScanSide;
    int myThreadIndex;
    int myProgress;
};

#endif // _CONVERSIONTHREAD_H_
