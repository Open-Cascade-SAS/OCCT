// Copyright (c) 2005-2026 OPEN CASCADE SAS
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

#ifndef NCollection_PackedMapAlgo_HeaderFile
#define NCollection_PackedMapAlgo_HeaderFile

#include <NCollection_PackedMap.hxx>

//! @brief Algorithms and utilities for NCollection_PackedMap operations.
//!
//! This namespace provides template functions for boolean operations
//! on packed maps: union, intersection, subtraction, and symmetric difference.
//! These algorithms are optimized for the packed bit representation.
namespace NCollection_PackedMapAlgo
{
//! Apply to theMap the boolean operation union (aka addition, fuse, merge, boolean OR)
//! with another (given) Map. The result contains the values that were previously
//! contained in theMap or contained in the given (operand) map.
//! @return True if content of theMap is changed
template <class IntType>
bool Unite(NCollection_PackedMap<IntType>&       theMap,
           const NCollection_PackedMap<IntType>& theOtherMap)
{
  if (theOtherMap.IsEmpty() || &theMap == &theOtherMap)
    return false;
  else if (theMap.IsEmpty())
  {
    theMap.Assign(theOtherMap);
    return true;
  }

  const int anOldExtent = theMap.Extent();
  for (typename NCollection_PackedMap<IntType>::Iterator anIter(theOtherMap); anIter.More();
       anIter.Next())
  {
    theMap.Add(anIter.Key());
  }
  return anOldExtent != theMap.Extent();
}

//! Sets theMap to be the result of union (aka addition, fuse, merge, boolean OR)
//! operation between two given Maps. The new Map contains the values that are
//! contained either in the first map or in the second map or in both.
//! All previous content of theMap is cleared. theMap (result of the boolean
//! operation) can also be passed as one of operands.
template <class IntType>
void Union(NCollection_PackedMap<IntType>&       theMap,
           const NCollection_PackedMap<IntType>& theLeftMap,
           const NCollection_PackedMap<IntType>& theRightMap)
{
  if (theLeftMap.IsEmpty())
    theMap.Assign(theRightMap);
  else if (theRightMap.IsEmpty())
    theMap.Assign(theLeftMap);
  else if (&theMap == &theLeftMap)
    Unite(theMap, theRightMap);
  else if (&theMap == &theRightMap)
    Unite(theMap, theLeftMap);
  else
  {
    theMap.Clear();
    for (typename NCollection_PackedMap<IntType>::Iterator anIter(theLeftMap); anIter.More();
         anIter.Next())
    {
      theMap.Add(anIter.Key());
    }
    for (typename NCollection_PackedMap<IntType>::Iterator anIter(theRightMap); anIter.More();
         anIter.Next())
    {
      theMap.Add(anIter.Key());
    }
  }
}

//! Apply to theMap the intersection operation (aka multiplication, common, boolean AND)
//! with another (given) Map. The result contains only the values that are contained in
//! both theMap and the given map.
//! @return True if content of theMap is changed
template <class IntType>
bool Intersect(NCollection_PackedMap<IntType>&       theMap,
               const NCollection_PackedMap<IntType>& theOtherMap)
{
  if (theMap.IsEmpty())
    return false;
  else if (theOtherMap.IsEmpty())
  {
    theMap.Clear();
    return true;
  }
  else if (&theMap == &theOtherMap)
    return false;

  const int                       anOldExtent = theMap.Extent();
  NCollection_PackedMap<IntType>  aCopy;
  for (typename NCollection_PackedMap<IntType>::Iterator anIter(theMap); anIter.More();
       anIter.Next())
  {
    if (theOtherMap.Contains(anIter.Key()))
    {
      aCopy.Add(anIter.Key());
    }
  }
  theMap = std::move(aCopy);
  return anOldExtent != theMap.Extent();
}

//! Sets theMap to be the result of intersection (aka multiplication, common, boolean AND)
//! operation between two given Maps. The new Map contains only the values that are
//! contained in both map operands. All previous content of theMap is cleared.
//! theMap (result of the boolean operation) can also be used as one of operands.
template <class IntType>
void Intersection(NCollection_PackedMap<IntType>&       theMap,
                  const NCollection_PackedMap<IntType>& theLeftMap,
                  const NCollection_PackedMap<IntType>& theRightMap)
{
  if (theLeftMap.IsEmpty() || theRightMap.IsEmpty())
    theMap.Clear();
  else if (&theMap == &theLeftMap)
    Intersect(theMap, theRightMap);
  else if (&theMap == &theRightMap)
    Intersect(theMap, theLeftMap);
  else
  {
    theMap.Clear();
    const NCollection_PackedMap<IntType>* aSmaller = &theLeftMap;
    const NCollection_PackedMap<IntType>* aLarger  = &theRightMap;
    if (theLeftMap.Extent() > theRightMap.Extent())
    {
      aSmaller = &theRightMap;
      aLarger  = &theLeftMap;
    }
    for (typename NCollection_PackedMap<IntType>::Iterator anIter(*aSmaller); anIter.More();
         anIter.Next())
    {
      if (aLarger->Contains(anIter.Key()))
      {
        theMap.Add(anIter.Key());
      }
    }
  }
}

//! Apply to theMap the subtraction (aka set-theoretic difference, relative complement,
//! exclude, cut, boolean NOT) operation with another (given) Map. The result contains
//! only the values that were previously contained in theMap and not contained in theOtherMap.
//! @return True if content of theMap is changed
template <class IntType>
bool Subtract(NCollection_PackedMap<IntType>&       theMap,
              const NCollection_PackedMap<IntType>& theOtherMap)
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

