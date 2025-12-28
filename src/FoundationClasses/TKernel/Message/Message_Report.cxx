// Created on: 2017-06-26
// Created by: Andrey Betenev
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <Message_Report.hxx>

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_AttributeMeter.hxx>
#include <Message_Attribute.hxx>
#include <Message_CompositeAlerts.hxx>
#include <Message_Msg.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterToReport.hxx>

#include <Precision.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_Report, Standard_Transient)

//=================================================================================================

Message_Report::Message_Report()
    : myLimit(-1),
      myIsActiveInMessenger(false)
{
}

//=================================================================================================

void Message_Report::AddAlert(Message_Gravity                   theGravity,
                              const occ::handle<Message_Alert>& theAlert)
{
  std::lock_guard<std::mutex> aLock(myMutex);

  // alerts of the top level
  if (myAlertLevels.IsEmpty())
  {
    occ::handle<Message_CompositeAlerts> aCompositeAlert = compositeAlerts(true);
    if (aCompositeAlert->AddAlert(theGravity, theAlert))
    {
      return;
    }

    // remove alerts under the report only
    const NCollection_List<occ::handle<Message_Alert>>& anAlerts =
      aCompositeAlert->Alerts(theGravity);
    if (anAlerts.Extent() > myLimit)
    {
      aCompositeAlert->RemoveAlert(theGravity, anAlerts.First());
    }
    return;
  }

  // if there are some levels of alerts, the new alert will be placed below the root
  myAlertLevels.Last()->AddAlert(theGravity, theAlert);
}

//=================================================================================================

const NCollection_List<occ::handle<Message_Alert>>& Message_Report::GetAlerts(
  Message_Gravity theGravity) const
{
  static const NCollection_List<occ::handle<Message_Alert>> anEmptyList;
  if (myCompositAlerts.IsNull())
  {
    return anEmptyList;
  }
  return myCompositAlerts->Alerts(theGravity);
}

//=================================================================================================

bool Message_Report::HasAlert(const occ::handle<Standard_Type>& theType)
{
  for (int iGravity = Message_Trace; iGravity <= Message_Fail; ++iGravity)
  {
    if (HasAlert(theType, (Message_Gravity)iGravity))
      return true;
  }
  return false;
}

//=================================================================================================

bool Message_Report::HasAlert(const occ::handle<Standard_Type>& theType, Message_Gravity theGravity)
{
  if (compositeAlerts().IsNull())
  {
    return false;
  }

  return compositeAlerts()->HasAlert(theType, theGravity);
}

//=================================================================================================

bool Message_Report::IsActiveInMessenger(const occ::handle<Message_Messenger>&) const
{
  return myIsActiveInMessenger;
}

//=================================================================================================

void Message_Report::ActivateInMessenger(const bool                            toActivate,
                                         const occ::handle<Message_Messenger>& theMessenger)
{
  if (toActivate == IsActiveInMessenger())
    return;

  myIsActiveInMessenger = toActivate;
  occ::handle<Message_Messenger> aMessenger =
    theMessenger.IsNull() ? Message::DefaultMessenger() : theMessenger;
  if (toActivate)
  {
    occ::handle<Message_PrinterToReport> aPrinterToReport = new Message_PrinterToReport();
    aPrinterToReport->SetReport(this);
    aMessenger->AddPrinter(aPrinterToReport);
  }
  else // deactivate
  {
    NCollection_Sequence<occ::handle<Message_Printer>> aPrintersToRemove;
    for (NCollection_Sequence<occ::handle<Message_Printer>>::Iterator anIterator(
           aMessenger->Printers());
         anIterator.More();
         anIterator.Next())
    {
      const occ::handle<Message_Printer>& aPrinter = anIterator.Value();
      if (aPrinter->IsKind(STANDARD_TYPE(Message_PrinterToReport))
          && occ::down_cast<Message_PrinterToReport>(aPrinter)->Report() == this)
        aPrintersToRemove.Append(aPrinter);
    }
    for (NCollection_Sequence<occ::handle<Message_Printer>>::Iterator anIterator(aPrintersToRemove);
         anIterator.More();
         anIterator.Next())
    {
      aMessenger->RemovePrinter(anIterator.Value());
    }
  }
}

//=================================================================================================

