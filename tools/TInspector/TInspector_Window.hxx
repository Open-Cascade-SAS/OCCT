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

#include <Standard_WarningsDisable.hxx>
#include <QMap>
#include <QObject>
#include <QString>
#include <Standard_WarningsRestore.hxx>

class TInspectorAPI_Communicator;
class TInspector_Shortcut;

class QButtonGroup;
class QMainWindow;
class QHBoxLayout;
class QPushButton;
class QStackedWidget;
class QToolButton;

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
    TInspector_ToolInfo(const TCollection_AsciiString& theName = TCollection_AsciiString())
        : myName(theName),
          myCommunicator(0),
          myButton(0),
          myWidget(0)
    {
    }

    TCollection_AsciiString     myName;         //!< plugin name
    TInspectorAPI_Communicator* myCommunicator; //!< plugin communicator
    QPushButton*
      myButton; //!< button with plugin name, that will be added into TInspector window layout
    QWidget* myWidget; //!< parent widget of the plugin
  };

public:
  //! Constructor
  Standard_EXPORT TInspector_Window();

  //! Destructor
  virtual ~TInspector_Window() {}

  //! Appends the plugin names into internal container
  //! \param thePluginName a name of the plugin
  Standard_EXPORT void RegisterPlugin(const TCollection_AsciiString& thePluginName);

  //! Returns list of registered plugins
  //! \return container of plugin names
  Standard_EXPORT NCollection_List<TCollection_AsciiString> RegisteredPlugins() const;

  //! Stores parameters for the plugin. If the plugin name is empty, it inits all plugins with the
  //! parameters \param thePluginName a name of the plugin \param theParameters container of
  //! parameters(e.g. AIS_InteractiveContext, TDocStd_Application) \param theAppend boolean state
  //! whether the parameters should be added to existing
  Standard_EXPORT void Init(const TCollection_AsciiString&                      thePluginName,
                            const NCollection_List<Handle(Standard_Transient)>& theParameters,
                            const Standard_Boolean theAppend = Standard_False);

  //! Appends to container of parameters the given name, if the given parameter is active, cal
  //! UpdateContent \param thePluginName a name of the plugin \param theParameters container of
  //! parameters(e.g. AIS_InteractiveContext, TDocStd_Application)
  Standard_EXPORT void OpenFile(const TCollection_AsciiString& thePluginName,
                                const TCollection_AsciiString& theFileName);

  //! Calls UpdateContent for the active plugin
  Standard_EXPORT void UpdateContent();

  //! Returns the main TInspector window
  QMainWindow* GetMainWindow() const { return myMainWindow; }

  //! Activates the plugin. Loads the plugin if it has not been loaded yet
  //! \param thePluginName a name of the plugin
  Standard_EXPORT void ActivateTool(const TCollection_AsciiString& thePluginName);

  //! Sets item selected in the active plugin
  //! \param theItemNames a container of name of items in plugin that should become selected
  Standard_EXPORT void SetSelected(const NCollection_List<TCollection_AsciiString>& theItemNames);

  //! Sets objects to be selected in the plugin
  //! \param theObjects an objects
  Standard_EXPORT void SetSelected(const NCollection_List<Handle(Standard_Transient)>& theObjects);

  //! Sets open button. Stores into objectName for the button the name of the current plugin to know
  //! where the file should be applied \param theButton a button
  Standard_EXPORT void SetOpenButton(QPushButton* theButton);

  //! Loads plugin, appends the plugin widget into layout, stores the plugin information
  //! \param thePluginName a name of the plugin
  //! \param theInfo an output parameter for plugin info
  Standard_EXPORT bool LoadPlugin(const TCollection_AsciiString& thePluginName,
                                  TInspector_ToolInfo&           theInfo);

  //! Sets path to a directory for temporary plugin files. If parameter is empty, using default
  //! directory \param thePath a path
  void SetTemporaryDirectory(const TCollection_AsciiString& thePath)
  {
    myParameters->SetTemporaryDirectory(thePath.IsEmpty() ? myDefaultDirectory : thePath);
  }

  //! Returns path to a directory for temporary plugin files
  //! \return path
  TCollection_AsciiString GetTemporaryDirectory() const
  {
    return myParameters->GetTemporaryDirectory();
  }

  //! Returns plugins parameters container
  //! \return instance of parameters container
  Handle(TInspectorAPI_PluginParameters) GetPluginParameters() const { return myParameters; }

  //! Returns plugin preferences: dock widgets state, tree view columns.
  //! \param theItem container of preference elements
  Standard_EXPORT void GetPreferences(TInspectorAPI_PreferencesDataMap& theItem);

  //! Applies plugin preferences
  //! \param theItem container of preference elements
  Standard_EXPORT void SetPreferences(const TInspectorAPI_PreferencesDataMap& theItem);

  //! Puts in the stream information about communicator
  //! \param theStream stream for output
  Standard_EXPORT void Dump(Standard_OStream& theStream) const;

public slots:
  //! Stores preferences (current state) of all plugins into a preferences file
  Standard_EXPORT void OnStorePreferences();

  //! Removes preferences file
  Standard_EXPORT void OnRemovePreferences();

protected slots:

  //! Activates plugin corresponded to the clicked button
  void onButtonClicked();

  //! Updates available actions by the active plugin
  void onShowActionsMenu();

  //! Updates the TInspector window title giving object name of plugin widget (available only in
  //! Qt5)
  void onCommuncatorNameChanged();

protected:
  //! Activates plugin by the plugin info
  //! \param theToolInfo an information about plugin
  bool activeToolInfo(TInspector_ToolInfo& theToolInfo) const;

  //! Returns true if there is plugin registered by the given name
  //! \param thePluginName a name of the plugin
  //! \param theToolInfo an output parameter for plugin information
  //! \param theToolId an index in internal map
  //! \return true if the plugin is found
  bool findPlugin(const TCollection_AsciiString& thePluginName,
                  TInspector_ToolInfo&           theToolInfo,
                  int&                           theToolId);

  //! Applies desktop preferences to window
  void applyPreferences();

  //! Generates default temp directory by 'TEMP' or 'TMP' environment variables
  //! \return generated path
  TCollection_AsciiString defaultTemporaryDirectory() const;

private:
  QWidget* myEmptyWidget; //!< widget that is active in tools stack while no one plugin is loaded
  QMainWindow*               myMainWindow;    //!< main control of the window
  QStackedWidget*            myToolsStack;    //!< stack widget of plugin windows
  QWidget*                   myButtonWidget;  //!< container of plugin buttons
  QPushButton*               myOpenButton;    //!< button to open file for the active plugin
  QHBoxLayout*               myButtonLay;     //!< layout of plugin buttons
  QButtonGroup*              myButtonGroup;   //!< exclusive toggled button
  QToolButton*               myActionsWidget; //!< tool button for plugin actions
  QList<TInspector_ToolInfo> myToolNames;     //!< container of plugin names
  Handle(TInspectorAPI_PluginParameters) myParameters;       //!< plugins parameters container
  TInspector_Shortcut*                   myShortcut;         //!< listener of key events
  TCollection_AsciiString                myDefaultDirectory; //!< default directory
};

#endif
