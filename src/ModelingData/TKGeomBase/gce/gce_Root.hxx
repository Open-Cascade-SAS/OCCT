// Created on: 1992-09-29
// Created by: Remi GILET
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

#ifndef _gce_Root_HeaderFile
#define _gce_Root_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gce_ErrorType.hxx>
#include <Standard_Boolean.hxx>

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Provides common status services for all `gce` construction classes.
class gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns true if the construction is successful.
  //! @return true if status is `gce_Done`
  bool IsDone() const { return TheError == gce_Done; }

  //! Returns true if the construction has failed.
  //! @return true if status is not `gce_Done`
  bool IsError() const { return TheError != gce_Done; }

  //! Returns the status of the construction:
  //! -   gce_Done, if the construction is successful, or
  //! -   another value of the gce_ErrorType enumeration
  //! indicating why the construction failed.
  //! @return construction status
  gce_ErrorType Status() const { return TheError; }

protected:
  gce_ErrorType TheError;
};

#endif // _gce_Root_HeaderFile
