// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <Draw.hxx>
#include <Draw_Printer.hxx>

#include <Message_PrinterOStream.hxx>
#include <Message_PrinterSystemLog.hxx>
#include <Message_PrinterToReport.hxx>
#include <Message_Report.hxx>
#include <NCollection_Shared.hxx>

//=================================================================================================

static bool printerType(const TCollection_AsciiString& theTypeName,
                        occ::handle<Standard_Type>&    theType)
{
  if (theTypeName == "ostream")
  {
    theType = STANDARD_TYPE(Message_PrinterOStream);
    return true;
  }
  else if (theTypeName == "systemlog")
  {
    theType = STANDARD_TYPE(Message_PrinterSystemLog);
    return true;
  }
  else if (theTypeName == "report")
  {
    theType = STANDARD_TYPE(Message_PrinterToReport);
    return true;
  }
  else if (theTypeName == "draw")
  {
    theType = STANDARD_TYPE(Draw_Printer);
    return true;
  }

  return false;
}

//=================================================================================================

static occ::handle<Message_Printer> createPrinter(const occ::handle<Standard_Type>& theType,
                                                  Draw_Interpretor&                 theDI)
{
  const TCollection_AsciiString aTypeName(theType->Name());
  if (aTypeName == STANDARD_TYPE(Message_PrinterOStream)->Name())
  {
    return new Message_PrinterOStream();
  }
  else if (aTypeName == STANDARD_TYPE(Message_PrinterSystemLog)->Name())
  {
    return new Message_PrinterSystemLog("draw_messages");
  }
  else if (aTypeName == STANDARD_TYPE(Message_PrinterToReport)->Name())
  {
    occ::handle<Message_PrinterToReport> aMessagePrinter = new Message_PrinterToReport();
    const occ::handle<Message_Report>&   aReport         = Message::DefaultReport(true);
    aMessagePrinter->SetReport(aReport);
    return aMessagePrinter;
  }
  else if (aTypeName == STANDARD_TYPE(Draw_Printer)->Name())
  {
    return new Draw_Printer(theDI);
  }
  return occ::handle<Message_Printer>();
}

//=================================================================================================

static int SendMessage(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb < 2)
  {
    theDI << "Error: wrong number of arguments";
    return 1;
  }

  const occ::handle<Message_Messenger>& aMessenger = Message::DefaultMessenger();
  for (int anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    aMessenger->Send(anArg);
  }

  return 0;
}

//=================================================================================================

static int PrintMessenger(Draw_Interpretor& theDI, int, const char**)
{
  const occ::handle<Message_Messenger>& aMessenger = Message::DefaultMessenger();

  Standard_SStream aSStream;
  aMessenger->DumpJson(aSStream);
  theDI << aSStream;
  std::cout << aSStream.str() << std::endl;

  return 0;
}

//=================================================================================================

static int SetMessagePrinter(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb < 2)
  {
    theDI << "Error: wrong number of arguments";
    return 1;
  }

  bool                                      toAddPrinter = true;
  NCollection_List<TCollection_AsciiString> aPrinterTypes;
  const occ::handle<Message_Messenger>&     aMessenger = Message::DefaultMessenger();
  for (int anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-state")
    {
      if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toAddPrinter))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-type" && anArgIter + 1 < theArgNb)
    {
      TCollection_AsciiString aVal(theArgVec[++anArgIter]);
      aPrinterTypes.Append(aVal);
    }
    else
    {
      theDI << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  for (NCollection_List<TCollection_AsciiString>::Iterator anIterator(aPrinterTypes);
       anIterator.More();
       anIterator.Next())
  {
    occ::handle<Standard_Type> aPrinterType;
    if (!printerType(anIterator.Value(), aPrinterType))
    {
      theDI << "Syntax error: unknown printer type '" << anIterator.Value() << "'";
      return 1;
    }

    if (toAddPrinter)
    {
      occ::handle<Message_Printer> aPrinter = createPrinter(aPrinterType, theDI);
      aMessenger->AddPrinter(aPrinter);
      if (!occ::down_cast<Message_PrinterToReport>(aPrinter).IsNull())
      {
        Message::DefaultReport(false)->UpdateActiveInMessenger();
      }
    }
    else
    {
      aMessenger->RemovePrinters(aPrinterType);
    }
  }
  return 0;
}

//=================================================================================================

static int ClearReport(Draw_Interpretor& theDI, int theArgNb, const char**)
{
  if (theArgNb < 1)
  {
    theDI << "Error: wrong number of arguments";
    return 1;
  }

  const occ::handle<Message_Report>& aReport = Message::DefaultReport(false);
  if (aReport.IsNull())
  {
    theDI << "Error: report is no created";
    return 1;
  }

  aReport->Clear();
  return 0;
}

