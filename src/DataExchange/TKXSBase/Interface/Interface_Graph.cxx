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

#include <Interface_BitMap.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_Graph.hxx>
#include <Interface_GTool.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReportEntity.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IncAllocator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>

// Flags : 0 = Presence, 1 = Sharing Error
#define Graph_Present 0
#define Graph_ShareError 1

//  ###########################################################################

//  ....                           CONSTRUCTEURS                           ....

//  ....       Construction from Entity knowledge        ....

Interface_Graph::Interface_Graph(const occ::handle<Interface_InterfaceModel>& amodel,
                                 const Interface_GeneralLib& /*lib*/,
                                 bool theModeStat)
    : themodel(amodel),
      thepresents("")
{
  if (theModeStat)
    InitStats();
  Evaluate();
}

Interface_Graph::Interface_Graph(const occ::handle<Interface_InterfaceModel>& amodel,
                                 const occ::handle<Interface_Protocol>& /*protocol*/,
                                 bool theModeStat)
    : themodel(amodel),
      thepresents("")

{
  if (theModeStat)
    InitStats();
  Evaluate();
}

Interface_Graph::Interface_Graph(const occ::handle<Interface_InterfaceModel>& amodel,
                                 const occ::handle<Interface_GTool>& /*gtool*/,
                                 bool theModeStat)
    : themodel(amodel),
      thepresents("")
{
  if (theModeStat)
    InitStats();
  Evaluate();
}

Interface_Graph::Interface_Graph(const occ::handle<Interface_InterfaceModel>& amodel,
                                 bool                                         theModeStat)
    : themodel(amodel),
      thepresents("")
{
  if (theModeStat)
    InitStats();
  Evaluate();
}

//  ....                Construction from another Graph                ....

Interface_Graph::Interface_Graph(const Interface_Graph& agraph, const bool /*copied*/)
    : themodel(agraph.Model()),
      thepresents("")
{
  thesharings = agraph.SharingTable();
  int nb      = agraph.NbStatuses();
  if (!nb)
    return;
  if (thestats.IsNull())
    thestats = new NCollection_HArray1<int>(1, nb);
  for (int i = 1; i <= nb; i++)
    thestats->SetValue(i, agraph.Status(i));
  theflags.Initialize(agraph.BitMap(), true);
}

Interface_Graph& Interface_Graph::operator=(const Interface_Graph& theOther)
{
  themodel    = theOther.Model();
  thepresents = theOther.thepresents;
  thesharings = theOther.SharingTable();
  thestats.Nullify();

  const int nb = theOther.NbStatuses();
  if (nb != 0)
  {
    thestats = new NCollection_HArray1<int>(1, nb);
    for (int i = 1; i <= nb; ++i)
    {
      thestats->SetValue(i, theOther.Status(i));
    }
    theflags.Initialize(theOther.BitMap(), true);
  }
  return *this;
}

void Interface_Graph::InitStats()
{
  thestats = new NCollection_HArray1<int>(1, themodel->NbEntities()),
  theflags.Initialize(themodel->NbEntities(), 2);
  theflags.AddFlag("ShareError");
}

int Interface_Graph::NbStatuses() const
{
  return (thestats.IsNull() ? 0 : thestats->Length());
}

const occ::handle<NCollection_HArray1<NCollection_List<int>>>& Interface_Graph::SharingTable() const
{
  return thesharings;
}

