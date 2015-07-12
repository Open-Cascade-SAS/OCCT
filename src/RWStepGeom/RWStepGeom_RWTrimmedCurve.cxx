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
#include <Interface_EntityIterator.hxx>
#include <RWStepGeom_RWTrimmedCurve.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_HArray1OfTrimmingSelect.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepGeom_TrimmingPreference.hxx>
#include <StepGeom_TrimmingSelect.hxx>
#include <TCollection_AsciiString.hxx>

// --- Enum : TrimmingPreference ---
static TCollection_AsciiString tpParameter(".PARAMETER.");
static TCollection_AsciiString tpUnspecified(".UNSPECIFIED.");
static TCollection_AsciiString tpCartesian(".CARTESIAN.");

RWStepGeom_RWTrimmedCurve::RWStepGeom_RWTrimmedCurve () {}

void RWStepGeom_RWTrimmedCurve::ReadStep
(const Handle(StepData_StepReaderData)& data,
 const Standard_Integer num,
 Handle(Interface_Check)& ach,
 const Handle(StepGeom_TrimmedCurve)& ent) const
{
  
  
  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,6,ach,"trimmed_curve")) return;
  
  // --- inherited field : name ---
  
  Handle(TCollection_HAsciiString) aName;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString (num,1,"name",ach,aName);
  
  // --- own field : basisCurve ---
  
  Handle(StepGeom_Curve) aBasisCurve;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadEntity(num, 2,"basis_curve", ach, STANDARD_TYPE(StepGeom_Curve), aBasisCurve);
  
  // --- own field : trim1 ---
  
  Handle(StepGeom_CartesianPoint) aCartesianPoint;
  //Standard_Real aParameterValue; //szv#4:S4163:12Mar99 unused
  
  Handle(StepGeom_HArray1OfTrimmingSelect) aTrim1;
  Standard_Integer nsub3;
  if (data->ReadSubList (num,3,"trim_1",ach,nsub3)) {
    Standard_Integer nb3 = data->NbParams(nsub3);
    aTrim1 = new StepGeom_HArray1OfTrimmingSelect (1, nb3);
    for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
      StepGeom_TrimmingSelect aTrim1Item;
      //szv#4:S4163:12Mar99 `Standard_Boolean stat3a =` not needed
      if (data->ReadEntity (nsub3,i3,"trim_1",ach,aTrim1Item))
	aTrim1->SetValue(i3,aTrim1Item);
    }
  }
  
  // --- own field : trim2 ---
  
  Handle(StepGeom_HArray1OfTrimmingSelect) aTrim2;
  Standard_Integer nsub4;
  if (data->ReadSubList (num,4,"trim_2",ach,nsub4)) {
    Standard_Integer nb4 = data->NbParams(nsub4);
    aTrim2 = new StepGeom_HArray1OfTrimmingSelect (1, nb4);
    for (Standard_Integer i4 = 1; i4 <= nb4; i4 ++) {
      
      StepGeom_TrimmingSelect aTrim2Item;
      //szv#4:S4163:12Mar99 `Standard_Boolean stat4a =` not needed
      if (data->ReadEntity (nsub4,i4,"trim_2",ach,aTrim2Item))
	aTrim2->SetValue(i4,aTrim2Item);
    }
  }
  
  // --- own field : senseAgreement ---
  
  Standard_Boolean aSenseAgreement;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
  data->ReadBoolean (num,5,"sense_agreement",ach,aSenseAgreement);
  
  // --- own field : masterRepresentation ---
  
  StepGeom_TrimmingPreference aMasterRepresentation = StepGeom_tpCartesian;
  if (data->ParamType(num,6) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num,6);
    if      (tpParameter.IsEqual(text)) aMasterRepresentation = StepGeom_tpParameter;
    else if (tpUnspecified.IsEqual(text)) aMasterRepresentation = StepGeom_tpUnspecified;
    else if (tpCartesian.IsEqual(text)) aMasterRepresentation = StepGeom_tpCartesian;
    else ach->AddFail("Enumeration trimming_preference has not an allowed value");
  }
  else ach->AddFail("Parameter #6 (master_representation) is not an enumeration");
  
  //--- Initialisation of the read entity ---
  
  
  ent->Init(aName, aBasisCurve, aTrim1, aTrim2, aSenseAgreement, aMasterRepresentation);
}


void RWStepGeom_RWTrimmedCurve::WriteStep
(StepData_StepWriter& SW,
 const Handle(StepGeom_TrimmedCurve)& ent) const
{
  
  // --- inherited field name ---
  
  SW.Send(ent->Name());
  
  // --- own field : basisCurve ---
  
  SW.Send(ent->BasisCurve());
  
  // --- own field : trim1 ---
  
  SW.OpenSub();
  for (Standard_Integer i2 = 1;  i2 <= ent->NbTrim1();  i2 ++) {
    SW.Send(ent->Trim1Value(i2).Value());
  }
  SW.CloseSub();
  
  // --- own field : trim2 ---
  
  SW.OpenSub();
  for (Standard_Integer i3 = 1;  i3 <= ent->NbTrim2();  i3 ++) {
    SW.Send(ent->Trim2Value(i3).Value());
  }
  SW.CloseSub();
  
  // --- own field : senseAgreement ---
  
  SW.SendBoolean(ent->SenseAgreement());
  
  // --- own field : masterRepresentation ---
  
  switch(ent->MasterRepresentation()) {
  case StepGeom_tpParameter : 
    SW.SendEnum (tpParameter); 
    break;
  case StepGeom_tpUnspecified : 
    SW.SendEnum (tpUnspecified); 
    break;
  case StepGeom_tpCartesian : 
    SW.SendEnum (tpCartesian); 
    break;
  }
}


void RWStepGeom_RWTrimmedCurve::Share(const Handle(StepGeom_TrimmedCurve)& ent, Interface_EntityIterator& iter) const
{

  iter.GetOneItem(ent->BasisCurve());
  
  Standard_Integer nbElem2 = ent->NbTrim1();
  for (Standard_Integer is2=1; is2<=nbElem2; is2 ++) {
    if (ent->Trim1Value(is2).CaseNumber() > 0) {
      iter.GetOneItem(ent->Trim1Value(is2).Value());
    }
  }
  
  Standard_Integer nbElem3 = ent->NbTrim2();
  for (Standard_Integer is3=1; is3<=nbElem3; is3 ++) {
    if (ent->Trim2Value(is3).CaseNumber() > 0) {
      iter.GetOneItem(ent->Trim2Value(is3).Value());
    }
  }
}

