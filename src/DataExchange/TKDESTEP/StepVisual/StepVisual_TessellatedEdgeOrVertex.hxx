// Created on : Thu Mar 24 18:30:12 2022
// Created by: snn
// Generator: Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2022
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

#ifndef _StepVisual_TessellatedEdgeOrVertex_HeaderFile
#define _StepVisual_TessellatedEdgeOrVertex_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>

class Standard_Transient;
class StepVisual_TessellatedEdge;
class StepVisual_TessellatedVertex;

//! Representation of STEP SELECT type TessellatedEdgeOrVertex
class StepVisual_TessellatedEdgeOrVertex : public StepData_SelectType
{

public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepVisual_TessellatedEdgeOrVertex();

  //! Recognizes a kind of TessellatedEdgeOrVertex select type
  //! -- 1 -> TessellatedEdge
  //! -- 2 -> TessellatedVertex
  Standard_EXPORT Standard_Integer
    CaseNum(const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;

  //! Returns Value as TessellatedEdge (or Null if another type)
  Standard_EXPORT Handle(StepVisual_TessellatedEdge) TessellatedEdge() const;

  //! Returns Value as TessellatedVertex (or Null if another type)
  Standard_EXPORT Handle(StepVisual_TessellatedVertex) TessellatedVertex() const;
};
#endif // _StepVisual_TessellatedEdgeOrVertex_HeaderFile
