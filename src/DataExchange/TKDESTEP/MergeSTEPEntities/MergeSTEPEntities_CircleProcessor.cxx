// Copyright (c) 2025 OPEN CASCADE SAS
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

// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <MergeSTEPEntities_CircleProcessor.hxx>

#include <Interface_Graph.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepGeom_SeamCurve.hxx>

//==================================================================================================

MergeSTEPEntities_CircleProcessor::MergeSTEPEntities_CircleProcessor(
  const Handle(XSControl_WorkSession)& theWS)
    : MergeSTEPEntities_EntityProcessor<StepGeom_Circle, MergeSTEPEntities_CircleHasher>(theWS)
{
  registerReplacer(STANDARD_TYPE(StepShape_EdgeCurve), replaceEdgeCurve);
  registerReplacer(STANDARD_TYPE(StepGeom_SurfaceCurve), replaceSurfaceCurve);
  registerReplacer(STANDARD_TYPE(StepGeom_SeamCurve), replaceSeamCurve);
}

//==================================================================================================

bool MergeSTEPEntities_CircleProcessor::replaceEdgeCurve(
  const Handle(StepGeom_Circle)& theOldEntity,
  const Handle(StepGeom_Circle)& theNewEntity,
  Handle(Standard_Transient)     theSharing)
{
  Handle(StepShape_EdgeCurve) aSharing = Handle(StepShape_EdgeCurve)::DownCast(theSharing);
  if (aSharing->EdgeGeometry() == theOldEntity)
  {
    aSharing->SetEdgeGeometry(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool MergeSTEPEntities_CircleProcessor::replaceSurfaceCurve(
  const Handle(StepGeom_Circle)& theOldEntity,
  const Handle(StepGeom_Circle)& theNewEntity,
  Handle(Standard_Transient)     theSharing)
{
  Handle(StepGeom_SurfaceCurve) aSharing = Handle(StepGeom_SurfaceCurve)::DownCast(theSharing);
  if (aSharing->Curve3d() == theOldEntity)
  {
    aSharing->SetCurve3d(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool MergeSTEPEntities_CircleProcessor::replaceSeamCurve(
  const Handle(StepGeom_Circle)& theOldEntity,
  const Handle(StepGeom_Circle)& theNewEntity,
  Handle(Standard_Transient)     theSharing)
{
  Handle(StepGeom_SeamCurve) aSharing = Handle(StepGeom_SeamCurve)::DownCast(theSharing);
  if (aSharing->Curve3d() == theOldEntity)
  {
    aSharing->SetCurve3d(theNewEntity);
    return true;
  }
  return false;
}
