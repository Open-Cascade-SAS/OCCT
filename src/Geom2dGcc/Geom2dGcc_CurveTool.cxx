// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <Geom2dGcc_CurveTool.ixx>

#include <Geom2d_Line.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Vec.hxx>

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



