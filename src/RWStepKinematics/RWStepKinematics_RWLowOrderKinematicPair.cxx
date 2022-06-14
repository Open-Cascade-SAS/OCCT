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

#include <RWStepKinematics_RWLowOrderKinematicPair.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_LowOrderKinematicPair.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>

//=======================================================================
//function : RWStepKinematics_RWLowOrderKinematicPair
//purpose  :
//=======================================================================
RWStepKinematics_RWLowOrderKinematicPair::RWStepKinematics_RWLowOrderKinematicPair() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWLowOrderKinematicPair::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                         const Standard_Integer theNum,
                                                         Handle(Interface_Check)& theArch,
                                                         const Handle(StepKinematics_LowOrderKinematicPair)& theEnt) const
{
  // Check number of parameters
  if ( ! theData->CheckNbParams(theNum,12,theArch,"low_order_kinematic_pair") ) return;

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

  // Own fields of LowOrderKinematicPair

  Standard_Boolean aTX;
  theData->ReadBoolean (theNum, 7, "t_x", theArch, aTX);

  Standard_Boolean aTY;
  theData->ReadBoolean (theNum, 8, "t_y", theArch, aTY);

  Standard_Boolean aTZ;
  theData->ReadBoolean (theNum, 9, "t_z", theArch, aTZ);

  Standard_Boolean aRX;
  theData->ReadBoolean (theNum, 10, "r_x", theArch, aRX);

  Standard_Boolean aRY;
  theData->ReadBoolean (theNum, 11, "r_y", theArch, aRY);

  Standard_Boolean aRZ;
  theData->ReadBoolean (theNum, 12, "r_z", theArch, aRZ);

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aTX,
            aTY,
            aTZ,
            aRX,
            aRY,
            aRZ);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWLowOrderKinematicPair::WriteStep (StepData_StepWriter& theSW,
                                                          const Handle(StepKinematics_LowOrderKinematicPair)& theEnt) const
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

  // Own fields of LowOrderKinematicPair

  theSW.SendBoolean (theEnt->TX());

  theSW.SendBoolean (theEnt->TY());

  theSW.SendBoolean (theEnt->TZ());

  theSW.SendBoolean (theEnt->RX());

  theSW.SendBoolean (theEnt->RY());

  theSW.SendBoolean (theEnt->RZ());
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================
void RWStepKinematics_RWLowOrderKinematicPair::Share (const Handle(StepKinematics_LowOrderKinematicPair)& theEnt,
                                                      Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem (theEnt->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem (theEnt->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem (theEnt->StepKinematics_KinematicPair::Joint());

  // Own fields of LowOrderKinematicPair
}
