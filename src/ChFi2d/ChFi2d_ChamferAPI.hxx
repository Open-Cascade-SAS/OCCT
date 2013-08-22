// Created on: 2013-05-20
// Created by: Mikhail PONIKAROV
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

#ifndef _CHAMFERAPI_H_
#define _CHAMFERAPI_H_

#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <Geom_Curve.hxx>

//! A class making a chamfer between two linear edges.
class ChFi2d_ChamferAPI
{
public:

  //! An empty constructor.
  Standard_EXPORT ChFi2d_ChamferAPI();

  //! A constructor accepting a wire consisting of two linear edges.
  Standard_EXPORT ChFi2d_ChamferAPI(const TopoDS_Wire& theWire);

  //! A constructor accepting two linear edges.
  Standard_EXPORT ChFi2d_ChamferAPI(const TopoDS_Edge& theEdge1, const TopoDS_Edge& theEdge2);

  //! Initializes the class by a wire consisting of two libear edges.
  Standard_EXPORT void Init(const TopoDS_Wire& theWire);

  //! Initializes the class by two linear edges.
  Standard_EXPORT void Init(const TopoDS_Edge& theEdge1, const TopoDS_Edge& theEdge2);

  //! Constructs a chamfer edge.
  //! Returns true if the edge is constructed.
  Standard_EXPORT Standard_Boolean Perform();

  // Returns the result (chamfer edge, modified edge1, modified edge2).
  Standard_EXPORT TopoDS_Edge Result(TopoDS_Edge& theEdge1, TopoDS_Edge& theEdge2,
                                     const Standard_Real theLength1, const Standard_Real theLength2);

private:

  TopoDS_Edge myEdge1, myEdge2;
  Handle(Geom_Curve) myCurve1, myCurve2;
  Standard_Real myStart1, myEnd1, myStart2, myEnd2;
  Standard_Boolean myCommonStart1, myCommonStart2;
};

#endif // _CHAMFERAPI_H_