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
#include <Precision.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_CellFilter.hxx>

//! Auxiliary class to find circles shot by the given point.
class BRepMesh_CircleInspector : public NCollection_CellFilter_InspectorXY
{
public:
  typedef Standard_Integer Target;

  //! Constructor.
  //! @param theTolerance tolerance to be used for identification of shot circles.
  //! @param theReservedSize size to be reserved for vector of circles.
  //! @param theAllocator memory allocator to be used by internal collections.
  BRepMesh_CircleInspector(
    const Standard_Real                     theTolerance,
    const Standard_Integer                  theReservedSize,
    const Handle(NCollection_IncAllocator)& theAllocator)
  : mySqTolerance(theTolerance*theTolerance),
    myResIndices(theAllocator),
    myCircles(theReservedSize, theAllocator)
  {
  }

  //! Adds the circle to vector of circles at the given position.
  //! @param theIndex position of circle in the vector.
  //! @param theCircle circle to be added.
  void Bind(const Standard_Integer theIndex,
            const BRepMesh_Circle& theCircle)
  {
    myCircles.SetValue(theIndex, theCircle);
  }

  //! Resutns vector of registered circles.
  const IMeshData::VectorOfCircle& Circles() const
  {
    return myCircles; 
  }

  //! Returns circle with the given index.
  //! @param theIndex index of circle.
  //! @return circle with the given index.
  BRepMesh_Circle& Circle(const Standard_Integer theIndex)
  {
    return myCircles(theIndex);
  }

  //! Set reference point to be checked.
  //! @param thePoint bullet point.
  void SetPoint(const gp_XY& thePoint)
  {
    myResIndices.Clear();
    myPoint = thePoint;
  }

  //! Returns list of circles shot by the reference point.
  IMeshData::ListOfInteger& GetShotCircles()
  {
    return myResIndices;
  }

  //! Performs inspection of a circle with the given index.
  //! @param theTargetIndex index of a circle to be checked.
  //! @return status of the check.
  NCollection_CellFilter_Action Inspect(
    const Standard_Integer theTargetIndex)
  {
    BRepMesh_Circle& aCircle = myCircles(theTargetIndex);
    const Standard_Real& aRadius = aCircle.Radius();
    if (aRadius < 0.)
      return CellFilter_Purge;

    gp_XY& aLoc = const_cast<gp_XY&>(aCircle.Location());

    const Standard_Real aDX = myPoint.ChangeCoord(1) - aLoc.ChangeCoord(1);
    const Standard_Real aDY = myPoint.ChangeCoord(2) - aLoc.ChangeCoord(2);

    //This check is wrong. It is better to use 
    //  
    //  const Standard_Real aR = aRadius + aToler;
    //  if ((aDX * aDX + aDY * aDY) <= aR * aR)
    //  {
    //    ...
    //  }

    //where aToler = sqrt(mySqTolerance). Taking into account the fact
    //that the input parameter of the class (see constructor) is linear
    //(not quadratic) tolerance there is no point in square root computation.
    //Simply, we do not need to compute square of the input tolerance and to
    //assign it to mySqTolerance. The input linear tolerance is needed to be used.

    //However, this change leads to hangs the test case "perf mesh bug27119".
    //So, this correction is better to be implemented in the future.

    if ((aDX * aDX + aDY * aDY) - (aRadius * aRadius) <= mySqTolerance)
      myResIndices.Append(theTargetIndex);

    return CellFilter_Keep;
  }

  //! Checks indices for equlity.
  static Standard_Boolean IsEqual(
    const Standard_Integer theIndex,
    const Standard_Integer theTargetIndex)
  {
    return (theIndex == theTargetIndex);
  }

private:
  Standard_Real             mySqTolerance;
  IMeshData::ListOfInteger  myResIndices;
  IMeshData::VectorOfCircle myCircles;
  gp_XY                     myPoint;
};

#endif
