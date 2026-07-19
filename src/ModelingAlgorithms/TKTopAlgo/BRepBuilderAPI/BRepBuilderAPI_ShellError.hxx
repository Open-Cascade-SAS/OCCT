// Created on: 1993-07-06
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepBuilderAPI_ShellError_HeaderFile
#define _BRepBuilderAPI_ShellError_HeaderFile

//! Indicates the outcome of the construction of a face, i.e.
//! whether it is successful or not, as explained below:
//! -   BRepBuilderAPI_ShellDone No error occurred.
//! The shell is correctly built.
//! -   BRepBuilderAPI_EmptyShell No initialization of
//! the algorithm: only an empty constructor was used.
//! -   BRepBuilderAPI_DisconnectedShell not yet used
//! -   BRepBuilderAPI_ShellParametersOutOfRange
//! The parameters given to limit the surface are out of its bounds.
enum BRepBuilderAPI_ShellError
{
  BRepBuilderAPI_ShellDone,
  BRepBuilderAPI_EmptyShell,
  BRepBuilderAPI_DisconnectedShell,
  BRepBuilderAPI_ShellParametersOutOfRange
};

#endif // _BRepBuilderAPI_ShellError_HeaderFile
