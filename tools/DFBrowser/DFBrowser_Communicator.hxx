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

#ifndef DFBrowser_Communicator_H
#define DFBrowser_Communicator_H

#include <inspector/DFBrowser_Window.hxx>
#include <inspector/TInspectorAPI_Communicator.hxx>

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <QObject>

class DFBrowser_Window;

//! \class DFBrowser_Communicator.
//! \brief This is a connector from TInspector application to DFBrowser view
class DFBrowser_Communicator : public QObject, public TInspectorAPI_Communicator
{
public:

  //! Constructor
  Standard_EXPORT DFBrowser_Communicator();

  //! Destructor
  virtual ~DFBrowser_Communicator() Standard_OVERRIDE {}

  //! Provides the container with a parent where this container should be inserted.
  //! If Qt implementation, it should be QWidget with QLayout set inside
  //! \param theParent a parent class
  virtual void SetParent (void* theParent) Standard_OVERRIDE { myWindow->SetParent (theParent); }

  //! Sets parameters container, it should be used when the plugin is initialized or in update content
  //! \param theParameters a parameters container
  Standard_EXPORT virtual void SetParameters (const Handle(TInspectorAPI_PluginParameters)& theParameters) Standard_OVERRIDE;

  //! Updates content of the current window
  virtual void UpdateContent() Standard_OVERRIDE { myWindow->UpdateContent(); }

private:

  DFBrowser_Window* myWindow; //!< the current window
};

#endif
