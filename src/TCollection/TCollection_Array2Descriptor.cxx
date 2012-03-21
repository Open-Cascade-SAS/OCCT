// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
