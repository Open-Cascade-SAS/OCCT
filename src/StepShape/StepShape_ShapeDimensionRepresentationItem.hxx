// Created on: 2015-07-21
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepShape_ShapeDimensionRepresentationItem_HeaderFile
#define _StepShape_ShapeDimensionRepresentationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>

class Standard_Transient;
class StepRepr_CompoundRepresentationItem;
class StepRepr_DescriptiveRepresentationItem;
class StepRepr_MeasureRepresentationItem;
class StepGeom_Placement;

class StepShape_ShapeDimensionRepresentationItem  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! Returns a ShapeDimensionRepresentationItem select type
  Standard_EXPORT StepShape_ShapeDimensionRepresentationItem();
  
  //! Recognizes a ShapeDimensionRepresentationItem Kind Entity that is :
  //! 1 -> CompoundRepresentationItem
  //! 2 -> DescriptiveRepresentationItem
  //! 3 -> MeasureRepresentationItem
  //! 4 -> Placement
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent)  const;
  
  //! returns Value as a CompoundRepresentationItem (Null if another type)
  Standard_EXPORT Handle(StepRepr_CompoundRepresentationItem) CompoundRepresentationItem()  const;
  
  //! returns Value as a DescriptiveRepresentationItem (Null if another type)
  Standard_EXPORT Handle(StepRepr_DescriptiveRepresentationItem) DescriptiveRepresentationItem()  const;
  
  //! returns Value as a MeasureRepresentationItem (Null if another type)
  Standard_EXPORT Handle(StepRepr_MeasureRepresentationItem) MeasureRepresentationItem()  const;
  
  //! returns Value as a Placement (Null if another type)
  Standard_EXPORT Handle(StepGeom_Placement) Placement()  const; 
};
#endif // _StepShape_ShapeDimensionRepresentationItem_HeaderFile
