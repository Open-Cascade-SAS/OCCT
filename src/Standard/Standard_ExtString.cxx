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

#include <Standard_ExtString.hxx>
#include <Standard_Type.hxx> 
#include <Standard_OStream.hxx>

Standard_Integer HashCode (const Standard_ExtString Value, 
			   const Standard_Integer Upper)
{
  // compute SDBM hash of an ext string
  Standard_Integer hash = 0;
  for (const Standard_ExtCharacter *c = Value; *c; c++)
  {
    /* hash = hash * 33 ^ c */
    hash = (*c) + (hash << 6) + (hash << 16) - hash;
  }

  return HashCode (hash, Upper);
}
