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

#include <StepTidy_CircleReducer.pxx>

#include <Interface_Graph.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepGeom_SeamCurve.hxx>

//==================================================================================================

StepTidy_CircleReducer::StepTidy_CircleReducer(const occ::handle<XSControl_WorkSession>& theWS)
    : StepTidy_EntityReducer<StepGeom_Circle, StepTidy_CircleHasher>(theWS)
{
  registerReplacer(STANDARD_TYPE(StepShape_EdgeCurve), replaceEdgeCurve);
  registerReplacer(STANDARD_TYPE(StepGeom_SurfaceCurve), replaceSurfaceCurve);
  registerReplacer(STANDARD_TYPE(StepGeom_SeamCurve), replaceSeamCurve);
}

//==================================================================================================

bool StepTidy_CircleReducer::replaceEdgeCurve(const occ::handle<StepGeom_Circle>& theOldEntity,
                                              const occ::handle<StepGeom_Circle>& theNewEntity,
                                              const occ::handle<Standard_Transient>&     theSharing)
{
  occ::handle<StepShape_EdgeCurve> aSharing = occ::down_cast<StepShape_EdgeCurve>(theSharing);
  if (aSharing->EdgeGeometry() == theOldEntity)
  {
    aSharing->SetEdgeGeometry(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_CircleReducer::replaceSurfaceCurve(const occ::handle<StepGeom_Circle>& theOldEntity,
                                                 const occ::handle<StepGeom_Circle>& theNewEntity,
                                                 const occ::handle<Standard_Transient>&     theSharing)
{
  occ::handle<StepGeom_SurfaceCurve> aSharing = occ::down_cast<StepGeom_SurfaceCurve>(theSharing);
  if (aSharing->Curve3d() == theOldEntity)
  {
    aSharing->SetCurve3d(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_CircleReducer::replaceSeamCurve(const occ::handle<StepGeom_Circle>& theOldEntity,
                                              const occ::handle<StepGeom_Circle>& theNewEntity,
                                              const occ::handle<Standard_Transient>&     theSharing)
{
  occ::handle<StepGeom_SeamCurve> aSharing = occ::down_cast<StepGeom_SeamCurve>(theSharing);
  if (aSharing->Curve3d() == theOldEntity)
  {
    aSharing->SetCurve3d(theNewEntity);
    return true;
  }
  return false;
}
