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
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <V3d_Viewer.hxx>

#include <inspector/Convert_Tools.hxx>

#include <inspector/TreeModel_ColumnType.hxx>
#include <inspector/TreeModel_ContextMenu.hxx>
#include <inspector/TreeModel_Tools.hxx>

#include <inspector/ViewControl_MessageDialog.hxx>
#include <inspector/ViewControl_TableModel.hxx>
#include <inspector/ViewControl_Tools.hxx>
#include <inspector/Convert_TransientShape.hxx>

#include <inspector/VInspector_ToolBar.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/VInspector_ViewModel.hxx>
#include <inspector/VInspector_Communicator.hxx>
#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemContextProperties.hxx>
#include <inspector/VInspector_ItemGraphic3dCLight.hxx>
#include <inspector/VInspector_ToolBar.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/VInspector_ViewModel.hxx>

#include <inspector/ViewControl_PropertyView.hxx>
#include <inspector/ViewControl_TreeView.hxx>

#include <inspector/View_Displayer.hxx>
#include <inspector/View_DisplayPreview.hxx>
#include <inspector/View_PreviewParameters.hxx>
#include <inspector/View_Widget.hxx>
#include <inspector/View_Window.hxx>
#include <inspector/View_Viewer.hxx>

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

const int VINSPECTOR_DEFAULT_PROPERTY_VIEW_WIDTH = 300;
const int VINSPECTOR_DEFAULT_PROPERTY_VIEW_HEIGHT = 1000;

const int VINSPECTOR_DEFAULT_WIDTH  = 1250;
const int VINSPECTOR_DEFAULT_HEIGHT = 800;

const int VINSPECTOR_DEFAULT_POSITION_X = 200;
const int VINSPECTOR_DEFAULT_POSITION_Y = 60;

const int VINSPECTOR_DEFAULT_VIEW_WIDTH = 400;
const int VINSPECTOR_DEFAULT_VIEW_HEIGHT = 1000;

