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

#ifndef _StepVisual_TessellatedStructuredItem_HeaderFile_
#define _StepVisual_TessellatedStructuredItem_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepVisual_TessellatedItem.hxx>

DEFINE_STANDARD_HANDLE(StepVisual_TessellatedStructuredItem, StepVisual_TessellatedItem)

//! Representation of STEP entity TessellatedStructuredItem
class StepVisual_TessellatedStructuredItem : public StepVisual_TessellatedItem
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_TessellatedStructuredItem();

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedStructuredItem, StepVisual_TessellatedItem)
};

#endif // _StepVisual_TessellatedStructuredItem_HeaderFile_
