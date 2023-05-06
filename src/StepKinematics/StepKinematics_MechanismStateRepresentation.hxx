// Created on: 2020-06-18
// Created by: PASUKHIN DMITRY
// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _StepKinematics_MechanismStateRepresentation_HeaderFile
#define _StepKinematics_MechanismStateRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_Representation.hxx>

class StepKinematics_MechanismRepresentation;

class StepKinematics_MechanismStateRepresentation;
DEFINE_STANDARD_HANDLE(StepKinematics_MechanismStateRepresentation, StepRepr_Representation)


class StepKinematics_MechanismStateRepresentation : public StepRepr_Representation
{
public:
  
  //! Returns a MechanismStateRepresentation
  Standard_EXPORT StepKinematics_MechanismStateRepresentation();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theName, const Handle(StepRepr_HArray1OfRepresentationItem)& theItems, const Handle(StepRepr_RepresentationContext)& theContextOfItems, const Handle(StepKinematics_MechanismRepresentation)& theMechanism);

  Standard_EXPORT void SetMechanism(const Handle(StepKinematics_MechanismRepresentation)& theMechanism);

  Standard_EXPORT Handle(StepKinematics_MechanismRepresentation) Mechanism() const;

private:
  Handle(StepKinematics_MechanismRepresentation) myRepresentedMechanism;

  DEFINE_STANDARD_RTTIEXT(StepKinematics_MechanismStateRepresentation,StepRepr_Representation)
};

#endif // _StepKinematics_MechanismStateRepresentation_HeaderFile
