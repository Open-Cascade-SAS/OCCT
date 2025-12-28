// Created on: 1992-02-03
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Transfer_ProcessForFinder.hxx>

#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceError.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Message_ProgressScope.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Map.hxx>
#include <Transfer_ActorOfProcessForFinder.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FindHasher.hxx>
#include <Transfer_IteratorOfProcessForFinder.hxx>
#include <Transfer_MultipleBinder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_StatusResult.hxx>
#include <Transfer_TransferFailure.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Transfer_VoidBinder.hxx>

//=================================================================================================

Transfer_ProcessForFinder::Transfer_ProcessForFinder(const int nb)
    : themap(nb)
{
  theerrh      = true;
  therootm     = false;
  thelevel     = 0;
  therootl     = 0;
  themessenger = Message::DefaultMessenger();
  thetrace     = 0;
  theindex     = 0;
}

//=================================================================================================

Transfer_ProcessForFinder::Transfer_ProcessForFinder(
  const occ::handle<Message_Messenger>& messenger,
  const int                             nb)
    : themap(nb)
{
  theerrh  = true;
  therootm = false;
  thelevel = 0;
  therootl = 0;
  SetMessenger(messenger);
  thetrace = 1;
  theindex = 0;
}

//=================================================================================================

void Transfer_ProcessForFinder::Clear()
{
  thelevel = 0;
  therootl = 0;
  theroots.Clear();
  themap.Clear();
  theindex = 0;
  thelastobj.Nullify();
  thelastbnd.Nullify();
}

//=================================================================================================

void Transfer_ProcessForFinder::Clean()
{
  int i, nb  = NbMapped();
  int j, unb = 0;
  for (i = 1; i <= nb; i++)
  {
    if (themap(i).IsNull())
      unb++;
  }
  if (unb == 0)
    return;

  // Redo the map -> offsets
  NCollection_Array1<int> unbs(1, nb);
  unbs.Init(0);
  NCollection_IndexedDataMap<occ::handle<Transfer_Finder>,
                             occ::handle<Transfer_Binder>,
                             Transfer_FindHasher>
    newmap(nb * 2);
  for (i = 1; i <= nb; i++)
  {
    occ::handle<Transfer_Finder> ent = Mapped(i);
    occ::handle<Transfer_Binder> bnd = MapItem(i);
    if (bnd.IsNull())
      continue;
    j = newmap.Add(ent, bnd);
    unbs.SetValue(i, j);
  }
  themap.Assign(newmap);

  // Update Root List
  NCollection_IndexedMap<int> aNewRoots;
  for (i = 1; i <= theroots.Extent(); i++)
  {
    j     = theroots.FindKey(i);
    int k = unbs.Value(j);
    if (k)
      aNewRoots.Add(k);
  }
  theroots.Clear();
  theroots = aNewRoots;

  // The rest: cleaning
  thelastobj.Nullify();
  thelastbnd.Nullify();
  theindex = 0;
}

//=================================================================================================

void Transfer_ProcessForFinder::Resize(const int nb)
{
  if (nb > themap.NbBuckets())
    themap.ReSize(nb);
}

//=================================================================================================

void Transfer_ProcessForFinder::SetActor(const occ::handle<Transfer_ActorOfProcessForFinder>& actor)
{
  if (theactor == actor)
    return;
  if (theactor.IsNull())
    theactor = actor;
  else if (actor.IsNull())
    theactor = actor;
  else if (theactor->IsLast())
  {
    actor->SetNext(theactor);
    theactor = actor;
  }
  else
    theactor->SetNext(actor);
}

//=================================================================================================

occ::handle<Transfer_ActorOfProcessForFinder> Transfer_ProcessForFinder::Actor() const
{
  return theactor;
}

//=================================================================================================

occ::handle<Transfer_Binder> Transfer_ProcessForFinder::Find(
  const occ::handle<Transfer_Finder>& start) const
{
  if (thelastobj == start)
  {
    if (theindex > 0)
      return thelastbnd;
  }
  int index = themap.FindIndex(start);
  if (index > 0)
  {
    const occ::handle<Transfer_Binder>& binder = themap.FindFromIndex(index);
    return binder;
  }
  return occ::handle<Transfer_Binder>();
}

//=================================================================================================

bool Transfer_ProcessForFinder::IsBound(const occ::handle<Transfer_Finder>& start) const
{
  occ::handle<Transfer_Binder> binder = Find(start);
  if (binder.IsNull())
    return false;
  return binder->HasResult();
}

