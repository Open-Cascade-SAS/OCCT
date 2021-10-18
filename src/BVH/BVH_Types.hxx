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

#ifndef BVH_Types_HeaderFile
#define BVH_Types_HeaderFile

// Use this macro to switch between STL and OCCT vector types
#define _BVH_USE_STD_VECTOR_

#include <vector>

#include <Bnd_Box.hxx>
#include <NCollection_Mat4.hxx>
#include <NCollection_Vec2.hxx>
#include <NCollection_Vec3.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Type.hxx>

// GCC supports shrink function only in C++11 mode
#if defined(_BVH_USE_STD_VECTOR_) && defined(_MSC_VER) && !defined(__INTEL_COMPILER)
  #define _STD_VECTOR_SHRINK
#endif

namespace BVH
{
  //! Tool class for selecting appropriate vector type (Eigen or NCollection).
  //! \tparam T Numeric data type
  //! \tparam N Component number
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

  template<class T> Bnd_Box ToBndBox (const T& theMin, const T& theMax)
  {
    return Bnd_Box (gp_Pnt (theMin, 0., 0.), gp_Pnt (theMax, 0., 0.));
  }

  template<class T> Bnd_Box ToBndBox (const NCollection_Vec2<T>& theMin,
                                      const NCollection_Vec2<T>& theMax)
  {
    return Bnd_Box (gp_Pnt (theMin.x(), theMin.y(), 0.),
                    gp_Pnt (theMax.x(), theMax.y(), 0.));
  }

  template<class T> Bnd_Box ToBndBox (const NCollection_Vec3<T>& theMin,
                                      const NCollection_Vec3<T>& theMax)
  {
    return Bnd_Box (gp_Pnt (theMin.x(), theMin.y(), theMin.z()),
                    gp_Pnt (theMax.x(), theMax.y(), theMax.z()));
  }

  template<class T> Bnd_Box ToBndBox (const NCollection_Vec4<T>& theMin,
                                      const NCollection_Vec4<T>& theMax)
  {
    return Bnd_Box (gp_Pnt (theMin.x(), theMin.y(), theMin.z()),
                    gp_Pnt (theMax.x(), theMax.y(), theMax.z()));
  }

  template<class T> struct VectorType<T, 4>
  {
    typedef NCollection_Vec4<T> Type;
  };

  //! Tool class for selecting appropriate matrix type (Eigen or NCollection).
  //! \tparam T Numeric data type
  //! \tparam N Matrix dimension
  template<class T, int N> struct MatrixType
  {
    // Not implemented
  };

  template<class T> struct MatrixType<T, 4>
  {
    typedef NCollection_Mat4<T> Type;
  };

  //! Tool class for selecting type of array of vectors (STD or NCollection vector).
  //! \tparam T Numeric data type
  //! \tparam N Component number
  template<class T, int N = 1> struct ArrayType
  {
  #ifndef _BVH_USE_STD_VECTOR_
    typedef NCollection_Vector<typename VectorType<T, N>::Type> Type;
  #else
    typedef std::vector<typename VectorType<T, N>::Type> Type;
  #endif
  };
}

//! 2D vector of integers.
typedef BVH::VectorType<Standard_Integer, 2>::Type BVH_Vec2i;
//! 3D vector of integers.
typedef BVH::VectorType<Standard_Integer, 3>::Type BVH_Vec3i;
//! 4D vector of integers.
typedef BVH::VectorType<Standard_Integer, 4>::Type BVH_Vec4i;

//! Array of 2D vectors of integers.
typedef BVH::ArrayType<Standard_Integer, 2>::Type BVH_Array2i;
//! Array of 3D vectors of integers.
typedef BVH::ArrayType<Standard_Integer, 3>::Type BVH_Array3i;
//! Array of 4D vectors of integers.
typedef BVH::ArrayType<Standard_Integer, 4>::Type BVH_Array4i;

//! 2D vector of single precision reals.
typedef BVH::VectorType<Standard_ShortReal, 2>::Type BVH_Vec2f;
//! 3D vector of single precision reals.
typedef BVH::VectorType<Standard_ShortReal, 3>::Type BVH_Vec3f;
//! 4D vector of single precision reals.
typedef BVH::VectorType<Standard_ShortReal, 4>::Type BVH_Vec4f;

//! Array of 2D vectors of single precision reals.
typedef BVH::ArrayType<Standard_ShortReal, 2>::Type BVH_Array2f;
//! Array of 3D vectors of single precision reals.
typedef BVH::ArrayType<Standard_ShortReal, 3>::Type BVH_Array3f;
//! Array of 4D vectors of single precision reals.
typedef BVH::ArrayType<Standard_ShortReal, 4>::Type BVH_Array4f;

//! 2D vector of double precision reals.
typedef BVH::VectorType<Standard_Real, 2>::Type BVH_Vec2d;
//! 3D vector of double precision reals.
typedef BVH::VectorType<Standard_Real, 3>::Type BVH_Vec3d;
//! 4D vector of double precision reals.
typedef BVH::VectorType<Standard_Real, 4>::Type BVH_Vec4d;

