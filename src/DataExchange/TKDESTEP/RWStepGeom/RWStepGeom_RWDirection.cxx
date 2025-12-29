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
#include <Interface_ShareTool.hxx>
#include "RWStepGeom_RWDirection.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Direction.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

RWStepGeom_RWDirection::RWStepGeom_RWDirection() = default;

void RWStepGeom_RWDirection::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                      const int                                   num,
                                      occ::handle<Interface_Check>&               ach,
                                      const occ::handle<StepGeom_Direction>&      ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "direction"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : directionRatios ---

  double aCoordinatesItem;
  int    aNSub2, aNbCoord = 0;
  double aXYZ[3] = {0., 0., 0.};
  if (data->ReadSubList(num, 2, "direction_ratios", ach, aNSub2))
  {
    int aNbElements = data->NbParams(aNSub2);
    if (aNbElements > 3)
    {
      ach->AddWarning("More than 3 direction ratios, ignored");
    }
    aNbCoord = std::min(aNbElements, 3);
    for (int i2 = 0; i2 < aNbCoord; i2++)
    {
      if (data->ReadReal(aNSub2, i2 + 1, "direction_ratios", ach, aCoordinatesItem))
      {
        aXYZ[i2] = aCoordinatesItem;
      }
    }
  }

  //--- Initialisation of the read entity ---
  if (aNbCoord == 3)
    ent->Init3D(aName, aXYZ[0], aXYZ[1], aXYZ[2]);
  else
    ent->Init2D(aName, aXYZ[0], aXYZ[1]);
}

void RWStepGeom_RWDirection::WriteStep(StepData_StepWriter&                   SW,
                                       const occ::handle<StepGeom_Direction>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : directionRatios ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbDirectionRatios(); i2++)
  {
    SW.Send(ent->DirectionRatiosValue(i2));
  }
  SW.CloseSub();
}

void RWStepGeom_RWDirection::Check(const occ::handle<StepGeom_Direction>& ent,
                                   const Interface_ShareTool&,
                                   occ::handle<Interface_Check>& ach) const
{
  int nbVal = ent->NbDirectionRatios();
  int i;
  for (i = 1; i <= nbVal; i++)
  {
    if (std::abs(ent->DirectionRatiosValue(i)) >= RealEpsilon())
      break;
  }
  if (i > nbVal)
    ach->AddFail("ERROR: DirectionRatios all 0.0");
}
