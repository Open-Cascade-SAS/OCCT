// Created on: 2001-12-28
// Created by: Andrey BETENEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Standard_Type.hxx>
#include <StepShape_ConnectedEdgeSet.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_ConnectedEdgeSet,StepShape_TopologicalRepresentationItem)

//=======================================================================
//function : StepShape_ConnectedEdgeSet
//purpose  : 
//=======================================================================
StepShape_ConnectedEdgeSet::StepShape_ConnectedEdgeSet ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_ConnectedEdgeSet::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                       const Handle(StepShape_HArray1OfEdge) &aCesEdges)
{
  StepShape_TopologicalRepresentationItem::Init(aRepresentationItem_Name);

  theCesEdges = aCesEdges;
}

//=======================================================================
//function : CesEdges
//purpose  : 
//=======================================================================

Handle(StepShape_HArray1OfEdge) StepShape_ConnectedEdgeSet::CesEdges () const
{
  return theCesEdges;
}

//=======================================================================
//function : SetCesEdges
//purpose  : 
//=======================================================================

void StepShape_ConnectedEdgeSet::SetCesEdges (const Handle(StepShape_HArray1OfEdge) &aCesEdges)
{
  theCesEdges = aCesEdges;
}
