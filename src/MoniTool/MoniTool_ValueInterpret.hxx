// Created on: 2000-02-28
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef MoniTool_ValueInterpret_HeaderFile
#define MoniTool_ValueInterpret_HeaderFile

#include <TCollection_HAsciiString.hxx>

class MoniTool_TypedValue;
typedef Handle(TCollection_HAsciiString)  (*MoniTool_ValueInterpret) (const Handle(MoniTool_TypedValue)& typval,
								      const Handle(TCollection_HAsciiString)& val,
								      const Standard_Boolean native);

#endif
