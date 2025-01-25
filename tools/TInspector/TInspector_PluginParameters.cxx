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

#include <inspector/TInspector_PluginParameters.hxx>
#include <inspector/TInspector_Preferences.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TInspector_PluginParameters::TInspector_PluginParameters(TInspector_Window* theWindow)
    : myWindow(theWindow),
      myPreferences(new TInspector_Preferences())
{
  myPreferences->SetDirectory(GetTemporaryDirectory());
}

// =======================================================================
// function : SetParameters
// purpose :
// =======================================================================
void TInspector_PluginParameters::SetParameters(
  const TCollection_AsciiString&                      thePluginName,
  const NCollection_List<Handle(Standard_Transient)>& theParameters,
  const Standard_Boolean&                             theToActivatePlugin)
{
  TInspectorAPI_PluginParameters::SetParameters(thePluginName, theParameters, Standard_False);

  if (!theToActivatePlugin)
    return;

  SetSelected(thePluginName, theParameters);
  myWindow->ActivateTool(thePluginName);
}

// =======================================================================
// function : SetTemporaryDirectory
// purpose :
// =======================================================================
void TInspector_PluginParameters::SetTemporaryDirectory(const TCollection_AsciiString& thePath)
{
  if (thePath.IsEqual(myPreferences->GetDirectory()))
    return;

  myPreferences->SetDirectory(thePath);
}
