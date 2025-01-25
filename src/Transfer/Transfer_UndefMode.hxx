// Created on: 1992-02-03
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Transfer_UndefMode_HeaderFile
#define _Transfer_UndefMode_HeaderFile

//! used on processing Undefined Entities (see TransferOutput)
enum Transfer_UndefMode
{
  Transfer_UndefIgnore,
  Transfer_UndefFailure,
  Transfer_UndefContent,
  Transfer_UndefUser
};

#endif // _Transfer_UndefMode_HeaderFile
