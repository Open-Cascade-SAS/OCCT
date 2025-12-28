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
#include "RWStepBasic_RWProductType.pxx"
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductType.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWProductType::RWStepBasic_RWProductType() {}

void RWStepBasic_RWProductType::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                         const int                 num,
                                         occ::handle<Interface_Check>&               ach,
                                         const occ::handle<StepBasic_ProductType>&   ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "product_type"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : description ---

  occ::handle<TCollection_HAsciiString> aDescription;
  bool                 hasAdescription = true;
  if (data->IsParamDefined(num, 2))
  {
    // szv#4:S4163:12Mar99 `bool stat2 =` not needed
    data->ReadString(num, 2, "description", ach, aDescription);
  }
  else
  {
    hasAdescription = false;
    aDescription.Nullify();
  }

  // --- inherited field : products ---

  occ::handle<NCollection_HArray1<occ::handle<StepBasic_Product>>> aProducts;
  occ::handle<StepBasic_Product>          anent3;
  int                   nsub3;
  if (data->ReadSubList(num, 3, "products", ach, nsub3))
  {
    int nb3 = data->NbParams(nsub3);
    aProducts            = new NCollection_HArray1<occ::handle<StepBasic_Product>>(1, nb3);
    for (int i3 = 1; i3 <= nb3; i3++)
    {
      // szv#4:S4163:12Mar99 `bool stat3 =` not needed
      if (data->ReadEntity(nsub3, i3, "product", ach, STANDARD_TYPE(StepBasic_Product), anent3))
        aProducts->SetValue(i3, anent3);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, hasAdescription, aDescription, aProducts);
}

void RWStepBasic_RWProductType::WriteStep(StepData_StepWriter&                 SW,
                                          const occ::handle<StepBasic_ProductType>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field description ---

  bool hasAdescription = ent->HasDescription();
  if (hasAdescription)
  {
    SW.Send(ent->Description());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field products ---

  SW.OpenSub();
  for (int i3 = 1; i3 <= ent->NbProducts(); i3++)
  {
    SW.Send(ent->ProductsValue(i3));
  }
  SW.CloseSub();
}

void RWStepBasic_RWProductType::Share(const occ::handle<StepBasic_ProductType>& ent,
                                      Interface_EntityIterator&            iter) const
{

  int nbElem1 = ent->NbProducts();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->ProductsValue(is1));
  }
}
