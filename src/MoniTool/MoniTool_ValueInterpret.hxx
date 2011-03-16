// File:	MoniTool_ValueInterpret.hxx
// Created:	Mon Feb 28 18:58:03 2000
// Author:	data exchange team
//		<det@kinox>


#ifndef MoniTool_ValueInterpret_HeaderFile
#define MoniTool_ValueInterpret_HeaderFile

#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Handle_MoniTool_TypedValue.hxx>

typedef Handle(TCollection_HAsciiString)  (*MoniTool_ValueInterpret) (const Handle(MoniTool_TypedValue)& typval,
								      const Handle(TCollection_HAsciiString)& val,
								      const Standard_Boolean native);

Standard_EXPORT Handle(Standard_Type)& STANDARD_TYPE(MoniTool_ValueInterpret);

#endif
