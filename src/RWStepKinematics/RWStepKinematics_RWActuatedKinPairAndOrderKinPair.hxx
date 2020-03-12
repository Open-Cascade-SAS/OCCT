// Created on: 2020-05-26
// Created by: PASUKHIN DMITRY
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _RWStepKinematics_RWActuatedKinPairAndOrderKinPair_HeaderFile
#define _RWStepKinematics_RWActuatedKinPairAndOrderKinPair_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class StepData_StepReaderData;
class Interface_Check;
class StepData_StepWriter;
class Interface_EntityIterator;
class StepKinematics_ActuatedKinPairAndOrderKinPair;


//! Read & Write Module for GeoTolAndGeoTolWthMod
class RWStepKinematics_RWActuatedKinPairAndOrderKinPair
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT RWStepKinematics_RWActuatedKinPairAndOrderKinPair();
  
  Standard_EXPORT void ReadStep (const Handle(StepData_StepReaderData)& theData, const Standard_Integer theNum, Handle(Interface_Check)& theArch, const Handle(StepKinematics_ActuatedKinPairAndOrderKinPair)& theEnt) const;
  
  Standard_EXPORT void WriteStep (StepData_StepWriter& theSW, const Handle(StepKinematics_ActuatedKinPairAndOrderKinPair)& theEnt) const;
  
  Standard_EXPORT void Share (const Handle(StepKinematics_ActuatedKinPairAndOrderKinPair)& theEnt, Interface_EntityIterator& iter) const;
};
#endif // _RWStepDimTol_RWGeoTolAndGeoTolWthMod_HeaderFile
