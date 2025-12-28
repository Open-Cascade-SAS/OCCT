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
#include "RWStepGeom_RWBoundaryCurve.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BoundaryCurve.hxx>
#include <StepGeom_CompositeCurveSegment.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

RWStepGeom_RWBoundaryCurve::RWStepGeom_RWBoundaryCurve() {}

void RWStepGeom_RWBoundaryCurve::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                          const int                                   num,
                                          occ::handle<Interface_Check>&               ach,
                                          const occ::handle<StepGeom_BoundaryCurve>&  ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "boundary_curve"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : segments ---

  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CompositeCurveSegment>>> aSegments;
  occ::handle<StepGeom_CompositeCurveSegment>                                   anent2;
  int                                                                           nsub2;
  if (data->ReadSubList(num, 2, "segments", ach, nsub2))
  {
    int nb2   = data->NbParams(nsub2);
    aSegments = new NCollection_HArray1<occ::handle<StepGeom_CompositeCurveSegment>>(1, nb2);
    for (int i2 = 1; i2 <= nb2; i2++)
    {
      // szv#4:S4163:12Mar99 `bool stat2 =` not needed
      if (data->ReadEntity(nsub2,
                           i2,
                           "composite_curve_segment",
                           ach,
                           STANDARD_TYPE(StepGeom_CompositeCurveSegment),
                           anent2))
        aSegments->SetValue(i2, anent2);
    }
  }

  // --- inherited field : selfIntersect ---

  StepData_Logical aSelfIntersect;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadLogical(num, 3, "self_intersect", ach, aSelfIntersect);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aSegments, aSelfIntersect);
}

void RWStepGeom_RWBoundaryCurve::WriteStep(StepData_StepWriter&                       SW,
                                           const occ::handle<StepGeom_BoundaryCurve>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field segments ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbSegments(); i2++)
  {
    SW.Send(ent->SegmentsValue(i2));
  }
  SW.CloseSub();

  // --- inherited field selfIntersect ---

  SW.SendLogical(ent->SelfIntersect());
}

void RWStepGeom_RWBoundaryCurve::Share(const occ::handle<StepGeom_BoundaryCurve>& ent,
                                       Interface_EntityIterator&                  iter) const
{

  int nbElem1 = ent->NbSegments();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->SegmentsValue(is1));
  }
}