  if (theMap.IsEmpty() || theOtherMap.IsEmpty())
    return false;

  const int anOldExtent = theMap.Extent();
  for (typename NCollection_PackedMap<IntType>::Iterator anIter(theOtherMap); anIter.More();
       anIter.Next())
  {
    theMap.Remove(anIter.Key());
  }
  return anOldExtent != theMap.Extent();
}

//! Sets theMap to be the result of subtraction (aka set-theoretic difference, relative
//! complement, exclude, cut, boolean NOT) operation between two given Maps. The new Map
//! contains only the values that are contained in the first map operand and not contained
//! in the second one. All previous content of theMap is cleared.
template <class IntType>
void Subtraction(NCollection_PackedMap<IntType>&       theMap,
                 const NCollection_PackedMap<IntType>& theLeftMap,
                 const NCollection_PackedMap<IntType>& theRightMap)
{
  if (&theMap == &theLeftMap)
  {
    Subtract(theMap, theRightMap);
    return;
  }
  else if (&theMap == &theRightMap)
  {
    NCollection_PackedMap<IntType> aCopy;
    Subtraction(aCopy, theLeftMap, theRightMap);
    theMap = std::move(aCopy);
    return;
  }

  theMap.Assign(theLeftMap);
  Subtract(theMap, theRightMap);
}

//! Apply to theMap the symmetric difference (aka exclusive disjunction, boolean XOR)
//! operation with another (given) Map. The result contains the values that are contained
//! only in theMap or the operand map, but not in both.
//! @return True if content of theMap is changed
template <class IntType>
bool Differ(NCollection_PackedMap<IntType>&       theMap,
            const NCollection_PackedMap<IntType>& theOtherMap)
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

  if (theOtherMap.IsEmpty())
    return false;
  else if (theMap.IsEmpty())
  {
    theMap.Assign(theOtherMap);
    return true;
  }

  const int anOldExtent = theMap.Extent();
  for (typename NCollection_PackedMap<IntType>::Iterator anIter(theOtherMap); anIter.More();
       anIter.Next())
  {
    const IntType aKey = anIter.Key();
    if (theMap.Contains(aKey))
      theMap.Remove(aKey);
    else
      theMap.Add(aKey);
  }
  return anOldExtent != theMap.Extent();
}

