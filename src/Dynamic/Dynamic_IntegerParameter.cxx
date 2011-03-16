// File:	Dynamic_IntegerParameter.cxx
// Created:	Wed Feb  3 15:54:02 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


#include <Dynamic_IntegerParameter.ixx>

//=======================================================================
//function : Dynamic_IntegerParameter
//purpose  : 
//=======================================================================

Dynamic_IntegerParameter::Dynamic_IntegerParameter
  (const Standard_CString aparameter)
     : Dynamic_Parameter(aparameter)
{}

//=======================================================================
//function : Dynamic_IntegerParameter
//purpose  : 
//=======================================================================

Dynamic_IntegerParameter::Dynamic_IntegerParameter
  (const Standard_CString aparameter,
   const Standard_Integer avalue)
     : Dynamic_Parameter(aparameter)
{
  thevalue = avalue;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Integer Dynamic_IntegerParameter::Value() const
{
  return thevalue;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void Dynamic_IntegerParameter::Value(const Standard_Integer avalue) 
{
  thevalue = avalue;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_IntegerParameter::Dump(Standard_OStream& astream) const
{
  Dynamic_Parameter::Dump(astream);
  astream<<" "<<thevalue;
}
