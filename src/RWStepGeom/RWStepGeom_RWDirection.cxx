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


#include <RWStepGeom_RWDirection.ixx>
#include <TColStd_HArray1OfReal.hxx>


RWStepGeom_RWDirection::RWStepGeom_RWDirection () {}

void RWStepGeom_RWDirection::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_Direction)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"direction")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : directionRatios ---

	Handle(TColStd_HArray1OfReal) aDirectionRatios;
	Standard_Real aDirectionRatiosItem;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"direction_ratios",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aDirectionRatios = new TColStd_HArray1OfReal (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	    if (data->ReadReal (nsub2,i2,"direction_ratios",ach,aDirectionRatiosItem))
	      aDirectionRatios->SetValue(i2,aDirectionRatiosItem);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, aDirectionRatios);
}


void RWStepGeom_RWDirection::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_Direction)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : directionRatios ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbDirectionRatios();  i2 ++) {
	  SW.Send(ent->DirectionRatiosValue(i2));
	}
	SW.CloseSub();
}


void  RWStepGeom_RWDirection::Check
  (const Handle(StepGeom_Direction)& ent,
   const Interface_ShareTool& ,
   Handle(Interface_Check)& ach) const
{
  Standard_Integer nbVal = ent->NbDirectionRatios();
  Standard_Integer i;
  for(i=1; i<=nbVal; i++) {
    if(Abs(ent->DirectionRatiosValue(i)) >= RealEpsilon())  break;
  }
  if(i>nbVal) ach->AddFail("ERROR: DirectionRatios all 0.0");
}
