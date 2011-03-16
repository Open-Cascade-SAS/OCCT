// File:	MoniTool_ValueInterpret.cxx
// Created:	Mon Feb 28 18:59:29 2000
// Author:	data exchange team
//		<det@kinox>


#include <MoniTool_ValueInterpret.hxx>

//=======================================================================
//function : MoniTool_ValueInterpret_Type_
//purpose  : 
//=======================================================================

Handle(Standard_Type)& STANDARD_TYPE(MoniTool_ValueInterpret)
{
  static Handle(Standard_Type) _aType = 
    new Standard_Type("MoniTool_ValueInterpret", sizeof(MoniTool_ValueInterpret), 0, NULL);

  return _aType;
}

