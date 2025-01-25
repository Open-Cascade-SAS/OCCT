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

#ifndef _StepKinematics_KinematicTopologyRepresentationSelect_HeaderFile
#define _StepKinematics_KinematicTopologyRepresentationSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>

class Standard_Transient;
class StepKinematics_KinematicTopologyDirectedStructure;
class StepKinematics_KinematicTopologyNetworkStructure;
class StepKinematics_KinematicTopologyStructure;

//! Representation of STEP SELECT type KinematicTopologyRepresentationSelect
class StepKinematics_KinematicTopologyRepresentationSelect : public StepData_SelectType
{

public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepKinematics_KinematicTopologyRepresentationSelect();

  //! Recognizes a kind of KinematicTopologyRepresentationSelect select type
  //! -- 1 -> KinematicTopologyDirectedStructure
  //! -- 2 -> KinematicTopologyNetworkStructure
  //! -- 3 -> KinematicTopologyStructure
  Standard_EXPORT Standard_Integer
    CaseNum(const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;

  //! Returns Value as KinematicTopologyDirectedStructure (or Null if another type)
  Standard_EXPORT Handle(StepKinematics_KinematicTopologyDirectedStructure)
    KinematicTopologyDirectedStructure() const;

  //! Returns Value as KinematicTopologyNetworkStructure (or Null if another type)
  Standard_EXPORT Handle(StepKinematics_KinematicTopologyNetworkStructure)
    KinematicTopologyNetworkStructure() const;

  //! Returns Value as KinematicTopologyStructure (or Null if another type)
  Standard_EXPORT Handle(StepKinematics_KinematicTopologyStructure) KinematicTopologyStructure()
    const;
};
#endif // _StepKinematics_KinematicTopologyRepresentationSelect_HeaderFile
