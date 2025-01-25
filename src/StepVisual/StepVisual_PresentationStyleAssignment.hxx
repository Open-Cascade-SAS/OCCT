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

#ifndef _StepVisual_PresentationStyleAssignment_HeaderFile
#define _StepVisual_PresentationStyleAssignment_HeaderFile

#include <Standard.hxx>

#include <StepVisual_HArray1OfPresentationStyleSelect.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class StepVisual_PresentationStyleSelect;

class StepVisual_PresentationStyleAssignment;
DEFINE_STANDARD_HANDLE(StepVisual_PresentationStyleAssignment, Standard_Transient)

class StepVisual_PresentationStyleAssignment : public Standard_Transient
{

public:
  //! Returns a PresentationStyleAssignment
  Standard_EXPORT StepVisual_PresentationStyleAssignment();

  Standard_EXPORT void Init(const Handle(StepVisual_HArray1OfPresentationStyleSelect)& aStyles);

  Standard_EXPORT void SetStyles(
    const Handle(StepVisual_HArray1OfPresentationStyleSelect)& aStyles);

  Standard_EXPORT Handle(StepVisual_HArray1OfPresentationStyleSelect) Styles() const;

  Standard_EXPORT StepVisual_PresentationStyleSelect StylesValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbStyles() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_PresentationStyleAssignment, Standard_Transient)

protected:
private:
  Handle(StepVisual_HArray1OfPresentationStyleSelect) styles;
};

#endif // _StepVisual_PresentationStyleAssignment_HeaderFile
