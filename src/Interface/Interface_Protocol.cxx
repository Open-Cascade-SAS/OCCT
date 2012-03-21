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

#include <Interface_Protocol.ixx>

//  Gestion du Protocol actif : tres simple, une variable statique


static Handle(Interface_Protocol)& theactive()
{
  static Handle(Interface_Protocol) theact;
  return theact;
}


//=======================================================================
//function : Active
//purpose  : 
//=======================================================================

Handle(Interface_Protocol) Interface_Protocol::Active ()
{
  return theactive();
}


//=======================================================================
//function : SetActive
//purpose  : 
//=======================================================================

void Interface_Protocol::SetActive(const Handle(Interface_Protocol)& aprotocol)
{
  theactive() = aprotocol;
}


//=======================================================================
//function : ClearActive
//purpose  : 
//=======================================================================

void Interface_Protocol::ClearActive ()
{
  theactive().Nullify();
}


//  ===   Typage (formules fournies par defaut)


//=======================================================================
//function : CaseNumber
//purpose  : 
//=======================================================================

Standard_Integer Interface_Protocol::CaseNumber
  (const Handle(Standard_Transient)& obj) const
{
  if (obj.IsNull()) return 0;
  return TypeNumber(Type(obj));
}


//=======================================================================
//function : IsDynamicType
//purpose  : 
//=======================================================================

Standard_Boolean Interface_Protocol::IsDynamicType
  (const Handle(Standard_Transient)& /*obj*/) const
{
  return Standard_True;
}


//=======================================================================
//function : NbTypes
//purpose  : 
//=======================================================================

Standard_Integer Interface_Protocol::NbTypes
  (const Handle(Standard_Transient)& /*obj*/) const
{
  return 1;
}


//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

Handle(Standard_Type) Interface_Protocol::Type
       (const Handle(Standard_Transient)& obj,
        const Standard_Integer /*nt*/) const
{
  if (obj.IsNull()) return STANDARD_TYPE(Standard_Transient);
  return obj->DynamicType();
}


//=======================================================================
//function : GlobalCheck
//purpose  : 
//=======================================================================

Standard_Boolean Interface_Protocol::GlobalCheck(const Interface_Graph& /*graph*/,
                                                 Handle(Interface_Check)& /*ach*/) const
{
  return Standard_True;
}
