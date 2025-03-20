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

#ifndef _StepVisual_BackgroundColour_HeaderFile
#define _StepVisual_BackgroundColour_HeaderFile

#include <Standard.hxx>

#include <StepVisual_AreaOrView.hxx>
#include <StepVisual_Colour.hxx>

class StepVisual_BackgroundColour;
DEFINE_STANDARD_HANDLE(StepVisual_BackgroundColour, StepVisual_Colour)

class StepVisual_BackgroundColour : public StepVisual_Colour
{

public:
  //! Returns a BackgroundColour
  Standard_EXPORT StepVisual_BackgroundColour();

  Standard_EXPORT void Init(const StepVisual_AreaOrView& aPresentation);

  Standard_EXPORT void SetPresentation(const StepVisual_AreaOrView& aPresentation);

  Standard_EXPORT StepVisual_AreaOrView Presentation() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_BackgroundColour, StepVisual_Colour)

protected:
private:
  StepVisual_AreaOrView presentation;
};

#endif // _StepVisual_BackgroundColour_HeaderFile
