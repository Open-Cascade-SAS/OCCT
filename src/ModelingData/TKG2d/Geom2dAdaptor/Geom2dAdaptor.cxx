// Created on: 1994-05-30
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Adaptor2d_Curve2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor.hxx>
#include <gp_Hypr2d.hxx>

//=================================================================================================

Handle(Geom2d_Curve) Geom2dAdaptor::MakeCurve(const Adaptor2d_Curve2d& HC)
{
  Handle(Geom2d_Curve) C2D;
  switch (HC.GetType())
  {

    case GeomAbs_Line: {
      Handle(Geom2d_Line) GL = new Geom2d_Line(HC.Line());
      C2D                    = GL;
    }
    break;

    case GeomAbs_Circle: {
      Handle(Geom2d_Circle) GL = new Geom2d_Circle(HC.Circle());
      C2D                      = GL;
    }
    break;

    case GeomAbs_Ellipse: {
      Handle(Geom2d_Ellipse) GL = new Geom2d_Ellipse(HC.Ellipse());
      C2D                       = GL;
    }
    break;

    case GeomAbs_Parabola: {
      Handle(Geom2d_Parabola) GL = new Geom2d_Parabola(HC.Parabola());
      C2D                        = GL;
    }
    break;

    case GeomAbs_Hyperbola: {
      Handle(Geom2d_Hyperbola) GL = new Geom2d_Hyperbola(HC.Hyperbola());
      C2D                         = GL;
    }
    break;

    case GeomAbs_BezierCurve: {
      C2D = HC.Bezier();
    }
    break;

    case GeomAbs_BSplineCurve: {
      C2D = HC.BSpline();
    }
    break;

    case GeomAbs_OffsetCurve: {
      const Geom2dAdaptor_Curve* pGAC = dynamic_cast<const Geom2dAdaptor_Curve*>(&HC);
      if (pGAC != 0)
      {
        C2D = pGAC->Curve();
      }
      else
      {
        Standard_DomainError::Raise("Geom2dAdaptor::MakeCurve, Not Geom2dAdaptor_Curve");
      }
    }
    break;

    default:
      throw Standard_DomainError("Geom2dAdaptor::MakeCurve, OtherCurve");
  }

  // trim the curve if necassary.
  if (!C2D.IsNull()
      && ((HC.FirstParameter() != C2D->FirstParameter())
          || (HC.LastParameter() != C2D->LastParameter())))
  {

    if (C2D->IsPeriodic()
        || (HC.FirstParameter() >= C2D->FirstParameter()
            && HC.LastParameter() <= C2D->LastParameter()))
    {
      C2D = new Geom2d_TrimmedCurve(C2D, HC.FirstParameter(), HC.LastParameter());
    }
    else
    {
      Standard_Real tf = Max(HC.FirstParameter(), C2D->FirstParameter());
      Standard_Real tl = Min(HC.LastParameter(), C2D->LastParameter());
      C2D              = new Geom2d_TrimmedCurve(C2D, tf, tl);
    }
  }

  return C2D;
}
