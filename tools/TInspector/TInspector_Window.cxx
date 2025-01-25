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

#include <inspector/TInspector_Window.hxx>

#include <inspector/TInspectorAPI_Communicator.hxx>
#include <inspector/TInspector_PluginParameters.hxx>
#include <inspector/TInspector_Shortcut.hxx>
#include <inspector/TreeModel_Tools.hxx>

#include <inspector/ViewControl_Tools.hxx>

#include <OSD_Directory.hxx>
#include <OSD_Environment.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QButtonGroup>
#include <QDockWidget>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QPushButton>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <Standard_WarningsRestore.hxx>

const int TINSPECTOR_DEFAULT_WIDTH      = 650;
const int TINSPECTOR_DEFAULT_HEIGHT     = 500; // 350;
const int TINSPECTOR_DEFAULT_POSITION_X = 200;
const int TINSPECTOR_DEFAULT_POSITION_Y = 60;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TInspector_Window::TInspector_Window()
    : QObject(),
      myOpenButton(0)
{
  myMainWindow = new QMainWindow();

  QWidget* aCentralWidget = new QWidget(myMainWindow);
  myMainWindow->setCentralWidget(aCentralWidget);
  QVBoxLayout* aCentralLayout = new QVBoxLayout(aCentralWidget);
  aCentralLayout->setContentsMargins(0, 0, 0, 0);
  aCentralLayout->setSpacing(0);

  myToolsStack = new QStackedWidget(aCentralWidget);
  myToolsStack->setFrameShape(QFrame::Box);
  aCentralLayout->addWidget(myToolsStack);

  myEmptyWidget = new QWidget(aCentralWidget);
  myToolsStack->addWidget(myEmptyWidget);

  QWidget*     aTopWidget       = new QWidget(aCentralWidget);
  QHBoxLayout* aTopWidgetLayout = new QHBoxLayout(aTopWidget);
  aTopWidgetLayout->setContentsMargins(0, 0, 0, 0);
  aTopWidgetLayout->setSpacing(0);

  myButtonWidget = new QWidget(aCentralWidget);
  myButtonLay    = new QHBoxLayout(myButtonWidget);
  myButtonLay->setContentsMargins(0, 0, 0, 0);
  myButtonLay->setSpacing(0);
  myButtonLay->insertStretch(0, 1);

  myButtonGroup = new QButtonGroup(aCentralWidget);
  myButtonGroup->setExclusive(true);

  myActionsWidget = new QToolButton(aCentralWidget);
  myActionsWidget->setPopupMode(QToolButton::InstantPopup);
  myActionsWidget->setIcon(QIcon(":plugin_actions.png"));
  myActionsWidget->setIconSize(QSize(20, 20));
  QMenu* anActionsMenu = new QMenu(myActionsWidget);
  myActionsWidget->setMenu(anActionsMenu);
  connect(anActionsMenu, SIGNAL(aboutToShow()), this, SLOT(onShowActionsMenu()));

  aTopWidgetLayout->addWidget(myButtonWidget, 1);
  aTopWidgetLayout->addWidget(myActionsWidget);

  aCentralLayout->addWidget(aTopWidget);
  aCentralLayout->addWidget(myToolsStack);

  myMainWindow->resize(TINSPECTOR_DEFAULT_WIDTH, TINSPECTOR_DEFAULT_HEIGHT);
  myMainWindow->move(TINSPECTOR_DEFAULT_POSITION_X, TINSPECTOR_DEFAULT_POSITION_Y);
  myMainWindow->setDockOptions(QMainWindow::VerticalTabs);

  myParameters = new TInspector_PluginParameters(this);

  myDefaultDirectory = defaultTemporaryDirectory();
  myParameters->SetTemporaryDirectory(myDefaultDirectory);

  applyPreferences();

  myShortcut = new TInspector_Shortcut(myMainWindow, this);
}

