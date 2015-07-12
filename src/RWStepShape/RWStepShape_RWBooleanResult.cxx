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
#include <RWStepShape_RWBooleanResult.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_BooleanOperand.hxx>
#include <StepShape_BooleanOperator.hxx>
#include <StepShape_BooleanResult.hxx>
#include <StepShape_SolidModel.hxx>
#include <TCollection_AsciiString.hxx>

// --- Enum : BooleanOperator ---
static TCollection_AsciiString boDifference(".DIFFERENCE.");
static TCollection_AsciiString boIntersection(".INTERSECTION.");
static TCollection_AsciiString boUnion(".UNION.");

RWStepShape_RWBooleanResult::RWStepShape_RWBooleanResult () {}

void RWStepShape_RWBooleanResult::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_BooleanResult)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"boolean_result")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : operator ---

	StepShape_BooleanOperator aOperator = StepShape_boDifference;
	if (data->ParamType(num,2) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,2);
	  if      (boDifference.IsEqual(text)) aOperator = StepShape_boDifference;
	  else if (boIntersection.IsEqual(text)) aOperator = StepShape_boIntersection;
	  else if (boUnion.IsEqual(text)) aOperator = StepShape_boUnion;
	  else ach->AddFail("Enumeration boolean_operator has not an allowed value");
	}
	else ach->AddFail("Parameter #2 (operator) is not an enumeration");

	// --- own field : firstOperand (is a select type) ---

	// firstOperand Type can be : SolidModel
	//                            HalfSpaceSolid
	//                            CsgPrimitive (Select Type)
	//                              sphere
	//                              block
	//                              right_angular_wedge
	//                              torus
	//                              right_circular_cone
	//                              right_circular_cylinder
	//                            BooleanResult
	// pour que le code soit correct, il faut tester ces types un par un
	// comme on n'implemente pas la CSG (Decembre 1995), on ne fait pas
	// Au cas ou cela change : il faut creer un select type 
	// StepShape_BooleanOperand dans lequel on met la vraie valeur instanciee,
	// on l'occurrence un autre Select Type si cette valeur est sphere, 
	// block, ...
	
	//StepShape_BooleanOperand aFirstOperand;
	//Standard_Boolean stat3;
	//stat3 = data->ReadEntity(num,3,"first_operand",ach,aFirstOperand);
	Handle(StepShape_SolidModel) aSolidModel1;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num,3,"first_operand",ach, STANDARD_TYPE(StepShape_SolidModel), aSolidModel1);
	StepShape_BooleanOperand aFirstOperand;
	aFirstOperand.SetSolidModel(aSolidModel1);

	// --- own field : secondOperand ---

	//StepShape_BooleanOperand aSecondOperand;
	//Standard_Boolean stat4;
	//stat4 = data->ReadEntity(num,4,"second_operand",ach,aSecondOperand);
	Handle(StepShape_SolidModel) aSolidModel2;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num,4,"second_operand",ach, STANDARD_TYPE(StepShape_SolidModel), aSolidModel2);

	StepShape_BooleanOperand aSecondOperand;
	aSecondOperand.SetSolidModel(aSolidModel2);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aOperator, aFirstOperand, aSecondOperand);
}


void RWStepShape_RWBooleanResult::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_BooleanResult)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : operator ---

	switch(ent->Operator()) {
	  case StepShape_boDifference : SW.SendEnum (boDifference); break;
	  case StepShape_boIntersection : SW.SendEnum (boIntersection); break;
	  case StepShape_boUnion : SW.SendEnum (boUnion); break;
	}

	// --- own field : firstOperand ---
	// --- idem au ReadStep : il faut envoyer le bon type :
	// au cas ou : switch(ent->FirstOperand().TypeOfContent())
	//               case 1: SW.Send(ent->FirstOperand().SolidModel())
	//               case 2: ...

	SW.Send(ent->FirstOperand().SolidModel());

	// --- own field : secondOperand ---

	SW.Send(ent->SecondOperand().SolidModel());
}


void RWStepShape_RWBooleanResult::Share(const Handle(StepShape_BooleanResult)& ent, Interface_EntityIterator& iter) const
{
  
  // idem
  iter.GetOneItem(ent->FirstOperand().SolidModel());
  
  iter.GetOneItem(ent->SecondOperand().SolidModel());
}

