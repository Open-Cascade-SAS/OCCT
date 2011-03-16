// File:	Dynamic_FuzzyDefinition.cxx
// Created:	Fri Jan 22 11:45:20 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


#include <Dynamic_FuzzyDefinition.ixx>
#include <TCollection_AsciiString.hxx>


//=======================================================================
//function : Dynamic_FuzzyDefinition
//purpose  : 
//=======================================================================

Dynamic_FuzzyDefinition::Dynamic_FuzzyDefinition(const Standard_CString aname)
{
  thename = new TCollection_HAsciiString(aname);
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

TCollection_AsciiString Dynamic_FuzzyDefinition::Type() const
{
  return thename->String();
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

//void Dynamic_FuzzyDefinition::Dump(Standard_OStream& astream) const
void Dynamic_FuzzyDefinition::Dump(Standard_OStream& ) const
{}
