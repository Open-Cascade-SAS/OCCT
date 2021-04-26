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

#include <inspector/MessageView_Window.hxx>
#include <inspector/MessageView_VisibilityState.hxx>
#include <inspector/MessageView_ActionsTest.hxx>

#include <inspector/MessageModel_Actions.hxx>
#include <inspector/MessageModel_ItemAlert.hxx>
#include <inspector/MessageModel_ItemReport.hxx>
#include <inspector/MessageModel_ItemRoot.hxx>
#include <inspector/MessageModel_TreeModel.hxx>
#include <inspector/MessageView_MetricStatisticModel.hxx>

#include <inspector/TreeModel_Tools.hxx>

#include <inspector/ViewControl_PropertyView.hxx>
#include <inspector/ViewControl_TableModelValues.hxx>
#include <inspector/ViewControl_TreeView.hxx>
#include <inspector/Convert_Tools.hxx>

#include <inspector/View_Viewer.hxx>
#include <inspector/View_Widget.hxx>

#include <AIS_Shape.hxx>
#include <Graphic3d_Camera.hxx>
#include <Message.hxx>
#include <TCollection_AsciiString.hxx>

#include <inspector/ViewControl_Tools.hxx>
#include <inspector/View_Displayer.hxx>
#include <inspector/View_ToolBar.hxx>
#include <inspector/View_Widget.hxx>
#include <inspector/View_Window.hxx>
#include <inspector/View_Viewer.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QAction>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QLayout>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QTextStream>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

const int DEFAULT_SHAPE_VIEW_WIDTH = 400;
const int DEFAULT_SHAPE_VIEW_HEIGHT = 450;
const int DEFAULT_SHAPE_VIEW_POSITION_X = 60;
const int DEFAULT_SHAPE_VIEW_POSITION_Y = 60;

const int MESSAGEVIEW_DEFAULT_TREE_VIEW_WIDTH = 950;
const int MESSAGEVIEW_DEFAULT_TREE_VIEW_HEIGHT = 500;

