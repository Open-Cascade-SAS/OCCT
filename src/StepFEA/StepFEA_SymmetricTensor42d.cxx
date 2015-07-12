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
#include <StepData_SelectArrReal.hxx>
#include <StepFEA_SymmetricTensor42d.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HSequenceOfReal.hxx>

//=======================================================================
//function : StepFEA_SymmetricTensor42d
//purpose  : 
//=======================================================================
StepFEA_SymmetricTensor42d::StepFEA_SymmetricTensor42d ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepFEA_SymmetricTensor42d::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepData_SelectArrReal))) return 1;
  return 0;
}

//=======================================================================
//function : AnisotropicSymmetricTensor42d
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepFEA_SymmetricTensor42d::AnisotropicSymmetricTensor42d () const
{
  //return Handle(TColStd_HArray1OfReal)::DownCast(Value());
  Handle(StepData_SelectArrReal) SSR = Handle(StepData_SelectArrReal)::DownCast(Value());
  if(SSR.IsNull()) return new TColStd_HArray1OfReal(1,6);
  return SSR->ArrReal();
}
