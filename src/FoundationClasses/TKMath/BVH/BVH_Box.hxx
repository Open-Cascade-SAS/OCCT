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

#ifndef BVH_Box_HeaderFile
#define BVH_Box_HeaderFile

#include <BVH_Constants.hxx>
#include <BVH_Types.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Dump.hxx>
#include <Standard_ShortReal.hxx>

#include <limits>

//! Base class for BVH_Box (CRTP idiom is used).
//! @tparam T             Numeric data type
//! @tparam N             Vector dimension
//! @tparam TheDerivedBox Template of derived class that defined axis aligned bounding box.
template <class T, int N, template <class /*T*/, int /*N*/> class TheDerivedBox>
class BVH_BaseBox
{
};

// forward declaration
template <class T, int N>
class BVH_Box;

//! Partial template specialization for BVH_Box when N = 3.
template <class T>
class BVH_BaseBox<T, 3, BVH_Box>
{
public:
  //! Transforms this box with given transformation.
  void Transform(const NCollection_Mat4<T>& theTransform)
  {
    if (theTransform.IsIdentity())
    {
      return;
    }

    BVH_Box<T, 3>* aThis = static_cast<BVH_Box<T, 3>*>(this);
    if (!aThis->IsValid())
    {
      return;
    }

    BVH_Box<T, 3> aBox = Transformed(theTransform);

    aThis->CornerMin() = aBox.CornerMin();
    aThis->CornerMax() = aBox.CornerMax();
  }

  //! Returns a box which is the result of applying the
  //! given transformation to this box.
  BVH_Box<T, 3> Transformed(const NCollection_Mat4<T>& theTransform) const
  {
    using BVH_VecNt = typename BVH_Box<T, 3>::BVH_VecNt;

    const BVH_Box<T, 3>* aThis = static_cast<const BVH_Box<T, 3>*>(this);
    if (theTransform.IsIdentity())
    {
      return *aThis;
    }

    if (!aThis->IsValid())
    {
      return *aThis;
    }

    // Untransformed AABB min and max points
    const BVH_VecNt& anOldMinPnt = aThis->CornerMin();
    const BVH_VecNt& anOldMaxPnt = aThis->CornerMax();

    // Define an empty AABB located in the transformation translation point
    const typename BVH::VectorType<T, 4>::Type aTranslation = theTransform.GetColumn(3);
    BVH_VecNt aNewMinPnt = BVH_VecNt(aTranslation.x(), aTranslation.y(), aTranslation.z());
    BVH_VecNt aNewMaxPnt = BVH_VecNt(aTranslation.x(), aTranslation.y(), aTranslation.z());

    // This implements James Arvo's algorithm for transforming an axis-aligned bounding box (AABB)
    // under an affine transformation. For each row of the transformation matrix, we compute
    // the products of the min and max coordinates with the matrix elements, and select the
    // minimum and maximum values to form the new bounding box. This ensures that the transformed
    // box tightly encloses the original box after transformation, accounting for rotation and
    // scaling.
    for (int aCol = 0; aCol < 3; ++aCol)
    {
      for (int aRow = 0; aRow < 3; ++aRow)
      {
        const T aMatValue = theTransform.GetValue(aRow, aCol);
        const T anOffset1 = aMatValue * anOldMinPnt.GetData()[aCol];
        const T anOffset2 = aMatValue * anOldMaxPnt.GetData()[aCol];

        aNewMinPnt.ChangeData()[aRow] += (std::min)(anOffset1, anOffset2);
        aNewMaxPnt.ChangeData()[aRow] += (std::max)(anOffset1, anOffset2);
      }
    }

    return BVH_Box<T, 3>(aNewMinPnt, aNewMaxPnt);
  }
};

//! Defines axis aligned bounding box (AABB) based on BVH vectors.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template <class T, int N>
class BVH_Box : public BVH_BaseBox<T, N, BVH_Box>
{
public:
  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

private:
  //! Returns the minimum point sentinel value for invalid box.
  static constexpr T minSentinel() noexcept { return (std::numeric_limits<T>::max)(); }

