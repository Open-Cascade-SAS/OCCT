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

#ifndef _StepShape_FaceBound_HeaderFile
#define _StepShape_FaceBound_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
class StepShape_Loop;
class TCollection_HAsciiString;

class StepShape_FaceBound;
DEFINE_STANDARD_HANDLE(StepShape_FaceBound, StepShape_TopologicalRepresentationItem)

class StepShape_FaceBound : public StepShape_TopologicalRepresentationItem
{

public:
  //! Returns a FaceBound
  Standard_EXPORT StepShape_FaceBound();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(StepShape_Loop)&           aBound,
                            const Standard_Boolean                  aOrientation);

  Standard_EXPORT void SetBound(const Handle(StepShape_Loop)& aBound);

  Standard_EXPORT Handle(StepShape_Loop) Bound() const;

  Standard_EXPORT void SetOrientation(const Standard_Boolean aOrientation);

  Standard_EXPORT Standard_Boolean Orientation() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_FaceBound, StepShape_TopologicalRepresentationItem)

protected:
private:
  Handle(StepShape_Loop) bound;
  Standard_Boolean       orientation;
};

#endif // _StepShape_FaceBound_HeaderFile
