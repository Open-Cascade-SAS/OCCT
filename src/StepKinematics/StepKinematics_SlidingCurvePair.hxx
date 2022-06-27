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

#ifndef _StepKinematics_SlidingCurvePair_HeaderFile_
#define _StepKinematics_SlidingCurvePair_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_PlanarCurvePair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepGeom_Curve.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_SlidingCurvePair, StepKinematics_PlanarCurvePair)

//! Representation of STEP entity SlidingCurvePair
class StepKinematics_SlidingCurvePair : public StepKinematics_PlanarCurvePair
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_SlidingCurvePair();

DEFINE_STANDARD_RTTIEXT(StepKinematics_SlidingCurvePair, StepKinematics_PlanarCurvePair)

};
#endif // _StepKinematics_SlidingCurvePair_HeaderFile_