const int VINSPECTOR_DEFAULT_VIEW_POSITION_X = 200 + 900 + 100; // TINSPECTOR_DEFAULT_POSITION_X + TINSPECTOR_DEFAULT_WIDTH + 100
const int VINSPECTOR_DEFAULT_VIEW_POSITION_Y = 60; // TINSPECTOR_DEFAULT_POSITION_Y + 50

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_Window::VInspector_Window()
: myParent (0), myExportToShapeViewDialog (0), myViewWindow (0)
{
  myDisplayer = new View_Displayer();

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
  aTreeModel->InitColumns();
  myTreeView->setModel (aTreeModel);
  // hide Visibility column
  TreeModel_HeaderSection* anItem = aTreeModel->ChangeHeaderItem ((int)TreeModel_ColumnType_Visibility);
  anItem->SetIsHidden (true);

  connect (myTreeView, SIGNAL(customContextMenuRequested(const QPoint&)),
           this, SLOT (onTreeViewContextMenuRequested(const QPoint&)));
  new TreeModel_ContextMenu (myTreeView);

  QItemSelectionModel* aSelModel = new QItemSelectionModel (myTreeView->model(), myTreeView);
  myTreeView->setSelectionModel (aSelModel);
  connect (aSelModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           this, SLOT (onTreeViewSelectionChanged (const QItemSelection&, const QItemSelection&)));

  aParentLay->addWidget(myTreeView, 1, 0);
  aParentLay->setRowStretch (1, 1);
  myMainWindow->setCentralWidget (aCentralWidget);

  // property view
  myPropertyView = new ViewControl_PropertyView (myMainWindow,
    QSize(VINSPECTOR_DEFAULT_PROPERTY_VIEW_WIDTH, VINSPECTOR_DEFAULT_PROPERTY_VIEW_HEIGHT));
  myPropertyPanelWidget = new QDockWidget (tr ("PropertyPanel"), myMainWindow);
  myPropertyPanelWidget->setObjectName (myPropertyPanelWidget->windowTitle());
  myPropertyPanelWidget->setTitleBarWidget (new QWidget(myMainWindow));
  myPropertyPanelWidget->setWidget (myPropertyView->GetControl());
  myMainWindow->addDockWidget (Qt::RightDockWidgetArea, myPropertyPanelWidget);
  connect (myPropertyPanelWidget->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT (onPropertyPanelShown (bool)));

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
  View_PreviewParameters::SaveState (displayer()->DisplayPreview()->GetPreviewParameters(), anItems, "preview_parameters_");
  for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
    theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());

  anItems.clear();
  TreeModel_Tools::SaveState (myTreeView, anItems);
  for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
  {
    theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());
  }

  anItems.clear();
  ViewControl_PropertyView::SaveState (myPropertyView, anItems, "property_view_parameters_");
  for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
    theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());

  if (myViewWindow)
  {
    anItems.clear();
    View_Window::SaveState(myViewWindow, anItems);
    for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
      theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());
  }
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
    return;
  }

  for (TInspectorAPI_IteratorOfPreferencesDataMap anItemIt (theItem); anItemIt.More(); anItemIt.Next())
  {
    TCollection_AsciiString anItemKey = anItemIt.Key();
    TCollection_AsciiString anItemValue = anItemIt.Value();
    if (anItemKey.IsEqual ("geometry"))
      myMainWindow->restoreState (TreeModel_Tools::ToByteArray (anItemValue.ToCString()));
    else if (TreeModel_Tools::RestoreState (myTreeView, anItemKey.ToCString(), anItemValue.ToCString()))
      continue;
    else if (View_PreviewParameters::RestoreState (displayer()->DisplayPreview()->GetPreviewParameters(), anItemKey.ToCString(),
      anItemValue.ToCString(), "preview_parameters_"))
      continue;
    else if (ViewControl_PropertyView::RestoreState (myPropertyView, anItemKey.ToCString(),
      anItemValue.ToCString(), "property_view_parameters_"))
      continue;
    else if (myViewWindow && View_Window::RestoreState(myViewWindow, anItemIt.Key().ToCString(), anItemIt.Value().ToCString()))
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
// function : SelectedPresentations
// purpose :
// =======================================================================
NCollection_List<Handle(AIS_InteractiveObject)> VInspector_Window::SelectedPresentations (QItemSelectionModel* theModel)
{
  NCollection_List<Handle(AIS_InteractiveObject)> aSelectedPresentations;

  QList<TreeModel_ItemBasePtr> anItems = TreeModel_ModelBase::SelectedItems (theModel->selectedIndexes());

  QList<size_t> aSelectedIds; // Remember of selected address in order to avoid duplicates
  NCollection_List<Handle(Standard_Transient)> anItemPresentations;
  for (QList<TreeModel_ItemBasePtr>::const_iterator anItemIt = anItems.begin(); anItemIt != anItems.end(); ++anItemIt)
  {
    TreeModel_ItemBasePtr anItem = *anItemIt;
    VInspector_ItemBasePtr aVItem = itemDynamicCast<VInspector_ItemBase>(anItem);
    if (!aVItem)
      continue;

    anItemPresentations.Clear();
    aVItem->Presentations (anItemPresentations);

    for (NCollection_List<Handle(Standard_Transient)>::Iterator anIt (anItemPresentations); anIt.More(); anIt.Next())
    {
      Handle(AIS_InteractiveObject) aPresentation = Handle(AIS_InteractiveObject)::DownCast (anIt.Value());
      if (aSelectedIds.contains ((size_t)aPresentation.get()))
        continue;
      aSelectedIds.append ((size_t)aPresentation.get());
      if (!aPresentation.IsNull())
        aSelectedPresentations.Append (aPresentation);
    }
  }
  return aSelectedPresentations;
}

// =======================================================================
// function : SelectedShapes
// purpose :
// =======================================================================
void VInspector_Window::SelectedShapes (NCollection_List<Handle(Standard_Transient)>& theSelPresentations)
{
  QModelIndexList theIndices = myTreeView->selectionModel()->selectedIndexes();

  QList<TreeModel_ItemBasePtr> anItems = TreeModel_ModelBase::SelectedItems (theIndices);
  for (QList<TreeModel_ItemBasePtr>::const_iterator anItemIt = anItems.begin(); anItemIt != anItems.end(); ++anItemIt)
  {                                                                                                                  
    TreeModel_ItemBasePtr anItem = *anItemIt;
    VInspector_ItemBasePtr aVItem = itemDynamicCast<VInspector_ItemBase>(anItem);
    if (!aVItem /*|| aVItem->Row() == 0*/)
    {
      anItem->Presentations (theSelPresentations);
      continue;
    }

    TopoDS_Shape aShape = aVItem->GetPresentationShape();
    if (aShape.IsNull())
      continue;

    theSelPresentations.Append (new Convert_TransientShape (aShape));
  }
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
bool VInspector_Window::Init (const NCollection_List<Handle(Standard_Transient)>& theParameters)
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return Standard_False;

  Handle(AIS_InteractiveContext) aContext;
  Standard_Boolean isModelUpdated = Standard_False;

  for (NCollection_List<Handle(Standard_Transient)>::Iterator aParamsIt (theParameters); aParamsIt.More(); aParamsIt.Next())
  {
    Handle(Standard_Transient) anObject = aParamsIt.Value();
    if (aContext.IsNull())
      aContext = Handle(AIS_InteractiveContext)::DownCast (anObject);

  }
  if (aViewModel->GetContext() != aContext)
    SetContext(aContext);
  else
    isModelUpdated = Standard_True;

  if (isModelUpdated)
    UpdateTreeModel();

  return true;
}

// =======================================================================
// function : SetContext
// purpose :
// =======================================================================
void VInspector_Window::SetContext (const Handle(AIS_InteractiveContext)& theContext)
{
  if (theContext.IsNull())
    return;

  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  aViewModel->SetContext (theContext);
  myTreeView->setExpanded (aViewModel->index (0, 0), true);

  if (myDisplayer)
    myDisplayer->SetContext (theContext);
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

    const Handle(V3d_Viewer) aViewer = aViewModel->GetContext()->CurrentViewer();
    if (!aViewer.IsNull())
    {
      addLight (Graphic3d_TOLS_POSITIONAL, aViewer);
      addLight (Graphic3d_TOLS_SPOT, aViewer);
    }

    isModelUpdated = true;
  }

  TopoDS_Shape aShape = Convert_Tools::ReadShape (theFileName);
  if (aShape.IsNull())
    return isModelUpdated;

  Handle(AIS_Shape) aPresentation = new AIS_Shape (aShape);
  aPresentation->Attributes()->SetAutoTriangulation (Standard_False);

  View_Displayer* aDisplayer = displayer();
  aDisplayer->DisplayPresentation (aPresentation);
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
  aMenu->addSeparator();

  QModelIndex anIndex = TreeModel_ModelBase::SingleSelected (myTreeView->selectionModel()->selectedIndexes(), 0);
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
  if (anItemBase)
  {
    if (itemDynamicCast<VInspector_ItemContextProperties> (anItemBase))
    {
      aMenu->addSeparator();
      QMenu* anExplodeMenu = aMenu->addMenu ("Add Light");

      anExplodeMenu->addAction (ViewControl_Tools::CreateAction ("Ambient", SLOT (onAddLight()), myMainWindow, this));
      anExplodeMenu->addAction (ViewControl_Tools::CreateAction ("Directional", SLOT (onAddLight()), myMainWindow, this));
      anExplodeMenu->addAction (ViewControl_Tools::CreateAction ("Positional", SLOT (onAddLight()), myMainWindow, this));
      anExplodeMenu->addAction (ViewControl_Tools::CreateAction ("Spot", SLOT (onAddLight()), myMainWindow, this));
    }

    if (itemDynamicCast<VInspector_ItemGraphic3dCLight> (anItemBase))
    {
      aMenu->addSeparator();
      aMenu->addAction (ViewControl_Tools::CreateAction ("Remove Light", SLOT (onRemoveLight()), myMainWindow, this));
      VInspector_ItemGraphic3dCLightPtr anItemLight = itemDynamicCast<VInspector_ItemGraphic3dCLight> (anItemBase);
      if (!anItemLight->GetLight().IsNull())
      {
        bool isOn = anItemLight->GetLight()->IsEnabled();
        aMenu->addAction (ViewControl_Tools::CreateAction (isOn ? "OFF Light" : "ON Light", SLOT (onOnOffLight()),
                          myMainWindow, this));
      }
    }
  }

  aMenu->addSeparator();
  for (int aTypeId = (int)View_DisplayActionType_DisplayId; aTypeId <= (int)View_DisplayActionType_RemoveId; aTypeId++)
  {
    aMenu->addAction (ViewControl_Tools::CreateAction (VInspector_Tools::DisplayActionTypeToString ((View_DisplayActionType) aTypeId),
                      SLOT (onDisplayActionTypeClicked()), GetMainWindow(), this));
  }
  aMenu->addSeparator();

  aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Expand"), SLOT (onExpand()), GetMainWindow(), this));
  aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Expand All"), SLOT (onExpandAll()), GetMainWindow(), this));
  aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Collapse All"), SLOT (onCollapseAll()), GetMainWindow(), this));

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
    default:
      break;
  }
}

