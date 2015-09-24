// Created on: 2015-06-22
// Created by: Irina KRYLOVA
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

#ifndef _RWStepRepr_RWPerpendicularTo_HeaderFile
#define _RWStepRepr_RWPerpendicularTo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>

class StepData_StepReaderData;
class Interface_Check;
class StepRepr_PerpendicularTo;
class StepData_StepWriter;
class Interface_EntityIterator;


//! Read & Write tool for PerpendicularTo
class RWStepRepr_RWPerpendicularTo 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT RWStepRepr_RWPerpendicularTo();
  
  //! Reads PerpendicularTo
  Standard_EXPORT   void ReadStep (const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepRepr_PerpendicularTo)& ent)  const;
  
  //! Writes PerpendicularTo
  Standard_EXPORT   void WriteStep (StepData_StepWriter& SW, const Handle(StepRepr_PerpendicularTo)& ent)  const;
  
  //! Fills data for graph (shared items)
  Standard_EXPORT   void Share (const Handle(StepRepr_PerpendicularTo)& ent, Interface_EntityIterator& iter)  const;
};
#endif // _RWStepRepr_RWPerpendicularTo_HeaderFile
