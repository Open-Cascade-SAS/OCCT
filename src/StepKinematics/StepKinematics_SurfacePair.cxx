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

#include <StepKinematics_SurfacePair.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_SurfacePair, StepKinematics_HighOrderKinematicPair)

//=======================================================================
//function : StepKinematics_SurfacePair
//purpose  :
//=======================================================================
StepKinematics_SurfacePair::StepKinematics_SurfacePair ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_SurfacePair::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                       const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                                       const Standard_Boolean hasItemDefinedTransformation_Description,
                                       const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                                       const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                                       const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                                       const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                                       const Handle(StepGeom_Surface)& theSurface1,
                                       const Handle(StepGeom_Surface)& theSurface2,
                                       const Standard_Boolean theOrientation)
{
  StepKinematics_HighOrderKinematicPair::Init(theRepresentationItem_Name,
                                              theItemDefinedTransformation_Name,
                                              hasItemDefinedTransformation_Description,
                                              theItemDefinedTransformation_Description,
                                              theItemDefinedTransformation_TransformItem1,
                                              theItemDefinedTransformation_TransformItem2,
                                              theKinematicPair_Joint);

  mySurface1 = theSurface1;

  mySurface2 = theSurface2;

  myOrientation = theOrientation;
}

//=======================================================================
//function : Surface1
//purpose  :
//=======================================================================
Handle(StepGeom_Surface) StepKinematics_SurfacePair::Surface1 () const
{
  return mySurface1;
}

//=======================================================================
//function : SetSurface1
//purpose  :
//=======================================================================
void StepKinematics_SurfacePair::SetSurface1 (const Handle(StepGeom_Surface)& theSurface1)
{
  mySurface1 = theSurface1;
}

//=======================================================================
//function : Surface2
//purpose  :
//=======================================================================
Handle(StepGeom_Surface) StepKinematics_SurfacePair::Surface2 () const
{
  return mySurface2;
}

//=======================================================================
//function : SetSurface2
//purpose  :
//=======================================================================
void StepKinematics_SurfacePair::SetSurface2 (const Handle(StepGeom_Surface)& theSurface2)
{
  mySurface2 = theSurface2;
}

//=======================================================================
//function : Orientation
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_SurfacePair::Orientation () const
{
  return myOrientation;
}

//=======================================================================
//function : SetOrientation
//purpose  :
//=======================================================================
void StepKinematics_SurfacePair::SetOrientation (const Standard_Boolean theOrientation)
{
  myOrientation = theOrientation;
}
