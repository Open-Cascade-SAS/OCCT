// Created on: 2013-12-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _BVH_Triangulation_Header
#define _BVH_Triangulation_Header

#include <BVH_PrimitiveSet.hxx>

//! Triangulation as an example of BVH primitive set.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template<class T, int N>
class BVH_Triangulation : public BVH_PrimitiveSet<T, N>
{
public:

  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:

  //! Creates empty triangulation.
  BVH_Triangulation();

  //! Releases resources of triangulation.
  virtual ~BVH_Triangulation();

public:

  //! Array of vertex coordinates.
  typename BVH::ArrayType<T, N>::Type Vertices;

  //! Array of indices of triangle vertices.
  BVH_Array4i Elements;

public:

  //! Returns total number of triangles.
  virtual Standard_Integer Size() const;

  //! Returns AABB of entire set of objects.
  using BVH_PrimitiveSet<T, N>::Box;

  //! Returns AABB of the given triangle.
  virtual BVH_Box<T, N> Box (const Standard_Integer theIndex) const;

  //! Returns centroid position along the given axis.
  virtual T Center (const Standard_Integer theIndex,
                    const Standard_Integer theAxis) const;

  //! Performs transposing the two given triangles in the set.
  virtual void Swap (const Standard_Integer theIndex1,
                     const Standard_Integer theIndex2);

};

#include <BVH_Triangulation.lxx>

#endif // _BVH_Triangulation_Header
