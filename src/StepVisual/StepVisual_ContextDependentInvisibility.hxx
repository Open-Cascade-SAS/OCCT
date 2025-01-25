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

#ifndef _StepVisual_ContextDependentInvisibility_HeaderFile
#define _StepVisual_ContextDependentInvisibility_HeaderFile

#include <Standard.hxx>

#include <StepVisual_InvisibilityContext.hxx>
#include <StepVisual_Invisibility.hxx>
#include <StepVisual_HArray1OfInvisibleItem.hxx>

class StepVisual_ContextDependentInvisibility;
DEFINE_STANDARD_HANDLE(StepVisual_ContextDependentInvisibility, StepVisual_Invisibility)

class StepVisual_ContextDependentInvisibility : public StepVisual_Invisibility
{

public:
  //! Returns a ContextDependentInvisibility
  Standard_EXPORT StepVisual_ContextDependentInvisibility();

  Standard_EXPORT void Init(const Handle(StepVisual_HArray1OfInvisibleItem)& aInvisibleItems,
                            const StepVisual_InvisibilityContext&            aPresentationContext);

  Standard_EXPORT void SetPresentationContext(
    const StepVisual_InvisibilityContext& aPresentationContext);

  Standard_EXPORT StepVisual_InvisibilityContext PresentationContext() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_ContextDependentInvisibility, StepVisual_Invisibility)

protected:
private:
  StepVisual_InvisibilityContext presentationContext;
};

#endif // _StepVisual_ContextDependentInvisibility_HeaderFile
