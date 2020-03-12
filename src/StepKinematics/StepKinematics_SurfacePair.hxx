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

#ifndef _StepKinematics_SurfacePair_HeaderFile_
#define _StepKinematics_SurfacePair_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepKinematics_HighOrderKinematicPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepGeom_Surface.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_SurfacePair, StepKinematics_HighOrderKinematicPair)

//! Representation of STEP entity SurfacePair
class StepKinematics_SurfacePair : public StepKinematics_HighOrderKinematicPair
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_SurfacePair();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                           const Standard_Boolean hasItemDefinedTransformation_Description,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                           const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                           const Handle(StepGeom_Surface)& theSurface1,
                           const Handle(StepGeom_Surface)& theSurface2,
                           const Standard_Boolean theOrientation);

  //! Returns field Surface1
  Standard_EXPORT Handle(StepGeom_Surface) Surface1() const;
  //! Sets field Surface1
  Standard_EXPORT void SetSurface1 (const Handle(StepGeom_Surface)& theSurface1);

  //! Returns field Surface2
  Standard_EXPORT Handle(StepGeom_Surface) Surface2() const;
  //! Sets field Surface2
  Standard_EXPORT void SetSurface2 (const Handle(StepGeom_Surface)& theSurface2);

  //! Returns field Orientation
  Standard_EXPORT Standard_Boolean Orientation() const;
  //! Sets field Orientation
  Standard_EXPORT void SetOrientation (const Standard_Boolean theOrientation);

DEFINE_STANDARD_RTTIEXT(StepKinematics_SurfacePair, StepKinematics_HighOrderKinematicPair)

private:
  Handle(StepGeom_Surface) mySurface1;
  Handle(StepGeom_Surface) mySurface2;
  Standard_Boolean myOrientation;

};
#endif // _StepKinematics_SurfacePair_HeaderFile_
