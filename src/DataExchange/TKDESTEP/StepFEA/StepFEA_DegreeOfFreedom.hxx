// Created on: 2002-12-14
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

#ifndef _StepFEA_DegreeOfFreedom_HeaderFile
#define _StepFEA_DegreeOfFreedom_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
#include <StepFEA_EnumeratedDegreeOfFreedom.hxx>
class Standard_Transient;
class StepData_SelectMember;
class TCollection_HAsciiString;

//! Representation of STEP SELECT type DegreeOfFreedom
class StepFEA_DegreeOfFreedom : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepFEA_DegreeOfFreedom();

  //! Recognizes a kind of CurveElementFreedom select type
  //! return 0
  Standard_EXPORT Standard_Integer
    CaseNum(const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;

  //! Recognizes a items of select member CurveElementFreedomMember
  //! 1 -> EnumeratedCurveElementFreedom
  //! 2 -> ApplicationDefinedDegreeOfFreedom
  //! 0 else
  Standard_EXPORT virtual Standard_Integer CaseMem(const Handle(StepData_SelectMember)& ent) const
    Standard_OVERRIDE;

  //! Returns a new select member the type CurveElementFreedomMember
  Standard_EXPORT virtual Handle(StepData_SelectMember) NewMember() const Standard_OVERRIDE;

  //! Returns Value as EnumeratedDegreeOfFreedom (or Null if another type)
  Standard_EXPORT void SetEnumeratedDegreeOfFreedom(const StepFEA_EnumeratedDegreeOfFreedom aVal);

  //! Returns Value as EnumeratedDegreeOfFreedom (or Null if another type)
  Standard_EXPORT StepFEA_EnumeratedDegreeOfFreedom EnumeratedDegreeOfFreedom() const;

  //! Set Value for ApplicationDefinedDegreeOfFreedom
  Standard_EXPORT void SetApplicationDefinedDegreeOfFreedom(
    const Handle(TCollection_HAsciiString)& aVal);

  //! Returns Value as ApplicationDefinedDegreeOfFreedom (or Null if another type)
  Standard_EXPORT Handle(TCollection_HAsciiString) ApplicationDefinedDegreeOfFreedom() const;

protected:
private:
};

#endif // _StepFEA_DegreeOfFreedom_HeaderFile
