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

StepTidy_LineReducer::StepTidy_LineReducer(const occ::handle<XSControl_WorkSession>& theWS)
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

bool StepTidy_LineReducer::replaceEdgeCurve(const occ::handle<StepGeom_Line>& theOldEntity,
                                            const occ::handle<StepGeom_Line>& theNewEntity,
                                            occ::handle<Standard_Transient>   theSharing)
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

bool StepTidy_LineReducer::replaceTrimmedCurve(const occ::handle<StepGeom_Line>& theOldEntity,
                                               const occ::handle<StepGeom_Line>& theNewEntity,
                                               occ::handle<Standard_Transient>   theSharing)
{
  occ::handle<StepGeom_TrimmedCurve> aSharing = occ::down_cast<StepGeom_TrimmedCurve>(theSharing);
  if (aSharing->BasisCurve() == theOldEntity)
  {
    aSharing->SetBasisCurve(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_LineReducer::replaceSurfaceCurve(const occ::handle<StepGeom_Line>& theOldEntity,
                                               const occ::handle<StepGeom_Line>& theNewEntity,
                                               occ::handle<Standard_Transient>   theSharing)
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

bool StepTidy_LineReducer::replaceDefinitionalRepresentation(
  const occ::handle<StepGeom_Line>& theOldEntity,
  const occ::handle<StepGeom_Line>& theNewEntity,
  occ::handle<Standard_Transient>   theSharing)
{
  occ::handle<StepRepr_DefinitionalRepresentation> aSharing =
    occ::down_cast<StepRepr_DefinitionalRepresentation>(theSharing);
  bool                                         isReplaced = false;
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anItems    = aSharing->Items();
  for (int anIndex = 1; anIndex <= aSharing->NbItems(); ++anIndex)
  {
    const occ::handle<StepRepr_RepresentationItem> aRepItem = anItems->Value(anIndex);
    if (aRepItem == theOldEntity)
    {
      anItems->SetValue(anIndex, theNewEntity);
      isReplaced = true;
    }
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_LineReducer::replaceSeamCurve(const occ::handle<StepGeom_Line>& theOldEntity,
                                            const occ::handle<StepGeom_Line>& theNewEntity,
                                            occ::handle<Standard_Transient>   theSharing)
{
  occ::handle<StepGeom_SeamCurve> aSharing = occ::down_cast<StepGeom_SeamCurve>(theSharing);
  if (aSharing->Curve3d() == theOldEntity)
  {
    aSharing->SetCurve3d(theNewEntity);
    return true;
  }
  return false;
}
