// File:	StepFEA_SymmetricTensor43d.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
