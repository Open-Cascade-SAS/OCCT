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

#ifndef ShapeView_Communicator_H
#define ShapeView_Communicator_H

#include <inspector/ShapeView_Window.hxx>
#include <inspector/TInspectorAPI_Communicator.hxx>

//! \class ShapeView_Communicator.
//! \brief This is a connector from TInspector application to ShapeView window
class ShapeView_Communicator : public TInspectorAPI_Communicator
{
public:

  //! Constructor
  Standard_EXPORT ShapeView_Communicator();

  //! Destructor
  Standard_EXPORT virtual ~ShapeView_Communicator() Standard_OVERRIDE;

  //! Provides the container with a parent where this container should be inserted.
  //! If Qt implementation, it should be QWidget with QLayout set inside
  //! \param theParent a parent class
  virtual void SetParent (void* theParent) Standard_OVERRIDE { myWindow->SetParent (theParent); }

  //! Sets parameters container, it should be used when the plugin is initialized or in update content
  //! \param theParameters a parameters container
  virtual void SetParameters (const Handle(TInspectorAPI_PluginParameters)& theParameters) Standard_OVERRIDE
  { myWindow->SetParameters (theParameters); }

  //! Calls update of the plugin's content
  virtual void UpdateContent() Standard_OVERRIDE { myWindow->UpdateContent(); }

private:

  ShapeView_Window* myWindow; //!< current window
};

#endif
