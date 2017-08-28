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

#ifndef DFBrowser_Window_H
#define DFBrowser_Window_H

#include <inspector/TInspectorAPI_PluginParameters.hxx>

#include <AIS_InteractiveObject.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <NCollection_List.hxx>
#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QObject>
#include <QItemSelection>
#include <QMainWindow>
#include <QModelIndexList>

class DFBrowser_DumpView;
class DFBrowser_Module;
class DFBrowser_PropertyPanel;
class DFBrowser_Shortcut;
class DFBrowser_Thread;
class DFBrowser_TreeLevelLine;

class TreeModel_MessageDialog;

class View_ToolBar;
class View_Window;

class QAbstractItemModel;
class QAction;
class QTreeView;
class QWidget;

//! \class DFBrowser_Window
//! Window that unites all DFBrowser controls.
//! External functionality : it processes plugin parameters, updates controls content and places itself in parent layout.
//! It Synchronizes controls content depending on current selection.
//! It shows context popup menu for OCAF tree view.
class DFBrowser_Window : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowser_Window();

  //! Destructor
  Standard_EXPORT virtual ~DFBrowser_Window() Standard_OVERRIDE;

  //! Appends main window into layout of the parent if the parent is child of QWidget
  //! \param theParent a parent class
  Standard_EXPORT void SetParent (void* theParent);

  //! Sets parameters container, it should be used when the plugin is initialized or in update content
  //! \param theParameters a parameters container
  void SetParameters (const Handle(TInspectorAPI_PluginParameters)& theParameters) { myParameters = theParameters; }

  //! Applyes parameters to Init controls, opens files if there are in parameters, updates OCAF tree view model
  Standard_EXPORT void UpdateContent();

  //! Fills controls of the plugin by parameters:
  //! - Find TDocStd_Application and fills OCAF tree model if it differs from the current application
  //! - Fine AIS_InteractiveObject and fills View if it if it differs from the current context
  //! - If it is the first call, it creates module, start thread to cache application information, fills selection models
  //! \param theParameters a parameters container
  Standard_EXPORT void Init (const NCollection_List<Handle(Standard_Transient)>& theParameters);

  //! Opens application by the name, it may be either OCAF document or STEP file.
  //! Before opening it cleans tree view history, current selections, stop threads(if it was started),
  //! reset OCAF tree view model. After opening document, it fills all controls by the created application.
  //! \param theFileName a file name to be opened
  Standard_EXPORT void OpenFile (const TCollection_AsciiString& theFileName);

  //! Returns main control
  QWidget* GetMainWindow() const { return myMainWindow; }

  //! Returns the current module
  DFBrowser_Module* GetModule() const { return myModule; }

  //! Clears thread cache
  Standard_EXPORT void ClearThreadCache();

  //! Returns tree level line control
  DFBrowser_TreeLevelLine* GetTreeLevelLine() const { return myTreeLevelLine; }

  //! Change palette of the widget to have white foreground
  //! \param theControl a widget to be modified
  Standard_EXPORT static void SetWhiteBackground (QWidget* theControl);

  //! Returns temporary directory defined by environment variables TEMP or TMP
  //! \return string value
  Standard_EXPORT static TCollection_AsciiString TmpDirectory();

  //! Returns single selected item in the cell of given orientation. If the orientation is Horizontal,
  //! in the cell id colum, one row should be selected.
  //! \param theIndices a container of selected indices
  //! \param theCellId column index if orientation is horizontal, row index otherwise
  //! \param theOrientation an orientation to apply the cell index
  //! \return model index from the list
  Standard_EXPORT static QModelIndex SingleSelected (const QModelIndexList& theIndices, const int theCellId,
                                                     const Qt::Orientation theOrientation = Qt::Horizontal);
