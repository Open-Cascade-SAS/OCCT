// Created on: 2003-05-06
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _Interface_MapAsciiStringHasher_HeaderFile
#define _Interface_MapAsciiStringHasher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class TCollection_AsciiString;



class Interface_MapAsciiStringHasher 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Integer HashCode (const TCollection_AsciiString& K, const Standard_Integer Upper);
  
  Standard_EXPORT static Standard_Boolean IsEqual (const TCollection_AsciiString& K1, const TCollection_AsciiString& K2);




protected:





private:





};







#endif // _Interface_MapAsciiStringHasher_HeaderFile
