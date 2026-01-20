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

#include <IFSelect_AppliedModifiers.hxx>
#include <IFSelect_ContextModif.hxx>
#include <IFSelect_ContextWrite.hxx>
#include <IFSelect_GeneralModifier.hxx>
#include <IFSelect_ModelCopier.hxx>
#include <IFSelect_Modifier.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_ShareOut.hxx>
#include <IFSelect_ShareOutResult.hxx>
#include <IFSelect_WorkLibrary.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_ModelCopier, Standard_Transient)

// #define MISOPOINT
IFSelect_ModelCopier::IFSelect_ModelCopier() {}

void IFSelect_ModelCopier::SetShareOut(const occ::handle<IFSelect_ShareOut>& sho)
{
  theshareout = sho;
}

//  ########################################################################
//  ########    GLOBAL TRANSFER OPERATIONS (memorized or not)    ########

void IFSelect_ModelCopier::ClearResult()
{
  thefilemodels.Clear();
  thefilenames.Clear();
  theapplieds.Clear();
  theremain.Nullify();
}

bool IFSelect_ModelCopier::AddFile(const TCollection_AsciiString&          filename,
                                               const occ::handle<Interface_InterfaceModel>& content)
{
  int nb = thefilenames.Length();
  for (int i = 1; i <= nb; i++)
  {
    if (filename.IsEmpty())
      continue;
    if (thefilenames(i).IsEqual(filename))
      return false;
  }
  occ::handle<IFSelect_AppliedModifiers> nulapplied;
  thefilenames.Append(filename);
  thefilemodels.Append(content);
  theapplieds.Append(nulapplied);
  return true;
}

bool IFSelect_ModelCopier::NameFile(const int         num,
                                                const TCollection_AsciiString& filename)
{
  int nb = thefilenames.Length();
  if (num <= 0 || num > nb)
    return false;
  for (int i = 1; i <= nb; i++)
  {
    if (filename.IsEmpty())
      continue;
    if (thefilenames(i).IsEqual(filename))
      return false;
  }
  thefilenames.SetValue(num, filename);
  return true;
}

bool IFSelect_ModelCopier::ClearFile(const int num)
{
  int nb = thefilenames.Length();
  if (num <= 0 || num > nb)
    return false;
  thefilenames.ChangeValue(num).Clear();
  return true;
}

bool IFSelect_ModelCopier::SetAppliedModifiers(
  const int                   num,
  const occ::handle<IFSelect_AppliedModifiers>& applied)
{
  int nb = theapplieds.Length();
  if (num <= 0 || num > nb)
    return false;
  theapplieds.SetValue(num, applied);
  return true;
}

bool IFSelect_ModelCopier::ClearAppliedModifiers(const int num)
{
  int nb = theapplieds.Length();
  if (num <= 0 || num > nb)
    return false;
  theapplieds.ChangeValue(num).Nullify();
  return true;
}

//  ....    Copy : Performs Transfers, Memorizes them (no file sending here)

Interface_CheckIterator IFSelect_ModelCopier::Copy(IFSelect_ShareOutResult&            eval,
                                                   const occ::handle<IFSelect_WorkLibrary>& WL,
                                                   const occ::handle<Interface_Protocol>&   protocol)
{
  Interface_CopyTool TC(eval.Graph().Model(), protocol);
  return Copying(eval, WL, protocol, TC);
}

//  Internal Copy

Interface_CheckIterator IFSelect_ModelCopier::Copying(IFSelect_ShareOutResult&            eval,
                                                      const occ::handle<IFSelect_WorkLibrary>& WL,
                                                      const occ::handle<Interface_Protocol>&   protocol,
                                                      Interface_CopyTool&                 TC)
{
  Message::SendInfo() << "** WorkSession : Copying split data before sending" << std::endl;
  const Interface_Graph&  G = eval.Graph();
  Interface_CheckIterator checks;
  theshareout = eval.ShareOut();
  theremain   = new NCollection_HArray1<int>(0, G.Size());
  theremain->Init(0);
  for (eval.Evaluate(); eval.More(); eval.Next())
  {
    occ::handle<Interface_InterfaceModel> model;
    TCollection_AsciiString          filename = eval.FileName();
    int                 dispnum  = eval.DispatchRank();
    int                 numod, nbmod;
    eval.PacketsInDispatch(numod, nbmod);
    occ::handle<IFSelect_AppliedModifiers> curapp;
    CopiedModel(G,
                WL,
                protocol,
                eval.PacketRoot(),
                filename,
                dispnum,
                numod,
                TC,
                model,
                curapp,
                checks);

    AddFile(filename, model);
    theapplieds.SetValue(theapplieds.Length(), curapp);
  }
  theshareout->SetLastRun(theshareout->NbDispatches());
  checks.SetName("X-STEP WorkSession : Split Copy (no Write)");
  return checks;
}