//! Sets theMap to be the result of symmetric difference (aka exclusive disjunction,
//! boolean XOR) operation between two given Maps. The new Map contains the values that
//! are contained only in the first or the second operand maps but not in both.
//! All previous content of theMap is cleared. theMap (result of the boolean operation)
//! can also be used as one of operands.
template <class IntType>
void Difference(NCollection_PackedMap<IntType>&       theMap,
                const NCollection_PackedMap<IntType>& theLeftMap,
                const NCollection_PackedMap<IntType>& theRightMap)
{
  if (&theLeftMap == &theRightMap)
  {
    theMap.Clear();
    return;
  }
  else if (&theMap == &theLeftMap)
  {
    Differ(theMap, theRightMap);
    return;
  }
  else if (&theMap == &theRightMap)
  {
    Differ(theMap, theLeftMap);
    return;
  }

  theMap.Clear();
  for (typename NCollection_PackedMap<IntType>::Iterator anIter(theLeftMap); anIter.More();
       anIter.Next())
  {
    if (!theRightMap.Contains(anIter.Key()))
    {
      theMap.Add(anIter.Key());
    }
  }
  for (typename NCollection_PackedMap<IntType>::Iterator anIter(theRightMap); anIter.More();
       anIter.Next())
  {
    if (!theLeftMap.Contains(anIter.Key()))
    {
      theMap.Add(anIter.Key());
    }
  }
}

//! Returns True if theMap and theOtherMap have common elements.
template <class IntType>
bool HasIntersection(const NCollection_PackedMap<IntType>& theMap,
                     const NCollection_PackedMap<IntType>& theOtherMap)
{
  if (theMap.IsEmpty() || theOtherMap.IsEmpty())
    return false;

  if (&theMap == &theOtherMap)
    return true;

  const NCollection_PackedMap<IntType>* aSmaller = &theMap;
  const NCollection_PackedMap<IntType>* aLarger  = &theOtherMap;
  if (theMap.Extent() > theOtherMap.Extent())
  {
    aSmaller = &theOtherMap;
    aLarger  = &theMap;
  }

  for (typename NCollection_PackedMap<IntType>::Iterator anIter(*aSmaller); anIter.More();
       anIter.Next())
  {
    if (aLarger->Contains(anIter.Key()))
    {
      return true;
    }
  }

  return false;
}

//! Checks if theMap contains all keys of theOtherMap.
//! Returns True if theMap is a superset of theOtherMap (theOtherMap is a subset of theMap).
template <class IntType>
bool Contains(const NCollection_PackedMap<IntType>& theMap,
              const NCollection_PackedMap<IntType>& theOtherMap)
{
  if (&theMap == &theOtherMap || theOtherMap.IsEmpty())
  {
    return true;
  }
  else if (theMap.Extent() < theOtherMap.Extent())
  {
    return false;
  }

  for (typename NCollection_PackedMap<IntType>::Iterator anIter(theOtherMap); anIter.More();
       anIter.Next())
  {
    if (!theMap.Contains(anIter.Key()))
    {
      return false;
    }
  }

  return true;
}

//! Returns True if theMap is a subset of theOtherMap, i.e. all elements
//! contained in theMap are also contained in theOtherMap.
//! If theMap is empty, this method returns true for any operand map.
template <class IntType>
bool IsSubset(const NCollection_PackedMap<IntType>& theMap,
              const NCollection_PackedMap<IntType>& theOtherMap)
{
  return Contains(theOtherMap, theMap);
}

//! Checks if two maps contain exactly the same keys.
//! Returns True if theMap and theOtherMap are equal.
template <class IntType>
bool IsEqual(const NCollection_PackedMap<IntType>& theMap,
             const NCollection_PackedMap<IntType>& theOtherMap)
{
  if (&theMap == &theOtherMap)
  {
    return true;
  }
  return theMap.Extent() == theOtherMap.Extent() && Contains(theMap, theOtherMap);
}

} // namespace NCollection_PackedMapAlgo

#endif // NCollection_PackedMapAlgo_HeaderFile
