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

#ifndef _StepKinematics_KinematicTopologyStructure_HeaderFile_
#define _StepKinematics_KinematicTopologyStructure_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepRepr_Representation.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationContext.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_KinematicTopologyStructure, StepRepr_Representation)

//! Representation of STEP entity KinematicTopologyStructure
class StepKinematics_KinematicTopologyStructure : public StepRepr_Representation
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_KinematicTopologyStructure();

DEFINE_STANDARD_RTTIEXT(StepKinematics_KinematicTopologyStructure, StepRepr_Representation)

};
#endif // _StepKinematics_KinematicTopologyStructure_HeaderFile_
