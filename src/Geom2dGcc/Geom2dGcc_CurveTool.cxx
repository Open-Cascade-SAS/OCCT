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


#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

//Template a respecter
Standard_Real Geom2dGcc_CurveTool::
  EpsX (const Geom2dAdaptor_Curve& C  ,
	const Standard_Real        Tol) {
  return C.Resolution(Tol);
}

Standard_Integer Geom2dGcc_CurveTool::
  NbSamples (const Geom2dAdaptor_Curve& /*C*/) {
  return 20;
}

gp_Pnt2d Geom2dGcc_CurveTool::Value (const Geom2dAdaptor_Curve& C,
				     const Standard_Real        U) {
  return C.Value(U);
}

Standard_Real 
  Geom2dGcc_CurveTool::FirstParameter (const Geom2dAdaptor_Curve& C) {
  return C.FirstParameter();
}

Standard_Real 
  Geom2dGcc_CurveTool::LastParameter (const Geom2dAdaptor_Curve& C) {
  return C.LastParameter();
}

void Geom2dGcc_CurveTool::D1 (const Geom2dAdaptor_Curve& C,
			      const Standard_Real        U,
			            gp_Pnt2d&            P,
			            gp_Vec2d&            T) {

  C.D1(U,P,T);
}

void Geom2dGcc_CurveTool::D2 (const Geom2dAdaptor_Curve& C,
			      const Standard_Real        U,
			            gp_Pnt2d&            P,
			            gp_Vec2d&            T,
			            gp_Vec2d&            N) {

  C.D2(U,P,T,N);
}

void Geom2dGcc_CurveTool::D3 (const Geom2dAdaptor_Curve& C ,
			      const Standard_Real        U ,
			            gp_Pnt2d&            P ,
			            gp_Vec2d&            T ,
			            gp_Vec2d&            N ,
			            gp_Vec2d&            dN) {

  C.D3(U,P,T,N,dN);
}



