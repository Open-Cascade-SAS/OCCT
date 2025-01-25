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

#ifndef TInspector_Shortcut_H
#define TInspector_Shortcut_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <Standard_WarningsRestore.hxx>

class TInspector_Window;
class QEvent;

//! \class TInspector_Shortcut
//! Listens application KeyRelease event. Processes key event:
//! - <Key_F5>: updates content (tree view model) of the active plugin
class TInspector_Shortcut : public QObject
{
public:
  //! Constructor
  Standard_EXPORT TInspector_Shortcut(QObject* theParent, TInspector_Window* theWindow);

  //! Destructor
  virtual ~TInspector_Shortcut() {}

  //! Processes key release event to update view model, otherwise do usual QObject functionality
  Standard_EXPORT virtual bool eventFilter(QObject* theObject, QEvent* theEvent) Standard_OVERRIDE;

private:
  TInspector_Window* myWindow; //!< the current window
};

#endif
