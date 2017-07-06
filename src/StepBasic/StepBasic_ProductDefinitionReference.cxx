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
#include <StepBasic_ProductDefinitionReference.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductDefinitionReference, Standard_Transient)

//=======================================================================
//function : StepBasic_ProductDefinitionReference
//purpose  : 
//=======================================================================
StepBasic_ProductDefinitionReference::StepBasic_ProductDefinitionReference () {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void StepBasic_ProductDefinitionReference::Init (const Handle(StepBasic_ExternalSource)& theSource,
                                                 const Handle(TCollection_HAsciiString)& theProductId,
                                                 const Handle(TCollection_HAsciiString)& theProductDefinitionFormationId,
                                                 const Handle(TCollection_HAsciiString)& theProductDefinitionId,
                                                 const Handle(TCollection_HAsciiString)& theIdOwningOrganizationName)
{
  mySource = theSource;
  myProductId = theProductId;
  myProductDefinitionFormationId = theProductDefinitionFormationId;
  myProductDefinitionId = theProductDefinitionId;
  myIdOwningOrganizationName = theIdOwningOrganizationName;
  hasIdOwningOrganizationName = (!theIdOwningOrganizationName.IsNull());
}
                             
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void StepBasic_ProductDefinitionReference::Init (const Handle(StepBasic_ExternalSource)& theSource,
                                                 const Handle(TCollection_HAsciiString)& theProductId,
                                                 const Handle(TCollection_HAsciiString)& theProductDefinitionFormationId,
                                                 const Handle(TCollection_HAsciiString)& theProductDefinitionId)
{
  mySource = theSource;
  myProductId = theProductId;
  myProductDefinitionFormationId = theProductDefinitionFormationId;
  myProductDefinitionId = theProductDefinitionId;
  hasIdOwningOrganizationName = Standard_False;
}