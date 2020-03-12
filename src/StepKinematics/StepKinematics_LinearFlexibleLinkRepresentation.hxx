// Created on : Fri May 08 19:02:07 2020 
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

#ifndef _StepKinematics_LinearFlexibleLinkRepresentation_HeaderFile_
#define _StepKinematics_LinearFlexibleLinkRepresentation_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepKinematics_KinematicLinkRepresentation.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepKinematics_KinematicLink.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_LinearFlexibleLinkRepresentation, StepKinematics_KinematicLinkRepresentation)

//! Representation of STEP entity LinearFlexibleLinkRepresentation
class StepKinematics_LinearFlexibleLinkRepresentation : public StepKinematics_KinematicLinkRepresentation
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_LinearFlexibleLinkRepresentation();

DEFINE_STANDARD_RTTIEXT(StepKinematics_LinearFlexibleLinkRepresentation, StepKinematics_KinematicLinkRepresentation)

};
#endif // _StepKinematics_LinearFlexibleLinkRepresentation_HeaderFile_
