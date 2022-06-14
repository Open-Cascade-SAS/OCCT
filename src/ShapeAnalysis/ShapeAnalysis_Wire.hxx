// Created on: 1998-06-03
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

#ifndef _ShapeAnalysis_Wire_HeaderFile
#define _ShapeAnalysis_Wire_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Face.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <IntRes2d_SequenceOfIntersectionPoint.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <ShapeExtend_Status.hxx>
class ShapeExtend_WireData;
class ShapeAnalysis_Surface;
class TopoDS_Wire;
class Geom_Surface;
class TopLoc_Location;
class ShapeAnalysis_WireOrder;
class Geom2d_Curve;
class gp_Pnt2d;
class TopoDS_Shape;
class TopoDS_Edge;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ShapeAnalysis_Wire;
DEFINE_STANDARD_HANDLE(ShapeAnalysis_Wire, Standard_Transient)

//! This class provides analysis of a wire to be compliant to
//! CAS.CADE requirements.
//!
//! The functionalities provided are the following:
//! 1. consistency of 2d and 3d edge curve senses
//! 2. connection of adjacent edges regarding to:
//! a. their vertices
//! b. their pcurves
//! c. their 3d curves
//! 3. adjacency of the edge vertices to its pcurve and 3d curve
//! 4. if a wire is closed or not (considering its 3d and 2d
//! contour)
//! 5. if a wire is outer on its face (considering pcurves)
//!
//! This class can be used in conjunction with class
//! ShapeFix_Wire, which will fix the problems detected by this class.
//!
//! The methods of the given class match to ones of the class
//! ShapeFix_Wire, e.g., CheckSmall and FixSmall.
//! This class also includes some auxiliary methods
//! (e.g., CheckOuterBound, etc.),
//! which have no pair in ShapeFix_Wire.
//!
//! Like methods of ShapeFix_Wire the ones of this class are
//! grouped into two levels:
//! - Public which are recommended for use (the most global
//! method is Perform),
//! - Advanced, for optional use only
//!
//! For analyzing result of Public API checking methods use
//! corresponding Status... method.
//! The 'advanced' functions share the single status field which
//! contains the result of the last performed 'advanced' method.
//! It is queried by the method LastCheckStatus().
//!
//! In order to prepare an analyzer, it is necessary to load a wire,
//! set face and precision.
class ShapeAnalysis_Wire : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT ShapeAnalysis_Wire();
  
  //! Creates object with standard TopoDS_Wire, face
  //! and precision
  Standard_EXPORT ShapeAnalysis_Wire(const TopoDS_Wire& wire, const TopoDS_Face& face, const Standard_Real precision);
  
  //! Creates the object with WireData object, face
  //! and precision
  Standard_EXPORT ShapeAnalysis_Wire(const Handle(ShapeExtend_WireData)& sbwd, const TopoDS_Face& face, const Standard_Real precision);
  
  //! Initializes the object with standard TopoDS_Wire, face
  //! and precision
  Standard_EXPORT void Init (const TopoDS_Wire& wire, const TopoDS_Face& face, const Standard_Real precision);
  
  //! Initializes the object with WireData object, face
  //! and precision
  Standard_EXPORT void Init (const Handle(ShapeExtend_WireData)& sbwd, const TopoDS_Face& face, const Standard_Real precision);
  
  //! Loads the object with standard TopoDS_Wire
  Standard_EXPORT void Load (const TopoDS_Wire& wire);
  
  //! Loads the object with WireData object
  Standard_EXPORT void Load (const Handle(ShapeExtend_WireData)& sbwd);
  
  //! Loads the face the wire lies on
  Standard_EXPORT void SetFace (const TopoDS_Face& face);
  
  //! Loads the surface the wire lies on
  Standard_EXPORT void SetSurface (const Handle(Geom_Surface)& surface);
  
  //! Loads the surface the wire lies on
  Standard_EXPORT void SetSurface (const Handle(Geom_Surface)& surface, const TopLoc_Location& location);
  
  Standard_EXPORT void SetPrecision (const Standard_Real precision);
  
  //! Unsets all the status and distance fields
  //! wire, face and precision are not cleared
  Standard_EXPORT void ClearStatuses();
  
  //! Returns True if wire is loaded and has number of edges >0
    Standard_Boolean IsLoaded() const;
  
  //! Returns True if IsLoaded and underlying face is not null
    Standard_Boolean IsReady() const;
  
  //! Returns the value of precision
    Standard_Real Precision() const;
  
  //! Returns wire object being analyzed
    const Handle(ShapeExtend_WireData)& WireData() const;
  
  //! Returns the number of edges in the wire, or 0 if it is not loaded
    Standard_Integer NbEdges() const;
  
  //! Returns the working face
    const TopoDS_Face& Face() const;
  
  //! Returns the working surface
    const Handle(ShapeAnalysis_Surface)& Surface() const;
  
  //! Performs all the checks in the following order :
  //! CheckOrder, CheckSmall, CheckConnected, CheckEdgeCurves,
  //! CheckDegenerated, CheckSelfIntersection, CheckLacking,
  //! CheckClosed
  //! Returns: True if at least one method returned True;
  //! For deeper analysis use Status...(status) methods
  Standard_EXPORT Standard_Boolean Perform();
  
  //! Calls CheckOrder and returns False if wire is already
  //! ordered (tail-to-head), True otherwise
  //! Flag <isClosed> defines if the wire is closed or not
  //! Flag <mode3d> defines which mode is used (3d or 2d)
  Standard_EXPORT Standard_Boolean CheckOrder (const Standard_Boolean isClosed = Standard_True, const Standard_Boolean mode3d = Standard_True);
  
  //! Calls to CheckConnected for each edge
  //! Returns: True if at least one pair of disconnected edges (not sharing the
  //! same vertex) was detected
  Standard_EXPORT Standard_Boolean CheckConnected (const Standard_Real prec = 0.0);
  
  //! Calls to CheckSmall for each edge
  //! Returns: True if at least one small edge was detected
  Standard_EXPORT Standard_Boolean CheckSmall (const Standard_Real precsmall = 0.0);
  
  //! Checks edges geometry (consistency of 2d and 3d senses, adjasment
  //! of curves to the vertices, etc.).
  //! The order of the checks :
  //! Call ShapeAnalysis_Wire to check:
  //! ShapeAnalysis_Edge::CheckCurve3dWithPCurve  (1),
  //! ShapeAnalysis_Edge::CheckVertcesWithPCurve  (2),
  //! ShapeAnalysis_Edge::CheckVertcesWithCurve3d (3),
  //! CheckSeam                                   (4)
  //! Additional:
  //! CheckGap3d                                  (5),
  //! CheckGap2d                                  (6),
  //! ShapeAnalysis_Edge::CheckSameParameter      (7)
  //! Returns: True if at least one check returned True
  //! Remark:  The numbers in brackets show with what DONEi or FAILi
  //! the status can be queried
  Standard_EXPORT Standard_Boolean CheckEdgeCurves();
  
  //! Calls to CheckDegenerated for each edge
  //! Returns: True if at least one incorrect degenerated edge was detected
  Standard_EXPORT Standard_Boolean CheckDegenerated();
  
  //! Checks if wire is closed, performs CheckConnected,
  //! CheckDegenerated and CheckLacking for the first and the last edges
  //! Returns: True if at least one check returned True
  //! Status:
  //! FAIL1 or DONE1: see CheckConnected
  //! FAIL2 or DONE2: see CheckDegenerated
  Standard_EXPORT Standard_Boolean CheckClosed (const Standard_Real prec = 0.0);
  
  //! Checks self-intersection of the wire (considering pcurves)
  //! Looks for self-intersecting edges and each pair of intersecting
  //! edges.
  //! Warning: It does not check each edge with any other one (only each two
  //! adjacent edges)
  //! The order of the checks :
  //! CheckSelfIntersectingEdge, CheckIntersectingEdges
  //! Returns: True if at least one check returned True
  //! Status:  FAIL1 or DONE1 - see CheckSelfIntersectingEdge
  //! FAIL2 or DONE2 - see CheckIntersectingEdges
  Standard_EXPORT Standard_Boolean CheckSelfIntersection();
  
  //! Calls to CheckLacking for each edge
  //! Returns: True if at least one lacking edge was detected
  Standard_EXPORT Standard_Boolean CheckLacking();
  
  Standard_EXPORT Standard_Boolean CheckGaps3d();
  
  Standard_EXPORT Standard_Boolean CheckGaps2d();
  
  Standard_EXPORT Standard_Boolean CheckCurveGaps();
  
  //! Analyzes the order of the edges in the wire,
  //! uses class WireOrder for that purpose.
  //! Flag <isClosed> defines if the wire is closed or not
  //! Flag <theMode3D> defines 3D or 2d mode.
  //! Flag <theModeBoth> defines miscible mode and the flag <theMode3D> is ignored.
  //! Returns False if wire is already ordered (tail-to-head),
  //! True otherwise.
  //! Use returned WireOrder object for deeper analysis.
  //! Status:
  //! OK   : the same edges orientation, the same edges sequence
  //! DONE1: the same edges orientation, not the same edges sequence
  //! DONE2: as DONE1 and gaps more than myPrecision
  //! DONE3: not the same edges orientation (some need to be reversed)
  //! DONE4: as DONE3 and gaps more than myPrecision
  //! FAIL : algorithm failed (could not detect order)
  Standard_EXPORT Standard_Boolean CheckOrder(ShapeAnalysis_WireOrder &sawo,
                                              Standard_Boolean isClosed = Standard_True,
                                              Standard_Boolean theMode3D = Standard_True,
                                              Standard_Boolean theModeBoth = Standard_False);
  
  //! Checks connected edges (num-th and preceding).
  //! Tests with starting preci from <SBWD> or  with <prec> if
  //! it is greater.
  //! Considers Vertices.
  //! Returns: False if edges are connected by the common vertex, else True
  //! Status  :
  //! OK    : Vertices (end of num-1 th edge and start on num-th one)
  //! are already the same
  //! DONE1 : Absolutely confused (gp::Resolution)
  //! DONE2 : Confused at starting <preci> from <SBWD>
  //! DONE3 : Confused at <prec> but not <preci>
  //! FAIL1 : Not confused
  //! FAIL2 : Not confused but confused with <preci> if reverse num-th edge
  Standard_EXPORT Standard_Boolean CheckConnected (const Standard_Integer num, const Standard_Real prec = 0.0);
  
  //! Checks if an edge has a length not greater than myPreci or
  //! precsmall (if it is smaller)
  //! Returns: False if its length is greater than precision
  //! Status:
  //! OK   : edge is not small or degenerated
  //! DONE1: edge is small, vertices are the same
  //! DONE2: edge is small, vertices are not the same
  //! FAIL : no 3d curve and pcurve
  Standard_EXPORT Standard_Boolean CheckSmall (const Standard_Integer num, const Standard_Real precsmall = 0.0);
  
  //! Checks if a seam pcurves are correct oriented
  //! Returns: False (status OK) if given edge is not a seam or if it is OK
  //! C1 - current pcurve for FORWARD edge,
  //! C2 - current pcurve for REVERSED edge (if returns True they
  //! should be swapped for the seam),
  //! cf, cl - first and last parameters on curves
  //! Status:
  //! OK   : Pcurves are correct or edge is not seam
  //! DONE : Seam pcurves should be swapped
  Standard_EXPORT Standard_Boolean CheckSeam (const Standard_Integer num, Handle(Geom2d_Curve)& C1, Handle(Geom2d_Curve)& C2, Standard_Real& cf, Standard_Real& cl);
  
  //! Checks if a seam pcurves are correct oriented
  //! See previous functions for details
  Standard_EXPORT Standard_Boolean CheckSeam (const Standard_Integer num);
  
  //! Checks for degenerated edge between two adjacent ones.
  //! Fills parameters dgnr1 and dgnr2 with points in parametric
  //! space that correspond to the singularity (either gap that
  //! needs to be filled by degenerated edge or that already filled)
  //! Returns: False if no singularity or edge is already degenerated,
  //! otherwise True
  //! Status:
  //! OK   : No surface singularity, or edge is already degenerated
  //! DONE1: Degenerated edge should be inserted (gap in 2D)
  //! DONE2: Edge <num> should be made degenerated (recompute pcurve
  //! and set the flag)
  //! FAIL1: One of edges neighbouring to degenerated one has
  //! no pcurve
  //! FAIL2: Edge marked as degenerated and has no pcurve
  //! but singularity is not detected
  Standard_EXPORT Standard_Boolean CheckDegenerated (const Standard_Integer num, gp_Pnt2d& dgnr1, gp_Pnt2d& dgnr2);
  
  //! Checks for degenerated edge between two adjacent ones.
  //! Remark : Calls previous function
  //! Status : See the function above for details
  Standard_EXPORT Standard_Boolean CheckDegenerated (const Standard_Integer num);
  
  //! Checks gap between edges in 3D (3d curves).
  //! Checks the distance between ends of 3d curves of the num-th
  //! and preceding edge.
  //! The distance can be queried by MinDistance3d.
  //!
  //! Returns: True if status is DONE
  //! Status:
  //! OK   : Gap is less than myPrecision
  //! DONE : Gap is greater than myPrecision
  //! FAIL : No 3d curve(s) on the edge(s)
  Standard_EXPORT Standard_Boolean CheckGap3d (const Standard_Integer num = 0);
  
  //! Checks gap between edges in 2D (pcurves).
  //! Checks the distance between ends of pcurves of the num-th
  //! and preceding edge.
  //! The distance can be queried by MinDistance2d.
  //!
  //! Returns: True if status is DONE
  //! Status:
  //! OK   : Gap is less than parametric precision out of myPrecision
  //! DONE : Gap is greater than parametric precision out of myPrecision
  //! FAIL : No pcurve(s) on the edge(s)
  Standard_EXPORT Standard_Boolean CheckGap2d (const Standard_Integer num = 0);
  
  //! Checks gap between points on 3D curve and points on surface
  //! generated by pcurve of the num-th edge.
  //! The distance can be queried by MinDistance3d.
  //!
  //! Returns: True if status is DONE
  //! Status:
  //! OK   : Gap is less than myPrecision
  //! DONE : Gap is greater than myPrecision
  //! FAIL : No 3d curve(s) on the edge(s)
  Standard_EXPORT Standard_Boolean CheckCurveGap (const Standard_Integer num = 0);
  
  //! Checks if num-th edge is self-intersecting.
  //! Self-intersection is reported only if intersection point lies outside
  //! of both end vertices of the edge.
  //! Returns: True if edge is self-intersecting.
  //! If returns True it also fills the sequences of intersection points
  //! and corresponding 3d points (only that are not enclosed by a vertices)
  //! Status:
  //! FAIL1 : No pcurve
  //! FAIL2 : No vertices
  //! DONE1 : Self-intersection found
  Standard_EXPORT Standard_Boolean CheckSelfIntersectingEdge (const Standard_Integer num, IntRes2d_SequenceOfIntersectionPoint& points2d, TColgp_SequenceOfPnt& points3d);
  
  Standard_EXPORT Standard_Boolean CheckSelfIntersectingEdge (const Standard_Integer num);
  
  //! Checks two adjacent edges for intersecting.
  //! Intersection is reported only if intersection point is not enclosed
  //! by the common end vertex of the edges.
  //! Returns: True if intersection is found.
  //! If returns True it also fills the sequences of intersection points,
  //! corresponding 3d points, and errors for them (half-distances between
  //! intersection points in 3d calculated from one and from another edge)
  //! Status:
  //! FAIL1 : No pcurve
  //! FAIL2 : No vertices
  //! DONE1 : Self-intersection found
  Standard_EXPORT Standard_Boolean CheckIntersectingEdges (const Standard_Integer num, IntRes2d_SequenceOfIntersectionPoint& points2d, TColgp_SequenceOfPnt& points3d, TColStd_SequenceOfReal& errors);
  
  //! Checks two adjacent edges for intersecting.
  //! Remark : Calls the previous method
  //! Status : See the function above for details
  Standard_EXPORT Standard_Boolean CheckIntersectingEdges (const Standard_Integer num);
  
  //! Checks i-th and j-th edges for intersecting.
  //! Remark : See the previous method for details
  Standard_EXPORT Standard_Boolean CheckIntersectingEdges (const Standard_Integer num1, const Standard_Integer num2, IntRes2d_SequenceOfIntersectionPoint& points2d, TColgp_SequenceOfPnt& points3d, TColStd_SequenceOfReal& errors);
  
  //! Checks i-th and j-th edges for intersecting.
  //! Remark : Calls  previous method.
  //! Status : See the function above for details
  Standard_EXPORT Standard_Boolean CheckIntersectingEdges (const Standard_Integer num1, const Standard_Integer num2);
  
  //! Checks if there is a gap in 2d between edges, not comprised by
  //! the tolerance of their common vertex.
  //! If <Tolerance> is greater than 0. and less than tolerance of
  //! the vertex, then this value is used for check.
  //! Returns: True if not closed gap was detected
  //! p2d1 and p2d2 are the endpoint of <num-1>th edge and start of
  //! the <num>th edge in 2d.
  //! Status:
  //! OK: No edge is lacking (3d and 2d connection)
  //! FAIL1: edges have no vertices (at least one of them)
  //! FAIL2: edges are neither connected by common vertex, nor have
  //! coincided vertices
  //! FAIL1: edges have no pcurves
  //! DONE1: the gap is detected which cannot be closed by the tolerance
  //! of the common vertex (or with value of <Tolerance>)
  //! DONE2: is set (together with DONE1) if gap is detected and the
  //! vector (p2d2 - p2d1) goes in direction opposite to the pcurves
  //! of the edges (if angle is more than 0.9*PI).
  Standard_EXPORT Standard_Boolean CheckLacking (const Standard_Integer num, const Standard_Real Tolerance, gp_Pnt2d& p2d1, gp_Pnt2d& p2d2);
  
  //! Checks if there is a gap in 2D between edges and not comprised by vertex tolerance
  //! The value of SBWD.thepreci is used.
  //! Returns: False if no edge should be inserted
  //! Status:
  //! OK    : No edge is lacking (3d and 2d connection)
  //! DONE1 : The vertex tolerance should be increased only (2d gap is
  //! small)
  //! DONE2 : Edge can be inserted (3d and 2d gaps are large enough)
  Standard_EXPORT Standard_Boolean CheckLacking (const Standard_Integer num, const Standard_Real Tolerance = 0.0);
  
  //! Checks if wire defines an outer bound on the face
  //! Uses ShapeAnalysis::IsOuterBound for analysis
  //! If <APIMake> is True uses BRepAPI_MakeWire to build the
  //! wire, if False (to be used only when edges share common
  //! vertices) uses BRep_Builder to build the wire
  Standard_EXPORT Standard_Boolean CheckOuterBound (const Standard_Boolean APIMake = Standard_True);
  
  //! Detects a notch
  Standard_EXPORT Standard_Boolean CheckNotchedEdges (const Standard_Integer num, Standard_Integer& shortNum, Standard_Real& param, const Standard_Real Tolerance = 0.0);
  
  //! Checks if wire has parametric area less than precision.
  Standard_EXPORT Standard_Boolean CheckSmallArea (const TopoDS_Wire& theWire);
  
  //! Checks with what orientation <shape> (wire or edge) can be
  //! connected to the wire.
  //! Tests distances with starting <preci> from <SBWD> (close confusion),
  //! but if given <prec> is greater, tests with <prec> (coarse confusion).
  //! The smallest found distance can be returned by MinDistance3d
  //!
  //! Returns: False if status is FAIL (see below)
  //! Status:
  //! DONE1 : If <shape> follows <SBWD>, direct sense (normal)
  //! DONE2 : If <shape> follows <SBWD>, but if reversed
  //! DONE3 : If <shape> precedes <SBWD>, direct sense
  //! DONE4 : If <shape> precedes <SBWD>, but if reversed
  //! FAIL1 : If <shape> is neither an edge nor a wire
  //! FAIL2 : If <shape> cannot be connected to <SBWD>
  //!
  //! DONE5 : To the tail of <SBWD> the <shape> is closer with
  //! direct sense
  //! DONE6 : To the head of <SBWD> the <shape> is closer with
  //! direct sense
  //!
  //! Remark:   Statuses DONE1 - DONE4, FAIL1 - FAIL2 are basic and
  //! describe the nearest connection of the <shape> to <SBWD>.
  //! Statuses DONE5 and DONE6 are advanced and are to be used when
  //! analyzing with what sense (direct or reversed) the <shape>
  //! should be connected to <SBWD>:
  //! For tail of <SBWD> if DONE4 is True <shape> should be direct,
  //! otherwise reversed.
  //! For head of <SBWD> if DONE5 is True <shape> should be direct,
  //! otherwise reversed.
  Standard_EXPORT Standard_Boolean CheckShapeConnect (const TopoDS_Shape& shape, const Standard_Real prec = 0.0);
  
  //! The same as previous CheckShapeConnect but is more advanced.
  //! It returns the distances between each end of <sbwd> and each
  //! end of <shape>. For example, <tailhead> stores distance
  //! between tail of <sbwd> and head of <shape>
  //! Remark:  First method CheckShapeConnect calls this one
  Standard_EXPORT Standard_Boolean CheckShapeConnect (Standard_Real& tailhead, Standard_Real& tailtail, Standard_Real& headtail, Standard_Real& headhead, const TopoDS_Shape& shape, const Standard_Real prec = 0.0);
  
  //! Checks existence of loop on wire and return vertices which are loop vertices
  //! (vertices belonging to a few pairs of edges)
  Standard_EXPORT Standard_Boolean CheckLoop (TopTools_IndexedMapOfShape& aMapLoopVertices, TopTools_DataMapOfShapeListOfShape& aMapVertexEdges, TopTools_MapOfShape& aMapSmallEdges, TopTools_MapOfShape& aMapSeemEdges);
  
  Standard_EXPORT Standard_Boolean CheckTail (const TopoDS_Edge& theEdge1, const TopoDS_Edge& theEdge2, const Standard_Real theMaxSine, const Standard_Real theMaxWidth, const Standard_Real theMaxTolerance, TopoDS_Edge& theEdge11, TopoDS_Edge& theEdge12, TopoDS_Edge& theEdge21, TopoDS_Edge& theEdge22);
  
    Standard_Boolean StatusOrder (const ShapeExtend_Status Status) const;
  
    Standard_Boolean StatusConnected (const ShapeExtend_Status Status) const;
  
    Standard_Boolean StatusEdgeCurves (const ShapeExtend_Status Status) const;
  
    Standard_Boolean StatusDegenerated (const ShapeExtend_Status Status) const;
  
    Standard_Boolean StatusClosed (const ShapeExtend_Status Status) const;
  
    Standard_Boolean StatusSmall (const ShapeExtend_Status Status) const;
  
    Standard_Boolean StatusSelfIntersection (const ShapeExtend_Status Status) const;
  
    Standard_Boolean StatusLacking (const ShapeExtend_Status Status) const;
  
    Standard_Boolean StatusGaps3d (const ShapeExtend_Status Status) const;
  
    Standard_Boolean StatusGaps2d (const ShapeExtend_Status Status) const;
  
    Standard_Boolean StatusCurveGaps (const ShapeExtend_Status Status) const;
  
    Standard_Boolean StatusLoop (const ShapeExtend_Status Status) const;
  
  //! Querying the status of the LAST performed 'Advanced' checking procedure
    Standard_Boolean LastCheckStatus (const ShapeExtend_Status Status) const;
  
  //! Returns the last lowest distance in 3D computed by
  //! CheckOrientation, CheckConnected, CheckContinuity3d,
  //! CheckVertex, CheckNewVertex
    Standard_Real MinDistance3d() const;
  
  //! Returns the last lowest distance in 2D-UV computed by
  //! CheckContinuity2d
    Standard_Real MinDistance2d() const;
  
  //! Returns the last maximal distance in 3D computed by
  //! CheckOrientation, CheckConnected, CheckContinuity3d,
  //! CheckVertex, CheckNewVertex, CheckSameParameter
    Standard_Real MaxDistance3d() const;
  
  //! Returns the last maximal distance in 2D-UV computed by
  //! CheckContinuity2d
    Standard_Real MaxDistance2d() const;




  DEFINE_STANDARD_RTTIEXT(ShapeAnalysis_Wire,Standard_Transient)

protected:


  Handle(ShapeExtend_WireData) myWire;
  TopoDS_Face myFace;
  Handle(ShapeAnalysis_Surface) mySurf;
  Standard_Real myPrecision;
  Standard_Real myMin3d;
  Standard_Real myMin2d;
  Standard_Real myMax3d;
  Standard_Real myMax2d;
  Standard_Integer myStatusOrder;
  Standard_Integer myStatusConnected;
  Standard_Integer myStatusEdgeCurves;
  Standard_Integer myStatusDegenerated;
  Standard_Integer myStatusClosed;
  Standard_Integer myStatusSmall;
  Standard_Integer myStatusSelfIntersection;
  Standard_Integer myStatusLacking;
  Standard_Integer myStatusGaps3d;
  Standard_Integer myStatusGaps2d;
  Standard_Integer myStatusCurveGaps;
  Standard_Integer myStatusLoop;
  Standard_Integer myStatus;


private:




};


#include <ShapeAnalysis_Wire.lxx>





#endif // _ShapeAnalysis_Wire_HeaderFile
