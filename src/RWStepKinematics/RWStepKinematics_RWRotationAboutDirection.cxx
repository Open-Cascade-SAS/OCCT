// Created on : Sat May 02 12:41:14 2020 
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

#include <RWStepKinematics_RWRotationAboutDirection.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_RotationAboutDirection.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepGeom_Direction.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepKinematics_RWRotationAboutDirection
//purpose  :
//=======================================================================
RWStepKinematics_RWRotationAboutDirection::RWStepKinematics_RWRotationAboutDirection() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWRotationAboutDirection::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                          const Standard_Integer theNum,
                                                          Handle(Interface_Check)& theArch,
                                                          const Handle(StepKinematics_RotationAboutDirection)& theEnt) const
{
  // Check number of parameters
  if ( ! theData->CheckNbParams(theNum,3,theArch,"rotation_about_direction") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString (theNum, 1, "representation_item.name", theArch, aRepresentationItem_Name);

  // Own fields of RotationAboutDirection

  Handle(StepGeom_Direction) aDirectionOfAxis;
  theData->ReadEntity (theNum, 2, "direction_of_axis", theArch, STANDARD_TYPE(StepGeom_Direction), aDirectionOfAxis);

  Standard_Real aRotationAngle;
  theData->ReadReal (theNum, 3, "rotation_angle", theArch, aRotationAngle);

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
            aDirectionOfAxis,
            aRotationAngle);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWRotationAboutDirection::WriteStep (StepData_StepWriter& theSW,
                                                           const Handle(StepKinematics_RotationAboutDirection)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send (theEnt->Name());

  // Own fields of RotationAboutDirection

  theSW.Send (theEnt->DirectionOfAxis());

  theSW.Send (theEnt->RotationAngle());
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================
void RWStepKinematics_RWRotationAboutDirection::Share (const Handle(StepKinematics_RotationAboutDirection)& theEnt,
                                                       Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of RotationAboutDirection

  iter.AddItem (theEnt->DirectionOfAxis());
}
