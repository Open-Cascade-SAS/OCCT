// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
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

#include <inspector/VInspector_Window.hxx>

#include <AIS_Shape.hxx>

#include <inspector/TreeModel_ColumnType.hxx>
#include <inspector/TreeModel_ContextMenu.hxx>
#include <inspector/TreeModel_Tools.hxx>

#include <inspector/ViewControl_MessageDialog.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <inspector/VInspector_ItemPresentableObject.hxx>
#include <inspector/VInspector_ToolBar.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/VInspector_ViewModel.hxx>
#include <inspector/VInspector_CallBack.hxx>
#include <inspector/VInspector_Communicator.hxx>
#include <inspector/VInspector_ItemEntityOwner.hxx>
#include <inspector/VInspector_ItemPresentableObject.hxx>
#include <inspector/VInspector_ToolBar.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/VInspector_ViewModel.hxx>
#include <inspector/VInspector_ViewModelHistory.hxx>

#include <inspector/ViewControl_TreeView.hxx>

#include <inspector/View_Widget.hxx>
#include <inspector/View_Window.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QDockWidget>
#include <QHeaderView>
#include <QGridLayout>
#include <QItemSelectionModel>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QToolButton>
#include <QTreeView>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

const int VINSPECTOR_DEFAULT_WIDTH  = 1250;
const int VINSPECTOR_DEFAULT_HEIGHT = 800;

const int VINSPECTOR_DEFAULT_POSITION_X = 200;
const int VINSPECTOR_DEFAULT_POSITION_Y = 60;

const int VINSPECTOR_DEFAULT_VIEW_WIDTH = 400;
const int VINSPECTOR_DEFAULT_VIEW_HEIGHT = 1000;

const int VINSPECTOR_DEFAULT_HISTORY_VIEW_WIDTH = 400;
const int VINSPECTOR_DEFAULT_HISTORY_VIEW_HEIGHT = 50;

const int VINSPECTOR_DEFAULT_VIEW_POSITION_X = 200 + 900 + 100; // TINSPECTOR_DEFAULT_POSITION_X + TINSPECTOR_DEFAULT_WIDTH + 100
const int VINSPECTOR_DEFAULT_VIEW_POSITION_Y = 60; // TINSPECTOR_DEFAULT_POSITION_Y + 50

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_Window::VInspector_Window()
: myParent (0), myExportToShapeViewDialog (0), myViewWindow (0)
{
  myMainWindow = new QMainWindow (0);

  QWidget* aCentralWidget = new QWidget (myMainWindow);
  QGridLayout* aParentLay = new QGridLayout (aCentralWidget);
  aParentLay->setContentsMargins (0, 0, 0, 0);
  aParentLay->setSpacing(0);

  // restore state of tool bar: on the bottom of the window
  myToolBar = new VInspector_ToolBar(aCentralWidget);
  connect (myToolBar, SIGNAL (actionClicked (int)), this, SLOT (onToolBarActionClicked (int)));
  aParentLay->addWidget (myToolBar->GetControl(), 0, 0);

  // tree view
  myTreeView = new QTreeView (aCentralWidget);
  myTreeView->setSelectionBehavior (QAbstractItemView::SelectRows);
  myTreeView->setSelectionMode (QAbstractItemView::ExtendedSelection);
  myTreeView->header()->setStretchLastSection (true);
  myTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
  VInspector_ViewModel* aTreeModel = new VInspector_ViewModel (myTreeView);
  myTreeView->setModel (aTreeModel);
  // hide Visibility column
  TreeModel_HeaderSection anItem = aTreeModel->GetHeaderItem ((int)TreeModel_ColumnType_Visibility);
  anItem.SetIsHidden (true);
  aTreeModel->SetHeaderItem ((int)TreeModel_ColumnType_Visibility, anItem);

  connect (myTreeView, SIGNAL(customContextMenuRequested(const QPoint&)),
           this, SLOT (onTreeViewContextMenuRequested(const QPoint&)));
  new TreeModel_ContextMenu (myTreeView);

  QItemSelectionModel* aSelModel = new QItemSelectionModel (myTreeView->model(), myTreeView);
  myTreeView->setSelectionModel (aSelModel);
  connect (aSelModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           this, SLOT (onSelectionChanged (const QItemSelection&, const QItemSelection&)));

  aParentLay->addWidget(myTreeView, 1, 0);
  aParentLay->setRowStretch (1, 1);
  myMainWindow->setCentralWidget (aCentralWidget);

  myHistoryView = new ViewControl_TreeView (myMainWindow);
  myHistoryView->setSelectionBehavior (QAbstractItemView::SelectRows);
  ((ViewControl_TreeView*)myHistoryView)->SetPredefinedSize (QSize (VINSPECTOR_DEFAULT_HISTORY_VIEW_WIDTH,
                                                                    VINSPECTOR_DEFAULT_HISTORY_VIEW_HEIGHT));
  myHistoryView->setContextMenuPolicy (Qt::CustomContextMenu);
  myHistoryView->header()->setStretchLastSection (true);
  new TreeModel_ContextMenu (myHistoryView);

  myHistoryView->setSelectionMode (QAbstractItemView::ExtendedSelection);
  VInspector_ViewModelHistory* aHistoryModel = new VInspector_ViewModelHistory (myHistoryView);
  myHistoryView->setModel (aHistoryModel);

  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (aHistoryModel);
  myHistoryView->setSelectionModel (aSelectionModel);
  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
    this, SLOT (onHistoryViewSelectionChanged (const QItemSelection&, const QItemSelection&)));

  anItem = aHistoryModel->GetHeaderItem (0);
  TreeModel_Tools::UseVisibilityColumn (myHistoryView, false);
  // hide Visibility column
  anItem = aHistoryModel->GetHeaderItem ((int)TreeModel_ColumnType_Visibility);
  anItem.SetIsHidden (true);
  aHistoryModel->SetHeaderItem ((int)TreeModel_ColumnType_Visibility, anItem);

  QModelIndex aParentIndex = myHistoryView->model()->index (0, 0);
  myHistoryView->setExpanded (aParentIndex, true);

  QDockWidget* aHistoryDockWidget = new QDockWidget (tr ("HistoryView"), myMainWindow);
  aHistoryDockWidget->setObjectName (aHistoryDockWidget->windowTitle());
  aHistoryDockWidget->setTitleBarWidget (new QWidget(myMainWindow));
  aHistoryDockWidget->setWidget (myHistoryView);
  myMainWindow->addDockWidget (Qt::BottomDockWidgetArea, aHistoryDockWidget);

  myMainWindow->resize (450, 800);
  myMainWindow->move (60, 20);

  myMainWindow->resize (VINSPECTOR_DEFAULT_WIDTH, VINSPECTOR_DEFAULT_HEIGHT);
  myMainWindow->move (VINSPECTOR_DEFAULT_POSITION_X, VINSPECTOR_DEFAULT_POSITION_Y);
}

