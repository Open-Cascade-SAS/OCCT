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

#ifndef _StepVisual_RepositionedTessellatedGeometricSet_HeaderFile
#define _StepVisual_RepositionedTessellatedGeometricSet_HeaderFile

#include <StepVisual_TessellatedGeometricSet.hxx>

class StepGeom_Axis2Placement3d;

DEFINE_STANDARD_HANDLE(StepVisual_RepositionedTessellatedGeometricSet, StepVisual_TessellatedGeometricSet)

//! Representation of complex STEP entity RepositionedTessellatedGeometricSet
class StepVisual_RepositionedTessellatedGeometricSet : public StepVisual_TessellatedGeometricSet
{
public:

  DEFINE_STANDARD_ALLOC

  DEFINE_STANDARD_RTTIEXT(StepVisual_RepositionedTessellatedGeometricSet, StepVisual_TessellatedGeometricSet)

  //! Default constructor
  StepVisual_RepositionedTessellatedGeometricSet() {};

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theName,
                            const NCollection_Handle<StepVisual_Array1OfTessellatedItem>& theItems,
                            const Handle(StepGeom_Axis2Placement3d)& theLocation);

  //! Returns location
  Handle(StepGeom_Axis2Placement3d) Location() const { return myLocation; }

  //! Sets location
  void SetLocation(const Handle(StepGeom_Axis2Placement3d)& theLocation) { myLocation = theLocation; }

private:

  Handle(StepGeom_Axis2Placement3d) myLocation;
};
#endif // StepVisual_RepositionedTessellatedGeometricSet_HeaderFile
