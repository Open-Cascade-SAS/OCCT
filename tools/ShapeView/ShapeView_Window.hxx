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

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QItemSelection>
#include <QList>
#include <QModelIndexList>
#include <QObject>
#include <QPoint>
#include <QString>
#include <QTreeView>

class View_Displayer;
class View_Window;

class QAction;
class QMainWindow;
class QWidget;

//! \class ShapeView_Window
//! Window that unites all ShapeView controls.
class ShapeView_Window : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT ShapeView_Window (QWidget* theParent, const TCollection_AsciiString& theTemporaryDirectory);

  //! Destructor
  Standard_EXPORT virtual ~ShapeView_Window();

  //! Provides the container with a parent where this container should be inserted.
  //! If Qt implementation, it should be QWidget with QLayout set inside
  //! \param theParent a parent class
  Standard_EXPORT void SetParent (void* theParent);

  //! Sets parameters container, it should be used when the plugin is initialized or in update content
  //! \param theParameters a parameters container
  void SetParameters (const Handle(TInspectorAPI_PluginParameters)& theParameters) { myParameters = theParameters; }

  //! Applyes parameters to Init controls, opens files if there are in parameters, updates OCAF tree view model
  Standard_EXPORT void UpdateContent();

  //! Returns main control
  QMainWindow* GetMainWindow() const { return myMainWindow; }

  //! Returns current tree view
  QTreeView* GetTreeView() const { return myTreeView; }

  //! Returns path to temporary directory
  TCollection_AsciiString GetTemporaryDirectory() const { return myTemporaryDirectory; }

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

  //! Read Shape from the file name, add Shape into tree view
  //! \param theFileName BREP file name
  void OpenFile (const TCollection_AsciiString& theFileName);

protected slots:

  //! Displays shapes obtained by selected indices
  //! \param theSelected a container of selected indices in tree view
  //! \param theDeselected a container of deselected indices in tree view
  void onTreeViewSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected)
  { (void)theDeselected; displaySelectedShapes(theSelected.indexes()); }

  //! Shows context menu for tree view selected item. It contains expand/collapse actions.
  //! \param thePosition a clicked point
  void onTreeViewContextMenuRequested (const QPoint& thePosition);

  //! Exports shape to BREP file and view result file
  void onBREPDirectory();

  //! Removes all shapes in tree view
  void onClearView() { RemoveAllShapes(); }

  //! Load BREP file and updates tree model to have shape of the file
  void onLoadFile();

  //! View BREP files of selected items if exist
  void onBREPView();

  //! Remove BREP views, close views
  void onCloseAllBREPViews();

  //! Remove all BREP Viewse excepting active
  void onEditorDestroyed (QObject* theObject);

  //! Convers file name to Ascii String and perform opeging file
  //! \param theFileName a file name to be opened
  void onOpenFile(const QString& theFileName) { OpenFile (TCollection_AsciiString (theFileName.toUtf8().data())); }

protected:

  //! Views file name content in a text editor. It creates new Qt free control with content.
  //! \param theFileName a file name
  void viewFile (const QString& theFileName);

  //! Removes all BREP files in tmp directory
  void removeBREPFiles();

  //! Creates new action and connect it to the given slot
  //! \param theText an action text
  //! \param theSlot a listener method
  QAction* createAction (const QString& theText, const char* theSlot);

  //! Key that uses to generate BREP file name
  //! \return string value
  static TCollection_AsciiString viewBREPPrefix() { return "ShapeView_Window"; }

  //! Returns newxt temporary name using BREPPrefix and pointer information
  //! \param thePointerInfo a pointer value
  //! \return string value
  TCollection_AsciiString getNextTmpName (const TCollection_AsciiString& thePointerInfo);

  //! Finds shapes for selected items in tree view and display presentations for the shapes
  //! \param theSelected a list of selected indices in tree view
  void displaySelectedShapes (const QModelIndexList& theSelected);

private:

  QMainWindow* myMainWindow; //!< main control, parent for all ShapeView controls
  View_Window* myViewWindow; //!< OCC 3d view to visualize presentations
  QTreeView* myTreeView; //!< tree view visualized shapes

  TCollection_AsciiString myTemporaryDirectory; //!< path to the temporary directory
  int myNextPosition; //!< delta of moving control of view BREP file

  QList<QWidget*> myBREPViews; //!< list of view BREP file controls
  Handle(TInspectorAPI_PluginParameters) myParameters; //!< plugins parameters container
};

#endif
