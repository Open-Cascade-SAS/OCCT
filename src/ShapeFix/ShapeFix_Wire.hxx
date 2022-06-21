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

#ifndef _ShapeFix_Wire_HeaderFile
#define _ShapeFix_Wire_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <ShapeFix_Root.hxx>
#include <TopoDS_Wire.hxx>
#include <ShapeExtend_Status.hxx>
class ShapeFix_Edge;
class ShapeAnalysis_Wire;
class TopoDS_Wire;
class TopoDS_Face;
class ShapeExtend_WireData;
class Geom_Surface;
class TopLoc_Location;
class ShapeAnalysis_WireOrder;


class ShapeFix_Wire;
DEFINE_STANDARD_HANDLE(ShapeFix_Wire, ShapeFix_Root)

//! This class provides a set of tools for repairing a wire.
//!
//! These are methods Fix...(), organised in two levels:
//!
//! Level 1: Advanced - each method in this level fixes one separate problem,
//! usually dealing with either single edge or connection of the
//! two adjacent edges. These methods should be used carefully and
//! called in right sequence, because some of them depend on others.
//!
//! Level 2: Public (API) - methods which group several methods of level 1
//! and call them in a proper sequence in order to make some
//! consistent set of fixes for a whole wire. It is possible to
//! control calls to methods of the advanced level from methods of
//! the public level by use of flags Fix..Mode() (see below).
//!
//! Fixes can be made in three ways:
//! 1. Increasing tolerance of an edge or a vertex
//! 2. Changing topology (adding/removing/replacing edge in the wire
//! and/or replacing the vertex in the edge)
//! 3. Changing geometry (shifting vertex or adjusting ends of edge
//! curve to vertices, or recomputing curves of the edge)
//!
//! When fix can be made in more than one way (e.g., either
//! by increasing tolerance or shifting a vertex), it is chosen
//! according to the flags:
//! ModifyTopologyMode - allows modification of the topology.
//! This flag can be set when fixing a wire on
//! the separate (free) face, and should be
//! unset for face which is part of shell.
//! ModifyGeometryMode - allows modification of the geometry.
//!
//! The order of descriptions of Fix() methods in this CDL
//! approximately corresponds to the optimal order of calls.
//!
//! NOTE: most of fixing methods expect edges in the
//! ShapeExtend_WireData to be ordered, so it is necessary to make
//! call to FixReorder() before any other fixes
//!
//! ShapeFix_Wire should be initialized prior to any fix by the
//! following data:
//! a) Wire (ether TopoDS_Wire or ShapeExtend_Wire)
//! b) Face or surface
//! c) Precision
//! d) Maximal tail angle and width
//! This can be done either by calling corresponding methods
//! (LoadWire, SetFace or SetSurface, SetPrecision, SetMaxTailAngle
//! and SetMaxTailWidth), or
//! by loading already filled ShapeAnalisis_Wire with method Load
class ShapeFix_Wire : public ShapeFix_Root
{

public:

  
  //! Empty Constructor, creates clear object with default flags
  Standard_EXPORT ShapeFix_Wire();
  
  //! Create new object with default flags and prepare it for use
  //! (Loads analyzer with all the data for the wire and face)
  Standard_EXPORT ShapeFix_Wire(const TopoDS_Wire& wire, const TopoDS_Face& face, const Standard_Real prec);
  
  //! Sets all modes to default
  Standard_EXPORT void ClearModes();
  
  //! Clears all statuses
  Standard_EXPORT void ClearStatuses();
  
  //! Load analyzer with all the data for the wire and face
  //! and drops all fixing statuses
  Standard_EXPORT void Init (const TopoDS_Wire& wire, const TopoDS_Face& face, const Standard_Real prec);
  
  //! Load analyzer with all the data already prepared
  //! and drops all fixing statuses
  //! If analyzer contains face, there is no need to set it
  //! by SetFace or SetSurface
  Standard_EXPORT void Init (const Handle(ShapeAnalysis_Wire)& saw);
  
