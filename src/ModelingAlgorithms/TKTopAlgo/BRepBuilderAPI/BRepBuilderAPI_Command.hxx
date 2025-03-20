// Created on: 1993-07-21
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

#ifndef _BRepBuilderAPI_Command_HeaderFile
#define _BRepBuilderAPI_Command_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>

//! Root class for all commands in BRepBuilderAPI.
//!
//! Provides :
//!
//! * Managements of the notDone flag.
//!
//! * Catching of exceptions (not implemented).
//!
//! * Logging (not implemented).
class BRepBuilderAPI_Command
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT virtual ~BRepBuilderAPI_Command();

  Standard_EXPORT virtual Standard_Boolean IsDone() const;

  //! Raises NotDone if done is false.
  Standard_EXPORT void Check() const;

protected:
  //! Set done to False.
  Standard_EXPORT BRepBuilderAPI_Command();

  //! Set done to true.
  Standard_EXPORT void Done();

  //! Set done to false.
  Standard_EXPORT void NotDone();

private:
  Standard_Boolean myDone;
};

#endif // _BRepBuilderAPI_Command_HeaderFile
