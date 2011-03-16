#ifndef _MFT_FileHandle_HeaderFile
#define _MFT_FileHandle_HeaderFile

#define MFT_PAGESIZE 4096

#include <Standard_Type.hxx>

typedef Standard_Integer MFT_FileHandle; 

#ifndef _Standard_Type_HeaderFile
#include <Standard_Type.hxx>
#endif
const Handle(Standard_Type)& STANDARD_TYPE(MFT_FileHandle);

#endif