  //! Load data for the wire, and drops all fixing statuses
  Standard_EXPORT void Load (const TopoDS_Wire& wire);
  
  //! Load data for the wire, and drops all fixing statuses
  Standard_EXPORT void Load (const Handle(ShapeExtend_WireData)& sbwd);
  
  //! Set working face for the wire
    void SetFace (const TopoDS_Face& face);
  
  //! Set surface for the wire
    void SetSurface (const Handle(Geom_Surface)& surf);
  
  //! Set surface for the wire
    void SetSurface (const Handle(Geom_Surface)& surf, const TopLoc_Location& loc);
  
  //! Set working precision (to root and to analyzer)
  Standard_EXPORT virtual void SetPrecision (const Standard_Real prec) Standard_OVERRIDE;
  
  //! Sets the maximal allowed angle of the tails in radians.
  Standard_EXPORT void SetMaxTailAngle (const Standard_Real theMaxTailAngle);
  
  //! Sets the maximal allowed width of the tails.
  Standard_EXPORT void SetMaxTailWidth (const Standard_Real theMaxTailWidth);
  
  //! Tells if the wire is loaded
    Standard_Boolean IsLoaded() const;
  
  //! Tells if the wire and face are loaded
    Standard_Boolean IsReady() const;
  
  //! returns number of edges in the working wire
  Standard_EXPORT Standard_Integer NbEdges() const;
  
  //! Makes the resulting Wire (by basic Brep_Builder)
    TopoDS_Wire Wire() const;
  
  //! Makes the resulting Wire (by BRepAPI_MakeWire)
    TopoDS_Wire WireAPIMake() const;
  
  //! returns field Analyzer (working tool)
  Handle(ShapeAnalysis_Wire) Analyzer() const;
  
  //! returns working wire
    const Handle(ShapeExtend_WireData)& WireData() const;
  
  //! returns working face (Analyzer.Face())
    const TopoDS_Face& Face() const;
  
  //! Returns (modifiable) the flag which defines whether it is
  //! allowed to modify topology of the wire during fixing
  //! (adding/removing edges etc.)
    Standard_Boolean& ModifyTopologyMode();
  
  //! Returns (modifiable) the flag which defines whether the Fix..()
  //! methods are allowed to modify geometry of the edges and vertices
    Standard_Boolean& ModifyGeometryMode();
  
  //! Returns (modifiable) the flag which defines whether the Fix..()
  //! methods are allowed to modify RemoveLoop of the edges
    Standard_Integer& ModifyRemoveLoopMode();
  
  //! Returns (modifiable) the flag which defines whether the wire
  //! is to be closed (by calling methods like FixDegenerated()
  //! and FixConnected() for last and first edges).
    Standard_Boolean& ClosedWireMode();
  
  //! Returns (modifiable) the flag which defines whether the 2d (True)
  //! representation of the wire is preferable over 3d one (in the
  //! case of ambiguity in FixEdgeCurves).
    Standard_Boolean& PreferencePCurveMode();
  
  //! Returns (modifiable) the flag which defines whether tool
  //! tries to fix gaps first by changing curves ranges (i.e.
  //! using intersection, extrema, projections) or not.
    Standard_Boolean& FixGapsByRangesMode();
  
    Standard_Integer& FixReorderMode();
  
    Standard_Integer& FixSmallMode();
  
    Standard_Integer& FixConnectedMode();
  
    Standard_Integer& FixEdgeCurvesMode();
  
    Standard_Integer& FixDegeneratedMode();
  
    Standard_Integer& FixSelfIntersectionMode();
  
    Standard_Integer& FixLackingMode();
  
    Standard_Integer& FixGaps3dMode();
  
  //! Returns (modifiable) the flag for corresponding Fix..() method
  //! which defines whether this method will be called from the
  //! method APIFix():
  //! -1 default
  //! 1 method will be called
  //! 0 method will not be called
    Standard_Integer& FixGaps2dMode();
  
