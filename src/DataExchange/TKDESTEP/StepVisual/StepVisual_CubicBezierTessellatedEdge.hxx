// Created on : Thu Mar 24 18:30:11 2022
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

#ifndef _StepVisual_CubicBezierTessellatedEdge_HeaderFile_
#define _StepVisual_CubicBezierTessellatedEdge_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepVisual_TessellatedEdge.hxx>

DEFINE_STANDARD_HANDLE(StepVisual_CubicBezierTessellatedEdge, StepVisual_TessellatedEdge)

//! Representation of STEP entity CubicBezierTessellatedEdge
class StepVisual_CubicBezierTessellatedEdge : public StepVisual_TessellatedEdge
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_CubicBezierTessellatedEdge();

  DEFINE_STANDARD_RTTIEXT(StepVisual_CubicBezierTessellatedEdge, StepVisual_TessellatedEdge)
};

#endif // _StepVisual_CubicBezierTessellatedEdge_HeaderFile_
