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


#include <RWStepShape_RWCsgSolid.ixx>
#include <StepShape_CsgSelect.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_CsgSolid.hxx>


RWStepShape_RWCsgSolid::RWStepShape_RWCsgSolid () {}

void RWStepShape_RWCsgSolid::ReadStep
(const Handle(StepData_StepReaderData)& data,
 const Standard_Integer num,
 Handle(Interface_Check)& ach,
 const Handle(StepShape_CsgSolid)& ent) const
{
  
  
  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,2,ach,"csg_solid")) return;
  
  // --- inherited field : name ---
  
  Handle(TCollection_HAsciiString) aName;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString (num,1,"name",ach,aName);
  
  // --- own field : treeRootExpression (Select Type) ---
  // Meme problem que pour la classe RWStepShape_RWBooleanResult
  
  //StepShape_CsgSelect aTreeRootExpression;
  //Standard_Boolean stat2;
  //stat2 = data->ReadEntity(num,2,"tree_root_expression",ach,aTreeRootExpression);
  
  Handle(StepShape_BooleanResult) aBooleanResult;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadEntity(num,2,"tree_root_expression",ach, STANDARD_TYPE(StepShape_BooleanResult), aBooleanResult);
  StepShape_CsgSelect aTreeRootExpression;
  aTreeRootExpression.SetBooleanResult(aBooleanResult);

  //--- Initialisation of the read entity ---
  
  
  ent->Init(aName, aTreeRootExpression);
}


void RWStepShape_RWCsgSolid::WriteStep
(StepData_StepWriter& SW,
 const Handle(StepShape_CsgSolid)& ent) const
{
  
  // --- inherited field name ---
  
  SW.Send(ent->Name());
  
  // --- own field : treeRootExpression ---
  // idem ...
  
  SW.Send(ent->TreeRootExpression().BooleanResult());
}


void RWStepShape_RWCsgSolid::Share(const Handle(StepShape_CsgSolid)& ent, Interface_EntityIterator& iter) const
{
  // idem ...
  iter.GetOneItem(ent->TreeRootExpression().BooleanResult());
}

