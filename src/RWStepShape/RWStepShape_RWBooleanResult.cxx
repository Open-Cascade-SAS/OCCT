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


#include <RWStepShape_RWBooleanResult.ixx>
#include <StepShape_BooleanOperator.hxx>
#include <StepShape_BooleanOperand.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_BooleanResult.hxx>

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

