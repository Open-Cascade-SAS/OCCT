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

#ifndef BRepMesh_CircleInspector_Header
#define BRepMesh_CircleInspector_Header

#include <BRepMesh.hxx>
#include <BRepMesh_Circle.hxx>
#include <Precision.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_CellFilter.hxx>

//! Auxilary class to find circles shot by the given point.
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
  : myTolerance(theTolerance*theTolerance),
    myResIndices(theAllocator),
    myCircles(theReservedSize)
  {
  }

  //! Adds the circle to vector of circles at the given position.
  //! @param theIndex position of circle in the vector.
  //! @param theCircle circle to be added.
  inline void Bind(const Standard_Integer theIndex,
                   const BRepMesh_Circle& theCircle)
  {
    myCircles.SetValue(theIndex, theCircle);
  }

  //! Resutns vector of registered circles.
  inline const BRepMesh::VectorOfCircle& Circles() const
  {
    return myCircles; 
  }

  //! Returns circle with the given index.
  //! @param theIndex index of circle.
  //! @return circle with the given index.
  inline BRepMesh_Circle& Circle(const Standard_Integer theIndex)
  {
    return myCircles(theIndex);
  }

  //! Set reference point to be checked.
  //! @param thePoint bullet point.
  inline void SetPoint(const gp_XY& thePoint)
  {
    myResIndices.Clear();
    myPoint = thePoint;
  }

  //! Returns list of circles shot by the reference point.
  inline BRepMesh::ListOfInteger& GetShotCircles()
  {
    return myResIndices;
  }

  //! Performs inspection of a circle with the given index.
  //! @param theTargetIndex index of a circle to be checked.
  //! @return status of the check.
  Standard_EXPORT NCollection_CellFilter_Action Inspect(
    const Standard_Integer theTargetIndex);

  //! Checks indices for equlity.
  static Standard_Boolean IsEqual(
    const Standard_Integer theIndex,
    const Standard_Integer theTargetIndex)
  {
    return (theIndex == theTargetIndex);
  }

private:
  Standard_Real            myTolerance;
  BRepMesh::ListOfInteger  myResIndices;
  BRepMesh::VectorOfCircle myCircles;
  gp_XY                    myPoint;
};

#endif
