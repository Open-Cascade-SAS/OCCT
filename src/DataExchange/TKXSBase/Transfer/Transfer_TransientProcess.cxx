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
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_MSG.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Transfer_TransientProcess.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_TransientProcess, Transfer_ProcessForTransient)

//=================================================================================================

Transfer_TransientProcess::Transfer_TransientProcess(const int nb)
    : Transfer_ProcessForTransient(nb)
{
  thetrroots = new NCollection_HSequence<occ::handle<Standard_Transient>>;
}

//=================================================================================================

void Transfer_TransientProcess::SetModel(const occ::handle<Interface_InterfaceModel>& model)
{
  themodel = model;
}

//=================================================================================================

occ::handle<Interface_InterfaceModel> Transfer_TransientProcess::Model() const
{
  return themodel;
}

void Transfer_TransientProcess::SetGraph(const occ::handle<Interface_HGraph>& HG)
{
  thegraph = HG;
  if (!thegraph.IsNull())
    SetModel(thegraph->Graph().Model());
  else
    themodel.Nullify();
}

//=================================================================================================

bool Transfer_TransientProcess::HasGraph() const
{
  return !thegraph.IsNull();
}

//=================================================================================================

occ::handle<Interface_HGraph> Transfer_TransientProcess::HGraph() const
{
  return thegraph;
}

//=================================================================================================

const Interface_Graph& Transfer_TransientProcess::Graph() const
{
  return thegraph->Graph();
}

//=================================================================================================

void Transfer_TransientProcess::SetContext(const char* const            name,
                                           const occ::handle<Standard_Transient>& ctx)
{
  thectx.Bind(name, ctx);
}

//=================================================================================================

bool Transfer_TransientProcess::GetContext(const char* const       name,
                                                       const occ::handle<Standard_Type>& type,
                                                       occ::handle<Standard_Transient>&  ctx) const
{
  if (thectx.IsEmpty())
    return false;
  if (!thectx.Find(name, ctx))
    ctx.Nullify();

  if (ctx.IsNull())
    return false;
  if (type.IsNull())
    return true;
  if (!ctx->IsKind(type))
    ctx.Nullify();
  return !ctx.IsNull();
}

//=================================================================================================

NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>&
  Transfer_TransientProcess::Context()
{
  return thectx;
}

//=================================================================================================

void Transfer_TransientProcess::PrintTrace(const occ::handle<Standard_Transient>& start,
                                           Standard_OStream&                 S) const
{
  if (!start.IsNull())
  {
    if (!themodel.IsNull())
    {
      S << "Entity ";
      themodel->Print(start, S, 1);
      //      S<<"id ";  themodel->Print (start,S);
      //      S<<" Type:"<<themodel->TypeName (start);
    }
    else
      S << "Entity Type:" << Interface_InterfaceModel::ClassName(start->DynamicType()->Name());
    //  << start (handle)  ??
  }
}

//=================================================================================================

int Transfer_TransientProcess::CheckNum(const occ::handle<Standard_Transient>& start) const
{
  return (themodel.IsNull() ? 0 : themodel->Number(start));
}

//=================================================================================================

Interface_EntityIterator Transfer_TransientProcess::TypedSharings(
  const occ::handle<Standard_Transient>& start,
  const occ::handle<Standard_Type>&      type) const
{
  Interface_EntityIterator iter;
  if (thegraph.IsNull())
    return iter;
  return thegraph->Graph().TypedSharings(start, type);
}

//=================================================================================================

bool Transfer_TransientProcess::IsDataLoaded(
  const occ::handle<Standard_Transient>& start) const
{
  if (themodel.IsNull())
    return true;
  int num = themodel->Number(start);
  if (num == 0)
    return true;
  if (themodel->IsUnknownEntity(num))
    return false;
  return !themodel->IsRedefinedContent(num);
}

//=================================================================================================

bool Transfer_TransientProcess::IsDataFail(
  const occ::handle<Standard_Transient>& start) const
{
  if (themodel.IsNull())
    return false;
  int num = themodel->Number(start);
  if (num == 0)
    return false;
  if (themodel->IsErrorEntity(num))
    return true;
  const occ::handle<Interface_Check> ach = themodel->Check(num, false); // semantic
  return ach->HasFailed();
}

//=================================================================================================

void Transfer_TransientProcess::PrintStats(const int /*mode*/,
                                           Standard_OStream& S) const
{
  S << "\n*******************************************************************\n";
  //  if (mode == 1) {    //  Basic statistics
  S << "********                 Basic Statistics                  ********" << std::endl;

  occ::handle<Interface_InterfaceModel> model = Model();
  if (model.IsNull())
    S << "****        Model unknown" << std::endl;
  else
    S << "****        Nb Entities         : " << model->NbEntities() << std::endl;

  int nbr = 0, nbe = 0, nbw = 0;
  int i, max = NbMapped(), nbroots = NbRoots();
  S << "****        Nb Final Results    : " << nbroots << std::endl;

  for (i = 1; i <= max; i++)
  {
    const occ::handle<Transfer_Binder>& binder = MapItem(i);
    if (binder.IsNull())
      continue;
    const occ::handle<Interface_Check> ach  = binder->Check();
    Transfer_StatusExec           stat = binder->StatusExec();
    if (stat != Transfer_StatusInitial && stat != Transfer_StatusDone)
      nbe++;
    else
    {
      if (ach->NbWarnings() > 0)
        nbw++;
      if (binder->HasResult())
        nbr++;
    }
  }
  if (nbr > nbroots)
    S << "****      ( Itermediate Results : " << nbr - nbroots << " )\n";
  if (nbe > 0)
    S << "****                  Errors on : " << Interface_MSG::Blanks(nbe, 4) << nbe
      << " Entities\n";
  if (nbw > 0)
    S << "****                Warnings on : " << Interface_MSG::Blanks(nbw, 4) << nbw
      << " Entities\n";
  S << "*******************************************************************";
  //  }
  S << std::endl;
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Transfer_TransientProcess::RootsForTransfer()
{
  return thetrroots;
}
