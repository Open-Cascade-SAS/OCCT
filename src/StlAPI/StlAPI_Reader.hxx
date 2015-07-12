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

#ifndef _StlAPI_Reader_HeaderFile
#define _StlAPI_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
class TopoDS_Shape;


//! Reading from stereolithography format.
class StlAPI_Reader 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT StlAPI_Reader();
  
  Standard_EXPORT void Read (TopoDS_Shape& aShape, const Standard_CString aFileName);




protected:





private:





};







#endif // _StlAPI_Reader_HeaderFile
