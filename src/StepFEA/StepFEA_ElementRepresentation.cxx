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

#include <Standard_Type.hxx>
#include <StepFEA_ElementRepresentation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_ElementRepresentation,StepRepr_Representation)

//=======================================================================
//function : StepFEA_ElementRepresentation
//purpose  : 
//=======================================================================
StepFEA_ElementRepresentation::StepFEA_ElementRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_ElementRepresentation::Init (const Handle(TCollection_HAsciiString) &aRepresentation_Name,
                                          const Handle(StepRepr_HArray1OfRepresentationItem) &aRepresentation_Items,
                                          const Handle(StepRepr_RepresentationContext) &aRepresentation_ContextOfItems,
                                          const Handle(StepFEA_HArray1OfNodeRepresentation) &aNodeList)
{
  StepRepr_Representation::Init(aRepresentation_Name,
                                aRepresentation_Items,
                                aRepresentation_ContextOfItems);

  theNodeList = aNodeList;
}

//=======================================================================
//function : NodeList
//purpose  : 
//=======================================================================

Handle(StepFEA_HArray1OfNodeRepresentation) StepFEA_ElementRepresentation::NodeList () const
{
  return theNodeList;
}

//=======================================================================
//function : SetNodeList
//purpose  : 
//=======================================================================

void StepFEA_ElementRepresentation::SetNodeList (const Handle(StepFEA_HArray1OfNodeRepresentation) &aNodeList)
{
  theNodeList = aNodeList;
}
