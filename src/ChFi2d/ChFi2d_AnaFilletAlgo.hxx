// Created on: 2013-05-20
// Created by: Vlad ROMASHKO
// Copyright (c) 2003-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _ANAFILLETALGO_H_
#define _ANAFILLETALGO_H_

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
  Standard_EXPORT ChFi2d_AnaFilletAlgo(const TopoDS_Wire& theWire, 
                                       const gp_Pln& thePlane);

  //! A constructor.
  //! It expects two edges having a common point of type:
  //! - segment
  //! - arc of circle.
  Standard_EXPORT ChFi2d_AnaFilletAlgo(const TopoDS_Edge& theEdge1, 
                                       const TopoDS_Edge& theEdge2,
                                       const gp_Pln& thePlane);

  //! Initializes the class by a wire consisting of two edges.
  Standard_EXPORT void Init(const TopoDS_Wire& theWire, const gp_Pln& thePlane);

  //! Initializes the class by two edges.
  Standard_EXPORT void Init(const TopoDS_Edge& theEdge1, const TopoDS_Edge& theEdge2, 
                            const gp_Pln& thePlane);

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
                                        Standard_Real& xc, Standard_Real& yc, 
                                        Standard_Boolean& cw,
                                        Standard_Real& start, Standard_Real& end);

  // A function constructs a fillet between a segment and an arc.
  Standard_Boolean SegmentFilletArc(const Standard_Real radius, 
                                    Standard_Real& xc, Standard_Real& yc, 
                                    Standard_Boolean& cw,
                                    Standard_Real& start, Standard_Real& end, 
                                    Standard_Real& xend, Standard_Real& yend);

  // A function constructs a fillet between an arc and a segment.
  Standard_Boolean ArcFilletSegment(const Standard_Real radius, 
                                    Standard_Real& xc, Standard_Real& yc, 
                                    Standard_Boolean& cw,
                                    Standard_Real& start, Standard_Real& end, 
                                    Standard_Real& xstart, Standard_Real& ystart);

  // WW5 method to compute fillet: arc - arc.
  // It returns a constructed fillet definition:
  //     center point (xc, yc)
  //     shrinking parameter of the 1st circle (start)
  //     shrinking parameter of the 2nd circle (end)
  //     if the arc of fillet clockwise (cw = true) or counterclockwise (cw = false).
  Standard_Boolean ArcFilletArc(const Standard_Real radius, 
                                Standard_Real& xc, Standard_Real& yc, 
                                Standard_Boolean& cw,
                                Standard_Real& start, Standard_Real& end);

  // Cuts intersecting edges of a contour.
  Standard_Boolean Cut(const gp_Pln& plane, TopoDS_Edge& e1, TopoDS_Edge& e2);

  // Plane.
  gp_Pln           plane;

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
