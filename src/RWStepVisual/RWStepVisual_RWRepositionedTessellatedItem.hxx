// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _RWStepVisual_RWRepositionedTessellatedItem_HeaderFile
#define _RWStepVisual_RWRepositionedTessellatedItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class StepData_StepReaderData;
class Interface_Check;
class StepVisual_RepositionedTessellatedItem;
class StepData_StepWriter;

//! Read & Write tool for RepositionedTessellatedItem
class RWStepVisual_RWRepositionedTessellatedItem
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  RWStepVisual_RWRepositionedTessellatedItem() {};

  //! Reads RepositionedTessellatedItem
  Standard_EXPORT void ReadStep (const Handle(StepData_StepReaderData)& theData,
   const Standard_Integer theNum,
   Handle(Interface_Check)& theAch,
   const Handle(StepVisual_RepositionedTessellatedItem)& theEnt) const;

  //! Writes RepositionedTessellatedItem
  Standard_EXPORT void WriteStep (StepData_StepWriter& theSW,
    const Handle(StepVisual_RepositionedTessellatedItem)& theEnt) const;
};
#endif // _RWStepVisual_RWRepositionedTessellatedItem_HeaderFile
