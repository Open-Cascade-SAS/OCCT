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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Standard_Transient.hxx>
#include <StepData_SelectMember.hxx>
#include <StepFEA_SymmetricTensor43d.hxx>
#include <StepFEA_SymmetricTensor43dMember.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//function : StepFEA_SymmetricTensor43d
//purpose  : 
//=======================================================================
StepFEA_SymmetricTensor43d::StepFEA_SymmetricTensor43d ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepFEA_SymmetricTensor43d::CaseNum (const Handle(Standard_Transient)& /*ent*/) const
{
  return 0;
}

//=======================================================================
//function : CaseMem
//purpose  : 
//=======================================================================

Standard_Integer StepFEA_SymmetricTensor43d::CaseMem (const Handle(StepData_SelectMember)& ent) const
{
 if(ent.IsNull()) return 0;
 if(ent->Matches("ANISOTROPIC_SYMMETRIC_TENSOR4_3D")) return 1;
 else if(ent->Matches("FEA_ISOTROPIC_SYMMETRIC_TENSOR4_3D")) return 2;
 else if(ent->Matches("FEA_ISO_ORTHOTROPIC_SYMMETRIC_TENSOR4_3D")) return 3;
 else if(ent->Matches("FEA_TRANSVERSE_ISOTROPIC_SYMMETRIC_TENSOR4_3D")) return 4;
 else if(ent->Matches("FEA_COLUMN_NORMALISED_ORTHOTROPIC_SYMMETRIC_TENSOR4_3D")) return 5;
 else if(ent->Matches("FEA_COLUMN_NORMALISED_MONOCLINIC_SYMMETRIC_TENSOR4_3D")) return 6;
 else return 0;
}

//=======================================================================
//function : NewMember
//purpose  : 
//=======================================================================

Handle(StepData_SelectMember) StepFEA_SymmetricTensor43d::NewMember() const
{
  //Handle(StepData_SelectMember) dummy;
  //return dummy;
  return new StepFEA_SymmetricTensor43dMember;
}

//=======================================================================
//function : AnisotropicSymmetricTensor43d
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepFEA_SymmetricTensor43d::AnisotropicSymmetricTensor43d () const
{
  Handle(TColStd_HArray1OfReal) anArr; // = new TColStd_HArray1OfReal(1,1);
  Handle(StepFEA_SymmetricTensor43dMember) SelMem =
    Handle(StepFEA_SymmetricTensor43dMember)::DownCast(Value());
  if(SelMem.IsNull()) return anArr;
  //Handle(TColStd_HSequenceOfReal) aSeq = SelMem->SeqReal();
  return anArr;
  //return Handle(TColStd_HArray1OfReal)::DownCast(Value());
}

//=======================================================================
//function : FeaIsotropicSymmetricTensor43d
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepFEA_SymmetricTensor43d::FeaIsotropicSymmetricTensor43d () const
{
  Handle(TColStd_HArray1OfReal) anArr; // = new TColStd_HArray1OfReal(1,1);
  Handle(StepFEA_SymmetricTensor43dMember) SelMem =
    Handle(StepFEA_SymmetricTensor43dMember)::DownCast(Value());
  if(SelMem.IsNull()) return anArr;
  //Handle(TColStd_HSequenceOfReal) aSeq = SelMem->SeqReal();
  return anArr;
  //return Handle(TColStd_HArray1OfReal)::DownCast(Value());
}

//=======================================================================
//function : FeaIsoOrthotropicSymmetricTensor43d
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepFEA_SymmetricTensor43d::FeaIsoOrthotropicSymmetricTensor43d () const
{
  return Handle(TColStd_HArray1OfReal)::DownCast(Value());
}

//=======================================================================
//function : FeaTransverseIsotropicSymmetricTensor43d
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepFEA_SymmetricTensor43d::FeaTransverseIsotropicSymmetricTensor43d () const
{
  return Handle(TColStd_HArray1OfReal)::DownCast(Value());
}

//=======================================================================
//function : FeaColumnNormalisedOrthotropicSymmetricTensor43d
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepFEA_SymmetricTensor43d::FeaColumnNormalisedOrthotropicSymmetricTensor43d () const
{
  return Handle(TColStd_HArray1OfReal)::DownCast(Value());
}

//=======================================================================
//function : FeaColumnNormalisedMonoclinicSymmetricTensor43d
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepFEA_SymmetricTensor43d::FeaColumnNormalisedMonoclinicSymmetricTensor43d () const
{
  return Handle(TColStd_HArray1OfReal)::DownCast(Value());
}
