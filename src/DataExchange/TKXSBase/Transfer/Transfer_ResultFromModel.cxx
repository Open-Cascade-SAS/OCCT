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
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Transfer_ResultFromTransient.hxx>
#include <Transfer_TransientProcess.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_ResultFromModel, Standard_Transient)

Transfer_ResultFromModel::Transfer_ResultFromModel()
{
  themnum = 0;
  themchk = Interface_CheckAny;
}

void Transfer_ResultFromModel::SetModel(const occ::handle<Interface_InterfaceModel>& model)
{
  themodel = model;
}

void Transfer_ResultFromModel::SetFileName(const char* const filename)
{
  thename.Clear();
  thename.AssignCat(filename);
}

occ::handle<Interface_InterfaceModel> Transfer_ResultFromModel::Model() const
{
  return themodel;
}

const char* Transfer_ResultFromModel::FileName() const
{
  return thename.ToCString();
}

bool Transfer_ResultFromModel::Fill(const occ::handle<Transfer_TransientProcess>& TP,
                                                const occ::handle<Standard_Transient>&        ent)
{
  if (TP.IsNull() || ent.IsNull())
    return false;
  occ::handle<Transfer_Binder> binder = TP->Find(ent);
  if (binder.IsNull())
    return false;
  themain = new Transfer_ResultFromTransient;
  themain->SetStart(ent);
  themain->SetBinder(binder);
  themain->Fill(TP);
  //   Result substitution for the Shape (-> HShape): not here, we are
  //   in the Transfer package which is general and doesn't know what a Shape is ...
  if (!TP->Model().IsNull())
    themodel = TP->Model();
  if (themodel.IsNull())
    return true;
  themnum = themodel->Number(ent);
  themlab.Clear();
  if (themnum > 0)
    themlab.AssignCat(themodel->StringLabel(ent)->ToCString());
  return true;
}

void Transfer_ResultFromModel::Strip(const int mode)
{
  if (themain.IsNull())
    return;
  themain->Strip();
  if (mode >= 10)
  {
    themchk = ComputeCheckStatus(false);
    themodel.Nullify();
    themain->ClearSubs();
    occ::handle<Standard_Transient> nulh;
    themain->SetStart(nulh);
    if (mode > 10)
      themain.Nullify();
  }
}

void Transfer_ResultFromModel::FillBack(const occ::handle<Transfer_TransientProcess>& TP) const
{
  if (!themodel.IsNull())
    TP->SetModel(themodel);
  themain->FillBack(TP);
}

bool Transfer_ResultFromModel::HasResult() const
{
  return (themain.IsNull() ? false : themain->HasResult());
}

occ::handle<Transfer_ResultFromTransient> Transfer_ResultFromModel::MainResult() const
{
  return themain;
}

void Transfer_ResultFromModel::SetMainResult(const occ::handle<Transfer_ResultFromTransient>& amain)
{
  themchk = Interface_CheckAny;
  themain = amain;
  if (themodel.IsNull() || themain.IsNull())
    return;
  themnum = themodel->Number(themain->Start());
  themlab.Clear();
  if (themnum > 0)
    themlab.AssignCat(themodel->StringLabel(themain->Start())->ToCString());
}

const char* Transfer_ResultFromModel::MainLabel() const
{
  return themlab.ToCString();
}

int Transfer_ResultFromModel::MainNumber() const
{
  return themnum;
}

//  ############  INFORMATIONS  GLOBALES  ###########

occ::handle<Transfer_ResultFromTransient> Transfer_ResultFromModel::ResultFromKey(
  const occ::handle<Standard_Transient>& start) const
{
  return themain->ResultFromKey(start);
}

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Transfer_ResultFromModel::Results(
  const int level) const
{
  int                     i, nb;
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  if (level > 1)
  {
    NCollection_IndexedMap<occ::handle<Standard_Transient>> map(themodel.IsNull() ? 1000 : themodel->NbEntities());
    map.Add(themain);
    themain->FillMap(map);
    nb = map.Extent();
    for (i = 1; i <= nb; i++)
      list->Append(map.FindKey(i));
  }
  else
    list->Append(themain);
  if (level == 1)
  {
    nb = themain->NbSubResults();
    for (i = 1; i <= nb; i++)
      list->Append(themain->SubResult(i));
    list->Append(themain);
  }
  return list;
}

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Transfer_ResultFromModel::TransferredList(
  const int level) const
{
  int                     i, nb;
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> res  = Results(level);
  nb                                        = res->Length();
  for (i = 1; i <= nb; i++)
  {
    DeclareAndCast(Transfer_ResultFromTransient, unres, res->Value(i));
    if (unres.IsNull())
      continue;
    if (unres->HasResult())
      list->Append(unres->Start());
  }
  return list;
}

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Transfer_ResultFromModel::CheckedList(
  const Interface_CheckStatus check,
  const bool      result) const
{
  int                     i, nb;
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> res  = Results(2);
  nb                                        = res->Length();
  for (i = 1; i <= nb; i++)
  {
    DeclareAndCast(Transfer_ResultFromTransient, unres, res->Value(i));
    if (unres.IsNull())
      continue;
    if (result && !unres->HasResult())
      continue;
    const occ::handle<Interface_Check> ach = unres->Check();
    if (ach->Complies(check))
      list->Append(unres->Start());
  }
  return list;
}

Interface_CheckIterator Transfer_ResultFromModel::CheckList(const bool erronly,
                                                            const int level) const
{
  Interface_CheckIterator              chl;
  int                     i, nb;
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> res  = Results(level);
  nb                                        = res->Length();
  for (i = 1; i <= nb; i++)
  {
    DeclareAndCast(Transfer_ResultFromTransient, unres, res->Value(i));
    if (unres.IsNull())
      continue;
    Interface_CheckStatus stat = unres->CheckStatus();
    if (stat == Interface_CheckOK || (stat == Interface_CheckWarning && erronly))
      continue;
    occ::handle<Transfer_Binder> binder = unres->Binder();
    occ::handle<Interface_Check> bch    = binder->Check();
    bch->SetEntity(unres->Start());
    chl.Add(bch, (themodel.IsNull() ? 0 : themodel->Number(unres->Start())));
  }
  return chl;
}

Interface_CheckStatus Transfer_ResultFromModel::CheckStatus() const
{
  if (themchk != Interface_CheckAny)
    return themchk;
  Interface_CheckIterator chl = CheckList(false, 2);
  return chl.Status();
}

Interface_CheckStatus Transfer_ResultFromModel::ComputeCheckStatus(const bool enforce)
{
  if (themchk == Interface_CheckAny || enforce)
    themchk = CheckStatus();
  return themchk;
}
