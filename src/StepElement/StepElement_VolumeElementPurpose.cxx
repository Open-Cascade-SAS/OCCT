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

#include <StepElement_VolumeElementPurpose.ixx>
#include <StepElement_VolumeElementPurposeMember.hxx>
#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : StepElement_VolumeElementPurpose
//purpose  : 
//=======================================================================

StepElement_VolumeElementPurpose::StepElement_VolumeElementPurpose ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepElement_VolumeElementPurpose::CaseNum (const Handle(Standard_Transient)& ent) const
{
  return 0;
}

//=======================================================================
//function : CaseMem
//purpose  : 
//=======================================================================

Standard_Integer StepElement_VolumeElementPurpose::CaseMem (const Handle(StepData_SelectMember)& ent) const
{
 if(ent.IsNull()) return 0;
 if(ent->Matches("EnumeratedVolumeElementPurpose")) return 1;
 else if(ent->Matches("ApplicationDefinedElementPurpose")) return 2;
 else return 0;
}

//=======================================================================
//function : NewMember
//purpose  : 
//=======================================================================

Handle(StepData_SelectMember) StepElement_VolumeElementPurpose::NewMember() const
{
 return new StepElement_VolumeElementPurposeMember;
}

//=======================================================================
//function : SetEnumeratedVolumeElementPurpose
//purpose  : 
//=======================================================================

void StepElement_VolumeElementPurpose::SetEnumeratedVolumeElementPurpose (const StepElement_EnumeratedVolumeElementPurpose val)
{
  Handle(StepElement_VolumeElementPurposeMember) SelMem = Handle(StepElement_VolumeElementPurposeMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("EnumeratedVolumeElementPurpose");
 SelMem->SetName(name->ToCString());
 SelMem->SetEnum((Standard_Integer)val);
}

//=======================================================================
//function : EnumeratedVolumeElementPurpose
//purpose  : 
//=======================================================================

StepElement_EnumeratedVolumeElementPurpose StepElement_VolumeElementPurpose::EnumeratedVolumeElementPurpose () const
{
  Handle(StepElement_VolumeElementPurposeMember) SelMem = Handle(StepElement_VolumeElementPurposeMember)::DownCast(Value());
  if(SelMem.IsNull()) return StepElement_StressDisplacement;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
  name->AssignCat(SelMem->Name());
  Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("EnumeratedVolumeElementPurpose");
  if(name->IsDifferent(nameitem)) return StepElement_StressDisplacement;
  Standard_Integer numit = SelMem->Enum();
  StepElement_EnumeratedVolumeElementPurpose val;
  switch(numit) {
  case 1 : val = StepElement_StressDisplacement; break;
    default : return StepElement_StressDisplacement;break;
  }
 return val;
}

//=======================================================================
//function : SetApplicationDefinedElementPurpose
//purpose  : 
//=======================================================================

void StepElement_VolumeElementPurpose::SetApplicationDefinedElementPurpose (const Handle(TCollection_HAsciiString) &val)
{
  Handle(StepElement_VolumeElementPurposeMember) SelMem = Handle(StepElement_VolumeElementPurposeMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("ApplicationDefinedElementPurpose");
 SelMem->SetName(name->ToCString());
 SelMem->SetString(val->ToCString());
}

//=======================================================================
//function : ApplicationDefinedElementPurpose
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepElement_VolumeElementPurpose::ApplicationDefinedElementPurpose () const
{
  Handle(StepElement_VolumeElementPurposeMember) SelMem = Handle(StepElement_VolumeElementPurposeMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("ApplicationDefinedElementPurpose");
 if(name->IsDifferent(nameitem)) return 0;
 Handle(TCollection_HAsciiString) val = new TCollection_HAsciiString;
 val->AssignCat(SelMem->String());
 return val;
}
