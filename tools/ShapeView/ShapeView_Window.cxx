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

#include <inspector/TreeModel_Tools.hxx>
#include <inspector/TreeModel_ContextMenu.hxx>

#include <inspector/ViewControl_Tools.hxx>
#include <inspector/ViewControl_TreeView.hxx>

#include <inspector/View_Displayer.hxx>
#include <inspector/View_PresentationType.hxx>
#include <inspector/View_Tools.hxx>
#include <inspector/View_ToolBar.hxx>
#include <inspector/View_Widget.hxx>
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

const int DEFAULT_TEXT_VIEW_WIDTH = 800;
const int DEFAULT_TEXT_VIEW_HEIGHT = 700;
const int DEFAULT_TEXT_VIEW_POSITION_X = 430;
const int DEFAULT_TEXT_VIEW_POSITION_Y = 30;
const int DEFAULT_TEXT_VIEW_DELTA = 100;

const int DEFAULT_SHAPE_VIEW_WIDTH = 900;
const int DEFAULT_SHAPE_VIEW_HEIGHT = 450;
const int DEFAULT_SHAPE_VIEW_POSITION_X = 60;
const int DEFAULT_SHAPE_VIEW_POSITION_Y = 60;

const int SHAPEVIEW_DEFAULT_TREE_VIEW_WIDTH = 600;
const int SHAPEVIEW_DEFAULT_TREE_VIEW_HEIGHT = 500;

const int SHAPEVIEW_DEFAULT_VIEW_WIDTH = 300;
const int SHAPEVIEW_DEFAULT_VIEW_HEIGHT = 1000;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
ShapeView_Window::ShapeView_Window (QWidget* theParent)
: QObject (theParent), myNextPosition (0)
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
  myTreeView->setModel (aModel);
  ShapeView_VisibilityState* aVisibilityState = new ShapeView_VisibilityState (aModel);
  aModel->SetVisibilityState (aVisibilityState);
  TreeModel_Tools::UseVisibilityColumn (myTreeView);

  QModelIndex aParentIndex = myTreeView->model()->index (0, 0);
  myTreeView->setExpanded (aParentIndex, true);
  myMainWindow->setCentralWidget (myTreeView);

  // view
  myViewWindow = new View_Window (myMainWindow, false);
  connect (myViewWindow, SIGNAL(eraseAllPerformed()), this, SLOT(onEraseAllPerformed()));
  aVisibilityState->SetDisplayer (myViewWindow->GetDisplayer());
  aVisibilityState->SetPresentationType (View_PresentationType_Main);
  myViewWindow->GetView()->SetPredefinedSize (SHAPEVIEW_DEFAULT_VIEW_WIDTH, SHAPEVIEW_DEFAULT_VIEW_HEIGHT);

  QDockWidget* aViewDockWidget = new QDockWidget (tr ("View"), myMainWindow);
  aViewDockWidget->setObjectName (aViewDockWidget->windowTitle());
  aViewDockWidget->setWidget (myViewWindow);
  aViewDockWidget->setTitleBarWidget (myViewWindow->GetViewToolBar()->GetControl());
  myMainWindow->addDockWidget (Qt::RightDockWidgetArea, aViewDockWidget);

  myMainWindow->resize (DEFAULT_SHAPE_VIEW_WIDTH, DEFAULT_SHAPE_VIEW_HEIGHT);
  myMainWindow->move (DEFAULT_SHAPE_VIEW_POSITION_X, DEFAULT_SHAPE_VIEW_POSITION_Y);
}

// =======================================================================
// function : Destructor
// purpose :
// =======================================================================
ShapeView_Window::~ShapeView_Window()
{
  onCloseAllBREPViews();
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
  View_Tools::SaveState(myViewWindow, anItems);
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
    else if (View_Tools::RestoreState(myViewWindow, anItemIt.Key().ToCString(), anItemIt.Value().ToCString()))
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
  TopoDS_Shape aShape = ShapeView_Tools::ReadShape (theFileName);
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

  onCloseAllBREPViews();
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
    if (!GetTemporaryDirectory().IsEmpty())
      aMenu->addAction (ViewControl_Tools::CreateAction ("BREP view", SLOT (onBREPView()), myMainWindow, this));
    aMenu->addAction (ViewControl_Tools::CreateAction ("Close All BREP views", SLOT (onCloseAllBREPViews()), myMainWindow, this));
    aMenu->addAction (ViewControl_Tools::CreateAction ("BREP directory", SLOT (onBREPDirectory()), myMainWindow, this));
  }

  QPoint aPoint = myTreeView->mapToGlobal (thePosition);
  aMenu->exec (aPoint);
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
// function : onBREPDirectory
// purpose :
// =======================================================================
void ShapeView_Window::onBREPDirectory()
{
  QString aFilter (tr ("BREP file (*.brep*)"));
  QString aSelectedFilter;
  QString aFileName = QFileDialog::getOpenFileName (0, tr ("Export shape to BREP file"),
                                                    GetTemporaryDirectory().ToCString(), aSelectedFilter);
  if (!aFileName.isEmpty())
    viewFile (aFileName);
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
  if (!aFileName.isEmpty())
    onOpenFile(aFileName);
}

