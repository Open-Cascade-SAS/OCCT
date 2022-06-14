// Created on : Sat May 02 12:41:16 2020 
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

#include <RWStepKinematics_RWSurfacePairWithRange.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_SurfacePairWithRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_RectangularTrimmedSurface.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepKinematics_RWSurfacePairWithRange
//purpose  :
//=======================================================================
RWStepKinematics_RWSurfacePairWithRange::RWStepKinematics_RWSurfacePairWithRange() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWSurfacePairWithRange::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                        const Standard_Integer theNum,
                                                        Handle(Interface_Check)& theArch,
                                                        const Handle(StepKinematics_SurfacePairWithRange)& theEnt) const
{
  // Check number of parameters
  if ( ! theData->CheckNbParams(theNum,13,theArch,"surface_pair_with_range") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString (theNum, 1, "representation_item.name", theArch, aRepresentationItem_Name);

  // Inherited fields of ItemDefinedTransformation

  Handle(TCollection_HAsciiString) aItemDefinedTransformation_Name;
  theData->ReadString (theNum, 2, "item_defined_transformation.name", theArch, aItemDefinedTransformation_Name);

  Handle(TCollection_HAsciiString) aItemDefinedTransformation_Description;
  Standard_Boolean hasItemDefinedTransformation_Description = Standard_True;
  if ( theData->IsParamDefined (theNum,3) ) {
    theData->ReadString (theNum, 3, "item_defined_transformation.description", theArch, aItemDefinedTransformation_Description);
  }
  else {
    hasItemDefinedTransformation_Description = Standard_False;
    aItemDefinedTransformation_Description.Nullify();
  }

  Handle(StepRepr_RepresentationItem) aItemDefinedTransformation_TransformItem1;
  theData->ReadEntity (theNum, 4, "item_defined_transformation.transform_item1", theArch, STANDARD_TYPE(StepRepr_RepresentationItem), aItemDefinedTransformation_TransformItem1);

  Handle(StepRepr_RepresentationItem) aItemDefinedTransformation_TransformItem2;
  theData->ReadEntity (theNum, 5, "item_defined_transformation.transform_item2", theArch, STANDARD_TYPE(StepRepr_RepresentationItem), aItemDefinedTransformation_TransformItem2);

  // Inherited fields of KinematicPair

  Handle(StepKinematics_KinematicJoint) aKinematicPair_Joint;
  theData->ReadEntity (theNum, 6, "kinematic_pair.joint", theArch, STANDARD_TYPE(StepKinematics_KinematicJoint), aKinematicPair_Joint);

  // Inherited fields of SurfacePair

  Handle(StepGeom_Surface) aSurfacePair_Surface1;
  theData->ReadEntity (theNum, 7, "surface_pair.surface1", theArch, STANDARD_TYPE(StepGeom_Surface), aSurfacePair_Surface1);

  Handle(StepGeom_Surface) aSurfacePair_Surface2;
  theData->ReadEntity (theNum, 8, "surface_pair.surface2", theArch, STANDARD_TYPE(StepGeom_Surface), aSurfacePair_Surface2);

  Standard_Boolean aSurfacePair_Orientation;
  theData->ReadBoolean (theNum, 9, "surface_pair.orientation", theArch, aSurfacePair_Orientation);

  // Own fields of SurfacePairWithRange

  Handle(StepGeom_RectangularTrimmedSurface) aRangeOnSurface1;
  theData->ReadEntity (theNum, 10, "range_on_surface1", theArch, STANDARD_TYPE(StepGeom_RectangularTrimmedSurface), aRangeOnSurface1);

  Handle(StepGeom_RectangularTrimmedSurface) aRangeOnSurface2;
  theData->ReadEntity (theNum, 11, "range_on_surface2", theArch, STANDARD_TYPE(StepGeom_RectangularTrimmedSurface), aRangeOnSurface2);

  Standard_Real aLowerLimitActualRotation;
  Standard_Boolean hasLowerLimitActualRotation = Standard_True;
  if ( theData->IsParamDefined (theNum,12) ) {
    theData->ReadReal (theNum, 12, "lower_limit_actual_rotation", theArch, aLowerLimitActualRotation);
  }
  else {
    hasLowerLimitActualRotation = Standard_False;
    aLowerLimitActualRotation = 0;
  }

  Standard_Real aUpperLimitActualRotation;
  Standard_Boolean hasUpperLimitActualRotation = Standard_True;
  if ( theData->IsParamDefined (theNum,13) ) {
    theData->ReadReal (theNum, 13, "upper_limit_actual_rotation", theArch, aUpperLimitActualRotation);
  }
  else {
    hasUpperLimitActualRotation = Standard_False;
    aUpperLimitActualRotation = 0;
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aSurfacePair_Surface1,
            aSurfacePair_Surface2,
            aSurfacePair_Orientation,
            aRangeOnSurface1,
            aRangeOnSurface2,
            hasLowerLimitActualRotation,
            aLowerLimitActualRotation,
            hasUpperLimitActualRotation,
            aUpperLimitActualRotation);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWSurfacePairWithRange::WriteStep (StepData_StepWriter& theSW,
                                                         const Handle(StepKinematics_SurfacePairWithRange)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send (theEnt->Name());

  // Inherited fields of ItemDefinedTransformation

  theSW.Send (theEnt->ItemDefinedTransformation()->Name());

  if ( theEnt->ItemDefinedTransformation()->HasDescription() ) {
    theSW.Send (theEnt->ItemDefinedTransformation()->Description());
  }
  else theSW.SendUndef();

  theSW.Send (theEnt->ItemDefinedTransformation()->TransformItem1());

  theSW.Send (theEnt->ItemDefinedTransformation()->TransformItem2());

  // Own fields of KinematicPair

  theSW.Send (theEnt->Joint());

  // Own fields of SurfacePair

  theSW.Send (theEnt->Surface1());

  theSW.Send (theEnt->Surface2());

  theSW.SendBoolean (theEnt->Orientation());

  // Own fields of SurfacePairWithRange

  theSW.Send (theEnt->RangeOnSurface1());

  theSW.Send (theEnt->RangeOnSurface2());

  if ( theEnt->HasLowerLimitActualRotation() ) {
    theSW.Send (theEnt->LowerLimitActualRotation());
  }
  else theSW.SendUndef();

  if ( theEnt->HasUpperLimitActualRotation() ) {
    theSW.Send (theEnt->UpperLimitActualRotation());
  }
  else theSW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================
void RWStepKinematics_RWSurfacePairWithRange::Share (const Handle(StepKinematics_SurfacePairWithRange)& theEnt,
                                                     Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem (theEnt->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem (theEnt->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem (theEnt->StepKinematics_KinematicPair::Joint());

  // Inherited fields of SurfacePair

  iter.AddItem (theEnt->StepKinematics_SurfacePair::Surface1());

  iter.AddItem (theEnt->StepKinematics_SurfacePair::Surface2());

  // Own fields of SurfacePairWithRange

  iter.AddItem (theEnt->RangeOnSurface1());

  iter.AddItem (theEnt->RangeOnSurface2());
}