    Standard_Integer& FixReversed2dMode();
  
    Standard_Integer& FixRemovePCurveMode();
  
    Standard_Integer& FixAddPCurveMode();
  
    Standard_Integer& FixRemoveCurve3dMode();
  
    Standard_Integer& FixAddCurve3dMode();
  
    Standard_Integer& FixSeamMode();
  
    Standard_Integer& FixShiftedMode();
  
    Standard_Integer& FixSameParameterMode();
  
    Standard_Integer& FixVertexToleranceMode();
  
    Standard_Integer& FixNotchedEdgesMode();
  
    Standard_Integer& FixSelfIntersectingEdgeMode();
  
    Standard_Integer& FixIntersectingEdgesMode();
  
  //! Returns (modifiable) the flag for corresponding Fix..() method
  //! which defines whether this method will be called from the
  //! corresponding Fix..() method of the public level:
  //! -1 default
  //! 1 method will be called
  //! 0 method will not be called
    Standard_Integer& FixNonAdjacentIntersectingEdgesMode();
  
    Standard_Integer& FixTailMode();
  
  //! This method performs all the available fixes.
  //! If some fix is turned on or off explicitly by the Fix..Mode() flag,
  //! this fix is either called or not depending on that flag.
  //! Else (i.e. if flag is default) fix is called depending on the
  //! situation: some fixes are not called or are limited if order of
  //! edges in the wire is not OK, or depending on modes
  //!
  //! The order of the fixes and default behaviour of Perform() are:
  //! FixReorder
  //! FixSmall (with lockvtx true if ! TopoMode or if wire is not ordered)
  //! FixConnected (if wire is ordered)
  //! FixEdgeCurves (without FixShifted if wire is not ordered)
  //! FixDegenerated (if wire is ordered)
  //! FixSelfIntersection (if wire is ordered and ClosedMode is True)
  //! FixLacking (if wire is ordered)
  Standard_EXPORT Standard_Boolean Perform();
  
  //! Performs an analysis and reorders edges in the wire using class WireOrder.
  //! Flag <theModeBoth> determines the use of miscible mode if necessary.
  Standard_EXPORT Standard_Boolean FixReorder(Standard_Boolean theModeBoth = Standard_False);
  
  //! Applies FixSmall(num) to all edges in the wire
  Standard_EXPORT Standard_Integer FixSmall (const Standard_Boolean lockvtx, const Standard_Real precsmall = 0.0);
  
  //! Applies FixConnected(num) to all edges in the wire
  //! Connection between first and last edges is treated only if
  //! flag ClosedMode is True
  //! If <prec> is -1 then MaxTolerance() is taken.
  Standard_EXPORT Standard_Boolean FixConnected (const Standard_Real prec = -1.0);
  
  //! Groups the fixes dealing with 3d and pcurves of the edges.
  //! The order of the fixes and the default behaviour are:
  //! ShapeFix_Edge::FixReversed2d
  //! ShapeFix_Edge::FixRemovePCurve (only if forced)
  //! ShapeFix_Edge::FixAddPCurve
  //! ShapeFix_Edge::FixRemoveCurve3d (only if forced)
  //! ShapeFix_Edge::FixAddCurve3d
  //! FixSeam,
  //! FixShifted,
  //! ShapeFix_Edge::FixSameParameter
  Standard_EXPORT Standard_Boolean FixEdgeCurves();
  
  //! Applies FixDegenerated(num) to all edges in the wire
  //! Connection between first and last edges is treated only if
  //! flag ClosedMode is True
  Standard_EXPORT Standard_Boolean FixDegenerated();
  
  //! Applies FixSelfIntersectingEdge(num) and
  //! FixIntersectingEdges(num) to all edges in the wire and
  //! FixIntersectingEdges(num1, num2) for all pairs num1 and num2
  //! such that num2 >= num1 + 2
  //! and removes wrong edges if any
  Standard_EXPORT Standard_Boolean FixSelfIntersection();
  