// =======================================================================
// function : onPropertyPanelShown
// purpose :
// =======================================================================
void VInspector_Window::onPropertyPanelShown (bool isToggled)
{
  if (!isToggled)
    return;

  myPropertyView->Init (ViewControl_Tools::CreateTableModelValues (myTreeView->selectionModel()));
}

// =======================================================================
// function : onTreeViewSelectionChanged
// purpose :
// =======================================================================
void VInspector_Window::onTreeViewSelectionChanged (const QItemSelection&,
                                                    const QItemSelection&)
{
  if (myPropertyPanelWidget->toggleViewAction()->isChecked())
    myPropertyView->Init (ViewControl_Tools::CreateTableModelValues (myTreeView->selectionModel()));

  NCollection_List<Handle(Standard_Transient)> aSelPresentations;

  QModelIndexList aSelectedIndices = myTreeView->selectionModel()->selectedIndexes();
  for (QModelIndexList::const_iterator aSelIt = aSelectedIndices.begin(); aSelIt != aSelectedIndices.end(); aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() != 0)
      continue;

    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    if (!anItemBase)
      continue;

    anItemBase->Presentations (aSelPresentations);
  }

  SelectedShapes (aSelPresentations);
  displayer()->UpdatePreview (View_DisplayActionType_DisplayId, aSelPresentations);
}

