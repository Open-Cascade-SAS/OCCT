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

#include <VInspector_Window.hxx>

#include <AIS_Shape.hxx>
#include <VInspector_CallBack.hxx>
#include <VInspector_Communicator.hxx>
#include <VInspector_ItemEntityOwner.hxx>
#include <VInspector_ItemPresentableObject.hxx>
#include <VInspector_ToolBar.hxx>
#include <VInspector_Tools.hxx>
#include <VInspector_ViewModel.hxx>
#include <VInspector_ViewModelHistory.hxx>

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

const int FIRST_COLUMN_WIDTH = 250;

const int COLUMN_1_WIDTH = 30;
const int COLUMN_2_WIDTH = 70;
const int COLUMN_3_WIDTH = 130;
const int COLUMN_4_WIDTH = 90;
const int COLUMN_5_WIDTH = 180;
const int COLUMN_6_WIDTH = 70;
const int COLUMN_7_WIDTH = 70;

const int HISTORY_AIS_NAME_COLUMN_WIDTH = 140;

const int VINSPECTOR_DEFAULT_WIDTH  = 1250;
const int VINSPECTOR_DEFAULT_HEIGHT = 800;

const int VINSPECTOR_DEFAULT_POSITION_X = 200;
const int VINSPECTOR_DEFAULT_POSITION_Y = 60;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_Window::VInspector_Window()
{
  myMainWindow = new QMainWindow (0);

  QWidget* aCentralWidget = new QWidget (myMainWindow);
  QGridLayout* aParentLay = new QGridLayout (aCentralWidget);
  aParentLay->setContentsMargins (0, 0, 0, 0);

  // tool bar: on the bottom of the window
  myToolBar = new VInspector_ToolBar(aCentralWidget);
  connect (myToolBar, SIGNAL (actionClicked (int)), this, SLOT (onToolBarActionClicked (int)));
  aParentLay->addWidget (myToolBar->GetControl(), 0, 0);

  // tree view
  myTreeView = new QTreeView (aCentralWidget);
  myTreeView->setSelectionBehavior (QAbstractItemView::SelectRows);
  myTreeView->setSelectionMode (QAbstractItemView::ExtendedSelection);
  myTreeView->header()->setStretchLastSection (true);
  myTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect (myTreeView, SIGNAL(customContextMenuRequested(const QPoint&)),
           this, SLOT (onTreeViewContextMenuRequested(const QPoint&)));
  aParentLay->addWidget(myTreeView, 1, 0);
  aParentLay->setRowStretch (1, 1);
  myMainWindow->setCentralWidget (aCentralWidget);

  // history view in bottom dock widget
  myHistoryView = new QTreeView (aCentralWidget);
  myHistoryView->setSelectionBehavior (QAbstractItemView::SelectRows);
  myHistoryView->setSelectionMode (QAbstractItemView::ExtendedSelection);
  VInspector_ViewModelHistory* aHistoryModel = new VInspector_ViewModelHistory (myHistoryView);
  myHistoryView->setModel (aHistoryModel);

  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (myTreeView->model());
  myHistoryView->setSelectionModel (aSelectionModel);
  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           this, SLOT (onHistoryViewSelectionChanged (const QItemSelection&, const QItemSelection&)));

  myHistoryView->setColumnWidth (0, FIRST_COLUMN_WIDTH);
  myHistoryView->setColumnWidth (1, COLUMN_1_WIDTH);
  myHistoryView->setColumnWidth (2, COLUMN_2_WIDTH);
  myHistoryView->setColumnWidth (3, COLUMN_3_WIDTH);
  myHistoryView->setColumnWidth (4, HISTORY_AIS_NAME_COLUMN_WIDTH);

  QDockWidget* aHistoryDockWidget = new QDockWidget (tr ("Tree Level Line"), myMainWindow);
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
  QWidget* aParent = (QWidget*)theParent;
  if (!aParent)
    return;

  QLayout* aLayout = aParent->layout();
  if (aLayout)
    aLayout->addWidget (GetMainWindow());
}