// =======================================================================
// function : onBREPView
// purpose :
// =======================================================================
void ShapeView_Window::onBREPView()
{
  if (GetTemporaryDirectory().IsEmpty())
    return;

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

  QString aFileName = anItem->GetFileName();
  QDir aDir;
  if (aFileName.isEmpty() || !aDir.exists (aFileName))
  {
    TCollection_AsciiString aFileNameIndiced = GetTemporaryDirectory() + TCollection_AsciiString ("\\") +
                                               getNextTmpName (anItem->TShapePointer());
    const TopoDS_Shape& aShape = anItem->GetItemShape();
    BRepTools::Write (aShape, aFileNameIndiced.ToCString());
    anItem->SetFileName (aFileNameIndiced.ToCString());
    aFileName = aFileNameIndiced.ToCString();
  }
  viewFile (aFileName);
}

// =======================================================================
// function : onCloseAllBREPViews
// purpose :
// =======================================================================
void ShapeView_Window::onCloseAllBREPViews()
{
  removeBREPFiles();

  for (int aViewId = myBREPViews.size()-1; aViewId >= 0; aViewId--)
    delete myBREPViews[aViewId];

  myBREPViews.clear();
}

// =======================================================================
// function : onEditorDestroyed
// purpose :
// =======================================================================
void ShapeView_Window::onEditorDestroyed(QObject* theObject)
{
  QWidget* aWidget = dynamic_cast<QWidget*> (theObject);

  for (int aViewId = myBREPViews.size()-1; aViewId >= 0; aViewId--)
  {
    if (myBREPViews[aViewId] == aWidget)
      myBREPViews.removeAll(aWidget);
  }
}

// =======================================================================
// function : viewFile
// purpose :
// =======================================================================
void ShapeView_Window::viewFile (const QString& theFileName)
{
  QApplication::setOverrideCursor (Qt::WaitCursor);
  QString aFileText;
  QFile aFile (theFileName);
  if (aFile.open (QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream aStream(&aFile);
    QString aLine = aStream.readLine();
    while (!aLine.isNull())
    {
      aLine = aStream.readLine();
      aFileText.append (aLine + QString ("\n"));
    }
    if (!aFileText.isEmpty())
    {
      QPlainTextEdit* anEditor = new QPlainTextEdit (0);
      anEditor->setAttribute (Qt::WA_DeleteOnClose, true);
      connect (anEditor, SIGNAL (destroyed(QObject*)), this, SLOT (onEditorDestroyed(QObject*)));
      anEditor->setPlainText (aFileText);
      anEditor->resize (DEFAULT_TEXT_VIEW_WIDTH, DEFAULT_TEXT_VIEW_HEIGHT);
      anEditor->move (DEFAULT_TEXT_VIEW_POSITION_X + myNextPosition, DEFAULT_TEXT_VIEW_POSITION_Y);
      myNextPosition += DEFAULT_TEXT_VIEW_DELTA;
      anEditor->show();
      myBREPViews.append (anEditor);
    }
  }
  QApplication::restoreOverrideCursor();
}

// =======================================================================
// function : removeBREPFiles
// purpose :
// =======================================================================
void ShapeView_Window::removeBREPFiles()
{
  QDir aDir (GetTemporaryDirectory().ToCString());

  QStringList anEntries = aDir.entryList();
  QString aPrefix(viewBREPPrefix().ToCString());
  for (int anEntryId = 0, aSize = anEntries.size(); anEntryId < aSize; anEntryId++)
  {
    if (anEntries[anEntryId].contains (aPrefix))
      aDir.remove (anEntries[anEntryId]);
  }
}

// =======================================================================
// function : getNextTmpName
// purpose :
// =======================================================================
TCollection_AsciiString ShapeView_Window::getNextTmpName (const TCollection_AsciiString& thePointerInfo)
{
  TCollection_AsciiString aTmpName(viewBREPPrefix());
  aTmpName += thePointerInfo;
  return aTmpName;
}