const int MESSAGEVIEW_DEFAULT_VIEW_WIDTH = 200;
const int MESSAGEVIEW_DEFAULT_VIEW_HEIGHT = 300;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
MessageView_Window::MessageView_Window (QWidget* theParent)
: QObject (theParent)
{
  myMainWindow = new QMainWindow (theParent);

  myTreeView = new ViewControl_TreeView (myMainWindow);
  ((ViewControl_TreeView*)myTreeView)->SetPredefinedSize (QSize (MESSAGEVIEW_DEFAULT_TREE_VIEW_WIDTH,
                                                                 MESSAGEVIEW_DEFAULT_TREE_VIEW_HEIGHT));
  MessageModel_TreeModel* aModel = new MessageModel_TreeModel (myTreeView);
  aModel->InitColumns();

  connect (myTreeView->header(), SIGNAL (sectionResized (int, int, int)),
           this, SLOT(onHeaderResized (int, int, int)));

  myTreeView->setModel (aModel);
  MessageView_VisibilityState* aVisibilityState = new MessageView_VisibilityState (aModel);
  aModel->SetVisibilityState (aVisibilityState);
  connect (aVisibilityState, SIGNAL (itemClicked (const QModelIndex&)),
           this, SLOT(onTreeViewVisibilityClicked(const QModelIndex&)));

  TreeModel_Tools::UseVisibilityColumn (myTreeView);
  TreeModel_Tools::SetDefaultHeaderSections (myTreeView);

  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (aModel);
  myTreeView->setSelectionMode (QAbstractItemView::ExtendedSelection);
  myTreeView->setSelectionModel (aSelectionModel);
  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           this, SLOT (onTreeViewSelectionChanged (const QItemSelection&, const QItemSelection&)));

  myTreeViewActions = new MessageModel_Actions (myMainWindow, aModel, aSelectionModel);
  myTestViewActions = new MessageView_ActionsTest (myMainWindow, aModel, aSelectionModel);

  myTreeView->setContextMenuPolicy (Qt::CustomContextMenu);
  connect (myTreeView, SIGNAL (customContextMenuRequested (const QPoint&)),
          this, SLOT (onTreeViewContextMenuRequested (const QPoint&)));

  connect (myTreeView->header(), SIGNAL (sectionResized (int, int, int)),
           this, SLOT(onHeaderResized (int, int, int)));

  QModelIndex aParentIndex = myTreeView->model()->index (0, 0);
  myTreeView->setExpanded (aParentIndex, true);

  myMainWindow->setCentralWidget (myTreeView);

  // property view
  myPropertyView = new ViewControl_PropertyView (myMainWindow);
  myPropertyPanelWidget = new QDockWidget (tr ("PropertyPanel"), myMainWindow);
  myPropertyPanelWidget->setObjectName (myPropertyPanelWidget->windowTitle());
  myPropertyPanelWidget->setTitleBarWidget (new QWidget(myMainWindow));
  myPropertyPanelWidget->setWidget (myPropertyView->GetControl());
  myMainWindow->addDockWidget (Qt::RightDockWidgetArea, myPropertyPanelWidget);
  connect (myPropertyPanelWidget->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT (onPropertyPanelShown (bool)));
  connect (myPropertyView, SIGNAL (propertyViewDataChanged()), this, SLOT (onPropertyViewDataChanged()));

  myCustomView = new QTableView (myMainWindow);
  myCustomPanelWidget = new QDockWidget (tr ("PropertyPanel (custom)"), myMainWindow);
  myCustomPanelWidget->setObjectName (myCustomPanelWidget->windowTitle());
  myCustomPanelWidget->setTitleBarWidget (new QWidget(myMainWindow));
  myCustomPanelWidget->setWidget (myCustomView);
  myMainWindow->addDockWidget (Qt::RightDockWidgetArea, myCustomPanelWidget);

  // view
  myViewWindow = new View_Window (myMainWindow, NULL, false);
  connect (myViewWindow, SIGNAL(eraseAllPerformed()), this, SLOT(onEraseAllPerformed()));
  aVisibilityState->SetDisplayer (myViewWindow->Displayer());
  aVisibilityState->SetPresentationType (View_PresentationType_Main);
  myViewWindow->ViewWidget()->SetPredefinedSize (MESSAGEVIEW_DEFAULT_VIEW_WIDTH, MESSAGEVIEW_DEFAULT_VIEW_HEIGHT);

  myViewDockWidget = new QDockWidget (tr ("View"), myMainWindow);
  myViewDockWidget->setObjectName (myViewDockWidget->windowTitle());
  myViewDockWidget->setWidget (myViewWindow);
  myMainWindow->addDockWidget (Qt::RightDockWidgetArea, myViewDockWidget);

  myMainWindow->tabifyDockWidget (myCustomPanelWidget, myViewDockWidget);

  myMainWindow->resize (DEFAULT_SHAPE_VIEW_WIDTH, DEFAULT_SHAPE_VIEW_HEIGHT);
  myMainWindow->move (DEFAULT_SHAPE_VIEW_POSITION_X, DEFAULT_SHAPE_VIEW_POSITION_Y);

  updateVisibleColumns();
}

// =======================================================================
// function : SetParent
// purpose :
// =======================================================================
void MessageView_Window::SetParent (void* theParent)
{
  QWidget* aParent = (QWidget*)theParent;
  if (aParent)
  {
    QLayout* aLayout = aParent->layout();
    if (aLayout)
      aLayout->addWidget (GetMainWindow());
  }
  else
  {
    GetMainWindow()->setParent (0);
    GetMainWindow()->setVisible (true);
  }
}

// =======================================================================
// function : FillActionsMenu
// purpose :
// =======================================================================
void MessageView_Window::FillActionsMenu (void* theMenu)
{
  QMenu* aMenu = (QMenu*)theMenu;
  QList<QDockWidget*> aDockwidgets = myMainWindow->findChildren<QDockWidget*>();
  for (QList<QDockWidget*>::iterator it = aDockwidgets.begin(); it != aDockwidgets.end(); ++it)
  {
    QDockWidget* aDockWidget = *it;
    if (aDockWidget->parentWidget() == myMainWindow)
      aMenu->addAction (aDockWidget->toggleViewAction());
  }
}

// =======================================================================
// function : GetPreferences
// purpose :
// =======================================================================
void MessageView_Window::GetPreferences (TInspectorAPI_PreferencesDataMap& theItem)
{
  theItem.Clear();
  theItem.Bind ("geometry",  TreeModel_Tools::ToString (myMainWindow->saveState()).toStdString().c_str());

  QMap<QString, QString> anItems;
  TreeModel_Tools::SaveState (myTreeView, anItems);
  for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
  {
    theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());
  }

  anItems.clear();
  View_Window::SaveState(myViewWindow, anItems);
  for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
  {
    theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());
  }
}

