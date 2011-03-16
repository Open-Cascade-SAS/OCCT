// File:	Dynamic_CompiledMethod.cxx
// Created:	Thu Jan 28 15:07:26 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


#include <Dynamic_CompiledMethod.ixx>
#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : Dynamic_CompiledMethod
//purpose  : 
//=======================================================================

Dynamic_CompiledMethod::Dynamic_CompiledMethod(const Standard_CString aname,
					       const Standard_CString afunction)
     : Dynamic_MethodDefinition(aname)
{
  thefunction = new TCollection_HAsciiString(afunction);
}

//=======================================================================
//function : Function
//purpose  : 
//=======================================================================

void Dynamic_CompiledMethod::Function(const Standard_CString afunction)
{
  thefunction = new TCollection_HAsciiString(afunction);
}

//=======================================================================
//function : Function
//purpose  : 
//=======================================================================

TCollection_AsciiString Dynamic_CompiledMethod::Function() const
{
  return thefunction->String();
}
