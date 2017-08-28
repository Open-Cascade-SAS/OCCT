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

#include <inspector/DFBrowser_Window.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListOfInteractive.hxx>

#include <CDF_Session.hxx>

#include <inspector/DFBrowserPane_AttributePaneAPI.hxx>

#include <inspector/DFBrowser_AttributePaneStack.hxx>
#include <inspector/DFBrowser_AttributePaneType.hxx>
#include <inspector/DFBrowser_DumpView.hxx>
#include <inspector/DFBrowser_Item.hxx>
#include <inspector/DFBrowser_ItemApplication.hxx>
#include <inspector/DFBrowser_Module.hxx>
#include <inspector/DFBrowser_OpenApplication.hxx>
#include <inspector/DFBrowser_PropertyPanel.hxx>
#include <inspector/DFBrowser_SearchLine.hxx>
#include <inspector/DFBrowser_SearchView.hxx>
#include <inspector/DFBrowser_Shortcut.hxx>
#include <inspector/DFBrowser_Thread.hxx>
#include <inspector/DFBrowser_ThreadItemSearch.hxx>
#include <inspector/DFBrowser_ThreadItemUsedShapesMap.hxx>
#include <inspector/DFBrowser_Tools.hxx>
#include <inspector/DFBrowser_TreeLevelLine.hxx>
#include <inspector/DFBrowser_TreeLevelView.hxx>
#include <inspector/DFBrowser_TreeModel.hxx>
#include <inspector/DFBrowser_TreeView.hxx>

#include <inspector/DFBrowserPane_AttributePaneSelector.hxx>
#include <inspector/DFBrowserPane_SelectionKind.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <OSD_Directory.hxx>
#include <OSD_Environment.hxx>
#include <OSD_Protection.hxx>

#include <inspector/View_Displayer.hxx>
#include <inspector/View_ToolBar.hxx>
#include <inspector/View_Viewer.hxx>
#include <inspector/View_Widget.hxx>
#include <inspector/View_Window.hxx>

#include <TDF_Tool.hxx>
#include <inspector/TreeModel_MessageDialog.hxx>

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QDockWidget>
#include <QGridLayout>
#include <QList>
#include <QMainWindow>
#include <QItemSelectionModel>
#include <QTabWidget>
#include <QToolBar>
#include <QTreeView>
#include <QMenu>
#include <QMessageBox>
#include <QStatusBar>
#include <QWidget>
#if QT_VERSION < 0x050000
#include <QWindowsStyle>
#else
#include <QStyleFactory>
#endif

const int DFBROWSER_DEFAULT_WIDTH = 650;
const int DFBROWSER_DEFAULT_HEIGHT = 350;
const int DFBROWSER_DEFAULT_TREE_VIEW_WIDTH = 325;
const int DFBROWSER_DEFAULT_TREE_VIEW_HEIGHT = 500;
const int DFBROWSER_DEFAULT_VIEW_WIDTH = 400;
const int DFBROWSER_DEFAULT_VIEW_HEIGHT = 300;

const int DFBROWSER_DEFAULT_POSITION_X = 200;
const int DFBROWSER_DEFAULT_POSITION_Y = 60;

