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


#include <TInspectorEXE_OpenFileDialog.hxx>

#include <inspector/TInspector_Communicator.hxx>

#include <OSD_Environment.hxx>

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QMainWindow>
#include <QToolBar>

#include <set>

// =======================================================================
// function : fileNameInDataDir
// purpose :
// =======================================================================
TCollection_AsciiString fileNameInDataDir(const TCollection_AsciiString& theEnvironmentDir,
                                              const TCollection_AsciiString& theName)
{
  OSD_Environment anEnvironment(theEnvironmentDir);
  
  TCollection_AsciiString aFileName = anEnvironment.Value();
  aFileName += TCollection_AsciiString("/") + theName;

  return aFileName;
}

// =======================================================================
// function : setPluginSampleDirectory
// purpose :
// =======================================================================
void setPluginSampleDirectory (const TCollection_AsciiString& theName, TInspector_Communicator* theCommunicator,
                               TInspectorEXE_OpenButton* theButtonControl)
{
  if (theName.IsEqual ("TKDFBrowser"))
  {
    theCommunicator->OpenFile (theName, fileNameInDataDir ("CSF_OCCTDataPath", "step/screw.step"));
    theButtonControl->SetPluginDir (theName, fileNameInDataDir ("CSF_OCCTDataPath", "step"));
  }
  else if (theName.IsEqual ("TKShapeView"))
  {
    theCommunicator->OpenFile (theName, fileNameInDataDir ("CSF_OCCTDataPath", "occ/hammer.brep"));
    theButtonControl->SetPluginDir (theName, fileNameInDataDir ("CSF_OCCTDataPath", "occ"));
  }
  else if (theName.IsEqual ("TKVInspector"))
  {
    theCommunicator->OpenFile (theName, fileNameInDataDir ("CSF_OCCTDataPath", "occ/face1.brep"));
    theCommunicator->OpenFile (theName, fileNameInDataDir ("CSF_OCCTDataPath", "occ/face2.brep"));
    theButtonControl->SetPluginDir (theName, fileNameInDataDir ("CSF_OCCTDataPath", "occ"));
  }
}

// =======================================================================
// function : main
// purpose :
// =======================================================================
int main (int argc, char** argv)
{
  OSD_Environment anEnvironment ("QTDIR");
  TCollection_AsciiString aPlugindsDirName = anEnvironment.Value();
  if (aPlugindsDirName.IsEmpty())
  {
    std::cout << "QTDIR is not defined" << std::endl;
    return 0;
  }
  aPlugindsDirName += "/plugins";
  QApplication::addLibraryPath (aPlugindsDirName.ToCString());
  QApplication anApp (argc, argv);

  std::set<TCollection_AsciiString> aPlugins;
  for (int anArgId = 1; anArgId < argc; anArgId++ )
  {
    if (!strcmp (argv[anArgId], "dfbrowser"))
      aPlugins.insert ("TKDFBrowser");

    if (!strcmp (argv[anArgId], "shapeview"))
      aPlugins.insert ("TKShapeView");

    if (!strcmp (argv[anArgId], "vinspector"))
      aPlugins.insert ("TKVInspector");
  }
  NCollection_List<Handle(Standard_Transient)> aParameters;

  // Create tool communicator
  TInspector_Communicator* aCommunicator = TInspectorEXE_OpenFileDialog::Communicator();
  if (!aCommunicator)
  {
    std::cout << "Communicator can not be created" << std::endl;
    return 0;
  }

  TInspectorEXE_OpenButton* aButtonControl = new TInspectorEXE_OpenButton (0);
  TCollection_AsciiString anActivatedPluginName;
  if (aPlugins.empty())
  {
    aPlugins.insert("TKDFBrowser");
    aPlugins.insert("TKShapeView");
    aPlugins.insert("TKVInspector");

    anActivatedPluginName = "TKDFBrowser";
  }
  else
    anActivatedPluginName = *aPlugins.rbegin();

  for (std::set<TCollection_AsciiString>::const_iterator aPluginIt = aPlugins.begin(); aPluginIt != aPlugins.end(); aPluginIt++)
  {
    TCollection_AsciiString aName = *aPluginIt;
    aCommunicator->RegisterPlugin (aName);
    aCommunicator->Init (aName, aParameters);
    setPluginSampleDirectory (aName, aCommunicator, aButtonControl);
  }
  aCommunicator->Activate (anActivatedPluginName);

  aCommunicator->SetVisible (true);
  aCommunicator->SetOpenButton (aButtonControl->StartButton());

  return anApp.exec();
}
