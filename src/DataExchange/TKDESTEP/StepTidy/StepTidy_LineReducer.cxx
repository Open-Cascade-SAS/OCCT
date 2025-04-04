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

#include <StepTidy_LineReducer.pxx>

#include <Interface_Graph.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>
#include <StepGeom_SeamCurve.hxx>

//==================================================================================================

StepTidy_LineReducer::StepTidy_LineReducer(const Handle(XSControl_WorkSession)& theWS)
    : StepTidy_EntityReducer<StepGeom_Line, StepTidy_LineHasher>(theWS)
{
  registerReplacer(STANDARD_TYPE(StepShape_EdgeCurve), replaceEdgeCurve);
  registerReplacer(STANDARD_TYPE(StepGeom_TrimmedCurve), replaceTrimmedCurve);
  registerReplacer(STANDARD_TYPE(StepGeom_SurfaceCurve), replaceSurfaceCurve);
  registerReplacer(STANDARD_TYPE(StepRepr_DefinitionalRepresentation),
                   replaceDefinitionalRepresentation);
  registerReplacer(STANDARD_TYPE(StepGeom_SeamCurve), replaceSeamCurve);
}

//==================================================================================================

bool StepTidy_LineReducer::replaceEdgeCurve(const Handle(StepGeom_Line)& theOldEntity,
                                            const Handle(StepGeom_Line)& theNewEntity,
                                            Handle(Standard_Transient)   theSharing)
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

bool StepTidy_LineReducer::replaceTrimmedCurve(const Handle(StepGeom_Line)& theOldEntity,
                                               const Handle(StepGeom_Line)& theNewEntity,
                                               Handle(Standard_Transient)   theSharing)
{
  Handle(StepGeom_TrimmedCurve) aSharing = Handle(StepGeom_TrimmedCurve)::DownCast(theSharing);
  if (aSharing->BasisCurve() == theOldEntity)
  {
    aSharing->SetBasisCurve(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_LineReducer::replaceSurfaceCurve(const Handle(StepGeom_Line)& theOldEntity,
                                               const Handle(StepGeom_Line)& theNewEntity,
                                               Handle(Standard_Transient)   theSharing)
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

bool StepTidy_LineReducer::replaceDefinitionalRepresentation(
  const Handle(StepGeom_Line)& theOldEntity,
  const Handle(StepGeom_Line)& theNewEntity,
  Handle(Standard_Transient)   theSharing)
{
  Handle(StepRepr_DefinitionalRepresentation) aSharing =
    Handle(StepRepr_DefinitionalRepresentation)::DownCast(theSharing);
  bool                                         isReplaced = false;
  Handle(StepRepr_HArray1OfRepresentationItem) anItems    = aSharing->Items();
  for (Standard_Integer anIndex = 1; anIndex <= aSharing->NbItems(); ++anIndex)
  {
    const Handle(StepRepr_RepresentationItem) aRepItem = anItems->Value(anIndex);
    if (aRepItem == theOldEntity)
    {
      anItems->SetValue(anIndex, theNewEntity);
      isReplaced = true;
    }
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_LineReducer::replaceSeamCurve(const Handle(StepGeom_Line)& theOldEntity,
                                            const Handle(StepGeom_Line)& theNewEntity,
                                            Handle(Standard_Transient)   theSharing)
{
  Handle(StepGeom_SeamCurve) aSharing = Handle(StepGeom_SeamCurve)::DownCast(theSharing);
  if (aSharing->Curve3d() == theOldEntity)
  {
    aSharing->SetCurve3d(theNewEntity);
    return true;
  }
  return false;
}
