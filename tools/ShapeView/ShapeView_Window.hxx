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

#ifndef ShapeView_Window_H
#define ShapeView_Window_H

#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <inspector/TInspectorAPI_PluginParameters.hxx>
#include <TopoDS_Shape.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QItemSelection>
#include <QList>
#include <QModelIndexList>
#include <QObject>
#include <QPoint>
#include <QString>
#include <QTreeView>
#include <Standard_WarningsRestore.hxx>

class View_Window;

class ViewControl_PropertyView;

class QAction;
class QDockWidget;
class QMainWindow;
class QWidget;

//! \class ShapeView_Window
//! Window that unites all ShapeView controls.
class ShapeView_Window : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT ShapeView_Window (QWidget* theParent);

  //! Destructor
  Standard_EXPORT virtual ~ShapeView_Window();

  //! Provides the container with a parent where this container should be inserted.
  //! If Qt implementation, it should be QWidget with QLayout set inside
  //! \param theParent a parent class
  Standard_EXPORT void SetParent (void* theParent);

  //! Sets parameters container, it should be used when the plugin is initialized or in update content
  //! \param theParameters a parameters container
  void SetParameters (const Handle(TInspectorAPI_PluginParameters)& theParameters) { myParameters = theParameters; }

  //! Provides container for actions available in inspector on general level
  //! \param theMenu if Qt implementation, it is QMenu object
  Standard_EXPORT virtual void FillActionsMenu (void* theMenu);

  //! Returns plugin preferences: dock widgets state, tree view columns.
  //! \param theItem container of preference elements
  Standard_EXPORT void GetPreferences (TInspectorAPI_PreferencesDataMap& theItem);

  //! Applies plugin preferences
  //! \param theItem container of preference elements
  Standard_EXPORT void SetPreferences (const TInspectorAPI_PreferencesDataMap& theItem);

  //! Applyes parameters to Init controls, opens files if there are in parameters, updates OCAF tree view model
  Standard_EXPORT void UpdateContent();

  //! Returns main control
  QMainWindow* GetMainWindow() const { return myMainWindow; }

  //! Returns current tree view
  QTreeView* GetTreeView() const { return myTreeView; }

  //! Removes all shapes in tree view model, remove all stored BREP files
  Standard_EXPORT void RemoveAllShapes();

protected:
  //! Appends shape into tree view model
  //! \param theShape a shape instance
  Standard_EXPORT void addShape (const TopoDS_Shape& theShape);

private:

  //! Fills controls of the plugin by parameters:
  //! - Fine AIS_InteractiveObject and fills View if it if it differs from the current context
  //! \param theParameters a parameters container
  void Init (NCollection_List<Handle(Standard_Transient)>& theParameters);

  //! Reads Shape from the file name, add Shape into tree view
  //! \param theFileName BREP file name
  void OpenFile (const TCollection_AsciiString& theFileName);

protected slots:

  //! Shows context menu for tree view selected item. It contains expand/collapse actions.
  //! \param thePosition a clicked point
  void onTreeViewContextMenuRequested (const QPoint& thePosition);

  //! Processes selection in tree view: make presentation or owner selected in the context if corresponding
  //! check box is checked
  //! \param theSelected a selected items
  //! \param theDeselected a deselected items
  void onTreeViewSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected);

  //! Updates visibility states by erase all in context
  void onEraseAllPerformed();

  //! Sets the shape item exploded
  void onExplode();

  //! Removes all shapes in tree view
  void onClearView() { RemoveAllShapes(); }

  //! Loads BREP file and updates tree model to have shape of the file
  void onLoadFile();

  //! Views BREP files of selected items if exist
  void onExportToBREP();

  //! Convers file name to Ascii String and perform opeging file
  //! \param theFileName a file name to be opened
  void onOpenFile(const QString& theFileName) { OpenFile (TCollection_AsciiString (theFileName.toUtf8().data())); }

protected:
  //! Creates new action and connect it to the given slot
  //! \param theText an action text
  //! \param theSlot a listener method
  QAction* createAction (const QString& theText, const char* theSlot);

private:

  QMainWindow* myMainWindow; //!< main control, parent for all ShapeView controls

  QDockWidget* myPropertyPanelWidget; //!< property pane dockable widget
  ViewControl_PropertyView* myPropertyView; //!< property control to display model item values if exist

  View_Window* myViewWindow; //!< OCC 3d view to visualize presentations
  QTreeView* myTreeView; //!< tree view visualized shapes

  Handle(TInspectorAPI_PluginParameters) myParameters; //!< plugins parameters container
};

#endif
