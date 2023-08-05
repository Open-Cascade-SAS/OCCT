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

#ifndef MeshVS_TwoNodes_HeaderFile
#define MeshVS_TwoNodes_HeaderFile

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_HashUtils.hxx>

//! Structure containing two IDs (of nodes) for using as a key in a map
//! (as representation of a mesh link)
//!
struct MeshVS_TwoNodes
{
  Standard_Integer First, Second;

  MeshVS_TwoNodes (Standard_Integer aFirst=0, Standard_Integer aSecond=0) 
  : First(aFirst), Second(aSecond) {}

  bool operator==(const MeshVS_TwoNodes& theTwoNode) const
  {
    return ((First == theTwoNode.First) && (Second == theTwoNode.Second)) ||
      ((First == theTwoNode.Second) && (Second == theTwoNode.First));
  }
};

namespace std
{
  template<>
  struct hash<MeshVS_TwoNodes>
  {
    size_t operator()(const MeshVS_TwoNodes& theTwoNodes) const noexcept
    {
      // Combine two int values into a single hash value.
      int aCombination[2]{ theTwoNodes.First, theTwoNodes.Second };
      if (aCombination[0] > aCombination[1])
      {
        std::swap(aCombination[0], aCombination[1]);
      }
      return opencascade::hashBytes(aCombination, sizeof(aCombination));
    }
  };
}

#endif