// =======================================================================
// function : RegisterPlugin
// purpose :
// =======================================================================
void TInspector_Window::RegisterPlugin(const TCollection_AsciiString& thePluginName)
{
  TInspector_ToolInfo anInfo;
  int                 aToolId;
  if (findPlugin(thePluginName, anInfo, aToolId))
    return;

  myToolNames.append(TInspector_ToolInfo(thePluginName));
}

// =======================================================================
// function : RegisteredPlugins
// purpose :
// =======================================================================
NCollection_List<TCollection_AsciiString> TInspector_Window::RegisteredPlugins() const
{
  NCollection_List<TCollection_AsciiString> aPlugins;

  for (int aToolId = 0, aSize = myToolNames.size(); aToolId < aSize; aToolId++)
    aPlugins.Append(myToolNames[aToolId].myName);

  return aPlugins;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void TInspector_Window::Init(const TCollection_AsciiString&                      thePluginName,
                             const NCollection_List<Handle(Standard_Transient)>& theParameters,
                             const Standard_Boolean                              theAppend)
{
  if (thePluginName.IsEmpty())
  {
    // Init all plugins by the given parameters
    for (int aToolId = 0, aSize = myToolNames.size(); aToolId < aSize; aToolId++)
      Init(myToolNames[aToolId].myName, theParameters, theAppend);

    // temporary activation of the first tool
    if (!myToolNames.isEmpty())
      ActivateTool(myToolNames[0].myName);
    return;
  }

  if (theAppend)
  {
    NCollection_List<Handle(Standard_Transient)> aParameters;
    if (myParameters->FindParameters(thePluginName))
      aParameters = myParameters->Parameters(thePluginName);

    for (NCollection_List<Handle(Standard_Transient)>::Iterator anIterator(theParameters);
         anIterator.More();
         anIterator.Next())
      aParameters.Append(anIterator.Value());

    myParameters->SetParameters(thePluginName, aParameters, Standard_False);
  }
  else
    myParameters->SetParameters(thePluginName, theParameters, Standard_False);

  TInspector_ToolInfo anInfo;
  int                 aToolId;
  if (!findPlugin(thePluginName, anInfo, aToolId))
    return;

  if (anInfo.myButton)
    return;

  QString aButtonName = anInfo.myName.ToCString();
  if (aButtonName.indexOf("TK") == 0)
    aButtonName = aButtonName.mid(2);

  QPushButton* aButton = new QPushButton(aButtonName, myButtonWidget);
  aButton->setCheckable(true);
  connect(aButton, SIGNAL(clicked(bool)), this, SLOT(onButtonClicked()));
  myButtonLay->insertWidget(myButtonLay->count() - 1, aButton);
  myButtonGroup->addButton(aButton);
  anInfo.myButton      = aButton;
  myToolNames[aToolId] = anInfo;
}

// =======================================================================
// function : ActivateTool
// purpose :
// =======================================================================
void TInspector_Window::ActivateTool(const TCollection_AsciiString& thePluginName)
{
  int aToolIndex = -1;
  for (int aToolId = 0, aSize = myToolNames.size(); aToolId < aSize; aToolId++)
  {
    if (myToolNames[aToolId].myName != thePluginName)
      continue;
    aToolIndex = aToolId;
    break;
  }

  if (aToolIndex < 0)
    return;

  TInspector_ToolInfo anInfo         = myToolNames[aToolIndex];
  bool                isPluginLoaded = false;
  if (!anInfo.myWidget)
  {
    if (!LoadPlugin(thePluginName, anInfo))
    {
      anInfo.myButton->setEnabled(false);
      return;
    }
    isPluginLoaded = true;
    myToolsStack->addWidget(anInfo.myWidget);
    myToolNames[aToolIndex] = anInfo;
  }

  QWidget* aWidget = anInfo.myWidget;
  myToolsStack->setCurrentWidget(aWidget);
  if (myOpenButton)
    myOpenButton->setObjectName(thePluginName.ToCString());

  anInfo.myCommunicator->UpdateContent();
  if (isPluginLoaded)
  {
    // apply preferences
    TInspectorAPI_PreferencesDataMap aPreferences;
    myParameters->GetPreferences(thePluginName, aPreferences);
    anInfo.myCommunicator->SetPreferences(aPreferences);
  }
  onCommuncatorNameChanged();
}

// =======================================================================
// function : SetSelected
// purpose :
// =======================================================================
void TInspector_Window::SetSelected(const NCollection_List<TCollection_AsciiString>& theItemNames)
{
  TInspector_ToolInfo anInfo;
  if (!activeToolInfo(anInfo))
    return;

  myParameters->SetSelectedNames(anInfo.myName, theItemNames);

  TInspectorAPI_Communicator* aCommunicator = anInfo.myCommunicator;
  if (aCommunicator)
  {
    aCommunicator->UpdateContent();
  }
}

// =======================================================================
// function : SetSelected
// purpose :
// =======================================================================
void TInspector_Window::SetSelected(const NCollection_List<Handle(Standard_Transient)>& theObjects)
{
  TInspector_ToolInfo anInfo;
  if (!activeToolInfo(anInfo))
    return;

  myParameters->SetSelected(anInfo.myName, theObjects);

  TInspectorAPI_Communicator* aCommunicator = anInfo.myCommunicator;
  if (aCommunicator)
  {
    aCommunicator->UpdateContent();
  }
}

// =======================================================================
// function : SetOpenButton
// purpose :
// =======================================================================
void TInspector_Window::SetOpenButton(QPushButton* theButton)
{
  myOpenButton = theButton;
  TInspector_ToolInfo anInfo;
  if (activeToolInfo(anInfo))
    myOpenButton->setObjectName(anInfo.myName.ToCString());
  myButtonLay->insertWidget(0, theButton);
}

// =======================================================================
// function : OpenFile
// purpose :
// =======================================================================
void TInspector_Window::OpenFile(const TCollection_AsciiString& thePluginName,
                                 const TCollection_AsciiString& theFileName)
{
  if (thePluginName.IsEmpty())
  {
    // Apply file name to all plugins
    for (int aToolId = 0, aSize = myToolNames.size(); aToolId < aSize; aToolId++)
      OpenFile(myToolNames[aToolId].myName, theFileName);
    return;
  }

  myParameters->AddFileName(thePluginName, theFileName);

  TInspector_ToolInfo anInfo;
  if (!activeToolInfo(anInfo) || anInfo.myName != thePluginName)
    return;

  TInspectorAPI_Communicator* aCommunicator = anInfo.myCommunicator;
  if (aCommunicator)
    aCommunicator->UpdateContent();
}

// =======================================================================
// function : UpdateContent
// purpose :
// =======================================================================
void TInspector_Window::UpdateContent()
{
  TInspector_ToolInfo anInfo;
  if (activeToolInfo(anInfo) && anInfo.myCommunicator)
    anInfo.myCommunicator->UpdateContent();
}

// =======================================================================
// function : LoadPlugin
// purpose :
// =======================================================================
bool TInspector_Window::LoadPlugin(const TCollection_AsciiString& thePluginName,
                                   TInspector_ToolInfo&           theInfo)
{
  bool aLoaded = false;

  QApplication::setOverrideCursor(Qt::WaitCursor);
  TInspectorAPI_Communicator* aCommunicator =
    TInspectorAPI_Communicator::LoadPluginLibrary(thePluginName);

  if (aCommunicator)
  {
    aCommunicator->SetParameters(myParameters);
    QWidget*     aParentWidget = new QWidget(myMainWindow);
    QVBoxLayout* aLayout       = new QVBoxLayout(aParentWidget);
    aLayout->setContentsMargins(0, 0, 0, 0);
    aLayout->setSpacing(0);
    aParentWidget->setLayout(aLayout);
    aCommunicator->SetParent(aParentWidget);
    theInfo.myWidget       = aParentWidget;
    theInfo.myCommunicator = aCommunicator;
#if QT_VERSION >= 0x050000
    connect(aParentWidget,
            SIGNAL(objectNameChanged(const QString&)),
            this,
            SLOT(onCommuncatorNameChanged()));
#endif
    aLoaded = true;
  }
  QApplication::restoreOverrideCursor();
  return aLoaded;
}

// =======================================================================
// function : GetPreferences
// purpose :
// =======================================================================
void TInspector_Window::GetPreferences(TInspectorAPI_PreferencesDataMap& theItem)
{
  theItem.Bind("geometry",
               TreeModel_Tools::ToString(myMainWindow->saveGeometry()).toStdString().c_str());
}

// =======================================================================
// function : SetPreferences
// purpose :
// =======================================================================
void TInspector_Window::SetPreferences(const TInspectorAPI_PreferencesDataMap& theItem)
{
  for (TInspectorAPI_IteratorOfPreferencesDataMap anItemIt(theItem); anItemIt.More();
       anItemIt.Next())
  {
    if (anItemIt.Key().IsEqual("geometry"))
      myMainWindow->restoreGeometry(TreeModel_Tools::ToByteArray(anItemIt.Value().ToCString()));
  }
}

// =======================================================================
// function : Dump
// purpose :
// =======================================================================
void TInspector_Window::Dump(Standard_OStream& theStream) const
{
  TInspector_ToolInfo anInfo;
  activeToolInfo(anInfo);

  theStream << "Active Plugin: " << anInfo.myName << "\n";
  theStream << "Temporary Directory: " << GetTemporaryDirectory() << "\n";
}

// =======================================================================
// function : OnStorePreferences
// purpose :
// =======================================================================
void TInspector_Window::OnStorePreferences()
{
  Handle(TInspector_PluginParameters) aParameters =
    Handle(TInspector_PluginParameters)::DownCast(myParameters);
  TInspectorAPI_PreferencesDataMap aPreferences;
  GetPreferences(aPreferences);
  aParameters->SetPreferences("Desktop", aPreferences);

  TInspector_ToolInfo anInfo;
  for (int aToolId = 0, aSize = myToolNames.size(); aToolId < aSize; aToolId++)
  {
    anInfo = myToolNames[aToolId];
    if (!anInfo.myCommunicator)
      continue;

    aParameters->GetPreferences(anInfo.myName, aPreferences);
    anInfo.myCommunicator->GetPreferences(aPreferences);
    myParameters->SetPreferences(anInfo.myName, aPreferences);
  }

  // store preferences parameters into a file
  aParameters->StorePreferences();
}

// =======================================================================
// function : OnRemovePreferences
// purpose :
// =======================================================================
void TInspector_Window::OnRemovePreferences()
{
  Handle(TInspector_PluginParameters) aParameters =
    Handle(TInspector_PluginParameters)::DownCast(myParameters);

  // remove preferences file
  aParameters->RemovePreferences();

  // restore plugins default state
  TInspector_ToolInfo anInfo;
  for (int aToolId = 0, aSize = myToolNames.size(); aToolId < aSize; aToolId++)
  {
    anInfo = myToolNames[aToolId];
    if (!anInfo.myCommunicator)
      continue;
    anInfo.myCommunicator->SetPreferences(TInspectorAPI_PreferencesDataMap());
  }
}

// =======================================================================
// function : onButtonClicked
// purpose :
// =======================================================================
void TInspector_Window::onButtonClicked()
{
  QPushButton* aButton = (QPushButton*)sender();

  TCollection_AsciiString aPluginName = aButton->text().toStdString().c_str();

  TInspector_ToolInfo anInfo;
  int                 aToolId;
  if (!findPlugin(aPluginName, anInfo, aToolId))
    aPluginName = TCollection_AsciiString("TK") + aPluginName;

  ActivateTool(aPluginName);
}

// =======================================================================
// function : onShowActionsMenu
// purpose :
// =======================================================================
void TInspector_Window::onShowActionsMenu()
{
  myActionsWidget->menu()->clear();

  TInspector_ToolInfo anInfo;
  activeToolInfo(anInfo);

  QMenu* aMenu = myActionsWidget->menu();
  anInfo.myCommunicator->FillActionsMenu(aMenu);

  aMenu->addSeparator();
  aMenu->addAction(ViewControl_Tools::CreateAction(tr("Store Preferences"),
                                                   SLOT(OnStorePreferences()),
                                                   myMainWindow,
                                                   this));
  QAction* anAction = ViewControl_Tools::CreateAction(tr("Remove Preferences"),
                                                      SLOT(OnRemovePreferences()),
                                                      myMainWindow,
                                                      this);
  anAction->setToolTip("Default state will be restored after restarting the application");
  aMenu->addAction(anAction);
}

// =======================================================================
// function : onCommuncatorNameChanged
// purpose :
// =======================================================================
void TInspector_Window::onCommuncatorNameChanged()
{
#if QT_VERSION >= 0x050000
  TInspector_ToolInfo anInfo;
  if (!activeToolInfo(anInfo))
    return;
  myMainWindow->setWindowTitle(anInfo.myWidget->objectName());
#endif
}

// =======================================================================
// function : activeToolInfo
// purpose :
// =======================================================================
bool TInspector_Window::activeToolInfo(TInspector_Window::TInspector_ToolInfo& theToolInfo) const
{
  QWidget* anActiveWidget = myToolsStack->currentWidget();
  if (anActiveWidget == myEmptyWidget)
    return false;

  for (int aToolId = 0, aSize = myToolNames.size(); aToolId < aSize; aToolId++)
  {
    if (myToolNames[aToolId].myWidget && myToolNames[aToolId].myWidget == anActiveWidget)
    {
      theToolInfo = myToolNames[aToolId];
      return true;
    }
  }
  return false;
}

// =======================================================================
// function : findPlugin
// purpose :
// =======================================================================
bool TInspector_Window::findPlugin(const TCollection_AsciiString& thePluginName,
                                   TInspector_ToolInfo&           theToolInfo,
                                   int&                           theToolId)
{
  for (int aToolId = 0, aSize = myToolNames.size(); aToolId < aSize; aToolId++)
  {
    TInspector_ToolInfo anInfo = myToolNames[aToolId];
    if (anInfo.myName != thePluginName)
      continue;
    theToolInfo = anInfo;
    theToolId   = aToolId;
    return true;
  }

  return false;
}

// =======================================================================
// function : applyPreferences
// purpose :
// =======================================================================
void TInspector_Window::applyPreferences()
{
  TInspectorAPI_PreferencesDataMap aPreferences;
  myParameters->GetPreferences("Desktop", aPreferences);
  SetPreferences(aPreferences);
}

// =======================================================================
// function : defaultTemporaryDirectory
// purpose :
// =======================================================================
TCollection_AsciiString TInspector_Window::defaultTemporaryDirectory() const
{
  // main window creation
  TCollection_AsciiString aTmpDir;
#ifdef _WIN32
  OSD_Environment anEnvironment("TEMP");
  aTmpDir = anEnvironment.Value();
  if (aTmpDir.IsEmpty())
  {
    anEnvironment.SetName("TMP");
    aTmpDir = anEnvironment.Value();
    if (aTmpDir.IsEmpty())
      aTmpDir = "C:\\";
  }
  if (!aTmpDir.EndsWith("\\"))
    aTmpDir += "\\";
  OSD_Path      aTmpPath(aTmpDir);
  OSD_Directory aTmpDirectory;
#else
  OSD_Directory aTmpDirectory = OSD_Directory::BuildTemporary();
  OSD_Path      aTmpPath;
  aTmpDirectory.Path(aTmpPath);
#endif
  aTmpPath.DownTrek("TInspector");
  aTmpDirectory.SetPath(aTmpPath);
  if (!aTmpDirectory.Exists())
    aTmpDirectory.Build(OSD_Protection());

  aTmpDirectory.Path(aTmpPath);
  TCollection_AsciiString aTmpDirectoryName;
  aTmpPath.SystemName(aTmpDirectoryName);

  return aTmpDir;
}
