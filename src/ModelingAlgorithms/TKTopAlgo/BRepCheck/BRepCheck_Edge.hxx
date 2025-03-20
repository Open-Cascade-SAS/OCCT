// Created on: 1995-12-11
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepCheck_Edge_HeaderFile
#define _BRepCheck_Edge_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Status.hxx>

class BRep_CurveRepresentation;
class TopoDS_Edge;
class TopoDS_Shape;

class BRepCheck_Edge;
DEFINE_STANDARD_HANDLE(BRepCheck_Edge, BRepCheck_Result)

class BRepCheck_Edge : public BRepCheck_Result
{

public:
  Standard_EXPORT BRepCheck_Edge(const TopoDS_Edge& E);

  Standard_EXPORT void InContext(const TopoDS_Shape& ContextShape) Standard_OVERRIDE;

  Standard_EXPORT void Minimum() Standard_OVERRIDE;

  Standard_EXPORT void Blind() Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean GeometricControls() const;

  Standard_EXPORT void GeometricControls(const Standard_Boolean B);

  Standard_EXPORT Standard_Real Tolerance();

  //! Sets status of Edge;
  Standard_EXPORT void SetStatus(const BRepCheck_Status theStatus);

  //! Sets method to calculate distance: Calculating in finite number of points (if theIsExact
  //! is false, faster, but possible not correct result) or exact calculating by using
  //! BRepLib_CheckCurveOnSurface class (if theIsExact is true, slowly, but more correctly).
  //! Exact method is used only when edge is SameParameter.
  //! Default method is calculating in finite number of points
  void SetExactMethod(Standard_Boolean theIsExact) { myIsExactMethod = theIsExact; }

  //! Returns true if exact method selected
  Standard_Boolean IsExactMethod() { return myIsExactMethod; }

  //! Checks, if polygon on triangulation of heEdge
  //! is out of 3D-curve of this edge.
  Standard_EXPORT BRepCheck_Status CheckPolygonOnTriangulation(const TopoDS_Edge& theEdge);

  DEFINE_STANDARD_RTTIEXT(BRepCheck_Edge, BRepCheck_Result)

private:
  Handle(BRep_CurveRepresentation) myCref;
  Handle(Adaptor3d_Curve)          myHCurve;
  Standard_Boolean                 myGctrl;
  Standard_Boolean                 myIsExactMethod;
};

#endif // _BRepCheck_Edge_HeaderFile
