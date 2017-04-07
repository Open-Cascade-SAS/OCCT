// Created on: 1999-03-05
// Created by: Fabrice SERVANT
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _IntPolyh_Triangle_HeaderFile
#define _IntPolyh_Triangle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Bnd_Box.hxx>
#include <IntPolyh_ArrayOfPoints.hxx>
#include <IntPolyh_ArrayOfTriangles.hxx>
#include <IntPolyh_ArrayOfEdges.hxx>
class Adaptor3d_HSurface;
class IntPolyh_StartPoint;

//! The class represents the triangle built from three IntPolyh points
//! and three IntPolyh edges.
class IntPolyh_Triangle 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor
  IntPolyh_Triangle() :
    myHasIntersection(Standard_False),
    myIsIntersectionPossible(Standard_True),
    myIsDegenerated(Standard_False),
    myDeflection(0.0)
  {
    myPoints[0] = -1;
    myPoints[1] = -1;
    myPoints[2] = -1;
    myEdges[0] = -1;
    myEdges[1] = -1;
    myEdges[2] = -1;
    myEdgesOrientations[0] = 0;
    myEdgesOrientations[1] = 0;
    myEdgesOrientations[2] = 0;
  }

  //! Constructor
  IntPolyh_Triangle(const Standard_Integer thePoint1,
                    const Standard_Integer thePoint2,
                    const Standard_Integer thePoint3)
  :
    myHasIntersection(Standard_False),
    myIsIntersectionPossible(Standard_True),
    myIsDegenerated(Standard_False),
    myDeflection(0.0)
  {
    myPoints[0] = thePoint1;
    myPoints[1] = thePoint2;
    myPoints[2] = thePoint3;
    myEdges[0] = -1;
    myEdges[1] = -1;
    myEdges[2] = -1;
    myEdgesOrientations[0] = 0;
    myEdgesOrientations[1] = 0;
    myEdgesOrientations[2] = 0;
  }

  //! Returns the first point
  Standard_Integer FirstPoint() const
  {
    return myPoints[0];
  }
  //! Returns the second point
  Standard_Integer SecondPoint() const
  {
    return myPoints[1];
  }
  //! Returns the third point
  Standard_Integer ThirdPoint() const
  {
    return myPoints[2];
  }
  //! Returns the first edge
  Standard_Integer FirstEdge() const
  {
    return myEdges[0];
  }
  //! Returns the orientation of the first edge
  Standard_Integer FirstEdgeOrientation() const
  {
    return myEdgesOrientations[0];
  }
  //! Returns the second edge
  Standard_Integer SecondEdge() const
  {
    return myEdges[1];
  }
  //! Returns the orientation of the second edge
  Standard_Integer SecondEdgeOrientation() const
  {
    return myEdgesOrientations[1];
  }
  //! Returns the third edge
  Standard_Integer ThirdEdge() const
  {
    return myEdges[2];
  }
  //! Returns the orientation of the third edge
  Standard_Integer ThirdEdgeOrientation() const
  {
    return myEdgesOrientations[2];
  }
  //! Returns the deflection of the triangle
  Standard_Real Deflection() const
  {
    return myDeflection;
  }
  //! Returns possibility of the intersection
  Standard_Boolean IsIntersectionPossible() const
  {
    return myIsIntersectionPossible;
  }
  //! Returns true if the triangle has interfered the other triangle
  Standard_Boolean HasIntersection() const
  {
    return myHasIntersection;
  }
  //! Returns the Degenerated flag
  Standard_Boolean IsDegenerated() const
  {
    return myIsDegenerated;
  }
  //! Sets the first point
  void SetFirstPoint(const Standard_Integer thePoint)
  {
    myPoints[0] = thePoint;
  }
  //! Sets the second point
  void SetSecondPoint(const Standard_Integer thePoint)
  {
    myPoints[1] = thePoint;
  }
  //! Sets the third point
  void SetThirdPoint(const Standard_Integer thePoint)
  {
    myPoints[2] = thePoint;
  }
  //! Sets the first edge
  void SetFirstEdge(const Standard_Integer theEdge,
                    const Standard_Integer theEdgeOrientation)
  {
    myEdges[0] = theEdge;
    myEdgesOrientations[0] = theEdgeOrientation;
  }
  //! Sets the second edge
  void SetSecondEdge(const Standard_Integer theEdge,
                     const Standard_Integer theEdgeOrientation)
  {
    myEdges[1] = theEdge;
    myEdgesOrientations[1] = theEdgeOrientation;
  }
  //! Sets the third edge
  void SetThirdEdge(const Standard_Integer theEdge,
                    const Standard_Integer theEdgeOrientation)
  {
    myEdges[2] = theEdge;
    myEdgesOrientations[2] = theEdgeOrientation;
  }
  //! Sets the deflection
  void SetDeflection(const Standard_Real theDeflection)
  {
    myDeflection = theDeflection;
  }
  //! Sets the flag of possibility of intersection
  void SetIntersectionPossible(const Standard_Boolean theIP)
  {
    myIsIntersectionPossible = theIP;
  }
  //! Sets the flag of intersection
  void SetIntersection(const Standard_Boolean theInt)
  {
    myHasIntersection = theInt;
  }
  //! Sets the degenerated flag
  void SetDegenerated(const Standard_Boolean theDegFlag)
  {
    myIsDegenerated = theDegFlag;
  }
  //! Gets the edge number by the index
  Standard_Integer GetEdgeNumber(const Standard_Integer theEdgeIndex) const
  {
    return ((theEdgeIndex >= 1 && theEdgeIndex <= 3) ? myEdges[theEdgeIndex - 1] : 0);
  }
  //! Sets the edge by the index
  void SetEdge(const Standard_Integer theEdgeIndex,
               const Standard_Integer theEdgeNumber)
  {
    if (theEdgeIndex >= 1 && theEdgeIndex <= 3) {
      myEdges[theEdgeIndex - 1] = theEdgeNumber;
    }
  }
  //! Gets the edges orientation by the index
  Standard_Integer GetEdgeOrientation(const Standard_Integer theEdgeIndex) const
  {
    return ((theEdgeIndex >= 1 && theEdgeIndex <= 3) ?
      myEdgesOrientations[theEdgeIndex - 1] : 0);
  }
  //! Sets the edges orientation by the index
  void SetEdgeOrientation(const Standard_Integer theEdgeIndex,
                          const Standard_Integer theEdgeOrientation)
  {
    if (theEdgeIndex >= 1 && theEdgeIndex <= 3) {
      myEdgesOrientations[theEdgeIndex - 1] = theEdgeOrientation;
    }
  }

  //! Computes the deflection for the triangle
  Standard_EXPORT Standard_Real ComputeDeflection(const Handle(Adaptor3d_HSurface)& theSurface,
                                                  const IntPolyh_ArrayOfPoints& thePoints);

  //! Gets the adjacent triangle
  Standard_EXPORT Standard_Integer GetNextTriangle(const Standard_Integer theTriangle,
                                                   const Standard_Integer theEdgeNum,
                                                   const IntPolyh_ArrayOfEdges& TEdges) const;

  //! Splits the triangle on two to decrease its deflection
  Standard_EXPORT void MiddleRefinement(const Standard_Integer theTriangleNumber,
                                        const Handle(Adaptor3d_HSurface)& theSurface,
                                        IntPolyh_ArrayOfPoints& TPoints,
                                        IntPolyh_ArrayOfTriangles& TTriangles,
                                        IntPolyh_ArrayOfEdges& TEdges);

  //! Splits the current triangle and new triangles until the refinement
  //! criterion is not achieved
  Standard_EXPORT void MultipleMiddleRefinement(const Standard_Real theRefineCriterion,
                                                const Bnd_Box& theBox,
                                                const Standard_Integer theTriangleNumber,
                                                const Handle(Adaptor3d_HSurface)& theSurface,
                                                IntPolyh_ArrayOfPoints& TPoints,
                                                IntPolyh_ArrayOfTriangles& TTriangles,
                                                IntPolyh_ArrayOfEdges& TEdges);

  //! Links edges to triangle
  Standard_EXPORT void LinkEdges2Triangle(const IntPolyh_ArrayOfEdges& TEdges,
                                          const Standard_Integer theEdge1,
                                          const Standard_Integer theEdge2,
                                          const Standard_Integer theEdge3);

  //! Sets the appropriate edge and orientation for the triangle.
  Standard_EXPORT void SetEdgeAndOrientation(const IntPolyh_Edge& theEdge,
                                             const Standard_Integer theEdgeIndex);

  //! Returns the bounding box of the triangle.
  Standard_EXPORT const Bnd_Box& BoundingBox(const IntPolyh_ArrayOfPoints& thePoints);

  //! Dumps the contents of the triangle.
  Standard_EXPORT void Dump (const Standard_Integer v) const;

protected:

private:

  Standard_Integer myPoints[3];
  Standard_Integer myEdges[3];
  Standard_Integer myEdgesOrientations[3];
  Standard_Boolean myHasIntersection:1;
  Standard_Boolean myIsIntersectionPossible:1;
  Standard_Boolean myIsDegenerated:1;
  Standard_Real myDeflection;
  Bnd_Box myBox;

};

#endif // _IntPolyh_Triangle_HeaderFile
