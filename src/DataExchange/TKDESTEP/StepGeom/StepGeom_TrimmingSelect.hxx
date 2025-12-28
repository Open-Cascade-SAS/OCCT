// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _StepGeom_TrimmingSelect_HeaderFile
#define _StepGeom_TrimmingSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepData_SelectMember;
class StepGeom_CartesianPoint;

class StepGeom_TrimmingSelect : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a TrimmingSelect SelectType
  Standard_EXPORT StepGeom_TrimmingSelect();

  //! Recognizes a TrimmingSelect Kind Entity that is :
  //! 1 -> CartesianPoint
  //! 0 else (i.e. Real)
  Standard_EXPORT int
    CaseNum(const occ::handle<Standard_Transient>& ent) const override;

  //! Returns a TrimmingMember (for PARAMETER_VALUE) as preferred
  Standard_EXPORT virtual occ::handle<StepData_SelectMember> NewMember() const override;

  //! Recognizes a SelectMember as Real, named as PARAMETER_VALUE
  //! 1 -> ParameterValue i.e. Real
  //! 0 else (i.e. Entity)
  Standard_EXPORT virtual int CaseMem(const occ::handle<StepData_SelectMember>& ent) const
    override;

  //! returns Value as a CartesianPoint (Null if another type)
  Standard_EXPORT occ::handle<StepGeom_CartesianPoint> CartesianPoint() const;

  //! sets the ParameterValue as Real
  Standard_EXPORT void SetParameterValue(const double aParameterValue);

  //! returns Value as a Real (0.0 if not a Real)
  Standard_EXPORT double ParameterValue() const;

};

#endif // _StepGeom_TrimmingSelect_HeaderFile
