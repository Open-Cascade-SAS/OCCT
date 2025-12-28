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
#include "RWStepVisual_RWContextDependentOverRidingStyledItem.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_ContextDependentOverRidingStyledItem.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepVisual_StyleContextSelect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_StyleContextSelect.hxx>
#include <StepVisual_StyledItem.hxx>

RWStepVisual_RWContextDependentOverRidingStyledItem::
  RWStepVisual_RWContextDependentOverRidingStyledItem()
{
}

void RWStepVisual_RWContextDependentOverRidingStyledItem::ReadStep(
  const occ::handle<StepData_StepReaderData>&                         data,
  const int                                         num,
  occ::handle<Interface_Check>&                                       ach,
  const occ::handle<StepVisual_ContextDependentOverRidingStyledItem>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 5, ach, "context_dependent_over_riding_styled_item"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : styles ---

  occ::handle<NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>> aStyles;
  occ::handle<StepVisual_PresentationStyleAssignment>          anent2;
  int                                        nsub2;
  if (data->ReadSubList(num, 2, "styles", ach, nsub2))
  {
    int nb2 = data->NbParams(nsub2);
    aStyles              = new NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>(1, nb2);
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

  // --- inherited field : item ---

  occ::handle<Standard_Transient> aItem;
  data->ReadEntity(num, 3, "item", ach, STANDARD_TYPE(Standard_Transient), aItem);

  // --- inherited field : overRiddenStyle ---

  occ::handle<StepVisual_StyledItem> aOverRiddenStyle;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadEntity(num,
                   4,
                   "over_ridden_style",
                   ach,
                   STANDARD_TYPE(StepVisual_StyledItem),
                   aOverRiddenStyle);

  // --- own field : styleContext ---

  occ::handle<NCollection_HArray1<StepVisual_StyleContextSelect>> aStyleContext;
  StepVisual_StyleContextSelect                  aStyleContextItem;
  int                               nsub5;
  if (data->ReadSubList(num, 5, "style_context", ach, nsub5))
  {
    int nb5 = data->NbParams(nsub5);
    aStyleContext        = new NCollection_HArray1<StepVisual_StyleContextSelect>(1, nb5);
    for (int i5 = 1; i5 <= nb5; i5++)
    {
      // szv#4:S4163:12Mar99 `bool stat5 =` not needed
      if (data->ReadEntity(nsub5, i5, "style_context", ach, aStyleContextItem))
        aStyleContext->SetValue(i5, aStyleContextItem);
    }
  }
  else
  {
    aStyleContext = new NCollection_HArray1<StepVisual_StyleContextSelect>();
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, aStyles, aItem, aOverRiddenStyle, aStyleContext);
}

void RWStepVisual_RWContextDependentOverRidingStyledItem::WriteStep(
  StepData_StepWriter&                                           SW,
  const occ::handle<StepVisual_ContextDependentOverRidingStyledItem>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field styles ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbStyles(); i2++)
  {
    SW.Send(ent->StylesValue(i2));
  }
  SW.CloseSub();

  // --- inherited field item ---

  SW.Send(ent->Item());

  // --- inherited field overRiddenStyle ---

  SW.Send(ent->OverRiddenStyle());

  // --- own field : styleContext ---

  SW.OpenSub();
  for (int i5 = 1; i5 <= ent->NbStyleContext(); i5++)
  {
    SW.Send(ent->StyleContextValue(i5).Value());
  }
  SW.CloseSub();
}

void RWStepVisual_RWContextDependentOverRidingStyledItem::Share(
  const occ::handle<StepVisual_ContextDependentOverRidingStyledItem>& ent,
  Interface_EntityIterator&                                      iter) const
{

  int nbElem1 = ent->NbStyles();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->StylesValue(is1));
  }

  iter.GetOneItem(ent->Item());

  iter.GetOneItem(ent->OverRiddenStyle());

  int nbElem4 = ent->NbStyleContext();
  for (int is4 = 1; is4 <= nbElem4; is4++)
  {
    iter.GetOneItem(ent->StyleContextValue(is4).Value());
  }
}
