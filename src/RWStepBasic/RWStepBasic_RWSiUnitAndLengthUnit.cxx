// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

// pdn 24.12.98 t3d_opt.stp: treatment of unsorted uncertanties

#include <RWStepBasic_RWSiUnitAndLengthUnit.ixx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_LengthUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_SiPrefix.hxx>
#include <StepBasic_SiUnitName.hxx>
#include <RWStepBasic_RWSiUnit.hxx>


RWStepBasic_RWSiUnitAndLengthUnit::RWStepBasic_RWSiUnitAndLengthUnit ()
{
}

void RWStepBasic_RWSiUnitAndLengthUnit::ReadStep(const Handle(StepData_StepReaderData)& data,
						 const Standard_Integer num0,
						 Handle(Interface_Check)& ach,
						 const Handle(StepBasic_SiUnitAndLengthUnit)& ent) const
{
  Standard_Integer num = 0;  // num0;
  Standard_Boolean sorted = data->NamedForComplex("LENGTH_UNIT LNGUNT",num0,num,ach);

  // --- Instance of plex componant LengthUnit ---
  if (!data->CheckNbParams(num,0,ach,"length_unit")) return;

  if (!sorted) num = 0; //pdn unsorted case 
  sorted &=data->NamedForComplex("NAMED_UNIT NMDUNT",num0,num,ach);

  // --- Instance of common supertype NamedUnit ---
  if (!data->CheckNbParams(num,1,ach,"named_unit")) return;

  // --- field : dimensions ---
  // --- this field is redefined ---
  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->CheckDerived(num,1,"dimensions",ach,Standard_False);

  if (!sorted) num = 0; //pdn unsorted case 
  data->NamedForComplex("SI_UNIT SUNT",num0,num,ach);

  // --- Instance of plex componant SiUnit ---
  if (!data->CheckNbParams(num,2,ach,"si_unit")) return;

  // --- field : prefix ---
  RWStepBasic_RWSiUnit reader;
  StepBasic_SiPrefix aPrefix;
  Standard_Boolean hasAprefix = Standard_False;
  if (data->IsParamDefined(num,1)) {
    if (data->ParamType(num,1) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num,1);
      hasAprefix = reader.DecodePrefix(aPrefix,text);
      if(!hasAprefix)
	ach->AddFail("Enumeration si_prefix has not an allowed value");
    }
    else ach->AddFail("Parameter #2 (prefix) is not an enumeration");
  } 
  
  // --- field : name ---
  StepBasic_SiUnitName aName;
  if (data->ParamType(num,2) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num,2);
    if(!reader.DecodeName(aName,text))
      ach->AddFail("Enumeration si_unit_name has not an allowed value");
  }
  else ach->AddFail("Parameter #2 (name) is not an enumeration");

  //--- Initialisation of the red entity ---
  ent->Init(hasAprefix,aPrefix,aName);
}


void RWStepBasic_RWSiUnitAndLengthUnit::WriteStep(StepData_StepWriter& SW,
						  const Handle(StepBasic_SiUnitAndLengthUnit)& ent) const
{

  // --- Instance of plex componant LengthUnit ---
  SW.StartEntity("LENGTH_UNIT");

  // --- Instance of common supertype NamedUnit ---
  SW.StartEntity("NAMED_UNIT");
  
  // --- field : dimensions ---
  // --- redefined field ---
  SW.SendDerived();

  // --- Instance of plex componant SiUnit ---
  SW.StartEntity("SI_UNIT");
  
  // --- field : prefix ---
  RWStepBasic_RWSiUnit writer;
  Standard_Boolean hasAprefix = ent->HasPrefix();
  if (hasAprefix) 
    SW.SendEnum(writer.EncodePrefix(ent->Prefix()));
  else
    SW.SendUndef();
  
  // --- field : name ---
  SW.SendEnum(writer.EncodeName(ent->Name()));
}
