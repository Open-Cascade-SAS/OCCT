// Created on: 2021-04-27
// Created by: Svetlana SHUTINA
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

#include <inspector/MessageView_MetricStatisticModel.hxx>
#include <inspector/MessageModel_ItemAlert.hxx>
#include <inspector/MessageModel_ItemReport.hxx>

#include <Message_AlertExtended.hxx>
#include <Message_Attribute.hxx>
#include <Message_AttributeMeter.hxx>
#include <Message_CompositeAlerts.hxx>
#include <Message_Report.hxx>

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void MessageView_MetricStatisticModel::Init (const TreeModel_ItemBasePtr theItemBase)
{
  MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport> (theItemBase);
  if (aReportItem)
  {
    Handle(Message_Report) aReport = aReportItem->GetReport();
    const Message_ListOfAlert& anAlerts = aReport->GetAlerts (Message_Info);
    for (Message_ListOfAlert::Iterator anIt(anAlerts); anIt.More(); anIt.Next())
    {
      appendAlert (anIt.Value());
    }
  }
  else
  {
    MessageModel_ItemAlertPtr anAlertItem = itemDynamicCast<MessageModel_ItemAlert> (theItemBase);
    if (anAlertItem)
    {
      appendAlert (anAlertItem->GetAlert());
    }
  }
  std::map<double, std::list<QString>> aTmpMap;
  std::list<double> aTimes;
  for (QMap<QString, QPair<int, double> >::Iterator anIterValue = myValues.begin();
       anIterValue != myValues.end(); ++anIterValue)
  {
    std::map<double, std::list<QString>>::iterator anIter = aTmpMap.find (anIterValue.value().second);
    if (anIter != aTmpMap.end())
    {
      aTmpMap.at (anIterValue.value().second).push_back (anIterValue.key());
    }
    else
    {
      std::list<QString> list;
      list.push_back (anIterValue.key());
      aTmpMap.insert (std::pair<double, std::list<QString> > (anIterValue.value().second, list));
      aTimes.push_back (anIterValue.value().second);
    }
  }
  aTimes.sort();
  aTimes.reverse();

  for (std::list<double>::iterator anIter = aTimes.begin(); anIter != aTimes.end(); anIter++)
  {
    double aTime = *anIter;
    std::list<QString> names = aTmpMap.at (aTime);
    for (std::list<QString>::iterator name = names.begin(); name != names.end(); name++)
    {
      int nb = myValues.find (*name).value().first;
      RowValues value = {*name, nb, aTime};
      setValueByIndex (-1, value);
    }
  }
}

// =======================================================================
// function : appendAlert
// purpose :
// =======================================================================
void MessageView_MetricStatisticModel::appendAlert(const Handle(Message_Alert)& theAlert)
{
  Handle(Message_AlertExtended) anExtAlert = Handle(Message_AlertExtended)::DownCast (theAlert);
  if (anExtAlert.IsNull())
  {
    return;
  }
  Handle(Message_Attribute) anAttr = anExtAlert->Attribute();
  Handle(Message_AttributeMeter) anAttrMeter = Handle(Message_AttributeMeter)::DownCast (anAttr);
  if(anAttrMeter.IsNull())
  {
    return;
  }

  int aCount = 1;
  double aTime = 0;
  if (myValues.contains (anAttr->GetName().ToCString()))
  {
    aCount = myValues.value (anAttr->GetName().ToCString()).first + 1;
    aTime = myValues.value (anAttr->GetName().ToCString()).second
          + anAttrMeter->StopValue (myMetricType) - anAttrMeter->StartValue (myMetricType);
  }
  else
  {
    aCount = 1;
    aTime = anAttrMeter->StopValue (myMetricType) - anAttrMeter->StartValue (myMetricType);
  }
  myValues[anAttr->GetName().ToCString()] = qMakePair (aCount, aTime);

  if (!anExtAlert->CompositeAlerts().IsNull())
  {
    const Message_ListOfAlert& anAlerts = anExtAlert->CompositeAlerts()->Alerts (Message_Info);
    for (Message_ListOfAlert::Iterator anIt (anAlerts); anIt.More(); anIt.Next())
    {
      appendAlert (anIt.Value());
    }
  }
}

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant MessageView_MetricStatisticModel::data (const QModelIndex& theIndex, int theRole) const
{
  switch (theRole)
  {
    case Qt::DisplayRole:
    {
      switch (theIndex.column())
      {
        case 0: return mySortValues[theIndex.row()].myName;
        case 1: return mySortValues[theIndex.row()].myCounter;
        case 2: return mySortValues[theIndex.row()].myTime;
      }
      break;
    }
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : setValueByIndex
// purpose :
// =======================================================================
void MessageView_MetricStatisticModel::setValueByIndex (const int theIndex, const RowValues theValue)
{
  mySortValues.insert (theIndex == -1 ? mySortValues.size() : theIndex, theValue);
}
