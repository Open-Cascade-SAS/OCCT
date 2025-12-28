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

#include <IFSelect_Act.hxx>
#include <IFSelect_CheckCounter.hxx>
#include <IFSelect_Functions.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_Functions.hxx>
#include <XSControl_SelectForTransfer.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

// #######################################################################
// ##									##
// ##									##
// ##				FUNCTIONS				##
// ##									##
// ##									##
// #######################################################################
//=======================================================================
// function : xinit
//=======================================================================
static IFSelect_ReturnStatus XSControl_xinit(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int         argc = pilot->NbWords();
  const char* arg1 = pilot->Arg(1);
  //        ****    xinit        ****
  if (argc > 1)
    return (XSControl::Session(pilot)->SelectNorm(arg1) ? IFSelect_RetDone : IFSelect_RetFail);
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  sout << "Selected Norm:" << XSControl::Session(pilot)->SelectedNorm() << std::endl;
  return IFSelect_RetVoid;
}

//=======================================================================
// function : xnorm
//=======================================================================
static IFSelect_ReturnStatus XSControl_xnorm(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int         argc = pilot->NbWords();
  const char* arg1 = pilot->Arg(1);
  //        ****    xnorm        ****
  occ::handle<XSControl_WorkSession> WS      = XSControl::Session(pilot);
  occ::handle<XSControl_Controller>  control = WS->NormAdaptor();
  Message_Messenger::StreamBuffer    sout    = Message::SendInfo();
  if (argc == 1)
    sout << "Current Norm. xnorm newnorm to change" << std::endl;
  else
    sout << "Current Norm :" << std::endl;
  if (control.IsNull())
    sout << "no norm currently defined" << std::endl;
  else
    sout << "  Long  Name (complete) : " << control->Name(false) << std::endl
         << "  Short name (resource) : " << control->Name(true) << std::endl;
  if (argc == 1)
    return IFSelect_RetVoid;

  control = XSControl_Controller::Recorded(arg1);
  if (control.IsNull())
  {
    sout << " No norm named : " << arg1 << std::endl;
    return IFSelect_RetError;
  }

  WS->SetController(control);
  sout << "new norm : " << control->Name() << std::endl;
  return IFSelect_RetDone;
}

//=======================================================================
// function : newmodel
//=======================================================================
static IFSelect_ReturnStatus XSControl_newmodel(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  //        ****    newmodel        ****
  if (!XSControl::Session(pilot)->NewModel().IsNull())
    return IFSelect_RetDone;
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  sout << "No new Model produced" << std::endl;
  return IFSelect_RetDone;
}

//=======================================================================
// function : tpclear
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpclear(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  //        ****    tpclear/twclear        ****
  const bool                                 modew = (pilot->Word(0).Value(2) == 'w');
  const occ::handle<Transfer_FinderProcess>& FP =
    XSControl::Session(pilot)->TransferWriter()->FinderProcess();
  const occ::handle<Transfer_TransientProcess>& TP =
    XSControl::Session(pilot)->TransferReader()->TransientProcess();
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (modew)
  {
    if (!FP.IsNull())
      FP->Clear();
    else
      sout << "No Transfer Write" << std::endl;
  }
  else
  {
    if (!TP.IsNull())
      TP->Clear();
    else
      sout << "No Transfer Read" << std::endl;
  }
  return IFSelect_RetDone;
}

