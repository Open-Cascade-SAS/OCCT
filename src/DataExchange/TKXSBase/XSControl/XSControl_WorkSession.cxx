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

//: i1 pdn 03.04.99  BUC60301

#include <Geom2d_Point.hxx>
#include <Interface_Check.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Transfer_ResultFromTransient.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_Vars.hxx>
#include <XSControl_WorkSession.hxx>

#include <mutex>

IMPLEMENT_STANDARD_RTTIEXT(XSControl_WorkSession, IFSelect_WorkSession)

namespace
{
std::mutex& GetGlobalMutex()
{
  static std::mutex WS_GLOBAL_MUTEX;
  return WS_GLOBAL_MUTEX;
}
} // namespace

//=================================================================================================

XSControl_WorkSession::XSControl_WorkSession()
    : myTransferReader(new XSControl_TransferReader),
      myTransferWriter(new XSControl_TransferWriter),
      myVars(new XSControl_Vars)
{
}

//=================================================================================================

void XSControl_WorkSession::ClearData(const int mode)
{
  // 1-2-3-4 : standard IFSelect
  if (mode >= 1 && mode <= 4)
    IFSelect_WorkSession::ClearData(mode);

  // 5 : Transfers only
  // 6 : Forced results only
  // 7 : Management, including all transfers (forced/calculated), views

  if (mode == 5 || mode == 7)
  {
    myTransferReader->Clear(-1);
    myTransferWriter->Clear(-1);
  }
  if (mode == 6 && !myTransferReader.IsNull())
    myTransferReader->Clear(1);
  myTransferReader->SetGraph(HGraph());
}

//=================================================================================================

bool XSControl_WorkSession::SelectNorm(const char* const normname)
{
  const std::lock_guard<std::mutex> aLock(GetGlobalMutex());
  // Old norm and results
  myTransferReader->Clear(-1);
  //  ????  Strictly speaking, cleanup to do in XWS: remove the items
  //        ( at the limit, why not, redo XWS entirely)

  occ::handle<XSControl_Controller> newadapt = XSControl_Controller::Recorded(normname);
  if (newadapt.IsNull())
    return false;
  if (newadapt == myController)
    return true;
  SetController(newadapt);
  return true;
}

//=================================================================================================

void XSControl_WorkSession::SetController(const occ::handle<XSControl_Controller>& ctl)
{
  myController = ctl;

  SetLibrary(myController->WorkLibrary());
  SetProtocol(myController->Protocol());

  ClearItems();
  ClearFinalModifiers();
  ClearShareOut(false);
  ClearFile();

  // Set worksession parameters from the controller
  occ::handle<XSControl_WorkSession> aWorkSession(this);
  myController->Customise(aWorkSession);

  myTransferReader->SetController(myController);
  myTransferWriter->SetController(myController);
}

//=================================================================================================

const char* XSControl_WorkSession::SelectedNorm(const bool rsc) const
{
  // JR/Hp :
  const char* astr = (const char*)(myController.IsNull() ? "" : myController->Name(rsc));
  return astr;
}

//              ##########################################
//              ############  Transfer Context ######
//              ##########################################

//=================================================================================================

void XSControl_WorkSession::SetAllContext(
  const NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>& context)
{
  myContext                   = context;
  myTransferReader->Context() = context;
}

//=================================================================================================

void XSControl_WorkSession::ClearContext()
{
  myContext.Clear();
  myTransferReader->Context().Clear();
}

//              ##########################################
//              ############    RESULTATS FORCES    ######
//              ##########################################

//=================================================================================================

