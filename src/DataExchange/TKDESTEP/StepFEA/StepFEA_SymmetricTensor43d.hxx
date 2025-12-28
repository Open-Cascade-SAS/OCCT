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

#ifndef _StepFEA_SymmetricTensor43d_HeaderFile
#define _StepFEA_SymmetricTensor43d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class Standard_Transient;
class StepData_SelectMember;

//! Representation of STEP SELECT type SymmetricTensor43d
class StepFEA_SymmetricTensor43d : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepFEA_SymmetricTensor43d();

  //! return 0
  Standard_EXPORT int CaseNum(const occ::handle<Standard_Transient>& ent) const override;

  //! Recognizes a items of select member CurveElementFreedomMember
  //! 1 -> AnisotropicSymmetricTensor43d
  //! 2 -> FeaIsotropicSymmetricTensor43d
  //! 3 -> FeaIsoOrthotropicSymmetricTensor43d
  //! 4 -> FeaTransverseIsotropicSymmetricTensor43d
  //! 5 -> FeaColumnNormalisedOrthotropicSymmetricTensor43d
  //! 6 -> FeaColumnNormalisedMonoclinicSymmetricTensor43d
  //! 0 else
  Standard_EXPORT virtual int CaseMem(const occ::handle<StepData_SelectMember>& ent) const override;

  Standard_EXPORT virtual occ::handle<StepData_SelectMember> NewMember() const override;

  //! Returns Value as AnisotropicSymmetricTensor43d (or Null if another type)
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> AnisotropicSymmetricTensor43d() const;

  Standard_EXPORT void SetFeaIsotropicSymmetricTensor43d(
    const occ::handle<NCollection_HArray1<double>>& val);

  //! Returns Value as FeaIsotropicSymmetricTensor43d (or Null if another type)
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> FeaIsotropicSymmetricTensor43d() const;

  //! Returns Value as FeaIsoOrthotropicSymmetricTensor43d (or Null if another type)
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> FeaIsoOrthotropicSymmetricTensor43d()
    const;

  //! Returns Value as FeaTransverseIsotropicSymmetricTensor43d (or Null if another type)
  Standard_EXPORT occ::handle<NCollection_HArray1<double>>
                  FeaTransverseIsotropicSymmetricTensor43d() const;

  //! Returns Value as FeaColumnNormalisedOrthotropicSymmetricTensor43d (or Null if another type)
  Standard_EXPORT occ::handle<NCollection_HArray1<double>>
                  FeaColumnNormalisedOrthotropicSymmetricTensor43d() const;

  //! Returns Value as FeaColumnNormalisedMonoclinicSymmetricTensor43d (or Null if another type)
  Standard_EXPORT occ::handle<NCollection_HArray1<double>>
                  FeaColumnNormalisedMonoclinicSymmetricTensor43d() const;
};

#endif // _StepFEA_SymmetricTensor43d_HeaderFile