// =======================================================================
// function : SetParent
// purpose :
// =======================================================================
void VInspector_Window::SetParent (void* theParent)
{
  myParent = (QWidget*)theParent;
  if (!myParent)
    return;

  QLayout* aLayout = myParent->layout();
  if (aLayout)
    aLayout->addWidget (GetMainWindow());
}

// =======================================================================
// function : FillActionsMenu
// purpose :
// =======================================================================
void VInspector_Window::FillActionsMenu (void* theMenu)
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
void VInspector_Window::GetPreferences (TInspectorAPI_PreferencesDataMap& theItem)
{
  theItem.Bind ("geometry",  TreeModel_Tools::ToString (myMainWindow->saveState()).toStdString().c_str());

  QMap<QString, QString> anItems;
  TreeModel_Tools::SaveState (myTreeView, anItems);
  for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
  {
    theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());
  }

  anItems.clear();
  TreeModel_Tools::SaveState (myHistoryView, anItems, "history_view_");
  for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
    theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());
}

// =======================================================================
// function : SetPreferences
// purpose :
// =======================================================================
void VInspector_Window::SetPreferences (const TInspectorAPI_PreferencesDataMap& theItem)
{
  if (theItem.IsEmpty())
  {
    TreeModel_Tools::SetDefaultHeaderSections (myTreeView);
    TreeModel_Tools::SetDefaultHeaderSections (myHistoryView);
    return;
  }

  for (TInspectorAPI_IteratorOfPreferencesDataMap anItemIt (theItem); anItemIt.More(); anItemIt.Next())
  {
    if (anItemIt.Key().IsEqual ("geometry"))
      myMainWindow->restoreState (TreeModel_Tools::ToByteArray (anItemIt.Value().ToCString()));
    else if (TreeModel_Tools::RestoreState (myTreeView, anItemIt.Key().ToCString(), anItemIt.Value().ToCString()))
      continue;
    else if (TreeModel_Tools::RestoreState (myHistoryView, anItemIt.Key().ToCString(), anItemIt.Value().ToCString(),
                                            "history_view_"))
      continue;
  }
}

