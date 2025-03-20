// Created on: 2013-05-20
// Created by: Mikhail PONIKAROV
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

#ifndef ChFi2d_ChamferAPI_HeaderFile
#define ChFi2d_ChamferAPI_HeaderFile

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
  Standard_EXPORT TopoDS_Edge Result(TopoDS_Edge&        theEdge1,
                                     TopoDS_Edge&        theEdge2,
                                     const Standard_Real theLength1,
                                     const Standard_Real theLength2);

private:
  TopoDS_Edge        myEdge1, myEdge2;
  Handle(Geom_Curve) myCurve1, myCurve2;
  Standard_Real      myStart1, myEnd1, myStart2, myEnd2;
  Standard_Boolean   myCommonStart1, myCommonStart2;
};

#endif // _CHAMFERAPI_H_
