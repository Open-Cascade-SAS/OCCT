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

#ifndef _StepVisual_PresentationSize_HeaderFile
#define _StepVisual_PresentationSize_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_PresentationSizeAssignmentSelect.hxx>
#include <Standard_Transient.hxx>
class StepVisual_PlanarBox;

class StepVisual_PresentationSize;
DEFINE_STANDARD_HANDLE(StepVisual_PresentationSize, Standard_Transient)

class StepVisual_PresentationSize : public Standard_Transient
{

public:
  //! Returns a PresentationSize
  Standard_EXPORT StepVisual_PresentationSize();

  Standard_EXPORT void Init(const StepVisual_PresentationSizeAssignmentSelect& aUnit,
                            const Handle(StepVisual_PlanarBox)&                aSize);

  Standard_EXPORT void SetUnit(const StepVisual_PresentationSizeAssignmentSelect& aUnit);

  Standard_EXPORT StepVisual_PresentationSizeAssignmentSelect Unit() const;

  Standard_EXPORT void SetSize(const Handle(StepVisual_PlanarBox)& aSize);

  Standard_EXPORT Handle(StepVisual_PlanarBox) Size() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_PresentationSize, Standard_Transient)

protected:
private:
  StepVisual_PresentationSizeAssignmentSelect unit;
  Handle(StepVisual_PlanarBox)                size;
};

#endif // _StepVisual_PresentationSize_HeaderFile