//  Send with two arguments : File Sending of Result already memorized

Interface_CheckIterator IFSelect_ModelCopier::SendCopied(const occ::handle<IFSelect_WorkLibrary>& WL,
                                                         const occ::handle<Interface_Protocol>& protocol)
{
  Message::SendInfo() << "** WorkSession : Sending split data already copied" << std::endl;
  int        nb = NbFiles();
  Interface_CheckIterator checks;
  if (nb > 0)
  {
    for (int i = 1; i <= nb; i++)
    {
      if (FileName(i).Length() == 0)
        continue;
      occ::handle<IFSelect_AppliedModifiers> curapp = theapplieds.Value(i);
      IFSelect_ContextWrite   ctx(FileModel(i), protocol, curapp, FileName(i).ToCString());
      bool        res      = WL->WriteFile(ctx);
      Interface_CheckIterator checklst = ctx.CheckList();
      checks.Merge(checklst);
      //	(FileName(i).ToCString(), FileModel(i),protocol,curapp,checks);
      //      if (!checks.IsEmpty(false)) {
      //	sout<<"  **  On Sending File n0."<<i<<", Check Messages :  **"<<std::endl;
      //	checks.Print (sout,false);
      //      }
      if (!res)
      {
        char mess[100];
        Sprintf(mess, "Split Send (WriteFile) abandon on file n0.%d", i);
        checks.CCheck(0)->AddFail(mess);
        Message::SendInfo() << "  **  Sending File n0." << i << " has failed, abandon  **"
                            << std::endl;
        return checks;
      }
      AddSentFile(FileName(i).ToCString());
    }
    ClearResult();
  }
  checks.SetName("X-STEP WorkSession : Split Send (Copy+Write)");
  return checks;
}

//  .... Send with 4 arguments : Transfer Calculation and File Sending

Interface_CheckIterator IFSelect_ModelCopier::Send(IFSelect_ShareOutResult&            eval,
                                                   const occ::handle<IFSelect_WorkLibrary>& WL,
                                                   const occ::handle<Interface_Protocol>&   protocol)
{
  Interface_CopyTool TC(eval.Graph().Model(), protocol);
  return Sending(eval, WL, protocol, TC);
}

Interface_CheckIterator IFSelect_ModelCopier::Sending(IFSelect_ShareOutResult&            eval,
                                                      const occ::handle<IFSelect_WorkLibrary>& WL,
                                                      const occ::handle<Interface_Protocol>&   protocol,
                                                      Interface_CopyTool&                 TC)
{
  const Interface_Graph&  G = eval.Graph();
  Interface_CheckIterator checks;
  int        i = 0;
  Message::SendInfo() << "** WorkSession : Copying then sending split data" << std::endl;
  theshareout = eval.ShareOut();
  theremain   = new NCollection_HArray1<int>(0, G.Size());
  theremain->Init(0);
  for (eval.Evaluate(); eval.More(); eval.Next())
  {
    i++;
    occ::handle<Interface_InterfaceModel> model;
    TCollection_AsciiString          filename = eval.FileName();
    int                 dispnum  = eval.DispatchRank();
    int                 numod, nbmod;
    eval.PacketsInDispatch(numod, nbmod);
    occ::handle<IFSelect_AppliedModifiers> curapp;
    CopiedModel(G,
                WL,
                protocol,
                eval.PacketRoot(),
                filename,
                dispnum,
                numod,
                TC,
                model,
                curapp,
                checks);
    IFSelect_ContextWrite   ctx(model, protocol, curapp, filename.ToCString());
    bool        res      = WL->WriteFile(ctx);
    Interface_CheckIterator checklst = ctx.CheckList();
    checks.Merge(checklst);
    //      (filename.ToCString(), model, protocol, curapp, checks);
    //    if (!checks.IsEmpty(false)) {
    //      sout<<"  **  On Sending File "<<filename<<", Check Messages :  **"<<std::endl;
    //      checks.Print (sout,model,false);
    //    }
    if (!res)
    {
      char mess[100];
      Sprintf(mess, "Split Send (WriteFile) abandon on file n0.%d", i);
      checks.CCheck(0)->AddFail(mess);
      Message::SendInfo() << "  **  Sending File " << filename << " has failed, abandon  **"
                          << std::endl;
      checks.SetName("X-STEP WorkSession : Split Send (only Write)");
      return checks;
    }
    AddSentFile(filename.ToCString());
  }
  theshareout->SetLastRun(theshareout->NbDispatches());
  checks.SetName("X-STEP WorkSession : Split Send (only Write)");
  return checks;
}

