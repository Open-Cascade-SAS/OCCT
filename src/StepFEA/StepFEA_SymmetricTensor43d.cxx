// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepFEA_SymmetricTensor43d.ixx>
#include <StepFEA_SymmetricTensor43dMember.hxx>
#include <TCollection_HAsciiString.hxx>

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

Standard_Integer StepFEA_SymmetricTensor43d::CaseNum (const Handle(Standard_Transient)& ent) const
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
