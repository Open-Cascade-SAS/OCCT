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

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QItemSelection>
#include <QMainWindow>
#include <QModelIndexList>
#include <Standard_WarningsRestore.hxx>

class DFBrowser_DumpView;
class DFBrowser_Module;
class DFBrowser_PropertyPanel;
class DFBrowser_TreeLevelLine;

class ViewControl_MessageDialog;
class ViewControl_PropertyView;

class View_ToolBar;
class View_Window;

class QAbstractItemModel;
class QAction;
class QDockWidget;
class QTreeView;
class QWidget;

//! \class DFBrowser_Window
//! Window that unites all DFBrowser controls.
//! External functionality : it processes plugin parameters, updates controls content and places
//! itself in parent layout. It Synchronizes controls content depending on current selection. It
//! shows context popup menu for OCAF tree view.
class DFBrowser_Window : public QObject
{
  Q_OBJECT
public:
  //! Constructor
  Standard_EXPORT DFBrowser_Window();

  //! Destructor
  Standard_EXPORT virtual ~DFBrowser_Window();

  //! Appends main window into layout of the parent if the parent is child of QWidget
  //! \param theParent a parent class
  Standard_EXPORT void SetParent(void* theParent);

  //! Sets parameters container, it should be used when the plugin is initialized or in update
  //! content \param theParameters a parameters container
  void SetParameters(const Handle(TInspectorAPI_PluginParameters)& theParameters)
  {
    myParameters = theParameters;
  }

  //! Provides container for actions available in inspector on general level
  //! \param theMenu if Qt implementation, it is QMenu object
  Standard_EXPORT virtual void FillActionsMenu(void* theMenu);

  //! Returns plugin preferences: dock widgets state, tree view columns.
  //! \param theItem container of preference elements
  Standard_EXPORT void GetPreferences(TInspectorAPI_PreferencesDataMap& theItem);

  //! Applies plugin preferences
  //! \param theItem container of preference elements
  Standard_EXPORT void SetPreferences(const TInspectorAPI_PreferencesDataMap& theItem);

  //! Applies parameters to Init controls, opens files if there are in parameters, updates OCAF tree
  //! view model
  Standard_EXPORT void UpdateContent();

  //! Fills controls of the plugin by parameters:
  //! - Find TDocStd_Application and fills OCAF tree model if it differs from the current
  //! application
  //! - Fine AIS_InteractiveObject and fills View if it if it differs from the current context
  //! - If it is the first call, it creates module, fills selection models
  //! \param theParameters a parameters container
  Standard_EXPORT void Init(const NCollection_List<Handle(Standard_Transient)>& theParameters);

  //! Opens application by the name, it may be either OCAF document or STEP file.
  //! Before opening it cleans tree view history, current selections,
  //! reset OCAF tree view model. After opening document, it fills all controls by the created
  //! application. \param theFileName a file name to be opened
  Standard_EXPORT void OpenFile(const TCollection_AsciiString& theFileName);

  //! Returns main control
  QWidget* GetMainWindow() const { return myMainWindow; }

  //! Returns the current module
  DFBrowser_Module* GetModule() const { return myModule; }

  //! Returns tree level line control
  DFBrowser_TreeLevelLine* GetTreeLevelLine() const { return myTreeLevelLine; }

  //! Returns temporary directory defined by environment variables TEMP or TMP
  //! \return string value
  Standard_EXPORT static TCollection_AsciiString TmpDirectory();

  //! Sets whether DumpJson is used when the tree view is generated
  Standard_EXPORT static void SetUseDumpJson(const Standard_Boolean theValue);

  //! Returns whether DumpJson is used when the tree view is generated
  Standard_EXPORT static Standard_Boolean IsUseDumpJson();

private slots:

  //! Cleans history in tree level line
  void onBeforeUpdateTreeModel();

  //! Shows context menu for tree view selected item. It contains clear view or BREP operations
  //! items \param thePosition a clicked point
  void onTreeViewContextMenuRequested(const QPoint& thePosition);

  //! Expands two next levels for all selected item
  void onExpand();

  //! Expands all levels for all selected items
  void onExpandAll();

  //! Collapses all levels for all selected items
  void onCollapseAll();

  //! Setting flag whether DumpJSon should be applied to build tree model items structure
  void onUseDumpJson();

