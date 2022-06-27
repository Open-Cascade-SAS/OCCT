// Created on: 2000-05-30
// Created by: Sergey MOZOKHIN
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

#ifndef _VrmlAPI_HeaderFile
#define _VrmlAPI_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
class TopoDS_Shape;


//! API for writing to VRML 1.0
class VrmlAPI 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! With help of this class user can change parameters of writing.
  //! Converts the shape aShape to VRML format of the passed version and writes it
  //! to the file identified by aFileName using default parameters.
  Standard_EXPORT static Standard_Boolean Write (const TopoDS_Shape& aShape, const Standard_CString aFileName, const Standard_Integer aVersion = 2);

};

#endif // _VrmlAPI_HeaderFile
