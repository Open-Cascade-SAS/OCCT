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
#include <gp_XYZ.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepVisual_TessellatedItem.hxx>

class StepVisual_CoordinatesList : public StepVisual_TessellatedItem
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a coordinate list
  Standard_EXPORT StepVisual_CoordinatesList();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& theName,
                            const occ::handle<NCollection_HArray1<gp_XYZ>>&      thePoints);

  Standard_EXPORT occ::handle<NCollection_HArray1<gp_XYZ>> Points() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_CoordinatesList, StepVisual_TessellatedItem)

private:
  occ::handle<NCollection_HArray1<gp_XYZ>> myPoints;
};
#endif // StepVisual_CoordinatesList
