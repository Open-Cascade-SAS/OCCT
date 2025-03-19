// Created on: 1994-12-16
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

#ifndef _StepToTopoDS_TranslateVertexLoop_HeaderFile
#define _StepToTopoDS_TranslateVertexLoop_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepToTopoDS_TranslateVertexLoopError.hxx>
#include <TopoDS_Shape.hxx>
#include <StepToTopoDS_Root.hxx>
class StepShape_VertexLoop;
class StepToTopoDS_Tool;
class StepToTopoDS_NMTool;

class StepToTopoDS_TranslateVertexLoop : public StepToTopoDS_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT StepToTopoDS_TranslateVertexLoop();

  Standard_EXPORT StepToTopoDS_TranslateVertexLoop(
    const Handle(StepShape_VertexLoop)& VL,
    StepToTopoDS_Tool&                  T,
    StepToTopoDS_NMTool&                NMTool,
    const StepData_Factors&             theLocalFactors = StepData_Factors());

  Standard_EXPORT void Init(const Handle(StepShape_VertexLoop)& VL,
                            StepToTopoDS_Tool&                  T,
                            StepToTopoDS_NMTool&                NMTool,
                            const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT const TopoDS_Shape& Value() const;

  Standard_EXPORT StepToTopoDS_TranslateVertexLoopError Error() const;

protected:
private:
  StepToTopoDS_TranslateVertexLoopError myError;
  TopoDS_Shape                          myResult;
};

#endif // _StepToTopoDS_TranslateVertexLoop_HeaderFile