const int OCAF_BROWSER_COLUMN_WIDTH_0 = 300;
const int DEFAULT_PROPERTY_PANEL_HEIGHT = 200;
const int DEFAULT_BROWSER_HEIGHT = 800;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_Window::DFBrowser_Window()
: myModule (0), myParent (0), myPropertyPanel (0), myExportToShapeViewDialog (0)
{
  myMainWindow = new QMainWindow (0);

  // tree view
  myTreeView = new DFBrowser_TreeView (myMainWindow);
  myTreeView->setContextMenuPolicy (Qt::CustomContextMenu);
  connect (myTreeView, SIGNAL (customContextMenuRequested (const QPoint&)),
           this, SLOT (onTreeViewContextMenuRequested (const QPoint&)));
  ((DFBrowser_TreeView*)myTreeView)->SetPredefinedSize (DFBROWSER_DEFAULT_TREE_VIEW_WIDTH,
                                                        DFBROWSER_DEFAULT_TREE_VIEW_HEIGHT);
  myTreeView->setHeaderHidden (true);
  myTreeView->setSortingEnabled (Standard_False);

  QDockWidget* aTreeViewWidget = new QDockWidget (tr ("TreeView"), myMainWindow);
  aTreeViewWidget->setTitleBarWidget (new QWidget(myMainWindow));
  aTreeViewWidget->setFeatures (QDockWidget::NoDockWidgetFeatures);
  aTreeViewWidget->setWidget (myTreeView);
  myMainWindow->addDockWidget (Qt::LeftDockWidgetArea, aTreeViewWidget);
  myMainWindow->setCorner (Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

#if QT_VERSION < 0x050000
  myTreeView->setStyle (new QWindowsStyle);
#else
  myTreeView->setStyle (QStyleFactory::create("Windows"));
#endif

  myTreeLevelLine = new DFBrowser_TreeLevelLine (myMainWindow);
  connect (myTreeLevelLine->GetSearchLine(), SIGNAL (searchActivated()), this, SLOT (onSearchActivated()));
  connect (myTreeLevelLine, SIGNAL (indexSelected (const QModelIndex&)),
          this, SLOT (onTreeLevelLineSelected (const QModelIndex&)));
  connect (myTreeLevelLine, SIGNAL (updateClicked()), this, SLOT (onUpdateClicked()));

  QDockWidget* aTreeLineDockWidget = new QDockWidget (tr ("Tree Level Line"), myMainWindow);
  aTreeLineDockWidget->setTitleBarWidget (new QWidget(myMainWindow));
  aTreeLineDockWidget->setFeatures (QDockWidget::NoDockWidgetFeatures);
  aTreeLineDockWidget->setWidget (myTreeLevelLine->GetControl());
  myMainWindow->addDockWidget (Qt::TopDockWidgetArea, aTreeLineDockWidget);

  // dump view window
  QWidget* aDumpWidget = new QWidget (myMainWindow);
  QVBoxLayout* aDumpLay = new QVBoxLayout (aDumpWidget);
  aDumpLay->setMargin (0);
  myDumpView = new DFBrowser_DumpView (aDumpWidget);
  aDumpLay->addWidget (myDumpView->GetControl());
  QDockWidget* aDumpDockWidget = new QDockWidget (tr ("Dump"), myMainWindow);

  aDumpDockWidget->setWidget (aDumpWidget);
  myMainWindow->addDockWidget (Qt::BottomDockWidgetArea, aDumpDockWidget);

  // property panel
  myPropertyPanel = new DFBrowser_PropertyPanel (myMainWindow);
  DFBrowser_AttributePaneStack* anAttributePaneStack = myPropertyPanel->GetAttributesStack();
  anAttributePaneStack->GetSearchView()->SetSearchLine (myTreeLevelLine->GetSearchLine());

  connect (anAttributePaneStack->GetPaneSelector(),
           SIGNAL (tableSelectionChanged (const QItemSelection&, const QItemSelection&, QItemSelectionModel*)),
           this, SLOT (onPaneSelectionChanged (const QItemSelection&, const QItemSelection&, QItemSelectionModel*)));

  DFBrowser_SearchView* aSearchView = anAttributePaneStack->GetSearchView();
  connect (aSearchView, SIGNAL (pathSelected (const QStringList&, const QString&)),
           this, SLOT (onSearchPathSelected (const QStringList&, const QString&)));
  connect (aSearchView, SIGNAL (pathDoubleClicked (const QStringList&, const QString&)),
           this, SLOT (onSearchPathDoubleClicked (const QStringList&, const QString&)));

  DFBrowser_TreeLevelView* aLevelView = anAttributePaneStack->GetTreeLevelView();
  connect (aLevelView, SIGNAL (indexSelected (const QModelIndex&)), this, SLOT (onLevelSelected (const QModelIndex&)));
  connect (aLevelView, SIGNAL (indexDoubleClicked (const QModelIndex&)),
           this, SLOT (onLevelDoubleClicked (const QModelIndex&)));

  myMainWindow->setCentralWidget (myPropertyPanel->GetControl());

  // view
  myViewWindow = new View_Window (myMainWindow);
  myViewWindow->GetView()->SetPredefinedSize (DFBROWSER_DEFAULT_VIEW_WIDTH, DFBROWSER_DEFAULT_VIEW_HEIGHT);

  QDockWidget* aViewDockWidget = new QDockWidget (tr ("View"), myMainWindow);
  aViewDockWidget->setTitleBarWidget (myViewWindow->GetViewToolBar()->GetControl());
  aViewDockWidget->setWidget (myViewWindow);
  myMainWindow->addDockWidget (Qt::BottomDockWidgetArea, aViewDockWidget);

  QColor aHColor (229, 243, 255);
  myViewWindow->GetDisplayer()->SetAttributeColor (Quantity_Color(aHColor.red() / 255., aHColor.green() / 255.,
                                                   aHColor.blue() / 255., Quantity_TOC_RGB), View_PresentationType_Additional);
  myMainWindow->tabifyDockWidget (aDumpDockWidget, aViewDockWidget);

  aTreeViewWidget->resize (DFBROWSER_DEFAULT_TREE_VIEW_WIDTH, DFBROWSER_DEFAULT_TREE_VIEW_HEIGHT);

  myThread = new DFBrowser_Thread (this);

  myShortcut = new DFBrowser_Shortcut (myMainWindow);
}

// =======================================================================
// function : Destructor
// purpose :
// =======================================================================
DFBrowser_Window::~DFBrowser_Window()
{
  delete myModule;
}

// =======================================================================
// function : SetParent
// purpose :
// =======================================================================
void DFBrowser_Window::SetParent (void* theParent)
{
  myParent = (QWidget*)theParent;
  if (myParent)
  {
    QLayout* aLayout = myParent->layout();
    if (aLayout)
      aLayout->addWidget (GetMainWindow());
  }
}

// =======================================================================
// function : UpdateContent
// purpose :
// =======================================================================
void DFBrowser_Window::UpdateContent()
{
  TCollection_AsciiString aName = "TKDFBrowser";

  if (myParameters->FindParameters (aName))
    Init(myParameters->Parameters (aName));
  else
    Init(NCollection_List<Handle(Standard_Transient)>());

  if (myParameters->FindFileNames(aName))
  {
    NCollection_List<TCollection_AsciiString> aFileNames = myParameters->FileNames (aName);
    if (aFileNames.Extent() > 0) // only one document file might be opened
      OpenFile (aFileNames.First());
    myParameters->SetFileNames(aName, NCollection_List<TCollection_AsciiString>());
  }
  onUpdateClicked();

  // make parameter items selected if defined
  if (myParameters->FindSelectedNames(aName))
  {
    const NCollection_List<TCollection_AsciiString>& aSelected = myParameters->GetSelectedNames (aName);

    DFBrowser_TreeModel* aTreeModel = dynamic_cast<DFBrowser_TreeModel*> (myTreeView->model());
    Handle(TDocStd_Application) anApplication = aTreeModel->GetTDocStdApplication();

    QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
    aSelectionModel->clear();

    NCollection_List<TCollection_AsciiString>::Iterator aSelectedIt (aSelected);
    if (aSelectedIt.More())
    {
      TCollection_AsciiString aLabelEntry = aSelectedIt.Value();

      TDF_Label aLabel;
      for (Standard_Integer aDocId = 1, aNbDoc = anApplication->NbDocuments(); aDocId <= aNbDoc; aDocId++)
      {
        Handle(TDocStd_Document) aDocument;
        anApplication->GetDocument (aDocId, aDocument);

        TDF_Tool::Label(aDocument->GetData(), aLabelEntry.ToCString(), aLabel, Standard_False);
        if (!aLabel.IsNull())
          break;
      }
      if (!aLabel.IsNull())
      {
        QModelIndex anIndexToBeSelected = aTreeModel->FindIndex (aLabel);

        TCollection_AsciiString anAttributeType;
        aSelectedIt.Next();
        // find attribute by attribute type on the given label
        if  (aSelectedIt.More())
        {
          anAttributeType = aSelectedIt.Value();

          TreeModel_ItemBasePtr aLabelItem = TreeModel_ModelBase::GetItemByIndex (anIndexToBeSelected);
          //DFBrowser_ItemPtr anItem = itemDynamicCast<DFBrowser_Item> (anItemBase);
          for (int aChildId = 0, aNbChildren = aLabelItem->rowCount(); aChildId < aNbChildren; aChildId++)
          {
            QModelIndex anIndex = aTreeModel->index (aChildId, 0, anIndexToBeSelected);
            TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
            DFBrowser_ItemPtr anItem = itemDynamicCast<DFBrowser_Item> (anItemBase);
            if (anItem->HasAttribute())
            {
              // processing attribute in theValue
              DFBrowser_ItemApplicationPtr aRootAppItem = itemDynamicCast<DFBrowser_ItemApplication>(aTreeModel->RootItem (0));
              QString anAttributeInfo = DFBrowser_Module::GetAttributeInfo (anItem->GetAttribute(), aRootAppItem->GetModule(),
                                                      Qt::DisplayRole, 0).toString();
              if (anAttributeInfo == anAttributeType.ToCString())
              {
                anIndexToBeSelected = anIndex;
                break;
              }
            }
          }
        }
        aSelectionModel->select (anIndexToBeSelected, QItemSelectionModel::Select);
        myTreeView->scrollTo (anIndexToBeSelected);
      }
    }

    myParameters->SetSelectedNames(aName, NCollection_List<TCollection_AsciiString>());
  }
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowser_Window::Init (const NCollection_List<Handle(Standard_Transient)>& theParameters)
{
  Handle(TDocStd_Application) anApplication;
  if (myModule)
  {
    DFBrowser_TreeModel* anOCAFViewModel = dynamic_cast<DFBrowser_TreeModel*> (myModule->GetOCAFViewModel());
    if (anOCAFViewModel)
      anApplication = anOCAFViewModel->GetTDocStdApplication();
  }
  Handle(AIS_InteractiveContext) aContext;
  if (myModule)
    aContext = myModule->GetExternalContext();

  bool aSameApplication = !anApplication.IsNull(), aSameContext = !aContext.IsNull();
  for (NCollection_List<Handle(Standard_Transient)>::Iterator aParametersIt (theParameters);
       aParametersIt.More(); aParametersIt.Next())
  {
    Handle(Standard_Transient) anObject = aParametersIt.Value();
    // check if the object is an application
    Handle(TDocStd_Application) anIApplication = Handle(TDocStd_Application)::DownCast (anObject);
    if (!anIApplication.IsNull())
    {
      aSameApplication = anApplication == anIApplication;
      if (!aSameApplication)
        anApplication = anIApplication;
    }
    // check if the object is an interactive context
    Handle(AIS_InteractiveContext) anIContext = Handle(AIS_InteractiveContext)::DownCast (anObject);
    if (!anIContext.IsNull())
    {
      aSameContext = aContext == anIContext;
      if (!aSameContext)
        aContext = anIContext;
    }
  }
  if (aSameApplication)
  {
    if (!aSameContext && !aContext.IsNull())
    {
      myModule->SetExternalContext (aContext);
      myViewWindow->SetContext (View_ContextType_External, aContext);
    }
    return;
  }

  myModule = new DFBrowser_Module();
  myModule->CreateViewModel (myMainWindow);

  myPropertyPanel->GetAttributesStack()->SetModule (myModule);

  // model should be set after the attribute pane stack is initialized by module
  QAbstractItemModel* aModel = myModule->GetOCAFViewModel();
  setOCAFModel (aModel);
  myModule->SetOCAFViewSelectionModel (myTreeView->selectionModel());
  myTreeLevelLine->GetSearchLine()->SetModule (myModule);
  myPropertyPanel->GetAttributesStack()->GetSearchView()->InitModels();

  myShortcut->SetModule (myModule);
  myThread->SetModule (myModule);

  connect (myModule, SIGNAL (beforeUpdateTreeModel()), this, SLOT (onBeforeUpdateTreeModel()));

  if (!aContext.IsNull())
  {
    myModule->SetExternalContext (aContext);
    myViewWindow->SetContext (View_ContextType_External, aContext);
  }

  myModule->SetApplication (anApplication);
  //! expand first three levels: CUSTOM
  QModelIndex aParentIndex = aModel->index (0, 0);
  setExpandedLevels (myTreeView, aParentIndex, 3/*levels*/);

  myThread->ProcessApplication();
  myModule->SetInitialTreeViewSelection();
}

// =======================================================================
// function : OpenFile
// purpose :
// =======================================================================
void DFBrowser_Window::OpenFile (const TCollection_AsciiString& theFileName)
{
  QApplication::setOverrideCursor (Qt::WaitCursor);
  myThread->TerminateThread();

  myTreeLevelLine->ClearHistory();
  QItemSelectionModel* aSelectionModel = myModule->GetOCAFViewSelectionModel();
  if (aSelectionModel)
  {
    aSelectionModel->clearSelection();
    QModelIndex anIndex;
    aSelectionModel->select (anIndex, QItemSelectionModel::ClearAndSelect);
  }
  ClearThreadCache();

  myTreeLevelLine->ClearHistory();

  DFBrowser_TreeModel* anOCAFViewModel = dynamic_cast<DFBrowser_TreeModel*> (myModule->GetOCAFViewModel());
  anOCAFViewModel->Reset();

  //! close previous documents to open new document
  Handle(TDocStd_Application) anApplication;
  if (CDF_Session::Exists())
  {
    Handle(CDF_Session) aSession = CDF_Session::CurrentSession();
    if (!aSession.IsNull())
    {
      anApplication = Handle(TDocStd_Application)::DownCast (CDF_Session::CurrentSession()->CurrentApplication());
      if (!anApplication.IsNull())
      {
        for (int aDocId = 1, aNbDocuments = anApplication->NbDocuments(); aDocId <= aNbDocuments; aDocId++)
        {
          Handle(TDocStd_Document) aDocument;
          anApplication->GetDocument (aDocId, aDocument);
          if (!aDocument.IsNull())
            anApplication->Close (aDocument);
        }
      }
    }
  }
  //! open new document
  bool isSTEPFileName = false;
  anApplication = DFBrowser_OpenApplication::OpenApplication (theFileName, isSTEPFileName);
  
  if (myParent)
    myParent->setObjectName (isSTEPFileName ? QString (TCollection_AsciiString (theFileName).ToCString()) : getWindowTitle());

  if (anApplication.IsNull())
  {
    QApplication::restoreOverrideCursor();
    QMessageBox::information (0, "Error", QString ("File %1 can't be opened by OCAF application")
                                                   .arg(TCollection_AsciiString (theFileName).ToCString()));
  }
  else {
    myModule->SetApplication (anApplication);
    //! expand first three levels: CUSTOM
    QModelIndex aParentIndex = anOCAFViewModel->index (0, 0);
    setExpandedLevels (myTreeView, aParentIndex, 3/*levels*/);

    myThread->ProcessApplication();
    myModule->SetInitialTreeViewSelection();
    QApplication::restoreOverrideCursor();
  }
}

// =======================================================================
// function : getWindowTitle
// purpose :
// =======================================================================
QString DFBrowser_Window::getWindowTitle() const
{
  DFBrowser_TreeModel* anOCAFViewModel = dynamic_cast<DFBrowser_TreeModel*> (myModule->GetOCAFViewModel());
  if (!anOCAFViewModel)
    return "";

  Handle(TDocStd_Application) anApplication = anOCAFViewModel->GetTDocStdApplication();
  if (anApplication.IsNull() || anApplication->NbDocuments() == 0)
    return "";

  Handle(TDocStd_Document) aDocument;
  anApplication->GetDocument (1, aDocument);
  if (aDocument.IsNull() || !aDocument->IsSaved())
    return "";

  return DFBrowserPane_Tools::ToString (aDocument->GetPath());
}

// =======================================================================
// function : setExpandedLevels
// purpose :
// =======================================================================
void DFBrowser_Window::setExpandedLevels (QTreeView* theTreeView, const QModelIndex& theParentIndex, const int theLevels)
{
  if (theLevels <= 0)
    return;

  QAbstractItemModel* aModel = theTreeView->model();
  if (!aModel)
    return;

  theTreeView->setExpanded (theParentIndex, true);
  for (int aRowId = 0, aRows = aModel->rowCount (theParentIndex); aRowId < aRows; aRowId++)
    setExpandedLevels (theTreeView, aModel->index (aRowId, 0, theParentIndex), theLevels - 1);
}

// =======================================================================
// function : setOCAFModel
// purpose :
// =======================================================================
void DFBrowser_Window::setOCAFModel (QAbstractItemModel* theModel)
{
  myTreeView->setModel (theModel);
  myTreeView->setColumnWidth (0, OCAF_BROWSER_COLUMN_WIDTH_0);

  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (theModel);
  myTreeView->setSelectionModel (aSelectionModel);

  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           myTreeLevelLine, SLOT (OnTreeViewSelectionChanged (const QItemSelection&, const QItemSelection&)));
  connect(aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
          myDumpView, SLOT (OnTreeViewSelectionChanged (const QItemSelection&, const QItemSelection&)));
  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           this, SLOT (onTreeViewSelectionChanged (const QItemSelection&, const QItemSelection&)));
}

