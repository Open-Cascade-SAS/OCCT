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

#ifndef TInspector_Preferences_H
#define TInspector_Preferences_H

#include <TCollection_AsciiString.hxx>
#include <inspector/TInspectorAPI_PreferencesDataMap.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QDomElement>
#include <Standard_WarningsRestore.hxx>

//! The class to read/write custom plugin preferences in XML format.
//! The preferences file is intended to know about:
//! - dock window placement
//! - tree view columns: visibility, width
class TInspector_Preferences
{
public:
  //! Constructs the communicator.
  TInspector_Preferences() {}

  //! Destructor
  virtual ~TInspector_Preferences() {}

  static Standard_CString PreferencesFileName() { return ".tinspector.xml"; }

  //! Sets path to a preferences file
  //! \param thePath a path
  void SetDirectory (const TCollection_AsciiString& thePath) { reset(); myDirectory = thePath; }

  //! Returns path to a preferences file
  //! \return path
  TCollection_AsciiString GetDirectory() const { return myDirectory; }

  //! Returns plugin preferences
  //! \param thePluginName a plugin name
  Standard_EXPORT void GetPreferences (const TCollection_AsciiString& thePluginName,
                                       TInspectorAPI_PreferencesDataMap& theItem);

  //! Stores plugin preferences
  //! \param thePluginName a plugin name
  //! \theItem container of plugin preferences values in form: <name, value>
  void SetPreferences (const TCollection_AsciiString& thePluginName, const TInspectorAPI_PreferencesDataMap& theItem)
  { myLoadedPreferences.Bind(thePluginName, theItem); }

  //! Stores plugin preferences into a preferences file
  Standard_EXPORT void StorePreferences();

  //! Removes plugin preferences file
  Standard_EXPORT void RemovePreferences();

private:
  //! Loads the directory preference file with filling internal container
  void loadPreferences();

  //! Clears all internal containers with information of already loaded file
  void reset() { myLoadedPreferences.Clear(); myIsLoadedPreferences = Standard_False; }

  //! Reads plugin preferences and fill container
  void readPluginItem(const QDomElement thePluginElement, TInspectorAPI_PreferencesDataMap& theItem);

  //! Returns text of attribute document
  static Standard_CString documentKey() { return "document"; }

  //! Returns text of attribute plugin
  static Standard_CString pluginKey() { return "plugin"; }

  //! Returns text of attribute parameter
  static Standard_CString parameterKey() { return "parameter"; }

  //! Returns text of attribute name
  static Standard_CString nameKey() { return "name"; }

  //! Returns text of attribute value
  static Standard_CString valueKey() { return "value"; }

private:
  //! directory of preferences file
  TCollection_AsciiString myDirectory;
  //! container of already loaded preferences : cache
  NCollection_DataMap<TCollection_AsciiString, TInspectorAPI_PreferencesDataMap> myLoadedPreferences;
  //! state whether the preferences of the current directory is loaded
  Standard_Boolean myIsLoadedPreferences;
};

#endif