// =======================================================================
// function : SetPreferences
// purpose :
// =======================================================================
void MessageView_Window::SetPreferences (const TInspectorAPI_PreferencesDataMap& theItem)
{
  for (TInspectorAPI_IteratorOfPreferencesDataMap anItemIt (theItem); anItemIt.More(); anItemIt.Next())
  {
    if (anItemIt.Key().IsEqual ("geometry"))
      myMainWindow->restoreState (TreeModel_Tools::ToByteArray (anItemIt.Value().ToCString()));
    else if (TreeModel_Tools::RestoreState (myTreeView, anItemIt.Key().ToCString(), anItemIt.Value().ToCString()))
      continue;
    else if (myViewWindow && View_Window::RestoreState(myViewWindow, anItemIt.Key().ToCString(), anItemIt.Value().ToCString()))
      continue;
  }
}

// =======================================================================
// function : UpdateContent
// purpose :
// =======================================================================
void MessageView_Window::UpdateContent()
{
  TCollection_AsciiString aName = "TKMessageView";
  if (myParameters->FindParameters (aName))
  {
    NCollection_List<Handle(Standard_Transient)> aParameters = myParameters->Parameters (aName);
    // Init will remove from parameters those, that are processed only one time (TShape)
    Init (aParameters);
    myParameters->SetParameters (aName, aParameters);
  }
  Handle(Message_Report) aDefaultReport = Message::DefaultReport();
  MessageModel_TreeModel* aViewModel = dynamic_cast<MessageModel_TreeModel*> (myTreeView->model());
  if (!aDefaultReport.IsNull() && !aViewModel->HasReport (aDefaultReport))
  {
    addReport (aDefaultReport);
  }

  updateTreeModel();
  updateVisibleColumns();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void MessageView_Window::Init (NCollection_List<Handle(Standard_Transient)>& theParameters)
{
  Handle(AIS_InteractiveContext) aContext;
  NCollection_List<Handle(Standard_Transient)> aParameters;

  Handle(Graphic3d_Camera) aViewCamera;

  for (NCollection_List<Handle(Standard_Transient)>::Iterator aParamsIt (theParameters);
       aParamsIt.More(); aParamsIt.Next())
  {
    Handle(Standard_Transient) anObject = aParamsIt.Value();
    Handle(Message_Report) aMessageReport = Handle(Message_Report)::DownCast (anObject);
    if (!aMessageReport.IsNull())
    {
      addReport (aMessageReport);
    }
    else if (!Handle(AIS_InteractiveContext)::DownCast (anObject).IsNull())
    {
      aParameters.Append (anObject);
      if (aContext.IsNull())
        aContext = Handle(AIS_InteractiveContext)::DownCast (anObject);
    }
    else if (!Handle(Graphic3d_Camera)::DownCast (anObject).IsNull())
    {
      aViewCamera = Handle(Graphic3d_Camera)::DownCast (anObject);
    }
  }
  MessageModel_TreeModel* aTreeModel = dynamic_cast<MessageModel_TreeModel*> (myTreeView->model());
  if (!aTreeModel)
    return;

  aTreeModel->EmitLayoutChanged();

  if (!aContext.IsNull())
  {
    myViewWindow->SetContext (View_ContextType_External, aContext);
  }
  theParameters = aParameters;
}

// =======================================================================
// function : updateTreeModel
// purpose :
// =======================================================================
void MessageView_Window::updateTreeModel()
{
  MessageModel_TreeModel* aViewModel = dynamic_cast<MessageModel_TreeModel*> (myTreeView->model());
  if (!aViewModel)
    return;

  aViewModel->UpdateTreeModel();
}

// =======================================================================
// function : addReport
// purpose :
// =======================================================================
void MessageView_Window::addReport (const Handle(Message_Report)& theReport,
                                    const TCollection_AsciiString& theReportDescription)
{
  MessageModel_TreeModel* aModel = dynamic_cast<MessageModel_TreeModel*> (myTreeView->model());
  aModel->AddReport (theReport, theReportDescription);
}

// =======================================================================
// function : onTreeViewVisibilityClicked
// purpose :
// =======================================================================
void MessageView_Window::onTreeViewVisibilityClicked(const QModelIndex& theIndex)
{
  MessageModel_TreeModel* aTreeModel = dynamic_cast<MessageModel_TreeModel*> (myTreeView->model());
  TreeModel_VisibilityState* aVisibilityState = aTreeModel->GetVisibilityState();
  if (!aVisibilityState->IsVisible (theIndex))
    myPropertyView->ClearActiveTablesSelection();
}

// =======================================================================
// function : onTreeViewSelectionChanged
// purpose :
// =======================================================================
void MessageView_Window::onTreeViewSelectionChanged (const QItemSelection&, const QItemSelection&)
{
  if (!myPropertyPanelWidget->toggleViewAction()->isChecked())
    return;

  updatePropertyPanelBySelection();
  updatePreviewPresentation();
}

// =======================================================================
// function : onTreeViewContextMenuRequested
// purpose :
// =======================================================================
void MessageView_Window::onTreeViewContextMenuRequested (const QPoint& thePosition)
{
  QMenu* aMenu = new QMenu (GetMainWindow());

  MessageModel_ItemRootPtr aRootItem;
  MessageModel_ItemReportPtr aReportItem;
  QModelIndexList aSelectedIndices = myTreeView->selectionModel()->selectedIndexes();

  for (QModelIndexList::const_iterator aSelIt = aSelectedIndices.begin(); aSelIt != aSelectedIndices.end(); aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() != 0)
      continue;

    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    if (!anItemBase)
      continue;

    aRootItem = itemDynamicCast<MessageModel_ItemRoot> (anItemBase);
    if (aRootItem)
      break;
    aReportItem = itemDynamicCast<MessageModel_ItemReport> (anItemBase);
    if (aReportItem)
      break;
  }
  if (aRootItem)
  {
    aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Create Default Report"),
                      SLOT (onCreateDefaultReport()), myMainWindow, this));
  }
  else if (aReportItem)
  {
    aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Export Report"), SLOT (onExportReport()), myMainWindow, this));

    QAction* anAction = ViewControl_Tools::CreateAction (tr ("WallClock Metric statistic"),
      SLOT (onMetricStatistic()), myMainWindow, this);
    anAction->setCheckable (true);
    aMenu->addAction (anAction);
  }
  aMenu->addSeparator();

  aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Preview children presentations"), SLOT (onPreviewChildren()), myMainWindow, this));
  aMenu->addSeparator();

  myTreeViewActions->AddMenuActions (aSelectedIndices, aMenu);
  addActivateMetricActions (aMenu);

  aMenu->addSeparator();
  myTestViewActions->AddMenuActions (aSelectedIndices, aMenu);

  QPoint aPoint = myTreeView->mapToGlobal (thePosition);
  aMenu->exec (aPoint);
}

