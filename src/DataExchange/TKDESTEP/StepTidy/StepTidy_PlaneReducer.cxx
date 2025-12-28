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

#include <StepTidy_PlaneReducer.pxx>

#include <StepShape_AdvancedFace.hxx>
#include <StepGeom_Pcurve.hxx>

//==================================================================================================

StepTidy_PlaneReducer::StepTidy_PlaneReducer(const occ::handle<XSControl_WorkSession>& theWS)
    : StepTidy_EntityReducer<StepGeom_Plane, StepTidy_PlaneHasher>(theWS)
{
  registerReplacer(STANDARD_TYPE(StepShape_AdvancedFace), replaceAdvancedFace);
  registerReplacer(STANDARD_TYPE(StepGeom_Pcurve), replacePcurve);
}

//==================================================================================================

bool StepTidy_PlaneReducer::replaceAdvancedFace(const occ::handle<StepGeom_Plane>& theOldEntity,
                                                const occ::handle<StepGeom_Plane>& theNewEntity,
                                                const occ::handle<Standard_Transient>&    theSharing)
{
  occ::handle<StepShape_AdvancedFace> aSharing = occ::down_cast<StepShape_AdvancedFace>(theSharing);
  if (aSharing->FaceGeometry() == theOldEntity)
  {
    aSharing->SetFaceGeometry(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================
bool StepTidy_PlaneReducer::replacePcurve(const occ::handle<StepGeom_Plane>& theOldEntity,
                                          const occ::handle<StepGeom_Plane>& theNewEntity,
                                          const occ::handle<Standard_Transient>&    theSharing)
{
  occ::handle<StepGeom_Pcurve> aSharing = occ::down_cast<StepGeom_Pcurve>(theSharing);
  if (aSharing->BasisSurface() == theOldEntity)
  {
    aSharing->SetBasisSurface(theNewEntity);
    return true;
  }
  return false;
}
