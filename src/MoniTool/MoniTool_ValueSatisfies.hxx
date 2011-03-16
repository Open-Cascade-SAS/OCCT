// File:	MoniTool_ValueSatisfies.hxx
// Created:	Mon Feb 28 18:26:00 2000
// Author:	data exchange team
//		<det@kinox>


#ifndef MoniTool_ValueSatisfies_HeaderFile
#define MoniTool_ValueSatisfies_HeaderFile

#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

typedef Standard_Boolean (*MoniTool_ValueSatisfies) (const Handle(TCollection_HAsciiString)& val);

Standard_EXPORT Handle(Standard_Type)& STANDARD_TYPE(MoniTool_ValueSatisfies);

#endif
