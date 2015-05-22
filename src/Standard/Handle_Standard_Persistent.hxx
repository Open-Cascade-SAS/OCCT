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

//============================================================================
// Title : Handle_Standard_Persistent.hxx 
// Role  : This file just include <Standard_Persistent.hxx>
//============================================================================

#ifndef _Handle_Standard_Persistent_HeaderFile
#define _Handle_Standard_Persistent_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <Standard_Persistent_proto.hxx>

DEFINE_STANDARD_HANDLE(Standard_Persistent, Standard_Transient)

Standard_EXPORT Standard_Integer HashCode(const Handle(Standard_Persistent)&, const Standard_Integer);

#endif
