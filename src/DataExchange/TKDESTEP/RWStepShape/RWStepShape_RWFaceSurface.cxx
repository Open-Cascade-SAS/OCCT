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
#include "RWStepShape_RWFaceSurface.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Surface.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_FaceBound.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

RWStepShape_RWFaceSurface::RWStepShape_RWFaceSurface() {}

void RWStepShape_RWFaceSurface::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                         const int                                   num,
                                         occ::handle<Interface_Check>&               ach,
                                         const occ::handle<StepShape_FaceSurface>&   ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "face_surface"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : bounds ---

  occ::handle<NCollection_HArray1<occ::handle<StepShape_FaceBound>>> aBounds;
  occ::handle<StepShape_FaceBound>                                   anent2;
  int                                                                nsub2;
  if (data->ReadSubList(num, 2, "bounds", ach, nsub2))
  {
    int nb2 = data->NbParams(nsub2);
    aBounds = new NCollection_HArray1<occ::handle<StepShape_FaceBound>>(1, nb2);
    for (int i2 = 1; i2 <= nb2; i2++)
    {
      // szv#4:S4163:12Mar99 `bool stat2 =` not needed
      if (data
            ->ReadEntity(nsub2, i2, "face_bound", ach, STANDARD_TYPE(StepShape_FaceBound), anent2))
        aBounds->SetValue(i2, anent2);
    }
  }

  // --- own field : faceGeometry ---

  occ::handle<StepGeom_Surface> aFaceGeometry;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num, 3, "face_geometry", ach, STANDARD_TYPE(StepGeom_Surface), aFaceGeometry);

  // --- own field : sameSense ---

  bool aSameSense;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadBoolean(num, 4, "same_sense", ach, aSameSense);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aBounds, aFaceGeometry, aSameSense);
}

void RWStepShape_RWFaceSurface::WriteStep(StepData_StepWriter&                      SW,
                                          const occ::handle<StepShape_FaceSurface>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field bounds ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbBounds(); i2++)
  {
    SW.Send(ent->BoundsValue(i2));
  }
  SW.CloseSub();

  // --- own field : faceGeometry ---

  SW.Send(ent->FaceGeometry());

  // --- own field : sameSense ---

  SW.SendBoolean(ent->SameSense());
}

void RWStepShape_RWFaceSurface::Share(const occ::handle<StepShape_FaceSurface>& ent,
                                      Interface_EntityIterator&                 iter) const
{

  int nbElem1 = ent->NbBounds();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->BoundsValue(is1));
  }

  iter.GetOneItem(ent->FaceGeometry());
}
