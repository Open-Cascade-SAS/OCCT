#ifndef INTERFACEGRAPHIC_RawBufferData_H
#define INTERFACEGRAPHIC_RawBufferData_H

#include <Standard.hxx>

// Most items are from GLenum, but values not equal!
typedef enum
{
  TRGB,
  TBGR,
  TRGBA,
  TBGRA,
  TDepthComponent,
  TRed,
  TGreen,
  TBlue,
  TAlpha,
} TRawBufferDataFormat;

typedef enum
{
  TUByte,
  TFloat,
} TRawBufferDataType;

struct TRawBufferData
{
  Standard_Integer widthPx;
  Standard_Integer heightPx;
  Standard_Integer rowAligmentBytes;
  TRawBufferDataFormat format;
  TRawBufferDataType type;
  Standard_Address dataPtr;
};

#endif /* INTERFACEGRAPHIC_RawBufferData_H */
