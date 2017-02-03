// Created on: 1998-06-08
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeAnalysis_Edge_HeaderFile
#define _ShapeAnalysis_Edge_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <ShapeExtend_Status.hxx>
class TopoDS_Edge;
class Geom_Curve;
class TopoDS_Face;
class Geom_Surface;
class TopLoc_Location;
class Geom2d_Curve;
class gp_Pnt2d;
class TopoDS_Vertex;
class gp_Vec2d;
class gp_Pnt;
class Adaptor3d_Curve;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Tool for analyzing the edge.
//! Queries geometrical representations of the edge (3d curve, pcurve
//! on the given face or surface) and topological sub-shapes (bounding
//! vertices).
//! Provides methods for analyzing geometry and topology consistency
//! (3d and pcurve(s) consistency, their adjacency to the vertices).
class ShapeAnalysis_Edge 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor; initialises Status to OK
  Standard_EXPORT ShapeAnalysis_Edge();
  
  //! Tells if the edge has a 3d curve
  Standard_EXPORT Standard_Boolean HasCurve3d (const TopoDS_Edge& edge) const;
  
  //! Returns the 3d curve and bounding parameteres for the edge
  //! Returns False if no 3d curve.
  //! If <orient> is True (default), takes orientation into account:
  //! if the edge is reversed, cf and cl are toggled
  Standard_EXPORT Standard_Boolean Curve3d (const TopoDS_Edge& edge, Handle(Geom_Curve)& C3d, Standard_Real& cf, Standard_Real& cl, const Standard_Boolean orient = Standard_True) const;
  
  //! Gives True if the edge has a 3d curve, this curve is closed,
  //! and the edge has the same vertex at start and end
  Standard_EXPORT Standard_Boolean IsClosed3d (const TopoDS_Edge& edge) const;
  
  //! Tells if the Edge has a pcurve on the face.
  Standard_EXPORT Standard_Boolean HasPCurve (const TopoDS_Edge& edge, const TopoDS_Face& face) const;
  
  //! Tells if the edge has a pcurve on the surface (with location).
  Standard_EXPORT Standard_Boolean HasPCurve (const TopoDS_Edge& edge, const Handle(Geom_Surface)& surface, const TopLoc_Location& location) const;
  
  Standard_EXPORT Standard_Boolean PCurve (const TopoDS_Edge& edge, const TopoDS_Face& face, Handle(Geom2d_Curve)& C2d, Standard_Real& cf, Standard_Real& cl, const Standard_Boolean orient = Standard_True) const;
  
  //! Returns the pcurve and bounding parameteres for the edge
  //! lying on the surface.
  //! Returns False if the edge has no pcurve on this surface.
  //! If <orient> is True (default), takes orientation into account:
  //! if the edge is reversed, cf and cl are toggled
  Standard_EXPORT Standard_Boolean PCurve (const TopoDS_Edge& edge, const Handle(Geom_Surface)& surface, const TopLoc_Location& location, Handle(Geom2d_Curve)& C2d, Standard_Real& cf, Standard_Real& cl, const Standard_Boolean orient = Standard_True) const;
  
  Standard_EXPORT Standard_Boolean BoundUV (const TopoDS_Edge& edge, const TopoDS_Face& face, gp_Pnt2d& first, gp_Pnt2d& last) const;
  
  //! Returns the ends of pcurve
  //! Calls method PCurve with <orient> equal to True
  Standard_EXPORT Standard_Boolean BoundUV (const TopoDS_Edge& edge, const Handle(Geom_Surface)& surface, const TopLoc_Location& location, gp_Pnt2d& first, gp_Pnt2d& last) const;
  
  Standard_EXPORT Standard_Boolean IsSeam (const TopoDS_Edge& edge, const TopoDS_Face& face) const;
  
  //! Returns True if the edge has two pcurves on one surface
  Standard_EXPORT Standard_Boolean IsSeam (const TopoDS_Edge& edge, const Handle(Geom_Surface)& surface, const TopLoc_Location& location) const;
  
  //! Returns start vertex of the edge (taking edge orientation
  //! into account).
  Standard_EXPORT TopoDS_Vertex FirstVertex (const TopoDS_Edge& edge) const;
  
  //! Returns end vertex of the edge (taking edge orientation
  //! into account).
  Standard_EXPORT TopoDS_Vertex LastVertex (const TopoDS_Edge& edge) const;
  
  Standard_EXPORT Standard_Boolean GetEndTangent2d (const TopoDS_Edge& edge, const TopoDS_Face& face, const Standard_Boolean atEnd, gp_Pnt2d& pos, gp_Vec2d& tang, const Standard_Real dparam = 0.0) const;
  
  //! Returns tangent of the edge pcurve at its start (if atEnd is
  //! False) or end (if True), regarding the orientation of edge.
  //! If edge is REVERSED, tangent is reversed before return.
  //! Returns True if pcurve is available and tangent is computed
  //! and is not null, else False.
  Standard_EXPORT Standard_Boolean GetEndTangent2d (const TopoDS_Edge& edge, const Handle(Geom_Surface)& surface, const TopLoc_Location& location, const Standard_Boolean atEnd, gp_Pnt2d& pos, gp_Vec2d& tang, const Standard_Real dparam = 0.0) const;
  
  //! Checks the start and/or end vertex of the edge for matching
  //! with 3d curve with the given precision.
  //! <vtx> = 1 : start vertex only
  //! <vtx> = 2 : end vertex only
  //! <vtx> = 0 : both (default)
  //! If preci < 0 the vertices are considered with their own
  //! tolerances, else with the given <preci>.
  Standard_EXPORT Standard_Boolean CheckVerticesWithCurve3d (const TopoDS_Edge& edge, const Standard_Real preci = -1, const Standard_Integer vtx = 0);
  
  Standard_EXPORT Standard_Boolean CheckVerticesWithPCurve (const TopoDS_Edge& edge, const TopoDS_Face& face, const Standard_Real preci = -1, const Standard_Integer vtx = 0);
  
  //! Checks the start and/or end vertex of the edge for matching
  //! with pcurve with the given precision.
  //! <vtx> = 1 : start vertex
  //! <vtx> = 2 : end vertex
  //! <vtx> = 0 : both
  //! If preci < 0 the vertices are considered with their own
  //! tolerances, else with the given <preci>.
  Standard_EXPORT Standard_Boolean CheckVerticesWithPCurve (const TopoDS_Edge& edge, const Handle(Geom_Surface)& surface, const TopLoc_Location& location, const Standard_Real preci = -1, const Standard_Integer vtx = 0);
  
  Standard_EXPORT Standard_Boolean CheckVertexTolerance (const TopoDS_Edge& edge, const TopoDS_Face& face, Standard_Real& toler1, Standard_Real& toler2);
  
  //! Checks if it is necessary to increase tolerances of the edge
  //! vertices to comprise the ends of 3d curve and pcurve on
  //! the given face (first method) or all pcurves stored in an edge
  //! (second one)
  //! toler1 returns necessary tolerance for first vertex,
  //! toler2 returns necessary tolerance for last vertex.
  Standard_EXPORT Standard_Boolean CheckVertexTolerance (const TopoDS_Edge& edge, Standard_Real& toler1, Standard_Real& toler2);
  
  Standard_EXPORT Standard_Boolean CheckCurve3dWithPCurve (const TopoDS_Edge& edge, const TopoDS_Face& face);
  
  //! Checks mutual orientation of 3d curve and pcurve on the
  //! analysis of curves bounding points
  Standard_EXPORT Standard_Boolean CheckCurve3dWithPCurve (const TopoDS_Edge& edge, const Handle(Geom_Surface)& surface, const TopLoc_Location& location);
  
  //! Returns the status (in the form of True/False) of last Check
  Standard_EXPORT Standard_Boolean Status (const ShapeExtend_Status status) const;
  
  //! Checks the edge to be SameParameter.
  //! Calculates the maximal deviation between 3d curve and each
  //! pcurve of the edge on <NbControl> equidistant points (the same
  //! algorithm as in BRepCheck; default value is 23 as in BRepCheck).
  //! This deviation is returned in <maxdev> parameter.
  //! If deviation is greater than tolerance of the edge (i.e.
  //! incorrect flag) returns False, else returns True.
  Standard_EXPORT Standard_Boolean CheckSameParameter (const TopoDS_Edge& edge, Standard_Real& maxdev, const Standard_Integer NbControl = 23);

  //! Checks the edge to be SameParameter.
  //! Calculates the maximal deviation between 3d curve and each
  //! pcurve of the edge on <NbControl> equidistant points (the same
  //! algorithm as in BRepCheck; default value is 23 as in BRepCheck).
  //! This deviation is returned in <maxdev> parameter.
  //! If deviation is greater than tolerance of the edge (i.e.
  //! incorrect flag) returns False, else returns True.
  Standard_EXPORT Standard_Boolean CheckSameParameter (const TopoDS_Edge& theEdge, const TopoDS_Face& theFace, Standard_Real& theMaxdev, const Standard_Integer theNbControl = 23);
  
  //! Computes the maximal deviation between the two curve
  //! representations.
  //! dev is an input/output parameter and contains the computed
  //! deviation (should be initialized with 0. for the first call).
  //! Used by CheckSameParameter().
  Standard_EXPORT static Standard_Boolean ComputeDeviation (const Adaptor3d_Curve& CRef, const Adaptor3d_Curve& Other, const Standard_Boolean SameParameter, Standard_Real& dev, const Standard_Integer NCONTROL);
  
  //! Checks the first edge is overlapped with second edge.
  //! If distance between two edges is less then theTolOverlap
  //! edges is overlapped.
  //! theDomainDis - length of part of edges on wich edges is overlapped.
  Standard_EXPORT Standard_Boolean CheckOverlapping (const TopoDS_Edge& theEdge1, const TopoDS_Edge& theEdge2, Standard_Real& theTolOverlap, const Standard_Real theDomainDist = 0.0);




protected:



  Standard_Integer myStatus;


private:

  
  //! Check points by pairs (A and A, B and B) with precisions
  //! (preci1 and preci2).
  //! P1 are the points either from 3d curve or from vertices,
  //! P2 are the points from pcurve
  Standard_EXPORT Standard_Boolean CheckPoints (const gp_Pnt& P1A, const gp_Pnt& P1B, const gp_Pnt& P2A, const gp_Pnt& P2B, const Standard_Real preci1, const Standard_Real preci2);




};







#endif // _ShapeAnalysis_Edge_HeaderFile
