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
#include <inspector/View_Displayer.hxx>
#include <inspector/View_ToolBar.hxx>
#include <inspector/View_Widget.hxx>
#include <inspector/View_Window.hxx>
#include <inspector/View_Viewer.hxx>

#include <inspector/ShapeView_Window.hxx>
#include <inspector/ShapeView_ItemRoot.hxx>
#include <inspector/ShapeView_ItemShape.hxx>
#include <inspector/ShapeView_TreeModel.hxx>
#include <inspector/ShapeView_OpenFileDialog.hxx>
#include <inspector/ShapeView_Tools.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

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

const int FIRST_COLUMN_WIDTH = 190;
const int SIZE_COLUMN_WIDTH = 30;
const int POINTER_COLUMN_WIDTH = 70;
const int ORIENTATION_COLUMN_WIDTH = 70;
const int LOCATION_COLUMN_WIDTH = 120;
const int FLAGS_COLUMN_WIDTH = 70;

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

const int SHAPEVIEW_DEFAULT_VIEW_WIDTH = 200;//400;
const int SHAPEVIEW_DEFAULT_VIEW_HEIGHT = 1000;

//! \class ShapeView_TreeView
//! Extended tree view control with possibility to set predefined size.
class ShapeView_TreeView : public QTreeView
{
public:
  //! Constructor
  ShapeView_TreeView (QWidget* theParent) : QTreeView (theParent), myDefaultWidth (-1), myDefaultHeight (-1) {}

  //! Destructor
  virtual ~ShapeView_TreeView() {}

  //! Sets default size of control, that is used by the first control show
  //! \param theDefaultWidth the width value
  //! \param theDefaultHeight the height value
  void SetPredefinedSize (int theDefaultWidth, int theDefaultHeight);

  //! Returns predefined size if both values are positive, otherwise parent size hint
  virtual QSize sizeHint() const Standard_OVERRIDE;

private:

  int myDefaultWidth; //!< default width, -1 if it should not be used
  int myDefaultHeight; //!< default height, -1 if it should not be used
};

// =======================================================================
// function : SetPredefinedSize
// purpose :
// =======================================================================
void ShapeView_TreeView::SetPredefinedSize (int theDefaultWidth, int theDefaultHeight)
{
  myDefaultWidth = theDefaultWidth;
  myDefaultHeight = theDefaultHeight;
}