//=================================================================================================

bool Transfer_ProcessForFinder::IsAlreadyUsed(const occ::handle<Transfer_Finder>& start) const
{
  occ::handle<Transfer_Binder> binder = Find(start);
  if (binder.IsNull())
  {
    StartTrace(binder, start, thelevel, 4);
    throw Transfer_TransferFailure(
      "TransferProcess : IsAlreadyUsed, transfer not done cannot be used...");
  }
  return (binder->Status() == Transfer_StatusUsed);
}

//=================================================================================================

occ::handle<Transfer_Binder> Transfer_ProcessForFinder::FindAndMask(
  const occ::handle<Transfer_Finder>& start)
{
  if (thelastobj == start)
  {
    if (theindex > 0)
      return thelastbnd;
  }
  thelastobj = start;
  theindex   = themap.FindIndex(start);
  if (theindex > 0)
    thelastbnd = themap.FindFromIndex(theindex);
  else
    thelastbnd.Nullify();
  return thelastbnd;
}

//=================================================================================================

void Transfer_ProcessForFinder::Bind(const occ::handle<Transfer_Finder>& start,
                                     const occ::handle<Transfer_Binder>& binder)
{
  if (binder.IsNull())
    return;
  occ::handle<Transfer_Binder> former = FindAndMask(start);
  if (!former.IsNull())
  {
    // We admit VoidBinder: then we resume our Check
    if (former->DynamicType() == STANDARD_TYPE(Transfer_VoidBinder))
    {
      binder->Merge(former);
      themap(theindex) = binder;
    }
    else if (former->Status() == Transfer_StatusUsed)
    {
      StartTrace(former, start, thelevel, 4);
      throw Transfer_TransferFailure("TransferProcess : Bind, already Bound");
    }
    else
    {
      if (thetrace > 2)
        StartTrace(former, start, thelevel, 5);
      binder->CCheck()->GetMessages(former->Check());
    }
  }
  if (theindex == 0 || thelastbnd.IsNull())
  {
    if (theindex == 0)
      theindex = themap.Add(start, binder);
    else
      themap(theindex) = binder;
    thelastbnd = binder;
  }
  else
  {
    thelastbnd       = binder;
    themap(theindex) = binder;
  }
}

//=================================================================================================

void Transfer_ProcessForFinder::Rebind(const occ::handle<Transfer_Finder>& start,
                                       const occ::handle<Transfer_Binder>& binder)
{
  Bind(start, binder);
}

//=================================================================================================

bool Transfer_ProcessForFinder::Unbind(const occ::handle<Transfer_Finder>& start)
{
  occ::handle<Transfer_Binder> former = FindAndMask(start);
  if (theindex == 0)
    return false;
  if (former.IsNull())
    return false;
  if (former->DynamicType() == STANDARD_TYPE(Transfer_VoidBinder))
    return true;
  themap(theindex) = thelastbnd;
  if (theroots.Contains(theindex))
  {
    NCollection_IndexedMap<int> aNewRoots;
    for (int i = 1; i <= theroots.Extent(); i++)
      if (theindex != theroots.FindKey(i))
        aNewRoots.Add(theroots.FindKey(i));

    theroots.Clear();
    theroots = aNewRoots;
  }

  thelastobj.Nullify();
  thelastbnd.Nullify();
  theindex = 0;
  return true;
}

//=================================================================================================

occ::handle<Transfer_Binder> Transfer_ProcessForFinder::FindElseBind(
  const occ::handle<Transfer_Finder>& start)
{
  occ::handle<Transfer_Binder> binder = FindAndMask(start);
  if (!binder.IsNull())
    return binder;
  binder = new Transfer_VoidBinder;
  Bind(start, binder);
  return binder;
}

//=================================================================================================

void Transfer_ProcessForFinder::SetMessenger(const occ::handle<Message_Messenger>& messenger)
{
  if (messenger.IsNull())
    themessenger = Message::DefaultMessenger();
  else
    themessenger = messenger;
}

//=================================================================================================

occ::handle<Message_Messenger> Transfer_ProcessForFinder::Messenger() const
{
  return themessenger;
}

//=================================================================================================

void Transfer_ProcessForFinder::SetTraceLevel(const int tracelev)
{
  thetrace = tracelev;
}

//=================================================================================================

int Transfer_ProcessForFinder::TraceLevel() const
{
  return thetrace;
}

//=================================================================================================