// =======================================================================
// function : onBeforeUpdateTreeModel
// purpose :
// =======================================================================
void DFBrowser_Window::onBeforeUpdateTreeModel()
{
  myTreeLevelLine->ClearHistory();
  ClearThreadCache();
  myThread->ProcessApplication();
}

// =======================================================================
// function : ClearThreadCache
// purpose :
// =======================================================================
void DFBrowser_Window::ClearThreadCache()
{
  DFBrowser_ThreadItemUsedShapesMap::ClearSortedReferences (myModule);
  DFBrowser_ThreadItemSearch::ClearValues (GetTreeLevelLine()->GetSearchLine());
}

// =======================================================================
// function : SetWhiteBackground
// purpose :
// =======================================================================
void DFBrowser_Window::SetWhiteBackground (QWidget* theControl)
{
  QPalette aPalette = theControl->palette();
  aPalette.setColor (QPalette::All, QPalette::Foreground, Qt::white);
  theControl->setPalette (aPalette);
}

// =======================================================================
// function : TmpDirectory
// purpose :
// =======================================================================
TCollection_AsciiString DFBrowser_Window::TmpDirectory()
{
  TCollection_AsciiString aTmpDir;
#ifdef _WIN32
  OSD_Environment anEnvironment ("TEMP");
  aTmpDir = anEnvironment.Value();
  if (aTmpDir.IsEmpty() )
  {
    anEnvironment.SetName("TMP");
    aTmpDir = anEnvironment.Value();
    if (aTmpDir.IsEmpty())
      aTmpDir = "C:\\";
  }
  OSD_Path aTmpPath (aTmpDir);
  OSD_Directory aTmpDirectory (aTmpPath);
  if (!aTmpDirectory.Exists())
    aTmpDirectory.Build(OSD_Protection());
#else
  OSD_Directory aTmpDirectory = OSD_Directory::BuildTemporary();
  OSD_Path aTmpPath;
  aTmpDirectory.Path (aTmpPath);
  aTmpPath.SystemName(aTmpDir);
#endif

  return aTmpDir;
}

