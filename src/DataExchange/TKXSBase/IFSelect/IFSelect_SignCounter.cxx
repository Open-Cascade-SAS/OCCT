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

#include <IFSelect_Selection.hxx>
#include <IFSelect_Signature.hxx>
#include <IFSelect_SignCounter.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SignCounter, IFSelect_SignatureList)

IFSelect_SignCounter::IFSelect_SignCounter(const bool withmap, const bool withlist)
    : IFSelect_SignatureList(withlist)
{
  themapstat = withmap;
  thenbcomp1 = thenbcomp2 = theselmode = 0;
}

IFSelect_SignCounter::IFSelect_SignCounter(const occ::handle<IFSelect_Signature>& matcher,
                                           const bool                             withmap,
                                           const bool                             withlist)
    : IFSelect_SignatureList(withlist),
      thematcher(matcher)
{
  themapstat = withmap;
  thenbcomp1 = thenbcomp2 = theselmode = 0;
  TCollection_AsciiString sign         = thematcher->Name();
  SetName(sign.ToCString());
}

occ::handle<IFSelect_Signature> IFSelect_SignCounter::Signature() const
{
  return thematcher;
}

void IFSelect_SignCounter::SetMap(const bool withmap)
{
  themapstat = withmap;
}

bool IFSelect_SignCounter::AddEntity(const occ::handle<Standard_Transient>&       ent,
                                     const occ::handle<Interface_InterfaceModel>& model)
{
  if (themapstat && !ent.IsNull())
  {
    if (themap.Contains(ent))
      return false;
    themap.Add(ent);
  }
  AddSign(ent, model);
  return true;
}

void IFSelect_SignCounter::AddSign(const occ::handle<Standard_Transient>&       ent,
                                   const occ::handle<Interface_InterfaceModel>& model)
{
  char nulsign[2];
  nulsign[0] = '\0';
  if (ent.IsNull() || thematcher.IsNull())
    Add(ent, nulsign); // to count the Nulls
  else
    Add(ent, thematcher->Value(ent, model));
}

void IFSelect_SignCounter::AddList(
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list,
  const occ::handle<Interface_InterfaceModel>&                               model)
{
  if (list.IsNull())
    return;
  int nb = list->Length();
  for (int i = 1; i <= nb; i++)
    AddEntity(list->Value(i), model);
}

void IFSelect_SignCounter::AddWithGraph(
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list,
  const Interface_Graph&                                                     graph)
{
  AddList(list, graph.Model());
}

void IFSelect_SignCounter::AddModel(const occ::handle<Interface_InterfaceModel>& model)
{
  if (model.IsNull())
    return;
  int nb = model->NbEntities();
  //  If we start from empty, we know that each entity is unique in the model
  bool mapstat = themapstat;
  if (themap.Extent() == 0)
    themapstat = false;
  for (int i = 1; i <= nb; i++)
    AddEntity(model->Value(i), model);
  themapstat = mapstat;
}

void IFSelect_SignCounter::AddFromSelection(const occ::handle<IFSelect_Selection>& sel,
                                            const Interface_Graph&                 G)
{
  Interface_EntityIterator iter = sel->RootResult(G);
  AddWithGraph(iter.Content(), G);
}

//  #############    SELECTION    ##############

void IFSelect_SignCounter::SetSelection(const occ::handle<IFSelect_Selection>& sel)
{
  theselect = sel;
  SetSelMode(-1);
  SetSelMode(sel.IsNull() ? 0 : 2);
}

occ::handle<IFSelect_Selection> IFSelect_SignCounter::Selection() const
{
  return theselect;
}

void IFSelect_SignCounter::SetSelMode(const int selmode)
{
  if (selmode < 0)
    thenbcomp1 = thenbcomp2 = 0;
  else
    theselmode = selmode;
  if (selmode == 0)
    theselect.Nullify();
}

int IFSelect_SignCounter::SelMode() const
{
  return theselmode;
}

bool IFSelect_SignCounter::ComputeSelected(const Interface_Graph& G, const bool forced)
{
  if (theselmode < 2 || theselect.IsNull())
    return false;
  bool                     afaire = forced;
  Interface_EntityIterator iter   = theselect->RootResult(G);
  int                      nb1    = G.Size();
  int                      nb2    = iter.NbEntities();
  if (!afaire)
    afaire = (nb1 != thenbcomp1 || nb2 != thenbcomp2);
  thenbcomp1 = nb1;
  thenbcomp2 = nb2;
  if (afaire)
    AddWithGraph(iter.Content(), G);
  return true;
}

occ::handle<TCollection_HAsciiString> IFSelect_SignCounter::Sign(
  const occ::handle<Standard_Transient>&       ent,
  const occ::handle<Interface_InterfaceModel>& model) const
{
  occ::handle<TCollection_HAsciiString> res;
  if (ent.IsNull() || thematcher.IsNull())
    return res;
  res = new TCollection_HAsciiString(thematcher->Value(ent, model));
  return res;
}

const char* IFSelect_SignCounter::ComputedSign(const occ::handle<Standard_Transient>& ent,
                                               const Interface_Graph&                 G)
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list =
    new NCollection_HSequence<occ::handle<Standard_Transient>>();
  list->Append(ent);
  ModeSignOnly() = true;
  AddWithGraph(list, G);
  const char* val = LastValue();
  ModeSignOnly()  = false;
  return val;
}
