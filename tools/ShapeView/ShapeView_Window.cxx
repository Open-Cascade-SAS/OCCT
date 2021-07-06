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

#include <inspector/ShapeView_Window.hxx>
#include <inspector/ShapeView_ItemRoot.hxx>
#include <inspector/ShapeView_ItemShape.hxx>
#include <inspector/ShapeView_TreeModel.hxx>
#include <inspector/ShapeView_VisibilityState.hxx>

#include <inspector/Convert_Tools.hxx>

#include <inspector/TreeModel_Tools.hxx>
#include <inspector/TreeModel_ContextMenu.hxx>

#include <inspector/ViewControl_PropertyView.hxx>
#include <inspector/ViewControl_Tools.hxx>
#include <inspector/ViewControl_TreeView.hxx>

#include <inspector/View_Displayer.hxx>
#include <inspector/View_PresentationType.hxx>
#include <inspector/View_ToolBar.hxx>
#include <inspector/View_Window.hxx>
#include <inspector/View_Viewer.hxx>

#include <inspector/ShapeView_Window.hxx>
#include <inspector/ShapeView_OpenFileDialog.hxx>
#include <inspector/ShapeView_Tools.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QAction>
#include <QComboBox>
#include <QDockWidget>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMainWindow>
#include <QMenu>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QTextStream>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QWidget>
#include <QVBoxLayout>
#include <Standard_WarningsRestore.hxx>

static const int DEFAULT_SHAPE_VIEW_WIDTH = 900;
static const int DEFAULT_SHAPE_VIEW_HEIGHT = 450;
static const int DEFAULT_SHAPE_VIEW_POSITION_X = 60;
static const int DEFAULT_SHAPE_VIEW_POSITION_Y = 60;

static const int SHAPEVIEW_DEFAULT_TREE_VIEW_WIDTH = 600;
static const int SHAPEVIEW_DEFAULT_TREE_VIEW_HEIGHT = 500;

