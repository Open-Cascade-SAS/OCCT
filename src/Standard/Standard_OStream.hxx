#ifndef _Standard_OStream_HeaderFile
#define _Standard_OStream_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_TypeDef.hxx>
#endif
#ifndef _Standard_Stream_HeaderFile
#include <Standard_Stream.hxx>
#endif

class Handle_Standard_Type;

__Standard_API Handle_Standard_Type& Standard_OStream_Type_();

typedef ostream Standard_OStream;

__Standard_API void ShallowDump(const Standard_Boolean, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_CString, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_Character, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_ExtCharacter, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_ExtString, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_Integer, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_Real, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_ShortReal, Standard_OStream& );
class Handle(Standard_Transient);
__Standard_API void ShallowDump(const Handle(Standard_Transient)&, Standard_OStream& );
class Handle(Standard_Persistent);
__Standard_API void ShallowDump(const Handle(Standard_Persistent)&, Standard_OStream& );

#endif


