// Created on: 2000-06-23
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

#ifndef _StlAPI_Writer_HeaderFile
#define _StlAPI_Writer_HeaderFile

#include <Poly_Triangulation.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>

class TopoDS_Shape;

//! This class creates and writes
//! STL files from Open CASCADE shapes. An STL file can be written to an existing STL file or to a new one.
class StlAPI_Writer
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a writer object with default parameters: ASCIIMode.
  Standard_EXPORT StlAPI_Writer();

  //! Returns the address to the flag defining the mode for writing the file.
  //! This address may be used to either read or change the flag.
  //! If the mode returns True (default value) the generated file is an ASCII file.
  //! If the mode returns False, the generated file is a binary file.
  Standard_Boolean& ASCIIMode() { return myASCIIMode; }

  //! Converts a given shape to STL format and writes it to file with a given filename.
  //! \return the error state.
  Standard_EXPORT Standard_Boolean Write (const TopoDS_Shape& theShape,
                                          const Standard_CString theFileName);

private:
  Standard_Boolean myASCIIMode;
};

#endif // _StlAPI_Writer_HeaderFile