  //! Returns the maximum point sentinel value for invalid box.
  static constexpr T maxSentinel() noexcept { return (std::numeric_limits<T>::lowest)(); }

public:
  //! Creates uninitialized bounding box.
  constexpr BVH_Box() noexcept
      : myMinPoint(BVH_VecNt(minSentinel())),
        myMaxPoint(BVH_VecNt(maxSentinel()))
  {
  }

  //! Creates bounding box of given point.
  constexpr BVH_Box(const BVH_VecNt& thePoint) noexcept
      : myMinPoint(thePoint),
        myMaxPoint(thePoint)
  {
  }

  //! Creates bounding box from corner points.
  constexpr BVH_Box(const BVH_VecNt& theMinPoint, const BVH_VecNt& theMaxPoint) noexcept
      : myMinPoint(theMinPoint),
        myMaxPoint(theMaxPoint)
  {
  }

public:
  //! Clears bounding box.
  constexpr void Clear() noexcept
  {
    myMinPoint = BVH_VecNt(minSentinel());
    myMaxPoint = BVH_VecNt(maxSentinel());
  }

  //! Is bounding box valid?
  constexpr Standard_Boolean IsValid() const noexcept { return myMinPoint[0] <= myMaxPoint[0]; }

  //! Appends new point to the bounding box.
  void Add(const BVH_VecNt& thePoint);

  //! Combines bounding box with another one.
  void Combine(const BVH_Box& theBox);

  //! Returns minimum point of bounding box.
  constexpr const BVH_VecNt& CornerMin() const noexcept { return myMinPoint; }

  //! Returns maximum point of bounding box.
  constexpr const BVH_VecNt& CornerMax() const noexcept { return myMaxPoint; }

  //! Returns minimum point of bounding box.
  constexpr BVH_VecNt& CornerMin() noexcept { return myMinPoint; }

  //! Returns maximum point of bounding box.
  constexpr BVH_VecNt& CornerMax() noexcept { return myMaxPoint; }

  //! Returns surface area of bounding box.
  //! If the box is degenerated into line, returns the perimeter instead.
  T Area() const;

  //! Returns diagonal of bounding box.
  constexpr BVH_VecNt Size() const { return myMaxPoint - myMinPoint; }

  //! Returns center of bounding box.
  constexpr BVH_VecNt Center() const { return (myMinPoint + myMaxPoint) * static_cast<T>(0.5); }

  //! Returns center of bounding box along the given axis.
  inline T Center(const Standard_Integer theAxis) const;

  //! Dumps the content of me into the stream
  void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const
  {
    (void)theDepth;
    const Standard_Integer anIsValid = IsValid() ? 1 : 0;
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, anIsValid)

