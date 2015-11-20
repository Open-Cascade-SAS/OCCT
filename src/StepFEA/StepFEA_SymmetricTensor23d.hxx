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

#ifndef _StepFEA_SymmetricTensor23d_HeaderFile
#define _StepFEA_SymmetricTensor23d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TColStd_HArray1OfReal.hxx>
class Standard_Transient;
class StepData_SelectMember;


//! Representation of STEP SELECT type SymmetricTensor23d
class StepFEA_SymmetricTensor23d  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT StepFEA_SymmetricTensor23d();
  
  //! Recognizes a kind of SymmetricTensor23d select type
  //! return 0
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  //! Recognizes a items of select member SymmetricTensor23dMember
  //! 1 -> IsotropicSymmetricTensor23d
  //! 2 -> OrthotropicSymmetricTensor23d
  //! 3 -> AnisotropicSymmetricTensor23d
  //! 0 else
  Standard_EXPORT virtual Standard_Integer CaseMem (const Handle(StepData_SelectMember)& ent) const Standard_OVERRIDE;
  
  //! Returns a new select member the type SymmetricTensor23dMember
  Standard_EXPORT virtual Handle(StepData_SelectMember) NewMember() const Standard_OVERRIDE;
  
  //! Set Value for IsotropicSymmetricTensor23d
  Standard_EXPORT void SetIsotropicSymmetricTensor23d (const Standard_Real aVal);
  
  //! Returns Value as IsotropicSymmetricTensor23d (or Null if another type)
  Standard_EXPORT Standard_Real IsotropicSymmetricTensor23d() const;
  
  //! Set Value for OrthotropicSymmetricTensor23d
  Standard_EXPORT void SetOrthotropicSymmetricTensor23d (const Handle(TColStd_HArray1OfReal)& aVal);
  
  //! Returns Value as OrthotropicSymmetricTensor23d (or Null if another type)
  Standard_EXPORT Handle(TColStd_HArray1OfReal) OrthotropicSymmetricTensor23d() const;
  
  //! Set Value for AnisotropicSymmetricTensor23d
  Standard_EXPORT void SetAnisotropicSymmetricTensor23d (const Handle(TColStd_HArray1OfReal)& aVal);
  
  //! Returns Value as AnisotropicSymmetricTensor23d (or Null if another type)
  Standard_EXPORT Handle(TColStd_HArray1OfReal) AnisotropicSymmetricTensor23d() const;




protected:





private:





};







#endif // _StepFEA_SymmetricTensor23d_HeaderFile
