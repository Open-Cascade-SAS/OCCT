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

// gka 05.03.99 S4134 upgrade from CD to DIS

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include "RWStepBasic_RWProduct.pxx"
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductContext.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWProduct::RWStepBasic_RWProduct() {}

void RWStepBasic_RWProduct::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                     const int                                   num,
                                     occ::handle<Interface_Check>&               ach,
                                     const occ::handle<StepBasic_Product>&       ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "product"))
    return;

  // --- own field : id ---

  occ::handle<TCollection_HAsciiString> aId;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "id", ach, aId);

  // --- own field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadString(num, 2, "name", ach, aName);

  // --- own field : description ---

  occ::handle<TCollection_HAsciiString> aDescription;
  if (data->IsParamDefined(num, 3))
  { // gka 05.03.99 S4134 upgrade from CD to DIS
    // szv#4:S4163:12Mar99 `bool stat3 =` not needed
    data->ReadString(num, 3, "description", ach, aDescription);
  }
  // --- own field : frameOfReference ---

  occ::handle<NCollection_HArray1<occ::handle<StepBasic_ProductContext>>> aFrameOfReference;
  occ::handle<StepBasic_ProductContext>                                   anent4;
  int                                                                     nsub4;
  if (data->ReadSubList(num, 4, "frame_of_reference", ach, nsub4))
  {
    int nb4           = data->NbParams(nsub4);
    aFrameOfReference = new NCollection_HArray1<occ::handle<StepBasic_ProductContext>>(1, nb4);
    for (int i4 = 1; i4 <= nb4; i4++)
    {
      // szv#4:S4163:12Mar99 `bool stat4 =` not needed
      if (data->ReadEntity(nsub4,
                           i4,
                           "product_context",
                           ach,
                           STANDARD_TYPE(StepBasic_ProductContext),
                           anent4))
        aFrameOfReference->SetValue(i4, anent4);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aId, aName, aDescription, aFrameOfReference);
}

void RWStepBasic_RWProduct::WriteStep(StepData_StepWriter&                  SW,
                                      const occ::handle<StepBasic_Product>& ent) const
{

  // --- own field : id ---

  SW.Send(ent->Id());

  // --- own field : name ---

  SW.Send(ent->Name());

  // --- own field : description ---

  if (!ent->Description().IsNull())
  {
    SW.Send(ent->Description());
  }
  else
  {
    SW.SendUndef();
  }

  // --- own field : frameOfReference ---

  SW.OpenSub();
  for (int i4 = 1; i4 <= ent->NbFrameOfReference(); i4++)
  {
    SW.Send(ent->FrameOfReferenceValue(i4));
  }
  SW.CloseSub();
}

void RWStepBasic_RWProduct::Share(const occ::handle<StepBasic_Product>& ent,
                                  Interface_EntityIterator&             iter) const
{

  int nbElem1 = ent->NbFrameOfReference();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->FrameOfReferenceValue(is1));
  }
}
