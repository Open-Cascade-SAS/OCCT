// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_Face_HeaderFile
#define _StepShape_Face_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_FaceBound.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepShape_FaceBound;

class StepShape_Face : public StepShape_TopologicalRepresentationItem
{

public:
  //! Returns a Face
  Standard_EXPORT StepShape_Face();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                              aName,
    const occ::handle<NCollection_HArray1<occ::handle<StepShape_FaceBound>>>& aBounds);

  Standard_EXPORT virtual void SetBounds(
    const occ::handle<NCollection_HArray1<occ::handle<StepShape_FaceBound>>>& aBounds);

  Standard_EXPORT virtual occ::handle<NCollection_HArray1<occ::handle<StepShape_FaceBound>>>
    Bounds() const;

  Standard_EXPORT virtual occ::handle<StepShape_FaceBound> BoundsValue(const int num) const;

  Standard_EXPORT virtual int NbBounds() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_Face, StepShape_TopologicalRepresentationItem)

private:
  occ::handle<NCollection_HArray1<occ::handle<StepShape_FaceBound>>> bounds;
};

#endif // _StepShape_Face_HeaderFile
