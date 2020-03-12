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

#ifndef _RWStepKinematics_RWRevolutePairWithRange_HeaderFile_
#define _RWStepKinematics_RWRevolutePairWithRange_HeaderFile_

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class StepData_StepReaderData;
class Interface_Check;
class StepData_StepWriter;
class Interface_EntityIterator;
class StepKinematics_RevolutePairWithRange;

//! Read & Write tool for RevolutePairWithRange
class RWStepKinematics_RWRevolutePairWithRange
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT RWStepKinematics_RWRevolutePairWithRange();

  Standard_EXPORT void ReadStep(const Handle(StepData_StepReaderData)& theData, const Standard_Integer theNum, Handle(Interface_Check)& theArch, const Handle(StepKinematics_RevolutePairWithRange)& theEnt) const;

  Standard_EXPORT void WriteStep(StepData_StepWriter& theSW, const Handle(StepKinematics_RevolutePairWithRange)& theEnt) const;

  Standard_EXPORT void Share(const Handle(StepKinematics_RevolutePairWithRange)& theEnt, Interface_EntityIterator& iter) const;

};
#endif // _RWStepKinematics_RWRevolutePairWithRange_HeaderFile_
