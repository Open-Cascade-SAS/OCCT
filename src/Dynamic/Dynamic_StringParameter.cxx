// File:	Dynamic_StringParameter.cxx
// Created:	Wed Feb  3 15:54:02 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


#include <Dynamic_StringParameter.ixx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : Dynamic_StringParameter
//purpose  : 
//=======================================================================

Dynamic_StringParameter::Dynamic_StringParameter(const Standard_CString aparameter)
: Dynamic_Parameter(aparameter)
{}

//=======================================================================
//function : Dynamic_StringParameter
//purpose  : 
//=======================================================================

Dynamic_StringParameter::Dynamic_StringParameter(const Standard_CString aparameter,
						 const Standard_CString avalue)
: Dynamic_Parameter(aparameter)
{
  thestring = new TCollection_HAsciiString(avalue);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

TCollection_AsciiString Dynamic_StringParameter::Value() const
{
  return thestring->String();
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void Dynamic_StringParameter::Value(const Standard_CString avalue)
{
  thestring = new TCollection_HAsciiString(avalue);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_StringParameter::Dump(Standard_OStream& astream) const
{
  Dynamic_Parameter::Dump(astream);
  TCollection_AsciiString string = thestring->String();
  astream<<" "<<string.ToCString();
}
