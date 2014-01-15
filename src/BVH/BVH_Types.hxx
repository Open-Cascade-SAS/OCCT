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

#ifndef _BVH_Types_Header
#define _BVH_Types_Header

// Use this macro to switch between STL and OCCT vector types
#define _BVH_USE_STD_VECTOR_

#include <NCollection_Vec2.hxx>
#include <NCollection_Vec3.hxx>
#include <NCollection_Vec4.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_Mat4.hxx>
#include <Standard_Type.hxx>

#include <vector>

namespace BVHTools
{
  //! Allows to fast switching between Eigen and NCollection vectors.
  template<class T, int N> struct VectorType
  {
    // Not implemented
  };

  template<class T> struct VectorType<T, 1>
  {
    typedef T Type;
  };

  template<class T> struct VectorType<T, 2>
  {
    typedef NCollection_Vec2<T> Type;
  };

  template<class T> struct VectorType<T, 3>
  {
    typedef NCollection_Vec3<T> Type;
  };

  template<class T> struct VectorType<T, 4>
  {
    typedef NCollection_Vec4<T> Type;
  };

  template<class T, int N = 1> struct ArrayType
  {
  #ifndef _BVH_USE_STD_VECTOR_
    typedef NCollection_Vector<typename VectorType<T, N>::Type> Type;
  #else
    typedef std::vector<typename VectorType<T, N>::Type> Type;
  #endif
  };

  //! Allows to fast switching between Eigen and NCollection matrices.
  template<class T, int N> struct MatrixType
  {
    // Not implemented
  };

  template<class T> struct MatrixType<T, 4>
  {
    typedef NCollection_Mat4<T> Type;
  };
}

//! 2D vector of integers.
typedef BVHTools::VectorType<Standard_Integer, 2>::Type BVH_Vec2i;
//! 3D vector of integers.
typedef BVHTools::VectorType<Standard_Integer, 3>::Type BVH_Vec3i;
//! 4D vector of integers.
typedef BVHTools::VectorType<Standard_Integer, 4>::Type BVH_Vec4i;

//! Array of 2D vectors of integers.
typedef BVHTools::ArrayType<Standard_Integer, 2>::Type BVH_Array2i;
//! Array of 3D vectors of integers.
typedef BVHTools::ArrayType<Standard_Integer, 3>::Type BVH_Array3i;
//! Array of 4D vectors of integers.
typedef BVHTools::ArrayType<Standard_Integer, 4>::Type BVH_Array4i;

//! 2D vector of single precision reals.
typedef BVHTools::VectorType<Standard_ShortReal, 2>::Type BVH_Vec2f;
//! 3D vector of single precision reals.
typedef BVHTools::VectorType<Standard_ShortReal, 3>::Type BVH_Vec3f;
//! 4D vector of single precision reals.
typedef BVHTools::VectorType<Standard_ShortReal, 4>::Type BVH_Vec4f;

//! Array of 2D vectors of single precision reals.
typedef BVHTools::ArrayType<Standard_ShortReal, 2>::Type BVH_Array2f;
//! Array of 3D vectors of single precision reals.
typedef BVHTools::ArrayType<Standard_ShortReal, 3>::Type BVH_Array3f;
//! Array of 4D vectors of single precision reals.
typedef BVHTools::ArrayType<Standard_ShortReal, 4>::Type BVH_Array4f;

//! 2D vector of double precision reals.
typedef BVHTools::VectorType<Standard_Real, 2>::Type BVH_Vec2d;
//! 3D vector of double precision reals.
typedef BVHTools::VectorType<Standard_Real, 3>::Type BVH_Vec3d;
//! 4D vector of double precision reals.
typedef BVHTools::VectorType<Standard_Real, 4>::Type BVH_Vec4d;

//! Array of 2D vectors of double precision reals.
typedef BVHTools::ArrayType<Standard_Real, 2>::Type BVH_Array2d;
//! Array of 3D vectors of double precision reals.
typedef BVHTools::ArrayType<Standard_Real, 3>::Type BVH_Array3d;
//! Array of 4D vectors of double precision reals.
typedef BVHTools::ArrayType<Standard_Real, 4>::Type BVH_Array4d;

//! 4x4 matrix of single precision reals.
typedef BVHTools::MatrixType<Standard_ShortReal, 4>::Type BVH_Mat4f;

//! 4x4 matrix of double precision reals.
typedef BVHTools::MatrixType<Standard_Real, 4>::Type BVH_Mat4d;

#include <BVH_Types.lxx>

#endif // _BVH_Types_Header
