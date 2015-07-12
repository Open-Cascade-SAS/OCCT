// Created on: 1996-07-24
// Created by: Frederic MAUPAS
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _RWStepBasic_RWMechanicalContext_HeaderFile
#define _RWStepBasic_RWMechanicalContext_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class StepData_StepReaderData;
class Interface_Check;
class StepBasic_MechanicalContext;
class StepData_StepWriter;
class Interface_EntityIterator;


//! Read & Write Module for MechanicalContext
class RWStepBasic_RWMechanicalContext 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT RWStepBasic_RWMechanicalContext();
  
  Standard_EXPORT void ReadStep (const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepBasic_MechanicalContext)& ent) const;
  
  Standard_EXPORT void WriteStep (StepData_StepWriter& SW, const Handle(StepBasic_MechanicalContext)& ent) const;
  
  Standard_EXPORT void Share (const Handle(StepBasic_MechanicalContext)& ent, Interface_EntityIterator& iter) const;




protected:





private:





};







#endif // _RWStepBasic_RWMechanicalContext_HeaderFile
