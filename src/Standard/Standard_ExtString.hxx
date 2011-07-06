//============================================================================
//==== Titre: Standard_ExtString.hxx
//==== 
//==== Implementation:  This is a primitive type implementadef with typedef
//====        typedef short* Standard_ExtString;
//============================================================================

#ifndef _Standard_ExtString_HeaderFile
#define _Standard_ExtString_HeaderFile


#ifndef _Standard_TypeDef_HeaderFile
#include <Standard_TypeDef.hxx>
#endif

class Handle_Standard_Type;

__Standard_API Handle_Standard_Type& Standard_ExtString_Type_();
//class Standard_OStream;
//void ShallowDump (const Standard_ExtString, Standard_OStream& );
//============================================================================
//==== ShallowCopy: Returns a CString
//============================================================================
inline Standard_ExtString ShallowCopy (const Standard_ExtString Value)
{
  return Value;
}

//============================================================================
//==== HashCode : Returns a HashCode ExtString
//============================================================================
__Standard_API Standard_Integer HashCode (const Standard_ExtString, const Standard_Integer);

#endif
