// Created on: 2008-05-26
// Created by: Ekaterina SMIRNOVA
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef BRepMesh_CircleInspector_HeaderFile
#define BRepMesh_CircleInspector_HeaderFile

#include <IMeshData_Types.hxx>
#include <BRepMesh_Circle.hxx>
#include <gp_XY.hxx>
#include <NCollection_CellFilter.hxx>

//! Auxiliary class to find circles shot by the given point.
class BRepMesh_CircleInspector
{
public:
  static constexpr int Dimension = 2;

  typedef gp_XY Point;
  typedef int   Target;

  static double Coord(int i, const Point& thePnt) { return thePnt.Coord(i + 1); }

  Point Shift(const Point& thePnt, double theTol) const
  {
    return Point(thePnt.X() + theTol, thePnt.Y() + theTol);
  }

  //! Constructor.
  //! @param theTolerance tolerance to be used for identification of shot circles.
  //! @param theReservedSize size to be reserved for vector of circles.
  //! @param theAllocator memory allocator to be used by internal collections.
  BRepMesh_CircleInspector(const double                                 theTolerance,
                           const int                                    theReservedSize,
                           const occ::handle<NCollection_IncAllocator>& theAllocator)
      : mySqTolerance(theTolerance * theTolerance),
        myResIndices(theAllocator),
        myCircles(theReservedSize, theAllocator)
  {
  }

  //! Adds the circle to vector of circles at the given position.
  //! @param theIndex position of circle in the vector.
  //! @param theCircle circle to be added.
  void Bind(const int theIndex, const BRepMesh_Circle& theCircle)
  {
    myCircles.SetValue(theIndex, theCircle);
  }

  //! Resutns vector of registered circles.
  const IMeshData::VectorOfCircle& Circles() const { return myCircles; }

  //! Returns circle with the given index.
  //! @param theIndex index of circle.
  //! @return circle with the given index.
  BRepMesh_Circle& Circle(const int theIndex) { return myCircles(theIndex); }

  //! Set reference point to be checked.
  //! @param thePoint bullet point.
  void SetPoint(const gp_XY& thePoint)
  {
    myResIndices.Clear();
    myPoint = thePoint;
  }

  //! Returns list of circles shot by the reference point.
  IMeshData::ListOfInteger& GetShotCircles() { return myResIndices; }

  //! Performs inspection of a circle with the given index.
  //! @param theTargetIndex index of a circle to be checked.
  //! @return status of the check.
  NCollection_CellFilter_Action Inspect(const int theTargetIndex)
  {
    BRepMesh_Circle& aCircle = myCircles(theTargetIndex);
    const double&    aRadius = aCircle.Radius();
    if (aRadius < 0.)
      return CellFilter_Purge;

    gp_XY& aLoc = const_cast<gp_XY&>(aCircle.Location());

    const double aDX = myPoint.ChangeCoord(1) - aLoc.ChangeCoord(1);
    const double aDY = myPoint.ChangeCoord(2) - aLoc.ChangeCoord(2);

    // This check is wrong. It is better to use
    //
    //   const double aR = aRadius + aToler;
    //   if ((aDX * aDX + aDY * aDY) <= aR * aR)
    //   {
    //     ...
    //   }

    // where aToler = sqrt(mySqTolerance). Taking into account the fact
    // that the input parameter of the class (see constructor) is linear
    //(not quadratic) tolerance there is no point in square root computation.
    // Simply, we do not need to compute square of the input tolerance and to
    // assign it to mySqTolerance. The input linear tolerance is needed to be used.

    // However, this change leads to hangs the test case "perf mesh bug27119".
    // So, this correction is better to be implemented in the future.

    if ((aDX * aDX + aDY * aDY) - (aRadius * aRadius) <= mySqTolerance)
      myResIndices.Append(theTargetIndex);

    return CellFilter_Keep;
  }

  //! Checks indices for equality.
  static bool IsEqual(const int theIndex, const int theTargetIndex)
  {
    return (theIndex == theTargetIndex);
  }

private:
  double                    mySqTolerance;
  IMeshData::ListOfInteger  myResIndices;
  IMeshData::VectorOfCircle myCircles;
  gp_XY                     myPoint;
};

#endif
