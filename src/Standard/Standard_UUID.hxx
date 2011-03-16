#ifndef _Standard_UUID_HeaderFile
#define _Standard_UUID_HeaderFile

#include <Standard_Type.hxx>

#ifdef WNT
#include <windows.h>
#else
typedef struct {
  unsigned long Data1 ;
  unsigned short Data2 ;
  unsigned short Data3 ;
  unsigned char Data4[8] ;
} GUID ;
#endif

typedef GUID Standard_UUID ;

class Handle_Standard_Type;
const Handle(Standard_Type) & TYPE(Standard_UUID);

#endif


