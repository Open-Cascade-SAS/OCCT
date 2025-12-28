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

#ifndef _StepVisual_PresentationLayerAssignment_HeaderFile
#define _StepVisual_PresentationLayerAssignment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_LayeredItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepVisual_LayeredItem;

class StepVisual_PresentationLayerAssignment : public Standard_Transient
{

public:
  //! Returns a PresentationLayerAssignment
  Standard_EXPORT StepVisual_PresentationLayerAssignment();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                    aName,
    const occ::handle<TCollection_HAsciiString>&                    aDescription,
    const occ::handle<NCollection_HArray1<StepVisual_LayeredItem>>& aAssignedItems);

  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& aName);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  Standard_EXPORT void SetDescription(const occ::handle<TCollection_HAsciiString>& aDescription);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Description() const;

  Standard_EXPORT void SetAssignedItems(
    const occ::handle<NCollection_HArray1<StepVisual_LayeredItem>>& aAssignedItems);

  Standard_EXPORT occ::handle<NCollection_HArray1<StepVisual_LayeredItem>> AssignedItems() const;

  Standard_EXPORT StepVisual_LayeredItem AssignedItemsValue(const int num) const;

  Standard_EXPORT int NbAssignedItems() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_PresentationLayerAssignment, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString>                    name;
  occ::handle<TCollection_HAsciiString>                    description;
  occ::handle<NCollection_HArray1<StepVisual_LayeredItem>> assignedItems;
};

#endif // _StepVisual_PresentationLayerAssignment_HeaderFile
