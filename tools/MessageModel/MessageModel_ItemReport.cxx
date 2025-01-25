// Created on: 2021-04-27
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <inspector/MessageModel_ItemReport.hxx>

#include <inspector/MessageModel_ItemAlert.hxx>
#include <inspector/MessageModel_ItemRoot.hxx>
#include <inspector/MessageModel_TreeModel.hxx>

#include <Message.hxx>
#include <Message_Alert.hxx>
#include <Message_AttributeMeter.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterToReport.hxx>
#include <OSD_Path.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QColor>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant MessageModel_ItemReport::initValue(const int theRole) const
{
  QVariant aParentValue = MessageModel_ItemBase::initValue(theRole);
  if (aParentValue.isValid())
    return aParentValue;

  const Handle(Message_Report)& aReport = getReport();
  if (aReport.IsNull())
    return QVariant();

  if (theRole == Qt::ForegroundRole)
  {
    if (!aReport->IsActiveInMessenger())
      return QColor(Qt::darkGray);

    return QVariant();
  }
  if (theRole == Qt::ToolTipRole && !myDescription.IsEmpty()
      && Column() == 0) // display the exported file name in tool tip
  {
    OSD_Path aPath(myDescription);
    return QString("%1%2").arg(aPath.Name().ToCString()).arg(aPath.Extension().ToCString());
  }

  if (theRole != Qt::DisplayRole)
    return QVariant();

  if (Column() == 0)
    return aReport->DynamicType()->Name();

  Message_MetricType aMetricType;
  int                aPosition;
  if (MessageModel_TreeModel::IsMetricColumn(Column(), aMetricType, aPosition)
      && (aMetricType == Message_MetricType_ProcessCPUUserTime
          || aMetricType == Message_MetricType_ProcessCPUSystemTime
          || aMetricType == Message_MetricType_WallClock))
  {
    if (aPosition == 0)
      return CumulativeMetric(aReport, aMetricType);
    else if (aPosition == 1)
      return "100";
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int MessageModel_ItemReport::initRowCount() const
{
  const Handle(Message_Report)& aReport = getReport();
  if (aReport.IsNull())
    return 0;

  MessageModel_ItemReport* aCurrentItem = (MessageModel_ItemReport*)this;
  for (int aGravityId = Message_Trace; aGravityId <= Message_Fail; aGravityId++)
  {
    const Message_ListOfAlert& anAlerts = aReport->GetAlerts((Message_Gravity)aGravityId);
    for (Message_ListOfAlert::Iterator anIt(anAlerts); anIt.More(); anIt.Next())
    {
      Message_ListOfAlert aCurAlerts;
      aCurAlerts.Append(anIt.Value());
      aCurrentItem->myChildAlerts.Bind(myChildAlerts.Size(), aCurAlerts);
    }
  }
  return aCurrentItem->myChildAlerts.Size();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr MessageModel_ItemReport::createChild(int theRow, int theColumn)
{
  return MessageModel_ItemAlert::CreateItem(currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void MessageModel_ItemReport::Init()
{
  MessageModel_ItemRootPtr aRootItem = itemDynamicCast<MessageModel_ItemRoot>(Parent());
  myReport = aRootItem ? aRootItem->GetReport(Row(), myDescription) : Handle(Message_Report)();

  MessageModel_ItemBase::Init();
}

// =======================================================================
// function : getReport
// purpose :
// =======================================================================
const Handle(Message_Report)& MessageModel_ItemReport::getReport() const
{
  initItem();
  return myReport;
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void MessageModel_ItemReport::Reset()
{
  MessageModel_ItemBase::Reset();
  myReport = Handle(Message_Report)();
  myChildAlerts.Clear();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void MessageModel_ItemReport::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<MessageModel_ItemReport*>(this)->Init();
}

// =======================================================================
// function : FindReportItem
// purpose :
// =======================================================================
MessageModel_ItemReportPtr MessageModel_ItemReport::FindReportItem(
  const TreeModel_ItemBasePtr& theItem)
{
  TreeModel_ItemBasePtr anItem = theItem;
  while (anItem)
  {
    if (MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport>(anItem))
      return aReportItem;

    anItem = anItem->Parent();
  }
  return MessageModel_ItemReportPtr();
}

// =======================================================================
// function : FindReport
// purpose :
// =======================================================================
Handle(Message_Report) MessageModel_ItemReport::FindReport(const MessageModel_ItemBasePtr& theItem)
{
  Handle(Message_Report) aReport;

  MessageModel_ItemBasePtr anItem = theItem;
  while (anItem)
  {
    MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport>(anItem);

    if (aReportItem)
      return aReportItem->GetReport();

    anItem = itemDynamicCast<MessageModel_ItemBase>(anItem->Parent());
  }
  return NULL;
}

// =======================================================================
// function : CumulativeMetric
// purpose :
// =======================================================================
Standard_Real MessageModel_ItemReport::CumulativeMetric(const Handle(Message_Report)& theReport,
                                                        const Message_MetricType      theMetricType)
{
  if (!theReport->ActiveMetrics().Contains(theMetricType))
    return 0;

  Standard_Real aMetric = 0;
  for (int iGravity = Message_Trace; iGravity <= Message_Fail; ++iGravity)
  {
    const Message_ListOfAlert&     anAlerts = theReport->GetAlerts((Message_Gravity)iGravity);
    Handle(Message_AttributeMeter) aFirstAttribute /*, aLastAttribute*/;
    for (Message_ListOfAlert::Iterator anAlertsIterator(anAlerts); anAlertsIterator.More();
         anAlertsIterator.Next())
    {
      Handle(Message_AlertExtended) anAlert =
        Handle(Message_AlertExtended)::DownCast(anAlertsIterator.Value());
      if (anAlert.IsNull())
        continue;
      Handle(Message_AttributeMeter) anAttribute =
        Handle(Message_AttributeMeter)::DownCast(anAlert->Attribute());
      if (anAttribute.IsNull() || !anAttribute->HasMetric(theMetricType)
          || !anAttribute->IsMetricValid(theMetricType))
        continue;

      // if (aFirstAttribute.IsNull())
      //   aFirstAttribute = anAttribute;
      // else
      //{
      // aLastAttribute = anAttribute;
      //}
      aMetric += anAttribute->StopValue(theMetricType) - anAttribute->StartValue(theMetricType);
    }
    // if (aFirstAttribute.IsNull())
    //   continue;
    // if (aLastAttribute.IsNull())
    //   aLastAttribute = aFirstAttribute;

    // aMetric += aLastAttribute->StopValue (theMetricType) - aFirstAttribute->StartValue
    // (theMetricType);
  }
  return aMetric;
}
