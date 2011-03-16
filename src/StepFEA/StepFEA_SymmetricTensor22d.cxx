// File:	StepFEA_SymmetricTensor22d.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_SymmetricTensor22d.ixx>

#include <StepData_SelectArrReal.hxx>
#include <TColStd_HSequenceOfReal.hxx>


//=======================================================================
//function : StepFEA_SymmetricTensor22d
//purpose  : 
//=======================================================================

StepFEA_SymmetricTensor22d::StepFEA_SymmetricTensor22d ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepFEA_SymmetricTensor22d::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepData_SelectArrReal))) return 1;
  return 0;
}

//=======================================================================
//function : AnisotropicSymmetricTensor22d
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepFEA_SymmetricTensor22d::AnisotropicSymmetricTensor22d () const
{
  //return Handle(TColStd_HArray1OfReal)::DownCast(Value());
  Handle(StepData_SelectArrReal) SSR = Handle(StepData_SelectArrReal)::DownCast(Value());
  if(SSR.IsNull()) return new TColStd_HArray1OfReal(1,3);
  return SSR->ArrReal();
}