// =======================================================================
// function : onExportToShapeView
// purpose :
// =======================================================================
void VInspector_Window::onExportToShapeView()
{
  NCollection_List<Handle(Standard_Transient)> aSelectedShapes;
  SelectedShapes (aSelectedShapes);

  TCollection_AsciiString aPluginName ("TKShapeView");
  NCollection_List<Handle(Standard_Transient)> aParameters;
  NCollection_List<TCollection_AsciiString> anItemNames;

  QStringList anExportedPointers;
  if (aSelectedShapes.Extent() > 0)
  {
    for (NCollection_List<Handle(Standard_Transient)>::Iterator aShapeIt (aSelectedShapes); aShapeIt.More(); aShapeIt.Next())
    {
      Handle(Convert_TransientShape) aShapePtr = Handle(Convert_TransientShape)::DownCast (aShapeIt.Value());
      if (aShapePtr.IsNull())
        continue;

      const TopoDS_Shape& aShape = aShapePtr->Shape();
      if (aShape.IsNull())
        continue;
      aParameters.Append (aShape.TShape());
      anItemNames.Append (TInspectorAPI_PluginParameters::ParametersToString(aShape));
      anExportedPointers.append (Standard_Dump::GetPointerInfo (aShape.TShape(), true).ToCString());
    }
  }

  if (anExportedPointers.isEmpty())
    return;

  TCollection_AsciiString aPluginShortName = aPluginName.SubString (3, aPluginName.Length());
  QString aMessage = QString ("Objects %1 are sent to %2.")
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
// function : onAddLight
// purpose :
// =======================================================================
void VInspector_Window::onAddLight()
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return;
  const Handle(V3d_Viewer) aViewer = aViewModel->GetContext()->CurrentViewer();
  if (aViewer.IsNull())
    return;

  QAction* anAction = (QAction*)sender();
  QString aText = anAction->text();

  Graphic3d_TypeOfLightSource aLightSourceType = Graphic3d_TOLS_AMBIENT;

  if (aText == "Ambient")           { aLightSourceType = Graphic3d_TOLS_AMBIENT; }
  else if (aText == "Directional")  { aLightSourceType = Graphic3d_TOLS_DIRECTIONAL; }
  else if (aText == "Positional")   { aLightSourceType = Graphic3d_TOLS_POSITIONAL; }
  else if (aText == "Spot")         { aLightSourceType = Graphic3d_TOLS_SPOT; }
  else                              { return; }

  addLight(aLightSourceType, aViewer);
}

