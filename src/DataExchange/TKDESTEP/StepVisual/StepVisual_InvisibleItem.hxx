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

#ifndef _StepVisual_InvisibleItem_HeaderFile
#define _StepVisual_InvisibleItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepVisual_StyledItem;
class StepVisual_PresentationLayerAssignment;
class StepVisual_PresentationRepresentation;

class StepVisual_InvisibleItem : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a InvisibleItem SelectType
  Standard_EXPORT StepVisual_InvisibleItem();

  //! Recognizes a InvisibleItem Kind Entity that is :
  //! 1 -> StyledItem
  //! 2 -> PresentationLayerAssignment
  //! 3 -> PresentationRepresentation
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a StyledItem (Null if another type)
  Standard_EXPORT Handle(StepVisual_StyledItem) StyledItem() const;

  //! returns Value as a PresentationLayerAssignment (Null if another type)
  Standard_EXPORT Handle(StepVisual_PresentationLayerAssignment) PresentationLayerAssignment()
    const;

  //! returns Value as a PresentationRepresentation (Null if another type)
  Standard_EXPORT Handle(StepVisual_PresentationRepresentation) PresentationRepresentation() const;

protected:
private:
};

#endif // _StepVisual_InvisibleItem_HeaderFile
