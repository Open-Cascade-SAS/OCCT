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

#include <TInspector_Window.hxx>

#include <TInspectorAPI_Communicator.hxx>

#include <QApplication>
#include <QDockWidget>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

const int TINSPECTOR_DEFAULT_WIDTH = 900;
const int TINSPECTOR_DEFAULT_HEIGHT = 700;
const int TINSPECTOR_DEFAULT_POSITION_X = 200;
const int TINSPECTOR_DEFAULT_POSITION_Y = 60;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TInspector_Window::TInspector_Window()
: QObject(), myOpenButton (0)
{
  myMainWindow = new QMainWindow();

  QWidget* aCentralWidget = new QWidget (myMainWindow);
  myMainWindow->setCentralWidget (aCentralWidget);
  QVBoxLayout* aCentralLayout = new QVBoxLayout (aCentralWidget);
  aCentralLayout->setContentsMargins (0, 0, 0, 0);
  aCentralLayout->setSpacing (0);

  myToolsStack = new QStackedWidget (aCentralWidget);
  myToolsStack->setFrameShape (QFrame::Box);
  aCentralLayout->addWidget (myToolsStack);

  myEmptyWidget = new QWidget (aCentralWidget);
  myToolsStack->addWidget (myEmptyWidget);

  myButtonWidget = new QWidget (aCentralWidget);
  myButtonLay = new QHBoxLayout (myButtonWidget);
  myButtonLay->setContentsMargins (0, 0, 0, 0);
  myButtonLay->setSpacing (0);
  myButtonLay->insertStretch (0, 1);

  aCentralLayout->addWidget (myButtonWidget);
  aCentralLayout->addWidget (myToolsStack);

  myMainWindow->resize (TINSPECTOR_DEFAULT_WIDTH, TINSPECTOR_DEFAULT_HEIGHT);
  myMainWindow->move (TINSPECTOR_DEFAULT_POSITION_X, TINSPECTOR_DEFAULT_POSITION_Y);
  myMainWindow->setDockOptions (QMainWindow::VerticalTabs);

  myParameters = new TInspectorAPI_PluginParameters();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void TInspector_Window::Init (const TCollection_AsciiString& thePluginName,
                              const NCollection_List<Handle(Standard_Transient)>& theParameters)
{
  if (thePluginName.IsEmpty())
  {
    // Init all plugins by the given parameters
    for (int aToolId = 0, aSize = myToolNames.size(); aToolId < aSize; aToolId++)
      Init (myToolNames[aToolId].myName, theParameters);

    // temporary activation of the first tool
    if (!myToolNames.isEmpty())
      ActivateTool (myToolNames[0].myName);
    return;
  }

  myParameters->SetParameters (thePluginName, theParameters);

  for (int aToolId = 0, aSize = myToolNames.size(); aToolId < aSize; aToolId++)
  {
    TInspector_ToolInfo anInfo = myToolNames[aToolId];
    if (anInfo.myName != thePluginName)
      continue;

    if (anInfo.myCommunicator)
      break;

    QPushButton* aButton = new QPushButton(anInfo.myName.ToCString(), myButtonWidget);
    connect (aButton, SIGNAL (clicked (bool)), this, SLOT (onButtonClicked()));
    myButtonLay->insertWidget (myButtonLay->count()-1, aButton);
    anInfo.myButton = aButton;
    myToolNames[aToolId] = anInfo;
    break;
  }
}

// =======================================================================
// function : ActivateTool
// purpose :
// =======================================================================
void TInspector_Window::ActivateTool (const TCollection_AsciiString& thePluginName)
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

  TInspector_ToolInfo anInfo = myToolNames[aToolIndex];
  if (!anInfo.myWidget)
  {
    if (!LoadPlugin (thePluginName, anInfo))
    {
      anInfo.myButton->setEnabled (false);
      return;
    }
    myToolsStack->addWidget (anInfo.myWidget);
    myToolNames[aToolIndex] = anInfo;
  }

  QWidget* aWidget = anInfo.myWidget;
  myToolsStack->setCurrentWidget (aWidget);
  if (myOpenButton)
    myOpenButton->setObjectName (thePluginName.ToCString());

  anInfo.myCommunicator->UpdateContent();
  onCommuncatorNameChanged();
}

