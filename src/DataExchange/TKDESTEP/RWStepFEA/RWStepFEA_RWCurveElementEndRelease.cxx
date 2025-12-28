// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include "RWStepFEA_RWCurveElementEndRelease.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_CurveElementEndRelease.hxx>

//=================================================================================================

RWStepFEA_RWCurveElementEndRelease::RWStepFEA_RWCurveElementEndRelease() {}

//=================================================================================================

void RWStepFEA_RWCurveElementEndRelease::ReadStep(
  const occ::handle<StepData_StepReaderData>&        data,
  const int                        num,
  occ::handle<Interface_Check>&                      ach,
  const occ::handle<StepFEA_CurveElementEndRelease>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 2, ach, "curve_element_end_release"))
    return;

  // Own fields of CurveElementEndRelease

  StepFEA_CurveElementEndCoordinateSystem aCoordinateSystem;
  data->ReadEntity(num, 1, "coordinate_system", ach, aCoordinateSystem);

  occ::handle<NCollection_HArray1<occ::handle<StepElement_CurveElementEndReleasePacket>>> aReleases;
  int                                          sub2 = 0;
  if (data->ReadSubList(num, 2, "releases", ach, sub2))
  {
    int nb0  = data->NbParams(sub2);
    aReleases             = new NCollection_HArray1<occ::handle<StepElement_CurveElementEndReleasePacket>>(1, nb0);
    int num2 = sub2;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      occ::handle<StepElement_CurveElementEndReleasePacket> anIt0;
      data->ReadEntity(num2,
                       i0,
                       "curve_element_end_release_packet",
                       ach,
                       STANDARD_TYPE(StepElement_CurveElementEndReleasePacket),
                       anIt0);
      aReleases->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aCoordinateSystem, aReleases);
}

//=================================================================================================

void RWStepFEA_RWCurveElementEndRelease::WriteStep(
  StepData_StepWriter&                          SW,
  const occ::handle<StepFEA_CurveElementEndRelease>& ent) const
{

  // Own fields of CurveElementEndRelease

  SW.Send(ent->CoordinateSystem().Value());

  SW.OpenSub();
  for (int i1 = 1; i1 <= ent->Releases()->Length(); i1++)
  {
    occ::handle<StepElement_CurveElementEndReleasePacket> Var0 = ent->Releases()->Value(i1);
    SW.Send(Var0);
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepFEA_RWCurveElementEndRelease::Share(const occ::handle<StepFEA_CurveElementEndRelease>& ent,
                                               Interface_EntityIterator& iter) const
{

  // Own fields of CurveElementEndRelease

  iter.AddItem(ent->CoordinateSystem().Value());

  for (int i2 = 1; i2 <= ent->Releases()->Length(); i2++)
  {
    occ::handle<StepElement_CurveElementEndReleasePacket> Var0 = ent->Releases()->Value(i2);
    iter.AddItem(Var0);
  }
}