// =======================================================================
// function : onPropertyPanelShown
// purpose :
// =======================================================================
void MessageView_Window::onPropertyPanelShown (bool isToggled)
{
  if (!isToggled)
    return;

  updatePropertyPanelBySelection();
}

// =======================================================================
// function : onPropertyViewDataChanged
// purpose :
// =======================================================================
void MessageView_Window::onPropertyViewDataChanged()
{
  QItemSelectionModel* aModel = myTreeView->selectionModel();
  if (!aModel)
    return;
  QModelIndex anIndex = TreeModel_ModelBase::SingleSelected (aModel->selectedIndexes(), 0);
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
  if (!anItemBase)
    return;

  updatePropertyPanelBySelection();
  updatePreviewPresentation();
}

// =======================================================================
// function : onHeaderResized
// purpose :
// =======================================================================
void MessageView_Window::onHeaderResized (int theSectionId, int, int)
{
  TreeModel_ModelBase* aViewModel = dynamic_cast<TreeModel_ModelBase*> (myTreeView->model());

  TreeModel_HeaderSection* aSection = aViewModel->ChangeHeaderItem (theSectionId);
  aSection->SetWidth (myTreeView->columnWidth (theSectionId));
}

// =======================================================================
// function : onEraseAllPerformed
// purpose :
// =======================================================================
void MessageView_Window::onEraseAllPerformed()
{
  MessageModel_TreeModel* aTreeModel = dynamic_cast<MessageModel_TreeModel*> (myTreeView->model());

  aTreeModel->Reset();
  aTreeModel->EmitLayoutChanged();
}

