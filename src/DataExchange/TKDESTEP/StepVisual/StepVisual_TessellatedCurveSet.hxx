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

#ifndef _StepVisual_TessellatedCurveSet_HeaderFile
#define _StepVisual_TessellatedCurveSet_HeaderFile
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <StepVisual_TessellatedItem.hxx>

#include <NCollection_Vector.hxx>
#include <NCollection_Handle.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

class StepVisual_TessellatedCurveSet : public StepVisual_TessellatedItem
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a DraughtingCalloutElement select type
  Standard_EXPORT StepVisual_TessellatedCurveSet();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&   theName,
    const occ::handle<StepVisual_CoordinatesList>& theCoordList,
    const NCollection_Handle<NCollection_Vector<occ::handle<NCollection_HSequence<int>>>>&
      theCurves);

  Standard_EXPORT occ::handle<StepVisual_CoordinatesList> CoordList() const;
  Standard_EXPORT NCollection_Handle<NCollection_Vector<occ::handle<NCollection_HSequence<int>>>>
                  Curves() const;

private:
  occ::handle<StepVisual_CoordinatesList>                                         myCoordList;
  NCollection_Handle<NCollection_Vector<occ::handle<NCollection_HSequence<int>>>> myCurves;

public:
  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedCurveSet, StepVisual_TessellatedItem)
};
#endif // StepVisual_TessellatedCurveSet
