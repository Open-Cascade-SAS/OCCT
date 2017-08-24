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

#ifndef TInspectorAPI_PluginParameters_H
#define TInspectorAPI_PluginParameters_H

#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <Standard_Version.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_AsciiString.hxx>

class TInspectorAPI_PluginParameters;
DEFINE_STANDARD_HANDLE (TInspectorAPI_PluginParameters, Standard_Transient)

//! The container of parameters for all possible plugins. It stores list of parameters for each plugin, even
//! it was not be loaded yet. There is a map of plugin name into plugin parameters.
//! The parameters may be:
//! - child of Standard_Transient
//! - file name to be opened by the plugin
class TInspectorAPI_PluginParameters : public Standard_Transient
{
public:

  //! Constructs the container.
  Standard_EXPORT TInspectorAPI_PluginParameters();

  //! Destructor
  Standard_EXPORT virtual ~TInspectorAPI_PluginParameters() Standard_OVERRIDE {}

  //! Stores the parameters for plugin
  //! \param thePluginName a plugin name
  //! \param theParameters a list of parameters
  //! \param theToActivatePlugin a state whether the plugin should be immediately activated, to be used in a heir
  Standard_EXPORT virtual void SetParameters (const TCollection_AsciiString& thePluginName,
                                              const NCollection_List<Handle(Standard_Transient)>& theParameters,
                                              const Standard_Boolean& theToActivatePlugin = Standard_False);

  //! Adds a file name for the plugin
  //! \param thePluginName a plugin name
  //! \param theFileName a name
  Standard_EXPORT void AddFileName (const TCollection_AsciiString& thePluginName,
                                    const TCollection_AsciiString& theFileName);

  //! Sets file names for the plugin
  //! \param thePluginName a plugin name
  //! \param theFileNames container of names
  Standard_EXPORT void SetFileNames (const TCollection_AsciiString& thePluginName,
                                     const NCollection_List<TCollection_AsciiString>& theFileNames);

  //! Sets a name of item to be selected in the plugin
  //! \param thePluginName a plugin name
  //! \param theItemNames a container of names to be selected
  Standard_EXPORT void SetSelectedNames (const TCollection_AsciiString& thePluginName,
                                         const NCollection_List<TCollection_AsciiString>& theItemNames);

  //! Sets objects to be selected in the plugin
  //! \param thePluginName a plugin name
  //! \param theObjects an objects
  Standard_EXPORT void SetSelected (const TCollection_AsciiString& thePluginName,
                                    const NCollection_List<Handle(Standard_Transient)>& theObjects);

  //! Returns true if there are parameters set for the given plugin
  //! \param thePluginName a plugin name
  //! \return boolean result
  Standard_EXPORT bool FindParameters (const TCollection_AsciiString& thePluginName);

  //! Returns parameters set for the given plugin
  //! \param thePluginName a plugin name
  //! \return container of objects
  Standard_EXPORT const NCollection_List<Handle(Standard_Transient)>& Parameters (
                                                      const TCollection_AsciiString& thePluginName);

  //! Returns true if there are file names set for the given plugin
  //! \param thePluginName a plugin name
  //! \return boolean result
  Standard_EXPORT bool FindFileNames (const TCollection_AsciiString& thePluginName);

  //! Returns file names set for the given plugin
  //! \param thePluginName a plugin name
  //! \return container of names
  Standard_EXPORT const NCollection_List<TCollection_AsciiString>& FileNames(
                                                      const TCollection_AsciiString& thePluginName);

  //! Returns true if there are file names set for the given plugin
  //! \param thePluginName a plugin name
  //! \return boolean result
  Standard_EXPORT bool FindSelectedNames (const TCollection_AsciiString& thePluginName);

  //! Returns name to be selected in the plugin
  //! \param thePluginName a plugin name
  //! \return container of names
  Standard_EXPORT const NCollection_List<TCollection_AsciiString>& GetSelectedNames (
                                                      const TCollection_AsciiString& thePluginName);

  //! Returns objects set for the given plugin
  //! \param thePluginName a plugin name
  //! \return container of objects
  Standard_EXPORT Standard_Boolean GetSelectedObjects (const TCollection_AsciiString& thePluginName,
                                                       NCollection_List<Handle(Standard_Transient)>& theObjects);

#if OCC_VERSION_HEX <= 0x060901
  DEFINE_STANDARD_RTTI (TInspectorAPI_PluginParameters)
#else
  DEFINE_STANDARD_RTTIEXT (TInspectorAPI_PluginParameters, Standard_Transient)
#endif
private:
  //! container of parameters
  NCollection_DataMap<TCollection_AsciiString, NCollection_List<Handle(Standard_Transient)> > myParameters;
  //! container of names
  NCollection_DataMap<TCollection_AsciiString, NCollection_List<TCollection_AsciiString> > myFileNames;
  //! container of select item names
  NCollection_DataMap<TCollection_AsciiString, NCollection_List<TCollection_AsciiString> > mySelectedItemNames;
  //! container of select objects
  NCollection_DataMap<TCollection_AsciiString, NCollection_List<Handle(Standard_Transient)> > mySelectedObjects;
};

#endif
