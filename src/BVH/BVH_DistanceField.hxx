// Created on: 2014-09-06
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

#ifndef _BVH_DistanceField_Header
#define _BVH_DistanceField_Header

#include <BVH_Geometry.hxx>

template <class T, int N>
class BVH_ParallelDistanceFieldBuilder;

//! Tool object for building 3D distance field from the set of BVH triangulations.
//! Distance field is a scalar field that measures the distance from a given point
//! to some object, including optional information about the inside and outside of
//! the structure. Distance fields are used as alternative surface representations
//! (like polygons or NURBS).
template <class T, int N>
class BVH_DistanceField
{
  friend class BVH_ParallelDistanceFieldBuilder<T, N>;

public:
  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:
  //! Creates empty 3D distance field.
  BVH_DistanceField(const Standard_Integer theMaximumSize, const Standard_Boolean theComputeSign);

  //! Releases resources of 3D distance field.
  virtual ~BVH_DistanceField();

  //! Builds 3D distance field from BVH geometry.
  Standard_Boolean Build(BVH_Geometry<T, N>& theGeometry);

  //! Returns parallel flag.
  inline Standard_Boolean IsParallel() const { return myIsParallel; }

  //! Set parallel flag controlling possibility of parallel execution.
  inline void SetParallel(const Standard_Boolean isParallel) { myIsParallel = isParallel; }

public:
  //! Returns packed voxel data.
  const T* PackedData() const { return myVoxelData; }

  //! Returns distance value for the given voxel.
  T& Voxel(const Standard_Integer theX, const Standard_Integer theY, const Standard_Integer theZ)
  {
    return myVoxelData[theX + (theY + theZ * myDimensionY) * myDimensionX];
  }

  //! Returns distance value for the given voxel.
  T Voxel(const Standard_Integer theX,
          const Standard_Integer theY,
          const Standard_Integer theZ) const
  {
    return myVoxelData[theX + (theY + theZ * myDimensionY) * myDimensionX];
  }

  //! Returns size of voxel grid in X dimension.
  Standard_Integer DimensionX() const { return myDimensionX; }

  //! Returns size of voxel grid in Y dimension.
  Standard_Integer DimensionY() const { return myDimensionY; }

  //! Returns size of voxel grid in Z dimension.
  Standard_Integer DimensionZ() const { return myDimensionZ; }

  //! Returns size of single voxel.
  const BVH_VecNt& VoxelSize() const { return myVoxelSize; }

  //! Returns minimum corner of voxel grid.
  const BVH_VecNt& CornerMin() const { return myCornerMin; }

  //! Returns maximum corner of voxel grid.
  const BVH_VecNt& CornerMax() const { return myCornerMax; }

protected:
  //! Performs building of distance field for the given Z slices.
  void BuildSlices(BVH_Geometry<T, N>&    theGeometry,
                   const Standard_Integer theStartZ,
                   const Standard_Integer theFinalZ);

protected:
  //! Array of voxels.
  T* myVoxelData;

  //! Size of single voxel.
  BVH_VecNt myVoxelSize;

  //! Minimum corner of voxel grid.
  BVH_VecNt myCornerMin;

  //! Maximum corner of voxel grid.
  BVH_VecNt myCornerMax;

  //! Size of voxel grid in X dimension.
  Standard_Integer myDimensionX;

  //! Size of voxel grid in Y dimension.
  Standard_Integer myDimensionY;

  //! Size of voxel grid in Z dimension.
  Standard_Integer myDimensionZ;

  //! Size of voxel grid in maximum dimension.
  Standard_Integer myMaximumSize;

  //! Enables/disables signing of distance field.
  Standard_Boolean myComputeSign;

  Standard_Boolean myIsParallel;
};

#include <BVH_DistanceField.lxx>

#endif // _BVH_DistanceField_Header