// =======================================================================
// function : sizeHint
// purpose :
// =======================================================================
QSize ShapeView_TreeView::sizeHint() const
{
  if (myDefaultWidth > 0 && myDefaultHeight > 0)
    return QSize (myDefaultWidth, myDefaultHeight);
  return QTreeView::sizeHint();
}

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
ShapeView_Window::ShapeView_Window (QWidget* theParent, const TCollection_AsciiString& theTemporaryDirectory)
: QObject (theParent), myTemporaryDirectory (theTemporaryDirectory), myNextPosition (0)
{
  myMainWindow = new QMainWindow (theParent);

  myTreeView = new ShapeView_TreeView (myMainWindow);
  ((ShapeView_TreeView*)myTreeView)->SetPredefinedSize (SHAPEVIEW_DEFAULT_TREE_VIEW_WIDTH,
                                                        SHAPEVIEW_DEFAULT_TREE_VIEW_HEIGHT);
  myTreeView->setContextMenuPolicy (Qt::CustomContextMenu);
  connect (myTreeView, SIGNAL (customContextMenuRequested (const QPoint&)),
          this, SLOT (onTreeViewContextMenuRequested (const QPoint&)));
  ShapeView_TreeModel* aModel = new ShapeView_TreeModel (myTreeView);
  myTreeView->setModel (aModel);

  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (aModel);
  myTreeView->setSelectionMode (QAbstractItemView::ExtendedSelection);
  myTreeView->setSelectionModel (aSelectionModel);
  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
    this, SLOT (onTreeViewSelectionChanged (const QItemSelection&, const QItemSelection&)));

  QModelIndex aParentIndex = myTreeView->model()->index (0, 0);
  myTreeView->setExpanded (aParentIndex, true);
  myTreeView->setColumnWidth (0, FIRST_COLUMN_WIDTH);
  myTreeView->setColumnWidth (1, SIZE_COLUMN_WIDTH);
  myTreeView->setColumnWidth (2, POINTER_COLUMN_WIDTH);
  myTreeView->setColumnWidth (3, ORIENTATION_COLUMN_WIDTH);
  myTreeView->setColumnWidth (4, LOCATION_COLUMN_WIDTH);
  myTreeView->setColumnWidth (5, FLAGS_COLUMN_WIDTH);

  myMainWindow->setCentralWidget (myTreeView);

  // view
  myViewWindow = new View_Window (myMainWindow);
  myViewWindow->GetView()->SetPredefinedSize (SHAPEVIEW_DEFAULT_VIEW_WIDTH, SHAPEVIEW_DEFAULT_VIEW_HEIGHT);

  QDockWidget* aViewDockWidget = new QDockWidget (tr ("View"), myMainWindow);
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
  for (NCollection_List<Handle(Standard_Transient)>::Iterator aParamsIt (theParameters);
       aParamsIt.More(); aParamsIt.Next())
  {
    Handle(Standard_Transient) anObject = aParamsIt.Value();
    Handle(TopoDS_TShape) aShapePointer = Handle(TopoDS_TShape)::DownCast (anObject);
    if (!aShapePointer.IsNull())
    {
      TopoDS_Shape aShape;
      aShape.TShape (aShapePointer);
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

  QModelIndex anIndex = ShapeView_TreeModel::SingleSelected (aModel->selectedIndexes(), 0);
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
  if (!anItemBase)
    return;

  QMenu* aMenu = new QMenu(myMainWindow);
  ShapeView_ItemRootPtr aRootItem = itemDynamicCast<ShapeView_ItemRoot> (anItemBase);
  if (aRootItem) {
    aMenu->addAction (createAction("Load BREP file", SLOT (onLoadFile())));
    aMenu->addAction (createAction ("Remove all shape items", SLOT (onClearView())));
  }
  else {
    if (!myTemporaryDirectory.IsEmpty())
      aMenu->addAction (createAction ("BREP view", SLOT (onBREPView())));
    aMenu->addAction (createAction ("Close All BREP views", SLOT (onCloseAllBREPViews())));
    aMenu->addAction (createAction ("BREP directory", SLOT (onBREPDirectory())));
  }
  QPoint aPoint = myTreeView->mapToGlobal (thePosition);
  aMenu->exec (aPoint);
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
                                                    myTemporaryDirectory.ToCString(), aSelectedFilter);
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
  if (myTemporaryDirectory.IsEmpty())
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
    TCollection_AsciiString aFileNameIndiced = myTemporaryDirectory + TCollection_AsciiString ("\\") +
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
// function : displaySelectedShapes
// purpose :
// =======================================================================
void ShapeView_Window::displaySelectedShapes (const QModelIndexList& theSelected)
{
  for (QModelIndexList::const_iterator aSelIt = theSelected.begin(); aSelIt != theSelected.end(); aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() != 0)
      continue;

    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    if (!anItemBase)
      continue;

    ShapeView_ItemShapePtr aShapeItem = itemDynamicCast<ShapeView_ItemShape>(anItemBase);
    if (!aShapeItem)
      continue;
    TopoDS_Shape aShape = aShapeItem->GetItemShape();
    myViewWindow->GetDisplayer()->DisplayPresentation (ShapeView_Tools::CreatePresentation(aShape),
                                                       View_PresentationType_Main, true);
  }
  myViewWindow->GetDisplayer()->UpdateViewer();
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
  QDir aDir (myTemporaryDirectory.ToCString());

  QStringList anEntries = aDir.entryList();
  QString aPrefix(viewBREPPrefix().ToCString());
  for (int anEntryId = 0, aSize = anEntries.size(); anEntryId < aSize; anEntryId++)
  {
    if (anEntries[anEntryId].contains (aPrefix))
      aDir.remove (anEntries[anEntryId]);
  }
}

// =======================================================================
// function : createAction
// purpose :
// =======================================================================
QAction* ShapeView_Window::createAction (const QString& theText, const char* theSlot)
{
  QAction* anAction = new QAction (theText, myMainWindow);
  connect (anAction, SIGNAL (triggered(bool)), this, theSlot);
  return anAction;
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
