// Created on: 1994-05-30
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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



#include <Geom2dAdaptor.ixx>


#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Hypr2d.hxx>


//=======================================================================
//function : MakeCurve
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) Geom2dAdaptor::MakeCurve
       (const Adaptor2d_Curve2d& HC)
{
  Handle(Geom2d_Curve) C2D;
  switch (HC.GetType()) {

  case GeomAbs_Line:
    {
      Handle(Geom2d_Line) GL = new Geom2d_Line(HC.Line());
      C2D = GL;
    }
    break;
    
  case GeomAbs_Circle:
    {
      Handle(Geom2d_Circle) GL = new Geom2d_Circle(HC.Circle());
      C2D = GL;
    }
    break;
    
  case GeomAbs_Ellipse:
    {
      Handle(Geom2d_Ellipse) GL = new Geom2d_Ellipse(HC.Ellipse());
      C2D = GL;
    }
    break;
    
  case GeomAbs_Parabola:
    {
      Handle(Geom2d_Parabola) GL = new Geom2d_Parabola(HC.Parabola());
      C2D = GL;
    }
    break;
    
  case GeomAbs_Hyperbola:
    {
      Handle(Geom2d_Hyperbola) GL = new Geom2d_Hyperbola(HC.Hyperbola());
      C2D = GL;
    }
    break;

  case GeomAbs_BezierCurve:
    {
      C2D = HC.Bezier();
    }
    break;

  case GeomAbs_BSplineCurve:
    {
      C2D = HC.BSpline();
    }
    break;
    
  case GeomAbs_OtherCurve:
    Standard_DomainError::Raise("Geom2dAdaptor::MakeCurve, OtherCurve");

  }

  // trim the curve if necassary.
  if (! C2D.IsNull() &&
      (HC.FirstParameter() != C2D->FirstParameter()) ||
      (HC.LastParameter()  != C2D->LastParameter())) {

    C2D = new Geom2d_TrimmedCurve
      (C2D,HC.FirstParameter(),HC.LastParameter());
  }

  return C2D;
}
