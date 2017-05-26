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

#ifndef _BVH_Properties_Header
#define _BVH_Properties_Header

#include <BVH_Box.hxx>

#include <Standard_Macro.hxx>

//! Abstract properties of geometric object.
class BVH_Properties : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(BVH_Properties, Standard_Transient)
public:

  //! Releases resources of object properties.
  Standard_EXPORT virtual ~BVH_Properties() = 0;

};

//! Stores transform properties of geometric object.
template<class T, int N>
class BVH_Transform : public BVH_Properties
{
public:

  //! Type of transformation matrix.
  typedef typename BVH::MatrixType<T, N>::Type BVH_MatNt;

public:

  //! Creates new identity transformation.
  BVH_Transform() {}

  //! Creates new transformation with specified matrix.
  BVH_Transform (const BVH_MatNt& theTransform) : myTransform (theTransform) {}

  //! Releases resources of transformation properties.
  virtual ~BVH_Transform() {}

  //! Returns transformation matrix.
  const BVH_MatNt& Transform() const { return myTransform; }

  //! Sets new transformation matrix.
  void SetTransform (const BVH_MatNt& theTransform);

  //! Returns inversed transformation matrix.
  const BVH_MatNt& Inversed() const { return myTransformInversed; }

  //! Applies transformation matrix to bounding box.
  BVH_Box<T, N> Apply (const BVH_Box<T, N>& theBox) const;

protected:

  BVH_MatNt myTransform;         //!< Transformation matrix
  BVH_MatNt myTransformInversed; //!< Inversed transformation matrix

};

namespace BVH
{
  template<class T, int N> struct MatrixOp
  {
    // Not implemented
  };

  template<class T> struct MatrixOp<T, 4>
  {
    typedef typename BVH::MatrixType<T, 4>::Type BVH_Mat4t;

    static void Inverse (const BVH_Mat4t& theIn,
                         BVH_Mat4t&       theOut)
    {
      theIn.Inverted (theOut);
    }

    typedef typename BVH::VectorType<T, 4>::Type BVH_Vec4t;

    static BVH_Vec4t Multiply (const BVH_Mat4t& theMat,
                               const BVH_Vec4t& theVec)
    {
      BVH_Vec4t aOut = theMat * theVec;
      return aOut * static_cast<T> (1.0 / aOut.w());
    }
  };

  template<class T, int N>
  struct UnitVector
  {
    // Not implemented
  };

  template<class T>
  struct UnitVector<T, 2>
  {
    typedef typename BVH::VectorType<T, 2>::Type BVH_Vec2t;
    static BVH_Vec2t DX() { return BVH_Vec2t (static_cast<T> (1.0), static_cast<T> (0.0)); }
    static BVH_Vec2t DY() { return BVH_Vec2t (static_cast<T> (0.0), static_cast<T> (1.0)); }
    static BVH_Vec2t DZ() { return BVH_Vec2t (static_cast<T> (0.0), static_cast<T> (0.0)); }
  };

  template<class T>
  struct UnitVector<T, 3>
  {
    typedef typename BVH::VectorType<T, 3>::Type BVH_Vec3t;
    static BVH_Vec3t DX() { return BVH_Vec3t (static_cast<T> (1.0), static_cast<T> (0.0), static_cast<T> (0.0)); }
    static BVH_Vec3t DY() { return BVH_Vec3t (static_cast<T> (0.0), static_cast<T> (1.0), static_cast<T> (0.0)); }
    static BVH_Vec3t DZ() { return BVH_Vec3t (static_cast<T> (0.0), static_cast<T> (0.0), static_cast<T> (1.0)); }
  };

  template<class T>
  struct UnitVector<T, 4>
  {
    typedef typename BVH::VectorType<T, 4>::Type BVH_Vec4t;
    static BVH_Vec4t DX() { return BVH_Vec4t (static_cast<T> (1.0), static_cast<T> (0.0), static_cast<T> (0.0), static_cast<T> (0.0)); }
    static BVH_Vec4t DY() { return BVH_Vec4t (static_cast<T> (0.0), static_cast<T> (1.0), static_cast<T> (0.0), static_cast<T> (0.0)); }
    static BVH_Vec4t DZ() { return BVH_Vec4t (static_cast<T> (0.0), static_cast<T> (0.0), static_cast<T> (1.0), static_cast<T> (0.0)); }
  };
}

// =======================================================================
// function : SetTransform
// purpose  :
// =======================================================================
template<class T, int N>
void BVH_Transform<T, N>::SetTransform (const BVH_MatNt& theTransform)
{
  myTransform = theTransform;
  BVH::MatrixOp<T, N>::Inverse (myTransform, myTransformInversed);
}

// =======================================================================
// function : Apply
// purpose  :
// =======================================================================
template<class T, int N>
BVH_Box<T, N> BVH_Transform<T, N>::Apply (const BVH_Box<T, N>& theBox) const
{
  typename BVH_Box<T, N>::BVH_VecNt aSize = theBox.Size();

  BVH_Box<T, N> aBox;
  for (Standard_Integer aX = 0; aX <= 1; ++aX)
  {
    for (Standard_Integer aY = 0; aY <= 1; ++aY)
    {
      for (Standard_Integer aZ = 0; aZ <= 1; ++aZ)
      {
        typename BVH_Box<T, N>::BVH_VecNt aCorner = theBox.CornerMin() +
          BVH::UnitVector<T, N>::DX() * aSize * static_cast<T> (aX) +
          BVH::UnitVector<T, N>::DY() * aSize * static_cast<T> (aY) +
          BVH::UnitVector<T, N>::DZ() * aSize * static_cast<T> (aZ);

        aBox.Add (BVH::MatrixOp<T, N>::Multiply (myTransform, aCorner));
      }
    }
  }

  return aBox;
}

#endif // _BVH_Properties_Header
