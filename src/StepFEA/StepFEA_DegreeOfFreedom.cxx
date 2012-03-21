// Created on: 2002-12-14
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

#include <StepFEA_DegreeOfFreedom.ixx>
#include <StepFEA_DegreeOfFreedomMember.hxx>
#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : StepFEA_DegreeOfFreedom
//purpose  : 
//=======================================================================

StepFEA_DegreeOfFreedom::StepFEA_DegreeOfFreedom ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepFEA_DegreeOfFreedom::CaseNum (const Handle(Standard_Transient)& ent) const
{
  return 0;
}

//=======================================================================
//function : CaseMem
//purpose  : 
//=======================================================================

Standard_Integer StepFEA_DegreeOfFreedom::CaseMem (const Handle(StepData_SelectMember)& ent) const
{
 if(ent.IsNull()) return 0;
 if(ent->Matches("ENUMERATED_DEGREE_OF_FREEDOM")) return 1;
 else if(ent->Matches("APPLICATION_DEFINED_DEGREE_OF_FREEDOM")) return 2;
 else return 0;
}

//=======================================================================
//function : NewMember
//purpose  : 
//=======================================================================

Handle(StepData_SelectMember) StepFEA_DegreeOfFreedom::NewMember() const
{
 return new StepFEA_DegreeOfFreedomMember;
}

//=======================================================================
//function : SetEnumeratedCurveElementFreedom
//purpose  : 
//=======================================================================

void StepFEA_DegreeOfFreedom::SetEnumeratedDegreeOfFreedom (const StepFEA_EnumeratedDegreeOfFreedom val)
{
  Handle(StepFEA_DegreeOfFreedomMember) SelMem = Handle(StepFEA_DegreeOfFreedomMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("ENUMERATED_DEGREE_OF_FREEDOM");
 SelMem->SetName(name->ToCString());
 SelMem->SetEnum((Standard_Integer)val);
}

//=======================================================================
//function : EnumeratedCurveElementFreedom
//purpose  : 
//=======================================================================

StepFEA_EnumeratedDegreeOfFreedom StepFEA_DegreeOfFreedom::EnumeratedDegreeOfFreedom () const
{
  Handle(StepFEA_DegreeOfFreedomMember) SelMem = Handle(StepFEA_DegreeOfFreedomMember)::DownCast(Value());
  if(SelMem.IsNull()) return StepFEA_XTranslation;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
  name->AssignCat(SelMem->Name());
  Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("ENUMERATED_DEGREE_OF_FREEDOM");
  if(name->IsDifferent(nameitem)) return StepFEA_XTranslation;
  Standard_Integer numit = SelMem->Enum();
  StepFEA_EnumeratedDegreeOfFreedom val;
  switch(numit) {
  case 1 : val = StepFEA_XTranslation; break;
//  case 2 : val = StepElement_YTranslation; break;
//  case 3 : val = StepElement_ZTranslation; break;
//  case 4 : val = StepElement_XRotation; break;
//  case 5 : val = StepElement_YRotation; break;
//  case 6 : val = StepElement_ZRotation; break;
//  case 7 : val = StepElement_Warp; break;
//  case 8 : val = StepElement_None; break;
    default : return StepFEA_XTranslation;break;
  }
 return val;
}

//=======================================================================
//function : SetApplicationDefinedDegreeOfFreedom
//purpose  : 
//=======================================================================

void StepFEA_DegreeOfFreedom::SetApplicationDefinedDegreeOfFreedom (const Handle(TCollection_HAsciiString) &val)
{
  Handle(StepFEA_DegreeOfFreedomMember) SelMem = Handle(StepFEA_DegreeOfFreedomMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("APPLICATION_DEFINED_DEGREE_OF_FREEDOM");
 SelMem->SetName(name->ToCString());
 SelMem->SetString(val->ToCString());
}

//=======================================================================
//function : ApplicationDefinedDegreeOfFreedom
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepFEA_DegreeOfFreedom::ApplicationDefinedDegreeOfFreedom () const
{
  Handle(StepFEA_DegreeOfFreedomMember) SelMem = Handle(StepFEA_DegreeOfFreedomMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("APPLICATION_DEFINED_DEGREE_OF_FREEDOM");
 if(name->IsDifferent(nameitem)) return 0;
 Handle(TCollection_HAsciiString) val = new TCollection_HAsciiString;
 val->AssignCat(SelMem->String());
 return val;
}
