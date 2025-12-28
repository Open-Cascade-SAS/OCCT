// Created on: 2002-12-10
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

#ifndef _StepElement_CurveElementPurpose_HeaderFile
#define _StepElement_CurveElementPurpose_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
#include <StepElement_EnumeratedCurveElementPurpose.hxx>
class Standard_Transient;
class StepData_SelectMember;
class TCollection_HAsciiString;

//! Representation of STEP SELECT type CurveElementPurpose
class StepElement_CurveElementPurpose : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepElement_CurveElementPurpose();

  //! Recognizes a kind of CurveElementPurpose select type
  //! return 0
  Standard_EXPORT int
    CaseNum(const occ::handle<Standard_Transient>& ent) const override;

  //! Recognizes a items of select member CurveElementPurposeMember
  //! 1 -> EnumeratedCurveElementPurpose
  //! 2 -> ApplicationDefinedElementPurpose
  //! 0 else
  Standard_EXPORT virtual int CaseMem(const occ::handle<StepData_SelectMember>& ent) const
    override;

  //! Returns a new select member the type CurveElementPurposeMember
  Standard_EXPORT virtual occ::handle<StepData_SelectMember> NewMember() const override;

  //! Set Value for EnumeratedCurveElementPurpose
  Standard_EXPORT void SetEnumeratedCurveElementPurpose(
    const StepElement_EnumeratedCurveElementPurpose aVal);

  //! Returns Value as EnumeratedCurveElementPurpose (or Null if another type)
  Standard_EXPORT StepElement_EnumeratedCurveElementPurpose EnumeratedCurveElementPurpose() const;

  //! Set Value for ApplicationDefinedElementPurpose
  Standard_EXPORT void SetApplicationDefinedElementPurpose(
    const occ::handle<TCollection_HAsciiString>& aVal);

  //! Returns Value as ApplicationDefinedElementPurpose (or Null if another type)
  Standard_EXPORT occ::handle<TCollection_HAsciiString> ApplicationDefinedElementPurpose() const;

};

#endif // _StepElement_CurveElementPurpose_HeaderFile
