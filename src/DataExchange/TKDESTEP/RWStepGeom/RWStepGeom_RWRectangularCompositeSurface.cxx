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
#include "RWStepGeom_RWRectangularCompositeSurface.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_RectangularCompositeSurface.hxx>
#include <StepGeom_SurfacePatch.hxx>

RWStepGeom_RWRectangularCompositeSurface::RWStepGeom_RWRectangularCompositeSurface() = default;

void RWStepGeom_RWRectangularCompositeSurface::ReadStep(
  const occ::handle<StepData_StepReaderData>&              data,
  const int                                                num,
  occ::handle<Interface_Check>&                            ach,
  const occ::handle<StepGeom_RectangularCompositeSurface>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "rectangular_composite_surface"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : segments ---

  occ::handle<NCollection_HArray2<occ::handle<StepGeom_SurfacePatch>>> aSegments;
  occ::handle<StepGeom_SurfacePatch>                                   anent2;
  int                                                                  nsub2;
  if (data->ReadSubList(num, 2, "segments", ach, nsub2))
  {
    int nbi2  = data->NbParams(nsub2);
    int nbj2  = data->NbParams(data->ParamNumber(nsub2, 1));
    aSegments = new NCollection_HArray2<occ::handle<StepGeom_SurfacePatch>>(1, nbi2, 1, nbj2);
    for (int i2 = 1; i2 <= nbi2; i2++)
    {
      int nsi2;
      if (data->ReadSubList(nsub2, i2, "sub-part(segments)", ach, nsi2))
      {
        for (int j2 = 1; j2 <= nbj2; j2++)
        {
          // szv#4:S4163:12Mar99 `bool stat2 =` not needed
          if (data->ReadEntity(nsi2,
                               j2,
                               "surface_patch",
                               ach,
                               STANDARD_TYPE(StepGeom_SurfacePatch),
                               anent2))
            aSegments->SetValue(i2, j2, anent2);
        }
      }
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, aSegments);
}

void RWStepGeom_RWRectangularCompositeSurface::WriteStep(
  StepData_StepWriter&                                     SW,
  const occ::handle<StepGeom_RectangularCompositeSurface>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : segments ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbSegmentsI(); i2++)
  {
    SW.NewLine(false);
    SW.OpenSub();
    for (int j2 = 1; j2 <= ent->NbSegmentsJ(); j2++)
    {
      SW.Send(ent->SegmentsValue(i2, j2));
      SW.JoinLast(false);
    }
    SW.CloseSub();
  }
  SW.CloseSub();
}

void RWStepGeom_RWRectangularCompositeSurface::Share(
  const occ::handle<StepGeom_RectangularCompositeSurface>& ent,
  Interface_EntityIterator&                                iter) const
{

  int nbiElem1 = ent->NbSegmentsI();
  int nbjElem1 = ent->NbSegmentsJ();
  for (int is1 = 1; is1 <= nbiElem1; is1++)
  {
    for (int js1 = 1; js1 <= nbjElem1; js1++)
    {
      iter.GetOneItem(ent->SegmentsValue(is1, js1));
    }
  }
}
