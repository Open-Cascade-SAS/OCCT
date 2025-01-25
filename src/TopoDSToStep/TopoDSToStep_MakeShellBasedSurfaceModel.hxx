// Created on: 1994-06-24
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TopoDSToStep_MakeShellBasedSurfaceModel_HeaderFile
#define _TopoDSToStep_MakeShellBasedSurfaceModel_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDSToStep_Root.hxx>
#include <Message_ProgressRange.hxx>

class StepShape_ShellBasedSurfaceModel;
class StepVisual_TessellatedItem;
class TopoDS_Face;
class Transfer_FinderProcess;
class TopoDS_Shell;
class TopoDS_Solid;

//! This class implements the mapping between classes
//! Face, Shell or Solid from TopoDS and ShellBasedSurfaceModel
//! from StepShape. All the topology and geometry comprised
//! into the shape are taken into account and translated.
class TopoDSToStep_MakeShellBasedSurfaceModel : public TopoDSToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopoDSToStep_MakeShellBasedSurfaceModel(
    const TopoDS_Face&                    F,
    const Handle(Transfer_FinderProcess)& FP,
    const StepData_Factors&               theLocalFactors = StepData_Factors(),
    const Message_ProgressRange&          theProgress     = Message_ProgressRange());

  Standard_EXPORT TopoDSToStep_MakeShellBasedSurfaceModel(
    const TopoDS_Shell&                   S,
    const Handle(Transfer_FinderProcess)& FP,
    const StepData_Factors&               theLocalFactors = StepData_Factors(),
    const Message_ProgressRange&          theProgress     = Message_ProgressRange());

  Standard_EXPORT TopoDSToStep_MakeShellBasedSurfaceModel(
    const TopoDS_Solid&                   S,
    const Handle(Transfer_FinderProcess)& FP,
    const StepData_Factors&               theLocalFactors = StepData_Factors(),
    const Message_ProgressRange&          theProgress     = Message_ProgressRange());

  Standard_EXPORT const Handle(StepShape_ShellBasedSurfaceModel)& Value() const;
  Standard_EXPORT const Handle(StepVisual_TessellatedItem)&       TessellatedValue() const;

protected:
private:
  Handle(StepShape_ShellBasedSurfaceModel) theShellBasedSurfaceModel;
  Handle(StepVisual_TessellatedItem)       theTessellatedItem;
};

#endif // _TopoDSToStep_MakeShellBasedSurfaceModel_HeaderFile
