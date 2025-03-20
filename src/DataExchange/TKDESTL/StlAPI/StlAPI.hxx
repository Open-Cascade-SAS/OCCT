// Created on: 1997-05-13
// Created by: Fabien REUTER
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StlAPI_HeaderFile
#define _StlAPI_HeaderFile

#include <Poly_Triangulation.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_CString.hxx>
class TopoDS_Shape;

//! Offers the API for STL data manipulation.
class StlAPI
{
public:
  DEFINE_STANDARD_ALLOC

  //! Convert and write shape to STL format.
  //! File is written in binary if aAsciiMode is False otherwise it is written in Ascii (by
  //! default).
  Standard_EXPORT static Standard_Boolean Write(
    const TopoDS_Shape&    theShape,
    const Standard_CString theFile,
    const Standard_Boolean theAsciiMode = Standard_True);

  //! Legacy interface.
  //! Read STL file and create a shape composed of triangular faces, one per facet.
  //! This approach is very inefficient, especially for large files.
  //! Consider reading STL file to Poly_Triangulation object instead (see class RWStl).
  Standard_DEPRECATED("This method is very inefficient; see RWStl class for better alternative")
  Standard_EXPORT static Standard_Boolean Read(TopoDS_Shape&          theShape,
                                               const Standard_CString aFile);
};

#endif // _StlAPI_HeaderFile