// =======================================================================
// function : SingleSelected
// purpose :
// =======================================================================
QModelIndex DFBrowser_Window::SingleSelected (const QModelIndexList& theIndices, const int theCellId,
                                              const Qt::Orientation theOrientation)
{
  QModelIndexList aFirstColumnSelectedIndices;
  for (QModelIndexList::const_iterator anIndicesIt = theIndices.begin(), aLast = theIndices.end();
       anIndicesIt != aLast; anIndicesIt++)
  {
    QModelIndex anIndex = *anIndicesIt;
    if ((theOrientation == Qt::Horizontal && anIndex.column() == theCellId) ||
        (theOrientation == Qt::Vertical && anIndex.row() == theCellId))
      aFirstColumnSelectedIndices.append (anIndex);
  }
  return aFirstColumnSelectedIndices.size() == 1 ? aFirstColumnSelectedIndices.first() : QModelIndex();
}

// =======================================================================
// function : onTreeViewContextMenuRequested
// purpose :
// =======================================================================
void DFBrowser_Window::onTreeViewContextMenuRequested (const QPoint& thePosition)
{
  QMenu* aMenu = new QMenu(GetMainWindow());
  aMenu->addAction (createAction (tr ("Expand"), SLOT (onExpand())));
  aMenu->addAction (createAction (tr ("Expand All"), SLOT (onExpandAll())));
  aMenu->addAction (createAction (tr ("Collapse All"), SLOT (onCollapseAll())));

  QPoint aPoint = myTreeView->mapToGlobal (thePosition);
  aMenu->exec (aPoint);
}