  //! Applies FixLacking(num) to all edges in the wire
  //! Connection between first and last edges is treated only if
  //! flag ClosedMode is True
  //! If <force> is False (default), test for connectness is done with
  //! precision of vertex between edges, else it is done with minimal
  //! value of vertex tolerance and Analyzer.Precision().
  //! Hence, <force> will lead to inserting lacking edges in replacement
  //! of vertices which have big tolerances.
  Standard_EXPORT Standard_Boolean FixLacking (const Standard_Boolean force = Standard_False);
  
  //! Fixes a wire to be well closed
  //! It performs FixConnected, FixDegenerated and FixLacking between
  //! last and first edges (independingly on flag ClosedMode and modes
  //! for these fixings)
  //! If <prec> is -1 then MaxTolerance() is taken.
  Standard_EXPORT Standard_Boolean FixClosed (const Standard_Real prec = -1.0);
  
  //! Fixes gaps between ends of 3d curves on adjacent edges
  //! myPrecision is used to detect the gaps.
  Standard_EXPORT Standard_Boolean FixGaps3d();
  
  //! Fixes gaps between ends of pcurves on adjacent edges
  //! myPrecision is used to detect the gaps.
  Standard_EXPORT Standard_Boolean FixGaps2d();
  
  //! Reorder edges in the wire as determined by WireOrder
  //! that should be filled and computed before
  Standard_EXPORT Standard_Boolean FixReorder (const ShapeAnalysis_WireOrder& wi);
  
  //! Fixes Null Length Edge to be removed
  //! If an Edge has Null Length (regarding preci, or <precsmall>
  //! - what is smaller), it should be removed
  //! It can be with no problem if its two vertices are the same
  //! Else, if lockvtx is False, it is removed and its end vertex
  //! is put on the preceding edge
  //! But if lockvtx is True, this edge must be kept ...
  Standard_EXPORT Standard_Boolean FixSmall (const Standard_Integer num, const Standard_Boolean lockvtx, const Standard_Real precsmall);
  
  //! Fixes connected edges (preceding and current)
  //! Forces Vertices (end of preceding-begin of current) to be
  //! the same one
  //! Tests with starting preci or, if given greater, <prec>
  //! If <prec> is -1 then MaxTolerance() is taken.
  Standard_EXPORT Standard_Boolean FixConnected (const Standard_Integer num, const Standard_Real prec);
  
  //! Fixes a seam edge
  //! A Seam edge has two pcurves, one for forward. one for reversed
  //! The forward pcurve must be set as first
  //!
  //! NOTE that correct order of pcurves in the seam edge depends on
  //! its orientation (i.e., on orientation of the wire, method of
  //! exploration of edges etc.).
  //! Since wire represented by the ShapeExtend_WireData is always forward
  //! (orientation is accounted by edges), it will work correct if:
  //! 1. Wire created from ShapeExtend_WireData with methods
  //! ShapeExtend_WireData::Wire..() is added into the FORWARD face
  //! (orientation can be applied later)
  //! 2. Wire is extracted from the face with orientation not composed
  //! with orientation of the face
  Standard_EXPORT Standard_Boolean FixSeam (const Standard_Integer num);
  
  //! Fixes edges which have pcurves shifted by whole parameter
  //! range on the closed surface (the case may occur if pcurve
  //! of edge was computed by projecting 3d curve, which goes
  //! along the seam).
  //! It compares each two consequent edges and tries to connect them
  //! if distance between ends is near to range of the surface.
  //! It also can detect and fix the case if all pcurves are connected,
  //! but lie out of parametric bounds of the surface.
  //! In addition to FixShifted from ShapeFix_Wire, more
  //! sophisticated check of degenerate points is performed,
  //! and special cases like sphere given by two meridians
  //! are treated.
  Standard_EXPORT Standard_Boolean FixShifted();
  
