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

  //! Registers plugin into TInspector window
  //! \param thePluginName a name of the plugin
  void RegisterPlugin (const TCollection_AsciiString& thePluginName) { myWindow->RegisterPlugin (thePluginName); }

  //! Returns list of registered plugins
  //! \return container of plugin names
  NCollection_List<TCollection_AsciiString> RegisteredPlugins() const { return myWindow->RegisteredPlugins(); }

  //! Stores parameters for the plugin
  //! \param theParameters container of parameters(e.g. AIS_InteractiveContext, TDocStd_Application)
  //! \param theAppend boolean state whethe the parameters should be added to existing
  void Init (const NCollection_List<Handle(Standard_Transient)>& theParameters,
             const Standard_Boolean theAppend = Standard_False)
    { myWindow->Init ("", theParameters, theAppend); }

  //! Stores parameters for the plugin
  //! \param thePluginName a name of the plugin
  //! \param theParameters container of parameters(e.g. AIS_InteractiveContext, TDocStd_Application)
  //! \param theAppend boolean state whethe the parameters should be added to existing
  void Init (const TCollection_AsciiString& thePluginName,
             const NCollection_List<Handle(Standard_Transient)>& theParameters,
             const Standard_Boolean theAppend = Standard_False)
  { myWindow->Init (thePluginName, theParameters, theAppend); }

  //! UpdateContent for the TInspector window
  void UpdateContent() { myWindow->UpdateContent(); }

  //! SetOpenButton for the TInspector window
  void SetOpenButton (QPushButton* theButton) { myWindow->SetOpenButton (theButton); }

  //! OpenFile in TInspector window
  void OpenFile (const TCollection_AsciiString& thePluginName, const TCollection_AsciiString& theFileName)
  { myWindow->OpenFile (thePluginName, theFileName); }

  //! Activates plugin
  //! \param thePluginName a name of the plugin
  void Activate (const TCollection_AsciiString& thePluginName) { myWindow->ActivateTool (thePluginName); }

  //! Set item selected in the active plugin
  //! \param theItemName a containerr of name of items in plugin that should become selected
  void SetSelected (const NCollection_List<TCollection_AsciiString>& theItemNames) { myWindow->SetSelected (theItemNames); }

  //! Sets objects to be selected in the plugin
  //! \param theObjects an objects
  Standard_EXPORT void SetSelected (const NCollection_List<Handle(Standard_Transient)>& theObjects)
    { myWindow->SetSelected (theObjects); }

  //! Change window visibility
  //! \param theVisible boolean state
  Standard_EXPORT virtual void SetVisible (const bool theVisible);

private:

  TInspector_Window* myWindow; //!< current window
};

#endif