// =======================================================================
// function : UpdateContent
// purpose :
// =======================================================================
void VInspector_Window::UpdateContent()
{
  TCollection_AsciiString aName = "TKVInspector";

  if (myParameters->FindParameters (aName))
    Init(myParameters->Parameters (aName));
  if (myParameters->FindFileNames(aName))
  {
    for (NCollection_List<TCollection_AsciiString>::Iterator aFileNamesIt(myParameters->FileNames(aName));
         aFileNamesIt.More(); aFileNamesIt.Next())
      OpenFile (aFileNamesIt.Value());

    NCollection_List<TCollection_AsciiString> aNames;
    myParameters->SetFileNames(aName, aNames);
  }
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_Window::Init (const NCollection_List<Handle(Standard_Transient)>& theParameters)
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
    return;
  SetContext (aContext);

  if (!aCallBack.IsNull())
  {
    myCallBack = aCallBack;
    VInspector_ViewModelHistory* aHistoryModel = dynamic_cast<VInspector_ViewModelHistory*>
                                                                    (myHistoryView->model());
    myCallBack->SetContext (aContext);
    myCallBack->SetHistoryModel(aHistoryModel);
  }
}

// =======================================================================
// function : SetContext
// purpose :
// =======================================================================
void VInspector_Window::SetContext (const Handle(AIS_InteractiveContext)& theContext)
{
  VInspector_ViewModel* aViewModel = new VInspector_ViewModel (myTreeView);
  aViewModel->SetContext (theContext);
  myTreeView->setModel (aViewModel);

  myTreeView->setExpanded (aViewModel->index (0, 0), true);
  myTreeView->setColumnWidth (0, FIRST_COLUMN_WIDTH);

  if (!myCallBack.IsNull())
    myCallBack->SetContext (theContext);

  QItemSelectionModel* aSelModel = new QItemSelectionModel (aViewModel, myTreeView);
  myTreeView->setSelectionModel (aSelModel);
  connect (aSelModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           this, SLOT (onSelectionChanged (const QItemSelection&, const QItemSelection&)));

  myTreeView->setColumnWidth (1, COLUMN_1_WIDTH);
  myTreeView->setColumnWidth (2, COLUMN_2_WIDTH);
  myTreeView->setColumnWidth (3, COLUMN_3_WIDTH);
  myTreeView->setColumnWidth (4, COLUMN_4_WIDTH);
  myTreeView->setColumnWidth (5, COLUMN_5_WIDTH);
  myTreeView->setColumnWidth (6, COLUMN_6_WIDTH);
  myTreeView->setColumnWidth (7, COLUMN_7_WIDTH);
}

// =======================================================================
// function : OpenFile
// purpose :
// =======================================================================
void VInspector_Window::OpenFile(const TCollection_AsciiString& theFileName)
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return;
    
  Handle(AIS_InteractiveContext) aContext = aViewModel->GetContext();
  if (aContext.IsNull())
    return;

  TopoDS_Shape aShape = VInspector_Tools::ReadShape (theFileName);
  if (aShape.IsNull())
    return;

  Handle(AIS_Shape) aPresentation = new AIS_Shape (aShape);
  aContext->Display (aPresentation, false);
  aContext->Load (aPresentation, -1/*selection mode*/, true);
  aContext->UpdateCurrentViewer();

  UpdateTreeModel();
  myTreeView->setExpanded (aViewModel->index (0, 0), true);
}