//=======================================================================
// function : tpstat
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpstat(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int         argc = pilot->NbWords();
  const char* arg1 = pilot->Arg(1);
  // const char* arg2 = pilot->Arg(2);
  const occ::handle<Transfer_TransientProcess>& TP =
    XSControl::Session(pilot)->TransferReader()->TransientProcess();
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (TP.IsNull())
  {
    sout << "No Transfer Read" << std::endl;
    return IFSelect_RetError;
  }
  //        ****    tpstat        ****

  int mod1 = -1;
  int mod2 = 0;
  //  g : general  c : check (compte) C (liste)   f : fails(compte)  F (liste)
  //  resultats racines :  n : n0s entites  s : status  b : binders
  //    t : count by type  r : count by result   l : list(type-result)
  //   *n *s *b *t *r *l : same on everything
  //   ?n  etc.. : same on abnormal results
  //   ?  short for help

  if (argc > 1)
  {
    char a2 = arg1[1];
    if (a2 == '\0')
      a2 = '!';
    switch (arg1[0])
    {
      case 'g':
        mod1 = 0;
        break;
      case 'c':
        mod1 = 4;
        mod2 = 4;
        break;
      case 'C':
        mod1 = 4;
        mod2 = 2;
        break;
      case 'f':
        mod1 = 5;
        mod2 = 4;
        break;
      case 'F':
        mod1 = 5;
        mod2 = 2;
        break;
      case '*':
        mod1 = 2;
        break;
      case '?':
        mod1 = 3;
        break;
      default:
        mod1 = 1;
        if (argc > 2)
          mod1 = 2;
        a2 = arg1[0];
        break;
    }
    if (mod1 < 1 || mod1 > 3)
      a2 = '!';
    switch (a2)
    {
      case 'n':
        mod2 = 0;
        break;
      case 's':
        mod2 = 1;
        break;
      case 'b':
        mod2 = 2;
        break;
      case 't':
        mod2 = 3;
        break;
      case 'r':
        mod2 = 4;
        break;
      case 'l':
        mod2 = 5;
        break;
      case 'L':
        mod2 = 6;
        break;
      case '!':
        break;
      case '?':
        mod1 = -1;
        break;
      default:
        mod1 = -2;
        break;
    }
  }
  //  A present help eventuel
  if (mod1 < -1)
    sout << "Unknown Mode" << std::endl;
  if (mod1 < 0)
  {
    sout << "Modes available :\n"
         << "g : general    c : checks (count)  C (list)\n"
         << "               f : fails  (count)  F (list)\n"
         << "  n : numbers of transferred entities (on TRANSFER ROOTS)\n"
         << "  s : their status (type entity-result , presence checks)\n"
         << "  b : detail of binders\n"
         << "  t : count per entity type    r : per type/status result\n"
         << "  l : count per couple  type entity/result\n"
         << "  L : list  per couple  type entity/result\n"
         << "  *n  *s  *b  *t  *r  *l  *L : idem on ALL recorded items\n"
         << "  ?n  ?s  ?b  ?t ... : idem on abnormal items\n"
         << "  n select : n applied on a selection   idem for  s b t r l" << std::endl;
    if (mod1 < -1)
      return IFSelect_RetError;
    return IFSelect_RetVoid;
  }

  if (!TP.IsNull())
  {
    sout << "TransferRead :";
    if (TP->Model() != pilot->Session()->Model())
      sout << "Model differs from the session";
    occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list =
      IFSelect_Functions::GiveList(pilot->Session(), pilot->CommandPart(2));
    XSControl_TransferReader::PrintStatsOnList(TP, list, mod1, mod2);
    //    TP->PrintStats (1,sout);
  }
  else
    sout << "TransferRead : not defined" << std::endl;
  return IFSelect_RetVoid;
}

//=======================================================================
// function : tpent
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpent(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int                                           argc = pilot->NbWords();
  const char*                                   arg1 = pilot->Arg(1);
  const occ::handle<Transfer_TransientProcess>& TP =
    XSControl::Session(pilot)->TransferReader()->TransientProcess();
  //        ****    tpent        ****
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (TP.IsNull())
  {
    sout << "No Transfer Read" << std::endl;
    return IFSelect_RetError;
  }
  occ::handle<Interface_InterfaceModel> model = TP->Model();
  if (model.IsNull())
    return IFSelect_RetFail;

  if (argc < 2)
  {
    sout << "Give ENTITY NUMBER (IN MODEL TransferProcess)" << std::endl;
    return IFSelect_RetError;
  }
  int num = atoi(arg1);
  if (num <= 0 || num > model->NbEntities())
  {
    sout << "Number not in [1 - " << model->NbEntities() << "]" << std::endl;
    return IFSelect_RetError;
  }
  occ::handle<Standard_Transient> ent   = model->Value(num);
  int                             index = TP->MapIndex(ent);
  if (index == 0)
    sout << "Entity " << num << "  not recorded in transfer" << std::endl;
  else
    XSControl::Session(pilot)->PrintTransferStatus(index, false, sout);
  return IFSelect_RetVoid;
}

