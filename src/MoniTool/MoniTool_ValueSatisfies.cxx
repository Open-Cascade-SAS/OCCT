// File:	MoniTool_ValueSatisfies.cxx
// Created:	Mon Feb 28 18:28:47 2000
// Author:	data exchange team
//		<det@kinox>


#include <MoniTool_ValueSatisfies.hxx>

//=======================================================================
//function : MoniTool_ValueSatisfies_Type_
//purpose  : 
//=======================================================================

Handle(Standard_Type)& STANDARD_TYPE(MoniTool_ValueSatisfies)
{
  static Handle(Standard_Type) _aType = 
    new Standard_Type("MoniTool_ValueSatisfies", sizeof(MoniTool_ValueSatisfies), 0, NULL);

  return _aType;
}

