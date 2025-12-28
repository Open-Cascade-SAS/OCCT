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
#include "RWStepVisual_RWSurfaceSideStyle.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_SurfaceSideStyle.hxx>
#include <StepVisual_SurfaceStyleElementSelect.hxx>

RWStepVisual_RWSurfaceSideStyle::RWStepVisual_RWSurfaceSideStyle() {}

void RWStepVisual_RWSurfaceSideStyle::ReadStep(const occ::handle<StepData_StepReaderData>&     data,
                                               const int                     num,
                                               occ::handle<Interface_Check>&                   ach,
                                               const occ::handle<StepVisual_SurfaceSideStyle>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "surface_side_style"))
    return;

  // --- own field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : styles ---

  occ::handle<NCollection_HArray1<StepVisual_SurfaceStyleElementSelect>> aStyles;
  StepVisual_SurfaceStyleElementSelect                  aStylesItem;
  int                                      nsub2;
  if (data->ReadSubList(num, 2, "styles", ach, nsub2))
  {
    int nb2 = data->NbParams(nsub2);
    aStyles              = new NCollection_HArray1<StepVisual_SurfaceStyleElementSelect>(1, nb2);
    for (int i2 = 1; i2 <= nb2; i2++)
    {
      // szv#4:S4163:12Mar99 `bool stat2 =` not needed
      if (data->ReadEntity(nsub2, i2, "styles", ach, aStylesItem))
        aStyles->SetValue(i2, aStylesItem);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, aStyles);
}

void RWStepVisual_RWSurfaceSideStyle::WriteStep(
  StepData_StepWriter&                       SW,
  const occ::handle<StepVisual_SurfaceSideStyle>& ent) const
{

  // --- own field : name ---

  SW.Send(ent->Name());

  // --- own field : styles ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbStyles(); i2++)
  {
    SW.Send(ent->StylesValue(i2).Value());
  }
  SW.CloseSub();
}

void RWStepVisual_RWSurfaceSideStyle::Share(const occ::handle<StepVisual_SurfaceSideStyle>& ent,
                                            Interface_EntityIterator&                  iter) const
{

  int nbElem1 = ent->NbStyles();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->StylesValue(is1).Value());
  }
}
