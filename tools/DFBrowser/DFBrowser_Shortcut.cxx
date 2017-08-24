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

#include <inspector/DFBrowser_Shortcut.hxx>

#include <inspector/DFBrowser_Module.hxx>

#include <QApplication>
#include <QKeyEvent>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_Shortcut::DFBrowser_Shortcut (QObject* theParent)
: QObject (theParent), myModule (0)
{
  qApp->installEventFilter (this);
}

// =======================================================================
// function : eventFilter
// purpose :
// =======================================================================
bool DFBrowser_Shortcut::eventFilter (QObject* theObject, QEvent* theEvent)
{
  if (!myModule || theEvent->type() != QEvent::KeyRelease)
    return QObject::eventFilter (theObject, theEvent);

  QKeyEvent* aKeyEvent = dynamic_cast<QKeyEvent*> (theEvent);
  switch (aKeyEvent->key())
  {
    case Qt::Key_F5:
    {
      myModule->UpdateTreeModel();
      return true;
    }
    default: break;
  }
  return QObject::eventFilter (theObject, theEvent);
}
