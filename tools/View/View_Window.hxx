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

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QWidget>

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
  Standard_EXPORT View_Window (QWidget* theParent);

  //! Destructor
  virtual ~View_Window() {}

  //! Returns view displayer
  View_Displayer* GetDisplayer() const { return myDisplayer; }

  //! Returns view widget
  View_Widget* GetView() const { return myView; }

  //! Returns actions tool bar
  QToolBar* GetActionsToolBar() const { return myActionsToolBar; }

  //! Returns window tool bar
  View_ToolBar* GetViewToolBar() const { return myViewToolBar; }

  //! Sets a new context for context type
  //! \param theType a type of context, will be selected in the tool bar combo box
  //! \param theContext an AIS context
  Standard_EXPORT void SetContext (View_ContextType theType, const Handle(AIS_InteractiveContext)& theContext);

protected slots:

  //! Processing context change:
  //! - set an active context in the displayer,
  //! - erase all displayed presentations from the previous context,
  //! - sets the current view enabled only if a current context type is View_ContextType_Own
  void onViewSelectorActivated();

  //! Processing window tool bar actions
  void onToolBarActionClicked (const int theActionId);

  //! Sets selected display mode in the current context
  void onDisplayModeChanged();

private:

  View_Displayer* myDisplayer; //!< displayer
  View_Widget* myView; //!< view widget
  QToolBar* myActionsToolBar; //!< actions tool bar
  View_ToolBar* myViewToolBar; //!< window tool bar
};

#endif