// =======================================================================
// function : onTreeViewContextMenuRequested
// purpose :
// =======================================================================
void VInspector_Window::onTreeViewContextMenuRequested(const QPoint& thePosition)
{
  QMenu* aMenu = new QMenu (GetMainWindow());
  aMenu->addAction (createAction (tr ("Export to ShapeView"), SLOT(onExportToShapeView())));
  aMenu->addAction (createAction (tr ("Show"), SLOT(onShow())));
  aMenu->addAction (createAction (tr ("Hide"), SLOT(onHide())));
  QPoint aPoint = myTreeView->mapToGlobal (thePosition);
  aMenu->exec(aPoint);
}

// =======================================================================
// function : onToolBarActionClicked
// purpose :
// =======================================================================
void VInspector_Window::onToolBarActionClicked (const int theActionId)
{
  switch (theActionId)
  {
    case VInspector_ToolActionType_UpdateId:
    {
      VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
      if (aViewModel)
        UpdateTreeModel();
      break;
    }
    case VInspector_ToolActionType_SelectPresentationsId:
    {
       bool isChecked = myToolBar->GetToolButton((VInspector_ToolActionType)theActionId)->isChecked();
       NCollection_List<Handle(AIS_InteractiveObject)> aPresentationsForViewer;
       if (isChecked)
         aPresentationsForViewer = VInspector_ItemPresentableObject::GetSelectedPresentations(myTreeView->selectionModel());
       VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
       Handle(AIS_InteractiveContext) aContext = aViewModel->GetContext();
       VInspector_Tools::AddOrRemovePresentations(aContext, aPresentationsForViewer);
      break;
    }
    case VInspector_ToolActionType_SelectOwnersId:
    {
       NCollection_List<Handle(SelectBasics_EntityOwner)> anOwnersForViewer;
       if (myToolBar->GetToolButton((VInspector_ToolActionType)theActionId)->isChecked())
         VInspector_ViewModel::GetSelectedOwners(myTreeView->selectionModel(), anOwnersForViewer);
       VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
       VInspector_Tools::AddOrRemoveSelectedShapes(aViewModel->GetContext(), anOwnersForViewer);
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
  if (myParameters->FindParameters(aPluginName))
  {
    NCollection_List<Handle(Standard_Transient)> aPluginParameters = myParameters->Parameters(aPluginName);

    QStringList anExportedPointers;
    for (NCollection_List<Handle(AIS_InteractiveObject)>::Iterator anIOIt (aSelectedPresentations); anIOIt.More(); anIOIt.Next())
    {
      Handle(AIS_Shape) aShapePresentation = Handle(AIS_Shape)::DownCast (anIOIt.Value());
      if (aShapePresentation.IsNull())
        continue;

      const TopoDS_Shape& aShape = aShapePresentation->Shape();
      if (aShape.IsNull())
        continue;
      aPluginParameters.Append (aShape.TShape());
      anExportedPointers.append (VInspector_Tools::GetPointerInfo (aShape.TShape(), true).ToCString());
    }
    if (anExportedPointers.empty())
      return;
    myParameters->SetParameters (aPluginName, aPluginParameters);
    QMessageBox::information (0, "Information", QString("TShapes '%1' are sent to %2 tool.")
                             .arg (anExportedPointers.join(", ").arg (QString (aPluginName.ToCString()))));
  }
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
  if (!aViewModel)
    return;
  aViewModel->Reset();
  aViewModel->EmitLayoutChanged();
}

// =======================================================================
// function : createAction
// purpose :
// =======================================================================
QAction* VInspector_Window::createAction(const QString& theText, const char* theSlot)
{
  QAction* anAction = new QAction(theText, GetMainWindow());
  connect(anAction, SIGNAL(triggered(bool)), this, theSlot);
  return anAction;
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
    Handle(AIS_InteractiveObject) aPresentation = Handle(AIS_Shape)::DownCast(anIOIt.Value());
    if (aPresentation.IsNull())
      continue;
    if (theToDisplay) {
      aContext->Display(aPresentation, false);
      aContext->Load(aPresentation, -1, true);
    }
    else
      aContext->Erase(aPresentation, false);
  }
  aContext->UpdateCurrentViewer();
}
