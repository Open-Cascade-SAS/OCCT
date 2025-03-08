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
#include "RWStepShape_RWFace.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_HArray1OfFaceBound.hxx>

RWStepShape_RWFace::RWStepShape_RWFace() {}

void RWStepShape_RWFace::ReadStep(const Handle(StepData_StepReaderData)& data,
                                  const Standard_Integer                 num,
                                  Handle(Interface_Check)&               ach,
                                  const Handle(StepShape_Face)&          ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "face"))
    return;

  // --- inherited field : name ---

  Handle(TCollection_HAsciiString) aName;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : bounds ---

  Handle(StepShape_HArray1OfFaceBound) aBounds;
  Handle(StepShape_FaceBound)          anent2;
  Standard_Integer                     nsub2;
  if (data->ReadSubList(num, 2, "bounds", ach, nsub2))
  {
    Standard_Integer nb2 = data->NbParams(nsub2);
    aBounds              = new StepShape_HArray1OfFaceBound(1, nb2);
    for (Standard_Integer i2 = 1; i2 <= nb2; i2++)
    {
      // szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
      if (data
            ->ReadEntity(nsub2, i2, "face_bound", ach, STANDARD_TYPE(StepShape_FaceBound), anent2))
        aBounds->SetValue(i2, anent2);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, aBounds);
}

void RWStepShape_RWFace::WriteStep(StepData_StepWriter& SW, const Handle(StepShape_Face)& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : bounds ---

  SW.OpenSub();
  for (Standard_Integer i2 = 1; i2 <= ent->NbBounds(); i2++)
  {
    SW.Send(ent->BoundsValue(i2));
  }
  SW.CloseSub();
}

void RWStepShape_RWFace::Share(const Handle(StepShape_Face)& ent,
                               Interface_EntityIterator&     iter) const
{

  Standard_Integer nbElem1 = ent->NbBounds();
  for (Standard_Integer is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->BoundsValue(is1));
  }
}
