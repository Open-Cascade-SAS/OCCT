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

#include <Interface_EntityIterator.hxx>
#include <Interface_GTool.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

Interface_ShareTool::Interface_ShareTool(const occ::handle<Interface_InterfaceModel>& amodel,
                                         const Interface_GeneralLib&                  lib)
{
  theHGraph = new Interface_HGraph(amodel, lib);
}

Interface_ShareTool::Interface_ShareTool(const occ::handle<Interface_InterfaceModel>& amodel,
                                         const occ::handle<Interface_GTool>&          gtool)
{
  theHGraph = new Interface_HGraph(amodel, gtool);
}

Interface_ShareTool::Interface_ShareTool(const occ::handle<Interface_InterfaceModel>& amodel,
                                         const occ::handle<Interface_Protocol>&       protocol)
{
  theHGraph = new Interface_HGraph(amodel, protocol);
}

Interface_ShareTool::Interface_ShareTool(const occ::handle<Interface_InterfaceModel>& amodel)
{
  theHGraph = new Interface_HGraph(amodel);
}

Interface_ShareTool::Interface_ShareTool(const Interface_Graph& agraph)
{
  theHGraph = new Interface_HGraph(agraph.Model());
}

Interface_ShareTool::Interface_ShareTool(const occ::handle<Interface_HGraph>& ahgraph)
{
  theHGraph = ahgraph;
}

//    Addition of "Implied" on all Entities of the Graph
/*void Interface_ShareTool::AddImplied (const occ::handle<Interface_GTool>& gtool)
{
  Interface_Graph& thegraph = theHGraph->CGraph();
  int nb = thegraph.Size();
  bool yena = false;
  for (int i = 1; i <= nb; i ++) {
    occ::handle<Standard_Transient> ent = thegraph.Entity(i);
    if (ent.IsNull()) continue;
    occ::handle<Interface_GeneralModule> module;  int CN;
    if (gtool->Select(ent,module,CN)) {
      Interface_EntityIterator iter;
      module->ListImpliedCase(CN,ent,iter);
      if (iter.NbEntities() == 0) continue;
      yena = true;
      thegraph.SetShare(ent);
      for (iter.Start(); iter.More(); iter.Next())
    thegraph.AddShared(ent,iter.Value());
    }
  }
  if (yena) thegraph.EvalSharings();
}*/

occ::handle<Interface_InterfaceModel> Interface_ShareTool::Model() const
{
  return theHGraph->Graph().Model();
}

const Interface_Graph& Interface_ShareTool::Graph() const
{
  return theHGraph->Graph();
}

Interface_EntityIterator Interface_ShareTool::RootEntities() const
{
  return theHGraph->Graph().RootEntities();
}

bool Interface_ShareTool::IsShared(const occ::handle<Standard_Transient>& ent) const
{
  const Interface_Graph&                                              thegraph = theHGraph->Graph();
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list =
    thegraph.GetShareds(ent);
  return (!list.IsNull() && list->Length() > 0);
}

Interface_EntityIterator Interface_ShareTool::Shareds(
  const occ::handle<Standard_Transient>& ent) const
{
  return theHGraph->Graph().Shareds(ent);
}

Interface_EntityIterator Interface_ShareTool::Sharings(
  const occ::handle<Standard_Transient>& ent) const
{
  return theHGraph->Graph().Sharings(ent);
}

int Interface_ShareTool::NbTypedSharings(const occ::handle<Standard_Transient>& ent,
                                         const occ::handle<Standard_Type>&      atype) const
{
  Interface_Graph& thegraph = theHGraph->CGraph();
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list =
    thegraph.GetSharings(ent);
  if (list.IsNull())
    return 0;

  int result = 0;
  int n      = list->Length();
  for (int i = 1; i <= n; i++)
  {
    occ::handle<Standard_Transient> entsh = list->Value(i);
    if (entsh.IsNull())
      continue;
    if (entsh->IsKind(atype))
      result++;
  }
  return result;
}