//  .... SendAll : Data to transfer in G, no split, file sending

Interface_CheckIterator IFSelect_ModelCopier::SendAll(const char* const              filename,
                                                      const Interface_Graph&              G,
                                                      const occ::handle<IFSelect_WorkLibrary>& WL,
                                                      const occ::handle<Interface_Protocol>&   protocol)
{
  Interface_CheckIterator checks;
  checks.SetName("X-STEP WorkSession : Send All");
  Message::SendInfo() << "** WorkSession : Sending all data" << std::endl;
  const occ::handle<Interface_InterfaceModel>& model = G.Model();
  if (model.IsNull() || protocol.IsNull() || WL.IsNull())
    return checks;

  Interface_CopyTool TC(model, protocol);
  int   i, nb = model->NbEntities();
  for (i = 1; i <= nb; i++)
    TC.Bind(model->Value(i), model->Value(i));

  Interface_EntityIterator          pipo;
  occ::handle<Interface_InterfaceModel>  newmod;
  occ::handle<IFSelect_AppliedModifiers> applied;
  CopiedModel(G,
              WL,
              protocol,
              pipo,
              TCollection_AsciiString(filename),
              0,
              0,
              TC,
              newmod,
              applied,
              checks);

  IFSelect_ContextWrite   ctx(model, protocol, applied, filename);
  bool        res      = WL->WriteFile(ctx);
  Interface_CheckIterator checklst = ctx.CheckList();
  checks.Merge(checklst);
  if (!res)
    checks.CCheck(0)->AddFail("SendAll (WriteFile) has failed");
  //  if (!checks.IsEmpty(false)) {
  //    Message::SendWarning() <<
  //      "  **    SendAll has produced Check Messages :    **"<<std::endl;
  //    checks.Print (sout,model,false);
  //  }
  return checks;
}

//  .... SendSelected : Data to transfer in G, filtered by iter,
//       no split, file sending

Interface_CheckIterator IFSelect_ModelCopier::SendSelected(
  const char* const              filename,
  const Interface_Graph&              G,
  const occ::handle<IFSelect_WorkLibrary>& WL,
  const occ::handle<Interface_Protocol>&   protocol,
  const Interface_EntityIterator&     list)
{
  Interface_CheckIterator checks;
  checks.SetName("X-STEP WorkSession : Send Selected");
  Message::SendInfo() << "** WorkSession : Sending selected data" << std::endl;
  const occ::handle<Interface_InterfaceModel>& original = G.Model();
  if (original.IsNull() || protocol.IsNull() || WL.IsNull())
    return checks;
  occ::handle<Interface_InterfaceModel> newmod = original->NewEmptyModel();
  Interface_CopyTool               TC(original, protocol);
  TC.FillModel(newmod); // for Header ...

  //  No copy : AddWithRefs plus Bind declaration
  Interface_GeneralLib lib(protocol);
  for (list.Start(); list.More(); list.Next())
  {
    newmod->AddWithRefs(list.Value(), lib);
  }
  int i, nb = newmod->NbEntities();
  for (i = 1; i <= nb; i++)
    TC.Bind(newmod->Value(i), newmod->Value(i));
  if (theremain.IsNull())
  {
    theremain = new NCollection_HArray1<int>(0, G.Size());
    theremain->Init(0);
  }

  Interface_EntityIterator          pipo;
  occ::handle<IFSelect_AppliedModifiers> applied;
  CopiedModel(G,
              WL,
              protocol,
              pipo,
              TCollection_AsciiString(filename),
              0,
              0,
              TC,
              newmod,
              applied,
              checks);
  //  Feed Remaining : copied entities are to be noted
  occ::handle<Standard_Transient> ent1, ent2;
  for (int ic = TC.LastCopiedAfter(0, ent1, ent2); ic > 0;
       ic                  = TC.LastCopiedAfter(ic, ent1, ent2))
  {
    if (ic <= theremain->Upper())
      theremain->SetValue(ic, theremain->Value(ic) + 1);
  }
  IFSelect_ContextWrite   ctx(newmod, protocol, applied, filename);
  bool        res      = WL->WriteFile(ctx);
  Interface_CheckIterator checklst = ctx.CheckList();
  checks.Merge(checklst);
  if (!res)
    checks.CCheck(0)->AddFail("SendSelected (WriteFile) has failed");
  //  if (!checks.IsEmpty(false)) {
  //    Message::SendWarning() <<
  //      "  **    SendSelected has produced Check Messages :    **"<<std::endl;
  //    checks.Print (sout,original,false);
  //  }
  return checks;
}

