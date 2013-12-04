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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Standard_ExtString.hxx>
#include <Standard_Type.hxx> 
#ifndef _Standard_OStream_HeaderFile
#include <Standard_OStream.hxx>
#endif

//============================================================================
//==== ShallowDump : Writes a ExtString value.
//============================================================================
Standard_EXPORT void ShallowDump (const Standard_ExtString Value, Standard_OStream& s)
{  
   s << hex << Value << " Standard_ExtString " << endl; 
}

const Handle_Standard_Type& Standard_ExtString_Type_() 
{
  static Handle_Standard_Type _aType = 
    new Standard_Type("Standard_ExtString",sizeof(Standard_ExtString),0,NULL);
  
  return _aType;
}

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
