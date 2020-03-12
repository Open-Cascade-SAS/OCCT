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

#include <StepKinematics_SphericalPairSelect.hxx>
#include <StepKinematics_SphericalPair.hxx>
#include <StepKinematics_SphericalPairWithPin.hxx>

//=======================================================================
//function : StepKinematics_SphericalPairSelect
//purpose  :
//=======================================================================
StepKinematics_SphericalPairSelect::StepKinematics_SphericalPairSelect ()
{
}

//=======================================================================
//function : CaseNum
//purpose  :
//=======================================================================
Standard_Integer StepKinematics_SphericalPairSelect::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepKinematics_SphericalPair))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepKinematics_SphericalPairWithPin))) return 2;
  return 0;
}

//=======================================================================
//function : SphericalPair
//purpose  :
//=======================================================================
Handle(StepKinematics_SphericalPair) StepKinematics_SphericalPairSelect::SphericalPair () const
{
  return Handle(StepKinematics_SphericalPair)::DownCast(Value());
}

//=======================================================================
//function : SphericalPairWithPin
//purpose  :
//=======================================================================
Handle(StepKinematics_SphericalPairWithPin) StepKinematics_SphericalPairSelect::SphericalPairWithPin () const
{
  return Handle(StepKinematics_SphericalPairWithPin)::DownCast(Value());
}