void Interface_Graph::Evaluate()
{
  // Evaluation is performed on all entities of the model
  const int anEntityNumber = Size();

  // Global allocator stored as a field of the single container of the sharings
  // and will be destructed with the container.
  occ::handle<NCollection_IncAllocator> anAlloc =
    new NCollection_IncAllocator(NCollection_IncAllocator::THE_DEFAULT_BLOCK_SIZE);
  thesharings = new NCollection_HArray1<NCollection_List<int>>(1, anEntityNumber);
  NCollection_Array1<NCollection_List<int>>& aSharingArrayOfLists = thesharings->ChangeArray1();
  for (int i = 1; i <= anEntityNumber; i++)
  {
    aSharingArrayOfLists(i).Clear(anAlloc);
  }

  if (themodel->GTool().IsNull())
  {
    return;
  }

  // Fill the sharing table with the entities shared by each entity
  // and the entities which share each entity.
  // The entities are iterated in the order of their numbers in the model.
  // The entities which are not present in the model are ignored.
  // The entities which are not shared by any other entity are ignored.
  // Allocator is used to reuse memory for the lists of shared entities.
  occ::handle<NCollection_IncAllocator> anAlloc2 =
    new NCollection_IncAllocator(NCollection_IncAllocator::THE_MINIMUM_BLOCK_SIZE);
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> aListOfEntities =
    new NCollection_HSequence<occ::handle<Standard_Transient>>();
  for (int i = 1; i <= anEntityNumber; i++)
  {
    aListOfEntities->Clear(anAlloc2);
    anAlloc2->Reset();
    const occ::handle<Standard_Transient>& anEntity = themodel->Value(i);
    Interface_EntityIterator               anIter(aListOfEntities);
    const int                              aNumber = EntityNumber(anEntity);
    if (aNumber == 0)
    {
      continue;
    }

    occ::handle<Standard_Transient> aCurEnt = anEntity;
    if (themodel->IsRedefinedContent(aNumber))
      aCurEnt = themodel->ReportEntity(aNumber)->Content();

    occ::handle<Interface_GeneralModule> aModule;
    int                                  aCN;
    if (themodel->GTool()->Select(aCurEnt, aModule, aCN))
    {
      aModule->FillShared(themodel, aCN, aCurEnt, anIter);
    }

    for (anIter.Start(); anIter.More(); anIter.Next())
    {
      const occ::handle<Standard_Transient>& anEntShare = anIter.Value();
      if (anEntShare == anEntity)
      {
        continue;
      }

      const int aShareNum = EntityNumber(anEntShare);

      if (aShareNum == 0)
      {
        if (!thestats.IsNull())
        {
          theflags.SetTrue(i, Graph_ShareError);
        }
        continue;
      }
      aSharingArrayOfLists(aShareNum).Append(i);
    }
  }
}

//  ....                Construction from another Graph                ....

//  ###########################################################################

//  ....                UNITARY ACCESS TO BASE DATA                ....

void Interface_Graph::Reset()
{
  if (!thestats.IsNull())
  {
    thestats.Nullify();
    theflags.Init(false, Graph_Present);
  }
}

void Interface_Graph::ResetStatus()
{
  if (!thestats.IsNull())
  {
    thestats->Init(0);
    theflags.Init(false, Graph_Present);
  }
}

int Interface_Graph::Size() const
{
  return themodel->NbEntities();
} // thestats.Upper();  }

int Interface_Graph::EntityNumber(const occ::handle<Standard_Transient>& ent) const
{
  return themodel->Number(ent);
}

bool Interface_Graph::IsPresent(const int num) const
{
  if (num <= 0 || num > Size())
    return false;
  return (!thestats.IsNull() ? theflags.Value(num, Graph_Present) : false);
}

bool Interface_Graph::IsPresent(const occ::handle<Standard_Transient>& ent) const
{
  return IsPresent(EntityNumber(ent));
}

const occ::handle<Standard_Transient>& Interface_Graph::Entity(const int num) const
{
  return themodel->Value(num);
}

int Interface_Graph::Status(const int num) const
{
  return (!thestats.IsNull() ? thestats->Value(num) : 0);
}

void Interface_Graph::SetStatus(const int num, const int stat)
{
  if (!thestats.IsNull())
    thestats->SetValue(num, stat);
}

void Interface_Graph::RemoveItem(const int num)
{
  if (!thestats.IsNull())
  {
    thestats->SetValue(num, 0);
    theflags.SetFalse(num, Graph_Present);
  }
}

void Interface_Graph::ChangeStatus(const int oldstat, const int newstat)
{
  if (thestats.IsNull())
    return;
  int nb = thestats->Upper();
  for (int i = 1; i <= nb; i++)
  {
    if (thestats->Value(i) == oldstat)
      thestats->SetValue(i, newstat);
  }
}

void Interface_Graph::RemoveStatus(const int stat)
{
  if (thestats.IsNull())
    return;
  int nb = thestats->Upper();
  for (int i = 1; i <= nb; i++)
  {
    if (thestats->Value(i) == stat)
      RemoveItem(i);
  }
}

