#ifndef _MFT_CommandDescriptor_HeaderFile
#define _MFT_CommandDescriptor_HeaderFile

typedef unsigned int MFT_CommandDescriptor;

#define COMMAND_VALUETYPE(d,n) ((d >> (32 - 2*n)) & 0x3)
#define SET_COMMAND_VALUETYPE(d,n,t) (d |= t << (32 - 2*n))
#define COMMAND_LENGTH(d) ((d >> 8) & 0xFF)
#define SET_COMMAND_LENGTH(d,l) (d = (d & 0xFFFF00FF)  | (l << 8))
#define COMMAND_TYPE(d) MFT_TypeOfCommand(d & 0xFF)
#define SET_COMMAND_TYPE(d,t) (d = (d & 0xFFFFFF00) | t)

#ifndef _Standard_Type_HeaderFile
#include <Standard_Type.hxx>
#endif
const Handle(Standard_Type)& STANDARD_TYPE(MFT_CommandDescriptor);

#endif
