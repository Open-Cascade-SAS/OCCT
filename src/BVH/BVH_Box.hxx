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

#include <BVH_Constants.hxx>
#include <BVH_Types.hxx>
#include <Standard_ShortReal.hxx>

#include <limits>

//! Defines axis aligned bounding box (AABB) based on BVH vectors.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template<class T, int N>
class BVH_Box
{
public:

  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:

  //! Creates uninitialized bounding box.
  BVH_Box() : myIsInited (Standard_False) {}

  //! Creates bounding box of given point.
  BVH_Box (const BVH_VecNt& thePoint)
  : myMinPoint (thePoint),
    myMaxPoint (thePoint),
    myIsInited (Standard_True) {}

  //! Creates copy of another bounding box.
  BVH_Box (const BVH_Box& theBox)
  : myMinPoint (theBox.myMinPoint),
    myMaxPoint (theBox.myMaxPoint),
    myIsInited (theBox.myIsInited) {}

  //! Creates bounding box from corner points.
  BVH_Box (const BVH_VecNt& theMinPoint,
           const BVH_VecNt& theMaxPoint)
  : myMinPoint (theMinPoint),
    myMaxPoint (theMaxPoint),
    myIsInited (Standard_True) {}

public:

  //! Clears bounding box.
  void Clear() { myIsInited = Standard_False; }

  //! Is bounding box valid?
  Standard_Boolean IsValid() const { return myIsInited; }

  //! Appends new point to the bounding box.
  void Add (const BVH_VecNt& thePoint)
  {
    if (!myIsInited)
    {
      myMinPoint = thePoint;
      myMaxPoint = thePoint;
      myIsInited = Standard_True;
    }
    else
    {
      myMinPoint = myMinPoint.cwiseMin (thePoint);
      myMaxPoint = myMaxPoint.cwiseMax (thePoint);
    }
  }

  //! Combines bounding box with another one.
  void Combine (const BVH_Box& theBox);

  //! Returns minimum point of bounding box.
  const BVH_VecNt& CornerMin() const { return myMinPoint; }

  //! Returns maximum point of bounding box.
  const BVH_VecNt& CornerMax() const { return myMaxPoint; }

  //! Returns minimum point of bounding box.
  BVH_VecNt& CornerMin() { return myMinPoint; }

  //! Returns maximum point of bounding box.
  BVH_VecNt& CornerMax() { return myMaxPoint; }

  //! Returns surface area of bounding box.
  //! If the box is degenerated into line, returns the perimeter instead.
  T Area() const;

  //! Returns diagonal of bounding box.
  BVH_VecNt Size() const { return myMaxPoint - myMinPoint; }

  //! Returns center of bounding box.
  BVH_VecNt Center() const { return (myMinPoint + myMaxPoint) * static_cast<T> (0.5); }

  //! Returns center of bounding box along the given axis.
  T Center (const Standard_Integer theAxis) const;

protected:

  BVH_VecNt        myMinPoint; //!< Minimum point of bounding box
  BVH_VecNt        myMaxPoint; //!< Maximum point of bounding box
  Standard_Boolean myIsInited; //!< Is bounding box initialized?

};

namespace BVH
{
  //! Tool class for calculating box center along the given axis.
  //! \tparam T Numeric data type
  //! \tparam N Vector dimension
  template<class T, int N>
  struct CenterAxis
  {
    // Not implemented
  };

  template<class T>
  struct CenterAxis<T, 2>
  {
    static T Center (const BVH_Box<T, 2>& theBox, const Standard_Integer theAxis)
    {
      if (theAxis == 0)
      {
        return (theBox.CornerMin().x() + theBox.CornerMax().x()) * static_cast<T> (0.5);
      }
      else if (theAxis == 1)
      {
        return (theBox.CornerMin().y() + theBox.CornerMax().y()) * static_cast<T> (0.5);
      }
      return static_cast<T> (0.0);
    }
  };

  template<class T>
  struct CenterAxis<T, 3>
  {
    static T Center (const BVH_Box<T, 3>& theBox, const Standard_Integer theAxis)
    {
      if (theAxis == 0)
      {
        return (theBox.CornerMin().x() + theBox.CornerMax().x()) * static_cast<T> (0.5);
      }
      else if (theAxis == 1)
      {
        return (theBox.CornerMin().y() + theBox.CornerMax().y()) * static_cast<T> (0.5);
      }
      else if (theAxis == 2)
      {
        return (theBox.CornerMin().z() + theBox.CornerMax().z()) * static_cast<T> (0.5);
      }
      return static_cast<T> (0.0);
    }
  };

