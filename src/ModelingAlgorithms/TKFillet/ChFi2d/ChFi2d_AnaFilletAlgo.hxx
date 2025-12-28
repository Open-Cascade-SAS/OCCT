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
  Standard_EXPORT bool Perform(const double radius);

  //! Retrieves a result (fillet and shrinked neighbours).
  Standard_EXPORT const TopoDS_Edge& Result(TopoDS_Edge& e1, TopoDS_Edge& e2);

private:
  // WW5 method to compute fillet.
  // It returns a constructed fillet definition:
  //     center point (xc, yc)
  //     point on the 1st segment (xstart, ystart)
  //     point on the 2nd segment (xend, yend)
  //     is the arc of fillet clockwise (cw = true) or counterclockwise (cw = false).
  bool SegmentFilletSegment(const double radius,
                            double&      xc,
                            double&      yc,
                            bool&        cw,
                            double&      start,
                            double&      end);

  // A function constructs a fillet between a segment and an arc.
  bool SegmentFilletArc(const double radius,
                        double&      xc,
                        double&      yc,
                        bool&        cw,
                        double&      start,
                        double&      end,
                        double&      xend,
                        double&      yend);

  // A function constructs a fillet between an arc and a segment.
  bool ArcFilletSegment(const double radius,
                        double&      xc,
                        double&      yc,
                        bool&        cw,
                        double&      start,
                        double&      end,
                        double&      xstart,
                        double&      ystart);

  // WW5 method to compute fillet: arc - arc.
  // It returns a constructed fillet definition:
  //     center point (xc, yc)
  //     shrinking parameter of the 1st circle (start)
  //     shrinking parameter of the 2nd circle (end)
  //     if the arc of fillet clockwise (cw = true) or counterclockwise (cw = false).
  bool ArcFilletArc(const double radius,
                    double&      xc,
                    double&      yc,
                    bool&        cw,
                    double&      start,
                    double&      end);

  // Cuts intersecting edges of a contour.
  bool Cut(const gp_Pln& plane, TopoDS_Edge& e1, TopoDS_Edge& e2);

  // Plane.
  gp_Pln plane;

  // Left neighbour.
  TopoDS_Edge e1;
  bool        segment1;
  double      x11;
  double      y11;
  double      x12;
  double      y12;
  double      xc1;
  double      yc1;
  double      radius1;
  bool        cw1;

  // Right neighbour.
  TopoDS_Edge e2;
  bool        segment2;
  double      x21;
  double      y21;
  double      x22;
  double      y22;
  double      xc2;
  double      yc2;
  double      radius2;
  bool        cw2;

  // Fillet (result).
  TopoDS_Edge fillet;
  TopoDS_Edge shrinke1;
  TopoDS_Edge shrinke2;
};

#endif // _ANAFILLETALGO_H_
