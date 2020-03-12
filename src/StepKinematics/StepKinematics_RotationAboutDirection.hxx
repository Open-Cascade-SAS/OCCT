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

#ifndef _StepKinematics_RotationAboutDirection_HeaderFile_
#define _StepKinematics_RotationAboutDirection_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepGeom_Direction.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_RotationAboutDirection, StepGeom_GeometricRepresentationItem)

//! Representation of STEP entity RotationAboutDirection
class StepKinematics_RotationAboutDirection : public StepGeom_GeometricRepresentationItem
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_RotationAboutDirection();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(StepGeom_Direction)& theDirectionOfAxis,
                           const Standard_Real theRotationAngle);

  //! Returns field DirectionOfAxis
  Standard_EXPORT Handle(StepGeom_Direction) DirectionOfAxis() const;
  //! Sets field DirectionOfAxis
  Standard_EXPORT void SetDirectionOfAxis (const Handle(StepGeom_Direction)& theDirectionOfAxis);

  //! Returns field RotationAngle
  Standard_EXPORT Standard_Real RotationAngle() const;
  //! Sets field RotationAngle
  Standard_EXPORT void SetRotationAngle (const Standard_Real theRotationAngle);

DEFINE_STANDARD_RTTIEXT(StepKinematics_RotationAboutDirection, StepGeom_GeometricRepresentationItem)

private:
  Handle(StepGeom_Direction) myDirectionOfAxis;
  Standard_Real myRotationAngle;

};
#endif // _StepKinematics_RotationAboutDirection_HeaderFile_
