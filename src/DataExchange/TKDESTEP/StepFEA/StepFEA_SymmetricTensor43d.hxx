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
#include <TColStd_HArray1OfReal.hxx>
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
  Standard_EXPORT Standard_Integer
    CaseNum(const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;

  //! Recognizes a items of select member CurveElementFreedomMember
  //! 1 -> AnisotropicSymmetricTensor43d
  //! 2 -> FeaIsotropicSymmetricTensor43d
  //! 3 -> FeaIsoOrthotropicSymmetricTensor43d
  //! 4 -> FeaTransverseIsotropicSymmetricTensor43d
  //! 5 -> FeaColumnNormalisedOrthotropicSymmetricTensor43d
  //! 6 -> FeaColumnNormalisedMonoclinicSymmetricTensor43d
  //! 0 else
  Standard_EXPORT virtual Standard_Integer CaseMem(const Handle(StepData_SelectMember)& ent) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(StepData_SelectMember) NewMember() const Standard_OVERRIDE;

  //! Returns Value as AnisotropicSymmetricTensor43d (or Null if another type)
  Standard_EXPORT Handle(TColStd_HArray1OfReal) AnisotropicSymmetricTensor43d() const;

  Standard_EXPORT void SetFeaIsotropicSymmetricTensor43d(const Handle(TColStd_HArray1OfReal)& val);

  //! Returns Value as FeaIsotropicSymmetricTensor43d (or Null if another type)
  Standard_EXPORT Handle(TColStd_HArray1OfReal) FeaIsotropicSymmetricTensor43d() const;

  //! Returns Value as FeaIsoOrthotropicSymmetricTensor43d (or Null if another type)
  Standard_EXPORT Handle(TColStd_HArray1OfReal) FeaIsoOrthotropicSymmetricTensor43d() const;

  //! Returns Value as FeaTransverseIsotropicSymmetricTensor43d (or Null if another type)
  Standard_EXPORT Handle(TColStd_HArray1OfReal) FeaTransverseIsotropicSymmetricTensor43d() const;

  //! Returns Value as FeaColumnNormalisedOrthotropicSymmetricTensor43d (or Null if another type)
  Standard_EXPORT Handle(TColStd_HArray1OfReal) FeaColumnNormalisedOrthotropicSymmetricTensor43d()
    const;

  //! Returns Value as FeaColumnNormalisedMonoclinicSymmetricTensor43d (or Null if another type)
  Standard_EXPORT Handle(TColStd_HArray1OfReal) FeaColumnNormalisedMonoclinicSymmetricTensor43d()
    const;

protected:
private:
};

#endif // _StepFEA_SymmetricTensor43d_HeaderFile
