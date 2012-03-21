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

#include <StepElement_MeasureOrUnspecifiedValue.ixx>
#include <StepElement_MeasureOrUnspecifiedValueMember.hxx>
#include <TCollection_HAsciiString.hxx>

static Standard_CString aCDM = "CONTEXT_DEPENDENT_MEASURE";
static Standard_CString anUV = "UNSPECIFIED_VALUE";


//=======================================================================
//function : StepElement_MeasureOrUnspecifiedValue
//purpose  : 
//=======================================================================

StepElement_MeasureOrUnspecifiedValue::StepElement_MeasureOrUnspecifiedValue ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepElement_MeasureOrUnspecifiedValue::CaseNum (const Handle(Standard_Transient)& ent) const
{
  return 0;
}

//=======================================================================
//function : CaseMem
//purpose  : 
//=======================================================================

Standard_Integer StepElement_MeasureOrUnspecifiedValue::CaseMem (const Handle(StepData_SelectMember)& ent) const
{
 if(ent.IsNull()) return 0;
 if(ent->Matches(aCDM)) return 1;
 else if(ent->Matches(anUV)) return 2;
 else return 0;
}

//=======================================================================
//function : NewMember
//purpose  : 
//=======================================================================

Handle(StepData_SelectMember) StepElement_MeasureOrUnspecifiedValue::NewMember() const
{
 return new StepElement_MeasureOrUnspecifiedValueMember;
}

//=======================================================================
//function : SetContextDependentMeasure
//purpose  : 
//=======================================================================

void StepElement_MeasureOrUnspecifiedValue::SetContextDependentMeasure (const Standard_Real val)
{
  Handle(StepElement_MeasureOrUnspecifiedValueMember) SelMem = Handle(StepElement_MeasureOrUnspecifiedValueMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("CONTEXT_DEPENDENT_MEASURE");
 SelMem->SetName(name->ToCString());
 SelMem->SetReal(val);
}

//=======================================================================
//function : ContextDependentMeasure
//purpose  : 
//=======================================================================

Standard_Real StepElement_MeasureOrUnspecifiedValue::ContextDependentMeasure () const
{
  Handle(StepElement_MeasureOrUnspecifiedValueMember) SelMem = Handle(StepElement_MeasureOrUnspecifiedValueMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("CONTEXT_DEPENDENT_MEASURE");
 if(name->IsDifferent(nameitem)) return 0;
 Standard_Real val = SelMem->Real();
 return val;
}

//=======================================================================
//function : SetUnspecifiedValue
//purpose  : 
//=======================================================================

void StepElement_MeasureOrUnspecifiedValue::SetUnspecifiedValue (const StepElement_UnspecifiedValue val)
{
  Handle(StepElement_MeasureOrUnspecifiedValueMember) SelMem = Handle(StepElement_MeasureOrUnspecifiedValueMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("UNSPECIFIED_VALUE");
 SelMem->SetName(name->ToCString());
 SelMem->SetEnum((Standard_Integer)val);
}

//=======================================================================
//function : UnspecifiedValue
//purpose  : 
//=======================================================================

StepElement_UnspecifiedValue StepElement_MeasureOrUnspecifiedValue::UnspecifiedValue () const
{
  Handle(StepElement_MeasureOrUnspecifiedValueMember) SelMem = Handle(StepElement_MeasureOrUnspecifiedValueMember)::DownCast(Value());
  if(SelMem.IsNull()) return StepElement_Unspecified;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("UNSPECIFIED_VALUE");
 if(name->IsDifferent(nameitem)) return StepElement_Unspecified;
 Standard_Integer numit = SelMem->Enum();
  StepElement_UnspecifiedValue val;
 switch(numit) {
    case 1 : val = StepElement_Unspecified; break;
    default : return StepElement_Unspecified;break;
  }
 return val;
}
