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

#ifndef __TWO_NODES_STRUCT_HEADER__
#define __TWO_NODES_STRUCT_HEADER__

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

//! Structure containing two IDs (of nodes) for using as a key in a map
//! (as representation of a mesh link)
//!
struct MeshVS_TwoNodes
{
  Standard_Integer First, Second;

  MeshVS_TwoNodes (Standard_Integer aFirst=0, Standard_Integer aSecond=0) 
  : First(aFirst), Second(aSecond) {}
};

//================================================================
// Function : HashCode
// Purpose  :
//================================================================

inline Standard_Integer HashCode( const MeshVS_TwoNodes& obj,
                                  const Standard_Integer Upper )
{
  // symmetrical with respect to obj.First and obj.Second
  const Standard_Integer aKey = obj.First + obj.Second;
  return HashCode (aKey, Upper);
}

//================================================================
// Function : operator ==
// Purpose  :
//================================================================

inline Standard_Boolean operator==( const MeshVS_TwoNodes& obj1,
                                    const MeshVS_TwoNodes& obj2 )
{
  return ( ( obj1.First == obj2.First  ) && ( obj1.Second == obj2.Second ) ) ||
         ( ( obj1.First == obj2.Second ) && ( obj1.Second == obj2.First  ) );
}

#endif