void Transfer_ProcessForFinder::SendFail(const occ::handle<Transfer_Finder>& start,
                                         const Message_Msg&                  amsg)
{
  AddFail(start, amsg);
}

//=================================================================================================

void Transfer_ProcessForFinder::SendWarning(const occ::handle<Transfer_Finder>& start,
                                            const Message_Msg&                  amsg)
{
  AddWarning(start, amsg);
}

//=================================================================================================

void Transfer_ProcessForFinder::SendMsg(const occ::handle<Transfer_Finder>& start,
                                        const Message_Msg&                  amsg)
{
  occ::handle<Transfer_Binder> binder = FindAndMask(start);
  if (binder.IsNull())
  {
    binder = new Transfer_VoidBinder;
    Bind(start, binder);
  }
  // Feeds the trace: Causing rule (user messages)
  if (thetrace > 0)
  {
    StartTrace(binder, start, thelevel, 6);
    Message_Messenger::StreamBuffer aSender = themessenger->SendInfo();
    aSender << amsg.Value();
    if (amsg.IsEdited() && thetrace > 2)
      aSender << " [from: " << amsg.Original() << "]";
    aSender << std::endl;
  }
}

//=================================================================================================

void Transfer_ProcessForFinder::AddFail(const occ::handle<Transfer_Finder>& start,
                                        const char*                         mess,
                                        const char*                         orig)
{
  occ::handle<Transfer_Binder> binder = FindAndMask(start);
  if (binder.IsNull())
  {
    binder = new Transfer_VoidBinder;
    Bind(start, binder);
  }
  binder->AddFail(mess, orig);
  if (thetrace > 0)
  {
    StartTrace(binder, start, thelevel, 1);
    Message_Messenger::StreamBuffer aSender = themessenger->SendFail();
    aSender << "    --> Fail : " << mess;
    if (orig[0] != '\0' && thetrace > 2)
      aSender << " [from: " << orig << "]";
    aSender << std::endl;
  }
}

//=================================================================================================

void Transfer_ProcessForFinder::AddError(const occ::handle<Transfer_Finder>& start,
                                         const char*                         mess,
                                         const char*                         orig)
{
  AddFail(start, mess, orig);
}

//=================================================================================================

void Transfer_ProcessForFinder::AddFail(const occ::handle<Transfer_Finder>& start,
                                        const Message_Msg&                  amsg)
{
  if (amsg.IsEdited())
    AddFail(start,
            TCollection_AsciiString(amsg.Value()).ToCString(),
            TCollection_AsciiString(amsg.Original()).ToCString());
  else
    AddFail(start, TCollection_AsciiString(amsg.Value()).ToCString());
}

//=================================================================================================

void Transfer_ProcessForFinder::AddWarning(const occ::handle<Transfer_Finder>& start,
                                           const char*                         mess,
                                           const char*                         orig)
{
  occ::handle<Transfer_Binder> binder = FindAndMask(start);
  if (binder.IsNull())
  {
    binder = new Transfer_VoidBinder;
    Bind(start, binder);
  }
  binder->AddWarning(mess, orig);
  if (thetrace > 1)
  {
    StartTrace(binder, start, thelevel, 2);
    Message_Messenger::StreamBuffer aSender = themessenger->SendWarning();
    aSender << "    --> Warning : " << mess;
    if (orig[0] != '\0' && thetrace > 2)
      aSender << " [from: " << orig << "]";
    aSender << std::endl;
  }
}

//=================================================================================================

void Transfer_ProcessForFinder::AddWarning(const occ::handle<Transfer_Finder>& start,
                                           const Message_Msg&                  amsg)
{
  if (amsg.IsEdited())
    AddWarning(start,
               TCollection_AsciiString(amsg.Value()).ToCString(),
               TCollection_AsciiString(amsg.Original()).ToCString());
  else
    AddWarning(start, TCollection_AsciiString(amsg.Value()).ToCString());
}

//=================================================================================================

void Transfer_ProcessForFinder::Mend(const occ::handle<Transfer_Finder>& start, const char* pref)
{
  occ::handle<Transfer_Binder> binder = FindAndMask(start);
  if (binder.IsNull())
    return; // nothing to do ...
  occ::handle<Interface_Check> ach = binder->CCheck();
  ach->Mend(pref);
}

//=================================================================================================

occ::handle<Interface_Check> Transfer_ProcessForFinder::Check(
  const occ::handle<Transfer_Finder>& start) const
{
  const occ::handle<Transfer_Binder>& binder = Find(start);
  if (binder.IsNull())
  {
    occ::handle<Interface_Check> check;
    return check;
  }
  return binder->Check();
}