  template<class T>
  struct CenterAxis<T, 4>
  {
    static T Center (const BVH_Box<T, 4>& theBox, const Standard_Integer theAxis)
    {
      if (theAxis == 0)
      {
        return (theBox.CornerMin().x() + theBox.CornerMax().x()) * static_cast<T> (0.5);
      }
      else if (theAxis == 1)
      {
        return (theBox.CornerMin().y() + theBox.CornerMax().y()) * static_cast<T> (0.5);
      }
      else if (theAxis == 2)
      {
        return (theBox.CornerMin().z() + theBox.CornerMax().z()) * static_cast<T> (0.5);
      }
      return static_cast<T> (0.0);
    }
  };

  //! Tool class for calculating surface area of the box.
  //! \tparam T Numeric data type
  //! \tparam N Vector dimension
  template<class T, int N>
  struct SurfaceCalculator
  {
    // Not implemented
  };

  template<class T>
  struct SurfaceCalculator<T, 2>
  {
    static T Area (const typename BVH_Box<T, 2>::BVH_VecNt& theSize)
    {
      const T anArea = theSize.x() * theSize.y();

      if (anArea < std::numeric_limits<T>::epsilon())
      {
        return theSize.x() + theSize.y();
      }

      return anArea;
    }
  };

  template<class T>
  struct SurfaceCalculator<T, 3>
  {
    static T Area (const typename BVH_Box<T, 3>::BVH_VecNt& theSize)
    {
      const T anArea = ( theSize.x() * theSize.y() +
                         theSize.x() * theSize.z() +
                         theSize.z() * theSize.y() ) * static_cast<T> (2.0);

      if (anArea < std::numeric_limits<T>::epsilon())
      {
        return theSize.x() +
               theSize.y() +
               theSize.z();
      }

      return anArea;
    }
  };

  template<class T>
  struct SurfaceCalculator<T, 4>
  {
    static T Area (const typename BVH_Box<T, 4>::BVH_VecNt& theSize)
    {
      const T anArea = ( theSize.x() * theSize.y() +
                         theSize.x() * theSize.z() +
                         theSize.z() * theSize.y() ) * static_cast<T> (2.0);

      if (anArea < std::numeric_limits<T>::epsilon())
      {
        return theSize.x() +
               theSize.y() +
               theSize.z();
      }

      return anArea;
    }
  };

  //! Tool class for calculate component-wise vector minimum
  //! and maximum (optimized version).
  //! \tparam T Numeric data type
  //! \tparam N Vector dimension
  template<class T, int N>
  struct BoxMinMax
  {
    typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

    static void CwiseMin (BVH_VecNt& theVec1, const BVH_VecNt& theVec2)
    {
      theVec1.x() = Min (theVec1.x(), theVec2.x());
      theVec1.y() = Min (theVec1.y(), theVec2.y());
      theVec1.z() = Min (theVec1.z(), theVec2.z());
    }

    static void CwiseMax (BVH_VecNt& theVec1, const BVH_VecNt& theVec2)
    {
      theVec1.x() = Max (theVec1.x(), theVec2.x());
      theVec1.y() = Max (theVec1.y(), theVec2.y());
      theVec1.z() = Max (theVec1.z(), theVec2.z());
    }
  };

  template<class T>
  struct BoxMinMax<T, 2>
  {
    typedef typename BVH::VectorType<T, 2>::Type BVH_VecNt;

    static void CwiseMin (BVH_VecNt& theVec1, const BVH_VecNt& theVec2)
    {
      theVec1.x() = Min (theVec1.x(), theVec2.x());
      theVec1.y() = Min (theVec1.y(), theVec2.y());
    }

    static void CwiseMax (BVH_VecNt& theVec1, const BVH_VecNt& theVec2)
    {
      theVec1.x() = Max (theVec1.x(), theVec2.x());
      theVec1.y() = Max (theVec1.y(), theVec2.y());
    }
  };
}

// =======================================================================
// function : Combine
// purpose  :
// =======================================================================
template<class T, int N>
void BVH_Box<T, N>::Combine (const BVH_Box& theBox)
{
  if (theBox.myIsInited)
  {
    if (!myIsInited)
    {
      myMinPoint = theBox.myMinPoint;
      myMaxPoint = theBox.myMaxPoint;
      myIsInited = Standard_True;
    }
    else
    {
      BVH::BoxMinMax<T, N>::CwiseMin (myMinPoint, theBox.myMinPoint);
      BVH::BoxMinMax<T, N>::CwiseMax (myMaxPoint, theBox.myMaxPoint);
    }
  }
}

// =======================================================================
// function : Area
// purpose  :
// =======================================================================
template<class T, int N>
T BVH_Box<T, N>::Area() const
{
  return !myIsInited ? static_cast<T> (0.0) : BVH::SurfaceCalculator<T, N>::Area (myMaxPoint - myMinPoint);
}

// =======================================================================
// function : Center
// purpose  :
// =======================================================================
template<class T, int N>
T BVH_Box<T, N>::Center (const Standard_Integer theAxis) const
{
  return BVH::CenterAxis<T, N>::Center (*this, theAxis);
}

#endif // _BVH_Box_Header
