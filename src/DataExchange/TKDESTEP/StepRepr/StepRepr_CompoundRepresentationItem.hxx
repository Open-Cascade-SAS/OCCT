// Created on: 2001-04-24
// Created by: Christian CAILLET
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _StepRepr_CompoundRepresentationItem_HeaderFile
#define _StepRepr_CompoundRepresentationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;

//! Added for Dimensional Tolerances
class StepRepr_CompoundRepresentationItem : public StepRepr_RepresentationItem
{

public:
  Standard_EXPORT StepRepr_CompoundRepresentationItem();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                                      aName,
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>& item_element);

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>
                  ItemElement() const;

  Standard_EXPORT int NbItemElement() const;

  Standard_EXPORT void SetItemElement(
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>& item_element);

  Standard_EXPORT occ::handle<StepRepr_RepresentationItem> ItemElementValue(const int num) const;

  Standard_EXPORT void SetItemElementValue(
    const int                                       num,
    const occ::handle<StepRepr_RepresentationItem>& anelement);

  DEFINE_STANDARD_RTTIEXT(StepRepr_CompoundRepresentationItem, StepRepr_RepresentationItem)

private:
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> theItemElement;
};

#endif // _StepRepr_CompoundRepresentationItem_HeaderFile
