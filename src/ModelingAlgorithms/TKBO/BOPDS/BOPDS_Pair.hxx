// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _BOPDS_Pair_HeaderFile
#define _BOPDS_Pair_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_HashUtils.hxx>

//! The class is to provide the pair of indices of interfering shapes.

class BOPDS_Pair
{
public:
  DEFINE_STANDARD_ALLOC

  BOPDS_Pair()
      : myIndex1(-1),
        myIndex2(-1)
  {
  }

  //
  BOPDS_Pair(const Standard_Integer theIndex1, const Standard_Integer theIndex2)
      : myIndex1(theIndex1),
        myIndex2(theIndex2)
  {
  }

  ~BOPDS_Pair() {}

  //
  //! Sets the indices
  void SetIndices(const Standard_Integer theIndex1, const Standard_Integer theIndex2)
  {
    myIndex1 = theIndex1;
    myIndex2 = theIndex2;
  }

  //
  //! Gets the indices
  void Indices(Standard_Integer& theIndex1, Standard_Integer& theIndex2) const
  {
    theIndex1 = myIndex1;
    theIndex2 = myIndex2;
  }

  //
  //! Operator less
  Standard_Boolean operator<(const BOPDS_Pair& theOther) const
  {
    return ((myIndex1 != theOther.myIndex1) ? (myIndex1 < theOther.myIndex1)
                                            : (myIndex2 < theOther.myIndex2));
  }

  //
  //! Returns true if the Pair is equal to <the theOther>
  Standard_Boolean IsEqual(const BOPDS_Pair& theOther) const
  {
    return (myIndex1 == theOther.myIndex1 && myIndex2 == theOther.myIndex2)
           || (myIndex1 == theOther.myIndex2 && myIndex2 == theOther.myIndex1);
  }

  bool operator==(const BOPDS_Pair& theOther) const { return IsEqual(theOther); }

protected:
  Standard_Integer myIndex1;
  Standard_Integer myIndex2;
};

namespace std
{
template <>
struct hash<BOPDS_Pair>
{
  size_t operator()(const BOPDS_Pair& thePair) const noexcept
  {
    // Combine two int values into a single hash value.
    int aCombination[2];
    thePair.Indices(aCombination[0], aCombination[1]);
    if (aCombination[0] > aCombination[1])
    {
      std::swap(aCombination[0], aCombination[1]);
    }
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};
} // namespace std

#endif // _BOPDS_Pair