// =======================================================================
// function : createAction
// purpose :
// =======================================================================
QAction* DFBrowser_Window::createAction (const QString& theText, const char* theSlot)
{
  QAction* anAction = new QAction (theText, GetMainWindow());
  connect (anAction, SIGNAL (triggered(bool)), this, theSlot);
  return anAction;
}

// =======================================================================
// function : onExpand
// purpose :
// =======================================================================
void DFBrowser_Window::onExpand()
{
  QApplication::setOverrideCursor (Qt::WaitCursor);

  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
  for (int aSelectedId = 0, aSize = aSelectedIndices.size(); aSelectedId < aSize; aSelectedId++)
  {
    int aLevels = 2;
    setExpanded (myTreeView, aSelectedIndices[aSelectedId], true, aLevels);
  }
  QApplication::restoreOverrideCursor();
}

// =======================================================================
// function : onExpandAll
// purpose :
// =======================================================================
void DFBrowser_Window::onExpandAll()
{
  QApplication::setOverrideCursor (Qt::WaitCursor);

  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
  for (int  aSelectedId = 0, aSize = aSelectedIndices.size(); aSelectedId < aSize; aSelectedId++)
  {
    int aLevels = -1;
    setExpanded (myTreeView, aSelectedIndices[aSelectedId], true, aLevels);
  }
  QApplication::restoreOverrideCursor();
}