bool XSControl_WorkSession::PrintTransferStatus(const int         num,
                                                const bool        wri,
                                                Standard_OStream& S) const
{
  const occ::handle<Transfer_FinderProcess>& FP = myTransferWriter->FinderProcess();
  occ::handle<Transfer_TransientProcess>     TP = myTransferReader->TransientProcess();

  occ::handle<Transfer_Binder>    binder;
  occ::handle<Transfer_Finder>    finder;
  occ::handle<Standard_Transient> ent;

  //   ***   WRITE  ***
  if (wri)
  {
    if (FP.IsNull())
      return false;
    if (num == 0)
      return false;

    int ne = 0, nr = 0, max = FP->NbMapped(), maxr = FP->NbRoots();
    if (num > 0)
    {
      if (num > max)
        return false;
      ne     = num;
      finder = FP->Mapped(ne);
      nr     = FP->RootIndex(finder);
    }
    else if (num < 0)
    {
      nr = -num;
      if (nr > maxr)
        return false;
      finder = FP->Root(nr);
      ne     = FP->MapIndex(finder);
    }

    S << "Transfer Write item n0." << ne << " of " << max;
    if (nr > 0)
    {
      S << "  ** Transfer Root n0." << ne;
    }
    S << std::endl;
    ent = FP->FindTransient(finder);
    S << " -> Type " << finder->DynamicType()->Name() << std::endl;
    FP->StartTrace(binder, finder, 0, 0); // pb sout/S
    if (!ent.IsNull())
    {
      S << " ** Transient Result, type " << ent->DynamicType()->Name();
      const occ::handle<Interface_InterfaceModel>& model = Model();
      if (!model.IsNull())
      {
        S << " In output Model, Entity ";
        model->Print(ent, S);
      }
      S << std::endl;
    }
  }

  //    ***   READ   ***
  else
  {
    if (TP.IsNull())
      return false;
    occ::handle<Interface_InterfaceModel> model = TP->Model();
    if (model.IsNull())
      std::cout << "No Model" << std::endl;
    else if (model != Model())
      std::cout << "Model different from the session" << std::endl;
    if (num == 0)
      return false;

    int ne = 0, nr = 0, max = TP->NbMapped(), maxr = TP->NbRoots();
    if (num > 0)
    {
      if (num > max)
        return false;
      ne  = num;
      ent = TP->Mapped(ne);
      nr  = TP->RootIndex(finder);
    }
    else if (num < 0)
    {
      nr = -num;
      if (nr > maxr)
        return false;
      ent = TP->Root(nr);
      ne  = TP->MapIndex(ent);
    }

    S << "Transfer Read item n0." << ne << " of " << max;
    if (nr > 0)
    {
      S << "  ** Transfer Root n0." << ne;
    }
    S << std::endl;
    if (!model.IsNull())
    {
      S << " In Model, Entity ";
      model->Print(ent, S);
    }
    binder = TP->MapItem(ne);
    S << std::endl;
    TP->StartTrace(binder, ent, 0, 0);
  }

  //   ***   CHECK (common READ+WRITE)   ***
  if (!binder.IsNull())
  {
    const occ::handle<Interface_Check> ch = binder->Check();
    int                                i, nbw = ch->NbWarnings(), nbf = ch->NbFails();
    if (nbw > 0)
    {
      S << " - Warnings : " << nbw << " :\n";
      for (i = 1; i <= nbw; i++)
        S << ch->CWarning(i) << std::endl;
    }
    if (nbf > 0)
    {
      S << " - Fails : " << nbf << " :\n";
      for (i = 1; i <= nbf; i++)
        S << ch->CFail(i) << std::endl;
    }
  }
  return true;
}

//=================================================================================================

void XSControl_WorkSession::InitTransferReader(const int mode)
{
  if (mode == 0 || mode == 5)
    myTransferReader->Clear(-1); // full clear
  if (myTransferReader.IsNull())
    SetTransferReader(new XSControl_TransferReader);
  else
    SetTransferReader(myTransferReader);

  // mode = 0 done by SetTransferReader following Nullify
  if (mode == 1)
  {
    if (!myTransferReader.IsNull())
      myTransferReader->Clear(-1);
    else
      SetTransferReader(new XSControl_TransferReader);
  }
  if (mode == 2)
  {
    occ::handle<Transfer_TransientProcess> TP = myTransferReader->TransientProcess();
    if (TP.IsNull())
    {
      TP = new Transfer_TransientProcess;
      myTransferReader->SetTransientProcess(TP);
      TP->SetGraph(HGraph());
    }
    occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> lis =
      myTransferReader->RecordedList();
    int i, nb = lis->Length();
    for (i = 1; i <= nb; i++)
      TP->SetRoot(lis->Value(i));
  }
  if (mode == 3)
  {
    occ::handle<Transfer_TransientProcess> TP = myTransferReader->TransientProcess();
    if (TP.IsNull())
      return;
    int i, nb = TP->NbRoots();
    for (i = 1; i <= nb; i++)
      myTransferReader->RecordResult(TP->Root(i));
  }
  if (mode == 4 || mode == 5)
    myTransferReader->BeginTransfer();
}

//=================================================================================================

void XSControl_WorkSession::SetTransferReader(const occ::handle<XSControl_TransferReader>& TR)
{
  if (myTransferReader != TR) // i1 pdn 03.04.99 BUC60301
    myTransferReader = TR;
  if (TR.IsNull())
    return;
  TR->SetController(myController);
  TR->SetGraph(HGraph());
  if (!TR->TransientProcess().IsNull())
    return;
  occ::handle<Transfer_TransientProcess> TP =
    new Transfer_TransientProcess(Model().IsNull() ? 100 : Model()->NbEntities() + 100);
  TP->SetGraph(HGraph());
  TP->SetErrorHandle(true);
  TR->SetTransientProcess(TP);
}

//=================================================================================================

occ::handle<Transfer_TransientProcess> XSControl_WorkSession::MapReader() const
{
  return myTransferReader->TransientProcess();
}

//=================================================================================================

bool XSControl_WorkSession::SetMapReader(const occ::handle<Transfer_TransientProcess>& TP)
{
  if (TP.IsNull())
    return false;
  if (TP->Model().IsNull())
    TP->SetModel(Model());
  TP->SetGraph(HGraph());
  if (TP->Model() != Model())
    return false;
  //  TR must not move, it's a "hook" for signatures, selections ...
  //  On the other hand, better to reset it
  //  occ::handle<XSControl_TransferReader> TR = new XSControl_TransferReader;
  occ::handle<XSControl_TransferReader> TR = myTransferReader;
  TR->Clear(-1);

  SetTransferReader(TR);       // with the same but reinitializes it
  TR->SetTransientProcess(TP); // and takes the new TP
  return true;
}

