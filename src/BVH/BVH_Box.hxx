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

#ifndef _BVH_Box_Header
#define _BVH_Box_Header

#include <BVH_Types.hxx>

//! Axis aligned bounding box (AABB).
template<class T, int N>
class BVH_Box
{
public:

  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:

  //! Creates uninitialized bounding box.
  BVH_Box() : myInitialized (Standard_False) {}

  //! Creates bounding box of given point.
  BVH_Box (const BVH_VecNt& thePoint)
  : myMinPoint    (thePoint),
    myMaxPoint    (thePoint),
    myInitialized (Standard_True) {}

  //! Creates copy of another bounding box.
  BVH_Box (const BVH_Box& theBox)
  : myMinPoint    (theBox.myMinPoint),
    myMaxPoint    (theBox.myMaxPoint),
    myInitialized (theBox.myInitialized) {}

  //! Creates bounding box from corner points.
  BVH_Box (const BVH_VecNt& theMinPoint,
           const BVH_VecNt& theMaxPoint)
  : myMinPoint    (theMinPoint),
    myMaxPoint    (theMaxPoint),
    myInitialized (Standard_True) {}

public:

  //! Clears bounding box.
  void Clear();

  //! Is bounding box valid?
  Standard_Boolean IsValid() const;

  //! Appends new point to the bounding box.
  void Add (const BVH_VecNt& thePoint);

  //! Combines bounding box with another one.
  void Combine (const BVH_Box& theVolume);

  //! Returns minimum point of bounding box.
  const BVH_VecNt& CornerMin() const;

  //! Returns maximum point of bounding box.
  const BVH_VecNt& CornerMax() const;

  //! Returns minimum point of bounding box.
  BVH_VecNt& CornerMin();

  //! Returns maximum point of bounding box.
  BVH_VecNt& CornerMax();

  //! Returns surface area of bounding box.
  T Area() const;

  //! Returns diagonal of bounding box.
  BVH_VecNt Size() const;

  //! Returns center of bounding box.
  BVH_VecNt Center() const;

protected:

  BVH_VecNt        myMinPoint;    //!< Minimum point of bounding box
  BVH_VecNt        myMaxPoint;    //!< Maximum point of bounding box
  Standard_Boolean myInitialized; //!< Is bounding box valid?

};

#include <BVH_Box.lxx>

#endif // _BVH_Box_Header
