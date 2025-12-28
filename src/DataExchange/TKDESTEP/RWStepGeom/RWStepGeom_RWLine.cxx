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

#include <Interface_EntityIterator.hxx>
#include "RWStepGeom_RWLine.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_Vector.hxx>

RWStepGeom_RWLine::RWStepGeom_RWLine() {}

void RWStepGeom_RWLine::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                 const int                                   num,
                                 occ::handle<Interface_Check>&               ach,
                                 const occ::handle<StepGeom_Line>&           ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "line"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : pnt ---

  occ::handle<StepGeom_CartesianPoint> aPnt;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "pnt", ach, STANDARD_TYPE(StepGeom_CartesianPoint), aPnt);

  // --- own field : dir ---

  occ::handle<StepGeom_Vector> aDir;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num, 3, "dir", ach, STANDARD_TYPE(StepGeom_Vector), aDir);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aPnt, aDir);
}

void RWStepGeom_RWLine::WriteStep(StepData_StepWriter&              SW,
                                  const occ::handle<StepGeom_Line>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : pnt ---

  SW.Send(ent->Pnt());

  // --- own field : dir ---

  SW.Send(ent->Dir());
}

void RWStepGeom_RWLine::Share(const occ::handle<StepGeom_Line>& ent,
                              Interface_EntityIterator&         iter) const
{

  iter.GetOneItem(ent->Pnt());

  iter.GetOneItem(ent->Dir());
}
