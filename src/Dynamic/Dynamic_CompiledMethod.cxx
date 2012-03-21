// Created on: 1993-01-28
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
