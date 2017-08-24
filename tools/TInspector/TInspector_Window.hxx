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

#ifndef TInspector_Window_H
#define TInspector_Window_H

#include <NCollection_List.hxx>
#include <Standard.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <inspector/TInspectorAPI_PluginParameters.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QMap>
#include <QObject>
#include <QString>

class TInspectorAPI_Communicator;

class QMainWindow;
class QHBoxLayout;
class QPushButton;
class QStackedWidget;

//! \class Inspector_Window
//! Control that contains:
//! - stacked widget of loaded plugins
//! - Open button to open file in an active plugin
//! - plugin parameters container
class TInspector_Window : public QObject
{
  Q_OBJECT
private:

  //! Container of plugin information
  struct TInspector_ToolInfo
  {

    //! Constructor
    TInspector_ToolInfo (const TCollection_AsciiString& theName = TCollection_AsciiString())
      : myName(theName), myCommunicator (0), myButton (0), myWidget (0) {}

    TCollection_AsciiString myName; //!< plugin name
    TInspectorAPI_Communicator* myCommunicator; //!< plugin communicator
    QPushButton* myButton; //!< button with plugin name, that will be added into TInspector window layout
    QWidget* myWidget; //!< parent widget of the plugin
  };

public:

  //! Constructor
  Standard_EXPORT TInspector_Window();

  //! Destructor
  virtual ~TInspector_Window() Standard_OVERRIDE {}

  //! Appends the plugin names into internal conainer
  //! \param thePluginName a name of the plugin
  Standard_EXPORT void RegisterPlugin (const TCollection_AsciiString& thePluginName);

  //! Returns list of registered plugins
  //! \return container of plugin names
  Standard_EXPORT NCollection_List<TCollection_AsciiString> RegisteredPlugins() const;

  //! Stores parameters for the plugin. If the plugin name is empty, it inits all plugins with the parameters
  //! \param thePluginName a name of the plugin
  //! \param theParameters container of parameters(e.g. AIS_InteractiveContext, TDocStd_Application)
  //! \param theAppend boolean state whethe the parameters should be added to existing
  Standard_EXPORT void Init (const TCollection_AsciiString& thePluginName,
                             const NCollection_List<Handle(Standard_Transient)>& theParameters,
                             const Standard_Boolean theAppend = Standard_False);

  //! Appends to container of parameters the given name, if the given parameter is active, cal UpdateContent
  //! \param thePluginName a name of the plugin
  //! \param theParameters container of parameters(e.g. AIS_InteractiveContext, TDocStd_Application)
  Standard_EXPORT void OpenFile (const TCollection_AsciiString& thePluginName,
                                 const TCollection_AsciiString& theFileName);

  //! Calls UpdateContent for the active plugin
  Standard_EXPORT void UpdateContent();

  //! Returns the main TInspector window
  QMainWindow* GetMainWindow() const { return myMainWindow; }

  //! Activates the plugin. Loads the plugin if it has not been loaded yet
  //! \param thePluginName a name of the plugin
  Standard_EXPORT void ActivateTool (const TCollection_AsciiString& thePluginName);

  //! Set item selected in the active plugin
  //! \param theItemNames a container of name of items in plugin that should become selected
  Standard_EXPORT void SetSelected (const NCollection_List<TCollection_AsciiString>& theItemNames);

  //! Sets objects to be selected in the plugin
  //! \param theObjects an objects
  Standard_EXPORT void SetSelected (const NCollection_List<Handle(Standard_Transient)>& theObjects);

  //! Sets open button. Stores into objectName for the button the name of the current plugin to know where
  //! the file should be applied
  //! \param theButton a button
  Standard_EXPORT void SetOpenButton (QPushButton* theButton);

  //! Loads plugin, appends the plugin widget into layout, stores the plugin information
  //! \param thePluginName a name of the plugin
  //! \param theInfo an output parameter for plugin info
  Standard_EXPORT bool LoadPlugin (const TCollection_AsciiString& thePluginName, TInspector_ToolInfo& theInfo);

protected slots:

  //! Activates plugin correspnded to the clicked button
  void onButtonClicked();

  //! Updates the TInspector window title giving object name of plugin widget (available only in Qt5)
  void onCommuncatorNameChanged();

protected:

  //! Activates plugin by the plugin info 
  //! \param theToolInfo an information about plugin
  bool ActiveToolInfo (TInspector_ToolInfo& theToolInfo) const;

  //! Returns true if there is plugin registered by the given name
  //! \param thePluginName a name of the plugin
  //! \param theToolInfo an output parameter for plugin information
  //! \param theToolId an index in internal map
  //! \return true if the plugin is found
  bool FindPlugin (const TCollection_AsciiString& thePluginName, TInspector_ToolInfo& theToolInfo,
                   int& theToolId);

private:

  QWidget* myEmptyWidget; //!< widget that is active in tools stack while no one plugin is loaded
  QMainWindow* myMainWindow; //!< main control of the window
  QStackedWidget* myToolsStack; //!< stack widget of plugin windows
  QWidget* myButtonWidget; //!< container of plugin buttons
  QPushButton* myOpenButton; //!< button to open file for the active plugin
  QHBoxLayout* myButtonLay; //!< layout of plugin buttons
  QList<TInspector_ToolInfo> myToolNames; //!< container of plugin names
  Handle(TInspectorAPI_PluginParameters) myParameters; //!< plugins parameters container
};

#endif