private slots:

  //! Cleans history in tree level line, clears cache of thread processing, starts threads for application
  void onBeforeUpdateTreeModel();

  //! Shows context menu for tree view selected item. It contains clear view or BREP operations items
  //! \param thePosition a clicked point
  void onTreeViewContextMenuRequested (const QPoint& thePosition);

  //! Expand two next levels for all selected item
  void onExpand();

  //! Expand all levels for all selected items
  void onExpandAll();

  //! Collapse all levels for all selected items
  void onCollapseAll();

  //! Udpates all controls by changed selection in OCAF tree view
  //! \param theSelected list of selected tree view items
  //! \param theDeselected list of deselected tree view items
  void onTreeViewSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected);

  //! Changes attribute pane stack content depending on search control text
  void onSearchActivated();

  //! Processes selection change in attribute pane. Depending on selection kind, it will:
  //! - export to shape viewer
  //! - display presentation of the pane
  //! - display references
  void onPaneSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected,
                               QItemSelectionModel* theModel);

  //! Selects the item in OCAF tree view
  //! \param theIndex OCAF tree view index
  void onTreeLevelLineSelected (const QModelIndex& theIndex);

  //! Updates OCAF tree model
  void onUpdateClicked();

  //! Higlights OCAF tree model item
  //! \param thePath a container of entries to the item
  //! \param theValue a label entry or attribute name
  void onSearchPathSelected (const QStringList& thePath, const QString& theValue);

  //! Selects OCAF tree model item
  //! \param thePath a container of entries to the item
  //! \param theValue a label entry or attribute name
  void onSearchPathDoubleClicked (const QStringList& thePath, const QString& theValue);

  //! Higlights OCAF tree model item
  //! \param theIndex an OCAF tree model index
  void onLevelSelected (const QModelIndex& theIndex);

  //! Selects OCAF tree model item
  //! \param theIndex an OCAF tree model index
  void onLevelDoubleClicked (const QModelIndex& theIndex);

private:

  //! Inits OCAF tree view with the given model
  //! \param theModel a model
  void setOCAFModel (QAbstractItemModel* theModel);

  //! Sets expanded levels in OCAF tree view. Do recursive expand of items.
  //! \param theTreeView an OCAF tree view
  //! \param theParentIndex an index which children should be expanded
  //! \param theLevels a number of levels to be expanded, or -1 for all levels
  static void setExpandedLevels (QTreeView* theTreeView, const QModelIndex& theParentIndex, const int theLevels);

  //! Marks items highlighted in OCAF tree view model and move view scroll to the first item
  //! \param theIndices a container of OCAF tree view model indices
  void highlightIndices (const QModelIndexList& theIndices);

  //! Creates an action with the given text connected to the slot
  //! \param theText an action text value
  //! \param theSlot a listener of triggered signal of the new action
  //! \return a new action
  QAction* createAction (const QString& theText, const char* theSlot);

  //! Returns candidate to be the window title. It is either name of opened STEP file or the application path
  //! \return string value
  QString getWindowTitle() const;

protected:

  //! Returns presentation for the OCAF tree model index. To do this, it uses attribute pane for this item
  //! \param theIndex a model index
  //! \return presentation or NULL
  Handle(AIS_InteractiveObject) findPresentation (const QModelIndex& theIndex);

  //! Returns presentations for the OCAF tree model indices. To do this, it uses attribute pane for this items
  //! \param theIndex a model index
  //! \return container of presentations or NULL
  void findPresentations (const QModelIndexList& theIndices, AIS_ListOfInteractive& thePresentations);

  //! Recursive items expanding in tree view staring from the index
  //! \param theTreeView an OCAF tree view
  //! \param theParentIndex an index which children should be expanded
  //! \param isExpanded a boolean state if the item should be expanded or collapsed
  //! \param theLevels a number of levels to be expanded, or -1 for all levels
  static void setExpanded (QTreeView* theTreeView, const QModelIndex& theParentIndex, const bool isExpanded, int& theLevels);

private:

  DFBrowser_Module* myModule; //!< current module
  QWidget* myParent; //!< widget, comes when Init window, the window control lays in the layout, updates window title
  QMainWindow* myMainWindow; //!< main control for all components
  DFBrowser_TreeLevelLine* myTreeLevelLine; //!< navigate line of tree levels to the selected item
  QTreeView* myTreeView; //!< OCAF tree view
  DFBrowser_PropertyPanel* myPropertyPanel; //!< property panel shows full information about attribute or search view
  View_Window* myViewWindow; //!< V3d view to visualize presentations/references if it can be build for a selected item
  DFBrowser_DumpView* myDumpView; //!< Text editor where "Dump" method output is shown
  DFBrowser_Thread* myThread; //!< Threads manipulator, starting thread items, listens finalizing
  DFBrowser_Shortcut* myShortcut; //!< Short cut processor, F5 - updates OCAF view model content
  TreeModel_MessageDialog* myExportToShapeViewDialog; //!< dialog about exporting TopoDS_Shape to ShapeView plugin
  Handle(TInspectorAPI_PluginParameters) myParameters; //!< contains application, context, files that should be opened
};

#endif
