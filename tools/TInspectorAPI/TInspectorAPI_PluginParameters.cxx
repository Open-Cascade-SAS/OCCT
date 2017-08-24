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

#include <inspector/TInspectorAPI_PluginParameters.hxx>

#if OCC_VERSION_HEX <= 0x060901
IMPLEMENT_STANDARD_HANDLE (TInspectorAPI_PluginParameters, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT (TInspectorAPI_PluginParameters, Standard_Transient)
#else
IMPLEMENT_STANDARD_RTTIEXT (TInspectorAPI_PluginParameters, Standard_Transient)
#endif

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TInspectorAPI_PluginParameters::TInspectorAPI_PluginParameters()
{
}

// =======================================================================
// function : SetParameters
// purpose :
// =======================================================================
void TInspectorAPI_PluginParameters::SetParameters (const TCollection_AsciiString& thePluginName,
                                                    const NCollection_List<Handle(Standard_Transient)>& theParameters,
                                                    const Standard_Boolean&)
{
  if (theParameters.Size() > 0)
    myParameters.Bind (thePluginName, theParameters);
  else
    myParameters.UnBind (thePluginName);
}

// =======================================================================
// function : AddFileName
// purpose :
// =======================================================================
void TInspectorAPI_PluginParameters::AddFileName (const TCollection_AsciiString& thePluginName,
                                                  const TCollection_AsciiString& theFileName)
{
  if (myFileNames.IsBound(thePluginName))
    myFileNames.ChangeFind(thePluginName).Append (theFileName);
  else
  {
    NCollection_List<TCollection_AsciiString> aNames;
    aNames.Append (theFileName);
    myFileNames.Bind (thePluginName, aNames);
  }
}

// =======================================================================
// function : SetFileNames
// purpose :
// =======================================================================
void TInspectorAPI_PluginParameters::SetFileNames (const TCollection_AsciiString& thePluginName,
                                                   const NCollection_List<TCollection_AsciiString>& theFileNames)
{
  if (theFileNames.Size() > 0)
    myFileNames.Bind (thePluginName, theFileNames);
  else
    myFileNames.UnBind (thePluginName);
}

// =======================================================================
// function : SetSelectedNames
// purpose :
// =======================================================================
void TInspectorAPI_PluginParameters::SetSelectedNames (const TCollection_AsciiString& thePluginName,
                                                       const NCollection_List<TCollection_AsciiString>& theItemNames)
{
  mySelectedItemNames.Bind (thePluginName, theItemNames);
}

// =======================================================================
// function : SetSelected
// purpose :
// =======================================================================
void TInspectorAPI_PluginParameters::SetSelected (const TCollection_AsciiString& thePluginName,
                                                  const NCollection_List<Handle(Standard_Transient)>& theObjects)
{
  if (theObjects.Size() > 0)
    mySelectedObjects.Bind (thePluginName, theObjects);
  else
    mySelectedObjects.UnBind (thePluginName);
}

// =======================================================================
// function : FindParameters
// purpose :
// =======================================================================
bool TInspectorAPI_PluginParameters::FindParameters (const TCollection_AsciiString& thePluginName)
{
  return myParameters.IsBound(thePluginName);
}

// =======================================================================
// function : Parameters
// purpose :
// =======================================================================
const NCollection_List<Handle(Standard_Transient)>& TInspectorAPI_PluginParameters::Parameters
                                                     (const TCollection_AsciiString& thePluginName)
{
  return myParameters.Find (thePluginName);
}

// =======================================================================
// function : FindFileNames
// purpose :
// =======================================================================
bool TInspectorAPI_PluginParameters::FindFileNames (const TCollection_AsciiString& thePluginName)
{
  return myFileNames.IsBound(thePluginName);
}

// =======================================================================
// function : FileNames
// purpose :
// =======================================================================
const NCollection_List<TCollection_AsciiString>& TInspectorAPI_PluginParameters::FileNames
                                                       (const TCollection_AsciiString& thePluginName)
{
  return myFileNames.Find (thePluginName);
}

// =======================================================================
// function : FindSelectedNames
// purpose :
// =======================================================================
bool TInspectorAPI_PluginParameters::FindSelectedNames (const TCollection_AsciiString& thePluginName)
{
  return mySelectedItemNames.IsBound(thePluginName);
}

// =======================================================================
// function : GetSelectedNames
// purpose :
// =======================================================================
const NCollection_List<TCollection_AsciiString>& TInspectorAPI_PluginParameters::GetSelectedNames
                                                       (const TCollection_AsciiString& thePluginName)
{
  return mySelectedItemNames.Find (thePluginName);
}

// =======================================================================
// function : GetSelectedObjects
// purpose :
// =======================================================================
Standard_Boolean TInspectorAPI_PluginParameters::GetSelectedObjects (const TCollection_AsciiString& thePluginName,
                                                       NCollection_List<Handle(Standard_Transient)>& theObjects)
{
  return mySelectedObjects.Find (thePluginName, theObjects);
}
