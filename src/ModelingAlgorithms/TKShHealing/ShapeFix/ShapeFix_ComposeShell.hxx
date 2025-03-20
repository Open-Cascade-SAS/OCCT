// Created on: 1999-04-26
// Created by: Andrey BETENEV
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

#ifndef _ShapeFix_ComposeShell_HeaderFile
#define _ShapeFix_ComposeShell_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopLoc_Location.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <ShapeFix_Root.hxx>
#include <ShapeExtend_Status.hxx>
#include <ShapeFix_SequenceOfWireSegment.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TopTools_SequenceOfShape.hxx>
class ShapeExtend_CompositeSurface;
class ShapeAnalysis_TransferParameters;
class ShapeExtend_WireData;
class gp_Lin2d;
class ShapeFix_WireSegment;
class Geom_Surface;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ShapeFix_ComposeShell;
DEFINE_STANDARD_HANDLE(ShapeFix_ComposeShell, ShapeFix_Root)

//! This class is intended to create a shell from the composite
//! surface (grid of surfaces) and set of wires.
//! It may be either division of the supporting surface of the
//! face, or creating a shape corresponding to face on composite
//! surface which is missing in CAS.CADE but exists in some other
//! systems.
//!
//! It splits (if necessary) original face to several ones by
//! splitting lines which are joint lines on a supplied grid of
//! surfaces (U- and V- isolines of the  composite surface).
//! There are two modes of work, which differ in the way of
//! handling faces on periodic surfaces:
//!
//! - if ClosedMode is False (default), when splitting itself is
//! done as if surface were not periodic. The periodicity of the
//! underlying surface is taken into account by duplicating splitting
//! lines in the periodic direction, as necessary to split all
//! the wires (whole parametrical range of a face)
//! In this mode, some regularization procedures are performed
//! (indexation of split segments by patch numbers), and it is
//! expected to be more reliable and robust in case of bad shapes
//!
//! - if ClosedMode is True, when everything on a periodic surfaces
//! is considered as modulo period. This allows to deal with wires
//! which are closed in 3d but not in 2d, with wires which may be
//! shifted on several periods in 2d etc. However, this mode is
//! less reliable since some regularizations do not work for it.
//!
//! The work is made basing on pcurves of the edges. These pcurves
//! should already exist (for example, in the case of division of
//! existing face), then they are taken as is. The existing pcurves
//! should be assigned to one surface (face) for all edges,
//! this surface (face) will be used only for accessing pcurves,
//! and it may have any geometry.
//!
//! All the modifications are recorded in the context tool
//! (ShapeBuild_ReShape).
class ShapeFix_ComposeShell : public ShapeFix_Root
{

public:
  //! Creates empty tool.
  Standard_EXPORT ShapeFix_ComposeShell();

  //! Initializes with composite surface, face and precision.
  //! Here face defines both set of wires and way of getting
  //! pcurves. Precision is used (together with tolerance of edges)
  //! for handling subtle cases, such as tangential intersections.
  Standard_EXPORT void Init(const Handle(ShapeExtend_CompositeSurface)& Grid,
                            const TopLoc_Location&                      L,
                            const TopoDS_Face&                          Face,
                            const Standard_Real                         Prec);

  //! Returns (modifiable) flag for special 'closed'
  //! mode which forces ComposeShell to consider
  //! all pcurves on closed surface as modulo period.
  //! This can reduce reliability, but allows to deal
  //! with wires closed in 3d but open in 2d (missing seam)
  //! Default is False
  Standard_EXPORT Standard_Boolean& ClosedMode();

  //! Performs the work on already loaded data.
  Standard_EXPORT virtual Standard_Boolean Perform();

  //! Splits edges in the original shape by grid.
  //! This is a part of Perform() which does not produce any
  //! resulting shape; the only result is filled context
  //! where splittings are recorded.
  //!
  //! NOTE: If edge is split, it is replaced by wire, and
  //! order of edges in the wire corresponds to FORWARD orientation
  //! of the edge.
  Standard_EXPORT void SplitEdges();

