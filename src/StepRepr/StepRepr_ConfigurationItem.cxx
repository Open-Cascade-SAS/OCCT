// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepRepr_ConfigurationItem.ixx>

//=======================================================================
//function : StepRepr_ConfigurationItem
//purpose  : 
//=======================================================================

StepRepr_ConfigurationItem::StepRepr_ConfigurationItem ()
{
  defDescription = Standard_False;
  defPurpose = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationItem::Init (const Handle(TCollection_HAsciiString) &aId,
                                       const Handle(TCollection_HAsciiString) &aName,
                                       const Standard_Boolean hasDescription,
                                       const Handle(TCollection_HAsciiString) &aDescription,
                                       const Handle(StepRepr_ProductConcept) &aItemConcept,
                                       const Standard_Boolean hasPurpose,
                                       const Handle(TCollection_HAsciiString) &aPurpose)
{

  theId = aId;

  theName = aName;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();

  theItemConcept = aItemConcept;

  defPurpose = hasPurpose;
  if (defPurpose) {
    thePurpose = aPurpose;
  }
  else thePurpose.Nullify();
}

//=======================================================================
//function : Id
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_ConfigurationItem::Id () const
{
  return theId;
}

//=======================================================================
//function : SetId
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationItem::SetId (const Handle(TCollection_HAsciiString) &aId)
{
  theId = aId;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_ConfigurationItem::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationItem::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_ConfigurationItem::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationItem::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepRepr_ConfigurationItem::HasDescription () const
{
  return defDescription;
}

//=======================================================================
//function : ItemConcept
//purpose  : 
//=======================================================================

Handle(StepRepr_ProductConcept) StepRepr_ConfigurationItem::ItemConcept () const
{
  return theItemConcept;
}

//=======================================================================
//function : SetItemConcept
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationItem::SetItemConcept (const Handle(StepRepr_ProductConcept) &aItemConcept)
{
  theItemConcept = aItemConcept;
}

//=======================================================================
//function : Purpose
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_ConfigurationItem::Purpose () const
{
  return thePurpose;
}

//=======================================================================
//function : SetPurpose
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationItem::SetPurpose (const Handle(TCollection_HAsciiString) &aPurpose)
{
  thePurpose = aPurpose;
}

//=======================================================================
//function : HasPurpose
//purpose  : 
//=======================================================================

Standard_Boolean StepRepr_ConfigurationItem::HasPurpose () const
{
  return defPurpose;
}
