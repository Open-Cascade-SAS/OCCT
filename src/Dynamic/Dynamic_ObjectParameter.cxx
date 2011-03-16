// File:	Dynamic_ObjectParameter.cxx
// Created:	Fri Dec 24 10:31:23 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@mastox>

#include <Dynamic_ObjectParameter.ixx>

//=======================================================================
//function : Dynamic_ObjectParameter
//purpose  : 
//=======================================================================

Dynamic_ObjectParameter::Dynamic_ObjectParameter
  (const Standard_CString aparameter)
: Dynamic_Parameter(aparameter)
{}

//=======================================================================
//function : Dynamic_ObjectParameter
//purpose  : 
//=======================================================================

Dynamic_ObjectParameter::Dynamic_ObjectParameter
  (const Standard_CString aparameter,
   const Handle(Standard_Transient)& anobject)
: Dynamic_Parameter(aparameter)
{
  theobject = anobject;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(Standard_Transient) Dynamic_ObjectParameter::Value() const
{
  return theobject;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void Dynamic_ObjectParameter::Value(const Handle(Standard_Transient)& anobject)
{
  theobject = anobject;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_ObjectParameter::Dump(Standard_OStream& astream) const
{
   Dynamic_Parameter::Dump(astream);
}
