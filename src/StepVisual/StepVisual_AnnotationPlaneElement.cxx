// Created on: 2015-07-10
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

#include <StepVisual_AnnotationPlaneElement.hxx>
#include <Interface_Macros.hxx>
#include <StepVisual_DraughtingCallout.hxx>
#include <StepVisual_StyledItem.hxx>

//=======================================================================
//function : StepVisual_AnnotationPlaneElement
//purpose  : 
//=======================================================================

StepVisual_AnnotationPlaneElement::StepVisual_AnnotationPlaneElement () {  }

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepVisual_AnnotationPlaneElement::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_DraughtingCallout))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_StyledItem))) return 2;
  return 0;
}

Handle(StepVisual_DraughtingCallout) StepVisual_AnnotationPlaneElement::DraughtingCallout() const
{  return GetCasted(StepVisual_DraughtingCallout,Value());  }

Handle(StepVisual_StyledItem) StepVisual_AnnotationPlaneElement::StyledItem() const
{  return GetCasted(StepVisual_StyledItem,Value());  }
