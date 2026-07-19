// Copyright (c) 2024 OPEN CASCADE SAS
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

#ifndef NCollection_MapAlgo_HeaderFile
#define NCollection_MapAlgo_HeaderFile

//! This namespace contains algorithms and utilities for managing NCollection_Map.
//!
//! The NCollection_MapAlgo namespace provides a set of template functions
//! that facilitate the manipulation and management of NCollection_Map instances.
//! These algorithms are designed to be efficient and easy to use, providing
//! common operations such as union, intersection, subtraction, and symmetric difference
//! on map collections.
namespace NCollection_MapAlgo
{
//! Sets this Map to be the result of union (aka addition, fuse, merge, boolean OR)
//! operation between two given Maps. The new Map contains the values that are
//! contained either in the first map or in the second map or in both.
//! All previous content of this Map is cleared. This map (result of the boolean
//! operation) can also be passed as one of operands.
template <class MapType>
void Union(MapType& theMap, const MapType& theLeftMap, const MapType& theRightMap)
{
  if (&theLeftMap == &theRightMap)
  {
    theMap.Assign(theLeftMap);
    return;
  }

  if (&theMap != &theLeftMap && &theMap != &theRightMap)
  {
    theMap.Clear();
  }

  if (&theMap != &theLeftMap)
  {
    for (typename MapType::Iterator anIter(theLeftMap); anIter.More(); anIter.Next())
    {
      theMap.Add(anIter.Key());
    }
  }
  if (&theMap != &theRightMap)
  {
    for (typename MapType::Iterator anIter(theRightMap); anIter.More(); anIter.Next())
    {
      theMap.Add(anIter.Key());
    }
  }
}

//! Apply to this Map the boolean operation union (aka addition, fuse, merge, boolean OR)
//! with another (given) Map. The result contains the values that were previously
//! contained in this map or contained in the given (operand) map. This algorithm is
//! similar to method Union(). Returns True if contents of this map is changed.
template <class MapType>
bool Unite(MapType& theMap, const MapType& theOtherMap)
{
  if (&theMap == &theOtherMap)
  {
    return false;
  }

  const int anOldExtent = theMap.Extent();
  Union<MapType>(theMap, theMap, theOtherMap);
  return anOldExtent != theMap.Extent();
}

//! Returns true if this and theMap have common elements.
template <class MapType>
bool HasIntersection(const MapType& theMap, const MapType& theOtherMap)
{
  const MapType* aMap1 = &theMap;
  const MapType* aMap2 = &theOtherMap;
  if (theOtherMap.Size() < theMap.Size())
  {
    aMap1 = &theOtherMap;
    aMap2 = &theMap;
  }

  for (typename MapType::Iterator aIt(*aMap1); aIt.More(); aIt.Next())
  {
    if (aMap2->Contains(aIt.Value()))
    {
      return true;
    }
  }

  return false;
}

//! Sets this Map to be the result of intersection (aka multiplication, common, boolean AND)
//! operation between two given Maps. The new Map contains only the values that are
//! contained in both map operands. All previous content of this Map is cleared.
//! This same map (result of the boolean operation) can also be used as one of operands.
template <class MapType>
void Intersection(MapType& theMap, const MapType& theLeftMap, const MapType& theRightMap)
{
  if (&theLeftMap == &theRightMap)
  {
    theMap.Assign(theLeftMap);
    return;
  }

  if (&theMap == &theLeftMap)
  {
    MapType aCopy(1, theMap.Allocator());
    theMap.Exchange(aCopy);
    Intersection<MapType>(theMap, aCopy, theRightMap);
    return;
  }
  else if (&theMap == &theRightMap)
  {
    MapType aCopy(1, theMap.Allocator());
    theMap.Exchange(aCopy);
    Intersection<MapType>(theMap, theLeftMap, aCopy);
    return;
  }

  theMap.Clear();
  if (theLeftMap.Extent() < theRightMap.Extent())
  {
    for (typename MapType::Iterator anIter(theLeftMap); anIter.More(); anIter.Next())
    {
      if (theRightMap.Contains(anIter.Key()))
      {
        theMap.Add(anIter.Key());
      }
    }
  }
  else
  {
    for (typename MapType::Iterator anIter(theRightMap); anIter.More(); anIter.Next())
    {
      if (theLeftMap.Contains(anIter.Key()))
      {
        theMap.Add(anIter.Key());
      }
    }
  }
}

//! Apply to this Map the intersection operation (aka multiplication, common, boolean AND)
//! with another (given) Map. The result contains only the values that are contained in
//! both this and the given maps. This algorithm is similar to method Intersection().
//! Returns True if contents of this map is changed.
template <class MapType>
bool Intersect(MapType& theMap, const MapType& theOtherMap)
{
  if (&theMap == &theOtherMap || theMap.IsEmpty())
  {
    return false;
  }

  const int anOldExtent = theMap.Extent();
  Intersection<MapType>(theMap, theMap, theOtherMap);
  return anOldExtent != theMap.Extent();
}

//! Apply to this Map the subtraction (aka set-theoretic difference, relative complement,
//! exclude, cut, boolean NOT) operation with another (given) Map. The result contains
//! only the values that were previously contained in this map and not contained in this map.
//! This algorithm is similar to method Subtract() with two operands. Returns True if
//! contents of this map is changed.
template <class MapType>
bool Subtract(MapType& theMap, const MapType& theOtherMap)
{
  if (&theMap == &theOtherMap)
  {
    if (theMap.IsEmpty())
    {
      return false;
    }

    theMap.Clear();
    return true;
  }

  const int anOldExtent = theMap.Extent();
  for (typename MapType::Iterator anIter(theOtherMap); anIter.More(); anIter.Next())
  {
    theMap.Remove(anIter.Key());
  }
  return anOldExtent != theMap.Extent();
}

//! Sets this Map to be the result of subtraction (aka set-theoretic difference, relative
//! complement, exclude, cut, boolean NOT) operation between two given Maps. The new Map
//! contains only the values that are contained in the first map operands and not contained
//! in the second one. All previous content of this Map is cleared.
template <class MapType>
void Subtraction(MapType& theMap, const MapType& theLeftMap, const MapType& theRightMap)
{
  if (&theMap == &theLeftMap)
  {
    Subtract<MapType>(theMap, theRightMap);
    return;
  }
  else if (&theMap == &theRightMap)
  {
    MapType aCopy(1, theMap.Allocator());
    theMap.Exchange(aCopy);
    Subtraction(theMap, theLeftMap, aCopy);
    return;
  }

  theMap.Assign(theLeftMap);
  Subtract<MapType>(theMap, theRightMap);
}

//! Sets this Map to be the result of symmetric difference (aka exclusive disjunction,
//! boolean XOR) operation between two given Maps. The new Map contains the values that
//! are contained only in the first or the second operand maps but not in both.
//! All previous content of this Map is cleared. This map (result of the boolean operation)
//! can also be used as one of operands.
template <class MapType>
void Difference(MapType& theMap, const MapType& theLeftMap, const MapType& theRightMap)
{
  if (&theLeftMap == &theRightMap)
  {
    theMap.Clear();
    return;
  }
  else if (&theMap == &theLeftMap)
  {
    MapType aCopy(1, theMap.Allocator());
    theMap.Exchange(aCopy);
    Difference<MapType>(theMap, aCopy, theRightMap);
    return;
  }
  else if (&theMap == &theRightMap)
  {
    MapType aCopy(1, theMap.Allocator());
    theMap.Exchange(aCopy);
    Difference<MapType>(theMap, theLeftMap, aCopy);
    return;
  }

  theMap.Clear();
  for (typename MapType::Iterator anIter(theLeftMap); anIter.More(); anIter.Next())
  {
    if (!theRightMap.Contains(anIter.Key()))
    {
      theMap.Add(anIter.Key());
    }
  }
  for (typename MapType::Iterator anIter(theRightMap); anIter.More(); anIter.Next())
  {
    if (!theLeftMap.Contains(anIter.Key()))
    {
      theMap.Add(anIter.Key());
    }
  }
}

//! Apply to this Map the symmetric difference (aka exclusive disjunction, boolean XOR)
//! operation with another (given) Map. The result contains the values that are contained
//! only in this or the operand map, but not in both. This algorithm is similar to method
//! Difference(). Returns True if contents of this map is changed.
template <class MapType>
bool Differ(MapType& theMap, const MapType& theOtherMap)
{
  if (&theMap == &theOtherMap)
  {
    if (theMap.IsEmpty())
    {
      return false;
    }
    theMap.Clear();
    return true;
  }

  const int anOldExtent = theMap.Extent();
  Difference<MapType>(theMap, theMap, theOtherMap);
  return anOldExtent != theMap.Extent();
}

//! Checks if this map contains all keys of another map.
//! This function checks if theMap contains all keys of theOtherMap. It first
//! checks if the two maps are the same instance or if theOtherMap is empty, in
//! which case it returns true. Then it compares the number of elements in both
//! maps. If theMap has fewer elements than theOtherMap, it returns false. Finally,
//! it iterates through all keys in theOtherMap and checks if each key is present
//! in theMap.
template <class MapType>
bool Contains(const MapType& theMap, const MapType& theOtherMap)
{
  if (&theMap == &theOtherMap || theOtherMap.IsEmpty())
  {
    return true;
  }
  else if (theMap.Extent() < theOtherMap.Extent())
  {
    return false;
  }

  for (typename MapType::Iterator anIter(theOtherMap); anIter.More(); anIter.Next())
  {
    if (!theMap.Contains(anIter.Key()))
    {
      return false;
    }
  }

  return true;
}

//! Checks if two maps contain exactly the same keys.
//! This function compares the keys of two maps and returns true if they contain
//! exactly the same keys. It first checks if the two maps are the same instance,
//! in which case it returns true. Then it compares the number of elements in both
//! maps. If they are equal, it calls the Contains function to check if all keys
//! in theOtherMap are present in theMap.
template <class MapType>
bool IsEqual(const MapType& theMap, const MapType& theOtherMap)
{
  if (&theMap == &theOtherMap)
  {
    return true;
  }
  return theMap.Extent() == theOtherMap.Extent() && Contains<MapType>(theMap, theOtherMap);
}
} // namespace NCollection_MapAlgo

#endif // NCollection_MapAlgo_HeaderFile
