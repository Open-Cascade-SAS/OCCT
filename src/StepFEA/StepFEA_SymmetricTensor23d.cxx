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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0

#include <Standard_Transient.hxx>
#include <StepData_SelectMember.hxx>
#include <StepFEA_SymmetricTensor23d.hxx>
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

Standard_Integer StepFEA_SymmetricTensor23d::CaseNum (const Handle(Standard_Transient)& /*ent*/) const
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

void StepFEA_SymmetricTensor23d::SetOrthotropicSymmetricTensor23d (const Handle(TColStd_HArray1OfReal)& /*val*/)
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

void StepFEA_SymmetricTensor23d::SetAnisotropicSymmetricTensor23d (const Handle(TColStd_HArray1OfReal)& /*val*/)
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
