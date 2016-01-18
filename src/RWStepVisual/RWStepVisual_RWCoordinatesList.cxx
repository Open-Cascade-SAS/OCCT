// Created on: 2015-10-29
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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
#include <RWStepVisual_RWCoordinatesList.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_HArray1OfPresentationStyleAssignment.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <gp_XYZ.hxx>

//=======================================================================
//function : RWStepVisual_RWCoordinatesList
//purpose  : 
//=======================================================================
RWStepVisual_RWCoordinatesList::RWStepVisual_RWCoordinatesList () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWCoordinatesList::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num,
   Handle(Interface_Check)& ach,
   const Handle(StepVisual_CoordinatesList)& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 3, ach, "coordinate list")) return;

  // Inherited field : name
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);
  Standard_Integer nbP =0;
  data->ReadInteger(num, 2, "number_points", ach,nbP);
  
  Handle(TColgp_HArray1OfXYZ) aPoints;// = new TColgp_HArray1OfXYZ(1, nbP);
  Standard_Integer nsub2;
  if (data->ReadSubList (num,3,"items",ach,nsub2)) 
  {
    Standard_Integer nb2 = data->NbParams(nsub2);
    if( !nb2)
      return;
    aPoints = new TColgp_HArray1OfXYZ(1, nb2);
    for (Standard_Integer i = 1; i <= nb2; i++) 
    {
      gp_XYZ aXYZ(0.,0.,0.);
      Standard_Integer nsub3;
      if (data->ReadSubList (nsub2,i,"coordinates",ach,nsub3)) {
        Standard_Integer nb3 = data->NbParams(nsub3);
        if(nb3 > 3) {
          ach->AddWarning("More than 3 coordinates, ignored");
        }
        Standard_Integer nbcoord = Min (nb3, 3);
        for (Standard_Integer j = 1; j <= nbcoord; j++) {
          Standard_Real aVal =0.;
          if (data->ReadReal (nsub3,j,"coordinates",ach,aVal)) {
            aXYZ.SetCoord(j, aVal);
          }
        }
        
      }
      aPoints->SetValue(i, aXYZ);
    }
  }
	   

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPoints);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWCoordinatesList::WriteStep
  (StepData_StepWriter& SW, const  Handle(StepVisual_CoordinatesList)& ent) const
{
  // Inherited field : name
  SW.Send(ent->Name());

  // Own field : npoints
  SW.Send(ent->Points()->Length());

  // Own field : position_coords
  SW.OpenSub();
  for (Standard_Integer i = 1;  i <= ent->Points()->Length();  i++) {
    SW.OpenSub();
    gp_XYZ aPoint = ent->Points()->Value(i);
    SW.Send(aPoint.X());
    SW.Send(aPoint.Y());
    SW.Send(aPoint.Z());
    SW.CloseSub();
  }
  SW.CloseSub();
}