  //! Fixes Degenerated Edge
  //! Checks an <num-th> edge or a point between <num>th-1 and <num>th
  //! edges for a singularity on a supporting surface.
  //! If singularity is detected, either adds new degenerated edge
  //! (before <num>th), or makes <num>th edge to be degenerated.
  Standard_EXPORT Standard_Boolean FixDegenerated (const Standard_Integer num);
  
  //! Fixes Lacking Edge
  //! Test if two adjucent edges are disconnected in 2d (while
  //! connected in 3d), and in that case either increase tolerance
  //! of the vertex or add a new edge (straight in 2d space), in
  //! order to close wire in 2d.
  //! Returns True if edge was added or tolerance was increased.
  Standard_EXPORT Standard_Boolean FixLacking (const Standard_Integer num, const Standard_Boolean force = Standard_False);
  
  Standard_EXPORT Standard_Boolean FixNotchedEdges();
  
  //! Fixes gap between ends of 3d curves on num-1 and num-th edges.
  //! myPrecision is used to detect the gap.
  //! If convert is True, converts curves to bsplines to bend.
  Standard_EXPORT Standard_Boolean FixGap3d (const Standard_Integer num, const Standard_Boolean convert = Standard_False);
  
  //! Fixes gap between ends of pcurves on num-1 and num-th edges.
  //! myPrecision is used to detect the gap.
  //! If convert is True, converts pcurves to bsplines to bend.
  Standard_EXPORT Standard_Boolean FixGap2d (const Standard_Integer num, const Standard_Boolean convert = Standard_False);
  
  Standard_EXPORT Standard_Boolean FixTails();
  
    Standard_Boolean StatusReorder (const ShapeExtend_Status status) const;
  
    Standard_Boolean StatusSmall (const ShapeExtend_Status status) const;
  
    Standard_Boolean StatusConnected (const ShapeExtend_Status status) const;
  
    Standard_Boolean StatusEdgeCurves (const ShapeExtend_Status status) const;
  
    Standard_Boolean StatusDegenerated (const ShapeExtend_Status status) const;
  
    Standard_Boolean StatusSelfIntersection (const ShapeExtend_Status status) const;
  
    Standard_Boolean StatusLacking (const ShapeExtend_Status status) const;
  
    Standard_Boolean StatusClosed (const ShapeExtend_Status status) const;
  
    Standard_Boolean StatusGaps3d (const ShapeExtend_Status status) const;
  
    Standard_Boolean StatusGaps2d (const ShapeExtend_Status status) const;
  
    Standard_Boolean StatusNotches (const ShapeExtend_Status status) const;
  
  //! Querying the status of performed API fixing procedures
  //! Each Status..() methods gives information about the last call to
  //! the corresponding Fix..() method of API level:
  //! OK  : no problems detected; nothing done
  //! DONE: some problem(s) was(were) detected and successfully fixed
  //! FAIL: some problem(s) cannot be fixed
    Standard_Boolean StatusRemovedSegment() const;
  
    Standard_Boolean StatusFixTails (const ShapeExtend_Status status) const;
  
  //! Queries the status of last call to methods Fix... of
  //! advanced level
  //! For details see corresponding methods; universal statuses are:
  //! OK  : problem not detected; nothing done
  //! DONE: problem was detected and successfully fixed
  //! FAIL: problem cannot be fixed
    Standard_Boolean LastFixStatus (const ShapeExtend_Status status) const;
  
  //! Returns tool for fixing wires.
    Handle(ShapeFix_Edge) FixEdgeTool() const;




  DEFINE_STANDARD_RTTIEXT(ShapeFix_Wire,ShapeFix_Root)

protected:

  
  //! Updates WireData if some replacements are made
  //! This is necessary for wires (unlike other shape types)
  //! since one edge can present in wire several times
  Standard_EXPORT void UpdateWire();

