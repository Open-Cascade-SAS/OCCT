#ifndef _Interface_ValueInterpret_HeaderFile
#define _Interface_ValueInterpret_HeaderFile

#include <Handle_TCollection_HAsciiString.hxx>

typedef Handle(TCollection_HAsciiString)  (*Interface_ValueInterpret) (const Handle(Interface_TypedValue)& typval, const Handle(TCollection_HAsciiString)& val, const Standard_Boolean native);

#endif
