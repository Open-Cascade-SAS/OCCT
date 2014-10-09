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

#ifndef _MeshVS_SymmetricPairHasher_HeaderFile
#define _MeshVS_SymmetricPairHasher_HeaderFile

#include <Standard_Type.hxx>

typedef std::pair<Standard_Integer, Standard_Integer> MeshVS_NodePair;

//! Provides symmetric hash methods pair of integers.
struct MeshVS_SymmetricPairHasher
{
  static Standard_Integer HashCode (const MeshVS_NodePair& thePair, const Standard_Integer theMaxCode)
  {
    return ((thePair.first + thePair.second) & 0x7FFFFFFF) % theMaxCode + 1;
  }

  static Standard_Boolean IsEqual (const MeshVS_NodePair& thePair1, const MeshVS_NodePair& thePair2)
  {
    return (thePair1.first == thePair2.first && thePair1.second == thePair2.second)
        || (thePair1.first == thePair2.second && thePair1.second == thePair2.first);
  }
};

#endif // _MeshVS_SymmetricPairHasher_HeaderFile