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

#include <inspector/MessageModel_TreeModel.hxx>

#include <inspector/MessageModel_ItemAlert.hxx>
#include <inspector/MessageModel_ItemRoot.hxx>
#include <inspector/MessageModel_ItemReport.hxx>
#include <inspector/TreeModel_ColumnType.hxx>

#include <Message.hxx>

const int COLUMN_NAME_WIDTH = 230;
const int COLUMN_SIZE_WIDTH = 30;

const int COLUMN_REAL_VALUE_WIDTH = 115;
const int COLUMN_PERCENT_VALUE_WIDTH = 50;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
MessageModel_TreeModel::MessageModel_TreeModel (QObject* theParent)
: TreeModel_ModelBase (theParent)
{
}

// =======================================================================
// function : InitColumns
// purpose :
// =======================================================================
void MessageModel_TreeModel::InitColumns()
{
  // 0 - Name, 1 - visibility, 2 - Row
  setHeaderItem (TreeModel_ColumnType_Name,       TreeModel_HeaderSection ("Name", COLUMN_NAME_WIDTH));
  setHeaderItem (TreeModel_ColumnType_Visibility, TreeModel_HeaderSection ("Visibility", TreeModel_ModelBase::ColumnVisibilityWidth()));
  setHeaderItem (TreeModel_ColumnType_Row,        TreeModel_HeaderSection ("Row", COLUMN_SIZE_WIDTH, Standard_True /*hidden*/));

  int aNextIndex = 3;
  for (int aMetricId = (int)Message_MetricType_None + 1; aMetricId <= (int)Message_MetricType_MemHeapUsage; aMetricId++)
  {
    Message_MetricType aMetricType = (Message_MetricType)aMetricId;
    OSD_MemInfo::Counter aMemInfo;
    bool isMemInfo = Message::ToOSDMetric (aMetricType, aMemInfo);

    setHeaderItem (aNextIndex++,
      TreeModel_HeaderSection (QString("%1 [%2]").arg (Message::MetricToString (aMetricType)).arg(isMemInfo ? "Mb" : "s"),
      COLUMN_REAL_VALUE_WIDTH));
    setHeaderItem (aNextIndex++, TreeModel_HeaderSection (isMemInfo ? "Delta" : "%", COLUMN_PERCENT_VALUE_WIDTH));
  }
}

// =======================================================================
// function : GetMetricColumns
// purpose :
// =======================================================================
void MessageModel_TreeModel::GetMetricColumns (const Message_MetricType theMetricType, QList<int>& theMetricColumns)
{
  theMetricColumns.clear();
  int aNextIndex = 3; // after default parent columns, see InitColumns
  for (int aMetricId = (int)Message_MetricType_None + 1; aMetricId <= (int)Message_MetricType_MemHeapUsage; aMetricId++)
  {
    if (theMetricType != (Message_MetricType)aMetricId)
    {
      aNextIndex += 2;
      continue;
    }
    theMetricColumns.append (aNextIndex++);
    theMetricColumns.append (aNextIndex++);
  }
}

// =======================================================================
// function : IsMetricColumn
// purpose :
// =======================================================================
bool MessageModel_TreeModel::IsMetricColumn (const int theColumnId, Message_MetricType& theMetricType, int& thePosition)
{
  int aNextIndex = 3; // after default parent columns, see InitColumns
  for (int aMetricId = (int)Message_MetricType_None + 1; aMetricId <= (int)Message_MetricType_MemHeapUsage; aMetricId++)
  {
    if (theColumnId == aNextIndex || theColumnId == aNextIndex + 1)
    {
      theMetricType = (Message_MetricType)aMetricId;
      thePosition = theColumnId - aNextIndex;
      return true;
    }
    aNextIndex += 2;
  }
  return false;
}

// =======================================================================
// function : createRootItem
// purpose :
// =======================================================================
TreeModel_ItemBasePtr MessageModel_TreeModel::createRootItem (const int theColumnId)
{
  return MessageModel_ItemRoot::CreateItem (TreeModel_ItemBasePtr(), 0, theColumnId);
}

// =======================================================================
// function : HasReport
// purpose :
// =======================================================================
Standard_Boolean MessageModel_TreeModel::HasReport (const Handle(Message_Report)& theReport)
{
  if (columnCount() == 0)
    return Standard_False;

  MessageModel_ItemRootPtr aRootItem = itemDynamicCast<MessageModel_ItemRoot> (RootItem (0));
  return aRootItem && aRootItem->HasReport (theReport);
}

// =======================================================================
// function : AddReport
// purpose :
// =======================================================================
void MessageModel_TreeModel::AddReport (const Handle(Message_Report)& theReport,
                                        const TCollection_AsciiString& theReportDescription)
{
  for (int aColId = 0, aNbColumns = columnCount(); aColId < aNbColumns; aColId++)
  {
    MessageModel_ItemRootPtr aRootItem = itemDynamicCast<MessageModel_ItemRoot> (RootItem (aColId));
    if (!aRootItem)
      continue;
    aRootItem->AddReport (theReport, theReportDescription);
  }

  Reset();
  EmitLayoutChanged();
}

// =======================================================================
// function : SetReport
// purpose :
// =======================================================================
void MessageModel_TreeModel::SetReport (const int theRowId, const Handle(Message_Report)& theReport,
                                        const TCollection_AsciiString& theReportDescription)
{
  for (int aColId = 0, aNbColumns = columnCount(); aColId < aNbColumns; aColId++)
  {
    MessageModel_ItemRootPtr aRootItem = itemDynamicCast<MessageModel_ItemRoot> (RootItem (aColId));
    if (!aRootItem)
      continue;
    aRootItem->SetReport (theRowId, theReport, theReportDescription);
  }
  Reset();
  EmitLayoutChanged();
}

// =======================================================================
// function : Reports
// purpose :
// =======================================================================
const NCollection_List<MessageModel_ReportInformation>& MessageModel_TreeModel::Reports() const
{
  MessageModel_ItemRootPtr aRootItem = itemDynamicCast<MessageModel_ItemRoot> (RootItem (0));
  return aRootItem->Reports();
}

// =======================================================================
// function : UpdateTreeModel
// purpose :
// =======================================================================
void MessageModel_TreeModel::SetRootItemName (const TCollection_AsciiString& theName)
{
  MessageModel_ItemRootPtr aRootItem = itemDynamicCast<MessageModel_ItemRoot> (RootItem (0));
  if (aRootItem)
    aRootItem->SetName (theName);
}

// =======================================================================
// function : UpdateTreeModel
// purpose :
// =======================================================================
void MessageModel_TreeModel::UpdateTreeModel()
{
  Reset();
  EmitLayoutChanged();
}