    constexpr int n = (N < 3) ? N : 3;
    if constexpr (n == 1)
    {
      OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMinPoint[0])
      OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxPoint[0])
    }
    else if constexpr (n == 2)
    {
      OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream, "MinPoint", n, myMinPoint[0], myMinPoint[1])
      OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream, "MaxPoint", n, myMaxPoint[0], myMaxPoint[1])
    }
    else if constexpr (n == 3)
    {
      OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream,
                                       "MinPoint",
                                       n,
                                       myMinPoint[0],
                                       myMinPoint[1],
                                       myMinPoint[2])
      OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream,
                                       "MaxPoint",
                                       n,
                                       myMaxPoint[0],
                                       myMaxPoint[1],
                                       myMaxPoint[2])
    }
  }

  //! Inits the content of me from the stream
  Standard_Boolean InitFromJson(const Standard_SStream& theSStream, Standard_Integer& theStreamPos)
  {
    Standard_Integer aPos = theStreamPos;

    Standard_Integer        anIsValid  = 0;
    TCollection_AsciiString aStreamStr = Standard_Dump::Text(theSStream);

    OCCT_INIT_FIELD_VALUE_INTEGER(aStreamStr, aPos, anIsValid);

    if (anIsValid == 0)
    {
      Clear(); // Set to invalid state using sentinel values
      theStreamPos = aPos;
      return Standard_True;
    }

    constexpr int n = (N < 3) ? N : 3;
    if constexpr (n == 1)
    {
      Standard_Real aMinValue, aMaxValue;
      OCCT_INIT_FIELD_VALUE_REAL(aStreamStr, aPos, aMinValue);
      OCCT_INIT_FIELD_VALUE_REAL(aStreamStr, aPos, aMaxValue);
      myMinPoint[0] = (T)aMinValue;
      myMaxPoint[0] = (T)aMaxValue;
    }
    else if constexpr (n == 2)
    {
      Standard_Real aValue1, aValue2;
      OCCT_INIT_VECTOR_CLASS(aStreamStr, "MinPoint", aPos, n, &aValue1, &aValue2);
      myMinPoint[0] = (T)aValue1;
      myMinPoint[1] = (T)aValue2;

      OCCT_INIT_VECTOR_CLASS(aStreamStr, "MaxPoint", aPos, n, &aValue1, &aValue2);
      myMaxPoint[0] = (T)aValue1;
      myMaxPoint[1] = (T)aValue2;
    }
    else if constexpr (n == 3)
    {
      Standard_Real aValue1, aValue2, aValue3;
      OCCT_INIT_VECTOR_CLASS(aStreamStr, "MinPoint", aPos, n, &aValue1, &aValue2, &aValue3);
      myMinPoint[0] = (T)aValue1;
      myMinPoint[1] = (T)aValue2;
      myMinPoint[2] = (T)aValue3;

      OCCT_INIT_VECTOR_CLASS(aStreamStr, "MaxPoint", aPos, n, &aValue1, &aValue2, &aValue3);
      myMaxPoint[0] = (T)aValue1;
      myMaxPoint[1] = (T)aValue2;
      myMaxPoint[2] = (T)aValue3;
    }

    // For N > 3, initialize remaining dimensions to unbounded range
    // so they don't affect intersection checks
    for (int i = n; i < N; ++i)
    {
      myMinPoint[i] = (std::numeric_limits<T>::lowest)();
      myMaxPoint[i] = (std::numeric_limits<T>::max)();
    }

    theStreamPos = aPos;
    return Standard_True;
  }

public:
  //! Checks if the Box is out of the other box.
  constexpr Standard_Boolean IsOut(const BVH_Box<T, N>& theOther) const
  {
    if (!theOther.IsValid())
      return Standard_True;

    return IsOut(theOther.myMinPoint, theOther.myMaxPoint);
  }

  //! Checks if the Box is out of the other box defined by two points.
  constexpr Standard_Boolean IsOut(const BVH_VecNt& theMinPoint, const BVH_VecNt& theMaxPoint) const
  {
    if (!IsValid())
      return Standard_True;

    for (int i = 0; i < N; ++i)
    {
      if (myMinPoint[i] > theMaxPoint[i] || myMaxPoint[i] < theMinPoint[i])
        return Standard_True;
    }
    return Standard_False;
  }

  //! Checks if the Box fully contains the other box.
  constexpr Standard_Boolean Contains(const BVH_Box<T, N>& theOther,
                                      Standard_Boolean&    hasOverlap) const
  {
    hasOverlap = Standard_False;
    if (!theOther.IsValid())
      return Standard_False;

    return Contains(theOther.myMinPoint, theOther.myMaxPoint, hasOverlap);
  }

  //! Checks if the Box is fully contains the other box.
  constexpr Standard_Boolean Contains(const BVH_VecNt&  theMinPoint,
                                      const BVH_VecNt&  theMaxPoint,
                                      Standard_Boolean& hasOverlap) const
  {
    hasOverlap = Standard_False;
    if (!IsValid())
      return Standard_False;

    Standard_Boolean isInside = Standard_True;
    for (int i = 0; i < N; ++i)
    {
      hasOverlap = (myMinPoint[i] <= theMaxPoint[i] && myMaxPoint[i] >= theMinPoint[i]);
      if (!hasOverlap)
        return Standard_False;
      isInside = isInside && (myMinPoint[i] <= theMinPoint[i] && myMaxPoint[i] >= theMaxPoint[i]);
    }
    return isInside;
  }

  //! Checks if the Point is out of the box.
  constexpr Standard_Boolean IsOut(const BVH_VecNt& thePoint) const
  {
    if (!IsValid())
      return Standard_True;

    for (int i = 0; i < N; ++i)
    {
      if (thePoint[i] < myMinPoint[i] || thePoint[i] > myMaxPoint[i])
        return Standard_True;
    }
    return Standard_False;
  }

