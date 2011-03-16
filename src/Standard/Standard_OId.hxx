#ifndef _Standard_OId_HeaderFile
#define _Standard_OId_HeaderFile

#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif

#ifndef _Standard_OStream_HeaderFile
#include <Standard_OStream.hxx>
#endif

class Standard_Persistent;
typedef Standard_Persistent* Standard_OId;

extern Standard_OId ShallowCopy (const Standard_OId me) ;
extern Standard_Integer HashCode(const Standard_OId me,
			  const Standard_Integer Upper);
//extern void ShallowDump (const Standard_OId Value, Standard_OStream& s);

const Handle_Standard_Type& Standard_OId_Type_();

#endif 
