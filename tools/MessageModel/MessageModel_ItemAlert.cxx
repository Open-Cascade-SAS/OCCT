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

#include <inspector/MessageModel_ItemAlert.hxx>

#include <inspector/MessageModel_ItemRoot.hxx>
#include <inspector/MessageModel_ItemReport.hxx>
#include <inspector/MessageModel_TreeModel.hxx>

#include <inspector/Convert_TransientShape.hxx>
#include <inspector/TreeModel_ItemProperties.hxx>
#include <inspector/TreeModel_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Message_AlertExtended.hxx>
#include <Message_AttributeMeter.hxx>
#include <Message_AttributeStream.hxx>
#include <Message_CompositeAlerts.hxx>
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_AlertAttribute.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QColor>
#include <QIcon>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant MessageModel_ItemAlert::initValue(const int theRole) const
{
  QVariant aParentValue = MessageModel_ItemBase::initValue(theRole);
  if (aParentValue.isValid())
    return aParentValue;

  MessageModel_ItemReportPtr aReportItem = MessageModel_ItemReport::FindReportItem(Parent());
  if (!aReportItem)
    return QVariant();

  Handle(Message_Report) aReport = aReportItem->GetReport();
  if (aReport.IsNull())
    return QVariant();

  if (theRole == Qt::ForegroundRole)
  {
    if (!aReport->IsActiveInMessenger())
      return QColor(Qt::darkGray);

    return QVariant();
  }

  Handle(Message_Alert)         anAlert         = getAlert();
  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(anAlert);

  // if the alert is composite, process the real alert
  if (theRole == Qt::DecorationRole && Column() == 0)
  {
    if (anExtendedAlert.IsNull())
      return QVariant();

    Handle(Message_Attribute) anAttribute = anExtendedAlert->Attribute();
    if (anAttribute.IsNull())
      return QVariant();

    if (anAttribute->IsKind(STANDARD_TYPE(TopoDS_AlertAttribute)))
      return QIcon(":/icons/item_shape.png");
    else if (!Handle(Message_AttributeStream)::DownCast(anAttribute).IsNull())
      return QIcon(":/icons/item_streamValues.png");
    else
      return QVariant();
  }

  if (theRole != Qt::DisplayRole && theRole != Qt::ToolTipRole)
    return QVariant();

  if (anAlert.IsNull())
    return QVariant();

  if (Column() == 0)
  {
    if (theRole == Qt::DisplayRole)
    {
      TCollection_AsciiString aMessageKey = anAlert->GetMessageKey();
      if (aMessageKey.IsEmpty() && !Properties().IsNull())
        aMessageKey = Properties()->Key();
      return aMessageKey.ToCString();
    }
    else
      return anAlert->DynamicType()->Name();
  }

  Message_MetricType aMetricType;
  int                aPosition;
  if (MessageModel_TreeModel::IsMetricColumn(Column(), aMetricType, aPosition))
  {
    if (anExtendedAlert.IsNull())
      return QVariant();

    Handle(Message_AttributeMeter) anAttribute =
      Handle(Message_AttributeMeter)::DownCast(anExtendedAlert->Attribute());
    if (anAttribute.IsNull() || !anAttribute->HasMetric(aMetricType))
      return QVariant();

    if (!anAttribute->IsMetricValid(aMetricType))
      return QVariant("in process");

    if (aMetricType == Message_MetricType_ProcessCPUUserTime
        || aMetricType == Message_MetricType_ProcessCPUSystemTime
        || aMetricType == Message_MetricType_WallClock)
    {
      Standard_Real aCumulativeMetric =
        anAttribute->StopValue(aMetricType) - anAttribute->StartValue(aMetricType);
      if (fabs(aCumulativeMetric) < Precision::Confusion())
        return QVariant();

      if (aPosition == 0)
        return aCumulativeMetric;
      else if (aPosition == 1)
      {
        Standard_Real aReportCumulativeMetric =
          MessageModel_ItemReport::CumulativeMetric(aReport, aMetricType);
        if (fabs(aReportCumulativeMetric) > Precision::Confusion())
          return 100. * aCumulativeMetric / aReportCumulativeMetric;
        else
          return QVariant();
      }
    }
    else
    {
      if (aPosition == 0)
        return anAttribute->StopValue(aMetricType);
      else if (aPosition == 1)
      {
        Standard_Real aCumulativeMetric =
          anAttribute->StopValue(aMetricType) - anAttribute->StartValue(aMetricType);
        if (fabs(aCumulativeMetric) < Precision::Confusion())
          return QVariant();
        else
          return aCumulativeMetric;
      }
    }
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int MessageModel_ItemAlert::initRowCount() const
{
  const Handle(Message_Alert)& anAlert = getAlert();
  if (anAlert.IsNull())
    return 0;

  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(anAlert);
  if (anExtendedAlert.IsNull())
    return 0;

  Handle(Message_CompositeAlerts) aCompositeAlert = anExtendedAlert->CompositeAlerts();
  if (aCompositeAlert.IsNull())
    return 0;

  MessageModel_ItemAlert* aCurrentItem = (MessageModel_ItemAlert*)this;
  for (int aGravityId = Message_Trace; aGravityId <= Message_Fail; aGravityId++)
  {
    const Message_ListOfAlert& anAlerts = aCompositeAlert->Alerts((Message_Gravity)aGravityId);
    {
      for (Message_ListOfAlert::Iterator anIt(anAlerts); anIt.More(); anIt.Next())
      {
        Message_ListOfAlert aCurAlerts;
        aCurAlerts.Append(anIt.Value());
        aCurrentItem->myChildAlerts.Bind(myChildAlerts.Size(), aCurAlerts);
      }
    }
  }
  return aCurrentItem->myChildAlerts.Size();
}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void MessageModel_ItemAlert::initStream(Standard_OStream& theOStream) const
{
  Handle(Message_AlertExtended) anExtendedAlert =
    Handle(Message_AlertExtended)::DownCast(getAlert());
  if (anExtendedAlert.IsNull() || anExtendedAlert->Attribute().IsNull())
    return;

  Handle(Message_Attribute) anAttribute = anExtendedAlert->Attribute();
  if (anAttribute.IsNull())
    return;

  if (Handle(Message_AttributeStream)::DownCast(anAttribute).IsNull())
    return;

  Handle(Message_AttributeStream) anAttributeStream =
    Handle(Message_AttributeStream)::DownCast(anExtendedAlert->Attribute());
  theOStream << anAttributeStream->Stream().str();
}

// =======================================================================
// function : SetStream
// purpose :
// =======================================================================
bool MessageModel_ItemAlert::SetStream(const Standard_SStream& theSStream,
                                       Standard_Integer&       theStartPos,
                                       Standard_Integer&       theLastPos) const
{
  Handle(Message_AlertExtended) anExtendedAlert =
    Handle(Message_AlertExtended)::DownCast(getAlert());
  if (anExtendedAlert.IsNull() || anExtendedAlert->Attribute().IsNull())
    return false;

  Handle(Message_Attribute) anAttribute = anExtendedAlert->Attribute();
  if (anAttribute.IsNull())
    return false;

  if (Handle(Message_AttributeStream)::DownCast(anAttribute).IsNull())
    return false;

  Handle(Message_AttributeStream) anAttributeStream =
    Handle(Message_AttributeStream)::DownCast(anExtendedAlert->Attribute());
  TCollection_AsciiString aStreamValue = Standard_Dump::Text(anAttributeStream->Stream());

  TCollection_AsciiString aNewValue = Standard_Dump::Text(theSStream);

  Standard_SStream aStream;
  aStream << aStreamValue.SubString(1, theStartPos - 1);
  aStream << aNewValue;
  if (theLastPos + 1 <= aStreamValue.Length())
    aStream << aStreamValue.SubString(theLastPos + 1, aStreamValue.Length());

  anAttributeStream->SetStream(aStream);

  return true;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr MessageModel_ItemAlert::createChild(int theRow, int theColumn)
{
  return MessageModel_ItemAlert::CreateItem(currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void MessageModel_ItemAlert::Init()
{
  MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport>(Parent());
  MessageModel_ItemAlertPtr  anAlertItem;
  Handle(Message_Alert)      anAlert;
  if (aReportItem)
  {
    Message_ListOfAlert anAlerts;
    if (aReportItem->GetChildAlerts(Row(), anAlerts))
    {
      myAlert = anAlerts.First();
    }
  }
  else
  {
    anAlertItem = itemDynamicCast<MessageModel_ItemAlert>(Parent());
    if (anAlertItem)
    {
      Message_ListOfAlert anAlerts;
      if (anAlertItem->GetChildAlerts(Row(), anAlerts))
      {
        myAlert = anAlerts.First();
      }
    }
  }

  Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(myAlert);
  if (!anExtendedAlert.IsNull() && !anExtendedAlert->Attribute().IsNull())
  {
    Handle(Message_Attribute) anAttribute = anExtendedAlert->Attribute();
    if (!anAttribute.IsNull())
    {
      if (anAttribute->IsKind(STANDARD_TYPE(TopoDS_AlertAttribute)))
        myPresentation = new Convert_TransientShape(
          Handle(TopoDS_AlertAttribute)::DownCast(anAttribute)->GetShape());
    }
  }
  MessageModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void MessageModel_ItemAlert::Reset()
{
  MessageModel_ItemBase::Reset();
  myAlert = Handle(Message_Alert)();
  myChildAlerts.Clear();
  myPresentation = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void MessageModel_ItemAlert::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<MessageModel_ItemAlert*>(this)->Init();
}

// =======================================================================
// function : getAlert
// purpose :
// =======================================================================
const Handle(Message_Alert)& MessageModel_ItemAlert::getAlert() const
{
  initItem();
  return myAlert;
}
