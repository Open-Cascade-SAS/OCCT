// Created on: 2015-10-29
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

#ifndef _StepVisual_AnnotationPlaneElement_HeaderFile
#define _StepVisual_AnnotationPlaneElement_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepVisual_DraughtingCallout;
class StepVisual_StyledItem;

class StepVisual_AnnotationPlaneElement  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! Returns a AnnotationPlaneElement select type
  Standard_EXPORT StepVisual_AnnotationPlaneElement();
  
  //! Recognizes a IdAttributeSelect Kind Entity that is :
  //! 1 -> DraughtingCallout
  //! 2 -> StyledItem
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent)  const;
  
  //! returns Value as a DraughtingCallout (Null if another type)
  Standard_EXPORT Handle(StepVisual_DraughtingCallout) DraughtingCallout()  const;
  
  //! returns Value as a StyledItem (Null if another type)
  Standard_EXPORT Handle(StepVisual_StyledItem) StyledItem()  const;
};
#endif // StepVisual_AnnotationPlaneElement