//  ##########################################################################
//  ########        A UNIT TRANSFER (with Modifications)        ########

void IFSelect_ModelCopier::CopiedModel(const Interface_Graph&              G,
                                       const occ::handle<IFSelect_WorkLibrary>& WL,
                                       const occ::handle<Interface_Protocol>&   protocol,
                                       const Interface_EntityIterator&     tocopy,
                                       const TCollection_AsciiString&      filename,
                                       const int              dispnum,
                                       const int /* numod */,
                                       Interface_CopyTool&                TC,
                                       occ::handle<Interface_InterfaceModel>&  newmod,
                                       occ::handle<IFSelect_AppliedModifiers>& applied,
                                       Interface_CheckIterator&           checks) const
{
  //  ...  First "standard" part : filling the model  ...
  //  We create the Model, we fill it with Entities, and with the starting Header

  //  WARNING : dispnum = 0  means take original model, copy nothing
  //                             and also : no Dispatch (bulk sending)

  applied.Nullify();
  const occ::handle<Interface_InterfaceModel>& original = G.Model();
  if (dispnum > 0)
  {
    newmod = original->NewEmptyModel();
    TC.Clear();
    WL->CopyModel(original, newmod, tocopy, TC);

    occ::handle<Standard_Transient> ent1, ent2;
    //  Feed Remaining : copied entities are to be noted
    for (int ic = TC.LastCopiedAfter(0, ent1, ent2); ic > 0;
         ic                  = TC.LastCopiedAfter(ic, ent1, ent2))
    {
      if (ic <= theremain->Upper())
        theremain->SetValue(ic, theremain->Value(ic) + 1);
    }
  }
  else if (newmod.IsNull())
    newmod = original;

  //  ...  Then : We take into account the Model Modifiers  ...
  int nbmod = 0;
  if (!theshareout.IsNull())
    nbmod = theshareout->NbModifiers(true);
  int i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nbmod; i++)
  {
    occ::handle<IFSelect_Modifier> unmod = theshareout->ModelModifier(i);

    //    First,  Dispatch/Packet criterion
    if (dispnum > 0)
      if (!unmod->Applies(theshareout->Dispatch(dispnum)))
        continue;
    IFSelect_ContextModif ctx(G, TC, filename.ToCString());
    //    Then, the Selection
    occ::handle<IFSelect_Selection> sel = unmod->Selection();
    if (!sel.IsNull())
    {
      Interface_EntityIterator entiter = sel->UniqueResult(G);
      ctx.Select(entiter);
    }
    if (ctx.IsForNone())
      continue;
    unmod->Perform(ctx, newmod, protocol, TC);
    Interface_CheckIterator checklst = ctx.CheckList();
    checks.Merge(checklst);

    //    Should we record errors in newmod ? good question
    //    if (!checks.IsEmpty(false)) {
    //      Message::SendWarning() <<
    //        " Messages on Copied Model n0 "<<numod<<", Dispatch Rank "<<dispnum<<std::endl;
    //      checks.Print(sout,newmod,false);
    //    }
  }

  //  ...  Then the File Modifiers : in fact, we record them  ...
  nbmod = 0;
  if (!theshareout.IsNull())
    nbmod = theshareout->NbModifiers(false);
  if (nbmod == 0)
    return;
  applied = new IFSelect_AppliedModifiers(nbmod, newmod->NbEntities());
  for (i = 1; i <= nbmod; i++)
  {
    occ::handle<IFSelect_GeneralModifier> unmod = theshareout->GeneralModifier(false, i);

    //    First,  Dispatch/Packet criterion
    if (dispnum > 0)
      if (!unmod->Applies(theshareout->Dispatch(dispnum)))
        continue;
    //    Then, the Selection
    occ::handle<IFSelect_Selection> sel = unmod->Selection();
    if (sel.IsNull())
      applied->AddModif(unmod); // empty -> we take all
    else
    {
      Interface_EntityIterator   list = sel->UniqueResult(G);
      occ::handle<Standard_Transient> newent;

      //    Entities designated by the Selection and Copied ?
      //    -> if there is at least one, the Modifier applies, otherwise it is rejected
      //    -> and this list is exploitable by the Modifier ...
      for (list.Start(); list.More(); list.Next())
      {
        if (TC.Search(list.Value(), newent))
          applied->AddNum(newmod->Number(newent));
      }
    }
  }
}

