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

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StlAPI_ErrorStatus.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Shape;
class StlAPI_Writer;
class StlAPI_Reader;


//! Offers the API for STL data manipulation.
class StlAPI 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Convert and write shape to STL format.
  //! file is written in binary if aAsciiMode is False
  //! otherwise it is written in Ascii (by default)
  Standard_EXPORT static StlAPI_ErrorStatus Write (const TopoDS_Shape& aShape, const Standard_CString aFile, const Standard_Boolean aAsciiMode = Standard_True);
  
  //! Create a shape from a STL format.
  Standard_EXPORT static void Read (TopoDS_Shape& aShape, const Standard_CString aFile);




protected:





private:




friend class StlAPI_Writer;
friend class StlAPI_Reader;

};







#endif // _StlAPI_HeaderFile
