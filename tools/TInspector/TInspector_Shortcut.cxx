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

#include <inspector/TInspector_Shortcut.hxx>

#include <inspector/TInspector_Window.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QKeyEvent>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TInspector_Shortcut::TInspector_Shortcut(QObject* theParent, TInspector_Window* theWindow)
    : QObject(theParent),
      myWindow(theWindow)
{
  qApp->installEventFilter(this);
}

// =======================================================================
// function : eventFilter
// purpose :
// =======================================================================
bool TInspector_Shortcut::eventFilter(QObject* theObject, QEvent* theEvent)
{
  if (!myWindow || theEvent->type() != QEvent::KeyRelease)
    return QObject::eventFilter(theObject, theEvent);

  QKeyEvent* aKeyEvent = dynamic_cast<QKeyEvent*>(theEvent);
  switch (aKeyEvent->key())
  {
    case Qt::Key_F5: {
      myWindow->UpdateContent();
      return true;
    }
    default:
      break;
  }
  return QObject::eventFilter(theObject, theEvent);
}
