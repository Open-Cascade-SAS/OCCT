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

#include <Adaptor2d_Line2d.hxx>

#include <ElCLib.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Adaptor2d_Line2d, Adaptor2d_Curve2d)

//=================================================================================================

Adaptor2d_Line2d::Adaptor2d_Line2d()
    : myUfirst(0.0),
      myUlast(0.0)
{
}

//=================================================================================================

Adaptor2d_Line2d::Adaptor2d_Line2d(const gp_Pnt2d&     P,
                                   const gp_Dir2d&     D,
                                   const Standard_Real UFirst,
                                   const Standard_Real ULast)
    : myUfirst(UFirst),
      myUlast(ULast),
      myAx2d(P, D)
{
}

//=================================================================================================

Handle(Adaptor2d_Curve2d) Adaptor2d_Line2d::ShallowCopy() const
{
  Handle(Adaptor2d_Line2d) aCopy = new Adaptor2d_Line2d();

  aCopy->myUfirst = myUfirst;
  aCopy->myUlast  = myUlast;
  aCopy->myAx2d   = myAx2d;

  return aCopy;
}

//=================================================================================================

void Adaptor2d_Line2d::Load(const gp_Lin2d& L)
{
  myAx2d   = L.Position();
  myUfirst = -Precision::Infinite();
  myUlast  = Precision::Infinite();
}

//=================================================================================================

void Adaptor2d_Line2d::Load(const gp_Lin2d& L, const Standard_Real Fi, const Standard_Real La)
{
  myAx2d   = L.Position();
  myUfirst = Fi;
  myUlast  = La;
}

//=================================================================================================

Standard_Real Adaptor2d_Line2d::FirstParameter() const
{
  return myUfirst;
}

//=================================================================================================

Standard_Real Adaptor2d_Line2d::LastParameter() const
{
  return myUlast;
}

//=================================================================================================

GeomAbs_Shape Adaptor2d_Line2d::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

// Standard_Integer Adaptor2d_Line2d::NbIntervals(const GeomAbs_Shape S) const
Standard_Integer Adaptor2d_Line2d::NbIntervals(const GeomAbs_Shape) const
{
  return 1;
}

//=================================================================================================

void Adaptor2d_Line2d::Intervals(TColStd_Array1OfReal& T,
                                 //			       const GeomAbs_Shape S) const
                                 const GeomAbs_Shape) const
{
  T(T.Lower())     = myUfirst;
  T(T.Lower() + 1) = myUlast;
}

//=================================================================================================

Handle(Adaptor2d_Curve2d) Adaptor2d_Line2d::Trim(const Standard_Real First,
                                                 const Standard_Real Last,
                                                 const Standard_Real) const
{
  Handle(Adaptor2d_Line2d) HL = new Adaptor2d_Line2d();
  HL->Load(gp_Lin2d(myAx2d), First, Last);
  return HL;
}

//=================================================================================================

Standard_Boolean Adaptor2d_Line2d::IsClosed() const
{
  return Standard_False;
}

//=================================================================================================

Standard_Boolean Adaptor2d_Line2d::IsPeriodic() const
{
  return Standard_False;
}

//=================================================================================================

Standard_Real Adaptor2d_Line2d::Period() const
{
  throw Standard_NoSuchObject();
}

//=================================================================================================

gp_Pnt2d Adaptor2d_Line2d::Value(const Standard_Real X) const
{
  return ElCLib::LineValue(X, myAx2d);
}

//=================================================================================================

void Adaptor2d_Line2d::D0(const Standard_Real X, gp_Pnt2d& P) const
{
  P = ElCLib::LineValue(X, myAx2d);
}

//=================================================================================================

void Adaptor2d_Line2d::D1(const Standard_Real X, gp_Pnt2d& P, gp_Vec2d& V) const
{
  ElCLib::LineD1(X, myAx2d, P, V);
}

//=================================================================================================

void Adaptor2d_Line2d::D2(const Standard_Real X, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const
{
  ElCLib::LineD1(X, myAx2d, P, V1);
  V2.SetCoord(0., 0.);
}

//=================================================================================================

void Adaptor2d_Line2d::D3(const Standard_Real X,
                          gp_Pnt2d&           P,
                          gp_Vec2d&           V1,
                          gp_Vec2d&           V2,
                          gp_Vec2d&           V3) const
{
  ElCLib::LineD1(X, myAx2d, P, V1);
  V2.SetCoord(0., 0.);
  V3.SetCoord(0., 0.);
}

//=================================================================================================

// gp_Vec2d Adaptor2d_Line2d::DN(const Standard_Real U, const Standard_Integer N) const
gp_Vec2d Adaptor2d_Line2d::DN(const Standard_Real, const Standard_Integer N) const
{
  if (N <= 0)
  {
    throw Standard_OutOfRange();
  }
  if (N == 1)
  {
    return myAx2d.Direction();
  }
  return gp_Vec2d(0., 0.);
}

//=================================================================================================

Standard_Real Adaptor2d_Line2d::Resolution(const Standard_Real R3d) const
{
  return R3d; // nul !!!!
}

//=================================================================================================

GeomAbs_CurveType Adaptor2d_Line2d::GetType() const
{
  return GeomAbs_Line;
}

//=================================================================================================

gp_Lin2d Adaptor2d_Line2d::Line() const
{
  return gp_Lin2d(myAx2d);
}

//=================================================================================================

gp_Circ2d Adaptor2d_Line2d::Circle() const
{
  throw Standard_NoSuchObject();
}

//=================================================================================================

gp_Elips2d Adaptor2d_Line2d::Ellipse() const
{
  throw Standard_NoSuchObject();
}

//=================================================================================================

gp_Hypr2d Adaptor2d_Line2d::Hyperbola() const
{
  throw Standard_NoSuchObject();
}

//=================================================================================================

gp_Parab2d Adaptor2d_Line2d::Parabola() const
{
  throw Standard_NoSuchObject();
}

//=================================================================================================

Standard_Integer Adaptor2d_Line2d::Degree() const
{
  throw Standard_NoSuchObject();
}

//=================================================================================================

Standard_Boolean Adaptor2d_Line2d::IsRational() const
{
  throw Standard_NoSuchObject();
}

//=================================================================================================

Standard_Integer Adaptor2d_Line2d::NbPoles() const
{
  throw Standard_NoSuchObject();
}

//=================================================================================================

Standard_Integer Adaptor2d_Line2d::NbKnots() const
{
  throw Standard_NoSuchObject();
}

//=================================================================================================

Handle(Geom2d_BezierCurve) Adaptor2d_Line2d::Bezier() const
{
  throw Standard_NoSuchObject();
}

//=================================================================================================

Handle(Geom2d_BSplineCurve) Adaptor2d_Line2d::BSpline() const
{
  throw Standard_NoSuchObject();
}
