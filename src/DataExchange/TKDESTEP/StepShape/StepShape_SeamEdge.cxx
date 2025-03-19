// Created on: 2002-01-04
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <StepGeom_Pcurve.hxx>
#include <StepShape_SeamEdge.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_SeamEdge, StepShape_OrientedEdge)

//=================================================================================================

StepShape_SeamEdge::StepShape_SeamEdge() {}

//=================================================================================================

void StepShape_SeamEdge::Init(const Handle(TCollection_HAsciiString)& aRepresentationItem_Name,
                              const Handle(StepShape_Edge)&           aOrientedEdge_EdgeElement,
                              const Standard_Boolean                  aOrientedEdge_Orientation,
                              const Handle(StepGeom_Pcurve)&          aPcurveReference)
{
  StepShape_OrientedEdge::Init(aRepresentationItem_Name,
                               aOrientedEdge_EdgeElement,
                               aOrientedEdge_Orientation);

  thePcurveReference = aPcurveReference;
}

//=================================================================================================

Handle(StepGeom_Pcurve) StepShape_SeamEdge::PcurveReference() const
{
  return thePcurveReference;
}

//=================================================================================================

void StepShape_SeamEdge::SetPcurveReference(const Handle(StepGeom_Pcurve)& aPcurveReference)
{
  thePcurveReference = aPcurveReference;
}