// =======================================================================
// function : onCollapseAll
// purpose :
// =======================================================================
void DFBrowser_Window::onCollapseAll()
{
  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
  for (int aSelectedId = 0, aSize = aSelectedIndices.size(); aSelectedId < aSize; aSelectedId++) {
    int aLevels = -1;
    setExpanded (myTreeView, aSelectedIndices[aSelectedId], false, aLevels);
  }
}

// =======================================================================
// function : onTreeViewSelectionChanged
// purpose :
// =======================================================================
void DFBrowser_Window::onTreeViewSelectionChanged (const QItemSelection& theSelected,
                                                   const QItemSelection& theDeselected)
{
  if (!myModule)
    return;
  // previuos selection should be cleared in the panel selectors
  DFBrowser_AttributePaneStack* anAttributePaneStack = myPropertyPanel->GetAttributesStack();
  anAttributePaneStack->GetPaneSelector()->ClearSelected();

  myPropertyPanel->UpdateBySelectionChanged (theSelected, theDeselected);
  anAttributePaneStack->GetTreeLevelView()->UpdateByTreeSelectionChanged (theSelected, theDeselected);

  QModelIndexList aSelectedIndices = theSelected.indexes();
  QModelIndex aSelectedIndex = DFBrowser_Window::SingleSelected (aSelectedIndices, 0);

  myTreeView->scrollTo (aSelectedIndex);
  View_Displayer* aDisplayer = myViewWindow->GetDisplayer();
  
  aDisplayer->ErasePresentations (View_PresentationType_Additional, false);
  aDisplayer->DisplayPresentation (findPresentation (aSelectedIndex), View_PresentationType_Main);
}

// =======================================================================
// function : onSearchActivated
// purpose :
// =======================================================================
void DFBrowser_Window::onSearchActivated()
{
  myPropertyPanel->GetAttributesStack()->SetPaneMode ((myTreeLevelLine->GetSearchLine()->Text().isEmpty()
                        ? DFBrowser_AttributePaneType_ItemView : DFBrowser_AttributePaneType_SearchView));
}