void IFSelect_ModelCopier::CopiedRemaining(const Interface_Graph&              G,
                                           const occ::handle<IFSelect_WorkLibrary>& WL,
                                           Interface_CopyTool&                 TC,
                                           occ::handle<Interface_InterfaceModel>&   newmod)
{
  const occ::handle<Interface_InterfaceModel>& original = G.Model();
  //  Interface_CopyTool TC(original,protocol);
  newmod = original->NewEmptyModel();
  TC.Clear();
  Interface_EntityIterator tocopy;
  int         nb = G.Size();
  theremain                   = new NCollection_HArray1<int>(0, nb + 1);
  theremain->Init(0);
  for (int i = 1; i <= nb; i++)
  {
    if (G.Status(i) == 0)
      tocopy.AddItem(original->Value(i));
    else
      theremain->SetValue(i, -1); //  ?? -1
  }
  WL->CopyModel(original, newmod, tocopy, TC);

  if (newmod->NbEntities() == 0)
    newmod.Nullify();
  else
  {
    //  WHAT FOLLOWS MUST NOT BE DELETED ! cf theremain
    occ::handle<Standard_Transient> ent1, ent2;
    for (int ic = TC.LastCopiedAfter(0, ent1, ent2); ic > 0;
         ic                  = TC.LastCopiedAfter(ic, ent1, ent2))
    {
      if (ic <= theremain->Upper())
        theremain->SetValue(ic, 1);
    }
//  some debugging prints
#ifdef MISOPOINT
    std::cout << " Remaining Model : " << newmod->NbEntities() << " Entities" << std::endl;
    int ne = 0;
    for (i = 1; i <= nb; i++)
    {
      if (theremain->Value(i) == 0)
      {
        if (ne == 0)
          std::cout << " Refractory : ";
        ne++;
        std::cout << " " << i;
      }
    }
    if (ne > 0)
      std::cout << "  -- " << ne << " Entities" << std::endl;
    else
      std::cout << "  -- Remaining data complete" << std::endl;
#endif
  }
}

bool IFSelect_ModelCopier::SetRemaining(Interface_Graph& CG) const
{
  int nb = CG.Size();
  if (theremain.IsNull())
    return (nb == 0);
  if (nb != theremain->Upper())
    return false;
  for (int i = 1; i <= nb; i++)
  {
    if (CG.Status(i) >= 0)
      CG.SetStatus(i, CG.Status(i) + theremain->Value(i));
  }
  theremain->Init(0);
  return true;
}

//  ##########################################################################
//  ########        RESULT of Transfer Memorization        ########

int IFSelect_ModelCopier::NbFiles() const
{
  return thefilemodels.Length();
}

TCollection_AsciiString IFSelect_ModelCopier::FileName(const int num) const
{
  return thefilenames.Value(num);
}

occ::handle<Interface_InterfaceModel> IFSelect_ModelCopier::FileModel(const int num) const
{
  return thefilemodels.Value(num);
}

occ::handle<IFSelect_AppliedModifiers> IFSelect_ModelCopier::AppliedModifiers(
  const int num) const
{
  return theapplieds.Value(num);
}

void IFSelect_ModelCopier::BeginSentFiles(const occ::handle<IFSelect_ShareOut>& sho,
                                          const bool           record)
{
  thesentfiles.Nullify();
  if (record)
    thesentfiles = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  //  and default file numbering : held by ShareOut
  if (sho.IsNull())
    return;
  int lastrun = sho->LastRun();
  sho->ClearResult(true);
  sho->SetLastRun(lastrun); // we are only interested in the numbers
}

void IFSelect_ModelCopier::AddSentFile(const char* const filename)
{
  if (!thesentfiles.IsNull())
    thesentfiles->Append(new TCollection_HAsciiString(filename));
}

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> IFSelect_ModelCopier::SentFiles() const
{
  return thesentfiles;
}
