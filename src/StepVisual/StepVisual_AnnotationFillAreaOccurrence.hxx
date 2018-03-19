// Created on: 2016-12-28
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

#ifndef _StepVisual_AnnotationFillAreaOccurrence_HeaderFile
#define _StepVisual_AnnotationFillAreaOccurrence_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_AnnotationOccurrence.hxx>

class StepVisual_AnnotationFillAreaOccurrence;
DEFINE_STANDARD_HANDLE(StepVisual_AnnotationFillAreaOccurrence, StepVisual_AnnotationOccurrence)

class StepVisual_AnnotationFillAreaOccurrence : public StepVisual_AnnotationOccurrence
{
public:

  //! Returns a AnnotationFillAreaOccurrence
  Standard_EXPORT StepVisual_AnnotationFillAreaOccurrence();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& theName,
                             const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& theStyles,
                             const Handle(Standard_Transient)& theItem,
                             const Handle(StepGeom_GeometricRepresentationItem)& theFillStyleTarget);
  
  //! Returns field fill_style_target
  Handle(StepGeom_GeometricRepresentationItem) FillStyleTarget() const
  {
    return myFillStyleTarget;
  }
  
  //! Set field fill_style_target
  void SetFillStyleTarget (const Handle(StepGeom_GeometricRepresentationItem)& theTarget)
  {
    myFillStyleTarget = theTarget;
  }

  DEFINE_STANDARD_RTTIEXT(StepVisual_AnnotationFillAreaOccurrence, StepVisual_AnnotationOccurrence)
  
private:
  Handle(StepGeom_GeometricRepresentationItem) myFillStyleTarget;
};
#endif // _StepVisual_AnnotationFillAreaOccurrence_HeaderFile