const Interface_BitMap& Interface_Graph::BitMap() const
{
  return theflags;
}

Interface_BitMap& Interface_Graph::CBitMap()
{
  return theflags;
}

//  ###########################################################################

//  ....      Elementary Loadings with "Share" Propagation      .... //

const occ::handle<Interface_InterfaceModel>& Interface_Graph::Model() const
{
  return themodel;
}

void Interface_Graph::GetFromModel()
{
  if (themodel.IsNull() || thestats.IsNull())
    return; // no model ... (-> we won't go far)
  theflags.Init(true, Graph_Present);
  thestats->Init(0);
}

void Interface_Graph::GetFromEntity(const occ::handle<Standard_Transient>& ent,
                                    const bool                             shared,
                                    const int                              newstat)
{
  if (thestats.IsNull())
    return;
  int num = EntityNumber(ent);
  if (!num)
    return;
  if (theflags.CTrue(num, Graph_Present))
    return; // already taken : we skip
  thestats->SetValue(num, newstat);
  if (!shared)
    return;
  //  Watch out for redefinition !
  Interface_EntityIterator aIter = GetShareds(ent);

  for (; aIter.More(); aIter.Next())
    GetFromEntity(aIter.Value(), true, newstat);
}

void Interface_Graph::GetFromEntity(const occ::handle<Standard_Transient>& ent,
                                    const bool                             shared,
                                    const int                              newstat,
                                    const int                              overlapstat,
                                    const bool                             cumul)
{
  if (thestats.IsNull())
    return;
  int num = EntityNumber(ent);
  if (!num)
    return;
  bool pasla = !theflags.CTrue(num, Graph_Present);
  int  stat  = thestats->Value(num);

  if (pasla)
  {
    ///    theflags.SetTrue (num, Graph_Present);   // new : note with newstat
    thestats->SetValue(num, newstat);
  }
  else
  {
    int overstat = stat;
    if (stat != newstat)
    { // already taken, same status : skip
      if (cumul)
        overstat += overlapstat; // new status : with cumulation ...
      else
        overstat = overlapstat; // ... or without (forced status)
      if (stat != overstat)     // if repass already done, skip
        thestats->SetValue(num, overstat);
    }
  }
  if (!shared)
    return;
  //  Watch out for redefinition !
  Interface_EntityIterator aIter = GetShareds(ent);

  for (; aIter.More(); aIter.Next())
    GetFromEntity(aIter.Value(), true, newstat);
}

void Interface_Graph::GetFromIter(const Interface_EntityIterator& iter, const int newstat)
{
  if (thestats.IsNull())
    return;
  for (iter.Start(); iter.More(); iter.Next())
  {
    const occ::handle<Standard_Transient>& ent = iter.Value();
    int                                    num = EntityNumber(ent);
    if (!num)
      continue;
    if (theflags.CTrue(num, Graph_Present))
      continue;
    thestats->SetValue(num, newstat);
  }
}

void Interface_Graph::GetFromIter(const Interface_EntityIterator& iter,
                                  const int                       newstat,
                                  const int                       overlapstat,
                                  const bool                      cumul)
{
  if (thestats.IsNull())
    return;
  for (iter.Start(); iter.More(); iter.Next())
  {
    const occ::handle<Standard_Transient>& ent = iter.Value();
    int                                    num = EntityNumber(ent);
    if (!num)
      continue;
    /*bool pasla = !*/ theflags.Value(num, Graph_Present);
    /*int stat  = */ thestats->Value(num);
    GetFromEntity(ent, false, newstat, overlapstat, cumul);
  }
}

void Interface_Graph::GetFromGraph(const Interface_Graph& agraph)
{
  if (Model() != agraph.Model())
    throw Standard_DomainError("Graph from Interface : GetFromGraph");
  int nb = Size();
  for (int i = 1; i <= nb; i++)
  {
    if (agraph.IsPresent(i))
      GetFromEntity(agraph.Entity(i), false, agraph.Status(i));
  }
}

void Interface_Graph::GetFromGraph(const Interface_Graph& agraph, const int stat)
{
  if (Model() != agraph.Model())
    throw Standard_DomainError("Graph from Interface : GetFromGraph");
  int nb = Size();
  for (int i = 1; i <= nb; i++)
  {
    if (agraph.IsPresent(i) && agraph.Status(i) == stat)
      GetFromEntity(agraph.Entity(i), false, stat);
  }
}