// =======================================================================
// function : UpdateContent
// purpose :
// =======================================================================
void VInspector_Window::UpdateContent()
{
  TCollection_AsciiString aName = "TKVInspector";

  bool isModelUpdated = false;
  if (myParameters->FindParameters (aName))
    isModelUpdated = Init (myParameters->Parameters (aName));
  if (myParameters->FindFileNames (aName))
  {
    for (NCollection_List<TCollection_AsciiString>::Iterator aFileNamesIt (myParameters->FileNames (aName));
         aFileNamesIt.More(); aFileNamesIt.Next())
         isModelUpdated = OpenFile (aFileNamesIt.Value()) || isModelUpdated;

    NCollection_List<TCollection_AsciiString> aNames;
    myParameters->SetFileNames (aName, aNames);
  }
  if (!isModelUpdated)
    UpdateTreeModel();

  // make AIS_InteractiveObject selected selected if exist in select parameters
  NCollection_List<Handle(Standard_Transient)> anObjects;
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*>(myTreeView->model());
  if (aViewModel && myParameters->GetSelectedObjects(aName, anObjects))
  {
    QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
    aSelectionModel->clear();
    for (NCollection_List<Handle(Standard_Transient)>::Iterator aParamsIt (anObjects);
         aParamsIt.More(); aParamsIt.Next())
    {
      Handle(Standard_Transient) anObject = aParamsIt.Value();
      Handle(AIS_InteractiveObject) aPresentation = Handle(AIS_InteractiveObject)::DownCast (anObject);
      if (aPresentation.IsNull())
        continue;

      QModelIndex aPresentationIndex = aViewModel->FindIndex (aPresentation);
      if (!aPresentationIndex.isValid())
        continue;
       aSelectionModel->select (aPresentationIndex, QItemSelectionModel::Select);
       myTreeView->scrollTo (aPresentationIndex);
    }
  }
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
bool VInspector_Window::Init (const NCollection_List<Handle(Standard_Transient)>& theParameters)
{
  Handle(AIS_InteractiveContext) aContext;
  Handle(VInspector_CallBack) aCallBack;

  for (NCollection_List<Handle(Standard_Transient)>::Iterator aParamsIt (theParameters); aParamsIt.More(); aParamsIt.Next())
  {
    Handle(Standard_Transient) anObject = aParamsIt.Value();
    if (aContext.IsNull())
      aContext = Handle(AIS_InteractiveContext)::DownCast (anObject);

    if (aCallBack.IsNull())
      aCallBack = Handle(VInspector_CallBack)::DownCast (anObject);
  }
  if (aContext.IsNull())
    return false;
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (aViewModel && aViewModel->GetContext() == aContext)
    UpdateTreeModel();
  else
    SetContext (aContext);

  if (!aCallBack.IsNull() && aCallBack != myCallBack)
  {
    myCallBack = aCallBack;
    VInspector_ViewModelHistory* aHistoryModel = dynamic_cast<VInspector_ViewModelHistory*>
      (myHistoryView->model());
    myCallBack->SetContext(aContext);
    myCallBack->SetHistoryModel(aHistoryModel);
  }
  return true;
}

// =======================================================================
// function : SetContext
// purpose :
// =======================================================================
void VInspector_Window::SetContext (const Handle(AIS_InteractiveContext)& theContext)
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  aViewModel->SetContext (theContext);
  myTreeView->setExpanded (aViewModel->index (0, 0), true);

  if (!myCallBack.IsNull())
    myCallBack->SetContext (theContext);
}

// =======================================================================
// function : OpenFile
// purpose :
// =======================================================================
bool VInspector_Window::OpenFile(const TCollection_AsciiString& theFileName)
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return false;

  Handle(AIS_InteractiveContext) aContext = aViewModel->GetContext();
  bool isModelUpdated = false;
  if (aContext.IsNull())
  {
    aContext = createView();
    SetContext (aContext);
    isModelUpdated = true;
  }

  TopoDS_Shape aShape = VInspector_Tools::ReadShape (theFileName);
  if (aShape.IsNull())
    return isModelUpdated;

  Handle(AIS_Shape) aPresentation = new AIS_Shape (aShape);
  aContext->Display (aPresentation, false);
  aContext->Load (aPresentation, -1/*selection mode*/);
  aContext->UpdateCurrentViewer();

  UpdateTreeModel();
  myTreeView->setExpanded (aViewModel->index (0, 0), true);
  return true;
}

