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
#include "RWStepVisual_RWTessellatedCurveSet.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <StepVisual_TessellatedCurveSet.hxx>

//=================================================================================================

RWStepVisual_RWTessellatedCurveSet::RWStepVisual_RWTessellatedCurveSet() {}

//=================================================================================================

void RWStepVisual_RWTessellatedCurveSet::ReadStep(
  const occ::handle<StepData_StepReaderData>&        data,
  const int                                          num,
  occ::handle<Interface_Check>&                      ach,
  const occ::handle<StepVisual_TessellatedCurveSet>& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 3, ach, "tessellated_curve_set"))
    return;

  // Inherited field : name
  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "name", ach, aName);

  occ::handle<StepVisual_CoordinatesList> aCoordList;
  data
    ->ReadEntity(num, 2, "coord_list", ach, STANDARD_TYPE(StepVisual_CoordinatesList), aCoordList);
  //--- Initialisation of the read entity ---
  int                                                                             nsub2;
  NCollection_Handle<NCollection_Vector<occ::handle<NCollection_HSequence<int>>>> aCurves =
    new NCollection_Vector<occ::handle<NCollection_HSequence<int>>>;
  if (data->ReadSubList(num, 3, "curves", ach, nsub2))
  {
    int nb2 = data->NbParams(nsub2);
    if (!nb2)
      return;

    for (int i = 1; i <= nb2; i++)
    {
      occ::handle<NCollection_HSequence<int>> aCurve = new NCollection_HSequence<int>;
      int                                     nsub3;
      if (data->ReadSubList(nsub2, i, "number_coordinates", ach, nsub3))
      {
        int nb3 = data->NbParams(nsub3);
        for (int j = 1; j <= nb3; j++)
        {
          int aVal = 0;
          if (data->ReadInteger(nsub3, j, "coordinates", ach, aVal))
            aCurve->Append(aVal);
        }
        aCurves->Append(aCurve);
      }
    }
  }
  ent->Init(aName, aCoordList, aCurves);
}

//=================================================================================================

void RWStepVisual_RWTessellatedCurveSet::WriteStep(
  StepData_StepWriter&                               SW,
  const occ::handle<StepVisual_TessellatedCurveSet>& ent) const
{
  // Inherited field : name
  SW.Send(ent->Name());

  // Own filed : coordinates
  SW.Send(ent->CoordList());

  // Own field : line_strips
  SW.OpenSub();
  for (int curveIt = 0; curveIt < ent->Curves()->Length(); curveIt++)
  {
    occ::handle<NCollection_HSequence<int>> aCurve = ent->Curves()->Value(curveIt);
    SW.OpenSub();
    for (int i = 1; i <= aCurve->Length(); i++)
      SW.Send(aCurve->Value(i));
    SW.CloseSub();
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepVisual_RWTessellatedCurveSet::Share(
  const occ::handle<StepVisual_TessellatedCurveSet>& ent,
  Interface_EntityIterator&                          iter) const
{
  // Own filed : coordinates
  iter.AddItem(ent->CoordList());
}
