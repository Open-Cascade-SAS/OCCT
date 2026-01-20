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
  BVH_DistanceField(const int theMaximumSize, const bool theComputeSign);

  //! Releases resources of 3D distance field.
  virtual ~BVH_DistanceField();

  //! Builds 3D distance field from BVH geometry.
  bool Build(BVH_Geometry<T, N>& theGeometry);

  //! Returns parallel flag.
  inline bool IsParallel() const { return myIsParallel; }

  //! Set parallel flag controlling possibility of parallel execution.
  inline void SetParallel(const bool isParallel) { myIsParallel = isParallel; }

public:
  //! Returns packed voxel data.
  const T* PackedData() const { return myVoxelData; }

  //! Returns distance value for the given voxel.
  T& Voxel(const int theX, const int theY, const int theZ)
  {
    return myVoxelData[theX + (theY + theZ * myDimensionY) * myDimensionX];
  }

  //! Returns distance value for the given voxel.
  T Voxel(const int theX,
          const int theY,
          const int theZ) const
  {
    return myVoxelData[theX + (theY + theZ * myDimensionY) * myDimensionX];
  }

  //! Returns size of voxel grid in X dimension.
  int DimensionX() const { return myDimensionX; }

  //! Returns size of voxel grid in Y dimension.
  int DimensionY() const { return myDimensionY; }

  //! Returns size of voxel grid in Z dimension.
  int DimensionZ() const { return myDimensionZ; }

  //! Returns size of single voxel.
  const BVH_VecNt& VoxelSize() const { return myVoxelSize; }

  //! Returns minimum corner of voxel grid.
  const BVH_VecNt& CornerMin() const { return myCornerMin; }

  //! Returns maximum corner of voxel grid.
  const BVH_VecNt& CornerMax() const { return myCornerMax; }

protected:
  //! Performs building of distance field for the given Z slices.
  void BuildSlices(BVH_Geometry<T, N>&    theGeometry,
                   const int theStartZ,
                   const int theFinalZ);

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
  int myDimensionX;

  //! Size of voxel grid in Y dimension.
  int myDimensionY;

  //! Size of voxel grid in Z dimension.
  int myDimensionZ;

  //! Size of voxel grid in maximum dimension.
  int myMaximumSize;

  //! Enables/disables signing of distance field.
  bool myComputeSign;

  bool myIsParallel;
};

#include <BVH_DistanceField.lxx>

#endif // _BVH_DistanceField_Header
