// Created on: 2015-10-29
// Created by: Galina Kulikova
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepVisual_CoordinatesList_HeaderFile
#define _StepVisual_CoordinatesList_HeaderFile
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <StepVisual_TessellatedItem.hxx>

DEFINE_STANDARD_HANDLE(StepVisual_CoordinatesList, StepVisual_TessellatedItem)

class StepVisual_CoordinatesList : public StepVisual_TessellatedItem
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a coordinate list
  Standard_EXPORT StepVisual_CoordinatesList();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theName,
                            const Handle(TColgp_HArray1OfXYZ)&      thePoints);

  Standard_EXPORT Handle(TColgp_HArray1OfXYZ) Points() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_CoordinatesList, StepVisual_TessellatedItem)

private:
  Handle(TColgp_HArray1OfXYZ) myPoints;
};
#endif // StepVisual_CoordinatesList
