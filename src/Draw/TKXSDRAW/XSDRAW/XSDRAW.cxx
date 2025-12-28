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

#include <DBRep.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Printer.hxx>
#include <Draw_PluginMacro.hxx>
#include <IFSelect_Functions.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterOStream.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <XSAlgo.hxx>
#include <XSControl.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_FuncShape.hxx>
#include <XSControl_Functions.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_TransferWriter.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>
#include <XSDRAW_Vars.hxx>
#include <UnitsMethods.hxx>
#include <Interface_Static.hxx>
#include <XCAFDoc_DocumentTool.hxx>

#include <iostream>
#include <string>

namespace
{
static int deja = 0, dejald = 0;

static NCollection_DataMap<TCollection_AsciiString, int>           theolds;
static occ::handle<NCollection_HSequence<TCollection_AsciiString>> thenews;

static occ::handle<IFSelect_SessionPilot> thepilot; // detient Session, Model

//=================================================================================================

static void collectActiveWorkSessions(const occ::handle<XSControl_WorkSession>& theWS,
                                      const TCollection_AsciiString&            theName,
                                      XSControl_WorkSessionMap&                 theMap,
                                      const bool                                theIsFirst)
{
  if (theIsFirst)
  {
    theMap.Clear();
  }
  if (theWS.IsNull())
  {
    return;
  }
  if (theMap.IsBound(theName))
  {
    return;
  }
  theMap.Bind(theName, theWS);
  for (XSControl_WorkSessionMap::Iterator anIter(theWS->Context()); anIter.More(); anIter.Next())
  {
    occ::handle<XSControl_WorkSession> aWS = occ::down_cast<XSControl_WorkSession>(anIter.Value());
    collectActiveWorkSessions(aWS, anIter.Key(), theMap, false);
  }
}
} // namespace

static int XSTEPDRAWRUN(Draw_Interpretor& di, int argc, const char** argv)
{
  TCollection_AsciiString mess;
  for (int i = 0; i < argc; i++)
  {
    mess.AssignCat(argv[i]);
    mess.AssignCat(" ");
  }

  const occ::handle<Message_Messenger>&              aMsgMgr = Message::DefaultMessenger();
  NCollection_Sequence<occ::handle<Message_Printer>> aPrinters;
  aPrinters.Append(aMsgMgr->ChangePrinters());
  aMsgMgr->AddPrinter(new Draw_Printer(di));

  IFSelect_ReturnStatus stat = thepilot->Execute(mess.ToCString());

  aMsgMgr->RemovePrinters(STANDARD_TYPE(Draw_Printer));
  aMsgMgr->ChangePrinters().Append(aPrinters);

  if (stat == IFSelect_RetError || stat == IFSelect_RetFail)
    return 1;
  else
    return 0;
}

void XSDRAW::ChangeCommand(const char* oldname, const char* newname)
{
  int num = 0;
  if (newname[0] != '\0')
  {
    if (thenews.IsNull())
      thenews = new NCollection_HSequence<TCollection_AsciiString>();
    TCollection_AsciiString newstr(newname);
    thenews->Append(newstr);
    num = thenews->Length();
  }
  theolds.Bind(oldname, num);
}

void XSDRAW::RemoveCommand(const char* oldname)
{
  ChangeCommand(oldname, "");
}

bool XSDRAW::LoadSession()
{
  if (deja)
    return false;
  deja                                  = 1;
  thepilot                              = new IFSelect_SessionPilot("XSTEP-DRAW>");
  occ::handle<XSControl_WorkSession> WS = new XSControl_WorkSession;
  WS->SetVars(new XSDRAW_Vars);
  thepilot->SetSession(WS);

  IFSelect_Functions::Init();
  XSControl_Functions::Init();
  XSControl_FuncShape::Init();
  XSAlgo::Init();
  //  XSDRAW_Shape::Init();   passe a present par theCommands
  return true;
}

