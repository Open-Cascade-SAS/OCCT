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

#include <inspector/MessageModel_Actions.hxx>

#include <inspector/MessageModel_ItemReport.hxx>
#include <inspector/MessageModel_ItemRoot.hxx>
#include <inspector/MessageModel_ItemAlert.hxx>
#include <inspector/MessageModel_TreeModel.hxx>
#include <inspector/TInspectorAPI_PluginParameters.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterToReport.hxx>
#include <OSD_Chronometer.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_ColorRGBA.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_AlertAttribute.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAction>
#include <QFileDialog>
#include <QItemSelectionModel>
#include <QMenu>
#include <QMessageBox>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
MessageModel_Actions::MessageModel_Actions(QWidget*                theParent,
                                           MessageModel_TreeModel* theTreeModel,
                                           QItemSelectionModel*    theModel)
    : QObject(theParent),
      myTreeModel(theTreeModel),
      mySelectionModel(theModel)
{
  myActions.insert(
    MessageModel_ActionType_Activate,
    ViewControl_Tools::CreateAction(tr("Activate"), SLOT(OnActivateReport()), parent(), this));
  myActions.insert(
    MessageModel_ActionType_Deactivate,
    ViewControl_Tools::CreateAction(tr("Deactivate"), SLOT(OnDeactivateReport()), parent(), this));
  myActions.insert(
    MessageModel_ActionType_Clear,
    ViewControl_Tools::CreateAction(tr("Clear"), SLOT(OnClearReport()), parent(), this));
  myActions.insert(MessageModel_ActionType_ExportToShapeView,
                   ViewControl_Tools::CreateAction(tr("Export to ShapeView"),
                                                   SLOT(OnExportToShapeView()),
                                                   parent(),
                                                   this));
}

// =======================================================================
// function : GetAction
// purpose :
// =======================================================================
QAction* MessageModel_Actions::GetAction(const MessageModel_ActionType& theType)
{
  return myActions.contains(theType) ? myActions[theType] : 0;
}

// =======================================================================
// function : AddMenuActions
// purpose :
// =======================================================================
void MessageModel_Actions::AddMenuActions(const QModelIndexList& theSelectedIndices, QMenu* theMenu)
{
  MessageModel_ItemRootPtr   aRootItem;
  MessageModel_ItemReportPtr aReportItem;
  MessageModel_ItemAlertPtr  anAlertItem;
  for (QModelIndexList::const_iterator aSelIt = theSelectedIndices.begin();
       aSelIt != theSelectedIndices.end();
       aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() != 0)
      continue;

    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex(anIndex);
    if (!anItemBase)
      continue;

    aRootItem = itemDynamicCast<MessageModel_ItemRoot>(anItemBase);
    if (aRootItem)
      break;

    aReportItem = itemDynamicCast<MessageModel_ItemReport>(anItemBase);
    if (aReportItem)
      break;

    anAlertItem = itemDynamicCast<MessageModel_ItemAlert>(anItemBase);
    if (anAlertItem)
      break;
  }

  if (aReportItem && !aReportItem->GetReport().IsNull())
  {
    theMenu->addAction(myActions[MessageModel_ActionType_Deactivate]);
    theMenu->addAction(myActions[MessageModel_ActionType_Activate]);
    theMenu->addAction(myActions[MessageModel_ActionType_Clear]);
  }
  else if (anAlertItem)
  {
    theMenu->addAction(myActions[MessageModel_ActionType_ExportToShapeView]);
  }

  theMenu->addSeparator();
}

// =======================================================================
// function : getSelectedReport
// purpose :
// =======================================================================
Handle(Message_Report) MessageModel_Actions::getSelectedReport(QModelIndex& theReportIndex) const
{
  MessageModel_ItemReportPtr aReportItem;
  QModelIndexList            aSelectedIndices = mySelectionModel->selectedIndexes();
  for (QModelIndexList::const_iterator aSelIt = aSelectedIndices.begin();
       aSelIt != aSelectedIndices.end();
       aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() != 0)
      continue;

    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex(anIndex);
    if (!anItemBase)
      continue;

    aReportItem    = itemDynamicCast<MessageModel_ItemReport>(anItemBase);
    theReportIndex = anIndex;
    if (aReportItem)
      break;
  }
  if (!aReportItem)
    return NULL;

  return aReportItem->GetReport();
}

