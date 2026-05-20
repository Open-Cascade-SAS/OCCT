// Copyright (c) 1996-1999 Matra Datavision
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

#include <Adaptor3d_Curve.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <GeomBndLib_Curve.hxx>
#include <GeomBndLib_Line.hxx>
#include <GeomBndLib_Circle.hxx>
#include <GeomBndLib_Ellipse.hxx>
#include <GeomBndLib_Hyperbola.hxx>
#include <GeomBndLib_Parabola.hxx>

//=================================================================================================

void BndLib_Add3dCurve::Add(const Adaptor3d_Curve& C, const double Tol, Bnd_Box& B)
{
  BndLib_Add3dCurve::Add(C, C.FirstParameter(), C.LastParameter(), Tol, B);
}

//=================================================================================================

void BndLib_Add3dCurve::Add(const Adaptor3d_Curve& C,
                            const double           U1,
                            const double           U2,
                            const double           Tol,
                            Bnd_Box&               B)
{
  // Fast path for elementary curves: use the static Box overloads on
  // gp_* primitives, avoiding the per-call heap allocation of Geom_Line /
  // Geom_Circle / ... that GeomBndLib_Curve's constructor performs.
  switch (C.GetType())
  {
    case GeomAbs_Line:
      B.Add(GeomBndLib_Line::Box(C.Line(), U1, U2, Tol));
      return;
    case GeomAbs_Circle:
      B.Add(GeomBndLib_Circle::Box(C.Circle(), U1, U2, Tol));
      return;
    case GeomAbs_Ellipse:
      B.Add(GeomBndLib_Ellipse::Box(C.Ellipse(), U1, U2, Tol));
      return;
    case GeomAbs_Hyperbola:
      B.Add(GeomBndLib_Hyperbola::Box(C.Hyperbola(), U1, U2, Tol));
      return;
    case GeomAbs_Parabola:
      B.Add(GeomBndLib_Parabola::Box(C.Parabola(), U1, U2, Tol));
      return;
    default:
      break;
  }
  GeomBndLib_Curve(C).Add(U1, U2, Tol, B);
}

//=================================================================================================

void BndLib_Add3dCurve::AddOptimal(const Adaptor3d_Curve& C, const double Tol, Bnd_Box& B)
{
  BndLib_Add3dCurve::AddOptimal(C, C.FirstParameter(), C.LastParameter(), Tol, B);
}

//=================================================================================================

void BndLib_Add3dCurve::AddOptimal(const Adaptor3d_Curve& C,
                                   const double           U1,
                                   const double           U2,
                                   const double           Tol,
                                   Bnd_Box&               B)
{
  GeomBndLib_Curve(C).AddOptimal(U1, U2, Tol, B);
}