// =======================================================================
// function : onTreeViewContextMenuRequested
// purpose :
// =======================================================================
void VInspector_Window::onTreeViewContextMenuRequested(const QPoint& thePosition)
{
  QMenu* aMenu = new QMenu (GetMainWindow());
  aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Export to ShapeView"), SLOT (onExportToShapeView()), GetMainWindow(), this));
  aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Show"), SLOT (onShow()), GetMainWindow(), this));
  aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Hide"), SLOT (onHide()), GetMainWindow(), this));
  QPoint aPoint = myTreeView->mapToGlobal (thePosition);
  aMenu->exec(aPoint);
}

// =======================================================================
// function : onToolBarActionClicked
// purpose :
// =======================================================================
void VInspector_Window::onToolBarActionClicked (const int theActionId)
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return;

  switch (theActionId)
  {
    case VInspector_ToolActionType_UpdateId:
    {
      UpdateTreeModel();
      break;
    }
    case VInspector_ToolActionType_SelectPresentationsId:
    {
      bool isChecked = myToolBar->GetToolButton((VInspector_ToolActionType)theActionId)->isChecked();
      NCollection_List<Handle(AIS_InteractiveObject)> aPresentationsForViewer;
      if (isChecked)
        aPresentationsForViewer = VInspector_ItemPresentableObject::GetSelectedPresentations(myTreeView->selectionModel());
      Handle(AIS_InteractiveContext) aContext = aViewModel->GetContext();
      VInspector_Tools::AddOrRemovePresentations(aContext, aPresentationsForViewer);
      UpdateTreeModel();
      break;
    }
    case VInspector_ToolActionType_SelectOwnersId:
    {
      NCollection_List<Handle(SelectBasics_EntityOwner)> anOwnersForViewer;
      if (myToolBar->GetToolButton((VInspector_ToolActionType)theActionId)->isChecked())
        VInspector_ViewModel::GetSelectedOwners(myTreeView->selectionModel(), anOwnersForViewer);
      VInspector_Tools::AddOrRemoveSelectedShapes(aViewModel->GetContext(), anOwnersForViewer);
      UpdateTreeModel();
      break;
    }
    default:
      break;
  }
}

// =======================================================================
// function : onSelectionChanged
// purpose :
// =======================================================================
void VInspector_Window::onSelectionChanged (const QItemSelection&,
                                            const QItemSelection&)
{
  QApplication::setOverrideCursor (Qt::WaitCursor);

  if (myToolBar->GetToolButton(VInspector_ToolActionType_SelectPresentationsId)->isChecked())
    onToolBarActionClicked(VInspector_ToolActionType_SelectPresentationsId);
  else if (myToolBar->GetToolButton(VInspector_ToolActionType_SelectOwnersId)->isChecked())
    onToolBarActionClicked(VInspector_ToolActionType_SelectOwnersId);

  QApplication::restoreOverrideCursor();
}

// =======================================================================
// function : onHistoryViewSelectionChanged
// purpose :
// =======================================================================
void VInspector_Window::onHistoryViewSelectionChanged (const QItemSelection& theSelected,
                                                       const QItemSelection&)
{
  VInspector_ViewModelHistory* aHistoryModel = dynamic_cast<VInspector_ViewModelHistory*> (myHistoryView->model());
  if (!aHistoryModel)
    return;

  if (theSelected.size() == 0)
    return;

  QModelIndexList aSelectedIndices = theSelected.indexes();
  QStringList aPointers = aHistoryModel->GetSelectedPointers(aSelectedIndices.first());

  VInspector_ViewModel* aTreeModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aTreeModel)
    return;

  QModelIndexList anIndices = aTreeModel->FindPointers (aPointers);
  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  aSelectionModel->clear();
  for (int anIndicesId = 0, aSize = anIndices.size(); anIndicesId < aSize; anIndicesId++)
  {
    QModelIndex anIndex = anIndices[anIndicesId];
    myTreeView->setExpanded (aTreeModel->parent (anIndex), true);
    aSelectionModel->select (anIndex, QItemSelectionModel::Select);
  }
}

