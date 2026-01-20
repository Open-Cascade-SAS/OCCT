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
#include "RWStepVisual_RWCompositeTextWithExtent.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_CompositeTextWithExtent.hxx>
#include <StepVisual_TextOrCharacter.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepVisual_PlanarExtent.hxx>
#include <StepVisual_TextOrCharacter.hxx>

RWStepVisual_RWCompositeTextWithExtent::RWStepVisual_RWCompositeTextWithExtent() {}

void RWStepVisual_RWCompositeTextWithExtent::ReadStep(
  const occ::handle<StepData_StepReaderData>&            data,
  const int                            num,
  occ::handle<Interface_Check>&                          ach,
  const occ::handle<StepVisual_CompositeTextWithExtent>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "composite_text_with_extent has not 3 parameter(s)"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : collectedText ---

  occ::handle<NCollection_HArray1<StepVisual_TextOrCharacter>> aCollectedText;
  StepVisual_TextOrCharacter                  aCollectedTextItem;
  int                            nsub2;
  nsub2 = data->SubListNumber(num, 2, false);
  if (nsub2 != 0)
  {
    int nb2 = data->NbParams(nsub2);
    aCollectedText       = new NCollection_HArray1<StepVisual_TextOrCharacter>(1, nb2);
    for (int i2 = 1; i2 <= nb2; i2++)
    {
      // szv#4:S4163:12Mar99 `bool stat2 =` not needed
      if (data->ReadEntity(nsub2, i2, "collected_text", ach, aCollectedTextItem))
        aCollectedText->SetValue(i2, aCollectedTextItem);
    }
  }
  else
  {
    ach->AddFail("Parameter #2 (collected_text) is not a LIST");
  }

  // --- own field : extent ---

  occ::handle<StepVisual_PlanarExtent> aExtent;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num, 3, "extent", ach, STANDARD_TYPE(StepVisual_PlanarExtent), aExtent);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aCollectedText, aExtent);
}

void RWStepVisual_RWCompositeTextWithExtent::WriteStep(
  StepData_StepWriter&                              SW,
  const occ::handle<StepVisual_CompositeTextWithExtent>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field collectedText ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbCollectedText(); i2++)
  {
    SW.Send(ent->CollectedTextValue(i2).Value());
  }
  SW.CloseSub();

  // --- own field : extent ---

  SW.Send(ent->Extent());
}

void RWStepVisual_RWCompositeTextWithExtent::Share(
  const occ::handle<StepVisual_CompositeTextWithExtent>& ent,
  Interface_EntityIterator&                         iter) const
{

  int nbElem1 = ent->NbCollectedText();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->CollectedTextValue(is1).Value());
  }

  iter.GetOneItem(ent->Extent());
}
