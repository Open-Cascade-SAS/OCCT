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

#ifndef _StepKinematics_MechanismRepresentation_HeaderFile_
#define _StepKinematics_MechanismRepresentation_HeaderFile_

#include <Standard.hxx>
#include <StepRepr_Representation.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepKinematics_KinematicTopologyRepresentationSelect.hxx>

//! Representation of STEP entity MechanismRepresentation
class StepKinematics_MechanismRepresentation : public StepRepr_Representation
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_MechanismRepresentation();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                     theRepresentation_Name,
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>&         theRepresentation_Items,
    const occ::handle<StepRepr_RepresentationContext>&               theRepresentation_ContextOfItems,
    const StepKinematics_KinematicTopologyRepresentationSelect& theRepresentedTopology);

  //! Returns field RepresentedTopology
  Standard_EXPORT StepKinematics_KinematicTopologyRepresentationSelect RepresentedTopology() const;
  //! Sets field RepresentedTopology
  Standard_EXPORT void SetRepresentedTopology(
    const StepKinematics_KinematicTopologyRepresentationSelect& theRepresentedTopology);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_MechanismRepresentation, StepRepr_Representation)

private:
  StepKinematics_KinematicTopologyRepresentationSelect myRepresentedTopology;
};
#endif // _StepKinematics_MechanismRepresentation_HeaderFile_
