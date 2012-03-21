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