void Message_Report::UpdateActiveInMessenger(const occ::handle<Message_Messenger>& theMessenger)
{
  occ::handle<Message_Messenger> aMessenger =
    theMessenger.IsNull() ? Message::DefaultMessenger() : theMessenger;
  for (NCollection_Sequence<occ::handle<Message_Printer>>::Iterator anIterator(
         aMessenger->Printers());
       anIterator.More();
       anIterator.Next())
  {
    if (anIterator.Value()->IsKind(STANDARD_TYPE(Message_PrinterToReport))
        && occ::down_cast<Message_PrinterToReport>(anIterator.Value())->Report() == this)
    {
      myIsActiveInMessenger = true;
      return;
    }
  }
  myIsActiveInMessenger = false;
}

//=================================================================================================

void Message_Report::AddLevel(Message_Level* theLevel, const TCollection_AsciiString& theName)
{
  std::lock_guard<std::mutex> aLock(myMutex);

  myAlertLevels.Append(theLevel);

  occ::handle<Message_AlertExtended> aLevelRootAlert = new Message_AlertExtended();

  occ::handle<Message_Attribute> anAttribute;
  if (!ActiveMetrics().IsEmpty())
  {
    anAttribute = new Message_AttributeMeter(theName);
  }
  else
  {
    anAttribute = new Message_Attribute(theName);
  }
  aLevelRootAlert->SetAttribute(anAttribute);
  theLevel->SetRootAlert(aLevelRootAlert, myAlertLevels.Size() == 1);

  if (myAlertLevels.Size() == 1) // this is the first level, so root alert should be pushed in the
                                 // report composite of alerts
  {
    compositeAlerts(true)->AddAlert(Message_Info, theLevel->RootAlert());
  }
  if (myAlertLevels.Size() > 1) // this is the first level, so root alert should be pushed in the
                                // report composite of alerts
  {
    // root alert of next levels should be pushed under the previous level
    Message_Level* aPrevLevel = myAlertLevels.Value(myAlertLevels.Size() - 1); // previous level
    aPrevLevel->AddAlert(Message_Info, aLevelRootAlert);
  }
}

//=================================================================================================

void Message_Report::RemoveLevel(Message_Level* theLevel)
{
  std::lock_guard<std::mutex> aLock(myMutex);

  for (int aLevelIndex = myAlertLevels.Size(); aLevelIndex >= 1; aLevelIndex--)
  {
    Message_Level* aLevel = myAlertLevels.Value(aLevelIndex);
    Message_AttributeMeter::StopAlert(aLevel->RootAlert());

    myAlertLevels.Remove(aLevelIndex);
    if (aLevel == theLevel)
    {
      return;
    }
  }
}

//=================================================================================================

void Message_Report::Clear()
{
  if (compositeAlerts().IsNull())
  {
    return;
  }

  std::lock_guard<std::mutex> aLock(myMutex);

  compositeAlerts()->Clear();
  myAlertLevels.Clear();
}

//=================================================================================================

void Message_Report::Clear(Message_Gravity theGravity)
{
  if (compositeAlerts().IsNull())
  {
    return;
  }

  std::lock_guard<std::mutex> aLock(myMutex);

  compositeAlerts()->Clear(theGravity);
  myAlertLevels.Clear();
}

//=================================================================================================

void Message_Report::Clear(const occ::handle<Standard_Type>& theType)
{
  if (compositeAlerts().IsNull())
  {
    return;
  }

  std::lock_guard<std::mutex> aLock(myMutex);

  compositeAlerts()->Clear(theType);
  myAlertLevels.Clear();
}

//=================================================================================================

void Message_Report::SetActiveMetric(const Message_MetricType theMetricType, const bool theActivate)
{
  if (theActivate == myActiveMetrics.Contains(theMetricType))
  {
    return;
  }

  if (theActivate)
  {
    myActiveMetrics.Add(theMetricType);
  }
  else
  {
    myActiveMetrics.RemoveKey(theMetricType);
  }
}

//=================================================================================================

void Message_Report::Dump(Standard_OStream& theOS)
{
  for (int iGravity = Message_Trace; iGravity <= Message_Fail; ++iGravity)
  {
    Dump(theOS, (Message_Gravity)iGravity);
  }
}

//=================================================================================================

void Message_Report::Dump(Standard_OStream& theOS, Message_Gravity theGravity)
{
  if (compositeAlerts().IsNull())
  {
    return;
  }

  if (compositeAlerts().IsNull())
  {
    return;
  }

  dumpMessages(theOS, theGravity, compositeAlerts());
}