//=======================================================================
// function : tpitem
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpitem(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int         argc = pilot->NbWords();
  const char* arg1 = pilot->Arg(1);
  //        ****    tpitem/tproot/twitem/twroot        ****
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (argc < 2)
  {
    sout << "Give ITEM NUMBER (in TransferProcess)" << std::endl;
    return IFSelect_RetError;
  }
  int num = atoi(arg1);
  if (pilot->Word(0).Value(3) == 'r')
    num = -num;
  bool modew = false;
  if (pilot->Word(0).Value(2) == 'w')
    modew = true;
  occ::handle<Transfer_Binder>    binder;
  occ::handle<Transfer_Finder>    finder;
  occ::handle<Standard_Transient> ent;
  if (!XSControl::Session(pilot)->PrintTransferStatus(num, modew, sout))
  {
    sout << " - Num=" << num << " incorrect" << std::endl;
  }
  return IFSelect_RetVoid;
}

//=======================================================================
// function : trecord
//=======================================================================
static IFSelect_ReturnStatus XSControl_trecord(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int                                           argc = pilot->NbWords();
  const char*                                   arg1 = pilot->Arg(1);
  const occ::handle<Transfer_TransientProcess>& TP =
    XSControl::Session(pilot)->TransferReader()->TransientProcess();
  //        ****    trecord : TransferReader        ****
  bool                                         all = (argc == 1);
  int                                          num = -1;
  const occ::handle<Interface_InterfaceModel>& mdl = XSControl::Session(pilot)->Model();
  const occ::handle<XSControl_TransferReader>& TR  = XSControl::Session(pilot)->TransferReader();
  occ::handle<Standard_Transient>              ent;
  Message_Messenger::StreamBuffer              sout = Message::SendInfo();
  if (mdl.IsNull() || TR.IsNull() || TP.IsNull())
  {
    sout << " init not done" << std::endl;
    return IFSelect_RetError;
  }
  if (!all)
    num = atoi(arg1);
  //    Record the roots
  if (all)
  {
    int nb = TP->NbRoots();
    sout << " Recording " << nb << " Roots" << std::endl;
    for (int i = 1; i <= nb; i++)
    {
      ent = TP->Root(i);
      if (TR->RecordResult(ent))
        sout << " Root n0." << i << std::endl;
      else
        sout << " Root n0." << i << " not recorded" << std::endl;
    }
  }
  else
  {
    if (num < 1 || num > mdl->NbEntities())
      sout << "incorrect number:" << num << std::endl;
    else if (TR->RecordResult(mdl->Value(num)))
      sout << " Entity n0." << num << std::endl;
    else
      sout << " Entity n0." << num << " not recorded" << std::endl;
  }
  return IFSelect_RetDone;
}

//=======================================================================
// function : trstat
//=======================================================================
static IFSelect_ReturnStatus XSControl_trstat(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int                             argc = pilot->NbWords();
  const char*                     arg1 = pilot->Arg(1);
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  //        ****    trstat : TransferReader        ****
  const occ::handle<XSControl_TransferReader>& TR = XSControl::Session(pilot)->TransferReader();
  if (TR.IsNull())
  {
    sout << " init not done" << std::endl;
    return IFSelect_RetError;
  }
  occ::handle<Interface_InterfaceModel> mdl = TR->Model();
  if (mdl.IsNull())
  {
    sout << " No model" << std::endl;
    return IFSelect_RetError;
  }
  sout << " Statistics : FileName : " << TR->FileName() << std::endl;
  if (argc == 1)
  {
    // stats generales
    TR->PrintStats(sout, 10, 0);
  }
  else
  {
    // stats unitaires
    int num = atoi(arg1);
    if (num < 1 || num > mdl->NbEntities())
    {
      sout << " incorrect number:" << arg1 << std::endl;
      return IFSelect_RetError;
    }
    occ::handle<Standard_Transient> ent = mdl->Value(num);
    if (!TR->IsRecorded(ent))
    {
      sout << " Entity " << num << " not recorded" << std::endl;
      return IFSelect_RetError;
    }
    occ::handle<Transfer_ResultFromModel>                               RM   = TR->FinalResult(ent);
    occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list = TR->CheckedList(ent);
    int                                                                 i, nb = list->Length();
    if (nb > 0)
      sout << " Entities implied by Check/Result :" << nb << " i.e.:";
    for (i = 1; i <= nb; i++)
    {
      sout << "  ";
      mdl->Print(list->Value(i), sout);
    }
    sout << std::endl;
    if (RM.IsNull())
    {
      sout << " no other info" << std::endl;
      return IFSelect_RetVoid;
    }
    Interface_CheckIterator chl = RM->CheckList(false);
    pilot->Session()->PrintCheckList(sout, chl, false, IFSelect_EntitiesByItem);
  }
  return IFSelect_RetVoid;
}