// =======================================================================
// function : SetOpenButton
// purpose :
// =======================================================================
void TInspector_Window::SetOpenButton (QPushButton* theButton)
{
  myOpenButton = theButton;
  TInspector_ToolInfo anInfo;
  if (ActiveToolInfo (anInfo))
    myOpenButton->setObjectName (anInfo.myName.ToCString());
  myButtonLay->insertWidget (0, theButton);
}

// =======================================================================
// function : OpenFile
// purpose :
// =======================================================================
void TInspector_Window::OpenFile (const TCollection_AsciiString& thePluginName,
                                  const TCollection_AsciiString& theFileName)
{
  myParameters->AddFileName (thePluginName, theFileName);

  TInspector_ToolInfo anInfo;
  if (!ActiveToolInfo (anInfo) || anInfo.myName != thePluginName)
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
  if (!ActiveToolInfo (anInfo) || !anInfo.myCommunicator)
    return;

  anInfo.myCommunicator->UpdateContent();
}

// =======================================================================
// function : LoadPlugin
// purpose :
// =======================================================================
bool TInspector_Window::LoadPlugin (const TCollection_AsciiString& thePluginName, TInspector_ToolInfo& theInfo)
{
  bool aLoaded = false;

  QApplication::setOverrideCursor (Qt::WaitCursor);
  TInspectorAPI_Communicator* aCommunicator = TInspectorAPI_Communicator::LoadPluginLibrary (thePluginName);

  if (aCommunicator)
  {
    aCommunicator->SetParameters (myParameters);
    QWidget* aParentWidget = new QWidget (myMainWindow);
    QVBoxLayout* aLayout = new QVBoxLayout (aParentWidget);
    aLayout->setContentsMargins (0, 0, 0, 0);
    aLayout->setSpacing (0);
    aParentWidget->setLayout (aLayout);
    aCommunicator->SetParent (aParentWidget);
    theInfo.myWidget = aParentWidget;
    theInfo.myCommunicator = aCommunicator;
#if QT_VERSION >= 0x050000
    connect (aParentWidget, SIGNAL (objectNameChanged (const QString&)), this, SLOT (onCommuncatorNameChanged()));
#endif
    aLoaded = true;
  }
  QApplication::restoreOverrideCursor();
  return aLoaded;
}

// =======================================================================
// function : onLastApplicationWindowClosed
// purpose :
// =======================================================================
void TInspector_Window::OnLastApplicationWindowClosed()
{
  for (int aToolId = 0, aSize = myToolNames.size(); aToolId < aSize; aToolId++)
    delete myToolNames[aToolId].myCommunicator;
}

// =======================================================================
// function : onButtonClicked
// purpose :
// =======================================================================
void TInspector_Window::onButtonClicked()
{
  QPushButton* aButton = (QPushButton*)sender();
  ActivateTool (TCollection_AsciiString (aButton->text().toStdString().c_str()));
}

// =======================================================================
// function : onCommuncatorNameChanged
// purpose :
// =======================================================================
void TInspector_Window::onCommuncatorNameChanged()
{
#if QT_VERSION >= 0x050000
  TInspector_ToolInfo anInfo;
  if (!ActiveToolInfo (anInfo))
    return;
  myMainWindow->setWindowTitle (anInfo.myWidget->objectName());
#endif
}

// =======================================================================
// function : ActiveToolInfo
// purpose :
// =======================================================================
bool TInspector_Window::ActiveToolInfo (TInspector_Window::TInspector_ToolInfo& theToolInfo) const
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
