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

#include <Message_ProgressRange.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <ShapeFix_Root.hxx>
#include <TopoDS_Wire.hxx>
#include <ShapeExtend_Status.hxx>
#include <ShapeAnalysis_Surface.hxx>

class ShapeFix_Edge;
class ShapeAnalysis_Wire;
class TopoDS_Wire;
class TopoDS_Face;
class ShapeExtend_WireData;
class Geom_Surface;
class TopLoc_Location;
class ShapeAnalysis_WireOrder;

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
  Standard_EXPORT ShapeFix_Wire(const TopoDS_Wire& wire,
                                const TopoDS_Face& face,
                                const double       prec);

  //! Sets all modes to default
  Standard_EXPORT void ClearModes();

  //! Clears all statuses
  Standard_EXPORT void ClearStatuses();

  //! Load analyzer with all the data for the wire and face
  //! and drops all fixing statuses
  Standard_EXPORT void Init(const TopoDS_Wire& wire, const TopoDS_Face& face, const double prec);

  //! Load analyzer with all the data already prepared
  //! and drops all fixing statuses
  //! If analyzer contains face, there is no need to set it
  //! by SetFace or SetSurface
  Standard_EXPORT void Init(const occ::handle<ShapeAnalysis_Wire>& saw);

  //! Load data for the wire, and drops all fixing statuses
  Standard_EXPORT void Load(const TopoDS_Wire& wire);

  //! Load data for the wire, and drops all fixing statuses
  Standard_EXPORT void Load(const occ::handle<ShapeExtend_WireData>& sbwd);

  //! Set working face for the wire
  void SetFace(const TopoDS_Face& face);

  //! Set working face for the wire and surface analysis object
  void SetFace(const TopoDS_Face&                        theFace,
               const occ::handle<ShapeAnalysis_Surface>& theSurfaceAnalysis);

  //! Set surface analysis for the wire
  void SetSurface(const occ::handle<ShapeAnalysis_Surface>& theSurfaceAnalysis);

  //! Set surface for the wire
  void SetSurface(const occ::handle<Geom_Surface>& surf);

  //! Set surface for the wire
  void SetSurface(const occ::handle<Geom_Surface>& surf, const TopLoc_Location& loc);

  //! Set working precision (to root and to analyzer)
  Standard_EXPORT void SetPrecision(const double prec) override;

  //! Sets the maximal allowed angle of the tails in radians.
  Standard_EXPORT void SetMaxTailAngle(const double theMaxTailAngle);

  //! Sets the maximal allowed width of the tails.
  Standard_EXPORT void SetMaxTailWidth(const double theMaxTailWidth);

  //! Tells if the wire is loaded
  bool IsLoaded() const;

  //! Tells if the wire and face are loaded
  bool IsReady() const;

  //! returns number of edges in the working wire
  Standard_EXPORT int NbEdges() const;

  //! Makes the resulting Wire (by basic Brep_Builder)
  TopoDS_Wire Wire() const;

  //! Makes the resulting Wire (by BRepAPI_MakeWire)
  TopoDS_Wire WireAPIMake() const;

  //! returns field Analyzer (working tool)
  occ::handle<ShapeAnalysis_Wire> Analyzer() const;

  //! returns working wire
  const occ::handle<ShapeExtend_WireData>& WireData() const;

  //! returns working face (Analyzer.Face())
  const TopoDS_Face& Face() const;

  //! Returns (modifiable) the flag which defines whether it is
  //! allowed to modify topology of the wire during fixing
  //! (adding/removing edges etc.)
  bool& ModifyTopologyMode();

  //! Returns (modifiable) the flag which defines whether the Fix..()
  //! methods are allowed to modify geometry of the edges and vertices
  bool& ModifyGeometryMode();

  //! Returns (modifiable) the flag which defines whether the Fix..()
  //! methods are allowed to modify RemoveLoop of the edges
  int& ModifyRemoveLoopMode();

  //! Returns (modifiable) the flag which defines whether the wire
  //! is to be closed (by calling methods like FixDegenerated()
  //! and FixConnected() for last and first edges).
  bool& ClosedWireMode();

  //! Returns (modifiable) the flag which defines whether the 2d (True)
  //! representation of the wire is preferable over 3d one (in the
  //! case of ambiguity in FixEdgeCurves).
  bool& PreferencePCurveMode();

  //! Returns (modifiable) the flag which defines whether tool
  //! tries to fix gaps first by changing curves ranges (i.e.
  //! using intersection, extrema, projections) or not.
  bool& FixGapsByRangesMode();

  int& FixReorderMode();

  int& FixSmallMode();

  int& FixConnectedMode();

  int& FixEdgeCurvesMode();

  int& FixDegeneratedMode();

  int& FixSelfIntersectionMode();

  int& FixLackingMode();

  int& FixGaps3dMode();

  //! Returns (modifiable) the flag for corresponding Fix..() method
  //! which defines whether this method will be called from the
  //! method APIFix():
  //! -1 default
  //! 1 method will be called
  //! 0 method will not be called
  int& FixGaps2dMode();

  int& FixReversed2dMode();

  int& FixRemovePCurveMode();

  int& FixAddPCurveMode();

  int& FixRemoveCurve3dMode();

  int& FixAddCurve3dMode();

  int& FixSeamMode();

  int& FixShiftedMode();

  int& FixSameParameterMode();

  int& FixVertexToleranceMode();

  int& FixNotchedEdgesMode();

  int& FixSelfIntersectingEdgeMode();

  int& FixIntersectingEdgesMode();

  //! Returns (modifiable) the flag for corresponding Fix..() method
  //! which defines whether this method will be called from the
  //! corresponding Fix..() method of the public level:
  //! -1 default
  //! 1 method will be called
  //! 0 method will not be called
  int& FixNonAdjacentIntersectingEdgesMode();

  int& FixTailMode();

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
  Standard_EXPORT bool Perform(const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Performs an analysis and reorders edges in the wire using class WireOrder.
  //! Flag <theModeBoth> determines the use of miscible mode if necessary.
  Standard_EXPORT bool FixReorder(bool theModeBoth = false);

  //! Applies FixSmall(num) to all edges in the wire
  Standard_EXPORT int FixSmall(const bool lockvtx, const double precsmall = 0.0);

  //! Applies FixConnected(num) to all edges in the wire
  //! Connection between first and last edges is treated only if
  //! flag ClosedMode is True
  //! If <prec> is -1 then MaxTolerance() is taken.
  Standard_EXPORT bool FixConnected(const double prec = -1.0);

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
  Standard_EXPORT bool FixEdgeCurves();

  //! Applies FixDegenerated(num) to all edges in the wire
  //! Connection between first and last edges is treated only if
  //! flag ClosedMode is True
  Standard_EXPORT bool FixDegenerated();

  //! Applies FixSelfIntersectingEdge(num) and
  //! FixIntersectingEdges(num) to all edges in the wire and
  //! FixIntersectingEdges(num1, num2) for all pairs num1 and num2
  //! such that num2 >= num1 + 2
  //! and removes wrong edges if any
  Standard_EXPORT bool FixSelfIntersection();

  //! Applies FixLacking(num) to all edges in the wire
  //! Connection between first and last edges is treated only if
  //! flag ClosedMode is True
  //! If <force> is False (default), test for connectness is done with
  //! precision of vertex between edges, else it is done with minimal
  //! value of vertex tolerance and Analyzer.Precision().
  //! Hence, <force> will lead to inserting lacking edges in replacement
  //! of vertices which have big tolerances.
  Standard_EXPORT bool FixLacking(const bool force = false);

  //! Fixes a wire to be well closed
  //! It performs FixConnected, FixDegenerated and FixLacking between
  //! last and first edges (independingly on flag ClosedMode and modes
  //! for these fixings)
  //! If <prec> is -1 then MaxTolerance() is taken.
  Standard_EXPORT bool FixClosed(const double prec = -1.0);

  //! Fixes gaps between ends of 3d curves on adjacent edges
  //! myPrecision is used to detect the gaps.
  Standard_EXPORT bool FixGaps3d();

  //! Fixes gaps between ends of pcurves on adjacent edges
  //! myPrecision is used to detect the gaps.
  Standard_EXPORT bool FixGaps2d();

  //! Reorder edges in the wire as determined by WireOrder
  //! that should be filled and computed before
  Standard_EXPORT bool FixReorder(const ShapeAnalysis_WireOrder& wi);

  //! Fixes Null Length Edge to be removed
  //! If an Edge has Null Length (regarding preci, or <precsmall>
  //! - what is smaller), it should be removed
  //! It can be with no problem if its two vertices are the same
  //! Else, if lockvtx is False, it is removed and its end vertex
  //! is put on the preceding edge
  //! But if lockvtx is True, this edge must be kept ...
  Standard_EXPORT bool FixSmall(const int num, const bool lockvtx, const double precsmall);

  //! Fixes connected edges (preceding and current)
  //! Forces Vertices (end of preceding-begin of current) to be
  //! the same one
  //! Tests with starting preci or, if given greater, <prec>
  //! If <prec> is -1 then MaxTolerance() is taken.
  //! If <theUpdateWire> is true, synchronizes wire data with context replacements.
  Standard_EXPORT bool FixConnected(const int    num,
                                    const double prec,
                                    const bool   theUpdateWire = true);

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
  Standard_EXPORT bool FixSeam(const int num);

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
  Standard_EXPORT bool FixShifted();

  //! Fixes Degenerated Edge
  //! Checks an <num-th> edge or a point between <num>th-1 and <num>th
  //! edges for a singularity on a supporting surface.
  //! If singularity is detected, either adds new degenerated edge
  //! (before <num>th), or makes <num>th edge to be degenerated.
  Standard_EXPORT bool FixDegenerated(const int num);

  //! Fixes Lacking Edge
  //! Test if two adjucent edges are disconnected in 2d (while
  //! connected in 3d), and in that case either increase tolerance
  //! of the vertex or add a new edge (straight in 2d space), in
  //! order to close wire in 2d.
  //! Returns True if edge was added or tolerance was increased.
  Standard_EXPORT bool FixLacking(const int num, const bool force = false);

  Standard_EXPORT bool FixNotchedEdges();

  //! Fixes gap between ends of 3d curves on num-1 and num-th edges.
  //! myPrecision is used to detect the gap.
  //! If convert is True, converts curves to bsplines to bend.
  Standard_EXPORT bool FixGap3d(const int num, const bool convert = false);

  //! Fixes gap between ends of pcurves on num-1 and num-th edges.
  //! myPrecision is used to detect the gap.
  //! If convert is True, converts pcurves to bsplines to bend.
  Standard_EXPORT bool FixGap2d(const int num, const bool convert = false);

  Standard_EXPORT bool FixTails();

  bool StatusReorder(const ShapeExtend_Status status) const;

  bool StatusSmall(const ShapeExtend_Status status) const;

  bool StatusConnected(const ShapeExtend_Status status) const;

  bool StatusEdgeCurves(const ShapeExtend_Status status) const;

  bool StatusDegenerated(const ShapeExtend_Status status) const;

  bool StatusSelfIntersection(const ShapeExtend_Status status) const;

  bool StatusLacking(const ShapeExtend_Status status) const;

  bool StatusClosed(const ShapeExtend_Status status) const;

  bool StatusGaps3d(const ShapeExtend_Status status) const;

  bool StatusGaps2d(const ShapeExtend_Status status) const;

  bool StatusNotches(const ShapeExtend_Status status) const;

  //! Querying the status of performed API fixing procedures
  //! Each Status..() methods gives information about the last call to
  //! the corresponding Fix..() method of API level:
  //! OK  : no problems detected; nothing done
  //! DONE: some problem(s) was(were) detected and successfully fixed
  //! FAIL: some problem(s) cannot be fixed
  bool StatusRemovedSegment() const;

  bool StatusFixTails(const ShapeExtend_Status status) const;

  //! Queries the status of last call to methods Fix... of
  //! advanced level
  //! For details see corresponding methods; universal statuses are:
  //! OK  : problem not detected; nothing done
  //! DONE: problem was detected and successfully fixed
  //! FAIL: problem cannot be fixed
  bool LastFixStatus(const ShapeExtend_Status status) const;

  //! Returns tool for fixing wires.
  occ::handle<ShapeFix_Edge> FixEdgeTool() const;

  DEFINE_STANDARD_RTTIEXT(ShapeFix_Wire, ShapeFix_Root)

protected:
  //! Updates WireData if some replacements are made
  //! This is necessary for wires (unlike other shape types)
  //! since one edge can present in wire several times
  Standard_EXPORT void UpdateWire();

  occ::handle<ShapeFix_Edge>      myFixEdge;
  occ::handle<ShapeAnalysis_Wire> myAnalyzer;
  bool                            myGeomMode;
  bool                            myTopoMode;
  bool                            myClosedMode;
  bool                            myPreference2d;
  bool                            myFixGapsByRanges;
  int                             myFixReversed2dMode;
  int                             myFixRemovePCurveMode;
  int                             myFixAddPCurveMode;
  int                             myFixRemoveCurve3dMode;
  int                             myFixAddCurve3dMode;
  int                             myFixSeamMode;
  int                             myFixShiftedMode;
  int                             myFixSameParameterMode;
  int                             myFixVertexToleranceMode;
  int                             myFixNotchedEdgesMode;
  int                             myFixSelfIntersectingEdgeMode;
  int                             myFixIntersectingEdgesMode;
  int                             myFixNonAdjacentIntersectingEdgesMode;
  int                             myFixTailMode;
  int                             myRemoveLoopMode;
  int                             myFixReorderMode;
  int                             myFixSmallMode;
  int                             myFixConnectedMode;
  int                             myFixEdgeCurvesMode;
  int                             myFixDegeneratedMode;
  int                             myFixSelfIntersectionMode;
  int                             myFixLackingMode;
  int                             myFixGaps3dMode;
  int                             myFixGaps2dMode;
  int                             myLastFixStatus;
  int                             myStatusReorder;
  int                             myStatusSmall;
  int                             myStatusConnected;
  int                             myStatusEdgeCurves;
  int                             myStatusDegenerated;
  int                             myStatusClosed;
  int                             myStatusSelfIntersection;
  int                             myStatusLacking;
  int                             myStatusGaps3d;
  int                             myStatusGaps2d;
  bool                            myStatusRemovedSegment;
  int                             myStatusNotches;
  int                             myStatusFixTails;
  double                          myMaxTailAngleSine;
  double                          myMaxTailWidth;

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
  Standard_EXPORT bool FixSelfIntersectingEdge(const int num);

  //! Test if two consequent edges are intersecting and fix it
  //! by increasing of tolerance of vertex between edges,
  //! shifting this vertex to the point of intersection,
  //! cutting edges to the intersection point.
  //! It also can give signal to remove edge if it whole is cut by
  //! intersection (if flag ModifyTopologyMode is set).
  Standard_EXPORT bool FixIntersectingEdges(const int num);

  //! Tests if two edges <num1> and <num2> are intersecting and
  //! fix intersection by increasing of tolerance of vertex
  //! nearest to the point of intersection.
  Standard_EXPORT bool FixIntersectingEdges(const int num1, const int num2);

  Standard_EXPORT void FixDummySeam(const int num);
};

#include <ShapeFix_Wire.lxx>

#endif // _ShapeFix_Wire_HeaderFile
