// Created on : Sat May 02 12:41:15 2020
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#include "RWStepKinematics_RWPointOnSurfacePairValue.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_PointOnSurfacePairValue.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <StepGeom_PointOnSurface.hxx>
#include <StepKinematics_SpatialRotation.hxx>

//=================================================================================================

RWStepKinematics_RWPointOnSurfacePairValue::RWStepKinematics_RWPointOnSurfacePairValue() {}

//=================================================================================================

void RWStepKinematics_RWPointOnSurfacePairValue::ReadStep(
  const Handle(StepData_StepReaderData)&                theData,
  const Standard_Integer                                theNum,
  Handle(Interface_Check)&                              theArch,
  const Handle(StepKinematics_PointOnSurfacePairValue)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 4, theArch, "point_on_surface_pair_value"))
    return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theArch, aRepresentationItem_Name);

  // Inherited fields of PairValue

  Handle(StepKinematics_KinematicPair) aPairValue_AppliesToPair;
  theData->ReadEntity(theNum,
                      2,
                      "pair_value.applies_to_pair",
                      theArch,
                      STANDARD_TYPE(StepKinematics_KinematicPair),
                      aPairValue_AppliesToPair);

  // Own fields of PointOnSurfacePairValue

  Handle(StepGeom_PointOnSurface) aActualPointOnSurface;
  theData->ReadEntity(theNum,
                      3,
                      "actual_point_on_surface",
                      theArch,
                      STANDARD_TYPE(StepGeom_PointOnSurface),
                      aActualPointOnSurface);

  StepKinematics_SpatialRotation aInputOrientation;
  if (theData->SubListNumber(theNum, 4, Standard_True))
  {
    Handle(TColStd_HArray1OfReal) aItems;
    Standard_Integer              nsub = 0;
    if (theData->ReadSubList(theNum, 4, "items", theArch, nsub))
    {
      Standard_Integer nb   = theData->NbParams(nsub);
      aItems                = new TColStd_HArray1OfReal(1, nb);
      Standard_Integer num2 = nsub;
      for (Standard_Integer i0 = 1; i0 <= nb; i0++)
      {
        Standard_Real anIt0;
        theData->ReadReal(num2, i0, "real", theArch, anIt0);
        aItems->SetValue(i0, anIt0);
      }
    }
    aInputOrientation.SetValue(aItems);
  }
  else
    theData->ReadEntity(theNum, 4, "input_orientation", theArch, aInputOrientation);

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
               aPairValue_AppliesToPair,
               aActualPointOnSurface,
               aInputOrientation);
}

//=================================================================================================

void RWStepKinematics_RWPointOnSurfacePairValue::WriteStep(
  StepData_StepWriter&                                  theSW,
  const Handle(StepKinematics_PointOnSurfacePairValue)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Own fields of PairValue

  theSW.Send(theEnt->AppliesToPair());

  // Own fields of PointOnSurfacePairValue

  theSW.Send(theEnt->ActualPointOnSurface());

  if (!theEnt->InputOrientation().YprRotation().IsNull())
  {
    // Inherited field : YPR
    theSW.OpenSub();
    for (Standard_Integer i = 1; i <= theEnt->InputOrientation().YprRotation()->Length(); i++)
    {
      theSW.Send(theEnt->InputOrientation().YprRotation()->Value(i));
    }
    theSW.CloseSub();
  }
  else
    theSW.Send(theEnt->InputOrientation().Value());
}

//=================================================================================================

void RWStepKinematics_RWPointOnSurfacePairValue::Share(
  const Handle(StepKinematics_PointOnSurfacePairValue)& theEnt,
  Interface_EntityIterator&                             iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of PairValue

  iter.AddItem(theEnt->StepKinematics_PairValue::AppliesToPair());

  // Own fields of PointOnSurfacePairValue

  iter.AddItem(theEnt->ActualPointOnSurface());

  if (!theEnt->InputOrientation().RotationAboutDirection().IsNull())
    iter.AddItem(theEnt->InputOrientation().Value());
}