//=======================================================================
// function : trbegin
//=======================================================================
static IFSelect_ReturnStatus XSControl_trbegin(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  //        ****    trbegin : TransferReader        ****
  occ::handle<XSControl_TransferReader> TR   = XSControl::Session(pilot)->TransferReader();
  bool                                  init = TR.IsNull();
  if (pilot->NbWords() > 1)
  {
    if (pilot->Arg(1)[0] == 'i')
      init = true;
  }
  if (init)
  {
    XSControl::Session(pilot)->InitTransferReader(0);
    TR = XSControl::Session(pilot)->TransferReader();
    if (TR.IsNull())
    {
      Message_Messenger::StreamBuffer sout = Message::SendInfo();
      sout << " init not done or failed" << std::endl;
      return IFSelect_RetError;
    }
  }
  TR->BeginTransfer();
  return IFSelect_RetDone;
}

//=======================================================================
// function : tread
//=======================================================================
static IFSelect_ReturnStatus XSControl_tread(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int argc = pilot->NbWords();
  // const char* arg1 = pilot->Arg(1);
  //         ****    tread : TransferReader        ****
  Message_Messenger::StreamBuffer              sout = Message::SendInfo();
  const occ::handle<XSControl_TransferReader>& TR   = XSControl::Session(pilot)->TransferReader();
  if (TR.IsNull())
  {
    sout << " init not done" << std::endl;
    return IFSelect_RetError;
  }
  const occ::handle<Interface_InterfaceModel>& mdl = TR->Model();
  if (mdl.IsNull())
  {
    sout << " No model" << std::endl;
    return IFSelect_RetError;
  }
  if (argc < 2)
  {
    //      DeclareAndCast(IFSelect_Selection,sel,pilot->Session()->NamedItem("xst-model-roots"));
    occ::handle<Standard_Transient> sel = pilot->Session()->NamedItem("xst-model-roots");
    if (sel.IsNull())
    {
      sout << "Select Roots absent" << std::endl;
      return IFSelect_RetError;
    }
    occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list =
      pilot->Session()->GiveList(sel);
    sout << " Transferring all roots i.e. : " << TR->TransferList(list) << std::endl;
  }
  else
  {
    occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list =
      IFSelect_Functions::GiveList(pilot->Session(), pilot->CommandPart(1));
    sout << " Transfer of " << list->Length() << " entities" << std::endl;
    int nb = TR->TransferList(list);
    sout << " Gives " << nb << " results" << std::endl;
  }
  return IFSelect_RetDone;
}

//=======================================================================
// function : trtp
//=======================================================================
static IFSelect_ReturnStatus XSControl_trtp(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  //        ****    TReader -> TProcess         ****
  const occ::handle<XSControl_TransferReader>& TR   = XSControl::Session(pilot)->TransferReader();
  Message_Messenger::StreamBuffer              sout = Message::SendInfo();
  if (TR.IsNull())
    sout << " No TransferReader" << std::endl;
  else if (TR->TransientProcess().IsNull())
    sout << " Transfer Reader without Process" << std::endl;
  return IFSelect_RetVoid;
}

//=======================================================================
// function : tptr
//=======================================================================
static IFSelect_ReturnStatus XSControl_tptr(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  //        ****    TProcess -> TReader         ****
  XSControl::Session(pilot)->InitTransferReader(3);
  return IFSelect_RetDone;
}

//=======================================================================
// function : twmode
//=======================================================================
static IFSelect_ReturnStatus XSControl_twmode(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int         argc = pilot->NbWords();
  const char* arg1 = pilot->Arg(1);
  //        ****    twmode         ****
  occ::handle<XSControl_TransferWriter> TW      = XSControl::Session(pilot)->TransferWriter();
  occ::handle<XSControl_Controller>     control = XSControl::Session(pilot)->NormAdaptor();
  int                                   modemin, modemax;
  Message_Messenger::StreamBuffer       sout = Message::SendInfo();
  if (control->ModeWriteBounds(modemin, modemax))
  {
    sout << "Write Mode : allowed values  " << modemin << " to " << modemax << std::endl;
    for (int modd = modemin; modd <= modemax; modd++)
    {
      sout << modd << "	: " << control->ModeWriteHelp(modd) << std::endl;
    }
  }
  sout << "Write Mode : actual = " << TW->TransferMode() << std::endl;
  if (argc <= 1)
    return IFSelect_RetVoid;
  int mod = atoi(arg1);
  sout << "New value -> " << arg1 << std::endl;
  TW->SetTransferMode(mod);
  if (!control->IsModeWrite(mod))
    sout << "Warning : this new value is not supported" << std::endl;
  return IFSelect_RetDone;
}

