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
#include "RWStepGeom_RWPolyline.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Polyline.hxx>

RWStepGeom_RWPolyline::RWStepGeom_RWPolyline() {}

void RWStepGeom_RWPolyline::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                     const int                                   num,
                                     occ::handle<Interface_Check>&               ach,
                                     const occ::handle<StepGeom_Polyline>&       ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "polyline"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : points ---

  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aPoints;
  occ::handle<StepGeom_CartesianPoint>                                   anent2;
  int                                                                    nsub2;
  if (data->ReadSubList(num, 2, "points", ach, nsub2))
  {
    int nb2 = data->NbParams(nsub2);
    aPoints = new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, nb2);
    for (int i2 = 1; i2 <= nb2; i2++)
    {
      // szv#4:S4163:12Mar99 `bool stat2 =` not needed
      if (data->ReadEntity(nsub2,
                           i2,
                           "cartesian_point",
                           ach,
                           STANDARD_TYPE(StepGeom_CartesianPoint),
                           anent2))
        aPoints->SetValue(i2, anent2);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, aPoints);
}

void RWStepGeom_RWPolyline::WriteStep(StepData_StepWriter&                  SW,
                                      const occ::handle<StepGeom_Polyline>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : points ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbPoints(); i2++)
  {
    SW.Send(ent->PointsValue(i2));
  }
  SW.CloseSub();
}

void RWStepGeom_RWPolyline::Share(const occ::handle<StepGeom_Polyline>& ent,
                                  Interface_EntityIterator&             iter) const
{

  int nbElem1 = ent->NbPoints();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->PointsValue(is1));
  }
}
