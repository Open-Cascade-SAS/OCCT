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

#ifndef _StepVisual_RepositionedTessellatedItem_HeaderFile
#define _StepVisual_RepositionedTessellatedItem_HeaderFile

#include <StepVisual_TessellatedItem.hxx>

class StepGeom_Axis2Placement3d;

DEFINE_STANDARD_HANDLE(StepVisual_RepositionedTessellatedItem, StepVisual_TessellatedItem)

//! Representation of STEP entity RepositionedTessellatedItem
class StepVisual_RepositionedTessellatedItem : public StepVisual_TessellatedItem
{
public:
  DEFINE_STANDARD_RTTIEXT(StepVisual_RepositionedTessellatedItem, StepVisual_TessellatedItem)

  DEFINE_STANDARD_ALLOC

  //! Default constructor
  StepVisual_RepositionedTessellatedItem() {};

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&  theName,
                            const Handle(StepGeom_Axis2Placement3d)& theLocation);

  //! Returns location
  Handle(StepGeom_Axis2Placement3d) Location() const { return myLocation; }

  //! Sets location
  void SetLocation(const Handle(StepGeom_Axis2Placement3d)& theLocation)
  {
    myLocation = theLocation;
  }

private:
  Handle(StepGeom_Axis2Placement3d) myLocation;
};
#endif // StepVisual_RepositionedTessellatedItem_HeaderFile
