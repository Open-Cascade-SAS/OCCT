// Copyright (c) 2022 OPEN CASCADE SAS
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

#include <StepVisual_RepositionedTessellatedGeometricSet.hxx>

#include <StepGeom_Axis2Placement3d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_RepositionedTessellatedGeometricSet,
                           StepVisual_TessellatedGeometricSet)

//=================================================================================================

void StepVisual_RepositionedTessellatedGeometricSet::Init(
  const Handle(TCollection_HAsciiString)&                       theName,
  const NCollection_Handle<StepVisual_Array1OfTessellatedItem>& theItems,
  const Handle(StepGeom_Axis2Placement3d)&                      theLocation)
{
  StepVisual_TessellatedGeometricSet::Init(theName, theItems);
  myLocation = theLocation;
}
