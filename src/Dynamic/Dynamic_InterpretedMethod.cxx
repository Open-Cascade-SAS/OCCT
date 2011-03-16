// File:	Dynamic_InterpretedMethod.cxx
// Created:	Thu Jan 28 15:07:26 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


#include <Dynamic_InterpretedMethod.ixx>
#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : Dynamic_InterpretedMethod
//purpose  : 
//=======================================================================

Dynamic_InterpretedMethod::Dynamic_InterpretedMethod(const Standard_CString aname,
						     const Standard_CString afile)
: Dynamic_MethodDefinition(aname)
{
  thefunction = new TCollection_HAsciiString(afile);
}

//=======================================================================
//function : Function
//purpose  : 
//=======================================================================

void Dynamic_InterpretedMethod::Function(const Standard_CString afile)
{
  thefunction = new TCollection_HAsciiString(afile);
}

//=======================================================================
//function : Function
//purpose  : 
//=======================================================================

TCollection_AsciiString Dynamic_InterpretedMethod::Function() const
{
  return thefunction->String();
}