// =======================================================================
// function : onRemoveLight
// purpose :
// =======================================================================
void VInspector_Window::onRemoveLight()
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  const Handle(V3d_Viewer) aViewer = aViewModel ? aViewModel->GetContext()->CurrentViewer() : NULL;
  if (aViewer.IsNull())
  {
    return;
  }

  QModelIndex anIndex = TreeModel_ModelBase::SingleSelected (myTreeView->selectionModel()->selectedIndexes(), 0);
  VInspector_ItemGraphic3dCLightPtr aLightItem = itemDynamicCast<VInspector_ItemGraphic3dCLight> (
    TreeModel_ModelBase::GetItemByIndex (anIndex));
  if (!aLightItem)
  {
    return;
  }

  if (aViewer->ActiveLights().Extent() == 1) // not possible to remove the latest light
  {
    return;
  }

  Handle(Graphic3d_CLight) aLight = aLightItem->GetLight();
  aViewer->DelLight (aLight);
  aViewer->UpdateLights();

  aViewer->Invalidate();
  aViewer->Redraw();
  UpdateTreeModel();
}

// =======================================================================
// function : onOnOffLight
// purpose :
// =======================================================================
void VInspector_Window::onOnOffLight()
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  const Handle(V3d_Viewer) aViewer = aViewModel ? aViewModel->GetContext()->CurrentViewer() : NULL;
  if (aViewer.IsNull())
  {
    return;
  }

  QModelIndex anIndex = TreeModel_ModelBase::SingleSelected (myTreeView->selectionModel()->selectedIndexes(), 0);

  VInspector_ItemGraphic3dCLightPtr anItemLight = itemDynamicCast<VInspector_ItemGraphic3dCLight> (
    TreeModel_ModelBase::GetItemByIndex (anIndex));
  if (anItemLight->GetLight().IsNull())
    return;

  bool isOn = anItemLight->GetLight()->IsEnabled();
  anItemLight->GetLight()->SetEnabled (!isOn);

  aViewer->UpdateLights();
  aViewer->Invalidate();
  aViewer->Redraw();
  UpdateTreeModel();
}

// =======================================================================
// function : onDisplayActionTypeClicked
// purpose :
// =======================================================================
void VInspector_Window::onDisplayActionTypeClicked()
{
  QAction* anAction = (QAction*)sender();

  displaySelectedPresentations (VInspector_Tools::DisplayActionTypeFromString (anAction->text().toStdString().c_str()));
}

// =======================================================================
// function : onExpand
// purpose :
// =======================================================================
void VInspector_Window::onExpand()
{
  QApplication::setOverrideCursor (Qt::WaitCursor);

  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
  for (int aSelectedId = 0, aSize = aSelectedIndices.size(); aSelectedId < aSize; aSelectedId++)
  {
    int aLevels = 2;
    TreeModel_Tools::SetExpanded (myTreeView, aSelectedIndices[aSelectedId], true, aLevels);
  }
  QApplication::restoreOverrideCursor();
}

// =======================================================================
// function : onExpandAll
// purpose :
// =======================================================================
void VInspector_Window::onExpandAll()
{
  QApplication::setOverrideCursor (Qt::WaitCursor);

  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
  for (int  aSelectedId = 0, aSize = aSelectedIndices.size(); aSelectedId < aSize; aSelectedId++)
  {
    int aLevels = -1;
    TreeModel_Tools::SetExpanded (myTreeView, aSelectedIndices[aSelectedId], true, aLevels);
  }
  QApplication::restoreOverrideCursor();
}

// =======================================================================
// function : onCollapseAll
// purpose :
// =======================================================================
void VInspector_Window::onCollapseAll()
{
  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
  for (int aSelectedId = 0, aSize = aSelectedIndices.size(); aSelectedId < aSize; aSelectedId++)
  {
    int aLevels = -1;
    TreeModel_Tools::SetExpanded (myTreeView, aSelectedIndices[aSelectedId], false, aLevels);
  }
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

void VInspector_Window::displaySelectedPresentations (const View_DisplayActionType theType)
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return;

  Handle(AIS_InteractiveContext) aContext = aViewModel->GetContext();
  if (aContext.IsNull())
    return;

  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  if (!aSelectionModel)
    return;

  NCollection_List<Handle(AIS_InteractiveObject)> aSelectedPresentations = SelectedPresentations (aSelectionModel);
  // the order of objects returned by AIS_InteractiveContext is changed because the processed object is moved from
  // Erased to Displayed container or back
  aSelectionModel->clear();


  if (aSelectedPresentations.Extent() == 0)
    return;

  for (NCollection_List<Handle(AIS_InteractiveObject)>::Iterator anIOIt(aSelectedPresentations); anIOIt.More(); anIOIt.Next())
  {
    Handle(AIS_InteractiveObject) aPresentation = anIOIt.Value();
    switch (theType)
    {
      case View_DisplayActionType_DisplayId:
      {
        aContext->Display(aPresentation, false);
        aContext->Load(aPresentation, -1);
      }
      break;

      case View_DisplayActionType_RedisplayId: aContext->Redisplay (aPresentation, false); break;
      case View_DisplayActionType_EraseId: aContext->Erase (aPresentation, false); break;
      case View_DisplayActionType_RemoveId: aContext->Remove (aPresentation, false); break;
      default: break;
    }
  }
  aContext->UpdateCurrentViewer();

  UpdateTreeModel();
}

