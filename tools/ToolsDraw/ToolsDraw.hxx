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


#ifndef ToolsDraw_H
#define ToolsDraw_H

#include <Draw_Interpretor.hxx>
#include <Standard.hxx>

//! \class ToolsDraw
//! \brief Registers DRAW commands to connect to TInspector tools
class ToolsDraw
{
public:

  DEFINE_STANDARD_ALLOC

  //! Loads all Draw commands of  tools. Used for plugin.
  //! \param theDI Draw interpretor
  Standard_EXPORT static void Factory (Draw_Interpretor& theDI);

  //! Add all tools command in the Draw_Interpretor
  //! \param theCommands
  Standard_EXPORT static void Commands (Draw_Interpretor& theCommands);

private:
};

#endif //ToolsDraw_H
