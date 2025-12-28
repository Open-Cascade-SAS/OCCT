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

#ifndef _StepVisual_DraughtingCallout_HeaderFile
#define _StepVisual_DraughtingCallout_HeaderFile

#include <Standard.hxx>

#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepVisual_DraughtingCalloutElement.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

class StepVisual_DraughtingCallout : public StepGeom_GeometricRepresentationItem
{
public:
  //! Returns a DraughtingCallout
  Standard_EXPORT StepVisual_DraughtingCallout();

  //! Init
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                     theName,
    const occ::handle<NCollection_HArray1<StepVisual_DraughtingCalloutElement>>& theContents);

  //! Returns field Contents
  inline occ::handle<NCollection_HArray1<StepVisual_DraughtingCalloutElement>> Contents() const
  {
    return myContents;
  }

  //! Set field Contents
  inline void SetContents(const occ::handle<NCollection_HArray1<StepVisual_DraughtingCalloutElement>>& theContents)
  {
    myContents = theContents;
  }

  //! Returns number of Contents
  inline int NbContents() const
  {
    return (myContents.IsNull() ? 0 : myContents->Length());
  }

  //! Returns Contents with the given number
  inline StepVisual_DraughtingCalloutElement ContentsValue(const int theNum) const
  {
    return myContents->Value(theNum);
  }

  //! Sets Contents with given number
  inline void SetContentsValue(const int                     theNum,
                               const StepVisual_DraughtingCalloutElement& theItem)
  {
    myContents->SetValue(theNum, theItem);
  }

  DEFINE_STANDARD_RTTIEXT(StepVisual_DraughtingCallout, StepGeom_GeometricRepresentationItem)

private:
  occ::handle<NCollection_HArray1<StepVisual_DraughtingCalloutElement>> myContents;
};
#endif // _StepVisual_DraughtingCallout_HeaderFile
