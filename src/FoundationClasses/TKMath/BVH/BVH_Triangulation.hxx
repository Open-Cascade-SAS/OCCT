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

#ifndef BVH_Triangulation_HeaderFile
#define BVH_Triangulation_HeaderFile

#include <BVH_PrimitiveSet.hxx>

//! Triangulation as an example of BVH primitive set.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template <class T, int N>
class BVH_Triangulation : public BVH_PrimitiveSet<T, N>
{
public:
  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:
  //! Creates empty triangulation.
  BVH_Triangulation() {}

  //! Creates empty triangulation.
  BVH_Triangulation(const opencascade::handle<BVH_Builder<T, N>>& theBuilder)
      : BVH_PrimitiveSet<T, N>(theBuilder)
  {
    //
  }

  //! Releases resources of triangulation.
  virtual ~BVH_Triangulation() {}

public:
  //! Array of vertex coordinates.
  typename BVH::ArrayType<T, N>::Type Vertices;

  //! Array of indices of triangle vertices.
  BVH_Array4i Elements;

public:
  //! Returns total number of triangles.
  virtual Standard_Integer Size() const Standard_OVERRIDE
  {
    return BVH::Array<Standard_Integer, 4>::Size(Elements);
  }

  //! Returns AABB of entire set of objects.
  using BVH_PrimitiveSet<T, N>::Box;

  //! Returns AABB of the given triangle.
  virtual BVH_Box<T, N> Box(const Standard_Integer theIndex) const Standard_OVERRIDE
  {
    const BVH_Vec4i& anIndex = BVH::Array<Standard_Integer, 4>::Value(Elements, theIndex);

    const BVH_VecNt& aPoint0 = BVH::Array<T, N>::Value(Vertices, anIndex.x());
    const BVH_VecNt& aPoint1 = BVH::Array<T, N>::Value(Vertices, anIndex.y());
    const BVH_VecNt& aPoint2 = BVH::Array<T, N>::Value(Vertices, anIndex.z());

    BVH_VecNt aMinPoint(aPoint0), aMaxPoint(aPoint0);

    BVH::BoxMinMax<T, N>::CwiseMin(aMinPoint, aPoint1);
    BVH::BoxMinMax<T, N>::CwiseMin(aMinPoint, aPoint2);
    BVH::BoxMinMax<T, N>::CwiseMax(aMaxPoint, aPoint1);
    BVH::BoxMinMax<T, N>::CwiseMax(aMaxPoint, aPoint2);
    return BVH_Box<T, N>(aMinPoint, aMaxPoint);
  }

  //! Returns centroid position along the given axis.
  virtual T Center(const Standard_Integer theIndex,
                   const Standard_Integer theAxis) const Standard_OVERRIDE
  {
    const BVH_Vec4i& anIndex = BVH::Array<Standard_Integer, 4>::Value(Elements, theIndex);

    const BVH_VecNt& aPoint0 = BVH::Array<T, N>::Value(Vertices, anIndex.x());
    const BVH_VecNt& aPoint1 = BVH::Array<T, N>::Value(Vertices, anIndex.y());
    const BVH_VecNt& aPoint2 = BVH::Array<T, N>::Value(Vertices, anIndex.z());
    return (BVH::VecComp<T, N>::Get(aPoint0, theAxis) + BVH::VecComp<T, N>::Get(aPoint1, theAxis)
            + BVH::VecComp<T, N>::Get(aPoint2, theAxis))
           * static_cast<T>(1.0 / 3.0);
  }

  //! Performs transposing the two given triangles in the set.
  virtual void Swap(const Standard_Integer theIndex1,
                    const Standard_Integer theIndex2) Standard_OVERRIDE
  {
    BVH_Vec4i& anIndices1 = BVH::Array<Standard_Integer, 4>::ChangeValue(Elements, theIndex1);
    BVH_Vec4i& anIndices2 = BVH::Array<Standard_Integer, 4>::ChangeValue(Elements, theIndex2);
    std::swap(anIndices1, anIndices2);
  }
};

#endif // _BVH_Triangulation_Header
