// Created on: 1998-05-06
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _SWDRAW_HeaderFile
#define _SWDRAW_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>
#include <Standard_CString.hxx>

//! Provides DRAW interface to the functionalities of Shape Healing
//! toolkit (SHAPEWORKS Delivery Unit).
//!
//! Classes prefixed with Shape* corresponds to the packages of
//! Shape Healing.
class SWDRAW
{
public:
  DEFINE_STANDARD_ALLOC

  //! Loads commands defined in SWDRAW
  Standard_EXPORT static void Init(Draw_Interpretor& theCommands);

  //! Returns the name of the DRAW group accumulating the
  //! commands from the classes prefixed with Shape*.
  //! Returns "Shape Healing".
  Standard_EXPORT static Standard_CString GroupName();
};

#endif // _SWDRAW_HeaderFile
