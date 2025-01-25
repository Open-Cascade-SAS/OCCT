// Created on: 1994-11-30
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

#ifndef _TopoDSToStep_MakeStepVertex_HeaderFile
#define _TopoDSToStep_MakeStepVertex_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDSToStep_MakeVertexError.hxx>
#include <TopoDSToStep_Root.hxx>
class StepShape_TopologicalRepresentationItem;
class TopoDS_Vertex;
class TopoDSToStep_Tool;
class Transfer_FinderProcess;

//! This class implements the mapping between classes
//! Vertex from TopoDS and TopologicalRepresentationItem from
//! StepShape.
class TopoDSToStep_MakeStepVertex : public TopoDSToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopoDSToStep_MakeStepVertex();

  Standard_EXPORT TopoDSToStep_MakeStepVertex(
    const TopoDS_Vertex&                  V,
    TopoDSToStep_Tool&                    T,
    const Handle(Transfer_FinderProcess)& FP,
    const StepData_Factors&               theLocalFactors = StepData_Factors());

  Standard_EXPORT void Init(const TopoDS_Vertex&                  V,
                            TopoDSToStep_Tool&                    T,
                            const Handle(Transfer_FinderProcess)& FP,
                            const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT const Handle(StepShape_TopologicalRepresentationItem)& Value() const;

  Standard_EXPORT TopoDSToStep_MakeVertexError Error() const;

protected:
private:
  Handle(StepShape_TopologicalRepresentationItem) myResult;
  TopoDSToStep_MakeVertexError                    myError;
};

#endif // _TopoDSToStep_MakeStepVertex_HeaderFile
