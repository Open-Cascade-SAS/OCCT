// File:	StepElement_VolumeElementPurpose.cxx
// Created:	Tue Dec 10 18:13:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright:	Open CASCADE 2002

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
