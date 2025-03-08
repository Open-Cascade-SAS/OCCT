// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepRepr_ConfigurationItem.hxx>
#include <StepRepr_ProductConcept.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ConfigurationItem, Standard_Transient)

//=================================================================================================

StepRepr_ConfigurationItem::StepRepr_ConfigurationItem()
{
  defDescription = Standard_False;
  defPurpose     = Standard_False;
}

//=================================================================================================

void StepRepr_ConfigurationItem::Init(const Handle(TCollection_HAsciiString)& aId,
                                      const Handle(TCollection_HAsciiString)& aName,
                                      const Standard_Boolean                  hasDescription,
                                      const Handle(TCollection_HAsciiString)& aDescription,
                                      const Handle(StepRepr_ProductConcept)&  aItemConcept,
                                      const Standard_Boolean                  hasPurpose,
                                      const Handle(TCollection_HAsciiString)& aPurpose)
{

  theId = aId;

  theName = aName;

  defDescription = hasDescription;
  if (defDescription)
  {
    theDescription = aDescription;
  }
  else
    theDescription.Nullify();

  theItemConcept = aItemConcept;

  defPurpose = hasPurpose;
  if (defPurpose)
  {
    thePurpose = aPurpose;
  }
  else
    thePurpose.Nullify();
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepRepr_ConfigurationItem::Id() const
{
  return theId;
}

//=================================================================================================

void StepRepr_ConfigurationItem::SetId(const Handle(TCollection_HAsciiString)& aId)
{
  theId = aId;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepRepr_ConfigurationItem::Name() const
{
  return theName;
}

//=================================================================================================

void StepRepr_ConfigurationItem::SetName(const Handle(TCollection_HAsciiString)& aName)
{
  theName = aName;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepRepr_ConfigurationItem::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepRepr_ConfigurationItem::SetDescription(
  const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

Standard_Boolean StepRepr_ConfigurationItem::HasDescription() const
{
  return defDescription;
}

//=================================================================================================

Handle(StepRepr_ProductConcept) StepRepr_ConfigurationItem::ItemConcept() const
{
  return theItemConcept;
}

//=================================================================================================

void StepRepr_ConfigurationItem::SetItemConcept(const Handle(StepRepr_ProductConcept)& aItemConcept)
{
  theItemConcept = aItemConcept;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepRepr_ConfigurationItem::Purpose() const
{
  return thePurpose;
}

//=================================================================================================

void StepRepr_ConfigurationItem::SetPurpose(const Handle(TCollection_HAsciiString)& aPurpose)
{
  thePurpose = aPurpose;
}

//=================================================================================================

Standard_Boolean StepRepr_ConfigurationItem::HasPurpose() const
{
  return defPurpose;
}
