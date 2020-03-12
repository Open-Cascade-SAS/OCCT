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

#include <StepKinematics_KinematicTopologyRepresentationSelect.hxx>

#include <StepKinematics_KinematicTopologyDirectedStructure.hxx>
#include <StepKinematics_KinematicTopologyNetworkStructure.hxx>
#include <StepKinematics_KinematicTopologyStructure.hxx>

//=======================================================================
//function : StepKinematics_KinematicTopologyRepresentationSelect
//purpose  :
//=======================================================================
StepKinematics_KinematicTopologyRepresentationSelect::StepKinematics_KinematicTopologyRepresentationSelect ()
{
}

//=======================================================================
//function : CaseNum
//purpose  :
//=======================================================================
Standard_Integer StepKinematics_KinematicTopologyRepresentationSelect::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepKinematics_KinematicTopologyDirectedStructure))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepKinematics_KinematicTopologyNetworkStructure))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepKinematics_KinematicTopologyStructure))) return 3;
  return 0;
}

//=======================================================================
//function : KinematicTopologyDirectedStructure
//purpose  :
//=======================================================================
Handle(StepKinematics_KinematicTopologyDirectedStructure) StepKinematics_KinematicTopologyRepresentationSelect::KinematicTopologyDirectedStructure () const
{
  return Handle(StepKinematics_KinematicTopologyDirectedStructure)::DownCast(Value());
}

//=======================================================================
//function : KinematicTopologyNetworkStructure
//purpose  :
//=======================================================================
Handle(StepKinematics_KinematicTopologyNetworkStructure) StepKinematics_KinematicTopologyRepresentationSelect::KinematicTopologyNetworkStructure () const
{
  return Handle(StepKinematics_KinematicTopologyNetworkStructure)::DownCast(Value());
}

//=======================================================================
//function : KinematicTopologyStructure
//purpose  :
//=======================================================================
Handle(StepKinematics_KinematicTopologyStructure) StepKinematics_KinematicTopologyRepresentationSelect::KinematicTopologyStructure () const
{
  return Handle(StepKinematics_KinematicTopologyStructure)::DownCast(Value());
}