void XSDRAW::LoadDraw(Draw_Interpretor& theCommands)
{
  if (dejald)
  {
    return;
  }
  dejald = 1;
  //  Pour tout faire d un coup : BRepTest & cie:
  LoadSession();

  // skl: we make remove commands "x" and "exit" in order to this commands are
  //      performed not in IFSelect_SessionPilot but in standard Tcl interpreter
  XSDRAW::RemoveCommand("x");
  XSDRAW::RemoveCommand("exit");

  //  if (!getenv("WBHOSTTOP")) XSDRAW::RemoveCommand("xsnew");
  occ::handle<NCollection_HSequence<TCollection_AsciiString>> list =
    IFSelect_Activator::Commands(0);
  for (NCollection_HSequence<TCollection_AsciiString>::Iterator aCmdIter(*list); aCmdIter.More();
       aCmdIter.Next())
  {
    int                            num  = -1;
    const TCollection_AsciiString& aCmd = aCmdIter.Value();
    if (!theolds.IsEmpty())
    {
      theolds.Find(aCmd, num);
    }
    if (num == 0)
    {
      continue;
    }

    int                             nact = 0;
    occ::handle<IFSelect_Activator> anAct;
    TCollection_AsciiString         aHelp;
    if (!IFSelect_Activator::Select(aCmd.ToCString(), nact, anAct))
    {
      aHelp = TCollection_AsciiString("type :  xhelp ") + aCmd + " for help";
    }
    else if (!anAct.IsNull())
    {
      aHelp = anAct->Help(nact);
    }

    const TCollection_AsciiString& aCmdName = num < 0 ? aCmd : thenews->Value(num);
    theCommands.Add(aCmdName.ToCString(), aHelp.ToCString(), "", XSTEPDRAWRUN, anAct->Group());
  }
}

int XSDRAW::Execute(const char* command, const char* varname)
{
  char mess[100];
  Sprintf(mess, command, varname);
  thepilot->Execute(mess);
  return 1; // stat ?
}

occ::handle<IFSelect_SessionPilot> XSDRAW::Pilot()
{
  return thepilot;
}

void XSDRAW::SetSession(const occ::handle<XSControl_WorkSession>& theSession)
{
  Pilot()->SetSession(theSession);
}

const occ::handle<XSControl_WorkSession> XSDRAW::Session()
{
  return XSControl::Session(thepilot);
}

void XSDRAW::SetController(const occ::handle<XSControl_Controller>& control)
{
  if (thepilot.IsNull())
    XSDRAW::LoadSession();
  if (control.IsNull())
    std::cout << "XSTEP Controller not defined" << std::endl;
  else if (!Session().IsNull())
    Session()->SetController(control);
  else
    std::cout << "XSTEP Session badly or not defined" << std::endl;
}

occ::handle<XSControl_Controller> XSDRAW::Controller()
{
  return Session()->NormAdaptor();
}

bool XSDRAW::SetNorm(const char* norm)
{
  return Session()->SelectNorm(norm);
}

occ::handle<Interface_Protocol> XSDRAW::Protocol()
{
  return thepilot->Session()->Protocol();
}

occ::handle<Interface_InterfaceModel> XSDRAW::Model()
{
  return thepilot->Session()->Model();
}

void XSDRAW::SetModel(const occ::handle<Interface_InterfaceModel>& model, const char* file)
{
  thepilot->Session()->SetModel(model);
  if (file && file[0] != '\0')
    thepilot->Session()->SetLoadedFile(file);
}

occ::handle<Interface_InterfaceModel> XSDRAW::NewModel()
{
  return Session()->NewModel();
}

occ::handle<Standard_Transient> XSDRAW::Entity(const int num)
{
  return thepilot->Session()->StartingEntity(num);
}

int XSDRAW::Number(const occ::handle<Standard_Transient>& ent)
{
  return thepilot->Session()->StartingNumber(ent);
}

void XSDRAW::SetTransferProcess(const occ::handle<Standard_Transient>& ATP)
{
  DeclareAndCast(Transfer_FinderProcess, FP, ATP);
  DeclareAndCast(Transfer_TransientProcess, TP, ATP);

  //   Cas FinderProcess    ==> TransferWriter
  if (!FP.IsNull())
    Session()->SetMapWriter(FP);

  //   Cas TransientProcess ==> TransferReader
  if (!TP.IsNull())
  {
    if (!TP->Model().IsNull() && TP->Model() != Session()->Model())
      Session()->SetModel(TP->Model());
    Session()->SetMapReader(TP);
  }
}

