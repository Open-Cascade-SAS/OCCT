// File:	Dynamic_RealParameter.cxx
// Created:	Wed Feb  3 15:54:02 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


#include <Dynamic_RealParameter.ixx>
#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : Dynamic_RealParameter
//purpose  : 
//=======================================================================

Dynamic_RealParameter::Dynamic_RealParameter
  (const Standard_CString aparameter)
     : Dynamic_Parameter(aparameter)
{}

//=======================================================================
//function : Dynamic_RealParameter
//purpose  : 
//=======================================================================

Dynamic_RealParameter::Dynamic_RealParameter
  (const Standard_CString aparameter,
   const Standard_Real avalue)
     : Dynamic_Parameter(aparameter)
{
  thevalue = avalue;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Dynamic_RealParameter::Value() const
{
  return thevalue;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void Dynamic_RealParameter::Value(const Standard_Real avalue) 
{
  thevalue = avalue;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_RealParameter::Dump(Standard_OStream& astream) const
{
  Dynamic_Parameter::Dump(astream);
  astream<<" : "<<thevalue;
}
