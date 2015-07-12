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
#include <StepElement_CurveElementFreedom.hxx>
#include <StepElement_CurveElementFreedomMember.hxx>
#include <TCollection_HAsciiString.hxx>

static Standard_CString anEnum = "ENUMERATED_CURVE_ELEMENT_FREEDOM";
static Standard_CString anApp = "APPLICATION_DEFINED_DEGREE_OF_FREEDOM";
//=======================================================================
//function : StepElement_CurveElementFreedom
//purpose  : 
//=======================================================================

StepElement_CurveElementFreedom::StepElement_CurveElementFreedom ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepElement_CurveElementFreedom::CaseNum (const Handle(Standard_Transient)& /*ent*/) const
{
  return 0;
}

//=======================================================================
//function : CaseMem
//purpose  : 
//=======================================================================

Standard_Integer StepElement_CurveElementFreedom::CaseMem (const Handle(StepData_SelectMember)& ent) const
{
 if(ent.IsNull()) return 0;
 if(ent->Matches(anEnum)) return 1;
 else if(ent->Matches(anApp)) return 2;
 else return 0;
}

//=======================================================================
//function : NewMember
//purpose  : 
//=======================================================================

Handle(StepData_SelectMember) StepElement_CurveElementFreedom::NewMember() const
{
 return new StepElement_CurveElementFreedomMember;
}

//=======================================================================
//function : SetEnumeratedCurveElementFreedom
//purpose  : 
//=======================================================================

void StepElement_CurveElementFreedom::SetEnumeratedCurveElementFreedom (const StepElement_EnumeratedCurveElementFreedom val)
{
  Handle(StepElement_CurveElementFreedomMember) SelMem = Handle(StepElement_CurveElementFreedomMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("ENUMERATED_CURVE_ELEMENT_FREEDOM");
 SelMem->SetName(name->ToCString());
 SelMem->SetEnum((Standard_Integer)val);
}

//=======================================================================
//function : EnumeratedCurveElementFreedom
//purpose  : 
//=======================================================================

StepElement_EnumeratedCurveElementFreedom StepElement_CurveElementFreedom::EnumeratedCurveElementFreedom () const
{
  Handle(StepElement_CurveElementFreedomMember) SelMem = Handle(StepElement_CurveElementFreedomMember)::DownCast(Value());
  if(SelMem.IsNull()) return StepElement_None;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
  name->AssignCat(SelMem->Name());
  Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("ENUMERATED_CURVE_ELEMENT_FREEDOM");
  if(name->IsDifferent(nameitem)) return StepElement_None;
  Standard_Integer numit = SelMem->Enum();
  StepElement_EnumeratedCurveElementFreedom val;
  switch(numit) {
  case 1 : val = StepElement_XTranslation; break;
  case 2 : val = StepElement_YTranslation; break;
  case 3 : val = StepElement_ZTranslation; break;
  case 4 : val = StepElement_XRotation; break;
  case 5 : val = StepElement_YRotation; break;
  case 6 : val = StepElement_ZRotation; break;
  case 7 : val = StepElement_Warp; break;
  case 8 : val = StepElement_None; break;
    default : return StepElement_None;break;
  }
 return val;
}

//=======================================================================
//function : SetApplicationDefinedDegreeOfFreedom
//purpose  : 
//=======================================================================

void StepElement_CurveElementFreedom::SetApplicationDefinedDegreeOfFreedom (const Handle(TCollection_HAsciiString) &val)
{
  Handle(StepElement_CurveElementFreedomMember) SelMem = Handle(StepElement_CurveElementFreedomMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("APPLICATION_DEFINED_DEGREE_OF_FREEDOM");
 SelMem->SetName(name->ToCString());
 SelMem->SetString(val->ToCString());
}

//=======================================================================
//function : ApplicationDefinedDegreeOfFreedom
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepElement_CurveElementFreedom::ApplicationDefinedDegreeOfFreedom () const
{
  Handle(StepElement_CurveElementFreedomMember) SelMem = Handle(StepElement_CurveElementFreedomMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("APPLICATION_DEFINED_DEGREE_OF_FREEDOM");
 if(name->IsDifferent(nameitem)) return 0;
 Handle(TCollection_HAsciiString) val = new TCollection_HAsciiString;
 val->AssignCat(SelMem->String());
 return val;
}