// =======================================================================
// function : onPaneSelectionChanged
// purpose :
// =======================================================================
void DFBrowser_Window::onPaneSelectionChanged (const QItemSelection&,
                                               const QItemSelection&,
                                               QItemSelectionModel* theModel)
{
  DFBrowserPane_AttributePaneAPI* anAttributePane = myPropertyPanel->GetAttributesStack()->GetCurrentPane();
  switch (anAttributePane->GetSelectionKind (theModel))
  {
    case DFBrowserPane_SelectionKind_ExportToShapeViewer:
    {
      QItemSelectionModel* aSelectionModel = theModel;
      QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
      if (aSelectedIndices.size() != 1)
        return;

      TCollection_AsciiString aPluginName ("TKShapeView");
      NCollection_List<Handle(Standard_Transient)> aParameters;
      if (myParameters->FindParameters (aPluginName))
        aParameters = myParameters->Parameters (aPluginName);

      int aParametersCount = aParameters.Extent();
      anAttributePane->GetSelectionParameters (aSelectionModel, aParameters);
      if (aParametersCount != aParameters.Extent()) // some TShapes are added
      {
        TCollection_AsciiString aPluginShortName = aPluginName.SubString (3, aPluginName.Length());
        QString aMessage = QString ("TShape %1 is sent to %2.")
          .arg (DFBrowserPane_Tools::GetPointerInfo (aParameters.Last()).ToCString())
          .arg (aPluginShortName.ToCString());
        QString aQuestion = QString ("Would you like to activate %1 immediately?\n")
          .arg (aPluginShortName.ToCString()).toStdString().c_str();
        if (!myExportToShapeViewDialog)
          myExportToShapeViewDialog = new TreeModel_MessageDialog (myParent, aMessage, aQuestion);
        else
          myExportToShapeViewDialog->SetInformation (aMessage);
        myExportToShapeViewDialog->Start();

        myParameters->SetParameters (aPluginName, aParameters, myExportToShapeViewDialog->IsAccepted());
      }
      return;
    }
    case DFBrowserPane_SelectionKind_ExportToBREP:
    case DFBrowserPane_SelectionKind_LabelReferences:
    case DFBrowserPane_SelectionKind_AttributeReferences:
    default: break;
  }

  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
  if (aSelectedIndices.size() != 1)
    return;

  // make the shape visualized
  QModelIndex aSelectedIndex = aSelectedIndices.first();
  View_Displayer* aDisplayer = myViewWindow->GetDisplayer();
  aDisplayer->DisplayPresentation (findPresentation (aSelectedIndex), View_PresentationType_Main);

  // highlight and scroll to the referenced item if it exists
  Handle(TDF_Attribute) anAttribute = myModule->FindAttribute (aSelectedIndex);
  NCollection_List<TDF_Label> aReferences;
  Handle(Standard_Transient) aPresentation;
  anAttributePane->GetReferences (anAttribute, aReferences, aPresentation);
  QModelIndexList anIndices;
  DFBrowser_TreeModel* aTreeModel = dynamic_cast<DFBrowser_TreeModel*> (myTreeView->model());
  if (!aReferences.IsEmpty())
    aTreeModel->ConvertToIndices (aReferences, anIndices);
  else {
    NCollection_List<Handle(TDF_Attribute)> anAttributeReferences;
    anAttributePane->GetAttributeReferences (anAttribute, anAttributeReferences, aPresentation);
    aTreeModel->ConvertToIndices (anAttributeReferences, anIndices);
  }
  highlightIndices (anIndices);
  // display either the reference presentation of the panel or find a presentation if the reference
  // is an attribute
  if (!aPresentation.IsNull())
    aDisplayer->DisplayPresentation (aPresentation, View_PresentationType_Additional);
  else {
    aDisplayer->ErasePresentations (View_PresentationType_Additional, false);
    AIS_ListOfInteractive aDisplayed;
    findPresentations (anIndices, aDisplayed);
    for (AIS_ListIteratorOfListOfInteractive aDisplayedIt (aDisplayed); aDisplayedIt.More(); aDisplayedIt.Next())
      aDisplayer->DisplayPresentation (aDisplayedIt.Value(), View_PresentationType_Additional, false);

    aDisplayer->UpdateViewer();
  }
}

// =======================================================================
// function : onTreeLevelLineSelected
// purpose :
// =======================================================================
void DFBrowser_Window::onTreeLevelLineSelected (const QModelIndex& theIndex)
{
  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  if (theIndex.isValid())
    aSelectionModel->select (theIndex, QItemSelectionModel::ClearAndSelect);
  else
    aSelectionModel->clearSelection();
}

// =======================================================================
// function : onUpdateClicked
// purpose :
// =======================================================================
void DFBrowser_Window::onUpdateClicked()
{
  if (myModule)
    myModule->UpdateTreeModel();
}

// =======================================================================
// function : onSearchPathSelected
// purpose :
// =======================================================================
void DFBrowser_Window::onSearchPathSelected (const QStringList& thePath, const QString& theValue)
{
  DFBrowser_TreeModel* aDFBrowserModel = dynamic_cast<DFBrowser_TreeModel*> (myTreeView->model());
  const QModelIndex& anIndex = aDFBrowserModel->FindIndexByPath (thePath, theValue);

  if (anIndex.isValid())
  {
    QModelIndexList anIndices;
    anIndices.append (anIndex);
    highlightIndices (anIndices);
  }
}

