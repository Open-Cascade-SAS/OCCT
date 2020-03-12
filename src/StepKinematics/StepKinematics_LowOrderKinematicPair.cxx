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

#include <StepKinematics_LowOrderKinematicPair.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_LowOrderKinematicPair, StepKinematics_KinematicPair)

//=======================================================================
//function : StepKinematics_LowOrderKinematicPair
//purpose  :
//=======================================================================
StepKinematics_LowOrderKinematicPair::StepKinematics_LowOrderKinematicPair ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPair::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                                 const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                                                 const Standard_Boolean hasItemDefinedTransformation_Description,
                                                 const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                                                 const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                                                 const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                                                 const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                                                 const Standard_Boolean theTX,
                                                 const Standard_Boolean theTY,
                                                 const Standard_Boolean theTZ,
                                                 const Standard_Boolean theRX,
                                                 const Standard_Boolean theRY,
                                                 const Standard_Boolean theRZ)
{
  StepKinematics_KinematicPair::Init(theRepresentationItem_Name,
                                     theItemDefinedTransformation_Name,
                                     hasItemDefinedTransformation_Description,
                                     theItemDefinedTransformation_Description,
                                     theItemDefinedTransformation_TransformItem1,
                                     theItemDefinedTransformation_TransformItem2,
                                     theKinematicPair_Joint);

  myTX = theTX;

  myTY = theTY;

  myTZ = theTZ;

  myRX = theRX;

  myRY = theRY;

  myRZ = theRZ;
}

//=======================================================================
//function : TX
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPair::TX () const
{
  return myTX;
}

//=======================================================================
//function : SetTX
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPair::SetTX (const Standard_Boolean theTX)
{
  myTX = theTX;
}

//=======================================================================
//function : TY
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPair::TY () const
{
  return myTY;
}

//=======================================================================
//function : SetTY
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPair::SetTY (const Standard_Boolean theTY)
{
  myTY = theTY;
}

//=======================================================================
//function : TZ
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPair::TZ () const
{
  return myTZ;
}

//=======================================================================
//function : SetTZ
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPair::SetTZ (const Standard_Boolean theTZ)
{
  myTZ = theTZ;
}

//=======================================================================
//function : RX
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPair::RX () const
{
  return myRX;
}

//=======================================================================
//function : SetRX
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPair::SetRX (const Standard_Boolean theRX)
{
  myRX = theRX;
}

//=======================================================================
//function : RY
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPair::RY () const
{
  return myRY;
}

//=======================================================================
//function : SetRY
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPair::SetRY (const Standard_Boolean theRY)
{
  myRY = theRY;
}

//=======================================================================
//function : RZ
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPair::RZ () const
{
  return myRZ;
}

//=======================================================================
//function : SetRZ
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPair::SetRZ (const Standard_Boolean theRZ)
{
  myRZ = theRZ;
}
