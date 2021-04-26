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

#ifndef VInspector_Window_H
#define VInspector_Window_H

#include <AIS_InteractiveContext.hxx>
#include <NCollection_List.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <inspector/TInspectorAPI_PluginParameters.hxx>
#include <inspector/View_DisplayActionType.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QItemSelection>
#include <QMainWindow>
#include <Standard_WarningsRestore.hxx>

class ViewControl_MessageDialog;
class ViewControl_PropertyView;

class VInspector_ToolBar;

class View_Displayer;
class View_Window;

class QAbstractItemModel;
class QAction;
class QDockWidget;
class QTreeView;
class QWidget;

//! \class VInspector_Window
//! Window that unites all VInspector controls.
class VInspector_Window : public QObject
{
  Q_OBJECT

public:

  //! Constructor
  Standard_EXPORT VInspector_Window();

  //! Destructor
  virtual ~VInspector_Window() {}

  //! Provides the container with a parent where this container should be inserted.
  //! If Qt implementation, it should be QWidget with QLayout set inside
  //! \param theParent a parent class
  Standard_EXPORT void SetParent (void* theParent);

  //! Sets parameters container, it should be used when the plugin is initialized or in update content
  //! \param theParameters a parameters container
  void SetParameters (const Handle(TInspectorAPI_PluginParameters)& theParameters) { myParameters = theParameters; }

  //! Provide container for actions available in inspector on general level
  //! \param theMenu if Qt implementation, it is QMenu object
  Standard_EXPORT void FillActionsMenu (void* theMenu);

  //! Returns plugin preferences: dock widgets state, tree view columns.
  //! \param theItem container of preference elements
  Standard_EXPORT void GetPreferences (TInspectorAPI_PreferencesDataMap& theItem);

  //! Applies plugin preferences
  //! \param theItem container of preference elements
  Standard_EXPORT void SetPreferences (const TInspectorAPI_PreferencesDataMap& theItem);

  //! Applies parameters to Init controls, opens files if there are in parameters, updates OCAF tree view model
  Standard_EXPORT void UpdateContent();

  //! Returns main control
  QWidget* GetMainWindow() const { return myMainWindow; }

  //! Returns presentations of selected items in tree model
  //! \param theModel selection model
  //! \return container of presentations
  NCollection_List<Handle(AIS_InteractiveObject)> SelectedPresentations (QItemSelectionModel* theModel);

  //! Returns selected shapes
  //! \param [out] container of shapes
  void SelectedShapes (NCollection_List<Handle(Standard_Transient)>& theSelPresentations);

private:

  //! Fills controls of the plugin by parameters:
  //! - Fine AIS_InteractiveObject and fills View if it if it differs from the current context
  //! \param theParameters a parameters container
  bool Init (const NCollection_List<Handle(Standard_Transient)>& theParameters);

  //! Read BREP file, creates AIS presentation for the shape and visualize it in the current context
  //! \param theFileName a name of BREP file
  bool OpenFile (const TCollection_AsciiString& theFileName);

private slots:

  //! Shows context menu for tree view selected item. It contains clear view or BREP operations items
  //! \param thePosition a clicked point
  void onTreeViewContextMenuRequested(const QPoint& thePosition);

  //! Performs the functionality of the clicked action
  //! \param theActionId an action identifier in tool bar
  void onToolBarActionClicked (const int theActionId);

  //! Display content of selected tree view item if isToggled is true
  //! \param isToggled true if the property dock widget is shown
  void onPropertyPanelShown (bool isToggled);

  //! Processes selection in tree view: make presentation or owner selected in the context if corresponding
  //! check box is checked
  //! \param theSelected a selected items
  //! \param theDeselected a deselected items
  void onTreeViewSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected);

  //! Exports the first selected shape into ShapeViewer plugin.
  void onExportToShapeView();

  //! Appends lights into the active V3d view
  void onAddLight();

  //! Removes selected light from the active V3d view
  void onRemoveLight();

  //! Switch On/Off for selected light
  void onOnOffLight();

  //! Apply activated display action
  void onDisplayActionTypeClicked();

  //! Expand two next levels for all selected item
  void onExpand();

  //! Expand all levels for all selected items
  void onExpandAll();

  //! Collapse all levels for all selected items
  void onCollapseAll();

private:

  //! Inits the window content by the given context
  //! \param theContext a context
  void SetContext (const Handle(AIS_InteractiveContext)& theContext);

  //! Updates tree model
  void UpdateTreeModel();

  //! Set selected in tree view presentations displayed or erased in the current context. Note that erased presentations
  //! still belongs to the current context until Remove is called.
  //! \param theType display action type
  void displaySelectedPresentations (const View_DisplayActionType theType);

  //! Set items of the pointers highlighted in tree view
  //! \param theType display action type
  void highlightTreeViewItems (const QStringList& thePointers);

  //! Set items of the pointers selected in tree view
  //! \param theType display action type
  void selectTreeViewItems (const QStringList& thePointers);

  //! Returns displayer where the presentations/preview should be shown/erased
  //! If default view is created, it returns displayer of this view
  Standard_EXPORT View_Displayer* displayer();

  //! Creates an instance of 3D view to initialize context.
  //! \return a context of created view.
  Handle(AIS_InteractiveContext) createView();

  //! Creates a new default light into V3d viewer
  //! \param theSourceLight type of light source
  //! \param theViewer viewer to add the created light
  void addLight (const Graphic3d_TypeOfLightSource& theSourceLight, const Handle(V3d_Viewer)& theViewer);

private:

  QWidget* myParent; //!< widget, comes when Init window, the window control lays in the layout, updates window title

  QMainWindow* myMainWindow; //!< main control
  VInspector_ToolBar* myToolBar; //!< tool bar actions

  QDockWidget* myPropertyPanelWidget; //!< property pane dockable widget
  ViewControl_PropertyView* myPropertyView; //!< property control to display model item values if exist

  QTreeView* myTreeView; //!< tree view of AIS content

  ViewControl_MessageDialog* myExportToShapeViewDialog; //!< dialog about exporting TopoDS_Shape to ShapeView plugin
  View_Window* myViewWindow; //!< temporary view window, it is created if Open is called but context is still NULL

  Handle(TInspectorAPI_PluginParameters) myParameters; //!< plugins parameters container

  View_Displayer* myDisplayer; //!< class to display presentations/preview if myMainWindow is not used
};

#endif
