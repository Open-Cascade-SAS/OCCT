// Created on: 2016-03-18
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _StepVisual_StyledItemTarget_HeaderFile
#define _StepVisual_StyledItemTarget_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>

class Standard_Transient;
class StepGeom_GeometricRepresentationItem;
class StepRepr_MappedItem;
class StepRepr_Representation;
class StepShape_TopologicalRepresentationItem;

class StepVisual_StyledItemTarget : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a StyledItemTarget select type
  Standard_EXPORT StepVisual_StyledItemTarget();

  //! Recognizes a StyledItemTarget Kind Entity that is :
  //! 1 -> GeometricRepresentationItem
  //! 2 -> MappedItem
  //! 3 -> Representation
  //! 4 -> TopologicalRepresentationItem
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a GeometricRepresentationItem (Null if another type)
  Standard_EXPORT Handle(StepGeom_GeometricRepresentationItem) GeometricRepresentationItem() const;

  //! returns Value as a MappedItem (Null if another type)
  Standard_EXPORT Handle(StepRepr_MappedItem) MappedItem() const;

  //! returns Value as a Representation (Null if another type)
  Standard_EXPORT Handle(StepRepr_Representation) Representation() const;

  //! returns Value as a TopologicalRepresentationItem (Null if another type)
  Standard_EXPORT Handle(StepShape_TopologicalRepresentationItem) TopologicalRepresentationItem()
    const;
};
#endif // _StepVisual_StyledItemTarget_HeaderFile
