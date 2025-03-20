// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepElement_AnalysisItemWithinRepresentation.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_AnalysisItemWithinRepresentation, Standard_Transient)

//=================================================================================================

StepElement_AnalysisItemWithinRepresentation::StepElement_AnalysisItemWithinRepresentation() {}

//=================================================================================================

void StepElement_AnalysisItemWithinRepresentation::Init(
  const Handle(TCollection_HAsciiString)&    aName,
  const Handle(TCollection_HAsciiString)&    aDescription,
  const Handle(StepRepr_RepresentationItem)& aItem,
  const Handle(StepRepr_Representation)&     aRep)
{

  theName = aName;

  theDescription = aDescription;

  theItem = aItem;

  theRep = aRep;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepElement_AnalysisItemWithinRepresentation::Name() const
{
  return theName;
}

//=================================================================================================

void StepElement_AnalysisItemWithinRepresentation::SetName(
  const Handle(TCollection_HAsciiString)& aName)
{
  theName = aName;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepElement_AnalysisItemWithinRepresentation::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepElement_AnalysisItemWithinRepresentation::SetDescription(
  const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

Handle(StepRepr_RepresentationItem) StepElement_AnalysisItemWithinRepresentation::Item() const
{
  return theItem;
}

//=================================================================================================

void StepElement_AnalysisItemWithinRepresentation::SetItem(
  const Handle(StepRepr_RepresentationItem)& aItem)
{
  theItem = aItem;
}

//=================================================================================================

Handle(StepRepr_Representation) StepElement_AnalysisItemWithinRepresentation::Rep() const
{
  return theRep;
}

//=================================================================================================

void StepElement_AnalysisItemWithinRepresentation::SetRep(
  const Handle(StepRepr_Representation)& aRep)
{
  theRep = aRep;
}