//=================================================================================================

void Transfer_ProcessForFinder::BindTransient(const occ::handle<Transfer_Finder>&    start,
                                              const occ::handle<Standard_Transient>& res)
{
  if (res.IsNull())
    return;
  occ::handle<Transfer_Binder>                  former = Find(start);
  occ::handle<Transfer_SimpleBinderOfTransient> binder =
    occ::down_cast<Transfer_SimpleBinderOfTransient>(former);
  // Binding in place?
  if (!binder.IsNull())
  {
    if (binder->Status() == Transfer_StatusVoid)
    {
      binder->SetResult(res);
      return;
    }
  }
  // Otherwise, redo
  binder = new Transfer_SimpleBinderOfTransient;
  binder->SetResult(res);
  if (former.IsNull())
    Bind(start, binder);
  else
    Rebind(start, binder);
}

//=================================================================================================

const occ::handle<Standard_Transient>& Transfer_ProcessForFinder::FindTransient(
  const occ::handle<Transfer_Finder>& start) const
{
  static const occ::handle<Standard_Transient>  aDummy;
  occ::handle<Transfer_SimpleBinderOfTransient> binder =
    occ::down_cast<Transfer_SimpleBinderOfTransient>(Find(start));
  if (binder.IsNull())
    return aDummy;
  if (!binder->HasResult())
    return aDummy;
  return binder->Result();
}

//=================================================================================================

void Transfer_ProcessForFinder::BindMultiple(const occ::handle<Transfer_Finder>& start)
{
  occ::handle<Transfer_Binder> binder = FindAndMask(start);
  if (!binder.IsNull())
  {
    if (!binder->IsKind(STANDARD_TYPE(Transfer_MultipleBinder)))
    {
      StartTrace(thelastbnd, start, thelevel, 4);
      throw Transfer_TransferFailure("TransferProcess : BindMultiple");
    }
  }
  else
    Bind(start, new Transfer_MultipleBinder);
}

//=================================================================================================

void Transfer_ProcessForFinder::AddMultiple(const occ::handle<Transfer_Finder>&    start,
                                            const occ::handle<Standard_Transient>& res)
{
  occ::handle<Transfer_Binder>         binder = FindAndMask(start);
  occ::handle<Transfer_MultipleBinder> multr  = occ::down_cast<Transfer_MultipleBinder>(binder);
  if (multr.IsNull())
  {
    StartTrace(binder, start, thelevel, 4);
    if (binder.IsNull())
      throw Transfer_TransferFailure("TransferProcess : AddMultiple, nothing bound");
    else
      throw Transfer_TransferFailure("TransferProcess : AddMultiple, Binder not a MultipleBinder");
  }
  multr->AddResult(res);
}

//=================================================================================================

bool Transfer_ProcessForFinder::FindTypedTransient(const occ::handle<Transfer_Finder>& start,
                                                   const occ::handle<Standard_Type>&   atype,
                                                   occ::handle<Standard_Transient>&    val) const
{
  return GetTypedTransient(Find(start), atype, val);
}

//=================================================================================================

bool Transfer_ProcessForFinder::GetTypedTransient(const occ::handle<Transfer_Binder>& binder,
                                                  const occ::handle<Standard_Type>&   atype,
                                                  occ::handle<Standard_Transient>&    val) const
{
  return Transfer_SimpleBinderOfTransient::GetTypedResult(binder, atype, val);
}

//=================================================================================================

int Transfer_ProcessForFinder::NbMapped() const
{
  return themap.Extent();
}

//=================================================================================================

const occ::handle<Transfer_Finder>& Transfer_ProcessForFinder::Mapped(const int num) const
{
  return themap.FindKey(num);
}

//=================================================================================================

int Transfer_ProcessForFinder::MapIndex(const occ::handle<Transfer_Finder>& start) const
{
  return themap.FindIndex(start);
}

//=================================================================================================

occ::handle<Transfer_Binder> Transfer_ProcessForFinder::MapItem(const int num) const
{
  occ::handle<Transfer_Binder> binder = themap.FindFromIndex(num);
  return binder;
}

//=================================================================================================

void Transfer_ProcessForFinder::SetRoot(const occ::handle<Transfer_Finder>& start)
{
  int index = MapIndex(start);
  if (index == 0)
  {
    return;
  }
  theroots.Add(index);
  if (thetrace > 2)
    StartTrace(MapItem(index), start, thelevel, 3);
}

