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


#include <Adaptor3d_OffsetCurve.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2dGcc_CurveToolGeo.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_Failure.hxx>

GeomAbs_CurveType Geom2dGcc_CurveToolGeo::
TheType(const Adaptor3d_OffsetCurve& ) {
  return GeomAbs_OtherCurve;
}

gp_Lin2d Geom2dGcc_CurveToolGeo::
Line (const Adaptor3d_OffsetCurve& ) {
  cout << "Not implemented" << endl;
  return gp_Lin2d();
}

gp_Circ2d Geom2dGcc_CurveToolGeo::
Circle (const Adaptor3d_OffsetCurve& ) {
  cout << "Not implemented" << endl;
  return gp_Circ2d();
}

gp_Elips2d Geom2dGcc_CurveToolGeo::
Ellipse (const Adaptor3d_OffsetCurve& ) {
  cout << "Not implemented" << endl;
  return gp_Elips2d();
}

gp_Parab2d Geom2dGcc_CurveToolGeo::
Parabola (const Adaptor3d_OffsetCurve& ) {
  cout << "Not implemented" << endl;
  return gp_Parab2d();
}

gp_Hypr2d Geom2dGcc_CurveToolGeo::
Hyperbola (const Adaptor3d_OffsetCurve& ) {
  cout << "Not implemented" << endl;
  return gp_Hypr2d();
}

Standard_Real 
Geom2dGcc_CurveToolGeo::EpsX (const Adaptor3d_OffsetCurve& /*C*/,
                              const Standard_Real   Tol) {
                                return Tol;
}

Standard_Integer 
Geom2dGcc_CurveToolGeo::NbSamples (const Adaptor3d_OffsetCurve& C) {
  GeomAbs_CurveType typC = C.GetType();
  Standard_Integer nbs = 20;
  if(typC == GeomAbs_Line) 
    nbs = 2;
  else if(typC == GeomAbs_BezierCurve) 
    nbs = 3 + C.Bezier()->NbPoles();
  else if(typC == GeomAbs_BSplineCurve) { 
    Handle(Geom2d_BSplineCurve) BSC = C.BSpline();
    nbs = BSC->NbKnots();
    nbs*= BSC->Degree();
    if(nbs < 2) nbs=2;
  }
  return(nbs);
}

Standard_Real 
Geom2dGcc_CurveToolGeo::FirstParameter (const Adaptor3d_OffsetCurve& C) {
  return C.FirstParameter();
}

Standard_Real 
Geom2dGcc_CurveToolGeo::LastParameter (const Adaptor3d_OffsetCurve& C) {
  return C.LastParameter();
}

gp_Pnt2d 
Geom2dGcc_CurveToolGeo::Value (const Adaptor3d_OffsetCurve& C,
                               const Standard_Real   U) {

                                 return C.Value(U);
}

void Geom2dGcc_CurveToolGeo::D1(const Adaptor3d_OffsetCurve& C,
                                const Standard_Real   U,
                                gp_Pnt2d&       P,
                                gp_Vec2d&       T) {
                                  C.D1(U,P,T);
}

void Geom2dGcc_CurveToolGeo::D2(const Adaptor3d_OffsetCurve& C,
                                const Standard_Real U,
                                gp_Pnt2d&     P,
                                gp_Vec2d&     T,
                                gp_Vec2d&     N) {
                                  C.D2(U,P,T,N);
}

Standard_Boolean Geom2dGcc_CurveToolGeo::
IsComposite (const Adaptor3d_OffsetCurve& ) {
  return Standard_False;
}

Standard_Integer Geom2dGcc_CurveToolGeo::
GetIntervals (const Adaptor3d_OffsetCurve& ) {
  cout << "Not implemented" << endl;
  return 0;
}

void Geom2dGcc_CurveToolGeo::
GetInterval (const Adaptor3d_OffsetCurve&    ,
             const Standard_Integer   ,
             Standard_Real&     ,
             Standard_Real&     ) {
               cout << "Not implemented" << endl;
}

void Geom2dGcc_CurveToolGeo::
SetCurrentInterval (      Adaptor3d_OffsetCurve&   ,
                    const Standard_Integer  ) {
                      cout << "Not implemented" << endl;
}

