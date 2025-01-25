// Created on: 1993-07-23
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopoDSToStep_MakeBrepWithVoids_HeaderFile
#define _TopoDSToStep_MakeBrepWithVoids_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDSToStep_Root.hxx>
#include <Message_ProgressRange.hxx>

class StepShape_BrepWithVoids;
class StepVisual_TessellatedItem;
class TopoDS_Solid;
class Transfer_FinderProcess;

//! This class implements the mapping between classes
//! Solid from TopoDS and BrepWithVoids from
//! StepShape. All the topology and geometry comprised
//! into the shell or the solid are taken into account and
//! translated.
class TopoDSToStep_MakeBrepWithVoids : public TopoDSToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopoDSToStep_MakeBrepWithVoids(
    const TopoDS_Solid&                   S,
    const Handle(Transfer_FinderProcess)& FP,
    const StepData_Factors&               theLocalFactors = StepData_Factors(),
    const Message_ProgressRange&          theProgress     = Message_ProgressRange());

  Standard_EXPORT const Handle(StepShape_BrepWithVoids)&    Value() const;
  Standard_EXPORT const Handle(StepVisual_TessellatedItem)& TessellatedValue() const;

protected:
private:
  Handle(StepShape_BrepWithVoids)    theBrepWithVoids;
  Handle(StepVisual_TessellatedItem) theTessellatedItem;
};

#endif // _TopoDSToStep_MakeBrepWithVoids_HeaderFile