//=================================================================================================

void Transfer_ProcessForFinder::SetRootManagement(const bool stat)
{
  therootm = stat;
}

//=================================================================================================

int Transfer_ProcessForFinder::NbRoots() const
{
  return theroots.Extent();
}

//=================================================================================================

const occ::handle<Transfer_Finder>& Transfer_ProcessForFinder::Root(const int num) const
{
  int ind = 0;
  if (num > 0 && num <= theroots.Extent())
    ind = theroots.FindKey(num);
  return themap.FindKey(ind);
}

//=================================================================================================

occ::handle<Transfer_Binder> Transfer_ProcessForFinder::RootItem(const int num) const
{
  int ind = 0;
  if (num > 0 && num <= theroots.Extent())
    ind = theroots.FindKey(num);
  return themap.FindFromIndex(ind);
}

//=================================================================================================

int Transfer_ProcessForFinder::RootIndex(const occ::handle<Transfer_Finder>& start) const
{
  int index = MapIndex(start);
  if (index == 0)
    return 0;
  return theroots.FindIndex(index);
}

//=================================================================================================

int Transfer_ProcessForFinder::NestingLevel() const
{
  return thelevel;
}

//=================================================================================================

void Transfer_ProcessForFinder::ResetNestingLevel()
{
  thelevel = 0;
}

//=================================================================================================

bool Transfer_ProcessForFinder::Recognize(const occ::handle<Transfer_Finder>& start) const
{
  occ::handle<Transfer_ActorOfProcessForFinder> actor = theactor;
  // We scan the Next until we have a Result
  while (!actor.IsNull())
  {
    if (actor->Recognize(start))
      return true;
    actor = actor->Next();
  }
  return false;
}

//=================================================================================================

occ::handle<Transfer_Binder> Transfer_ProcessForFinder::Transferring(
  const occ::handle<Transfer_Finder>& start,
  const Message_ProgressRange&        theProgress)
{
  occ::handle<Transfer_Binder> former = FindAndMask(start);
  // Transfer already done with Result?

  // We consider that this new Transfer request therefore corresponds to a
  // additional use: note "AlreadyUsed", therefore non-modifiable result
  bool hasDeadLoop = false;
  if (!former.IsNull())
  {
    if (former->HasResult())
    {
      former->SetAlreadyUsed();
      return former;
    }
    // Initial State: perhaps already done... or infeasible!
    Message_Messenger::StreamBuffer aSender = themessenger->SendInfo();
    Transfer_StatusExec             statex  = former->StatusExec();
    switch (statex)
    {
      case Transfer_StatusInitial:
        break;
      case Transfer_StatusDone:
        aSender << " .. and Transfer done" << std::endl;
        return former;
      case Transfer_StatusRun:
        former->SetStatusExec(Transfer_StatusLoop);
        return former;
      case Transfer_StatusError:
        if (thetrace)
        {
          aSender << "                  *** Transfer in Error Status  :" << std::endl;
          StartTrace(former, start, thelevel, 0);
        }
        else
          StartTrace(former, start, thelevel, 4);
        throw Transfer_TransferFailure("TransferProcess : Transfer in Error Status");
      case Transfer_StatusLoop:
        if (thetrace)
        {
          aSender << "                  *** Transfer  Head of Dead Loop  :" << std::endl;
          StartTrace(former, start, thelevel, 0);
        }
        else
          StartTrace(former, start, thelevel, 4);
        hasDeadLoop = true;
        break;
    }
    if (!hasDeadLoop)
      former->SetStatusExec(Transfer_StatusRun);
  }
#ifdef TRANSLOG
  std::cout << " GO .." << std::endl;
#endif

  occ::handle<Transfer_Binder> binder;
  bool                         newbind = false;

  // Handle dead loop condition detected before calling TransferProduct
  // When hasDeadLoop is true, we know:
  // 1. former is NOT null (hasDeadLoop only set inside !former.IsNull() block)
  // 2. former->StatusExec() == Transfer_StatusLoop (that's what triggered hasDeadLoop)
  if (hasDeadLoop)
  {
    Message_Messenger::StreamBuffer aSender = themessenger->SendInfo();
    if (thetrace)
    {
      aSender << "                  *** Dead Loop : Finding head of Loop :" << std::endl;
      StartTrace(former, start, thelevel - 1, 0);
    }
    else
      StartTrace(former, start, thelevel - 1, 4);
    binder = former;
    binder->AddFail("Transfer: Head of Dead Loop");
    binder->AddFail("Transfer in dead Loop");
  }
  else if (theerrh)
  {
    Message_Messenger::StreamBuffer aSender = themessenger->SendInfo();

    // Transfer under protection for exceptions (for notification in fact)
    int oldlev = thelevel;
    try
    {
      OCC_CATCH_SIGNALS
      binder = TransferProduct(start, theProgress);
    }
    catch (Standard_Failure const& anException)
    {
      if (binder.IsNull())
      {
        aSender << "                  *** Exception Raised with no Result" << std::endl;
        binder = new Transfer_VoidBinder;
        Bind(start, binder);
        newbind = true;
      }
      binder->AddFail("Transfer stopped by exception raising");
      if (thetrace)
      {
        aSender << "    *** Raised : " << anException.GetMessageString() << std::endl;
        StartTrace(binder, start, thelevel - 1, 4);
      }
      thelevel = oldlev;
    }
  }

  // Unprotected transfer (thus, dbx by hand in case of crash by Raise)
  else
    binder = TransferProduct(start, theProgress);

  if (theProgress.UserBreak())
    return occ::handle<Transfer_Binder>();

  // Conclusion: Note in the Map

  if (!newbind && !binder.IsNull())
  {
    if (former.IsNull())
    {
      // Maybe <theactor> did Bind himself... check if not do it
      if (!IsBound(start))
        Bind(start, binder);
      else
      {
        Rebind(start, binder);
      }
    }
    else
      Rebind(start, binder);
  }
  else
  {
    if (!former.IsNull())
      former->SetStatusExec(Transfer_StatusDone); //+
    return occ::handle<Transfer_Binder>();
  }

  // Manage Roots (if provided)

  if (therootl >= thelevel)
  {
    therootl = 0;
    if (therootm && binder->Status() != Transfer_StatusVoid)
    {
      SetRoot(start);
    }
  }
  return thelastbnd;
}

