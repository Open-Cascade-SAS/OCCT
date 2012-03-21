// Created on: 2002-12-10
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0

#include <StepFEA_SymmetricTensor23d.ixx>
#include <StepFEA_SymmetricTensor23dMember.hxx>
#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : StepFEA_SymmetricTensor23d
//purpose  : 
//=======================================================================

StepFEA_SymmetricTensor23d::StepFEA_SymmetricTensor23d ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepFEA_SymmetricTensor23d::CaseNum (const Handle(Standard_Transient)& ent) const
{
  return 0;
}

//=======================================================================
//function : CaseMem
//purpose  : 
//=======================================================================

Standard_Integer StepFEA_SymmetricTensor23d::CaseMem (const Handle(StepData_SelectMember)& ent) const
{
 if(ent.IsNull()) return 0;
 if(ent->Matches("ISOTROPIC_SYMMETRIC_TENSOR2_3D")) return 1;
 else if(ent->Matches("ORTHOTROPIC_SYMMETRIC_TENSOR2_3D")) return 2;
 else if(ent->Matches("ANISOTROPIC_SYMMETRIC_TENSOR2_3D")) return 3;
 else return 0;
}

//=======================================================================
//function : NewMember
//purpose  : 
//=======================================================================

Handle(StepData_SelectMember) StepFEA_SymmetricTensor23d::NewMember() const
{
 return new StepFEA_SymmetricTensor23dMember;
}

//=======================================================================
//function : SetIsotropicSymmetricTensor23d
//purpose  : 
//=======================================================================

void StepFEA_SymmetricTensor23d::SetIsotropicSymmetricTensor23d (const Standard_Real val)
{
  Handle(StepFEA_SymmetricTensor23dMember) SelMem = Handle(StepFEA_SymmetricTensor23dMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("ISOTROPIC_SYMMETRIC_TENSOR2_3D");
 SelMem->SetName(name->ToCString());
 SelMem->SetReal(val);
}

//=======================================================================
//function : IsotropicSymmetricTensor23d
//purpose  : 
//=======================================================================

Standard_Real StepFEA_SymmetricTensor23d::IsotropicSymmetricTensor23d () const
{
  Handle(StepFEA_SymmetricTensor23dMember) SelMem = Handle(StepFEA_SymmetricTensor23dMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("ISOTROPIC_SYMMETRIC_TENSOR2_3D");
 if(name->IsDifferent(nameitem)) return 0;
 Standard_Real val = SelMem->Real();
 return val;
}

//=======================================================================
//function : SetOrthotropicSymmetricTensor23d
//purpose  : 
//=======================================================================

void StepFEA_SymmetricTensor23d::SetOrthotropicSymmetricTensor23d (const Handle(TColStd_HArray1OfReal)& val)
{
  Handle(StepFEA_SymmetricTensor23dMember) SelMem = Handle(StepFEA_SymmetricTensor23dMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("ORTHOTROPIC_SYMMETRIC_TENSOR2_3D");
 SelMem->SetName(name->ToCString());
// SelMem->SetHArray1OfReal(val);
}

//=======================================================================
//function : OrthotropicSymmetricTensor23d
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepFEA_SymmetricTensor23d::OrthotropicSymmetricTensor23d () const
{
  Handle(StepFEA_SymmetricTensor23dMember) SelMem = Handle(StepFEA_SymmetricTensor23dMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("ORTHOTROPIC_SYMMETRIC_TENSOR2_3D");
 if(name->IsDifferent(nameitem)) return 0;
 Handle(TColStd_HArray1OfReal) val/* = SelMem->HArray1OfReal()*/;
 return val;
}

//=======================================================================
//function : SetAnisotropicSymmetricTensor23d
//purpose  : 
//=======================================================================

void StepFEA_SymmetricTensor23d::SetAnisotropicSymmetricTensor23d (const Handle(TColStd_HArray1OfReal)& val)
{
  Handle(StepFEA_SymmetricTensor23dMember) SelMem = Handle(StepFEA_SymmetricTensor23dMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("ANISOTROPIC_SYMMETRIC_TENSOR2_3D");
 SelMem->SetName(name->ToCString());
// SelMem->SetHArray1OfReal(val);
}

//=======================================================================
//function : AnisotropicSymmetricTensor23d
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepFEA_SymmetricTensor23d::AnisotropicSymmetricTensor23d () const
{
  Handle(StepFEA_SymmetricTensor23dMember) SelMem = Handle(StepFEA_SymmetricTensor23dMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("ANISOTROPIC_SYMMETRIC_TENSOR2_3D");
 if(name->IsDifferent(nameitem)) return 0;
 Handle(TColStd_HArray1OfReal) val /*= SelMem->HArray1OfReal()*/;
 return val;
}