  Handle(ShapeFix_Edge) myFixEdge;
  Handle(ShapeAnalysis_Wire) myAnalyzer;
  Standard_Boolean myGeomMode;
  Standard_Boolean myTopoMode;
  Standard_Boolean myClosedMode;
  Standard_Boolean myPreference2d;
  Standard_Boolean myFixGapsByRanges;
  Standard_Integer myFixReversed2dMode;
  Standard_Integer myFixRemovePCurveMode;
  Standard_Integer myFixAddPCurveMode;
  Standard_Integer myFixRemoveCurve3dMode;
  Standard_Integer myFixAddCurve3dMode;
  Standard_Integer myFixSeamMode;
  Standard_Integer myFixShiftedMode;
  Standard_Integer myFixSameParameterMode;
  Standard_Integer myFixVertexToleranceMode;
  Standard_Integer myFixNotchedEdgesMode;
  Standard_Integer myFixSelfIntersectingEdgeMode;
  Standard_Integer myFixIntersectingEdgesMode;
  Standard_Integer myFixNonAdjacentIntersectingEdgesMode;
  Standard_Integer myFixTailMode;
  Standard_Integer myRemoveLoopMode;
  Standard_Integer myFixReorderMode;
  Standard_Integer myFixSmallMode;
  Standard_Integer myFixConnectedMode;
  Standard_Integer myFixEdgeCurvesMode;
  Standard_Integer myFixDegeneratedMode;
  Standard_Integer myFixSelfIntersectionMode;
  Standard_Integer myFixLackingMode;
  Standard_Integer myFixGaps3dMode;
  Standard_Integer myFixGaps2dMode;
  Standard_Integer myLastFixStatus;
  Standard_Integer myStatusReorder;
  Standard_Integer myStatusSmall;
  Standard_Integer myStatusConnected;
  Standard_Integer myStatusEdgeCurves;
  Standard_Integer myStatusDegenerated;
  Standard_Integer myStatusClosed;
  Standard_Integer myStatusSelfIntersection;
  Standard_Integer myStatusLacking;
  Standard_Integer myStatusGaps3d;
  Standard_Integer myStatusGaps2d;
  Standard_Boolean myStatusRemovedSegment;
  Standard_Integer myStatusNotches;
  Standard_Integer myStatusFixTails;
  Standard_Real myMaxTailAngleSine;
  Standard_Real myMaxTailWidth;


private:

  
  //! Detect and fix self-intersecting pcurve of edge <num>.
  //! Fix is made by one of two methods:
  //! - cut out the self-intersection loop on pcurve (thus
  //! producing C0 pcurve). This also increases tolerance of edge
  //! in order to satisfy SameParameter requirement.
  //! - increase tolerance of the vertex of edge nearest to the
  //! self-intersection point so that it comprises that point.
  //! The first method is considered only if ModifyGeometryMode
  //! is True. In that case, the method which requires less
  //! increasing of tolerance is selected.
  Standard_EXPORT Standard_Boolean FixSelfIntersectingEdge (const Standard_Integer num);
  
  //! Test if two consequent edges are intersecting and fix it
  //! by increasing of tolerance of vertex between edges,
  //! shifting this vertex to the point of intersection,
  //! cutting edges to the intersection point.
  //! It also can give signal to remove edge if it whole is cut by
  //! intersection (if flag ModifyTopologyMode is set).
  Standard_EXPORT Standard_Boolean FixIntersectingEdges (const Standard_Integer num);
  
  //! Tests if two edges <num1> and <num2> are intersecting and
  //! fix intersection by increasing of tolerance of vertex
  //! nearest to the point of intersection.
  Standard_EXPORT Standard_Boolean FixIntersectingEdges (const Standard_Integer num1, const Standard_Integer num2);
  
  Standard_EXPORT void FixDummySeam (const Standard_Integer num);



};


#include <ShapeFix_Wire.lxx>





#endif // _ShapeFix_Wire_HeaderFile
