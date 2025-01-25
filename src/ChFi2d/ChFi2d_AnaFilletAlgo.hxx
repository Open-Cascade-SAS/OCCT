// Created on: 2013-05-20
// Created by: Vlad ROMASHKO
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef ChFi2d_AnaFilletAlgo_HeaderFile
#define ChFi2d_AnaFilletAlgo_HeaderFile

#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pln.hxx>

//! An analytical algorithm for calculation of the fillets.
//! It is implemented for segments and arcs of circle only.
class ChFi2d_AnaFilletAlgo
{
public:
  //! An empty constructor.
  //! Use the method Init() to initialize the class.
  Standard_EXPORT ChFi2d_AnaFilletAlgo();

  //! A constructor.
  //! It expects a wire consisting of two edges of type (any combination of):
  //! - segment
  //! - arc of circle.
  Standard_EXPORT ChFi2d_AnaFilletAlgo(const TopoDS_Wire& theWire, const gp_Pln& thePlane);

  //! A constructor.
  //! It expects two edges having a common point of type:
  //! - segment
  //! - arc of circle.
  Standard_EXPORT ChFi2d_AnaFilletAlgo(const TopoDS_Edge& theEdge1,
                                       const TopoDS_Edge& theEdge2,
                                       const gp_Pln&      thePlane);

  //! Initializes the class by a wire consisting of two edges.
  Standard_EXPORT void Init(const TopoDS_Wire& theWire, const gp_Pln& thePlane);

  //! Initializes the class by two edges.
  Standard_EXPORT void Init(const TopoDS_Edge& theEdge1,
                            const TopoDS_Edge& theEdge2,
                            const gp_Pln&      thePlane);

  //! Calculates a fillet.
  Standard_EXPORT Standard_Boolean Perform(const Standard_Real radius);

  //! Retrieves a result (fillet and shrinked neighbours).
  Standard_EXPORT const TopoDS_Edge& Result(TopoDS_Edge& e1, TopoDS_Edge& e2);

private:
  // WW5 method to compute fillet.
  // It returns a constructed fillet definition:
  //     center point (xc, yc)
  //     point on the 1st segment (xstart, ystart)
  //     point on the 2nd segment (xend, yend)
  //     is the arc of fillet clockwise (cw = true) or counterclockwise (cw = false).
  Standard_Boolean SegmentFilletSegment(const Standard_Real radius,
                                        Standard_Real&      xc,
                                        Standard_Real&      yc,
                                        Standard_Boolean&   cw,
                                        Standard_Real&      start,
                                        Standard_Real&      end);

  // A function constructs a fillet between a segment and an arc.
  Standard_Boolean SegmentFilletArc(const Standard_Real radius,
                                    Standard_Real&      xc,
                                    Standard_Real&      yc,
                                    Standard_Boolean&   cw,
                                    Standard_Real&      start,
                                    Standard_Real&      end,
                                    Standard_Real&      xend,
                                    Standard_Real&      yend);

  // A function constructs a fillet between an arc and a segment.
  Standard_Boolean ArcFilletSegment(const Standard_Real radius,
                                    Standard_Real&      xc,
                                    Standard_Real&      yc,
                                    Standard_Boolean&   cw,
                                    Standard_Real&      start,
                                    Standard_Real&      end,
                                    Standard_Real&      xstart,
                                    Standard_Real&      ystart);

  // WW5 method to compute fillet: arc - arc.
  // It returns a constructed fillet definition:
  //     center point (xc, yc)
  //     shrinking parameter of the 1st circle (start)
  //     shrinking parameter of the 2nd circle (end)
  //     if the arc of fillet clockwise (cw = true) or counterclockwise (cw = false).
  Standard_Boolean ArcFilletArc(const Standard_Real radius,
                                Standard_Real&      xc,
                                Standard_Real&      yc,
                                Standard_Boolean&   cw,
                                Standard_Real&      start,
                                Standard_Real&      end);

  // Cuts intersecting edges of a contour.
  Standard_Boolean Cut(const gp_Pln& plane, TopoDS_Edge& e1, TopoDS_Edge& e2);

  // Plane.
  gp_Pln plane;

  // Left neighbour.
  TopoDS_Edge      e1;
  Standard_Boolean segment1;
  Standard_Real    x11;
  Standard_Real    y11;
  Standard_Real    x12;
  Standard_Real    y12;
  Standard_Real    xc1;
  Standard_Real    yc1;
  Standard_Real    radius1;
  Standard_Boolean cw1;

  // Right neighbour.
  TopoDS_Edge      e2;
  Standard_Boolean segment2;
  Standard_Real    x21;
  Standard_Real    y21;
  Standard_Real    x22;
  Standard_Real    y22;
  Standard_Real    xc2;
  Standard_Real    yc2;
  Standard_Real    radius2;
  Standard_Boolean cw2;

  // Fillet (result).
  TopoDS_Edge fillet;
  TopoDS_Edge shrinke1;
  TopoDS_Edge shrinke2;
};

#endif // _ANAFILLETALGO_H_
