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

#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <IntPolyh_ArrayOfPoints.hxx>
#include <IntPolyh_ArrayOfTriangles.hxx>
#include <IntPolyh_ArrayOfEdges.hxx>

//! The class represents the triangle built from three IntPolyh points
//! and three IntPolyh edges.
class IntPolyh_Triangle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor
  IntPolyh_Triangle()
      : myHasIntersection(false),
        myIsIntersectionPossible(true),
        myIsDegenerated(false),
        myDeflection(0.0)
  {
    myPoints[0]            = -1;
    myPoints[1]            = -1;
    myPoints[2]            = -1;
    myEdges[0]             = -1;
    myEdges[1]             = -1;
    myEdges[2]             = -1;
    myEdgesOrientations[0] = 0;
    myEdgesOrientations[1] = 0;
    myEdgesOrientations[2] = 0;
  }

  //! Constructor
  IntPolyh_Triangle(const int thePoint1,
                    const int thePoint2,
                    const int thePoint3)
      : myHasIntersection(false),
        myIsIntersectionPossible(true),
        myIsDegenerated(false),
        myDeflection(0.0)
  {
    myPoints[0]            = thePoint1;
    myPoints[1]            = thePoint2;
    myPoints[2]            = thePoint3;
    myEdges[0]             = -1;
    myEdges[1]             = -1;
    myEdges[2]             = -1;
    myEdgesOrientations[0] = 0;
    myEdgesOrientations[1] = 0;
    myEdgesOrientations[2] = 0;
  }

  //! Returns the first point
  int FirstPoint() const { return myPoints[0]; }

  //! Returns the second point
  int SecondPoint() const { return myPoints[1]; }

  //! Returns the third point
  int ThirdPoint() const { return myPoints[2]; }

  //! Returns the first edge
  int FirstEdge() const { return myEdges[0]; }

  //! Returns the orientation of the first edge
  int FirstEdgeOrientation() const { return myEdgesOrientations[0]; }

  //! Returns the second edge
  int SecondEdge() const { return myEdges[1]; }

  //! Returns the orientation of the second edge
  int SecondEdgeOrientation() const { return myEdgesOrientations[1]; }

  //! Returns the third edge
  int ThirdEdge() const { return myEdges[2]; }

  //! Returns the orientation of the third edge
  int ThirdEdgeOrientation() const { return myEdgesOrientations[2]; }

  //! Returns the deflection of the triangle
  double Deflection() const { return myDeflection; }

  //! Returns possibility of the intersection
  bool IsIntersectionPossible() const { return myIsIntersectionPossible; }

  //! Returns true if the triangle has interfered the other triangle
  bool HasIntersection() const { return myHasIntersection; }

  //! Returns the Degenerated flag
  bool IsDegenerated() const { return myIsDegenerated; }

  //! Sets the first point
  void SetFirstPoint(const int thePoint) { myPoints[0] = thePoint; }

  //! Sets the second point
  void SetSecondPoint(const int thePoint) { myPoints[1] = thePoint; }

  //! Sets the third point
  void SetThirdPoint(const int thePoint) { myPoints[2] = thePoint; }

  //! Sets the first edge
  void SetFirstEdge(const int theEdge, const int theEdgeOrientation)
  {
    myEdges[0]             = theEdge;
    myEdgesOrientations[0] = theEdgeOrientation;
  }

  //! Sets the second edge
  void SetSecondEdge(const int theEdge, const int theEdgeOrientation)
  {
    myEdges[1]             = theEdge;
    myEdgesOrientations[1] = theEdgeOrientation;
  }

  //! Sets the third edge
  void SetThirdEdge(const int theEdge, const int theEdgeOrientation)
  {
    myEdges[2]             = theEdge;
    myEdgesOrientations[2] = theEdgeOrientation;
  }

  //! Sets the deflection
  void SetDeflection(const double theDeflection) { myDeflection = theDeflection; }

  //! Sets the flag of possibility of intersection
  void SetIntersectionPossible(const bool theIP) { myIsIntersectionPossible = theIP; }

  //! Sets the flag of intersection
  void SetIntersection(const bool theInt) { myHasIntersection = theInt; }

  //! Sets the degenerated flag
  void SetDegenerated(const bool theDegFlag) { myIsDegenerated = theDegFlag; }

  //! Gets the edge number by the index
  int GetEdgeNumber(const int theEdgeIndex) const
  {
    return ((theEdgeIndex >= 1 && theEdgeIndex <= 3) ? myEdges[theEdgeIndex - 1] : 0);
  }

  //! Sets the edge by the index
  void SetEdge(const int theEdgeIndex, const int theEdgeNumber)
  {
    if (theEdgeIndex >= 1 && theEdgeIndex <= 3)
    {
      myEdges[theEdgeIndex - 1] = theEdgeNumber;
    }
  }

  //! Gets the edges orientation by the index
  int GetEdgeOrientation(const int theEdgeIndex) const
  {
    return ((theEdgeIndex >= 1 && theEdgeIndex <= 3) ? myEdgesOrientations[theEdgeIndex - 1] : 0);
  }

  //! Sets the edges orientation by the index
  void SetEdgeOrientation(const int theEdgeIndex,
                          const int theEdgeOrientation)
  {
    if (theEdgeIndex >= 1 && theEdgeIndex <= 3)
    {
      myEdgesOrientations[theEdgeIndex - 1] = theEdgeOrientation;
    }
  }

  //! Computes the deflection for the triangle
  Standard_EXPORT double ComputeDeflection(const occ::handle<Adaptor3d_Surface>& theSurface,
                                                  const IntPolyh_ArrayOfPoints&    thePoints);

  //! Gets the adjacent triangle
  Standard_EXPORT int GetNextTriangle(const int       theTriangle,
                                                   const int       theEdgeNum,
                                                   const IntPolyh_ArrayOfEdges& TEdges) const;

  //! Splits the triangle on two to decrease its deflection
  Standard_EXPORT void MiddleRefinement(const int           theTriangleNumber,
                                        const occ::handle<Adaptor3d_Surface>& theSurface,
                                        IntPolyh_ArrayOfPoints&          TPoints,
                                        IntPolyh_ArrayOfTriangles&       TTriangles,
                                        IntPolyh_ArrayOfEdges&           TEdges);

  //! Splits the current triangle and new triangles until the refinement
  //! criterion is not achieved
  Standard_EXPORT void MultipleMiddleRefinement(const double              theRefineCriterion,
                                                const Bnd_Box&                   theBox,
                                                const int           theTriangleNumber,
                                                const occ::handle<Adaptor3d_Surface>& theSurface,
                                                IntPolyh_ArrayOfPoints&          TPoints,
                                                IntPolyh_ArrayOfTriangles&       TTriangles,
                                                IntPolyh_ArrayOfEdges&           TEdges);

  //! Links edges to triangle
  Standard_EXPORT void LinkEdges2Triangle(const IntPolyh_ArrayOfEdges& TEdges,
                                          const int       theEdge1,
                                          const int       theEdge2,
                                          const int       theEdge3);

  //! Sets the appropriate edge and orientation for the triangle.
  Standard_EXPORT void SetEdgeAndOrientation(const IntPolyh_Edge&   theEdge,
                                             const int theEdgeIndex);

  //! Returns the bounding box of the triangle.
  Standard_EXPORT const Bnd_Box& BoundingBox(const IntPolyh_ArrayOfPoints& thePoints);

  //! Dumps the contents of the triangle.
  Standard_EXPORT void Dump(const int v) const;

private:
  int myPoints[3];
  int myEdges[3];
  int myEdgesOrientations[3];
  bool myHasIntersection : 1;
  bool myIsIntersectionPossible : 1;
  bool myIsDegenerated : 1;
  double    myDeflection;
  Bnd_Box          myBox;
};

#endif // _IntPolyh_Triangle_HeaderFile
