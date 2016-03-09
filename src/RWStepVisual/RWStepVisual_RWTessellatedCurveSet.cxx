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
#include <RWStepVisual_RWTessellatedCurveSet.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <StepVisual_TessellatedCurveSet.hxx>

//=======================================================================
//function : RWStepVisual_RWTessellatedCurveSet
//purpose  : 
//=======================================================================
RWStepVisual_RWTessellatedCurveSet::RWStepVisual_RWTessellatedCurveSet () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWTessellatedCurveSet::ReadStep
  (const Handle(StepData_StepReaderData)& data,
  const Standard_Integer num,
  Handle(Interface_Check)& ach,
  const Handle(StepVisual_TessellatedCurveSet)& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 3, ach, "tessellated_curve_set")) return;

  // Inherited field : name
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(StepVisual_CoordinatesList) aCoordList;
  data->ReadEntity (num, 2,"coord_list",ach,STANDARD_TYPE(StepVisual_CoordinatesList), aCoordList);
  //--- Initialisation of the read entity ---
  Standard_Integer nsub2;
  NCollection_Handle<StepVisual_VectorOfHSequenceOfInteger> aCurves = new StepVisual_VectorOfHSequenceOfInteger;
  if (data->ReadSubList (num,3,"curves",ach,nsub2)) 
  {
    Standard_Integer nb2 = data->NbParams(nsub2);
    if( !nb2)
      return;

    for (Standard_Integer i = 1; i <= nb2; i++) 
    {
      Handle(TColStd_HSequenceOfInteger) aCurve = new TColStd_HSequenceOfInteger;
      Standard_Integer nsub3;
      if (data->ReadSubList (nsub2,i,"number_coordinates",ach,nsub3)) {
        Standard_Integer nb3 = data->NbParams(nsub3);
        for (Standard_Integer j = 1; j <= nb3; j++) {
          Standard_Integer aVal =0;
          if (data->ReadInteger (nsub3,j,"coordinates",ach,aVal)) 
            aCurve->Append(aVal);

        }
        aCurves->Append(aCurve);

      }
    }
  }
  ent->Init(aName, aCoordList, aCurves);
  
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWTessellatedCurveSet::WriteStep
  (StepData_StepWriter& SW,
  const Handle(StepVisual_TessellatedCurveSet)& ent) const
{
  // Inherited field : name
  SW.Send(ent->Name());

  // Own filed : coordinates
  SW.Send(ent->CoordList());

  // Own field : line_strips
  SW.OpenSub();
  for (Standard_Integer curveIt = 0; curveIt < ent->Curves()->Length(); curveIt++) {
    Handle(TColStd_HSequenceOfInteger) aCurve = ent->Curves()->Value(curveIt);
    SW.OpenSub();
    for (Standard_Integer i = 1; i <= aCurve->Length(); i++)
        SW.Send(aCurve->Value(i));
    SW.CloseSub();
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================
void RWStepVisual_RWTessellatedCurveSet::Share (const Handle(StepVisual_TessellatedCurveSet) &ent,
                                                Interface_EntityIterator& iter) const
{
  // Own filed : coordinates
  iter.AddItem (ent->CoordList());
}

