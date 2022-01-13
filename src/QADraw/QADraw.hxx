// Created on: 2002-02-01
// Created by: QA Admin
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _QADraw_HeaderFile
#define _QADraw_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>

//! Draw Harness plugin defining non-general commands specific to test cases.
class QADraw 
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void CommonCommands (Draw_Interpretor& theCommands);

  Standard_EXPORT static void AdditionalCommands (Draw_Interpretor& theCommands);

  Standard_EXPORT static void TutorialCommands (Draw_Interpretor& theCommands);

  //! Loads all QA Draw commands. Used for plugin.
  Standard_EXPORT static void Factory (Draw_Interpretor& theCommands);

};

#endif // _QADraw_HeaderFile
