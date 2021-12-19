// Created on: 1995-03-13
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _XSControl_HeaderFile
#define _XSControl_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class XSControl_WorkSession;
class IFSelect_SessionPilot;
class XSControl_Vars;


//! This package provides complements to IFSelect & Co for
//! control of a session
class XSControl 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the WorkSession of a SessionPilot, but casts it as
  //! from XSControl : it then gives access to Control & Transfers
  Standard_EXPORT static Handle(XSControl_WorkSession) Session (const Handle(IFSelect_SessionPilot)& pilot);
  
  //! Returns the Vars of a SessionPilot, it is brought by Session
  //! it provides access to external variables
  Standard_EXPORT static Handle(XSControl_Vars) Vars (const Handle(IFSelect_SessionPilot)& pilot);

};

#endif // _XSControl_HeaderFile