  //! Updates all controls by changed selection in OCAF tree view
  //! \param theSelected list of selected tree view items
  //! \param theDeselected list of deselected tree view items
  void onTreeViewSelectionChanged(const QItemSelection& theSelected,
                                  const QItemSelection& theDeselected);

  //! Changes attribute pane stack content depending on search control text
  void onSearchActivated();

  //! Processes selection change in attribute pane. Depending on selection kind, it will:
  //! - export to shape viewer
  //! - display presentation of the pane
  //! - display references
  void onPaneSelectionChanged(const QItemSelection& theSelected,
                              const QItemSelection& theDeselected,
                              QItemSelectionModel*  theModel);

  //! Selects the item in OCAF tree view
  //! \param theIndex OCAF tree view index
  void onTreeLevelLineSelected(const QModelIndex& theIndex);

  //! Updates OCAF tree model
  void onUpdateClicked();

  //! Highlights OCAF tree model item
  //! \param thePath a container of entries to the item
  //! \param theValue a label entry or attribute name
  void onSearchPathSelected(const QStringList& thePath, const QString& theValue);

  //! Selects OCAF tree model item
  //! \param thePath a container of entries to the item
  //! \param theValue a label entry or attribute name
  void onSearchPathDoubleClicked(const QStringList& thePath, const QString& theValue);

  //! Highlights OCAF tree model item
  //! \param theIndex an OCAF tree model index
  void onLevelSelected(const QModelIndex& theIndex);

  //! Selects OCAF tree model item
  //! \param theIndex an OCAF tree model index
  void onLevelDoubleClicked(const QModelIndex& theIndex);

private:
  //! Inits OCAF tree view with the given model
  //! \param theModel a model
  void setOCAFModel(QAbstractItemModel* theModel);

  //! Sets expanded levels in OCAF tree view. Do recursive expand of items.
  //! \param theTreeView an OCAF tree view
  //! \param theParentIndex an index which children should be expanded
  //! \param theLevels a number of levels to be expanded, or -1 for all levels
  static void setExpandedLevels(QTreeView*         theTreeView,
                                const QModelIndex& theParentIndex,
                                const int          theLevels);

  //! Marks items highlighted in OCAF tree view model and move view scroll to the first item
  //! \param theIndices a container of OCAF tree view model indices
  void highlightIndices(const QModelIndexList& theIndices);

  //! Returns candidate to be the window title. It is either name of opened STEP file or the
  //! application path \return string value
  QString getWindowTitle() const;

protected:
  //! Returns presentation for the OCAF tree model index. To do this, it uses attribute pane for
  //! this item \param theIndex a model index \return presentation or NULL
  Handle(AIS_InteractiveObject) findPresentation(const QModelIndex& theIndex);

  //! Returns presentations for the OCAF tree model indices. To do this, it uses attribute pane for
  //! this items \param theIndex a model index \return container of presentations or NULL
  void findPresentations(const QModelIndexList& theIndices,
                         AIS_ListOfInteractive& thePresentations);

  //! Updates content of Property Panel dock widget. It contains button to activate DumpJson or view
  //! with content of it.
  void updatePropertyPanelWidget();

private:
  DFBrowser_Module* myModule; //!< current module
  QWidget* myParent; //!< widget, comes when Init window, the window control lays in the layout,
                     //!< updates window title
  QMainWindow*             myMainWindow;    //!< main control for all components
  DFBrowser_TreeLevelLine* myTreeLevelLine; //!< navigate line of tree levels to the selected item
  QTreeView*               myTreeView;      //!< OCAF tree view
  QDockWidget*             myPropertyPanelWidget; //!< property pane dockable widget
  QWidget*                 myUseDumpJson; //!< button to activate/deactivate using of DumpJson
  DFBrowser_PropertyPanel*
    myPropertyPanel; //!< property panel shows full information about attribute or search view
  ViewControl_PropertyView*
               myPropertyView; //!< property control to display model item values if exist
  View_Window* myViewWindow; //!< V3d view to visualize presentations/references if it can be build
                             //!< for a selected item
  DFBrowser_DumpView* myDumpView; //!< Text editor where "Dump" method output is shown
  ViewControl_MessageDialog*
    myExportToShapeViewDialog; //!< dialog about exporting TopoDS_Shape to ShapeView plugin
  Handle(TInspectorAPI_PluginParameters)
          myParameters;     //!< contains application, context, files that should be opened
  QString myOpenedFileName; //!< cached name of opened file between parent is set, apply it by
                            //!< parent setting and nullify
};

#endif
