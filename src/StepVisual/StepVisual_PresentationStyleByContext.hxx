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

#ifndef _StepVisual_PresentationStyleByContext_HeaderFile
#define _StepVisual_PresentationStyleByContext_HeaderFile

#include <Standard.hxx>

#include <StepVisual_StyleContextSelect.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_HArray1OfPresentationStyleSelect.hxx>

class StepVisual_PresentationStyleByContext;
DEFINE_STANDARD_HANDLE(StepVisual_PresentationStyleByContext,
                       StepVisual_PresentationStyleAssignment)

class StepVisual_PresentationStyleByContext : public StepVisual_PresentationStyleAssignment
{

public:
  //! Returns a PresentationStyleByContext
  Standard_EXPORT StepVisual_PresentationStyleByContext();

  Standard_EXPORT void Init(const Handle(StepVisual_HArray1OfPresentationStyleSelect)& aStyles,
                            const StepVisual_StyleContextSelect& aStyleContext);

  Standard_EXPORT void SetStyleContext(const StepVisual_StyleContextSelect& aStyleContext);

  Standard_EXPORT StepVisual_StyleContextSelect StyleContext() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_PresentationStyleByContext,
                          StepVisual_PresentationStyleAssignment)

protected:
private:
  StepVisual_StyleContextSelect styleContext;
};

#endif // _StepVisual_PresentationStyleByContext_HeaderFile