//=================================================================================================

occ::handle<Standard_Transient> XSControl_WorkSession::Result(
  const occ::handle<Standard_Transient>& ent,
  const int                              mode) const
{
  int ouca = (mode % 10);
  int kica = (mode / 10);

  occ::handle<Transfer_Binder>          binder;
  occ::handle<Transfer_ResultFromModel> resu;

  if (ouca != 1)
    resu = myTransferReader->FinalResult(ent);
  if (mode == 20)
    return resu;

  if (!resu.IsNull())
    binder = resu->MainResult()->Binder();
  if (binder.IsNull() && ouca > 0)
    binder = myTransferReader->TransientProcess()->Find(ent);

  if (kica == 1)
    return binder;
  DeclareAndCast(Transfer_SimpleBinderOfTransient, trb, binder);
  if (!trb.IsNull())
    return trb->Result();
  return binder;
}

//              ##########################################
//              ############    TRANSFERT    #############
//              ##########################################

//=================================================================================================

int XSControl_WorkSession::TransferReadOne(const occ::handle<Standard_Transient>& ent,
                                           const Message_ProgressRange&           theProgress)
{
  occ::handle<Interface_InterfaceModel> model = Model();
  if (ent == model)
    return TransferReadRoots(theProgress);

  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list = GiveList(ent);
  if (list->Length() == 1)
    return myTransferReader->TransferOne(list->Value(1), true, theProgress);
  else
    return myTransferReader->TransferList(list, true, theProgress);
}

//=================================================================================================

int XSControl_WorkSession::TransferReadRoots(const Message_ProgressRange& theProgress)
{
  return myTransferReader->TransferRoots(Graph(), theProgress);
}

//              ##########################################
//              ############    TRANSFERT  WRITE
//              ##########################################

//=================================================================================================

occ::handle<Interface_InterfaceModel> XSControl_WorkSession::NewModel()
{
  const std::lock_guard<std::mutex>     aLock(GetGlobalMutex());
  occ::handle<Interface_InterfaceModel> newmod;
  if (myController.IsNull())
    return newmod;
  newmod = myController->NewModel();

  SetModel(newmod);
  if (!myTransferReader->TransientProcess().IsNull())
    myTransferReader->TransientProcess()->Clear();
  // clear all contains of WS
  myTransferReader->Clear(3);
  myTransferWriter->Clear(-1);

  return newmod;
}

//=================================================================================================

IFSelect_ReturnStatus XSControl_WorkSession::TransferWriteShape(
  const TopoDS_Shape&          shape,
  const bool                   compgraph,
  const Message_ProgressRange& theProgress)
{
  const std::lock_guard<std::mutex> aLock(GetGlobalMutex());
  IFSelect_ReturnStatus             status;
  if (myController.IsNull())
    return IFSelect_RetError;
  const occ::handle<Interface_InterfaceModel>& model = Model();
  if (model.IsNull() || shape.IsNull())
  {
    return IFSelect_RetVoid;
  }

  status = myTransferWriter->TransferWriteShape(model, shape, theProgress);
  if (theProgress.UserBreak())
    return IFSelect_RetStop;
  //  which takes care of everything, try/catch included

  // skl insert param compgraph for XDE writing 10.12.2003
  if (compgraph)
    ComputeGraph(true);

  return status;
}

//=================================================================================================

Interface_CheckIterator XSControl_WorkSession::TransferWriteCheckList() const
{
  return myTransferWriter->ResultCheckList(Model());
}

//=================================================================================================

void XSControl_WorkSession::ClearBinders()
{
  const occ::handle<Transfer_FinderProcess>& FP = myTransferWriter->FinderProcess();
  // Due to big number of chains of binders it is necessary to
  // collect head binders of each chain in the sequence
  NCollection_Sequence<occ::handle<Standard_Transient>> aSeqBnd;
  NCollection_Sequence<occ::handle<Standard_Transient>> aSeqShapes;
  int                                                   i = 1;
  for (; i <= FP->NbMapped(); i++)
  {
    occ::handle<Transfer_Binder> bnd = FP->MapItem(i);
    if (!bnd.IsNull())
      aSeqBnd.Append(bnd);
    occ::handle<Standard_Transient> ash(FP->Mapped(i));
    aSeqShapes.Append(ash);
  }
  // removing finder process containing result of translation.
  FP->Clear();
  ClearData(1);
  ClearData(5);

  // removing each chain of binders
  while (aSeqBnd.Length() > 0)
  {
    occ::handle<Transfer_Binder>    aBnd = occ::down_cast<Transfer_Binder>(aSeqBnd.Value(1));
    occ::handle<Standard_Transient> ash  = aSeqShapes.Value(1);
    aSeqBnd.Remove(1);
    aSeqShapes.Remove(1);
    ash.Nullify();
    while (!aBnd.IsNull())
    {
      occ::handle<Transfer_Binder> aBndNext = aBnd->NextResult();
      aBnd.Nullify();
      aBnd = aBndNext;
    }
  }
}
