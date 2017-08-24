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

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <inspector/TInspectorAPI_PluginParameters.hxx>
#include <inspector/TInspector_Window.hxx>

//! \class TInspector_PluginParameters.
//! \brief This is plugin parameters extended by a possibility to activate module during setting new parameters
class TInspector_PluginParameters : public TInspectorAPI_PluginParameters
{
public:

  //! Constructor
  Standard_EXPORT TInspector_PluginParameters (TInspector_Window* theWindow) : myWindow (theWindow) {}

  //! Destructor
  virtual ~TInspector_PluginParameters() Standard_OVERRIDE {}

  //! Stores the parameters for plugin
  //! \param thePluginName a plugin name
  //! \param theParameters a list of parameters
  //! \param theToActivatePlugin a state whether the plugin should be immediately activated
  Standard_EXPORT virtual void SetParameters (const TCollection_AsciiString& thePluginName,
                                      const NCollection_List<Handle(Standard_Transient)>& theParameters,
                                      const Standard_Boolean& theToActivatePlugin = Standard_False) Standard_OVERRIDE;

private:

  TInspector_Window* myWindow; //!< the current window
};

#endif
