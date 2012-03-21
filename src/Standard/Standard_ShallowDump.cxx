// Copyright (c) 1998-1999 Matra Datavision
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

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

#include <Standard_TypeMismatch.hxx>
#include <Standard_Persistent.hxx>

// now with these two proto, we don't need the include of Standard_Persistent
class Handle(Standard_Persistent);
const Handle_Standard_Type &Standard_Persistent_Type_();

//============================================================================
void ShallowDump(const Handle(Standard_Persistent)& me, Standard_OStream &aOut)
{
  Handle(Standard_Type) aType = me->DynamicType();

  aOut << "class " << aType->Name() << endl;
}

//============================================================================
void ShallowDump(const Handle(Standard_Transient)& me, Standard_OStream &aOut)
{
  Handle(Standard_Type) aType = me->DynamicType();

  aOut << "class " << aType->Name() << endl;
}