  //! Returns resulting shell or face (or Null shape if not done)
  Standard_EXPORT const TopoDS_Shape& Result() const;

  //! Queries status of last call to Perform()
  //! OK   : nothing done (some kind of error)
  //! DONE1: splitting is done, at least one new face created
  //! DONE2: splitting is done, several new faces obtained
  //! FAIL1: misoriented wire encountered (handled)
  //! FAIL2: recoverable parity error
  //! FAIL3: edge with no pcurve on supporting face
  //! FAIL4: unrecoverable algorithm error (parity check)
  Standard_EXPORT Standard_Boolean Status(const ShapeExtend_Status status) const;

  //! Creates new faces from the set of (closed) wires. Each wire
  //! is put on corresponding patch in the composite surface,
  //! and all pcurves on the initial (pseudo)face are reassigned to
  //! that surface. If several wires are one inside another, single
  //! face is created.
  Standard_EXPORT void DispatchWires(TopTools_SequenceOfShape&       faces,
                                     ShapeFix_SequenceOfWireSegment& wires) const;

  //! Sets tool for transfer parameters from 3d to 2d and vice versa.
  Standard_EXPORT void SetTransferParamTool(
    const Handle(ShapeAnalysis_TransferParameters)& TransferParam);

  //! Gets tool for transfer parameters from 3d to 2d and vice versa.
  Standard_EXPORT Handle(ShapeAnalysis_TransferParameters) GetTransferParamTool() const;

  DEFINE_STANDARD_RTTIEXT(ShapeFix_ComposeShell, ShapeFix_Root)

protected:
  //! Fill sequence of wire segments by wires from myFace
  //! (pre-loaded). It performs reorder so that edges in segments
  //! are well-ordered. The context is applied to all wires
  //! before using them.
  Standard_EXPORT void LoadWires(ShapeFix_SequenceOfWireSegment& seqw) const;

  //! Analyze tangencies and compute orientation code for wire segment
  //! between two intersections: tells if segment is on left or right side
  //! of cutting line, or tangent to it (by several points recomputed to 3d,
  //! distance is compared with tolerance of corresponding edge).
  Standard_EXPORT Standard_Integer ComputeCode(const Handle(ShapeExtend_WireData)& wire,
                                               const gp_Lin2d&                     line,
                                               const Standard_Integer              begInd,
                                               const Standard_Integer              endInd,
                                               const Standard_Real                 begPar,
                                               const Standard_Real                 endPar,
                                               const Standard_Boolean IsInternal = Standard_False);

  //! Splits edges in the wire by given indices of edges and
  //! parameters on them. Returns resulting wire and vertices
  //! corresponding to splitting parameters. If two consecutive
  //! splitting points are too near one to another (with tolerance
  //! of edge), edge is divided in single point. In the same way,
  //! splitting which is too near to end of edge, is not applied
  //! (end vertex is returned instead).
  //!
  //! NOTE: If edge is split, it is replaced by wire, and
  //! order of edges in the wire corresponds to FORWARD orientation
  //! of the edge.
  Standard_EXPORT ShapeFix_WireSegment SplitWire(ShapeFix_WireSegment&            wire,
                                                 TColStd_SequenceOfInteger&       indexes,
                                                 const TColStd_SequenceOfReal&    values,
                                                 TopTools_SequenceOfShape&        vertices,
                                                 const TColStd_SequenceOfInteger& segcodes,
                                                 const Standard_Boolean           cutbyu,
                                                 const Standard_Integer           cutindex);

