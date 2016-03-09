// Created on: 2016-03-30
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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


#include <Standard_Type.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepBasic_ProductDefinitionReferenceWithLocalRepresentation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductDefinitionReferenceWithLocalRepresentation, StepBasic_ProductDefinition)

//=======================================================================
//function : StepBasic_ProductDefinitionReferenceWithLocalRepresentation
//purpose  : 
//=======================================================================
StepBasic_ProductDefinitionReferenceWithLocalRepresentation::StepBasic_ProductDefinitionReferenceWithLocalRepresentation ()  {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void StepBasic_ProductDefinitionReferenceWithLocalRepresentation::Init(
  const Handle(StepBasic_ExternalSource)& theSource,
  const Handle(TCollection_HAsciiString)& theId,
  const Handle(TCollection_HAsciiString)& theDescription,
  const Handle(StepBasic_ProductDefinitionFormation)& theFormation,
  const Handle(StepBasic_ProductDefinitionContext)& theFrameOfReference)
{
  StepBasic_ProductDefinition::Init(theId, theDescription, theFormation, theFrameOfReference);
  mySource = theSource;
}