static const int SHAPEVIEW_DEFAULT_VIEW_WIDTH = 300;
static const int SHAPEVIEW_DEFAULT_VIEW_HEIGHT = 1000;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
ShapeView_Window::ShapeView_Window (QWidget* theParent)
: QObject (theParent)
{
  myMainWindow = new QMainWindow (theParent);

  myTreeView = new ViewControl_TreeView (myMainWindow);
  ((ViewControl_TreeView*)myTreeView)->SetPredefinedSize (QSize (SHAPEVIEW_DEFAULT_TREE_VIEW_WIDTH,
                                                                 SHAPEVIEW_DEFAULT_TREE_VIEW_HEIGHT));
  myTreeView->setContextMenuPolicy (Qt::CustomContextMenu);
  connect (myTreeView, SIGNAL (customContextMenuRequested (const QPoint&)),
          this, SLOT (onTreeViewContextMenuRequested (const QPoint&)));
  new TreeModel_ContextMenu (myTreeView);
  ShapeView_TreeModel* aModel = new ShapeView_TreeModel (myTreeView);
  aModel->InitColumns();

  myTreeView->setModel (aModel);
  ShapeView_VisibilityState* aVisibilityState = new ShapeView_VisibilityState (aModel);
  aModel->SetVisibilityState (aVisibilityState);
  TreeModel_Tools::UseVisibilityColumn (myTreeView);
  QObject::connect (myTreeView, SIGNAL (clicked (const QModelIndex&)), 
                    aVisibilityState, SLOT (OnClicked(const QModelIndex&)));

  QItemSelectionModel* aSelModel = new QItemSelectionModel (myTreeView->model(), myTreeView);
  myTreeView->setSelectionModel (aSelModel);
  connect (aSelModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           this, SLOT (onTreeViewSelectionChanged (const QItemSelection&, const QItemSelection&)));

  QModelIndex aParentIndex = myTreeView->model()->index (0, 0);
  myTreeView->setExpanded (aParentIndex, true);
  myMainWindow->setCentralWidget (myTreeView);

  // property view
  myPropertyView = new ViewControl_PropertyView (myMainWindow,
    QSize(SHAPEVIEW_DEFAULT_VIEW_WIDTH, SHAPEVIEW_DEFAULT_VIEW_HEIGHT));
  myPropertyPanelWidget = new QDockWidget (tr ("PropertyPanel"), myMainWindow);
  myPropertyPanelWidget->setObjectName (myPropertyPanelWidget->windowTitle());
  myPropertyPanelWidget->setTitleBarWidget (new QWidget(myMainWindow));
  myPropertyPanelWidget->setWidget (myPropertyView->GetControl());
  myMainWindow->addDockWidget (Qt::RightDockWidgetArea, myPropertyPanelWidget);

  // view
  myViewWindow = new View_Window (myMainWindow, NULL, false);
  connect (myViewWindow, SIGNAL(eraseAllPerformed()), this, SLOT(onEraseAllPerformed()));
  aVisibilityState->SetDisplayer (myViewWindow->Displayer());
  aVisibilityState->SetPresentationType (View_PresentationType_Main);
  myViewWindow->SetPredefinedSize (SHAPEVIEW_DEFAULT_VIEW_WIDTH, SHAPEVIEW_DEFAULT_VIEW_HEIGHT);

  QDockWidget* aViewDockWidget = new QDockWidget (tr ("View"), myMainWindow);
  aViewDockWidget->setObjectName (aViewDockWidget->windowTitle());
  aViewDockWidget->setWidget (myViewWindow);
  aViewDockWidget->setTitleBarWidget (myViewWindow->ViewToolBar()->GetControl());
  myMainWindow->addDockWidget (Qt::RightDockWidgetArea, aViewDockWidget);

  myMainWindow->splitDockWidget(myPropertyPanelWidget, aViewDockWidget, Qt::Vertical);

  myMainWindow->resize (DEFAULT_SHAPE_VIEW_WIDTH, DEFAULT_SHAPE_VIEW_HEIGHT);
  myMainWindow->move (DEFAULT_SHAPE_VIEW_POSITION_X, DEFAULT_SHAPE_VIEW_POSITION_Y);
}

// =======================================================================
// function : Destructor
// purpose :
// =======================================================================
ShapeView_Window::~ShapeView_Window()
{
}

// =======================================================================
// function : SetParent
// purpose :
// =======================================================================
void ShapeView_Window::SetParent (void* theParent)
{
  QWidget* aParent = (QWidget*)theParent;
  if (aParent)
  {
    QLayout* aLayout = aParent->layout();
    if (aLayout)
      aLayout->addWidget (GetMainWindow());
  }
}

// =======================================================================
// function : FillActionsMenu
// purpose :
// =======================================================================
void ShapeView_Window::FillActionsMenu (void* theMenu)
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
void ShapeView_Window::GetPreferences (TInspectorAPI_PreferencesDataMap& theItem)
{
  theItem.Bind ("geometry",  TreeModel_Tools::ToString (myMainWindow->saveState()).toStdString().c_str());

  QMap<QString, QString> anItems;
  TreeModel_Tools::SaveState (myTreeView, anItems);
  View_Window::SaveState(myViewWindow, anItems);
  for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
    theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());
}

// =======================================================================
// function : SetPreferences
// purpose :
// =======================================================================
void ShapeView_Window::SetPreferences (const TInspectorAPI_PreferencesDataMap& theItem)
{
  if (theItem.IsEmpty())
  {
    TreeModel_Tools::SetDefaultHeaderSections (myTreeView);
    return;
  }

  for (TInspectorAPI_IteratorOfPreferencesDataMap anItemIt (theItem); anItemIt.More(); anItemIt.Next())
  {
    if (anItemIt.Key().IsEqual ("geometry"))
      myMainWindow->restoreState (TreeModel_Tools::ToByteArray (anItemIt.Value().ToCString()));
    else if (TreeModel_Tools::RestoreState (myTreeView, anItemIt.Key().ToCString(), anItemIt.Value().ToCString()))
      continue;
    else if (View_Window::RestoreState(myViewWindow, anItemIt.Key().ToCString(), anItemIt.Value().ToCString()))
      continue;
  }
}

