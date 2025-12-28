// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRepClass_FaceExplorer_HeaderFile
#define _BRepClass_FaceExplorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

#include <TopAbs_Orientation.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <Standard_Integer.hxx>
class gp_Pnt2d;
class gp_Lin2d;
class BRepClass_Edge;

//! Provide an exploration of a BRep Face for the
//! classification. Return UV edges.
class BRepClass_FaceExplorer
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepClass_FaceExplorer(const TopoDS_Face& F);

  //! Checks the point and change its coords if it is located too far
  //! from the bounding box of the face. New Coordinates of the point
  //! will be on the line between the point and the center of the
  //! bounding box. Returns True if point was not changed.
  Standard_EXPORT bool CheckPoint(gp_Pnt2d& thePoint);

  //! Should return True if the point is outside a
  //! bounding volume of the face.
  Standard_EXPORT bool Reject(const gp_Pnt2d& P) const;

  //! Returns in <L>, <Par> a segment having at least
  //! one intersection with the face boundary to
  //! compute intersections.
  Standard_EXPORT bool Segment(const gp_Pnt2d& P, gp_Lin2d& L, double& Par);

  //! Returns in <L>, <Par> a segment having at least
  //! one intersection with the face boundary to
  //! compute intersections. Each call gives another segment.
  Standard_EXPORT bool OtherSegment(const gp_Pnt2d& P, gp_Lin2d& L, double& Par);

  //! Starts an exploration of the wires.
  Standard_EXPORT void InitWires();

  //! Returns True if there is a current wire.
  bool MoreWires() const;

  //! Sets the explorer to the next wire.
  void NextWire();

  //! Returns True if the wire bounding volume does not
  //! intersect the segment.
  Standard_EXPORT bool RejectWire(const gp_Lin2d& L, const double Par) const;

  //! Starts an exploration of the edges of the current
  //! wire.
  Standard_EXPORT void InitEdges();

  //! Returns True if there is a current edge.
  bool MoreEdges() const;

  //! Sets the explorer to the next edge.
  void NextEdge();

  //! Returns True if the edge bounding volume does not
  //! intersect the segment.
  Standard_EXPORT bool RejectEdge(const gp_Lin2d& L, const double Par) const;

  //! Current edge in current wire and its orientation.
  Standard_EXPORT void CurrentEdge(BRepClass_Edge& E, TopAbs_Orientation& Or) const;

  //! Returns the maximum tolerance
  double MaxTolerance() const { return myMaxTolerance; }

  //! Sets the maximum tolerance at
  //! which to start checking in the intersector
  void SetMaxTolerance(const double theValue) { myMaxTolerance = theValue; }

  //! Returns true if we are using boxes
  //! in the intersector
  bool UseBndBox() const { return myUseBndBox; }

  //! Sets the status of whether we are
  //! using boxes or not
  void SetUseBndBox(const bool theValue) { myUseBndBox = theValue; }

protected:
  //! Computes UV bounds of a face
  Standard_EXPORT void ComputeFaceBounds();

private:
  TopoDS_Face     myFace;
  TopExp_Explorer myWExplorer;
  TopExp_Explorer myEExplorer;
  int             myCurEdgeInd;
  double          myCurEdgePar;
  double          myMaxTolerance;
  bool            myUseBndBox;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    myMapVE;

  double myUMin;
  double myUMax;
  double myVMin;
  double myVMax;
};

#include <BRepClass_FaceExplorer.lxx>

#endif // _BRepClass_FaceExplorer_HeaderFile