//=======================================================================
// function : twstat
//=======================================================================
static IFSelect_ReturnStatus XSControl_twstat(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  // int argc = pilot->NbWords();
  // const char* arg1 = pilot->Arg(1);
  // const char* arg2 = pilot->Arg(2);
  const occ::handle<Transfer_FinderProcess>& FP =
    XSControl::Session(pilot)->TransferWriter()->FinderProcess();
  //        ****    twstat        ****
  //  For Write
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (!FP.IsNull())
  {
    sout << "TransferWrite:";
    //    XSControl_TransferWriter::PrintStatsProcess (FP,mod1,mod2);
    FP->PrintStats(1, sout);
  }
  else
    sout << "TransferWrite: not defined" << std::endl;
  return IFSelect_RetVoid;
}

//=======================================================================
// function : settransfert
//=======================================================================
static IFSelect_ReturnStatus XSControl_settransfert(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  //        ****    SelectForTransfer           ****
  return pilot->RecordItem(
    new XSControl_SelectForTransfer(XSControl::Session(pilot)->TransferReader()));
}

static int THE_XSControl_Functions_initactor = 0;

//=================================================================================================

void XSControl_Functions::Init()
{
  if (THE_XSControl_Functions_initactor)
  {
    return;
  }

  THE_XSControl_Functions_initactor = 1;
  IFSelect_Act::SetGroup("DE: General");

  IFSelect_Act::AddFunc("xinit",
                        "[norm:string to change norme] reinitialises according to the norm",
                        XSControl_xinit);
  IFSelect_Act::AddFunc("xnorm", "displays current norm   +norm : changes it", XSControl_xnorm);

  IFSelect_Act::AddFunc("newmodel",
                        "produces a new empty model, for the session",
                        XSControl_newmodel);

  IFSelect_Act::AddFunc("tpclear", "Clears  TransferProcess (READ)", XSControl_tpclear);
  IFSelect_Act::AddFunc("twclear", "Clears  TransferProcess (WRITE)", XSControl_tpclear);

  IFSelect_Act::AddFunc("tpstat", "Statistics on TransferProcess (READ)", XSControl_tpstat);

  IFSelect_Act::AddFunc("tpent",
                        "[num:integer] Statistics on an entity of the model (READ)",
                        XSControl_tpent);

  IFSelect_Act::AddFunc("tpitem",
                        "[num:integer] Statistics on ITEM of transfer (READ)",
                        XSControl_tpitem);
  IFSelect_Act::AddFunc("tproot",
                        "[num:integer] Statistics on a ROOT of transfert (READ)",
                        XSControl_tpitem);
  IFSelect_Act::AddFunc("twitem",
                        "[num:integer] Statistics on an ITEM of transfer (WRITE)",
                        XSControl_tpitem);
  IFSelect_Act::AddFunc("twroot",
                        "[num:integer] Statistics on a ROOT of transfer (WRITE)",
                        XSControl_tpitem);

  IFSelect_Act::AddFunc("trecord",
                        "record : all root results; or num : for entity n0.num",
                        XSControl_trecord);
  IFSelect_Act::AddFunc("trstat",
                        "general statistics;  or num : stats on entity n0 num",
                        XSControl_trstat);
  IFSelect_Act::AddFunc("trbegin", "begin-transfer-reader [init]", XSControl_trbegin);
  IFSelect_Act::AddFunc("tread",
                        "transfers all roots, or num|sel|sel num : entity list, by transfer-reader",
                        XSControl_tread);

  IFSelect_Act::AddFunc("trtp", "feeds commands tp... with results from tr...", XSControl_trtp);
  IFSelect_Act::AddFunc("tptr", "feeds tr... from tp... (may be incomplete)", XSControl_tptr);

  IFSelect_Act::AddFunc("twmode",
                        "displays mode transfer write, + num  changes it",
                        XSControl_twmode);
  IFSelect_Act::AddFunc("twstat", "Statistics on TransferProcess (WRITE)", XSControl_twstat);

  IFSelect_Act::AddFSet("selecttransfer",
                        "selection (recognize from transfer actor)",
                        XSControl_settransfert);
}