occ::handle<Transfer_TransientProcess> XSDRAW::TransientProcess()
{
  return Session()->TransferReader()->TransientProcess();
}

occ::handle<Transfer_FinderProcess> XSDRAW::FinderProcess()
{
  return Session()->TransferWriter()->FinderProcess();
}

void XSDRAW::InitTransferReader(const int mode)
{
  //   0 nullify  1 clear
  //   2 init TR avec contenu TP (roots)  3 init TP avec contenu TR
  //   4 init avec model (debut scratch)
  Session()->InitTransferReader(mode);
}

occ::handle<XSControl_TransferReader> XSDRAW::TransferReader()
{
  return Session()->TransferReader();
}

//  ############  AUXILIAIRES  #############

occ::handle<Standard_Transient> XSDRAW::GetEntity(const char* name)
{
  return IFSelect_Functions::GiveEntity(Session(), name);
}

int XSDRAW::GetEntityNumber(const char* name)
{
  return IFSelect_Functions::GiveEntityNumber(Session(), name);
}

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> XSDRAW::GetList(
  const char* first,
  const char* second)
{
  if (!first || first[0] == '\0')
  {
    std::string aLineFirst;
    std::cin >> aLineFirst;

    char terminateSymbol = '\0';
    std::cin.get(terminateSymbol);

    if (terminateSymbol == '\n')
      return XSDRAW::GetList(aLineFirst.c_str(), nullptr);
    else
    {
      std::string aLineSecond;
      std::cin >> aLineSecond;
      return XSDRAW::GetList(aLineFirst.c_str(), aLineSecond.c_str());
    }
  }
  return IFSelect_Functions::GiveList(Session(), first, second);
}

bool XSDRAW::FileAndVar(const char*              file,
                        const char*              var,
                        const char*              def,
                        TCollection_AsciiString& resfile,
                        TCollection_AsciiString& resvar)
{
  return XSControl_FuncShape::FileAndVar(XSDRAW::Session(), file, var, def, resfile, resvar);
}

int XSDRAW::MoreShapes(occ::handle<NCollection_HSequence<TopoDS_Shape>>& list, const char* name)
{
  return XSControl_FuncShape::MoreShapes(XSDRAW::Session(), list, name);
}

//=================================================================================================

double XSDRAW::GetLengthUnit(const occ::handle<TDocStd_Document>& theDoc)
{
  if (!theDoc.IsNull())
  {
    double aUnit = 1.;
    if (XCAFDoc_DocumentTool::GetLengthUnit(theDoc, aUnit, UnitsMethods_LengthUnit_Millimeter))
    {
      return aUnit;
    }
  }
  if (Interface_Static::IsPresent("xstep.cascade.unit"))
  {
    UnitsMethods::SetCasCadeLengthUnit(Interface_Static::IVal("xstep.cascade.unit"));
  }
  return UnitsMethods::GetCasCadeLengthUnit();
}

//=================================================================================================

XSControl_WorkSessionMap& XSDRAW::WorkSessionList()
{
  static std::shared_ptr<XSControl_WorkSessionMap> THE_PREVIOUS_WORK_SESSIONS;
  if (THE_PREVIOUS_WORK_SESSIONS == nullptr)
  {
    THE_PREVIOUS_WORK_SESSIONS = std::make_shared<XSControl_WorkSessionMap>();
  }
  return *THE_PREVIOUS_WORK_SESSIONS;
}

//=================================================================================================

void XSDRAW::CollectActiveWorkSessions(const occ::handle<XSControl_WorkSession>& theWS,
                                       const TCollection_AsciiString&            theName,
                                       XSControl_WorkSessionMap&                 theMap)
{
  collectActiveWorkSessions(theWS, theName, theMap, true);
}

//=================================================================================================

void XSDRAW::CollectActiveWorkSessions(const TCollection_AsciiString& theName)
{
  collectActiveWorkSessions(Session(), theName, WorkSessionList(), true);
}

//=================================================================================================

void XSDRAW::Factory(Draw_Interpretor& theDI)
{
  XSDRAW::LoadDraw(theDI);
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAW)