//=================================================================================================

occ::handle<Transfer_Binder> Transfer_ProcessForFinder::TransferProduct(
  const occ::handle<Transfer_Finder>& start,
  const Message_ProgressRange&        theProgress)
{
  thelevel++; // if decrements and == 0, root transfer
  occ::handle<Transfer_Binder>                  binder;
  occ::handle<Transfer_ActorOfProcessForFinder> actor = theactor;

  // We scan the Next until we have a Result
  Message_ProgressScope aScope(theProgress, NULL, 1, true);
  while (!actor.IsNull())
  {
    if (actor->Recognize(start))
      binder = actor->Transferring(start, this, aScope.Next());
    else
      binder.Nullify();
    if (!binder.IsNull())
      break;
    actor = actor->Next();
  }
  if (aScope.UserBreak())
    return occ::handle<Transfer_Binder>();

  if (binder.IsNull())
  {
    if (thelevel > 0)
      thelevel--;
    return binder;
  }
  // Root level management (.. take a closer look..)
  if (therootl == 0 && binder->StatusExec() == Transfer_StatusDone)
    therootl = thelevel - 1;

  if (thelevel > 0)
    thelevel--;
  return binder;
}

//=================================================================================================

bool Transfer_ProcessForFinder::Transfer(const occ::handle<Transfer_Finder>& start,
                                         const Message_ProgressRange&        theProgress)
{
  occ::handle<Transfer_Binder> binder = Transferring(start, theProgress);
  return (!binder.IsNull());
}

//=================================================================================================

void Transfer_ProcessForFinder::SetErrorHandle(const bool err)
{
  theerrh = err;
}

//=================================================================================================

bool Transfer_ProcessForFinder::ErrorHandle() const
{
  return theerrh;
}

//=================================================================================================