//=================================================================================================

void Message_Report::SendMessages(const occ::handle<Message_Messenger>& theMessenger)
{
  for (int aGravIter = Message_Trace; aGravIter <= Message_Fail; ++aGravIter)
  {
    SendMessages(theMessenger, (Message_Gravity)aGravIter);
  }
}

//=================================================================================================

void Message_Report::SendMessages(const occ::handle<Message_Messenger>& theMessenger,
                                  Message_Gravity                       theGravity)
{
  if (compositeAlerts().IsNull())
  {
    return;
  }

  sendMessages(theMessenger, theGravity, compositeAlerts());
}

//=================================================================================================

void Message_Report::Merge(const occ::handle<Message_Report>& theOther)
{
  for (int aGravIter = Message_Trace; aGravIter <= Message_Fail; ++aGravIter)
  {
    Merge(theOther, (Message_Gravity)aGravIter);
  }
}

//=================================================================================================

void Message_Report::Merge(const occ::handle<Message_Report>& theOther, Message_Gravity theGravity)
{
  for (NCollection_List<occ::handle<Message_Alert>>::Iterator anIt(theOther->GetAlerts(theGravity));
       anIt.More();
       anIt.Next())
  {
    AddAlert(theGravity, anIt.Value());
  }
}

//=================================================================================================

const occ::handle<Message_CompositeAlerts>& Message_Report::compositeAlerts(const bool isCreate)
{
  if (myCompositAlerts.IsNull() && isCreate)
  {
    myCompositAlerts = new Message_CompositeAlerts();
  }

  return myCompositAlerts;
}

//=================================================================================================

void Message_Report::sendMessages(const occ::handle<Message_Messenger>&       theMessenger,
                                  Message_Gravity                             theGravity,
                                  const occ::handle<Message_CompositeAlerts>& theCompositeAlert)
{
  if (theCompositeAlert.IsNull())
  {
    return;
  }

  const NCollection_List<occ::handle<Message_Alert>>& anAlerts =
    theCompositeAlert->Alerts(theGravity);
  for (NCollection_List<occ::handle<Message_Alert>>::Iterator anIt(anAlerts); anIt.More();
       anIt.Next())
  {
    theMessenger->Send(anIt.Value()->GetMessageKey(), theGravity);
    occ::handle<Message_AlertExtended> anExtendedAlert =
      occ::down_cast<Message_AlertExtended>(anIt.Value());
    if (anExtendedAlert.IsNull())
    {
      continue;
    }

    occ::handle<Message_CompositeAlerts> aCompositeAlerts = anExtendedAlert->CompositeAlerts();
    if (aCompositeAlerts.IsNull())
    {
      continue;
    }

    sendMessages(theMessenger, theGravity, aCompositeAlerts);
  }
}

//=================================================================================================

void Message_Report::dumpMessages(Standard_OStream&                           theOS,
                                  Message_Gravity                             theGravity,
                                  const occ::handle<Message_CompositeAlerts>& theCompositeAlert)
{
  if (theCompositeAlert.IsNull())
  {
    return;
  }

  const NCollection_List<occ::handle<Message_Alert>>& anAlerts =
    theCompositeAlert->Alerts(theGravity);
  for (NCollection_List<occ::handle<Message_Alert>>::Iterator anIt(anAlerts); anIt.More();
       anIt.Next())
  {
    theOS << anIt.Value()->GetMessageKey() << std::endl;

    occ::handle<Message_AlertExtended> anExtendedAlert =
      occ::down_cast<Message_AlertExtended>(anIt.Value());
    if (anExtendedAlert.IsNull())
    {
      continue;
    }

    dumpMessages(theOS, theGravity, anExtendedAlert->CompositeAlerts());
  }
}

//=================================================================================================

void Message_Report::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  if (!myCompositAlerts.IsNull())
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myCompositAlerts.get())
  }

  int anAlertLevels = myAlertLevels.Size();
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, anAlertLevels)

  int anInc = 1;
  for (NCollection_IndexedMap<Message_MetricType>::Iterator anIterator(myActiveMetrics);
       anIterator.More();
       anIterator.Next())
  {
    Message_MetricType anActiveMetric = anIterator.Value();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL_INC(theOStream, anActiveMetric, anInc++)
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myLimit)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsActiveInMessenger)
}
