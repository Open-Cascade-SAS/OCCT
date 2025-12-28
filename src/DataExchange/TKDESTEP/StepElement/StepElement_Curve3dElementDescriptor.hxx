// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepElement_Curve3dElementDescriptor_HeaderFile
#define _StepElement_Curve3dElementDescriptor_HeaderFile

#include <Standard.hxx>

#include <StepElement_CurveElementPurposeMember.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepElement_ElementDescriptor.hxx>
#include <StepElement_ElementOrder.hxx>
class TCollection_HAsciiString;

//! Representation of STEP entity Curve3dElementDescriptor
class StepElement_Curve3dElementDescriptor : public StepElement_ElementDescriptor
{

public:
  //! Empty constructor
  Standard_EXPORT StepElement_Curve3dElementDescriptor();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const StepElement_ElementOrder               aElementDescriptor_TopologyOrder,
    const occ::handle<TCollection_HAsciiString>& aElementDescriptor_Description,
    const occ::handle<NCollection_HArray1<
      occ::handle<NCollection_HSequence<occ::handle<StepElement_CurveElementPurposeMember>>>>>&
      aPurpose);

  //! Returns field Purpose
  Standard_EXPORT occ::handle<NCollection_HArray1<
    occ::handle<NCollection_HSequence<occ::handle<StepElement_CurveElementPurposeMember>>>>>
                  Purpose() const;

  //! Set field Purpose
  Standard_EXPORT void SetPurpose(
    const occ::handle<NCollection_HArray1<
      occ::handle<NCollection_HSequence<occ::handle<StepElement_CurveElementPurposeMember>>>>>&
      Purpose);

  DEFINE_STANDARD_RTTIEXT(StepElement_Curve3dElementDescriptor, StepElement_ElementDescriptor)

private:
  occ::handle<NCollection_HArray1<
    occ::handle<NCollection_HSequence<occ::handle<StepElement_CurveElementPurposeMember>>>>>
    thePurpose;
};

#endif // _StepElement_Curve3dElementDescriptor_HeaderFile