void Transfer_ProcessForFinder::StartTrace(const occ::handle<Transfer_Binder>& binder,
                                           const occ::handle<Transfer_Finder>& start,
                                           const int                           level,
                                           const int                           mode) const
{
  Message_Messenger::StreamBuffer aSender = themessenger->SendInfo();
  // ###  Fail (Roots:50)  --  Start start->DynamicType()
  // ###  Fail (Roots:50)  --  Start id:#label.. Type:start->DynamicType()
  if (thetrace > 3)
  { // Internal to be switch when searching bug (trace >= 4)
    if (mode == 1)
      aSender << "  ###  Fail";
    if (mode == 2)
      aSender << "  ###  Warning";
    if (mode == 3)
      aSender << "  ###  New Root n0 " << theroots.Extent();
    if (mode == 4)
      aSender << "  ###  Exception";
    if (mode == 5)
      aSender << "  ###  Substitution";
    if (mode == 6)
      aSender << "  ###  Information";
    if (level > 1)
      aSender << " (nested)";
    if (mode >= 0 && mode != 3)
      aSender << " at " << theroots.Extent() << " Roots";
  }
  if (!start.IsNull())
    PrintTrace(start, aSender);

  if (!binder.IsNull())
  {
    occ::handle<Transfer_Binder> bnd    = binder;
    bool                         hasres = false;
    while (!bnd.IsNull())
    {
      if (bnd->Status() != Transfer_StatusVoid)
      {
        if (!hasres)
          aSender << "\n  ---  Result Type : ";
        else
          aSender << " , ";
        aSender << bnd->ResultTypeName();
        hasres = true;
      }
      bnd = bnd->NextResult();
    }
    if (!hasres && mode > 2)
    {
      aSender << "\n  ---  No Result recorded";
    }
  }
  aSender << std::endl;
}

//=================================================================================================

void Transfer_ProcessForFinder::PrintTrace(const occ::handle<Transfer_Finder>& start,
                                           Standard_OStream&                   S) const
{
  if (!start.IsNull())
    S << " Type:" << start->DynamicType()->Name();
}

//=================================================================================================

bool Transfer_ProcessForFinder::IsLooping(const int alevel) const
{
  return alevel > NbMapped();
}

//=================================================================================================

Transfer_IteratorOfProcessForFinder Transfer_ProcessForFinder::RootResult(
  const bool withstart) const
{
  Transfer_IteratorOfProcessForFinder iter(withstart);
  int                                 max = theroots.Extent();
  for (int j = 1; j <= max; j++)
  {
    int                          i      = theroots.FindKey(j);
    occ::handle<Transfer_Binder> binder = MapItem(i);
    if (binder.IsNull())
      continue;
    if (withstart)
      iter.Add(binder, Mapped(i));
    else
      iter.Add(binder);
  }
  return iter;
}

//=================================================================================================

Transfer_IteratorOfProcessForFinder Transfer_ProcessForFinder::CompleteResult(
  const bool withstart) const
{
  Transfer_IteratorOfProcessForFinder iter(withstart);
  int                                 max = NbMapped();
  for (int i = 1; i <= max; i++)
  {
    occ::handle<Transfer_Binder> binder = MapItem(i);
    if (binder.IsNull())
      continue;
    if (withstart)
      iter.Add(binder, Mapped(i));
    else
      iter.Add(binder);
  }
  return iter;
}

//=================================================================================================

Transfer_IteratorOfProcessForFinder Transfer_ProcessForFinder::AbnormalResult() const
{
  Transfer_IteratorOfProcessForFinder iter(true);
  int                                 max = NbMapped();
  for (int i = 1; i <= max; i++)
  {
    occ::handle<Transfer_Binder> binder = MapItem(i);
    if (binder.IsNull())
      continue;
    Transfer_StatusExec statex = binder->StatusExec();
    if (statex != Transfer_StatusInitial && statex != Transfer_StatusDone)
      iter.Add(binder, Mapped(i)); // we note the "not normal" cases
  }
  return iter;
}

//=================================================================================================

Interface_CheckIterator Transfer_ProcessForFinder::CheckList(const bool erronly) const
{
  Interface_CheckIterator list;
  int                     num, max = NbMapped();
  for (int i = 1; i <= max; i++)
  {
    occ::handle<Transfer_Binder> binder = MapItem(i);
    if (binder.IsNull())
      continue;
    Transfer_StatusExec          statex = binder->StatusExec();
    occ::handle<Interface_Check> check  = binder->Check();
    if (statex != Transfer_StatusInitial && statex != Transfer_StatusDone && !check->HasFailed())
      check->AddFail("Transfer in Abnormal Status (!= Initial or Done)");
    if (!check->HasFailed() && (erronly || check->NbWarnings() == 0))
      continue;
    const occ::handle<Transfer_Finder>& ent = Mapped(i);
    num                                     = CheckNum(ent);
    if (num == 0)
      num = i;
    check->SetEntity(ent);
    list.Add(check, num);
  }
  return list;
}

//=================================================================================================

