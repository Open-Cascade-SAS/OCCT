// File:	Dynamic_InstanceParameter.cxx
// Created:	Wed Feb  3 15:54:02 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


#include <Dynamic_InstanceParameter.ixx>

//=======================================================================
//function : Dynamic_InstanceParameter
//purpose  : 
//=======================================================================

Dynamic_InstanceParameter::Dynamic_InstanceParameter
  (const Standard_CString aparameter) : Dynamic_Parameter(aparameter)
{}

//=======================================================================
//function : Dynamic_InstanceParameter
//purpose  : 
//=======================================================================

Dynamic_InstanceParameter::Dynamic_InstanceParameter
  (const Standard_CString aparameter,
   const Handle(Dynamic_DynamicInstance)& avalue)
     : Dynamic_Parameter(aparameter)
{
  thevalue = avalue;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(Dynamic_DynamicInstance) Dynamic_InstanceParameter::Value() const
{
  return thevalue;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void Dynamic_InstanceParameter::Value
  (const Handle(Dynamic_DynamicInstance)& avalue)
{
  thevalue = avalue;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_InstanceParameter::Dump(Standard_OStream& astream) const
{
  Dynamic_Parameter::Dump(astream);
}
