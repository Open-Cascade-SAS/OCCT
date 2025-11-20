// Created by: Olga Suryaninova
// Created on: 2019-11-25
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _BVH_Ray_Header
#define _BVH_Ray_Header

#include <BVH_Types.hxx>

//! Describes a ray based on BVH vectors.
template <class T, int N>
class BVH_Ray
{
public:
  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:
  BVH_VecNt Origin; //!< Ray origin point
  BVH_VecNt Direct; //!< Ray direction vector

public:
  //! Creates ray with given origin and direction.
  constexpr BVH_Ray(const BVH_VecNt& theOrigin, const BVH_VecNt& theDirect) noexcept
      : Origin(theOrigin),
        Direct(theDirect)
  {
  }

  //! Default constructor (creates invalid ray at origin).
  constexpr BVH_Ray() noexcept
      : Origin(BVH_VecNt()),
        Direct(BVH_VecNt())
  {
  }
};

#endif // _BVH_Ray_Header
