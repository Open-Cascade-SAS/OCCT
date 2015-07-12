// Created on: 1994-08-03
// Created by: Modeling
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _MeshTest_HeaderFile
#define _MeshTest_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>


//! Provides methods for testing the mesh algorithms.
class MeshTest 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines meshing commands
  Standard_EXPORT static void Commands (Draw_Interpretor& DI);
  
  //! Defines plugin commands
  Standard_EXPORT static void PluginCommands (Draw_Interpretor& DI);




protected:





private:





};







#endif // _MeshTest_HeaderFile