Transfer_IteratorOfProcessForFinder Transfer_ProcessForFinder::ResultOne(
  const occ::handle<Transfer_Finder>& start,
  const int                           level,
  const bool                          withstart) const
{
  Transfer_IteratorOfProcessForFinder iter(withstart);
  int                                 max = NbMapped();
  int                                 ind = MapIndex(start);
  if (ind == 0)
    return iter;
  int                                   i1  = (level == 0 ? ind : 1);
  int                                   i2  = (level == 0 ? ind : max);
  occ::handle<NCollection_HArray1<int>> map = new NCollection_HArray1<int>(i1, i2, 0);

  for (int i = i1; i <= i2; i++)
  {
    ind = map->Value(i);
    if (ind == 0)
      continue;
    occ::handle<Transfer_Binder> binder = MapItem(i);
    if (binder.IsNull())
      continue;
    if (withstart)
      iter.Add(binder, Mapped(ind));
    else
      iter.Add(binder);
  }
  return iter;
}

//=================================================================================================

Interface_CheckIterator Transfer_ProcessForFinder::CheckListOne(
  const occ::handle<Transfer_Finder>& start,
  const int                           level,
  const bool                          erronly) const
{
  Interface_CheckIterator list;
  int                     max = NbMapped();
  int                     num, ind = MapIndex(start);
  if (ind == 0)
    return list;
  int                                   i1  = (level == 0 ? ind : 1);
  int                                   i2  = (level == 0 ? ind : max);
  occ::handle<NCollection_HArray1<int>> map = new NCollection_HArray1<int>(i1, i2, 0);

  for (int i = i1; i <= i2; i++)
  {
    ind = map->Value(i);
    if (ind == 0)
      continue;
    occ::handle<Transfer_Binder> binder = MapItem(ind);
    if (binder.IsNull())
      continue;
    Transfer_StatusExec          statex = binder->StatusExec();
    occ::handle<Interface_Check> check  = binder->Check();
    if (statex != Transfer_StatusInitial && statex != Transfer_StatusDone && !check->HasFailed())
      check->AddFail("Transfer in Abnormal Status (!= Initial or Done)");
    if (!check->HasFailed() && (erronly || check->NbWarnings() == 0))
      continue;
    const occ::handle<Transfer_Finder>& ent = Mapped(ind);
    num                                     = CheckNum(ent);
    if (num == 0)
      num = ind;
    check->SetEntity(ent);
    list.Add(check, num);
  }
  return list;
}

//=================================================================================================

bool Transfer_ProcessForFinder::IsCheckListEmpty(const occ::handle<Transfer_Finder>& start,
                                                 const int                           level,
                                                 const bool                          erronly) const
{
  int max = NbMapped();
  int ind = MapIndex(start);
  if (ind == 0)
    return false;
  int                                   i1  = (level == 0 ? ind : 1);
  int                                   i2  = (level == 0 ? ind : max);
  occ::handle<NCollection_HArray1<int>> map = new NCollection_HArray1<int>(i1, i2, 0);

  for (int i = i1; i <= i2; i++)
  {
    ind = map->Value(i);
    if (ind == 0)
      continue;
    occ::handle<Transfer_Binder> binder = MapItem(ind);
    if (binder.IsNull())
      continue;

    Transfer_StatusExec          statex = binder->StatusExec();
    occ::handle<Interface_Check> check  = binder->Check();
    if (statex != Transfer_StatusInitial && statex != Transfer_StatusDone)
      return false;
    if (check->HasFailed() || (!erronly && check->NbWarnings() > 0))
      return false;
  }
  return true;
}

//=================================================================================================

void Transfer_ProcessForFinder::RemoveResult(const occ::handle<Transfer_Finder>& start,
                                             const int                           level,
                                             const bool /*compute*/)
{
  int max = NbMapped();
  int ind = MapIndex(start);
  if (ind == 0)
    return;
  int                                   i1  = (level == 0 ? ind : 1);
  int                                   i2  = (level == 0 ? ind : max);
  occ::handle<NCollection_HArray1<int>> map = new NCollection_HArray1<int>(i1, i2, 0);

  int i;
  for (i = i1; i <= i2; i++)
  {
    ind = map->Value(i);
    if (ind == 0)
      continue;
    occ::handle<Transfer_Binder> binder = MapItem(ind);
    if (binder.IsNull())
      continue;
  }
}

//=================================================================================================

int Transfer_ProcessForFinder::CheckNum(const occ::handle<Transfer_Finder>&) const
{
  return 0;
}
