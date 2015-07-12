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

//gka 05.03.99 S4134 upgrade from CD to DIS

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_FloatWriter.hxx>
#include <Interface_MSG.hxx>
#include <RWStepBasic_RWUncertaintyMeasureWithUnit.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_UncertaintyMeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_AsciiString.hxx>

#include <string.h>
//=======================================================================
//function : RWStepBasic_RWUncertaintyMeasureWithUnit
//purpose  : 
//=======================================================================
RWStepBasic_RWUncertaintyMeasureWithUnit::RWStepBasic_RWUncertaintyMeasureWithUnit () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWUncertaintyMeasureWithUnit::ReadStep (const Handle(StepData_StepReaderData)& data,
							 const Standard_Integer num,
							 Handle(Interface_Check)& ach,
							 const Handle(StepBasic_UncertaintyMeasureWithUnit)& ent) const
{
  if (data->IsComplex (num)) {

    ach->AddWarning ("Complex Type not allowed, only suitable values are read");

//   CATIA ecrit des trucs complexes, comme suit :
//  LENGTH_MEASURE_WITH_UNIT ()
//  MEASURE_WITH_UNIT (LENGTH_MEASURE(val),#..)
//  UNCERTAINTY_MEASURE_WITH_UNIT ('str1','str2')
//   -> c est illicite, LENGTH_MEASURE est un invite importun
//  On relit en ignorant ce terme
//    REMARQUE : le ReadWriteModule a deja fait le filtrage des types

//    LENGTH_MEASURE_WITH_UNIT : on passe
    Standard_Integer num1 = num;

//  MEASURE_WITH_UNIT
    num1 = data->NextForComplex (num1);

    if (!data->CheckNbParams(num1,2,ach,"measure_with_unit")) return;

    // --- inherited field : valueComponent ---
    Handle(StepBasic_MeasureValueMember) mvc = new StepBasic_MeasureValueMember;
    data->ReadMember (num1,1, "value_component", ach, mvc);

    // --- inherited field : unitComponent ---
    StepBasic_Unit aUnitComponent;
    data->ReadEntity(num1, 2,"unit_component", ach, aUnitComponent);

//  UNCERTAINTY_MEASURE_WITH_UNIT (ce qu il en reste !)
    num1 = data->NextForComplex (num1);
  
    if (!data->CheckNbParams(num1,2,ach,"uncertainty_measure_with_unit")) return;
  
    // --- own field : name ---
    Handle(TCollection_HAsciiString) aName;
    data->ReadString (num1,1,"name",ach,aName);
  
    // --- own field : description ---
    Handle(TCollection_HAsciiString) aDescription;
    if (data->IsParamDefined (num1,2)) { //gka 05.03.99 S4134 upgrade from CD to DIS
      data->ReadString (num1,2,"description",ach,aDescription);
    }
//  Ca yest, on peut faire l init

    ent->Init(mvc, aUnitComponent, aName, aDescription);
    return;
  }
  
  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,4,ach,"uncertainty_measure_with_unit")) return;
  
  // --- inherited field : valueComponent ---
  //Standard_Real aValueComponent;
  //Standard_Boolean stat1;
  //stat1 = data->ReadReal (num,1,"value_component",ach,aValueComponent);
  // --- Update 12-02-96 by FMA  =>  31-MAR-1997 by CKY
  Handle(StepBasic_MeasureValueMember) mvc = new StepBasic_MeasureValueMember;
  data->ReadMember (num,1, "value_component", ach, mvc);

  // --- inherited field : unitComponent ---
  StepBasic_Unit aUnitComponent;
  data->ReadEntity(num, 2,"unit_component", ach, aUnitComponent);
  
  // --- own field : name ---
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num,3,"name",ach,aName);
  
  // --- own field : description ---
  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num,4,"description",ach,aDescription);
  
  //--- Initialisation of the read entity ---
  ent->Init(mvc, aUnitComponent, aName, aDescription);
}


//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWUncertaintyMeasureWithUnit::WriteStep (StepData_StepWriter& SW,
							  const Handle(StepBasic_UncertaintyMeasureWithUnit)& ent) const
{
  
  // --- inherited field valueComponent ---
  
  // Update 12-02-96 by FMA
  // The Value shall be Typed - LENGTH_MEASURE, PLANE_ANGLE_MEASURE,
  //                            SOLID_ANGLE_MEASURE
  // The actual state is LENGTH_MEASURE ONLY
  // UPDATE to be done later but mandatory

//  char lm[100],lmv[50];
//  Interface_FloatWriter::Convert
//    (Interface_MSG::Intervalled(ent->ValueComponent()*0.98,5,Standard_True),
//     lmv,Standard_True, 10.,0.1,"%E","%E");
//  sprintf (lm,"LENGTH_MEASURE(%s)",lmv);
//  SW.SendString(lm);

//  SW.AddString("LENGTH_MEASURE");
//  SW.OpenSub();

//   UPDATE 31-MARS-1997 by CKY : ca devrait etre bon desormais
  SW.Send(ent->ValueComponentMember());
//  SW.CloseSub();
  
  // --- inherited field unitComponent ---
  SW.Send(ent->UnitComponent().Value());
  
  // --- own field : name ---
  SW.Send(ent->Name());
  
  // --- own field : description ---
  SW.Send(ent->Description());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWUncertaintyMeasureWithUnit::Share (const Handle(StepBasic_UncertaintyMeasureWithUnit)& ent, 
						      Interface_EntityIterator& iter) const
{
  
  iter.GetOneItem(ent->UnitComponent().Value());
}