protected:
  BVH_VecNt myMinPoint; //!< Minimum point of bounding box (max<T> when invalid)
  BVH_VecNt myMaxPoint; //!< Maximum point of bounding box (lowest<T> when invalid)
};

namespace BVH
{
//! Tool class for calculating box center along the given axis.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template <class T, int N>
struct CenterAxis
{
  //! Returns the center of the box along the specified axis using array access.
  static inline T Center(const BVH_Box<T, N>& theBox, const Standard_Integer theAxis)
  {
    return (theBox.CornerMin()[theAxis] + theBox.CornerMax()[theAxis]) * static_cast<T>(0.5);
  }
};

//! Tool class for calculating surface area of the box.
//! For N=1, computes length (degenerate case).
//! For N=2, computes area (or perimeter for degenerate boxes).
//! For N>=3, computes 3D surface area using X, Y, Z components only.
//! The W component (4th dimension) is intentionally ignored as BVH surface area
//! heuristic (SAH) operates in 3D geometric space regardless of additional dimensions.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template <class T, int N>
struct SurfaceCalculator
{
  static inline T Area(const typename BVH_Box<T, N>::BVH_VecNt& theSize)
  {
    if constexpr (N == 1)
    {
      // For 1D, return the length
      return std::abs(theSize[0]);
    }
    else if constexpr (N == 2)
    {
      const T anArea = std::abs(theSize.x() * theSize.y());
      if (anArea < std::numeric_limits<T>::epsilon())
      {
        return std::abs(theSize.x()) + std::abs(theSize.y());
      }
      return anArea;
    }
    else
    {
      // For N >= 3, compute standard 3D surface area.
      const T anArea = (std::abs(theSize.x() * theSize.y()) + std::abs(theSize.x() * theSize.z())
                        + std::abs(theSize.z() * theSize.y()))
                       * static_cast<T>(2.0);
      if (anArea < std::numeric_limits<T>::epsilon())
      {
        return std::abs(theSize.x()) + std::abs(theSize.y()) + std::abs(theSize.z());
      }
      return anArea;
    }
  }
};

//! Tool class for computing component-wise vector minimum and maximum.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template <class T, int N>
struct BoxMinMax
{
  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

  //! Computes component-wise minimum in-place.
  static inline void CwiseMin(BVH_VecNt& theVec1, const BVH_VecNt& theVec2)
  {
    for (int i = 0; i < N; ++i)
    {
      theVec1[i] = (std::min)(theVec1[i], theVec2[i]);
    }
  }

  //! Computes component-wise maximum in-place.
  static inline void CwiseMax(BVH_VecNt& theVec1, const BVH_VecNt& theVec2)
  {
    for (int i = 0; i < N; ++i)
    {
      theVec1[i] = (std::max)(theVec1[i], theVec2[i]);
    }
  }
};
} // namespace BVH

//=================================================================================================

template <class T, int N>
void BVH_Box<T, N>::Add(const BVH_VecNt& thePoint)
{
  BVH::BoxMinMax<T, N>::CwiseMin(myMinPoint, thePoint);
  BVH::BoxMinMax<T, N>::CwiseMax(myMaxPoint, thePoint);
}

//=================================================================================================

template <class T, int N>
void BVH_Box<T, N>::Combine(const BVH_Box& theBox)
{
  if (theBox.IsValid())
  {
    BVH::BoxMinMax<T, N>::CwiseMin(myMinPoint, theBox.myMinPoint);
    BVH::BoxMinMax<T, N>::CwiseMax(myMaxPoint, theBox.myMaxPoint);
  }
}

//=================================================================================================

template <class T, int N>
T BVH_Box<T, N>::Area() const
{
  return !IsValid() ? static_cast<T>(0.0)
                    : BVH::SurfaceCalculator<T, N>::Area(myMaxPoint - myMinPoint);
}

//=================================================================================================

template <class T, int N>
T BVH_Box<T, N>::Center(const Standard_Integer theAxis) const
{
  return BVH::CenterAxis<T, N>::Center(*this, theAxis);
}

#endif // _BVH_Box_Header
