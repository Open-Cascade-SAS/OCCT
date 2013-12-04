// Copyright (c) 1992-1999 Matra Datavision
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

//-Version: 

//  Version	Date         Purpose
//              01/04/93     Creation   

//-Language     C++

//-Declarations
#include <Standard_Integer.hxx>
#include <PColStd_HashAsciiString.ixx>
#include <Standard_RangeError.hxx>
#include <PCollection_HAsciiString.hxx>

//=======================================================================
// Function : Create 
//=======================================================================

PColStd_HashAsciiString::PColStd_HashAsciiString()
{}

//=======================================================================
// Function : HashCode
//=======================================================================

Standard_Integer PColStd_HashAsciiString::HashCode
  (const Handle(PCollection_HAsciiString) &MyKey,
   const Standard_Integer Upper) const
{
  Standard_Integer total=0;

  if ( Upper < 1 ){
    Standard_RangeError::
      Raise("Try to apply HasCode method with negative or null argument.");
  }
  for (Standard_Integer I = 1 ; I <= MyKey->Length() ; I++ ) {
    total = total + (Standard_Integer) (MyKey->Value(I));
  }
  total = ::HashCode( total , Upper ) ;

  return total;
}

//=======================================================================
// Function : Compare
//=======================================================================

Standard_Boolean PColStd_HashAsciiString::Compare
  (const Handle(PCollection_HAsciiString) &One,
   const Handle(PCollection_HAsciiString) &Two) const
{
  return (One->IsSameString(Two));
}
