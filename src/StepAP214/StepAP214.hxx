// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepAP214_HeaderFile
#define _StepAP214_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class StepAP214_Protocol;

//! Complete AP214 CC1 , Revision 4
//! Upgrading from Revision 2 to Revision 4 : 26 Mar 1997
//! Splitting in sub-schemas : 5 Nov 1997
class StepAP214
{
public:
  DEFINE_STANDARD_ALLOC

  //! creates a Protocol
  Standard_EXPORT static Handle(StepAP214_Protocol) Protocol();
};

#endif // _StepAP214_HeaderFile
