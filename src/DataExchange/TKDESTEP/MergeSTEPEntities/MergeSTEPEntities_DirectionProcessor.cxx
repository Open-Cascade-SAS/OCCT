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

#include <MergeSTEPEntities_DirectionProcessor.hxx>

#include <Interface_Graph.hxx>
#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_Vector.hxx>

//==================================================================================================

MergeSTEPEntities_DirectionProcessor::MergeSTEPEntities_DirectionProcessor(
  const Handle(XSControl_WorkSession)& theWS)
    : MergeSTEPEntities_EntityProcessor<StepGeom_Direction, MergeSTEPEntities_DirectionHasher>(
        theWS)
{
  registerReplacer(STANDARD_TYPE(StepGeom_Axis1Placement), replaceAxis1Placement);
  registerReplacer(STANDARD_TYPE(StepGeom_Axis2Placement3d), replaceAxis2Placement3d);
  registerReplacer(STANDARD_TYPE(StepGeom_Vector), replaceVector);
}

//==================================================================================================

bool MergeSTEPEntities_DirectionProcessor::replaceAxis1Placement(
  const Handle(StepGeom_Direction)& theOldEntity,
  const Handle(StepGeom_Direction)& theNewEntity,
  Handle(Standard_Transient)        theSharing)
{
  Handle(StepGeom_Axis1Placement) aSharing = Handle(StepGeom_Axis1Placement)::DownCast(theSharing);
  if (aSharing->Axis() == theOldEntity)
  {
    aSharing->SetAxis(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool MergeSTEPEntities_DirectionProcessor::replaceAxis2Placement3d(
  const Handle(StepGeom_Direction)& theOldEntity,
  const Handle(StepGeom_Direction)& theNewEntity,
  Handle(Standard_Transient)        theSharing)
{
  Handle(StepGeom_Axis2Placement3d) aSharing =
    Handle(StepGeom_Axis2Placement3d)::DownCast(theSharing);
  if (aSharing->Axis() == theOldEntity)
  {
    aSharing->SetAxis(theNewEntity);
    return true;
  }
  else if (aSharing->RefDirection() == theOldEntity)
  {
    aSharing->SetRefDirection(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool MergeSTEPEntities_DirectionProcessor::replaceVector(
  const Handle(StepGeom_Direction)& theOldEntity,
  const Handle(StepGeom_Direction)& theNewEntity,
  Handle(Standard_Transient)        theSharing)
{
  Handle(StepGeom_Vector) aSharing = Handle(StepGeom_Vector)::DownCast(theSharing);
  if (aSharing->Orientation() == theOldEntity)
  {
    aSharing->SetOrientation(theNewEntity);
    return true;
  }
  return false;
}
