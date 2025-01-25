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

#ifndef _StepVisual_ColourSpecification_HeaderFile
#define _StepVisual_ColourSpecification_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_Colour.hxx>
class TCollection_HAsciiString;

class StepVisual_ColourSpecification;
DEFINE_STANDARD_HANDLE(StepVisual_ColourSpecification, StepVisual_Colour)

class StepVisual_ColourSpecification : public StepVisual_Colour
{

public:
  //! Returns a ColourSpecification
  Standard_EXPORT StepVisual_ColourSpecification();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName);

  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& aName);

  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_ColourSpecification, StepVisual_Colour)

protected:
private:
  Handle(TCollection_HAsciiString) name;
};

#endif // _StepVisual_ColourSpecification_HeaderFile
