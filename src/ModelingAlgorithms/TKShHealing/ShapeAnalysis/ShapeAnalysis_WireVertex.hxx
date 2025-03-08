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

#ifndef _ShapeAnalysis_WireVertex_HeaderFile
#define _ShapeAnalysis_WireVertex_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Integer.hxx>
class ShapeExtend_WireData;
class TopoDS_Wire;
class gp_XYZ;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Analyzes and records status of vertices in a Wire
//!
//! The Wire has formerly been loaded in a ShapeExtend_WireData
//! For each Vertex, a status and some data can be attached
//! (case found, position and parameters)
//! Then, these information can be used to fix problems
class ShapeAnalysis_WireVertex
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT ShapeAnalysis_WireVertex();

  Standard_EXPORT void Init(const TopoDS_Wire& wire, const Standard_Real preci);

  Standard_EXPORT void Init(const Handle(ShapeExtend_WireData)& swbd, const Standard_Real preci);

  Standard_EXPORT void Load(const TopoDS_Wire& wire);

  Standard_EXPORT void Load(const Handle(ShapeExtend_WireData)& sbwd);

  //! Sets the precision for work
  //! Analysing: for each Vertex, comparison between the end of the
  //! preceding edge and the start of the following edge
  //! Each Vertex rank corresponds to the End Vertex of the Edge of
  //! same rank, in the ShapeExtend_WireData. I.E. for Vertex <num>,
  //! Edge <num> is the preceding one, <num+1> is the following one
  Standard_EXPORT void SetPrecision(const Standard_Real preci);

  Standard_EXPORT void Analyze();

  //! Records status "Same Vertex" (logically) on Vertex <num>
  Standard_EXPORT void SetSameVertex(const Standard_Integer num);

  //! Records status "Same Coords" (at the Vertices Tolerances)
  Standard_EXPORT void SetSameCoords(const Standard_Integer num);

  //! Records status "Close Coords" (at the Precision of <me>)
  Standard_EXPORT void SetClose(const Standard_Integer num);

  //! <num> is the End of preceding Edge, and its projection on the
  //! following one lies on it at the Precision of <me>
  //! <ufol> gives the parameter on the following edge
  Standard_EXPORT void SetEnd(const Standard_Integer num,
                              const gp_XYZ&          pos,
                              const Standard_Real    ufol);

  //! <num> is the Start of following Edge, its projection on the
  //! preceding one lies on it at the Precision of <me>
  //! <upre> gives the parameter on the preceding edge
  Standard_EXPORT void SetStart(const Standard_Integer num,
                                const gp_XYZ&          pos,
                                const Standard_Real    upre);

  //! <num> is the Intersection of both Edges
  //! <upre> is the parameter on preceding edge, <ufol> on
  //! following edge
  Standard_EXPORT void SetInters(const Standard_Integer num,
                                 const gp_XYZ&          pos,
                                 const Standard_Real    upre,
                                 const Standard_Real    ufol);

  //! <num> cannot be said as same vertex
  Standard_EXPORT void SetDisjoined(const Standard_Integer num);

  //! Returns True if analysis was performed, else returns False
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns precision value used in analysis
  Standard_EXPORT Standard_Real Precision() const;

  //! Returns the number of edges in analyzed wire (i.e. the
  //! length of all arrays)
  Standard_EXPORT Standard_Integer NbEdges() const;

  //! Returns analyzed wire
  Standard_EXPORT const Handle(ShapeExtend_WireData)& WireData() const;

  //! Returns the recorded status for a vertex
  //! More detail by method Data
  Standard_EXPORT Standard_Integer Status(const Standard_Integer num) const;

  Standard_EXPORT gp_XYZ Position(const Standard_Integer num) const;

  Standard_EXPORT Standard_Real UPrevious(const Standard_Integer num) const;

  Standard_EXPORT Standard_Real UFollowing(const Standard_Integer num) const;

  //! Returns the recorded status for a vertex
  //! With its recorded position and parameters on both edges
  //! These values are relevant regarding the status:
  //! Status  Meaning    Position  Preceding   Following
  //! 0       Same       no        no          no
  //! 1       SameCoord  no        no          no
  //! 2       Close      no        no          no
  //! 3       End        yes       no          yes
  //! 4       Start      yes       yes         no
  //! 5       Inters     yes       yes         yes
  //! -1      Disjoined  no        no          no
  Standard_EXPORT Standard_Integer Data(const Standard_Integer num,
                                        gp_XYZ&                pos,
                                        Standard_Real&         upre,
                                        Standard_Real&         ufol) const;

  //! For a given status, returns the rank of the vertex which
  //! follows <num> and has the same status. 0 if no more
  //! Acts as an iterator, starts on the first one
  Standard_EXPORT Standard_Integer NextStatus(const Standard_Integer stat,
                                              const Standard_Integer num = 0) const;

  //! For a given criter, returns the rank of the vertex which
  //! follows <num> and has the same status. 0 if no more
  //! Acts as an iterator, starts on the first one
  //! Criters are:
  //! 0: same vertex (status 0)
  //! 1: a solution exists (status >= 0)
  //! 2: same coords (i.e. same params) (status 0 1 2)
  //! 3: same coods but not same vertex (status 1 2)
  //! 4: redefined coords (status 3 4 5)
  //! -1: no solution (status -1)
  Standard_EXPORT Standard_Integer NextCriter(const Standard_Integer crit,
                                              const Standard_Integer num = 0) const;

protected:
private:
  Handle(ShapeExtend_WireData)     myWire;
  Handle(TColStd_HArray1OfInteger) myStat;
  Handle(TColgp_HArray1OfXYZ)      myPos;
  Handle(TColStd_HArray1OfReal)    myUPre;
  Handle(TColStd_HArray1OfReal)    myUFol;
  Standard_Real                    myPreci;
  Standard_Boolean                 myDone;
};

#endif // _ShapeAnalysis_WireVertex_HeaderFile