//=================================================================================================

static int SetReportMetric(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb < 1)
  {
    theDI << "Error: wrong number of arguments";
    return 1;
  }

  const occ::handle<Message_Report>& aReport = Message::DefaultReport(true);
  if (aReport.IsNull())
  {
    return 1;
  }

  aReport->ClearMetrics();
  for (int i = 1; i < theArgNb; i++)
  {
    int aMetricId = Draw::Atoi(theArgVec[i]);
    if (aMetricId < Message_MetricType_ThreadCPUUserTime
        || aMetricId > Message_MetricType_MemHeapUsage)
    {
      theDI << "Error: unrecognized message metric: " << aMetricId;
      return 1;
    }
    aReport->SetActiveMetric((Message_MetricType)aMetricId, true);
  }
  return 0;
}

//=================================================================================================

static int CollectMetricMessages(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  static Handle(NCollection_Shared<Message_Level>) MyLevel;

  if (theArgNb < 1)
  {
    theDI << "Error: wrong number of arguments";
    return 1;
  }

  bool toActivate = false;
  for (int anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-activate")
    {
      if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toActivate))
      {
        ++anArgIter;
      }
    }
  }
  if (toActivate)
  {
    if (!MyLevel.IsNull())
    {
      theDI << "Error: collecting already activated";
      return 1;
    }
    MyLevel = new NCollection_Shared<Message_Level>("Level");
  }
  else
  {
    if (!MyLevel)
    {
      theDI << "Error: collecting was not activated";
      return 1;
    }
    MyLevel.Nullify();
    MyLevel = 0;
  }
  return 0;
}

//=================================================================================================

static int PrintReport(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb < 1)
  {
    theDI << "Error: wrong number of arguments";
    return 1;
  }

  const occ::handle<Message_Report>& aReport = Message::DefaultReport(false);
  if (aReport.IsNull())
  {
    theDI << "Error: report is no created";
    return 1;
  }

  for (int anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anArgCase == "-messenger")
    {
      aReport->SendMessages(Message::DefaultMessenger());
    }
    else if (anArgCase == "-dump" || anArgCase == "-print")
    {
      Standard_SStream aSStream;
      aReport->Dump(aSStream);
      theDI << aSStream;
    }
    else if (anArgCase == "-dumpjson")
    {
      Standard_SStream aSStream;
      aReport->DumpJson(aSStream);
      theDI << aSStream;
    }
  }

  return 0;
}

void Draw::MessageCommands(Draw_Interpretor& theCommands)
{
  static bool Done = false;
  if (Done)
    return;
  Done = true;

  const char* group = "DRAW Message Commands";

  theCommands.Add("PrintMessenger",
                  "PrintMessenger"
                  "\n\t\t: Prints DumpJson information about messenger.",
                  __FILE__,
                  PrintMessenger,
                  group);

  theCommands.Add(
    "SetMessagePrinter",
    "SetMessagePrinter [-type ostream|systemlog|report|draw] [-state {on|off}=on]"
    "\n\t\t: Sets or removes the printer in messenger."
    "\n\t\t: Option -type set type of printer. Printers are applied with And combination."
    "\n\t\t: Option -state add or remove printer",
    __FILE__,
    SetMessagePrinter,
    group);

  theCommands.Add("SendMessage",
                  "SendMessage text [text ...]"
                  "\n Sends the text into the messenger.\n",
                  __FILE__,
                  SendMessage,
                  group);

  theCommands.Add("ClearReport",
                  "Removes all alerts in default printer",
                  __FILE__,
                  ClearReport,
                  group);

  theCommands.Add(
    "SetReportMetric",
    "SetReportMetric [metric ...] \n Activate report metrics, deactivate all if there are no "
    "parameters.\n"
    "\n\t\t: metric is a value of Message_MetricType, e.g. 1 is Message_MetricType_UserTimeCPU",
    __FILE__,
    SetReportMetric,
    group);

  theCommands.Add("CollectMetricMessages",
                  "CollectMetricMessages [-activate {0|1}]"
                  "\n Start metric collection by 1, stop by 0. Result is placed in metric "
                  "attributes of message report.\n",
                  __FILE__,
                  CollectMetricMessages,
                  group);

  theCommands.Add("PrintReport",
                  "PrintReport [-messenger] [-dump] [-dumpJson]"
                  "\n\t\t: Send report content to default messenger or stream"
                  "\n\t\t: Output options:"
                  "\n\t\t:   -messenger Prints the information about report into messenger."
                  "\n\t\t:   -dump      Prints Dump information about report."
                  "\n\t\t:   -dumpJson  Prints DumpJson information about report.",
                  __FILE__,
                  PrintReport,
                  group);
}
