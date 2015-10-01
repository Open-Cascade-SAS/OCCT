// Created on: 1999-10-11
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Interface_Check.hxx>
#include <RWStepBasic_RWSiUnit.hxx>
#include <RWStepBasic_RWSiUnitAndAreaUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_SiUnitAndAreaUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWSiUnitAndAreaUnit::RWStepBasic_RWSiUnitAndAreaUnit ()
{
}

void RWStepBasic_RWSiUnitAndAreaUnit::ReadStep(const Handle(StepData_StepReaderData)& data,
					       const Standard_Integer num0,
					       Handle(Interface_Check)& ach,
					       const Handle(StepBasic_SiUnitAndAreaUnit)& ent) const
{
  Standard_Integer num = 0;
  data->NamedForComplex("AREA_UNIT","ARUNT",num0,num,ach);
  if (!data->CheckNbParams(num,0,ach,"area_unit")) return;

  data->NamedForComplex("NAMED_UNIT", "NMDUNT",num0,num,ach);
  if (!data->CheckNbParams(num,1,ach,"named_unit")) return;
  Handle(StepBasic_DimensionalExponents) aDimensions;
  data->ReadEntity(num, 1,"dimensions", ach, STANDARD_TYPE(StepBasic_DimensionalExponents), aDimensions);
  
  data->NamedForComplex("SI_UNIT", "SUNT",num0,num,ach);
  if (!data->CheckNbParams(num,2,ach,"si_unit")) return;
  
  RWStepBasic_RWSiUnit reader;
  StepBasic_SiPrefix aPrefix = StepBasic_spExa;
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
  