//! Array of 2D vectors of double precision reals.
typedef BVH::ArrayType<Standard_Real, 2>::Type BVH_Array2d;
//! Array of 3D vectors of double precision reals.
typedef BVH::ArrayType<Standard_Real, 3>::Type BVH_Array3d;
//! Array of 4D vectors of double precision reals.
typedef BVH::ArrayType<Standard_Real, 4>::Type BVH_Array4d;

//! 4x4 matrix of single precision reals.
typedef BVH::MatrixType<Standard_ShortReal, 4>::Type BVH_Mat4f;

//! 4x4 matrix of double precision reals.
typedef BVH::MatrixType<Standard_Real, 4>::Type BVH_Mat4d;

namespace BVH
{
  //! Tool class for accessing specific vector component (by index).
  //! \tparam T Numeric data type
  //! \tparam N Component number
  template<class T, int N> struct VecComp
  {
    // Not implemented
  };

  template<class T> struct VecComp<T, 2>
  {
    typedef typename BVH::VectorType<T, 2>::Type BVH_Vec2t;

    static T Get (const BVH_Vec2t& theVec, const Standard_Integer theAxis)
    {
      return theAxis == 0 ? theVec.x() : theVec.y();
    }
  };

  template<class T> struct VecComp<T, 3>
  {
    typedef typename BVH::VectorType<T, 3>::Type BVH_Vec3t;

    static T Get (const BVH_Vec3t& theVec, const Standard_Integer theAxis)
    {
      return theAxis == 0 ? theVec.x() : ( theAxis == 1 ? theVec.y() : theVec.z() );
    }
  };

  template<class T> struct VecComp<T, 4>
  {
    typedef typename BVH::VectorType<T, 4>::Type BVH_Vec4t;

    static T Get (const BVH_Vec4t& theVec, const Standard_Integer theAxis)
    {
      return theAxis == 0 ? theVec.x() :
        (theAxis == 1 ? theVec.y() : ( theAxis == 2 ? theVec.z() : theVec.w() ));
    }
  };

  //! Tool class providing typical operations on the array. It allows
  //! for interoperability between STD vector and NCollection vector.
  //! \tparam T Numeric data type
  //! \tparam N Component number
  template<class T, int N = 1> struct Array
  {
    typedef typename BVH::ArrayType<T, N>::Type BVH_ArrayNt;

    //! Returns a const reference to the element with the given index.
    static inline const typename BVH::VectorType<T, N>::Type& Value (
        const BVH_ArrayNt& theArray, const Standard_Integer theIndex)
    {
#ifdef _BVH_USE_STD_VECTOR_
      return theArray[theIndex];
#else
      return theArray.Value (theIndex);
#endif
    }

    //! Returns a reference to the element with the given index.
    static inline typename BVH::VectorType<T, N>::Type& ChangeValue (
      BVH_ArrayNt& theArray, const Standard_Integer theIndex)
    {
#ifdef _BVH_USE_STD_VECTOR_
      return theArray[theIndex];
#else
      return theArray.ChangeValue (theIndex);
#endif
    }

    //! Adds the new element at the end of the array.
    static inline void Append (BVH_ArrayNt& theArray,
      const typename BVH::VectorType<T, N>::Type& theElement)
    {
#ifdef _BVH_USE_STD_VECTOR_
      theArray.push_back (theElement);
#else
      theArray.Append (theElement);
#endif
    }

    //! Returns the number of elements in the given array.
    static inline Standard_Integer Size (const BVH_ArrayNt& theArray)
    {
#ifdef _BVH_USE_STD_VECTOR_
      return static_cast<Standard_Integer> (theArray.size());
#else
      return static_cast<Standard_Integer> (theArray.Size());
#endif
    }

    //! Removes all elements from the given array.
    static inline void Clear (BVH_ArrayNt& theArray)
    {
#ifdef _BVH_USE_STD_VECTOR_
      theArray.clear();
#else
      theArray.Clear();
#endif
    }

    //! Requests that the array capacity be at least enough to
    //! contain given number of elements. This function has no
    //! effect in case of NCollection based array.
    static inline void Reserve (BVH_ArrayNt& theArray, const Standard_Integer theCount)
    {
#ifdef _BVH_USE_STD_VECTOR_
      if (Size (theArray) == theCount)
      {
#ifdef _STD_VECTOR_SHRINK

#if(defined(_MSC_VER) && (_MSC_VER < 1600))
        BVH_ArrayNt aTmpArray(theArray);
        theArray.swap(aTmpArray);
#else
        theArray.shrink_to_fit();
#endif

#endif
      }
      else
      {
        theArray.reserve (theCount);
      }
#else
      // do nothing
#endif
    }
  };

  template<class T>
  static inline Standard_Integer IntFloor (const T theValue)
  {
    const Standard_Integer aRes = static_cast<Standard_Integer> (theValue);

    return aRes - static_cast<Standard_Integer> (aRes > theValue);
  }
}

#endif // _BVH_Types_Header
