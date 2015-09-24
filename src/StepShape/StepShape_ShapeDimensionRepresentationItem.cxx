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

#include <StepShape_ShapeDimensionRepresentationItem.hxx>
#include <Interface_Macros.hxx>
#include <StepRepr_CompoundRepresentationItem.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepGeom_Placement.hxx>

//=======================================================================
//function : StepShape_ShapeDimensionRepresentationItem
//purpose  : 
//=======================================================================

StepShape_ShapeDimensionRepresentationItem::StepShape_ShapeDimensionRepresentationItem () {  }

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepShape_ShapeDimensionRepresentationItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_CompoundRepresentationItem))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_DescriptiveRepresentationItem))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_MeasureRepresentationItem))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_Placement))) return 4;
  return 0;
}

Handle(StepRepr_CompoundRepresentationItem) StepShape_ShapeDimensionRepresentationItem::CompoundRepresentationItem() const
{  return GetCasted(StepRepr_CompoundRepresentationItem,Value());  }

Handle(StepRepr_DescriptiveRepresentationItem) StepShape_ShapeDimensionRepresentationItem::DescriptiveRepresentationItem() const
{  return GetCasted(StepRepr_DescriptiveRepresentationItem,Value());  }

Handle(StepRepr_MeasureRepresentationItem) StepShape_ShapeDimensionRepresentationItem::MeasureRepresentationItem() const
{  return GetCasted(StepRepr_MeasureRepresentationItem,Value());  }

Handle(StepGeom_Placement) StepShape_ShapeDimensionRepresentationItem::Placement() const
{  return GetCasted(StepGeom_Placement,Value());  }