// =======================================================================
// function : highlightTreeViewItems
// purpose :
// =======================================================================
void VInspector_Window::highlightTreeViewItems (const QStringList& thePointers)
{
  VInspector_ViewModel* aTreeModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aTreeModel)
    return;

  QModelIndexList anIndices;
  aTreeModel->FindPointers (thePointers, QModelIndex(), anIndices);
  for (int anIndicesId = 0, aSize = anIndices.size(); anIndicesId < aSize; anIndicesId++)
  {
    QModelIndex anIndex = anIndices[anIndicesId];
    TreeModel_Tools::SetExpandedTo (myTreeView, anIndex);
  }
  aTreeModel->SetHighlighted (anIndices);

  if (!anIndices.isEmpty())
    myTreeView->scrollTo (anIndices.last());
}

// =======================================================================
// function : selectTreeViewItems
// purpose :
// =======================================================================
void VInspector_Window::selectTreeViewItems (const QStringList& thePointers)
{
  VInspector_ViewModel* aTreeModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aTreeModel)
    return;
  
  QModelIndexList anIndices;
  aTreeModel->FindPointers (thePointers, QModelIndex(), anIndices);
  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  aSelectionModel->clear();
  for (int anIndicesId = 0, aSize = anIndices.size(); anIndicesId < aSize; anIndicesId++)
  {
    QModelIndex anIndex = anIndices[anIndicesId];
    TreeModel_Tools::SetExpandedTo (myTreeView, anIndex);
    aSelectionModel->select (anIndex, QItemSelectionModel::Select);
  }
}

// =======================================================================
// function : createView
// purpose :
// =======================================================================
Handle(AIS_InteractiveContext) VInspector_Window::createView()
{
  // create two view windows
  Handle(AIS_InteractiveContext) aContext = View_Viewer::CreateStandardViewer();

  myViewWindow = new View_Window (0, aContext, false /*for opening several BREP files*/, true);
  myViewWindow->SetPredefinedSize (VINSPECTOR_DEFAULT_VIEW_WIDTH, VINSPECTOR_DEFAULT_VIEW_HEIGHT);
  myViewWindow->move (VINSPECTOR_DEFAULT_VIEW_POSITION_X, VINSPECTOR_DEFAULT_VIEW_POSITION_Y);
  myViewWindow->show();

  return aContext;
}

// =======================================================================
// function : displayer
// purpose :
// =======================================================================
View_Displayer* VInspector_Window::displayer()
{
  if (myViewWindow)
    return myViewWindow->Displayer();

  return myDisplayer;
}

// =======================================================================
// function : addLight
// purpose :
// =======================================================================
void VInspector_Window::addLight (const Graphic3d_TypeOfLightSource& theSourceLight,
                                  const Handle(V3d_Viewer)& theViewer)
{
  Standard_Boolean aNeedDirection = theSourceLight == Graphic3d_TOLS_DIRECTIONAL ||
                                    theSourceLight == Graphic3d_TOLS_SPOT;

  Handle(Graphic3d_CLight) aLight = new Graphic3d_CLight (theSourceLight);
  if (aNeedDirection)
  {
    aLight->SetDirection (gp::DZ());
  }
  if (theSourceLight == Graphic3d_TOLS_SPOT)
  {
    aLight->SetAngle ((Standard_ShortReal)M_PI - ShortRealEpsilon());
    aLight->SetPosition (gp_Pnt (-100, -100, -100));
  }

  theViewer->AddLight (aLight);
  theViewer->SetLightOn (aLight);

  theViewer->Invalidate();
  theViewer->Redraw();
  UpdateTreeModel();
}
