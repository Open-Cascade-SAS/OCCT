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

#ifndef _TopoDSToStep_MakeManifoldSolidBrep_HeaderFile
#define _TopoDSToStep_MakeManifoldSolidBrep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDSToStep_Root.hxx>
#include <Message_ProgressRange.hxx>

class StepShape_ManifoldSolidBrep;
class StepVisual_TessellatedItem;
class TopoDS_Shell;
class Transfer_FinderProcess;
class TopoDS_Solid;

//! This class implements the mapping between classes
//! Shell or Solid from TopoDS and ManifoldSolidBrep from
//! StepShape. All the topology and geometry comprised
//! into the shell or the solid are taken into account and
//! translated.
class TopoDSToStep_MakeManifoldSolidBrep : public TopoDSToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopoDSToStep_MakeManifoldSolidBrep(
    const TopoDS_Shell&                   S,
    const Handle(Transfer_FinderProcess)& FP,
    const StepData_Factors&               theLocalFactors = StepData_Factors(),
    const Message_ProgressRange&          theProgress     = Message_ProgressRange());

  Standard_EXPORT TopoDSToStep_MakeManifoldSolidBrep(
    const TopoDS_Solid&                   S,
    const Handle(Transfer_FinderProcess)& FP,
    const StepData_Factors&               theLocalFactors = StepData_Factors(),
    const Message_ProgressRange&          theProgress     = Message_ProgressRange());

  Standard_EXPORT const Handle(StepShape_ManifoldSolidBrep)& Value() const;
  Standard_EXPORT const Handle(StepVisual_TessellatedItem)&  TessellatedValue() const;

protected:
private:
  Handle(StepShape_ManifoldSolidBrep) theManifoldSolidBrep;
  Handle(StepVisual_TessellatedItem)  theTessellatedItem;
};

#endif // _TopoDSToStep_MakeManifoldSolidBrep_HeaderFile