// =======================================================================
// function : onExportReport
// purpose :
// =======================================================================
void MessageView_Window::onExportReport()
{
  QItemSelectionModel* aModel = myTreeView->selectionModel();
  if (!aModel)
    return;
  QModelIndex anIndex = TreeModel_ModelBase::SingleSelected (aModel->selectedIndexes(), 0);
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
  if (!anItemBase)
    return;
  MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport>(anItemBase);
  if (!aReportItem)
    return;

  QString aFilter (tr ("Document file (*.json *)"));
  QString aSelectedFilter;
  QString aFileName = QFileDialog::getSaveFileName (0, tr ("Export report to file"), QString(), aFilter, &aSelectedFilter);

  Handle(Message_Report) aReport = aReportItem->GetReport();
  Standard_SStream aStream;
  aReport->DumpJson(aStream);

  QFile aLogFile(aFileName);
  if (!aLogFile.open(QFile::WriteOnly | QFile::Text))
  {
    return;
  }
  QTextStream anOut( &aLogFile );
  anOut << Standard_Dump::FormatJson (aStream).ToCString();
  aLogFile.close();
}

// =======================================================================
// function : onCreateDefaultReport
// purpose :
// =======================================================================
void MessageView_Window::onCreateDefaultReport()
{
  if (!Message::DefaultReport().IsNull())
  {
    return;
  }

  addReport (Message::DefaultReport (Standard_True));
}

// =======================================================================
// function : onPreviewChildren
// purpose :
// =======================================================================
void MessageView_Window::onPreviewChildren()
{
  QItemSelectionModel* aModel = myTreeView->selectionModel();
  if (!aModel)
    return;

  QModelIndexList aSelectedIndices = myTreeView->selectionModel()->selectedIndexes();
  NCollection_List<Handle(Standard_Transient)> aPresentations;
  TreeModel_ModelBase::SubItemsPresentations (aSelectedIndices, aPresentations);

  displayer()->UpdatePreview (View_DisplayActionType_DisplayId, aPresentations);
}

// =======================================================================
// function : onMetricStatistic
// purpose :
// =======================================================================
void MessageView_Window::onMetricStatistic()
{
  QItemSelectionModel* aModel = myTreeView->selectionModel();
  if (!aModel)
  {
    return;
  }

  QModelIndex anIndex = TreeModel_ModelBase::SingleSelected (aModel->selectedIndexes(), 0);
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
  if (!anItemBase)
  {
    return;
  }

  MessageView_MetricStatisticModel* aUnitByNameModel = new MessageView_MetricStatisticModel (Message_MetricType_WallClock, myCustomView);
  aUnitByNameModel->Init (anItemBase);
  myCustomView->setModel (aUnitByNameModel);
}

// =======================================================================
// function : addActivateMetricActions
// purpose :
// =======================================================================
void MessageView_Window::addActivateMetricActions (QMenu* theMenu)
{
  Handle(Message_Report) aReport = Message::DefaultReport();
  if (aReport.IsNull())
  {
    return;
  }

  QMenu* aSubMenu = new QMenu ("Activate metric");
  for (int aMetricId = (int)Message_MetricType_None + 1; aMetricId <= (int)Message_MetricType_MemHeapUsage; aMetricId++)
  {
    Message_MetricType aMetricType = (Message_MetricType)aMetricId;
    QAction* anAction = ViewControl_Tools::CreateAction (Message::MetricToString (aMetricType),
      SLOT (OnActivateMetric()), parent(), this);
    anAction->setCheckable (true);
    anAction->setChecked (aReport->ActiveMetrics().Contains (aMetricType));
    aSubMenu->addAction (anAction);
  }
  aSubMenu->addSeparator();
  aSubMenu->addAction (ViewControl_Tools::CreateAction ("Deactivate all", SLOT (OnDeactivateAllMetrics()), parent(), this));

  theMenu->addMenu (aSubMenu);
}