// =======================================================================
// function : UpdateContent
// purpose :
// =======================================================================
void ShapeView_Window::UpdateContent()
{
  TCollection_AsciiString aName = "TKShapeView";
  if (myParameters->FindParameters (aName))
  {
    NCollection_List<Handle(Standard_Transient)> aParameters = myParameters->Parameters (aName);
    // Init will remove from parameters those, that are processed only one time (TShape)
    Init(aParameters);
    myParameters->SetParameters (aName, aParameters);
  }
  if (myParameters->FindFileNames(aName))
  {
    for (NCollection_List<TCollection_AsciiString>::Iterator aFilesIt(myParameters->FileNames(aName));
         aFilesIt.More(); aFilesIt.Next())
      OpenFile (aFilesIt.Value());

    NCollection_List<TCollection_AsciiString> aNames;
    myParameters->SetFileNames (aName, aNames);
  }
  // make TopoDS_TShape selected if exist in select parameters
  NCollection_List<Handle(Standard_Transient)> anObjects;
  if (myParameters->GetSelectedObjects(aName, anObjects))
  {
    ShapeView_TreeModel* aModel = dynamic_cast<ShapeView_TreeModel*> (myTreeView->model());
    QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
    aSelectionModel->clear();
    for (NCollection_List<Handle(Standard_Transient)>::Iterator aParamsIt (anObjects);
         aParamsIt.More(); aParamsIt.Next())
    {
      Handle(Standard_Transient) anObject = aParamsIt.Value();
      Handle(TopoDS_TShape) aShapePointer = Handle(TopoDS_TShape)::DownCast (anObject);
      if (aShapePointer.IsNull())
        continue;

      TopoDS_Shape aShape;
      aShape.TShape (aShapePointer);

      QModelIndex aShapeIndex = aModel->FindIndex (aShape);
      if (!aShapeIndex.isValid())
        continue;
       aSelectionModel->select (aShapeIndex, QItemSelectionModel::Select);
       myTreeView->scrollTo (aShapeIndex);
    }
    myParameters->SetSelected (aName, NCollection_List<Handle(Standard_Transient)>());
  }
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void ShapeView_Window::Init (NCollection_List<Handle(Standard_Transient)>& theParameters)
{
  Handle(AIS_InteractiveContext) aContext;
  NCollection_List<Handle(Standard_Transient)> aParameters;

  TCollection_AsciiString aPluginName ("TKShapeView");
  NCollection_List<TCollection_AsciiString> aSelectedParameters;
  if (myParameters->FindSelectedNames (aPluginName)) // selected names have TShape parameters
    aSelectedParameters = myParameters->GetSelectedNames (aPluginName);

  NCollection_List<TCollection_AsciiString>::Iterator aParamsIt (aSelectedParameters);
  for (NCollection_List<Handle(Standard_Transient)>::Iterator anObjectsIt (theParameters);
       anObjectsIt.More(); anObjectsIt.Next())
  {
    Handle(Standard_Transient) anObject = anObjectsIt.Value();
    Handle(TopoDS_TShape) aShapePointer = Handle(TopoDS_TShape)::DownCast (anObject);
    if (!aShapePointer.IsNull())
    {
      TopoDS_Shape aShape;
      aShape.TShape (aShapePointer);
      if (aParamsIt.More())
      {
        // each Transient object has own location/orientation description
        TInspectorAPI_PluginParameters::ParametersToShape (aParamsIt.Value(), aShape);
        aParamsIt.Next();
      }
      addShape (aShape);
    }
    else
    {
      aParameters.Append (anObject);
      if (aContext.IsNull())
        aContext = Handle(AIS_InteractiveContext)::DownCast (anObject);
    }
  }
  if (!aContext.IsNull())
    myViewWindow->SetContext (View_ContextType_External, aContext);

  theParameters = aParameters;
  myParameters->SetSelectedNames (aPluginName, NCollection_List<TCollection_AsciiString>());
}

// =======================================================================
// function : OpenFile
// purpose :
// =======================================================================
void ShapeView_Window::OpenFile(const TCollection_AsciiString& theFileName)
{
  TopoDS_Shape aShape = Convert_Tools::ReadShape (theFileName);
  if (!aShape.IsNull())
    addShape(aShape);
}

// =======================================================================
// function : RemoveAllShapes
// purpose :
// =======================================================================
void ShapeView_Window::RemoveAllShapes()
{
  ShapeView_TreeModel* aModel = dynamic_cast<ShapeView_TreeModel*> (myTreeView->model());
  aModel->RemoveAllShapes();
}

// =======================================================================
// function : addShape
// purpose :
// =======================================================================
void ShapeView_Window::addShape (const TopoDS_Shape& theShape)
{
  ShapeView_TreeModel* aModel = dynamic_cast<ShapeView_TreeModel*> (myTreeView->model());
  aModel->AddShape (theShape);
}

// =======================================================================
// function : onTreeViewContextMenuRequested
// purpose :
// =======================================================================
void ShapeView_Window::onTreeViewContextMenuRequested (const QPoint& thePosition)
{
  QItemSelectionModel* aModel = myTreeView->selectionModel();
  if (!aModel)
    return;

  QModelIndex anIndex = TreeModel_ModelBase::SingleSelected (aModel->selectedIndexes(), 0);
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
  if (!anItemBase)
    return;

  QMenu* aMenu = new QMenu(myMainWindow);
  ShapeView_ItemRootPtr aRootItem = itemDynamicCast<ShapeView_ItemRoot> (anItemBase);
  if (aRootItem) {
    aMenu->addAction (ViewControl_Tools::CreateAction ("Load BREP file", SLOT (onLoadFile()), myMainWindow, this));
    aMenu->addAction (ViewControl_Tools::CreateAction ("Remove all shape items", SLOT (onClearView()), myMainWindow, this));
  }
  else {
    aMenu->addAction (ViewControl_Tools::CreateAction ("Export to BREP", SLOT (onExportToBREP()), myMainWindow, this));
    ShapeView_ItemShapePtr aShapeItem = itemDynamicCast<ShapeView_ItemShape>(anItemBase);
    const TopoDS_Shape& aShape = aShapeItem->GetItemShape();
    TopAbs_ShapeEnum anExplodeType = aShapeItem->ExplodeType();
    NCollection_List<TopAbs_ShapeEnum> anExplodeTypes;
    ShapeView_Tools::IsPossibleToExplode (aShape, anExplodeTypes);
    if (anExplodeTypes.Size() > 0)
    {
      QMenu* anExplodeMenu = aMenu->addMenu ("Explode");
      for (NCollection_List<TopAbs_ShapeEnum>::Iterator anExpIterator (anExplodeTypes); anExpIterator.More();
        anExpIterator.Next())
      {
        TopAbs_ShapeEnum aType = anExpIterator.Value();
        QAction* anAction = ViewControl_Tools::CreateAction (TopAbs::ShapeTypeToString (aType), SLOT (onExplode()), myMainWindow, this);
        anExplodeMenu->addAction (anAction);
        if (anExplodeType == aType)
        {
          anAction->setCheckable (true);
          anAction->setChecked (true);
        }
      }
      QAction* anAction = ViewControl_Tools::CreateAction ("NONE", SLOT (onExplode()), myMainWindow, this);
      anExplodeMenu->addSeparator();
      anExplodeMenu->addAction (anAction);
    }
  }

  QPoint aPoint = myTreeView->mapToGlobal (thePosition);
  aMenu->exec (aPoint);
}

// =======================================================================
// function : onTreeViewSelectionChanged
// purpose :
// =======================================================================
void ShapeView_Window::onTreeViewSelectionChanged (const QItemSelection&,
                                                   const QItemSelection&)
{
  QApplication::setOverrideCursor (Qt::WaitCursor);

  if (myPropertyPanelWidget->toggleViewAction()->isChecked())
    myPropertyView->Init (ViewControl_Tools::CreateTableModelValues (myTreeView->selectionModel()));

  QApplication::restoreOverrideCursor();
}

// =======================================================================
// function : onEraseAllPerformed
// purpose :
// =======================================================================
void ShapeView_Window::onEraseAllPerformed()
{
  ShapeView_TreeModel* aTreeModel = dynamic_cast<ShapeView_TreeModel*> (myTreeView->model());

  // TODO: provide update for only visibility state for better performance  TopoDS_Shape myCustomShape;

  aTreeModel->Reset();
  aTreeModel->EmitLayoutChanged();
}

// =======================================================================
// function : onExplode
// purpose :
// =======================================================================
void ShapeView_Window::onExplode()
{
  QItemSelectionModel* aModel = myTreeView->selectionModel();
  if (!aModel)
    return;

  QModelIndex anIndex = TreeModel_ModelBase::SingleSelected(aModel->selectedIndexes(), 0);
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex(anIndex);
  if (!anItemBase)
    return;

  ShapeView_ItemShapePtr aShapeItem = itemDynamicCast<ShapeView_ItemShape>(anItemBase);
  if (!aShapeItem)
    return;

  QAction* anAction = (QAction*)sender();
  if (!anAction)
    return;

  QApplication::setOverrideCursor (Qt::WaitCursor);
  TopAbs_ShapeEnum aShapeType;
  if (anAction->text() == "NONE")
    aShapeType = TopAbs_SHAPE;
  else
    aShapeType = TopAbs::ShapeTypeFromString(anAction->text().toStdString().c_str());

  myViewWindow->Displayer()->EraseAllPresentations();
  aShapeItem->SetExplodeType(aShapeType);

  //anItemBase->Parent()->Reset(); - TODO (update only modified sub-tree)
  ShapeView_TreeModel* aTreeModel = dynamic_cast<ShapeView_TreeModel*> (myTreeView->model());
  aTreeModel->Reset();
  aTreeModel->EmitLayoutChanged();
  QApplication::restoreOverrideCursor();
}

// =======================================================================
// function : onLoadFile
// purpose :
// =======================================================================
void ShapeView_Window::onLoadFile()
{
  QString aDataDirName = QDir::currentPath();

  QString aFileName = ShapeView_OpenFileDialog::OpenFile(0, aDataDirName);
  aFileName = QDir().toNativeSeparators (aFileName);
  if (aFileName.isEmpty())
    return;

  QApplication::setOverrideCursor (Qt::WaitCursor);
  onOpenFile(aFileName);
  QApplication::restoreOverrideCursor();
}

// =======================================================================
// function : onExportToBREP
// purpose :
// =======================================================================
void ShapeView_Window::onExportToBREP()
{
  QString aFilter (tr ("Boundary representation file (*.brep *)"));
  QString aSelectedFilter;
  QString aFileName = QFileDialog::getSaveFileName (0, tr ("Export shape to file"), QString(), aFilter, &aSelectedFilter);

  QItemSelectionModel* aModel = myTreeView->selectionModel();
  if (!aModel)
    return;

  QModelIndexList aSelectedRows = aModel->selectedRows();
  if (aSelectedRows.size() == 0)
    return;

  QModelIndex aSelectedIndex = aSelectedRows.at (0);
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (aSelectedIndex);
  if (!anItemBase)
    return;

  ShapeView_ItemShapePtr anItem = itemDynamicCast<ShapeView_ItemShape>(anItemBase);
  if (!anItem)
    return;

  TCollection_AsciiString aFileNameIndiced = aFileName.toStdString().c_str();
  const TopoDS_Shape& aShape = anItem->GetItemShape();
  BRepTools::Write (aShape, aFileNameIndiced.ToCString());
  anItem->SetFileName (aFileNameIndiced.ToCString());
  aFileName = aFileNameIndiced.ToCString();
}
