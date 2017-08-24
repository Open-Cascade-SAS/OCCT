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

#include <inspector/TInspector_Communicator.hxx>

#include <OSD_Environment.hxx>
#include <TCollection_AsciiString.hxx>
#include <inspector/TInspector_Window.hxx>

#include <QApplication>
#include <QDir>
#include <QMainWindow>
#include <QWidget>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TInspector_Communicator::TInspector_Communicator()
{
  if (!qApp)
  {
    static int argc = 1;
    static char* argv[] = { (char*)"", 0 };
#if QT_VERSION > 0x050000
  OSD_Environment anEnvironment ("QTDIR");
  TCollection_AsciiString aPlugindsDirName = anEnvironment.Value();
  aPlugindsDirName += "/plugins";
  QApplication::addLibraryPath (aPlugindsDirName.ToCString());
#endif
    new QApplication (argc, argv);
  }
  myWindow = new TInspector_Window();
}

// =======================================================================
// function : SetVisible
// purpose :
// =======================================================================
void TInspector_Communicator::SetVisible (const bool theVisible)
{
  myWindow->GetMainWindow()->setVisible (theVisible);
#ifndef _WIN32
  // window is not visualized on X11 patform under DRAW tool without the next row
  QApplication::processEvents();
#endif
}
