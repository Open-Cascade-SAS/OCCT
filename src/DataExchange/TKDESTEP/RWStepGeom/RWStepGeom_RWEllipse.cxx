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
#include <Interface_ShareTool.hxx>
#include "RWStepGeom_RWEllipse.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Ellipse.hxx>

RWStepGeom_RWEllipse::RWStepGeom_RWEllipse() {}

void RWStepGeom_RWEllipse::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                    const int                                   num,
                                    occ::handle<Interface_Check>&               ach,
                                    const occ::handle<StepGeom_Ellipse>&        ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "ellipse"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : position ---

  StepGeom_Axis2Placement aPosition;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "position", ach, aPosition);

  // --- own field : semiAxis1 ---

  double aSemiAxis1;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadReal(num, 3, "semi_axis_1", ach, aSemiAxis1);

  // --- own field : semiAxis2 ---

  double aSemiAxis2;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadReal(num, 4, "semi_axis_2", ach, aSemiAxis2);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aPosition, aSemiAxis1, aSemiAxis2);
}

void RWStepGeom_RWEllipse::WriteStep(StepData_StepWriter&                 SW,
                                     const occ::handle<StepGeom_Ellipse>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field position ---

  SW.Send(ent->Position().Value());

  // --- own field : semiAxis1 ---

  SW.Send(ent->SemiAxis1());

  // --- own field : semiAxis2 ---

  SW.Send(ent->SemiAxis2());
}

void RWStepGeom_RWEllipse::Share(const occ::handle<StepGeom_Ellipse>& ent,
                                 Interface_EntityIterator&            iter) const
{

  iter.GetOneItem(ent->Position().Value());
}

void RWStepGeom_RWEllipse::Check(const occ::handle<StepGeom_Ellipse>& ent,
                                 const Interface_ShareTool&,
                                 occ::handle<Interface_Check>& ach) const
{
  if (ent->SemiAxis1() < ent->SemiAxis2())
    ach->AddWarning("ERROR: Ellipse: SemiMajor smaller than SemiMinor");
}