// =======================================================================
// function : OnActivateReport
// purpose :
// =======================================================================
static Handle(Message_PrinterToReport) MyPrinterToReport;
static Message_SequenceOfPrinters      MyDeactivatedPrinters;

void MessageModel_Actions::OnActivateReport()
{
  if (MyPrinterToReport.IsNull())
    MyPrinterToReport = new Message_PrinterToReport();

  if (MyPrinterToReport->Report()->IsActiveInMessenger())
    return;

  MyDeactivatedPrinters = Message::DefaultMessenger()->Printers();
  Message::DefaultMessenger()->ChangePrinters().Clear();

  Message::DefaultMessenger()->AddPrinter(MyPrinterToReport);
  Message::DefaultReport()->UpdateActiveInMessenger();

  myTreeModel->UpdateTreeModel();
}

// =======================================================================
// function : OnDeactivateReport
// purpose :
// =======================================================================
void MessageModel_Actions::OnDeactivateReport()
{
  if (MyPrinterToReport.IsNull() || !MyPrinterToReport->Report()->IsActiveInMessenger())
    return;

  Message::DefaultMessenger()->RemovePrinter(MyPrinterToReport);
  Message::DefaultMessenger()->ChangePrinters().Assign(MyDeactivatedPrinters);

  myTreeModel->UpdateTreeModel();
}

// =======================================================================
// function : OnClearReport
// purpose :
// =======================================================================
void MessageModel_Actions::OnClearReport()
{
  QModelIndex            aReportIndex;
  Handle(Message_Report) aReport = getSelectedReport(aReportIndex);
  if (aReport.IsNull())
    return;

  aReport->Clear();
  myTreeModel->UpdateTreeModel();
}

// =======================================================================
// function : OnExportToShapeView
// purpose :
// =======================================================================
void MessageModel_Actions::OnExportToShapeView()
{
  TCollection_AsciiString aPluginName("TKShapeView");

  NCollection_List<Handle(Standard_Transient)> aPluginParameters;
  if (myParameters->FindParameters(aPluginName))
    aPluginParameters = myParameters->Parameters(aPluginName);
  NCollection_List<TCollection_AsciiString> anItemNames;
  if (myParameters->FindSelectedNames(aPluginName))
    anItemNames = myParameters->GetSelectedNames(aPluginName);

  QModelIndexList aSelectedIndices = mySelectionModel->selectedIndexes();
  QStringList     anExportedPointers;
  for (QModelIndexList::const_iterator aSelIt = aSelectedIndices.begin();
       aSelIt != aSelectedIndices.end();
       aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() != 0)
      continue;

    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex(anIndex);
    if (!anItemBase)
      continue;

    MessageModel_ItemAlertPtr anAlertItem = itemDynamicCast<MessageModel_ItemAlert>(anItemBase);
    if (!anAlertItem)
      continue;

    Handle(Message_Alert) anAlert = anAlertItem->GetAlert();
    if (anAlert.IsNull())
      continue;

    Handle(Message_AlertExtended) anExtAlert = Handle(Message_AlertExtended)::DownCast(anAlert);
    if (anExtAlert.IsNull())
      continue;

    Handle(Message_Attribute) anAttribute = anExtAlert->Attribute();
    if (anAttribute.IsNull())
      continue;

    if (!anAttribute->IsKind(STANDARD_TYPE(TopoDS_AlertAttribute)))
      continue;

    const TopoDS_Shape aShape = Handle(TopoDS_AlertAttribute)::DownCast(anAttribute)->GetShape();
    if (aShape.IsNull())
      continue;
    aPluginParameters.Append(aShape.TShape());
    anItemNames.Append(TInspectorAPI_PluginParameters::ParametersToString(aShape));

    anExportedPointers.append(Standard_Dump::GetPointerInfo(aShape.TShape(), true).ToCString());
  }

  if (anExportedPointers.empty())
    return;
  myParameters->SetSelectedNames(aPluginName, anItemNames);
  myParameters->SetParameters(aPluginName, aPluginParameters);
  QMessageBox::information(0,
                           "Information",
                           QString("TShapes '%1' are sent to %2 tool.")
                             .arg(anExportedPointers.join(", "))
                             .arg(QString(aPluginName.ToCString())));
}
