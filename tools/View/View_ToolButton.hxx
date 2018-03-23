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

#ifndef View_ToolButton_H
#define View_ToolButton_H

#include <Standard_WarningsDisable.hxx>
#include <QToolButton>
#include <QMouseEvent>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

//! \class View_Widget
//! \brief It is a Qt control that implements change checked state for button by double click event
//! Button becomes checked by double click mouse pressed and unchecked by the next press mouse
class View_ToolButton : public QToolButton
{
  Q_OBJECT

public:
  View_ToolButton (QWidget* theParent) : QToolButton (theParent) {}
  ~View_ToolButton() {}

  //! Sets the button checkable, set whether the button checkable or not
  //! \param theChecked boolean value
  void SetButtonChecked (const bool theChecked) {setCheckable (theChecked); setChecked (theChecked); emit checkedStateChanged (theChecked); }

signals:
  //! Sends a signal about checked state is changed
  //! \param theState the checked state
  void checkedStateChanged (bool theState);

protected:
  //! Sets the button unchecked if it was checked
  virtual void mousePressEvent (QMouseEvent* theEvent)
  {
    if (isChecked())
      SetButtonChecked (false);
    else
      QToolButton::mousePressEvent (theEvent);
  }

  //! Sets the button checked if it was unchecked
  virtual void mouseDoubleClickEvent (QMouseEvent* theEvent)
  {
    QToolButton::mouseDoubleClickEvent (theEvent);
    if (!isChecked())
      SetButtonChecked (true);
  }
};

#endif
