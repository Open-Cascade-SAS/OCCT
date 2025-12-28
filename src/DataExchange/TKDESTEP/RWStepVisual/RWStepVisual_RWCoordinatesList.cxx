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
#include "RWStepVisual_RWCoordinatesList.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_XYZ.hxx>

//=================================================================================================

RWStepVisual_RWCoordinatesList::RWStepVisual_RWCoordinatesList() {}

//=================================================================================================

void RWStepVisual_RWCoordinatesList::ReadStep(const occ::handle<StepData_StepReaderData>&    data,
                                              const int                    num,
                                              occ::handle<Interface_Check>&                  ach,
                                              const occ::handle<StepVisual_CoordinatesList>& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 3, ach, "coordinate list"))
    return;

  // Inherited field : name
  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "name", ach, aName);
  int nbP = 0;
  data->ReadInteger(num, 2, "number_points", ach, nbP);

  occ::handle<NCollection_HArray1<gp_XYZ>> aPoints; // = new NCollection_HArray1<gp_XYZ>(1, nbP);
  int            nsub2;
  if (data->ReadSubList(num, 3, "items", ach, nsub2))
  {
    int nb2 = data->NbParams(nsub2);
    if (!nb2)
      return;
    aPoints = new NCollection_HArray1<gp_XYZ>(1, nb2);
    for (int i = 1; i <= nb2; i++)
    {
      gp_XYZ           aXYZ(0., 0., 0.);
      int nsub3;
      if (data->ReadSubList(nsub2, i, "coordinates", ach, nsub3))
      {
        int nb3 = data->NbParams(nsub3);
        if (nb3 > 3)
        {
          ach->AddWarning("More than 3 coordinates, ignored");
        }
        int nbcoord = std::min(nb3, 3);
        for (int j = 1; j <= nbcoord; j++)
        {
          double aVal = 0.;
          if (data->ReadReal(nsub3, j, "coordinates", ach, aVal))
          {
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

//=================================================================================================

void RWStepVisual_RWCoordinatesList::WriteStep(StepData_StepWriter&                      SW,
                                               const occ::handle<StepVisual_CoordinatesList>& ent) const
{
  // Inherited field : name
  SW.Send(ent->Name());

  // Own field : npoints
  SW.Send(ent->Points()->Length());

  // Own field : position_coords
  SW.OpenSub();
  for (int i = 1; i <= ent->Points()->Length(); i++)
  {
    SW.OpenSub();
    gp_XYZ aPoint = ent->Points()->Value(i);
    SW.Send(aPoint.X());
    SW.Send(aPoint.Y());
    SW.Send(aPoint.Z());
    SW.CloseSub();
  }
  SW.CloseSub();
}
