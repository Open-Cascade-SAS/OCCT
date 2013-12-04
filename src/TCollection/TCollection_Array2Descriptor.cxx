// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <TCollection_Array2Descriptor.ixx>

// ---------------------------------------------------------------------

TCollection_Array2Descriptor::TCollection_Array2Descriptor 
    (const Standard_Integer aLowerRow,     
     const Standard_Integer aUpperRow,
     const Standard_Integer aLowerCol,     
     const Standard_Integer aUpperCol,
     const Standard_Address anAddress
     )
{
  myLowerRow  = aLowerRow;
  myUpperRow  = aUpperRow;
  myLowerCol  = aLowerCol;
  myUpperCol  = aUpperCol;
  myAddress   = anAddress;
}

// ---------------------------------------------------------------------

Standard_Integer TCollection_Array2Descriptor::UpperRow() const 
{ 
  return myUpperRow;
} 

// ---------------------------------------------------------------------
Standard_Integer TCollection_Array2Descriptor::LowerRow() const 
{ 
  return myLowerRow;
} 

// ---------------------------------------------------------------------

Standard_Integer TCollection_Array2Descriptor::UpperCol() const 
{ 
  return myUpperCol;
} 

// ---------------------------------------------------------------------
Standard_Integer TCollection_Array2Descriptor::LowerCol() const 
{ 
  return myLowerCol;
} 

// ---------------------------------------------------------------------
Standard_Address TCollection_Array2Descriptor::Address() const 
{ 
  return myAddress;
} 
