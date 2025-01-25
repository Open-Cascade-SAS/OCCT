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

#ifndef TInspector_Communicator_H
#define TInspector_Communicator_H

#include <inspector/TInspector_Window.hxx>
#include <inspector/TInspectorAPI_PluginParameters.hxx>

#include <NCollection_List.hxx>
#include <Standard.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

class QPushButton;

//! \class TInspector_Communicator.
//! \brief This is a connector from TInspector window to:
//! - register tool plugin
//! - give parameters into plugin
class TInspector_Communicator
{
public:
  //! Constructor
  Standard_EXPORT TInspector_Communicator();

  //! Destructor
  virtual ~TInspector_Communicator() {}

  //! Returns directory of Qt plugins. Firstly it founds it in QTDIR, else if not defined in PATH
  Standard_EXPORT static Standard_Boolean PluginsDir(TCollection_AsciiString& thePlugindsDirName);

  //! Registers plugin into TInspector window
  //! \param thePluginName a name of the plugin
  void RegisterPlugin(const TCollection_AsciiString& thePluginName)
  {
    myWindow->RegisterPlugin(thePluginName);
  }

  //! Returns list of registered plugins
  //! \return container of plugin names
  NCollection_List<TCollection_AsciiString> RegisteredPlugins() const
  {
    return myWindow->RegisteredPlugins();
  }

  //! Stores parameters for the plugin
  //! \param theParameters container of parameters(e.g. AIS_InteractiveContext, TDocStd_Application)
  //! \param theAppend boolean state whether the parameters should be added to existing
  void Init(const NCollection_List<Handle(Standard_Transient)>& theParameters,
            const Standard_Boolean                              theAppend = Standard_False)
  {
    myWindow->Init("", theParameters, theAppend);
  }

  //! Stores parameters for the plugin
  //! \param thePluginName a name of the plugin
  //! \param theParameters container of parameters(e.g. AIS_InteractiveContext, TDocStd_Application)
  //! \param theAppend boolean state whether the parameters should be added to existing
  void Init(const TCollection_AsciiString&                      thePluginName,
            const NCollection_List<Handle(Standard_Transient)>& theParameters,
            const Standard_Boolean                              theAppend = Standard_False)
  {
    myWindow->Init(thePluginName, theParameters, theAppend);
  }

  //! Updates content for the TInspector window
  void UpdateContent() { myWindow->UpdateContent(); }

  //! Sets open button for the TInspector window
  void SetOpenButton(QPushButton* theButton) { myWindow->SetOpenButton(theButton); }

  //! Opens file in TInspector window
  void OpenFile(const TCollection_AsciiString& thePluginName,
                const TCollection_AsciiString& theFileName)
  {
    myWindow->OpenFile(thePluginName, theFileName);
  }

  //! Activates plugin
  //! \param thePluginName a name of the plugin
  void Activate(const TCollection_AsciiString& thePluginName)
  {
    myWindow->ActivateTool(thePluginName);
  }

  //! Sets item selected in the active plugin
  //! \param theItemName a container of name of items in plugin that should become selected
  void SetSelected(const NCollection_List<TCollection_AsciiString>& theItemNames)
  {
    myWindow->SetSelected(theItemNames);
  }

  //! Sets objects to be selected in the plugin
  //! \param theObjects an objects
  void SetSelected(const NCollection_List<Handle(Standard_Transient)>& theObjects)
  {
    myWindow->SetSelected(theObjects);
  }

  //! Sets path to a directory for temporary plugin files
  //! \param thePath a path
  void SetTemporaryDirectory(const TCollection_AsciiString& thePath)
  {
    myWindow->SetTemporaryDirectory(thePath);
  }

  //! Returns path to a directory for temporary plugin files
  //! \return path
  TCollection_AsciiString GetTemporaryDirectory() const
  {
    return myWindow->GetTemporaryDirectory();
  }

  //! Changes window visibility
  //! \param theVisible boolean state
  Standard_EXPORT virtual void SetVisible(const bool theVisible);

  //! Changes window position
  //! \param theX X pixel position of top left corner of the window
  //! \param theY Y pixel position
  Standard_EXPORT virtual void Move(const int theXPosition, const int theYPosition);

  //! Puts in the stream information about communicator
  //! \param theStream stream for output
  void Dump(Standard_OStream& theStream) const { return myWindow->Dump(theStream); }

  //! Returns plugins parameters container
  Handle(TInspectorAPI_PluginParameters) const GetPluginParameters()
  {
    return myWindow->GetPluginParameters();
  }

private:
  TInspector_Window* myWindow; //!< current window
};

#endif
