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

#include <StepKinematics_MechanismRepresentation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_MechanismRepresentation, StepRepr_Representation)

//=================================================================================================

StepKinematics_MechanismRepresentation::StepKinematics_MechanismRepresentation() {}

//=================================================================================================

void StepKinematics_MechanismRepresentation::Init(
  const occ::handle<TCollection_HAsciiString>& theRepresentation_Name,
  const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>&
                                                              theRepresentation_Items,
  const occ::handle<StepRepr_RepresentationContext>&          theRepresentation_ContextOfItems,
  const StepKinematics_KinematicTopologyRepresentationSelect& theRepresentedTopology)
{
  StepRepr_Representation::Init(theRepresentation_Name,
                                theRepresentation_Items,
                                theRepresentation_ContextOfItems);

  myRepresentedTopology = theRepresentedTopology;
}

//=================================================================================================

StepKinematics_KinematicTopologyRepresentationSelect StepKinematics_MechanismRepresentation::
  RepresentedTopology() const
{
  return myRepresentedTopology;
}

//=================================================================================================

void StepKinematics_MechanismRepresentation::SetRepresentedTopology(
  const StepKinematics_KinematicTopologyRepresentationSelect& theRepresentedTopology)
{
  myRepresentedTopology = theRepresentedTopology;
}