// =======================================================================
// function : onExportToShapeView
// purpose :
// =======================================================================
void VInspector_Window::onExportToShapeView()
{
  NCollection_List<Handle(AIS_InteractiveObject)> aSelectedPresentations =
            VInspector_ItemPresentableObject::GetSelectedPresentations(myTreeView->selectionModel());
  if (aSelectedPresentations.Extent() <= 0)
    return;

  TCollection_AsciiString aPluginName ("TKShapeView");
  NCollection_List<Handle(Standard_Transient)> aParameters;
  if (myParameters->FindParameters (aPluginName))
    aParameters = myParameters->Parameters (aPluginName);

  NCollection_List<TCollection_AsciiString> anItemNames;
  if (myParameters->FindSelectedNames (aPluginName))
    anItemNames = myParameters->GetSelectedNames (aPluginName);

  QStringList anExportedPointers;
  for (NCollection_List<Handle(AIS_InteractiveObject)>::Iterator anIOIt (aSelectedPresentations); anIOIt.More(); anIOIt.Next())
  {
    Handle(AIS_Shape) aShapePresentation = Handle(AIS_Shape)::DownCast (anIOIt.Value());
    if (aShapePresentation.IsNull())
      continue;

    const TopoDS_Shape& aShape = aShapePresentation->Shape();
    if (aShape.IsNull())
      continue;
    aParameters.Append (aShape.TShape());
    anItemNames.Append (TInspectorAPI_PluginParameters::ParametersToString(aShape));
    anExportedPointers.append (VInspector_Tools::GetPointerInfo (aShape.TShape(), true).ToCString());
  }
  if (anExportedPointers.empty())
    return;

  TCollection_AsciiString aPluginShortName = aPluginName.SubString (3, aPluginName.Length());
  QString aMessage = QString ("TShape %1 is sent to %2.")
    .arg (anExportedPointers.join(", "))
    .arg (aPluginShortName.ToCString());
  QString aQuestion = QString ("Would you like to activate %1 immediately?\n")
    .arg (aPluginShortName.ToCString()).toStdString().c_str();
  if (!myExportToShapeViewDialog)
    myExportToShapeViewDialog = new ViewControl_MessageDialog (myParent, aMessage, aQuestion);
  else
    myExportToShapeViewDialog->SetInformation (aMessage);
  myExportToShapeViewDialog->Start();

  myParameters->SetSelectedNames (aPluginName, anItemNames);
  myParameters->SetParameters (aPluginName, aParameters, myExportToShapeViewDialog->IsAccepted());
}

// =======================================================================
// function : onShow
// purpose :
// =======================================================================
void VInspector_Window::onShow()
{
  displaySelectedPresentations (true);
}

// =======================================================================
// function : onHide
// purpose :
// =======================================================================
void VInspector_Window::onHide()
{
  displaySelectedPresentations (false);
}

// =======================================================================
// function : UpdateTreeModel
// purpose :
// =======================================================================
void VInspector_Window::UpdateTreeModel()
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (aViewModel)
    aViewModel->UpdateTreeModel();
}

// =======================================================================
// function : displaySelectedPresentations
// purpose :
// =======================================================================
void VInspector_Window::displaySelectedPresentations(const bool theToDisplay)
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return;

  Handle(AIS_InteractiveContext) aContext = aViewModel->GetContext();
  if (aContext.IsNull())
    return;

  NCollection_List<Handle(AIS_InteractiveObject)> aSelectedPresentations =
    VInspector_ItemPresentableObject::GetSelectedPresentations(myTreeView->selectionModel());
  if (aSelectedPresentations.Extent() <= 0)
    return;

  for (NCollection_List<Handle(AIS_InteractiveObject)>::Iterator anIOIt(aSelectedPresentations); anIOIt.More(); anIOIt.Next())
  {
    Handle(AIS_InteractiveObject) aPresentation = anIOIt.Value();
    if (theToDisplay)
    {
      aContext->Display(aPresentation, false);
      aContext->Load(aPresentation, -1);
    }
    else
      aContext->Erase(aPresentation, false);
  }
  aContext->UpdateCurrentViewer();

  // the order of objects returned by AIS_InteractiveContext is changed because the processed object is moved from
  // Erased to Displayed container or back
  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  aSelectionModel->clear();

  UpdateTreeModel();
}

// =======================================================================
// function : createView
// purpose :
// =======================================================================
Handle(AIS_InteractiveContext) VInspector_Window::createView()
{
  myViewWindow = new View_Window (0);
  myViewWindow->GetView()->SetPredefinedSize (VINSPECTOR_DEFAULT_VIEW_WIDTH, VINSPECTOR_DEFAULT_VIEW_HEIGHT);
  myViewWindow->move (VINSPECTOR_DEFAULT_VIEW_POSITION_X, VINSPECTOR_DEFAULT_VIEW_POSITION_Y);
  myViewWindow->show();

  return myViewWindow->GetView()->GetViewer()->GetContext();
}
