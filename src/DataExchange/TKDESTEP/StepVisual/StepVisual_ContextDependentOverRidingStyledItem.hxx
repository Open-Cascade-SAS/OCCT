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

#ifndef _StepVisual_ContextDependentOverRidingStyledItem_HeaderFile
#define _StepVisual_ContextDependentOverRidingStyledItem_HeaderFile

#include <Standard.hxx>

#include <StepVisual_StyleContextSelect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepVisual_OverRidingStyledItem.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepVisual_StyledItem;
class StepVisual_StyleContextSelect;

class StepVisual_ContextDependentOverRidingStyledItem : public StepVisual_OverRidingStyledItem
{

public:
  //! Returns a ContextDependentOverRidingStyledItem
  Standard_EXPORT StepVisual_ContextDependentOverRidingStyledItem();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& aName,
    const occ::handle<NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>>&
                                                                           aStyles,
    const occ::handle<Standard_Transient>&                                 aItem,
    const occ::handle<StepVisual_StyledItem>&                              aOverRiddenStyle,
    const occ::handle<NCollection_HArray1<StepVisual_StyleContextSelect>>& aStyleContext);

  Standard_EXPORT void SetStyleContext(
    const occ::handle<NCollection_HArray1<StepVisual_StyleContextSelect>>& aStyleContext);

  Standard_EXPORT occ::handle<NCollection_HArray1<StepVisual_StyleContextSelect>> StyleContext()
    const;

  Standard_EXPORT StepVisual_StyleContextSelect StyleContextValue(const int num) const;

  Standard_EXPORT int NbStyleContext() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_ContextDependentOverRidingStyledItem,
                          StepVisual_OverRidingStyledItem)

private:
  occ::handle<NCollection_HArray1<StepVisual_StyleContextSelect>> styleContext;
};

#endif // _StepVisual_ContextDependentOverRidingStyledItem_HeaderFile
