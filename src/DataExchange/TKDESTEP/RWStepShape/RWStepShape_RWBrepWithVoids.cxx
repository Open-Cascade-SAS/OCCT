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

//: k4 abv 30.11.98: TR9: warnings for BWV

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include "RWStepShape_RWBrepWithVoids.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepShape_OrientedClosedShell.hxx>

RWStepShape_RWBrepWithVoids::RWStepShape_RWBrepWithVoids() {}

void RWStepShape_RWBrepWithVoids::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                           const int                 num,
                                           occ::handle<Interface_Check>&               ach,
                                           const occ::handle<StepShape_BrepWithVoids>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "brep_with_voids"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : outer ---

  occ::handle<StepShape_ClosedShell> aOuter;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "outer", ach, STANDARD_TYPE(StepShape_ClosedShell), aOuter);

  // --- own field : voids ---

  occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>> aVoids;
  occ::handle<StepShape_OrientedClosedShell>          anent3;
  int                               nsub3;
  if (data->ReadSubList(num, 3, "voids", ach, nsub3))
  {
    int nb3 = data->NbParams(nsub3);
    aVoids               = new NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>(1, nb3);
    for (int i3 = 1; i3 <= nb3; i3++)
    {
      // szv#4:S4163:12Mar99 `bool stat3 =` not needed
      if (data->ReadEntity(nsub3,
                           i3,
                           "oriented_closed_shell",
                           ach,
                           STANDARD_TYPE(StepShape_OrientedClosedShell),
                           anent3))
        aVoids->SetValue(i3, anent3);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, aOuter, aVoids);
}

void RWStepShape_RWBrepWithVoids::WriteStep(StepData_StepWriter&                   SW,
                                            const occ::handle<StepShape_BrepWithVoids>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field outer ---

  SW.Send(ent->Outer());

  // --- own field : voids ---

  SW.OpenSub();
  for (int i3 = 1; i3 <= ent->NbVoids(); i3++)
  {
    SW.Send(ent->VoidsValue(i3));
  }
  SW.CloseSub();
}

void RWStepShape_RWBrepWithVoids::Share(const occ::handle<StepShape_BrepWithVoids>& ent,
                                        Interface_EntityIterator&              iter) const
{

  iter.GetOneItem(ent->Outer());

  int nbElem2 = ent->NbVoids();
  for (int is2 = 1; is2 <= nbElem2; is2++)
  {
    iter.GetOneItem(ent->VoidsValue(is2));
  }
}

//: k4 abv 30 Nov 98: ProSTEP TR9: add warning
void RWStepShape_RWBrepWithVoids::Check(const occ::handle<StepShape_BrepWithVoids>& ent,
                                        const Interface_ShareTool&,
                                        occ::handle<Interface_Check>& ach) const
{
  for (int i = 1; i <= ent->NbVoids(); i++)
    if (ent->VoidsValue(i)->Orientation())
    {
      ach->AddWarning("Void has orientation .T. while .F. is required by API 214");
      break;
    }
}
