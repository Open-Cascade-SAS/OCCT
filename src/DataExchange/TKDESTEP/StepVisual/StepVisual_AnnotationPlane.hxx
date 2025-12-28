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

#ifndef _StepVisual_AnnotationPlane_HeaderFile
#define _StepVisual_AnnotationPlane_HeaderFile

#include <Standard.hxx>

#include <StepVisual_AnnotationOccurrence.hxx>
#include <StepVisual_AnnotationPlaneElement.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

class StepVisual_AnnotationPlane : public StepVisual_AnnotationOccurrence
{
public:
  //! Returns a AnnotationPlane
  Standard_EXPORT StepVisual_AnnotationPlane();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& theName,
    const occ::handle<NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>>&
                                                                               theStyles,
    const occ::handle<Standard_Transient>&                                     theItem,
    const occ::handle<NCollection_HArray1<StepVisual_AnnotationPlaneElement>>& theElements);

  //! Returns field Elements
  inline occ::handle<NCollection_HArray1<StepVisual_AnnotationPlaneElement>> Elements() const
  {
    return myElements;
  }

  //! Set field Elements
  inline void SetElements(
    const occ::handle<NCollection_HArray1<StepVisual_AnnotationPlaneElement>>& theElements)
  {
    myElements = theElements;
  }

  //! Returns number of Elements
  inline int NbElements() const { return (myElements.IsNull() ? 0 : myElements->Length()); }

  //! Returns Elements with the given number
  inline StepVisual_AnnotationPlaneElement ElementsValue(const int theNum) const
  {
    return myElements->Value(theNum);
  }

  //! Sets Elements with given number
  inline void SetElementsValue(const int theNum, const StepVisual_AnnotationPlaneElement& theItem)
  {
    myElements->SetValue(theNum, theItem);
  }

  DEFINE_STANDARD_RTTIEXT(StepVisual_AnnotationPlane, StepVisual_AnnotationOccurrence)

private:
  occ::handle<NCollection_HArray1<StepVisual_AnnotationPlaneElement>> myElements;
};
#endif // _StepVisual_AnnotationPlane_HeaderFile
