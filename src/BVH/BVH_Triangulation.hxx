// Created on: 2013-12-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _BVH_Triangulation_Header
#define _BVH_Triangulation_Header

#include <BVH_PrimitiveSet.hxx>

//! Triangulation as an example of primitive set.
template<class T, int N>
class BVH_Triangulation : public BVH_PrimitiveSet<T, N>
{
public:

  typedef typename BVHTools::VectorType<T, N>::Type BVH_VecNt;

public:

  //! Creates new triangulation.
  BVH_Triangulation();

  //! Releases resources of geometric object.
  virtual ~BVH_Triangulation();

public:

  //! Array of vertex coordinates.
  typename BVHTools::ArrayType<T, N>::Type Vertices;

  //! Array of indices of triangle indicies vertices.
  BVH_Array4i Elements;

public:

  //! Return total number of triangles.
  virtual Standard_Integer Size() const;

  //! Returns AABB of specified triangle.
  virtual BVH_Box<T, N> Box (const Standard_Integer theIndex) const;

  //! Returns centroid position in specified axis.
  virtual T Center (const Standard_Integer theIndex,
                    const Standard_Integer theAxis) const;

  //! Swaps indices of two specified triangles.
  virtual void Swap (const Standard_Integer theIndex1,
                     const Standard_Integer theIndex2);

};

#include <BVH_Triangulation.lxx>

#endif // _BVH_Triangulation_Header
