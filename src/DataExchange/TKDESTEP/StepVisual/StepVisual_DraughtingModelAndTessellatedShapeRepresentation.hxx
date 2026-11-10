// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _StepVisual_DraughtingModelAndTessellatedShapeRepresentation_HeaderFile
#define _StepVisual_DraughtingModelAndTessellatedShapeRepresentation_HeaderFile

#include <StepVisual_DraughtingModel.hxx>

//! STEP complex entity combining DRAUGHTING_MODEL, REPRESENTATION, SHAPE_REPRESENTATION and
//! TESSELLATED_SHAPE_REPRESENTATION.
class StepVisual_DraughtingModelAndTessellatedShapeRepresentation
    : public StepVisual_DraughtingModel
{
public:
  //! Construct an empty complex representation.
  Standard_EXPORT StepVisual_DraughtingModelAndTessellatedShapeRepresentation();

  DEFINE_STANDARD_RTTIEXT(StepVisual_DraughtingModelAndTessellatedShapeRepresentation,
                          StepVisual_DraughtingModel)
};

#endif // _StepVisual_DraughtingModelAndTessellatedShapeRepresentation_HeaderFile
