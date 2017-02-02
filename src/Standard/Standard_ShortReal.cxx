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

#include <Standard_ShortReal.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_NullValue.hxx>
#include <Standard_Stream.hxx>
#include <Standard_OStream.hxx>

// ------------------------------------------------------------------
// Hascode : Computes a hascoding value for a given ShortReal
// ------------------------------------------------------------------
Standard_Integer HashCode(const Standard_ShortReal me, const Standard_Integer Upper)
{
  if (Upper < 1){
     throw Standard_RangeError("Try to apply HashCode method with negative or null argument.");
  }
  union 
    {
    Standard_ShortReal R;
    Standard_Integer I;
    } U;
  U.R = me;
  return HashCode( U.I , Upper) ;
}
