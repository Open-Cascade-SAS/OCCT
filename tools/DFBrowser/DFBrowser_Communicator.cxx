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


#include <inspector/DFBrowser_Communicator.hxx>

#include <CDF_Session.hxx>
#include <inspector/DFBrowser_Module.hxx>
#include <inspector/DFBrowser_Window.hxx>

// =======================================================================
// function : CreateCommunicator
// purpose : Creates a communicator by the library loading
// =======================================================================
Standard_EXPORTEXTERNC TInspectorAPI_Communicator* CreateCommunicator()
{
  return new DFBrowser_Communicator();
}

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_Communicator::DFBrowser_Communicator()
 : TInspectorAPI_Communicator(), myWindow (0)
{
  myWindow = new DFBrowser_Window();
}

// =======================================================================
// function : SetParameters
// purpose :
// =======================================================================
void DFBrowser_Communicator::SetParameters (const Handle(TInspectorAPI_PluginParameters)& theParameters)
{
  myWindow->SetParameters (theParameters);
  myWindow->UpdateContent();
}
