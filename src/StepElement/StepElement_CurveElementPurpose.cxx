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

#include <StepElement_CurveElementPurpose.ixx>
#include <StepElement_CurveElementPurposeMember.hxx>
#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : StepElement_CurveElementPurpose
//purpose  : 
//=======================================================================

StepElement_CurveElementPurpose::StepElement_CurveElementPurpose ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepElement_CurveElementPurpose::CaseNum (const Handle(Standard_Transient)& ent) const
{
  return 0;
}

//=======================================================================
//function : CaseMem
//purpose  : 
//=======================================================================

Standard_Integer StepElement_CurveElementPurpose::CaseMem (const Handle(StepData_SelectMember)& ent) const
{
 if(ent.IsNull()) return 0;
 if(ent->Matches("EnumeratedCurveElementPurpose")) return 1;
 else if(ent->Matches("ApplicationDefinedElementPurpose")) return 2;
 else return 0;
}

//=======================================================================
//function : NewMember
//purpose  : 
//=======================================================================

Handle(StepData_SelectMember) StepElement_CurveElementPurpose::NewMember() const
{
 return new StepElement_CurveElementPurposeMember;
}

//=======================================================================
//function : SetEnumeratedCurveElementPurpose
//purpose  : 
//=======================================================================

void StepElement_CurveElementPurpose::SetEnumeratedCurveElementPurpose (const StepElement_EnumeratedCurveElementPurpose val)
{
  Handle(StepElement_CurveElementPurposeMember) SelMem = Handle(StepElement_CurveElementPurposeMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("ENUMERATED_CURVE_ELEMENT_PURPOSE");
 SelMem->SetName(name->ToCString());
 SelMem->SetEnum((Standard_Integer)val);
}

//=======================================================================
//function : EnumeratedCurveElementPurpose
//purpose  : 
//=======================================================================

StepElement_EnumeratedCurveElementPurpose StepElement_CurveElementPurpose::EnumeratedCurveElementPurpose () const
{
  Handle(StepElement_CurveElementPurposeMember) SelMem = Handle(StepElement_CurveElementPurposeMember)::DownCast(Value());
  if(SelMem.IsNull()) return StepElement_Axial;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
  name->AssignCat(SelMem->Name());
  Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("ENUMERATED_CURVEELEMENT_PURPOSE");
  if(name->IsDifferent(nameitem)) return StepElement_Axial;
  Standard_Integer numit = SelMem->Enum();
  StepElement_EnumeratedCurveElementPurpose val;
  switch(numit) {
  case 1 : val = StepElement_Axial; break;
  case 2 : val = StepElement_YYBending; break;
  case 3 : val = StepElement_ZZBending; break;
  case 4 : val = StepElement_Torsion; break;
  case 5 : val = StepElement_XYShear; break;
  case 6 : val = StepElement_XZShear; break;
  case 7 : val = StepElement_Warping; break;
    default : return StepElement_Axial;break;
  }
 return val;
}

//=======================================================================
//function : SetApplicationDefinedElementPurpose
//purpose  : 
//=======================================================================

void StepElement_CurveElementPurpose::SetApplicationDefinedElementPurpose (const Handle(TCollection_HAsciiString) &val)
{
  Handle(StepElement_CurveElementPurposeMember) SelMem = Handle(StepElement_CurveElementPurposeMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("APPLICATION_DEFINED_ELEMENT_PURPOSE");
 SelMem->SetName(name->ToCString());
 SelMem->SetString(val->ToCString());
}

//=======================================================================
//function : ApplicationDefinedElementPurpose
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepElement_CurveElementPurpose::ApplicationDefinedElementPurpose () const
{
  Handle(StepElement_CurveElementPurposeMember) SelMem = Handle(StepElement_CurveElementPurposeMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("APPLICATION_DEFINED_ELEMENT_PURPOSE");
 if(name->IsDifferent(nameitem)) return 0;
 Handle(TCollection_HAsciiString) val = new TCollection_HAsciiString;
 val->AssignCat(SelMem->String());
 return val;
}
