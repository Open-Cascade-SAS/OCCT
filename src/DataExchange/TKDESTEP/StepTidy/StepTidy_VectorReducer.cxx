// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <StepTidy_VectorReducer.pxx>

#include <Interface_Graph.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_Vector.hxx>

//==================================================================================================

StepTidy_VectorReducer::StepTidy_VectorReducer(const Handle(XSControl_WorkSession)& theWS)
    : StepTidy_EntityReducer<StepGeom_Vector, StepTidy_VectorHasher>(theWS)
{
  registerReplacer(STANDARD_TYPE(StepGeom_Line), replaceLine);
}

//==================================================================================================

bool StepTidy_VectorReducer::replaceLine(const Handle(StepGeom_Vector)& theOldEntity,
                                         const Handle(StepGeom_Vector)& theNewEntity,
                                         Handle(Standard_Transient)     theSharing)
{
  Handle(StepGeom_Line) aLine = Handle(StepGeom_Line)::DownCast(theSharing);
  if (aLine->Dir() == theOldEntity)
  {
    aLine->SetDir(theNewEntity);
    return true;
  }
  return false;
}