// =======================================================================
// function : onSearchPathDoubleClicked
// purpose :
// =======================================================================
void DFBrowser_Window::onSearchPathDoubleClicked (const QStringList& thePath, const QString& theValue)
{
  DFBrowser_TreeModel* aDFBrowserModel = dynamic_cast<DFBrowser_TreeModel*> (myTreeView->model());
  const QModelIndex& anIndex = aDFBrowserModel->FindIndexByPath (thePath, theValue);

  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  if (anIndex.isValid())
    aSelectionModel->select (anIndex, QItemSelectionModel::ClearAndSelect);
  else
    aSelectionModel->clearSelection();
}

// =======================================================================
// function : onLevelSelected
// purpose :
// =======================================================================
void DFBrowser_Window::onLevelSelected (const QModelIndex& theIndex)
{
  if (!theIndex.isValid())
    return;

  QModelIndexList anIndices;
  anIndices.append (theIndex);
  highlightIndices (anIndices);
  View_Displayer* aDisplayer = myViewWindow->GetDisplayer();
  aDisplayer->ErasePresentations (View_PresentationType_Additional, false);
  aDisplayer->DisplayPresentation (findPresentation (theIndex), View_PresentationType_Main);
}

// =======================================================================
// function : onLevelDoubleClicked
// purpose :
// =======================================================================
void DFBrowser_Window::onLevelDoubleClicked (const QModelIndex& theIndex)
{
  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  if (theIndex.isValid())
    aSelectionModel->select (theIndex, QItemSelectionModel::ClearAndSelect);
  else
    aSelectionModel->clearSelection();
}

// =======================================================================
// function : highlightIndices
// purpose :
// =======================================================================
void DFBrowser_Window::highlightIndices (const QModelIndexList& theIndices)
{
  QAbstractItemModel* aModel = myTreeView->model();
  if (!aModel)
    return;

  DFBrowser_TreeModel* aTreeModel = dynamic_cast<DFBrowser_TreeModel*> (aModel);
  if (!aTreeModel)
    return;

  aTreeModel->SetHighlighted (theIndices);

  QModelIndex anIndexToScroll;
  if (!theIndices.isEmpty())
    anIndexToScroll = theIndices.last(); // scroll to last selected index
  else
  {
    // scroll to tree selected item
    QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
    QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
    if (aSelectedIndices.size() == 1)
      anIndexToScroll = aSelectedIndices.first();
  }
  if (anIndexToScroll.isValid())
    myTreeView->scrollTo (anIndexToScroll);

  if (theIndices.isEmpty())
    myTreeView->setFocus(); // to see the selected item in active palette color

  aTreeModel->EmitLayoutChanged();
}

// =======================================================================
// function : findPresentation
// purpose :
// =======================================================================
Handle(AIS_InteractiveObject) DFBrowser_Window::findPresentation (const QModelIndex& theIndex)
{
  Handle(AIS_InteractiveObject) aPresentation;

  QModelIndexList anIndices;
  anIndices.append (theIndex);
  AIS_ListOfInteractive aDisplayed;
  findPresentations (anIndices, aDisplayed);
  if (!aDisplayed.IsEmpty())
    aPresentation = aDisplayed.First();

  return aPresentation;
}

// =======================================================================
// function : findPresentations
// purpose :
// =======================================================================
void DFBrowser_Window::findPresentations (const QModelIndexList& theIndices, AIS_ListOfInteractive& thePresentations)
{
  for (int anIndexId = 0, aCount = theIndices.size(); anIndexId < aCount; anIndexId++)
  {
    Handle(AIS_InteractiveObject) aPresentation;
    Handle(TDF_Attribute) anAttribute = myModule->FindAttribute (theIndices[anIndexId]);
    if (anAttribute.IsNull())
      continue;
    DFBrowserPane_AttributePaneAPI* anAttributePane = myModule->GetAttributePane (anAttribute);
    if (!anAttributePane)
      continue;
    aPresentation = Handle(AIS_InteractiveObject)::DownCast (anAttributePane->GetPresentation (anAttribute));
    if (aPresentation.IsNull())
      continue;

    thePresentations.Append (aPresentation);
  }
}

// =======================================================================
// function : setExpanded
// purpose :
// =======================================================================
void DFBrowser_Window::setExpanded (QTreeView* theTreeView, const QModelIndex& theIndex, const bool isExpanded,
                                    int& theLevels)
{
  bool isToExpand = theLevels == -1 || theLevels > 0;
  if (!isToExpand)
    return;

  theTreeView->setExpanded (theIndex, isExpanded);
  if (theLevels != -1)
    theLevels--;

  QAbstractItemModel* aModel = theTreeView->model();
  for (int aRowId = 0, aRows = aModel->rowCount (theIndex); aRowId < aRows; aRowId++)
  {
    int aLevels = theLevels;
    setExpanded (theTreeView, aModel->index (aRowId, 0, theIndex), isExpanded, aLevels);
  }
}
