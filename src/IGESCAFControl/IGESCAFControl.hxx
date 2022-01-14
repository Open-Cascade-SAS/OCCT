// Created on: 2000-08-16
// Created by: Andrey BETENEV
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

#ifndef _IGESCAFControl_HeaderFile
#define _IGESCAFControl_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Quantity_Color;


//! Provides high-level API to translate IGES file
//! to and from DECAF document
class IGESCAFControl 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Provides a tool for writing IGES file
  //! Converts IGES color index to CASCADE color
  Standard_EXPORT static Quantity_Color DecodeColor (const Standard_Integer col);
  
  //! Tries to Convert CASCADE color to IGES color index
  //! If no corresponding color defined in IGES, returns 0
  Standard_EXPORT static Standard_Integer EncodeColor (const Quantity_Color& col);

};

#endif // _IGESCAFControl_HeaderFile