// =======================================================================
// function : OnActivateMetric
// purpose :
// =======================================================================
void MessageView_Window::OnActivateMetric()
{
  QAction* anAction = (QAction*)(sender());

  Message_MetricType aMetricType;
  if (!Message::MetricFromString (anAction->text().toStdString().c_str(), aMetricType))
    return;

  Handle(Message_Report) aReport = Message::DefaultReport();
  const NCollection_IndexedMap<Message_MetricType>& anActiveMetrics = aReport->ActiveMetrics();
  aReport->SetActiveMetric (aMetricType, !anActiveMetrics.Contains (aMetricType));

  updateVisibleColumns();
}

// =======================================================================
// function : OnDeactivateAllMetrics
// purpose :
// =======================================================================
void MessageView_Window::OnDeactivateAllMetrics()
{
  Handle(Message_Report) aReport = Message::DefaultReport();
  if (aReport.IsNull())
    return;
  aReport->ClearMetrics();

  updateVisibleColumns();
}

// =======================================================================
// function : displayer
// purpose :
// =======================================================================
View_Displayer* MessageView_Window::displayer()
{
  return myViewWindow->Displayer();
}

// =======================================================================
// function : updatePropertyPanelBySelection
// purpose :
// =======================================================================
void MessageView_Window::updatePropertyPanelBySelection()
{
  ViewControl_TableModelValues* aTableValues = 0;

  QItemSelectionModel* aModel = myTreeView->selectionModel();
  if (!aModel)
    return;

  QModelIndex anIndex = TreeModel_ModelBase::SingleSelected (aModel->selectedIndexes(), 0);
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
  if (anItemBase)
  {
    Handle(TreeModel_ItemProperties) anItemProperties = anItemBase->Properties ();
    if (!anItemProperties.IsNull())
    {
      aTableValues = new ViewControl_TableModelValues();
      aTableValues->SetProperties (anItemProperties);
    }
  }
  myPropertyView->Init (aTableValues);
}

// =======================================================================
// function : updatePreviewPresentation
// purpose :
// =======================================================================
void MessageView_Window::updatePreviewPresentation()
{
  Handle(AIS_InteractiveContext) aContext = myViewWindow->ViewToolBar()->CurrentContext();
  if (aContext.IsNull())
    return;

  NCollection_List<Handle(Standard_Transient)> aPresentations;
  QModelIndexList aSelectedIndices = myTreeView->selectionModel()->selectedIndexes();
  for (QModelIndexList::const_iterator aSelIt = aSelectedIndices.begin(); aSelIt != aSelectedIndices.end(); aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() != 0)
      continue;

    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    if (!anItemBase)
      continue;

    anItemBase->Presentations (aPresentations);
  }

  displayer()->UpdatePreview (View_DisplayActionType_DisplayId, aPresentations);
}

// =======================================================================
// function : updateVisibleColumns
// purpose :
// =======================================================================
void MessageView_Window::updateVisibleColumns()
{
  MessageModel_TreeModel* aViewModel = dynamic_cast<MessageModel_TreeModel*> (myTreeView->model());

  NCollection_IndexedMap<Message_MetricType> anActiveMetrics;
  for (NCollection_List<MessageModel_ReportInformation>::Iterator anIterator (aViewModel->Reports()); anIterator.More(); anIterator.Next())
  {
    Handle(Message_Report) aReport = anIterator.Value().myReport;
    for (NCollection_IndexedMap<Message_MetricType>::Iterator aMetricsIterator (aReport->ActiveMetrics()); aMetricsIterator.More(); aMetricsIterator.Next())
    {
      if (anActiveMetrics.Contains (aMetricsIterator.Value()))
        continue;
      anActiveMetrics.Add (aMetricsIterator.Value());
    }
  }

  for (int aMetricId = (int)Message_MetricType_None + 1; aMetricId <= (int)Message_MetricType_MemHeapUsage; aMetricId++)
  {
    Message_MetricType aMetricType = (Message_MetricType)aMetricId;
    QList<int> aMetricColumns;
    aViewModel->GetMetricColumns (aMetricType, aMetricColumns);
    bool isColumnHidden = !anActiveMetrics.Contains (aMetricType);
    for (int i = 0; i < aMetricColumns.size(); i++)
    {
      int aColumnId = aMetricColumns[i];
      myTreeView->setColumnHidden (aColumnId, isColumnHidden);
      TreeModel_HeaderSection* aSection = aViewModel->ChangeHeaderItem (aColumnId);
      aSection->SetIsHidden (isColumnHidden);
    }
  }
}
