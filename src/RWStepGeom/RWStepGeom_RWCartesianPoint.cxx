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
#include <RWStepGeom_RWCartesianPoint.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <TCollection_HAsciiString.hxx>

//#include <TColStd_HArray1OfReal.hxx>
RWStepGeom_RWCartesianPoint::RWStepGeom_RWCartesianPoint () {}

void RWStepGeom_RWCartesianPoint::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_CartesianPoint)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"cartesian_point")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	//  Protected
	if (!data->ReadString (num,1,"name",ach,aName)) {
	  ach->Mend("Set to empty string");
	  aName = new TCollection_HAsciiString;
	}

	// --- own field : coordinates ---
	//  OPTIMISED : X Y Z directly read and set

//	Handle(TColStd_HArray1OfReal) aCoordinates;
	Standard_Real aCoordinatesItem;
	Standard_Integer nsub2, nbcoord=0;
	Standard_Real XYZ[3] = {0.,0.,0.};
	if (data->ReadSubList (num,2,"coordinates",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
          if(nb2 > 3) {
            ach->AddWarning("More than 3 coordinates, ignored");
          }
	  nbcoord = Min (nb2, 3);
	  for (Standard_Integer i2 = 0; i2 < nbcoord; i2 ++) {
	    if (data->ReadReal (nsub2,i2+1,"coordinates",ach,aCoordinatesItem)) {
              XYZ[i2] = aCoordinatesItem;
            }
	  }
	}

	//--- Initialisation of the read entity ---


//	ent->Init(aName, aCoordinates);
	if (nbcoord == 3) ent->Init3D (aName, XYZ[0],XYZ[1],XYZ[2]);
	else              ent->Init2D (aName, XYZ[0],XYZ[1]);
}


void RWStepGeom_RWCartesianPoint::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_CartesianPoint)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : coordinates ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbCoordinates();  i2 ++) {
	  SW.Send(ent->CoordinatesValue(i2));
	}
	SW.CloseSub();
}
