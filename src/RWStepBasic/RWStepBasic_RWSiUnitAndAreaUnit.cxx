// Created on: 1999-10-11
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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



#include <RWStepBasic_RWSiUnitAndAreaUnit.ixx>
#include <StepBasic_SiUnit.hxx>
#include <RWStepBasic_RWSiUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>


RWStepBasic_RWSiUnitAndAreaUnit::RWStepBasic_RWSiUnitAndAreaUnit ()
{
}

void RWStepBasic_RWSiUnitAndAreaUnit::ReadStep(const Handle(StepData_StepReaderData)& data,
					       const Standard_Integer num0,
					       Handle(Interface_Check)& ach,
					       const Handle(StepBasic_SiUnitAndAreaUnit)& ent) const
{
  Standard_Integer num = 0;
  data->NamedForComplex("AREA_UNIT",num0,num,ach);
  if (!data->CheckNbParams(num,0,ach,"area_unit")) return;

  data->NamedForComplex("NAMED_UNIT NMDUNT",num0,num,ach);
  if (!data->CheckNbParams(num,1,ach,"named_unit")) return;
  Handle(StepBasic_DimensionalExponents) aDimensions;
  data->ReadEntity(num, 1,"dimensions", ach, STANDARD_TYPE(StepBasic_DimensionalExponents), aDimensions);
  
  data->NamedForComplex("SI_UNIT SUNT",num0,num,ach);
  if (!data->CheckNbParams(num,2,ach,"si_unit")) return;
  
  RWStepBasic_RWSiUnit reader;
  StepBasic_SiPrefix aPrefix;
  Standard_Boolean hasAprefix = Standard_False;
  if (data->IsParamDefined(num,1)) {
    if (data->ParamType(num,1) == Interface_ParamEnum) {
      Standard_CString text = data->ParamCValue(num,1);
      hasAprefix = reader.DecodePrefix(aPrefix,text);
      if(!hasAprefix){
	      ach->AddFail("Enumeration si_prefix has not an allowed value");
        return;
      }
    }
    else{
      ach->AddFail("Parameter #2 (prefix) is not an enumeration");
      return;
    }
  }
     
  StepBasic_SiUnitName aName;
  if (data->ParamType(num,2) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num,2);
    if(!reader.DecodeName(aName,text)){
      ach->AddFail("Enumeration si_unit_name has not an allowed value");
      return;
    }
  }
  else{
    ach->AddFail("Parameter #3 (name) is not an enumeration");
    return;
  }
  
  ent->Init(hasAprefix,aPrefix,aName);
  ent->SetDimensions(aDimensions);
}

void RWStepBasic_RWSiUnitAndAreaUnit::WriteStep(StepData_StepWriter& SW,
						const Handle(StepBasic_SiUnitAndAreaUnit)& ent) const
{
  SW.StartEntity("AREA_UNIT");
  SW.StartEntity("NAMED_UNIT");
  SW.Send(ent->Dimensions());
  SW.StartEntity("SI_UNIT");
  
  RWStepBasic_RWSiUnit writer;
  Standard_Boolean hasAprefix = ent->HasPrefix();
  if (hasAprefix) 
    SW.SendEnum(writer.EncodePrefix(ent->Prefix()));
  else
    SW.SendUndef();
  
  SW.SendEnum(writer.EncodeName(ent->Name()));   
}
  
