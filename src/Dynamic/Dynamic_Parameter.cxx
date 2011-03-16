// File:	Dynamic_Parameter.cxx
// Created:	Thu Jan 28 15:06:03 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


#include <Dynamic_Parameter.ixx>

//=======================================================================
//function : Dynamic_Parameter
//purpose  : 
//=======================================================================

Dynamic_Parameter::Dynamic_Parameter(const Standard_CString aname)
{
  theparametername = new TCollection_HAsciiString(aname);
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================
TCollection_AsciiString Dynamic_Parameter::Name() const
{
  return theparametername->String();
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_Parameter::Dump(Standard_OStream& astream) const
{
  TCollection_AsciiString string = theparametername->String();
  astream << string.ToCString();
}
