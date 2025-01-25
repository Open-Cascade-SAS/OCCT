// Created on: 2021-04-27
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef MessageView_Window_H
#define MessageView_Window_H

#include <Message_Report.hxx>
#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>

#include <inspector/MessageModel_Actions.hxx>
#include <inspector/TInspectorAPI_PluginParameters.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <TopoDS_Shape.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QItemSelection>
#include <QList>
#include <QModelIndexList>
#include <QObject>
#include <QPoint>
#include <QString>
#include <QTableView>
#include <QTreeView>
#include <Standard_WarningsRestore.hxx>

class View_Displayer;
class View_Window;

class ViewControl_PropertyView;

class MessageView_ActionsTest;

class QDockWidget;
class QMainWindow;
class QMenu;
class QWidget;

//! \class MessageView_Window
//! Window that unites all MessageView controls.
class MessageView_Window : public QObject
{
  Q_OBJECT
public:
  //! Constructor
  Standard_EXPORT MessageView_Window(QWidget* theParent);

  //! Destructor
  virtual ~MessageView_Window() {}

  //! Provides the container with a parent where this container should be inserted.
  //! If Qt implementation, it should be QWidget with QLayout set inside
  //! \param theParent a parent class
  Standard_EXPORT void SetParent(void* theParent);

  //! Sets parameters container, it should be used when the plugin is initialized or in update
  //! content \param theParameters a parameters container
  void SetParameters(const Handle(TInspectorAPI_PluginParameters)& theParameters)
  {
    myParameters = theParameters;
    myTreeViewActions->SetParameters(theParameters);
  }

  //! Provide container for actions available in inspector on general level
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

  //! Returns main control
  QMainWindow* GetMainWindow() const { return myMainWindow; }

  //! Returns current tree view
  QTreeView* GetTreeView() const { return myTreeView; }

protected:
  //! Appends shape into tree view model
  //! \param theShape a shape instance
  //! \param theReportDescription an additional report information
  void addReport(const Handle(Message_Report)&  theReport,
                 const TCollection_AsciiString& theReportDescription = "");

private:
  //! Fills controls of the plugin by parameters:
  //! - Fine AIS_InteractiveObject and fills View if it if it differs from the current context
  //! \param theParameters a parameters container
  void Init(NCollection_List<Handle(Standard_Transient)>& theParameters);

  //! Updates tree model
  void updateTreeModel();

protected slots:
  //! Updates property view selection in table if the item is hidden
  //! \param theIndex tree view model index
  void onTreeViewVisibilityClicked(const QModelIndex& theIndex);

  //! Updates all controls by changed selection in OCAF tree view
  //! \param theSelected list of selected tree view items
  //! \param theDeselected list of deselected tree view items
  void onTreeViewSelectionChanged(const QItemSelection& theSelected,
                                  const QItemSelection& theDeselected);

  //! Shows context menu for tree view selected item. It contains expand/collapse actions.
  //! \param thePosition a clicked point
  void onTreeViewContextMenuRequested(const QPoint& thePosition);

  //! Display content of selected tree view item if isToggled is true
  //! \param isToggled true if the property dock widget is shown
  void onPropertyPanelShown(bool isToggled);

  //! Update tree view item, preview presentation by item value change
  void onPropertyViewDataChanged();

  //! Update tree view header item width
  void onHeaderResized(int theSectionId, int, int);

  //! Updates visibility states by erase all in context
  void onEraseAllPerformed();

  //! Export report into document
  void onExportReport();

  //! Create default report into document
  void onCreateDefaultReport();

  //! Iterates by children items of selected items and display its presentations if found
  void onPreviewChildren();

  //! Creates the table that sums the number of calls and
  //! the time spent on the functionality inside the value.
  void onMetricStatistic();

  //! Switch active state in report for clicked type of metric
  void OnActivateMetric();

  //! Deactivate all types of metrics for the current report
  void OnDeactivateAllMetrics();

protected:
  //! Appends items to activate report metrics
  void addActivateMetricActions(QMenu* theMenu);

  //! Returns displayer where the presentations/preview should be shown/erased
  //! If default view is created, it returns displayer of this view
  Standard_EXPORT View_Displayer* displayer();

  //! Updates property panel content by item selected in tree view.
  void updatePropertyPanelBySelection();

  //!< Updates presentation of preview for parameter shapes. Creates a compound of the shapes
  void updatePreviewPresentation();

  //!< Sets reports metric columns visible if used
  void updateVisibleColumns();

private:
  QMainWindow* myMainWindow;     //!< main control, parent for all MessageView controls
  QDockWidget* myViewDockWidget; //!< view dock widget to hide/show

  QDockWidget* myPropertyPanelWidget; //!< property pane dockable widget
  ViewControl_PropertyView*
    myPropertyView; //!< property control to display model item values if exist

  View_Window*             myViewWindow;      //!< OCC 3d view to visualize presentations
  QTreeView*               myTreeView;        //!< tree view visualized shapes
  MessageModel_Actions*    myTreeViewActions; //!< processing history view actions
  MessageView_ActionsTest* myTestViewActions; //!< check view actions

  QTableView*  myCustomView;        //!< table that units messages by name.
  QDockWidget* myCustomPanelWidget; //!< panel for table that units messages by name.

  Handle(TInspectorAPI_PluginParameters) myParameters; //!< plugins parameters container

  Handle(AIS_InteractiveObject)
    myPreviewPresentation; //!< presentation of preview for a selected object
};

#endif