//  #####################################################################

//  ....                Listing of Shared Entities                ....

bool Interface_Graph::HasShareErrors(const occ::handle<Standard_Transient>& ent) const
{
  if (thestats.IsNull())
    return false;
  int num = EntityNumber(ent);
  if (num == 0)
    return true;
  return theflags.Value(num, Graph_ShareError);
}

Interface_EntityIterator Interface_Graph::Shareds(const occ::handle<Standard_Transient>& ent) const
{
  Interface_EntityIterator iter;
  int                      num = EntityNumber(ent);
  if (!num)
    return iter;

  occ::handle<Standard_Transient> aCurEnt = ent;
  if (themodel->IsRedefinedContent(num))
    aCurEnt = themodel->ReportEntity(num)->Content();

  // if (num == 0)  throw Standard_DomainError("Interface : Shareds");
  occ::handle<Interface_GeneralModule> module;
  int                                  CN;
  if (themodel->GTool()->Select(aCurEnt, module, CN))
    module->FillShared(themodel, CN, aCurEnt, iter);
  return iter;
}

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Interface_Graph::GetShareds(
  const occ::handle<Standard_Transient>& ent) const
{
  return Shareds(ent).Content();
}

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Interface_Graph::GetSharings(
  const occ::handle<Standard_Transient>& ent) const
{
  int num = EntityNumber(ent);
  if (!num)
    return nullptr;
  // return
  // occ::down_cast<NCollection_HSequence<occ::handle<Standard_Transient>>>(thesharings->Value(num));
  const NCollection_List<int>& alist = thesharings->Value(num);
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> aSharings =
    new NCollection_HSequence<occ::handle<Standard_Transient>>;
  NCollection_List<int>::Iterator aIt(alist);
  for (; aIt.More(); aIt.Next())
    aSharings->Append(Entity(aIt.Value()));
  return aSharings;
}

Interface_EntityIterator Interface_Graph::Sharings(const occ::handle<Standard_Transient>& ent) const
{
  return Interface_EntityIterator(GetSharings(ent));
}

static void AddTypedSharings(const occ::handle<Standard_Transient>& ent,
                             const occ::handle<Standard_Type>&      type,
                             Interface_EntityIterator&              iter,
                             const int                              n,
                             const Interface_Graph&                 G)
{
  if (ent.IsNull())
    return;
  if (ent->IsKind(type))
  {
    iter.AddItem(ent);
    return;
  }
  if (iter.NbEntities() > n)
    return;

  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list = G.GetSharings(ent);
  if (list.IsNull())
    return;

  int nb = list->Length();
  for (int i = 1; i <= nb; i++)
    AddTypedSharings(list->Value(i), type, iter, nb, G);
}

Interface_EntityIterator Interface_Graph::TypedSharings(
  const occ::handle<Standard_Transient>& ent,
  const occ::handle<Standard_Type>&      type) const
{
  Interface_EntityIterator iter;
  int                      n = Size();
  AddTypedSharings(ent, type, iter, n, *this);
  return iter;
}

Interface_EntityIterator Interface_Graph::RootEntities() const
{
  Interface_EntityIterator iter;
  int                      nb = thesharings->Length();
  for (int i = 1; i <= nb; i++)
  {
    if (!thesharings->Value(i).IsEmpty())
      continue;
    iter.AddItem(Entity(i));
  }
  return iter;
}

occ::handle<TCollection_HAsciiString> Interface_Graph::Name(
  const occ::handle<Standard_Transient>& ent) const
{
  occ::handle<TCollection_HAsciiString> str;
  if (themodel.IsNull())
    return str;
  if (themodel->Number(ent))
    return str;

  occ::handle<Interface_GTool> gtool = themodel->GTool();
  if (gtool.IsNull())
    return str;

  occ::handle<Interface_GeneralModule> module;
  int                                  CN;
  if (!gtool->Select(ent, module, CN))
    return str;

  Interface_ShareTool sht(*this);
  return module->Name(CN, ent, sht);
}

bool Interface_Graph::ModeStat() const
{
  return (!thestats.IsNull());
}
