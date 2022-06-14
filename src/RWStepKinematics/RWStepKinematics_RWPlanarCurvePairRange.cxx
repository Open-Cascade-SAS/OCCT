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

#include <RWStepKinematics_RWPlanarCurvePairRange.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_PlanarCurvePairRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_TrimmedCurve.hxx>

//=======================================================================
//function : RWStepKinematics_RWPlanarCurvePairRange
//purpose  :
//=======================================================================
RWStepKinematics_RWPlanarCurvePairRange::RWStepKinematics_RWPlanarCurvePairRange() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWPlanarCurvePairRange::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                        const Standard_Integer theNum,
                                                        Handle(Interface_Check)& theArch,
                                                        const Handle(StepKinematics_PlanarCurvePairRange)& theEnt) const
{
  // Check number of parameters
  if ( ! theData->CheckNbParams(theNum,11,theArch,"planar_curve_pair_range") ) return;

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

  // Inherited fields of PlanarCurvePair

  Handle(StepGeom_Curve) aPlanarCurvePair_Curve1;
  theData->ReadEntity (theNum, 7, "planar_curve_pair.curve1", theArch, STANDARD_TYPE(StepGeom_Curve), aPlanarCurvePair_Curve1);

  Handle(StepGeom_Curve) aPlanarCurvePair_Curve2;
  theData->ReadEntity (theNum, 8, "planar_curve_pair.curve2", theArch, STANDARD_TYPE(StepGeom_Curve), aPlanarCurvePair_Curve2);

  Standard_Boolean aPlanarCurvePair_Orientation;
  theData->ReadBoolean (theNum, 9, "planar_curve_pair.orientation", theArch, aPlanarCurvePair_Orientation);

  // Own fields of PlanarCurvePairRange

  Handle(StepGeom_TrimmedCurve) aRangeOnCurve1;
  theData->ReadEntity (theNum, 10, "range_on_curve1", theArch, STANDARD_TYPE(StepGeom_TrimmedCurve), aRangeOnCurve1);

  Handle(StepGeom_TrimmedCurve) aRangeOnCurve2;
  theData->ReadEntity (theNum, 11, "range_on_curve2", theArch, STANDARD_TYPE(StepGeom_TrimmedCurve), aRangeOnCurve2);

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aPlanarCurvePair_Curve1,
            aPlanarCurvePair_Curve2,
            aPlanarCurvePair_Orientation,
            aRangeOnCurve1,
            aRangeOnCurve2);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWPlanarCurvePairRange::WriteStep (StepData_StepWriter& theSW,
                                                         const Handle(StepKinematics_PlanarCurvePairRange)& theEnt) const
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

  // Own fields of PlanarCurvePair

  theSW.Send (theEnt->Curve1());

  theSW.Send (theEnt->Curve2());

  theSW.SendBoolean (theEnt->Orientation());

  // Own fields of PlanarCurvePairRange

  theSW.Send (theEnt->RangeOnCurve1());

  theSW.Send (theEnt->RangeOnCurve2());
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================
void RWStepKinematics_RWPlanarCurvePairRange::Share (const Handle(StepKinematics_PlanarCurvePairRange)& theEnt,
                                                     Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem (theEnt->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem (theEnt->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem (theEnt->StepKinematics_KinematicPair::Joint());

  // Inherited fields of PlanarCurvePair

  iter.AddItem (theEnt->StepKinematics_PlanarCurvePair::Curve1());

  iter.AddItem (theEnt->StepKinematics_PlanarCurvePair::Curve2());

  // Own fields of PlanarCurvePairRange

  iter.AddItem (theEnt->RangeOnCurve1());

  iter.AddItem (theEnt->RangeOnCurve2());
}
