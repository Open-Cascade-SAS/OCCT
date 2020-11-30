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

#ifndef View_Window_H
#define View_Window_H

#include <AIS_InteractiveContext.hxx>
#include <inspector/View_ContextType.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

class View_Displayer;
class View_ToolBar;
class View_Widget;

class QToolBar;

//! \class View_Window
//! \brief It is a widget where in grid layout View widget and tool bars are placed. There are two tool bars.
//! The first, view actions, tool bar is placed on Vertical, the window tool bar is placed Horizontally.
//! The second tool bar contains actions of View_ToolBar.
class View_Window : public QWidget
{
  Q_OBJECT

public:

  //! Constructor
  Standard_EXPORT View_Window (QWidget* theParent,
                               const Handle(AIS_InteractiveContext)& theContext = Handle(AIS_InteractiveContext)(),
                               const bool isUseKeepView = true, const bool isFitAllActive = true);

  //! Destructor
  virtual ~View_Window() {}

  //! Returns view displayer
  View_Displayer* Displayer() const { return myDisplayer; }

  //! Returns view widget
  View_Widget* ViewWidget() const { return myView; }

  //! Returns actions tool bar
  QToolBar* ActionsToolBar() const { return myActionsToolBar; }

  //! Returns window tool bar
  View_ToolBar* ViewToolBar() const { return myViewToolBar; }

  //! Sets a new context for context type
  //! \param theType a type of context, will be selected in the tool bar combo box
  //! \param theContext an AIS context
  Standard_EXPORT void SetContext (View_ContextType theType, const Handle(AIS_InteractiveContext)& theContext);

  //! Sets default size that is used in sizeHint when the widget is firstly show
  Standard_EXPORT void SetPredefinedSize (int theDefaultWidth, int theDefaultHeight);

  //! Sets initial camera position
  //! \param theVx direction on Ox
  //! \param theVy direction on Oy
  //! \param theVz direction on Oz
  Standard_EXPORT void SetInitProj (const Standard_Real theVx, const Standard_Real theVy, const Standard_Real theVz);

  //! Returns an active view
  Standard_EXPORT Handle(V3d_View) View() const;

  //! Saves state of view window in a container in form: key, value. It saves:
  //! - visibility of columns,
  //! - columns width
  //! \param theTreeView a view instance
  //! \param theItems [out] properties
  //! \param thePrefix preference item prefix
  Standard_EXPORT static void SaveState (View_Window* theView, QMap<QString, QString>& theItems,
                                         const QString& thePrefix = QString());

  //! Restores state of view window by a container
  //! \param theTreeView a view instance
  //! \param theKey property key
  //! \param theValue property value
  //! \param thePrefix preference item prefix
  //! \return boolean value whether the property is applied to the tree view
  Standard_EXPORT static bool RestoreState (View_Window* theView, const QString& theKey, const QString& theValue,
                                            const QString& thePrefix = QString());

signals:
  //! Signals about calling erasing all presentations in context
  void eraseAllPerformed();

protected slots:

  //! Processing context change:
  //! - set an active context in the displayer,
  //! - erase all displayed presentations from the previous context,
  //! - sets the current view enabled only if a current context type is View_ContextType_Own
  void onViewSelectorActivated();

  //! Processing widget action checked state changed: for Fit All action, if checked, displayer do FitAll automatically
  //! \param theActionId a clicked action
  //! \param theState a result checked state
  void onCheckedStateChanged (int theActionId, bool theState);

  //! Processing window tool bar actions
  void onToolBarActionClicked (const int theActionId);

  //! Shows context menu for view. It contains set view orientation actions.
  //! \param thePosition a clicked point
  void onViewContextMenuRequested (const QPoint& thePosition);

  //! Sets the view scene orientation by the text of selected action
  void onSetOrientation();

  //! Sets selected display mode in the current context
  void onDisplayModeChanged();

private:

  View_Displayer* myDisplayer; //!< displayer
  View_Widget* myView; //!< view widget
  QToolBar* myActionsToolBar; //!< actions tool bar
  View_ToolBar* myViewToolBar; //!< window tool bar
};

#endif