  //! Split edges in the wire by cutting line.
  //! Wires with FORWARD or REVERSED orientation are considered
  //! as closed.
  //!
  //! All modifications (splitting) are recorded in context,
  //! except splitting of wires marked as EXTERNAL
  //! (they are supposed to be former cutting lines).
  //!
  //! Method fills sequences of parameters of intersection points
  //! of cutting line with all edges, their types, and corresponding
  //! vertices (including ones created during splitting edges).
  Standard_EXPORT Standard_Boolean SplitByLine(ShapeFix_WireSegment&      wire,
                                               const gp_Lin2d&            line,
                                               const Standard_Boolean     cutbyu,
                                               const Standard_Integer     cutindex,
                                               TColStd_SequenceOfReal&    SplitLinePar,
                                               TColStd_SequenceOfInteger& SplitLineCode,
                                               TopTools_SequenceOfShape&  SplitLineVertex);

  //! Split edges in the sequence of wires by cutting line.
  //! Wires with FORWARD or REVERSED orientation are considered
  //! as closed.
  //!
  //! Parts of cutting line which get inside the face (defined by
  //! parity check of intersections with all wires) are added
  //! into that sequence (with orientation EXTERNAL). Each part
  //! is represented by one-edge wire segment with no 3d curve.
  //! They share common vertices with all wires they intersect.
  //!
  //! All modifications (splitting) are recorded in context,
  //! except splitting of wires marked as EXTERNAL
  //! (they are supposed to be former cutting lines).
  Standard_EXPORT void SplitByLine(ShapeFix_SequenceOfWireSegment& seqw,
                                   const gp_Lin2d&                 line,
                                   const Standard_Boolean          cutbyu,
                                   const Standard_Integer          cutindex);

  //! Split initial set of (closed) wires by grid of lines corresponding
  //! to joints between patches on the composite surface.
  //! Parts of joint lines which get inside the face are also added
  //! into the sequence as wires with orientation EXTERNAL.
  //! They share common vertices with all wires they intersect.
  //! All modifications (splitting) are recorded in context,
  //! except splitting of joint edge itself and wires marked as
  //! EXTERNAL (they supposed to be another joint edges).
  Standard_EXPORT void SplitByGrid(ShapeFix_SequenceOfWireSegment& seqw);

  //! Break wires into open wire segments by common vertices
  //! (splitting points), so that each segment is either closed and
  //! not touching others, or touches others at ends (have common
  //! vertices).
  //! After that, each wire segment lies on its own patch of grid.
  Standard_EXPORT void BreakWires(ShapeFix_SequenceOfWireSegment& seqw);

  //! Collect set of wire segments (already split) into closed
  //! wires. This is done by traversing all the segments in allowed
  //! directions, starting only from the REVERSED and FORWARD and
  //! taking EXTERNAL as necessary in fork points. Forks are detected
  //! by common vertices. In fork point, most left way is selected
  //! among all possible ways.
  Standard_EXPORT void CollectWires(ShapeFix_SequenceOfWireSegment& wires,
                                    ShapeFix_SequenceOfWireSegment& seqw);

  //! Creates new faces on one path of grid. It dispatches given loops
  //! (wires) into one or several faces depending on their mutual
  //! position.
  Standard_EXPORT void MakeFacesOnPatch(TopTools_SequenceOfShape&   faces,
                                        const Handle(Geom_Surface)& surf,
                                        TopTools_SequenceOfShape&   loops) const;

  TopAbs_Orientation myOrient;
  TopoDS_Shape       myResult;
  Standard_Integer   myStatus;

private:
  Handle(ShapeExtend_CompositeSurface)     myGrid;
  TopLoc_Location                          myLoc;
  TopoDS_Face                              myFace;
  Standard_Real                            myUResolution;
  Standard_Real                            myVResolution;
  Handle(ShapeAnalysis_TransferParameters) myTransferParamTool;
  Standard_Boolean                         myInvertEdgeStatus;
  Standard_Boolean                         myClosedMode;
  Standard_Boolean                         myUClosed;
  Standard_Boolean                         myVClosed;
  Standard_Real                            myUPeriod;
  Standard_Real                            myVPeriod;
};

#endif // _ShapeFix_ComposeShell_HeaderFile