occ::handle<Standard_Transient> Interface_ShareTool::TypedSharing(
  const occ::handle<Standard_Transient>& ent,
  const occ::handle<Standard_Type>&      atype) const
{
  Interface_Graph& thegraph = theHGraph->CGraph();
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list =
    thegraph.GetSharings(ent);
  if (list.IsNull())
    return nullptr;
  occ::handle<Standard_Transient> entresult;
  int                             result = 0;
  int                             n      = list->Length();
  for (int i = 1; i <= n; i++)
  {
    occ::handle<Standard_Transient> entsh = list->Value(i);
    if (entsh.IsNull())
      continue;
    if (entsh->IsKind(atype))
    {
      entresult = entsh;
      result++;
      if (result > 1)
        throw Interface_InterfaceError("Interface ShareTool : TypedSharing, more than one found");
    }
  }
  if (result == 0)
    throw Interface_InterfaceError("Interface ShareTool : TypedSharing, not found");
  return entresult;
}

Interface_EntityIterator Interface_ShareTool::All(const occ::handle<Standard_Transient>& ent,
                                                  const bool rootlast) const
{
  occ::handle<Interface_InterfaceModel> model = Model();
  Interface_EntityIterator              list;
  int                                   i, n0 = 0, nb = model->NbEntities();
  occ::handle<NCollection_HArray1<int>> fl = new NCollection_HArray1<int>(0, nb);
  fl->Init(0);
  if (ent == model)
  {
    //    We review the roots (the base order is preserved)
    Interface_EntityIterator roots = RootEntities();
    for (roots.Start(); roots.More(); roots.Next())
    {
      Interface_EntityIterator subl = All(roots.Value(), rootlast);
      for (subl.Start(); subl.More(); subl.Next())
      {
        int nm = model->Number(subl.Value());
        if (fl->Value(nm) > 0)
          continue;
        n0++;
        fl->SetValue(nm, n0);
      }
    }
    //    Warning, are there any omissions?
    for (i = 1; i <= nb; i++)
      if (fl->Value(i) == 0)
      {
        n0++;
        fl->SetValue(i, n0);
      }
  }
  else
  {
    occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> sq =
      new NCollection_HSequence<occ::handle<Standard_Transient>>();
    sq->Append(ent);
    //    file type process
    for (i = 1; i <= sq->Length(); i++)
    { // Length croit
      occ::handle<Standard_Transient> en  = sq->Value(i);
      int                             num = model->Number(en);
      if (fl->Value(num) != 0)
        continue; // already seen
      n0++;
      fl->SetValue(num, n0);
      Interface_EntityIterator sh = Shareds(en);
      sq->Append(sh.Content());
    }
  }
  //    Remains to constitute the list, return if necessary
  occ::handle<NCollection_HArray1<int>> ord = new NCollection_HArray1<int>(0, nb);
  ord->Init(0);
  for (i = 1; i <= nb; i++)
  {
    n0 = fl->Value(i);
    ord->SetValue(n0, i);
  }
  if (rootlast && ent != model)
    for (i = 1; i <= nb; i++)
    {
      if (ord->Value(i) != 0)
        list.AddItem(model->Value(ord->Value(i)));
    }
  else
    for (i = nb; i > 0; i--)
    {
      if (ord->Value(i) != 0)
        list.AddItem(model->Value(ord->Value(i)));
    }

  return list;
}

void Interface_ShareTool::Print(const Interface_EntityIterator& iter, Standard_OStream& S) const
{
  S << " Nb.Entities : " << iter.NbEntities() << " : ";
  for (iter.Start(); iter.More(); iter.Next())
  {
    const occ::handle<Standard_Transient>& ent = iter.Value();
    S << " n0/id:";
    Model()->Print(ent, S);
  }
  S << std::endl;
}
