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

#ifndef DFBrowser_Shortcut_H
#define DFBrowser_Shortcut_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QObject>

class DFBrowser_Module;
class QEvent;

//! \class DFBrowser_Shortcut
//! Listens application KeyRelease event. Processes key event:
//! - <Key_F5>: updates tree view model
class DFBrowser_Shortcut : public QObject
{
public:

  //! Constructor
  Standard_EXPORT DFBrowser_Shortcut (QObject* theParent);

  //! Destructor
  virtual ~DFBrowser_Shortcut() {}

  //! Sets the current module
  //! \param theModule a module
  void SetModule (DFBrowser_Module* theModule) { myModule = theModule; }

  //! Processes key release event to update view model, otherwise do usual QObject functionality
  Standard_EXPORT virtual bool eventFilter (QObject *theObject, QEvent* theEvent) Standard_OVERRIDE;

private:
  DFBrowser_Module* myModule; //!< the current module
};


#endif
