#include "ConversionThread.h"

ConversionThread::ConversionThread():QThread(),
/*myConverter(0),*/myFastConverter(0),
myVolumicValue(0),myScanSide(1),
myThreadIndex(1)
{

}

ConversionThread::~ConversionThread()
{

}

/*
void ConversionThread::setConverter(Voxel_Converter* converter)
{
    myConverter = converter;
    myFastConverter = 0;
}
*/

void ConversionThread::setConverter(Voxel_FastConverter* converter)
{
    myFastConverter = converter;
    //myConverter = 0;
}

void ConversionThread::setVolumicValue(const int value)
{
    myVolumicValue = value;
}

void ConversionThread::setScanSide(const int side)
{
    myScanSide = side;
}

void ConversionThread::setThreadIndex(const int ithread)
{
    myThreadIndex = ithread;
}

int* ConversionThread::getProgress()
{
    return &myProgress;
}

void ConversionThread::run()
{
    if (/*!myConverter && */!myFastConverter)
        return;
    //if (myConverter)
    //    myConverter->Convert(myProgress, myVolumicValue, myScanSide, myThreadIndex);
    //else
        myFastConverter->Convert(myProgress, myThreadIndex);
}
