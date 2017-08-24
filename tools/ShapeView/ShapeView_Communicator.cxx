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

#include <inspector/ShapeView_Communicator.hxx>

#include <inspector/ShapeView_Window.hxx>
#include <OSD_Directory.hxx>
#include <OSD_Environment.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>

#include <QApplication>
#include <QMainWindow>
#include <QDir>

// =======================================================================
// function :  CreateCommunicator
// purpose : Creates a communicator by the library loading
// =======================================================================
Standard_EXPORTEXTERNC TInspectorAPI_Communicator* CreateCommunicator()
{
  return new ShapeView_Communicator();
}

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
ShapeView_Communicator::ShapeView_Communicator()
: TInspectorAPI_Communicator(), myWindow (0)
{
  // main window creation
  TCollection_AsciiString aTmpDir;
#ifdef _WIN32
  OSD_Environment anEnvironment ("TEMP");
  aTmpDir = anEnvironment.Value();
  if (aTmpDir.IsEmpty() )
  {
    anEnvironment.SetName("TMP");
    aTmpDir = anEnvironment.Value();
    if (aTmpDir.IsEmpty())
      aTmpDir = "C:\\";
  }
  if (!aTmpDir.EndsWith ("\\"))
    aTmpDir += "\\";
  OSD_Path aTmpPath (aTmpDir);
  OSD_Directory aTmpDirectory;
#else
  OSD_Directory aTmpDirectory = OSD_Directory::BuildTemporary();
  OSD_Path aTmpPath;
  aTmpDirectory.Path (aTmpPath);
#endif
  aTmpPath.DownTrek ("ShapeView");
  aTmpDirectory.SetPath (aTmpPath);
  if (!aTmpDirectory.Exists())
    aTmpDirectory.Build (OSD_Protection());

  aTmpDirectory.Path (aTmpPath);
  TCollection_AsciiString aTmpDirectoryName;
  aTmpPath.SystemName(aTmpDirectoryName);
  myWindow = new ShapeView_Window (0, aTmpDirectoryName);
}

ShapeView_Communicator::~ShapeView_Communicator()
{
  myWindow->RemoveAllShapes(); // removes all cached shapes and files in temporary directory
  OSD_Path aPath (myWindow->GetTemporaryDirectory());
  OSD_Directory aTmpDirectory (aPath);
  // temporary directory can be removed only if is empty
  aTmpDirectory.Remove();
}

