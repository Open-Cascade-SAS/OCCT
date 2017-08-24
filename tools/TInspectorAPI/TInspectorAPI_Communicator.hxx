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

#ifndef TInspectorAPI_Communicator_H
#define TInspectorAPI_Communicator_H

#include <NCollection_List.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Version.hxx>
#if OCC_VERSION_HEX > 0x060901
  #include <Standard_Handle.hxx>
#endif
#include <inspector/TInspectorAPI_PluginParameters.hxx>

//! The Communicator is an interface that should be implemented for a separate plugin
//! It will be placed in layout of the given parent. After the plugin is created, it is possible to
//! set container of parameters into plugin to provide the plugin's initialization by some external
//! objects(e.g. Interactive Context or OCAF Application). If the parameters are changed, it may be
//! applyed in UpdateContent function. The communicator can change parameters in the following cases:
//! - the plugin removes own processed parameters (e.g. file names, that was opened by the plugin)
//! - the plugin sends some parameters to another plugin(by name) (e.g. shape to be analized)
//!   (at the same time we should be careful here to do not change essential parameters of other plugins)
class TInspectorAPI_Communicator
{
public:

  //! Loads the plugin library
  //! \param thePluginName the name of the library
  //! \return an instance of the communicator or NULL
  static Standard_EXPORT TInspectorAPI_Communicator* LoadPluginLibrary (const TCollection_AsciiString& thePluginName);

  //! Sets parameters container, it should be used when the plugin is initialized or in update content
  //! \param theParameters a parameters container
  Standard_EXPORT virtual void SetParameters (const Handle(TInspectorAPI_PluginParameters)& theParameters) = 0;

  //! Provides the container with a parent where this container should be inserted.
  //! If Qt implementation, it should be QWidget with QLayout set inside
  //! \param theParent a parent class
  Standard_EXPORT virtual void SetParent (void* theParent) = 0;

  //! Calls update of the plugin's content
  Standard_EXPORT virtual void UpdateContent() = 0;

  //! Constructs the communicator.
  TInspectorAPI_Communicator() {}

  //! Destructor
  virtual ~TInspectorAPI_Communicator() {}
};

//! Declare plugin method
extern "C"
{
  //! Declares function to create an instance of communicator
  //! It should be implemented in a child plugin
  typedef TInspectorAPI_Communicator* (*COMMUNICATOR_INSTANCE)();
}
//! Defines name of the function that should be implemented in a child plugin
#define CREATE_COMMUNICATOR_FUNCTION_NAME "CreateCommunicator"

#endif
