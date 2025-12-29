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
#include "RWStepVisual_RWStyledItem.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_StyledItem.hxx>

RWStepVisual_RWStyledItem::RWStepVisual_RWStyledItem() = default;

void RWStepVisual_RWStyledItem::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                         const int                                   num,
                                         occ::handle<Interface_Check>&               ach,
                                         const occ::handle<StepVisual_StyledItem>&   ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "styled_item"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : styles ---

  occ::handle<NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>> aStyles;
  occ::handle<StepVisual_PresentationStyleAssignment>                                   anent2;
  int                                                                                   nsub2;
  if (data->ReadSubList(num, 2, "styles", ach, nsub2))
  {
    int nb2 = data->NbParams(nsub2);
    aStyles = new NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>(1, nb2);
    for (int i2 = 1; i2 <= nb2; i2++)
    {
      // szv#4:S4163:12Mar99 `bool stat2 =` not needed
      if (data->ReadEntity(nsub2,
                           i2,
                           "presentation_style_assignment",
                           ach,
                           STANDARD_TYPE(StepVisual_PresentationStyleAssignment),
                           anent2))
        aStyles->SetValue(i2, anent2);
    }
  }

  // --- own field : item ---

  occ::handle<Standard_Transient> aItem;
  data->ReadEntity(num, 3, "item", ach, STANDARD_TYPE(Standard_Transient), aItem);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aStyles, aItem);
}

void RWStepVisual_RWStyledItem::WriteStep(StepData_StepWriter&                      SW,
                                          const occ::handle<StepVisual_StyledItem>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : styles ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbStyles(); i2++)
  {
    SW.Send(ent->StylesValue(i2));
  }
  SW.CloseSub();

  // --- own field : item ---

  SW.Send(ent->Item());
}

void RWStepVisual_RWStyledItem::Share(const occ::handle<StepVisual_StyledItem>& ent,
                                      Interface_EntityIterator&                 iter) const
{

  int nbElem1 = ent->NbStyles();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->StylesValue(is1));
  }

  iter.GetOneItem(ent->Item());
}
