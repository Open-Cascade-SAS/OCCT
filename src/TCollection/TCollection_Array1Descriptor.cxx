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

#include <TCollection_Array1Descriptor.ixx>

// ---------------------------------------------------------------------

TCollection_Array1Descriptor::TCollection_Array1Descriptor 
    (const Standard_Integer Lower,     
     const Standard_Integer Upper,
     const Standard_Address anAddress
     )
{
  myLower  = Lower;
  myUpper  = Upper;
  myAddress= anAddress;
}

// ---------------------------------------------------------------------

Standard_Integer TCollection_Array1Descriptor::Upper() const 
{ 
  return myUpper;
} 

// ---------------------------------------------------------------------
Standard_Integer TCollection_Array1Descriptor::Lower() const 
{ 
  return myLower;
} 

// ---------------------------------------------------------------------
Standard_Address TCollection_Array1Descriptor::Address() const 
{ 
  return myAddress;
} 
