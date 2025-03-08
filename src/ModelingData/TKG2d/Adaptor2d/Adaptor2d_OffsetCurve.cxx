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

#include <Adaptor2d_OffsetCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dEvaluator.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_TypeMismatch.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Adaptor2d_OffsetCurve, Adaptor2d_Curve2d)

//=================================================================================================

Adaptor2d_OffsetCurve::Adaptor2d_OffsetCurve()
    : myOffset(0.0),
      myFirst(0.0),
      myLast(0.0)
{
}

//=================================================================================================

Adaptor2d_OffsetCurve::Adaptor2d_OffsetCurve(const Handle(Adaptor2d_Curve2d)& theCurve)
    : myCurve(theCurve),
      myOffset(0.0),
      myFirst(0.0),
      myLast(0.0)
{
}

//=================================================================================================

Adaptor2d_OffsetCurve::Adaptor2d_OffsetCurve(const Handle(Adaptor2d_Curve2d)& theCurve,
                                             const Standard_Real              theOffset)
    : myCurve(theCurve),
      myOffset(theOffset),
      myFirst(theCurve->FirstParameter()),
      myLast(theCurve->LastParameter())
{
}

//=================================================================================================

Adaptor2d_OffsetCurve::Adaptor2d_OffsetCurve(const Handle(Adaptor2d_Curve2d)& theCurve,
                                             const Standard_Real              theOffset,
                                             const Standard_Real              theWFirst,
                                             const Standard_Real              theWLast)
    : myCurve(theCurve),
      myOffset(theOffset),
      myFirst(theWFirst),
      myLast(theWLast)
{
}

//=================================================================================================

Handle(Adaptor2d_Curve2d) Adaptor2d_OffsetCurve::ShallowCopy() const
{
  Handle(Adaptor2d_OffsetCurve) aCopy = new Adaptor2d_OffsetCurve();

  if (!myCurve.IsNull())
  {
    aCopy->myCurve = myCurve->ShallowCopy();
  }
  aCopy->myOffset = myOffset;
  aCopy->myFirst  = myFirst;
  aCopy->myLast   = myLast;

  return aCopy;
}

//=================================================================================================

void Adaptor2d_OffsetCurve::Load(const Handle(Adaptor2d_Curve2d)& C)
{
  myCurve  = C;
  myOffset = 0.;
}

//=================================================================================================

void Adaptor2d_OffsetCurve::Load(const Standard_Real Offset)
{
  myOffset = Offset;
  myFirst  = myCurve->FirstParameter();
  myLast   = myCurve->LastParameter();
}

//=================================================================================================

void Adaptor2d_OffsetCurve::Load(const Standard_Real Offset,
                                 const Standard_Real WFirst,
                                 const Standard_Real WLast)
{
  myOffset = Offset;
  myFirst  = WFirst;
  myLast   = WLast;
}

//=================================================================================================

GeomAbs_Shape Adaptor2d_OffsetCurve::Continuity() const
{
  switch (myCurve->Continuity())
  {
    case GeomAbs_CN:
      return GeomAbs_CN;
    case GeomAbs_C3:
      return GeomAbs_C2;
    case GeomAbs_C2:
      return GeomAbs_G2;
    case GeomAbs_G2:
      return GeomAbs_C1;
    case GeomAbs_C1:
      return GeomAbs_G1;
    case GeomAbs_G1:
      return GeomAbs_C0;
    case GeomAbs_C0:
      // No Continuity !!
      throw Standard_TypeMismatch("Adaptor2d_OffsetCurve::IntervalContinuity");
      break;
  }

  // portage WNT
  return GeomAbs_C0;
}

//=================================================================================================

Standard_Integer Adaptor2d_OffsetCurve::NbIntervals(const GeomAbs_Shape S) const
{
  GeomAbs_Shape Sh;
  if (S >= GeomAbs_C2)
    Sh = GeomAbs_CN;
  else
    Sh = (GeomAbs_Shape)((Standard_Integer)S + 2);

  Standard_Integer nbInter = myCurve->NbIntervals(Sh);

  if (nbInter == 1)
    return nbInter;

  TColStd_Array1OfReal T(1, nbInter + 1);

  myCurve->Intervals(T, Sh);

  Standard_Integer first = 1;
  while (T(first) <= myFirst)
    first++;
  Standard_Integer last = nbInter + 1;
  while (T(last) >= myLast)
    last--;
  return (last - first + 2);
}

//=================================================================================================

void Adaptor2d_OffsetCurve::Intervals(TColStd_Array1OfReal& TI, const GeomAbs_Shape S) const
{
  GeomAbs_Shape Sh;
  if (S >= GeomAbs_C2)
    Sh = GeomAbs_CN;
  else
    Sh = (GeomAbs_Shape)((Standard_Integer)S + 2);

  Standard_Integer nbInter = myCurve->NbIntervals(Sh);

  if (nbInter == 1)
  {
    TI(TI.Lower())     = myFirst;
    TI(TI.Lower() + 1) = myLast;
    return;
  }

  TColStd_Array1OfReal T(1, nbInter + 1);
  myCurve->Intervals(T, Sh);

  Standard_Integer first = 1;
  while (T(first) <= myFirst)
    first++;
  Standard_Integer last = nbInter + 1;
  while (T(last) >= myLast)
    last--;

  Standard_Integer i = TI.Lower(), j;
  for (j = first - 1; j <= last + 1; j++)
  {
    TI(i) = T(j);
    i++;
  }

  TI(TI.Lower())                    = myFirst;
  TI(TI.Lower() + last - first + 2) = myLast;
}

//=================================================================================================

Handle(Adaptor2d_Curve2d) Adaptor2d_OffsetCurve::Trim(const Standard_Real First,
                                                      const Standard_Real Last,
                                                      const Standard_Real) const
{
  Handle(Adaptor2d_OffsetCurve) HO = new Adaptor2d_OffsetCurve(*this);
  HO->Load(myOffset, First, Last);
  return HO;
}

//=================================================================================================

Standard_Boolean Adaptor2d_OffsetCurve::IsClosed() const
{
  if (myOffset == 0.)
  {
    return myCurve->IsClosed();
  }
  else
  {
    if (myCurve->Continuity() == GeomAbs_C0)
      return Standard_False;
    else
    {
      if (myCurve->IsClosed())
      {
        gp_Vec2d Dummy[2];
        gp_Pnt2d P;
        myCurve->D1(myCurve->FirstParameter(), P, Dummy[0]);
        myCurve->D1(myCurve->LastParameter(), P, Dummy[1]);
        if (Dummy[0].IsParallel(Dummy[1], Precision::Angular())
            && !(Dummy[0].IsOpposite(Dummy[1], Precision::Angular())))
          return Standard_True;
        else
          return Standard_False;
      }
      else
        return Standard_False;
    }
  }
}

//=================================================================================================

Standard_Boolean Adaptor2d_OffsetCurve::IsPeriodic() const
{
  return myCurve->IsPeriodic();
}

//=================================================================================================

Standard_Real Adaptor2d_OffsetCurve::Period() const
{
  return myCurve->Period();
}

//=================================================================================================

gp_Pnt2d Adaptor2d_OffsetCurve::Value(const Standard_Real U) const
{
  if (myOffset != 0.)
  {
    gp_Pnt2d aP;
    gp_Vec2d aV;
    myCurve->D1(U, aP, aV);
    Geom2dEvaluator::CalculateD0(aP, aV, myOffset);
    return aP;
  }
  else
  {
    return myCurve->Value(U);
  }
}

//=================================================================================================

void Adaptor2d_OffsetCurve::D0(const Standard_Real U, gp_Pnt2d& P) const
{
  P = Value(U);
}

//=================================================================================================

void Adaptor2d_OffsetCurve::D1(const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V) const
{
  if (myOffset != 0.)
  {
    gp_Vec2d aV2;
    myCurve->D2(U, P, V, aV2);
    Geom2dEvaluator::CalculateD1(P, V, aV2, myOffset);
  }
  else
  {
    myCurve->D1(U, P, V);
  }
}

//=================================================================================================

void Adaptor2d_OffsetCurve::D2(const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const
{
  if (myOffset != 0.)
  {
    gp_Vec2d aV3;
    myCurve->D3(U, P, V1, V2, aV3);
    Geom2dEvaluator::CalculateD2(P, V1, V2, aV3, Standard_False, myOffset);
  }
  else
  {
    myCurve->D2(U, P, V1, V2);
  }
}

//=================================================================================================

void Adaptor2d_OffsetCurve::D3(const Standard_Real U,
                               gp_Pnt2d&           P,
                               gp_Vec2d&           V1,
                               gp_Vec2d&           V2,
                               gp_Vec2d&           V3) const
{
  if (myOffset != 0.)
  {
    gp_Vec2d aV4 = myCurve->DN(U, 4);
    myCurve->D3(U, P, V1, V2, V3);
    Geom2dEvaluator::CalculateD3(P, V1, V2, V3, aV4, Standard_False, myOffset);
  }
  else
  {
    myCurve->D3(U, P, V1, V2, V3);
  }
}

//=================================================================================================

gp_Vec2d Adaptor2d_OffsetCurve::DN(const Standard_Real, const Standard_Integer) const
{
  throw Standard_NotImplemented("Adaptor2d_OffsetCurve::DN");
}

//=================================================================================================

Standard_Real Adaptor2d_OffsetCurve::Resolution(const Standard_Real R3d) const
{
  return Precision::PConfusion(R3d);
}

//=================================================================================================

GeomAbs_CurveType Adaptor2d_OffsetCurve::GetType() const
{

  if (myOffset == 0.)
  {
    return myCurve->GetType();
  }
  else
  {
    switch (myCurve->GetType())
    {

      case GeomAbs_Line:
        return GeomAbs_Line;

      case GeomAbs_Circle:
        return GeomAbs_Circle;

      default:
        return GeomAbs_OffsetCurve;
    }
  }
}

//=================================================================================================

gp_Lin2d Adaptor2d_OffsetCurve::Line() const
{
  if (GetType() == GeomAbs_Line)
  {
    gp_Pnt2d P;
    gp_Vec2d V;
    D1(0, P, V);
    return gp_Lin2d(P, V);
  }
  else
  {
    throw Standard_NoSuchObject("Adaptor2d_OffsetCurve::Line");
  }
}

//=================================================================================================

gp_Circ2d Adaptor2d_OffsetCurve::Circle() const
{
  if (GetType() == GeomAbs_Circle)
  {
    if (myOffset == 0.)
    {
      return myCurve->Circle();
    }
    else
    {
      gp_Circ2d     C1(myCurve->Circle());
      Standard_Real radius = C1.Radius();
      gp_Ax22d      axes(C1.Axis());
      gp_Dir2d      Xd      = axes.XDirection();
      gp_Dir2d      Yd      = axes.YDirection();
      Standard_Real Crossed = Xd.X() * Yd.Y() - Xd.Y() * Yd.X();
      Standard_Real Signe   = (Crossed > 0.) ? 1. : -1.;

      radius += Signe * myOffset;
      if (radius > 0.)
      {
        return gp_Circ2d(axes, radius);
      }
      else if (radius < 0.)
      {
        radius = -radius;
        axes.SetXDirection((axes.XDirection()).Reversed());
        return gp_Circ2d(axes, radius);
      }
      else
      { // Cercle de rayon Nul
        throw Standard_NoSuchObject("Adaptor2d_OffsetCurve::Circle");
      }
    }
  }
  else
  {
    throw Standard_NoSuchObject("Adaptor2d_OffsetCurve::Circle");
  }
}

//=================================================================================================

gp_Elips2d Adaptor2d_OffsetCurve::Ellipse() const
{
  if (myCurve->GetType() == GeomAbs_Ellipse && myOffset == 0.)
  {
    return myCurve->Ellipse();
  }
  else
  {
    throw Standard_NoSuchObject("Adaptor2d_OffsetCurve:Ellipse");
  }
}

//=================================================================================================

gp_Hypr2d Adaptor2d_OffsetCurve::Hyperbola() const
{
  if (myCurve->GetType() == GeomAbs_Hyperbola && myOffset == 0.)
  {
    return myCurve->Hyperbola();
  }
  else
  {
    throw Standard_NoSuchObject("Adaptor2d_OffsetCurve:Hyperbola");
  }
}

//=================================================================================================

gp_Parab2d Adaptor2d_OffsetCurve::Parabola() const
{
  if (myCurve->GetType() == GeomAbs_Parabola && myOffset == 0.)
  {
    return myCurve->Parabola();
  }
  else
  {
    throw Standard_NoSuchObject("Adaptor2d_OffsetCurve:Parabola");
  }
}

//=================================================================================================

Standard_Integer Adaptor2d_OffsetCurve::Degree() const
{
  GeomAbs_CurveType type = myCurve->GetType();
  if ((type == GeomAbs_BezierCurve || type == GeomAbs_BSplineCurve) && myOffset == 0.)
  {
    return myCurve->Degree();
  }
  else
  {
    throw Standard_NoSuchObject("Adaptor2d_OffsetCurve::Degree");
  }
}

//=================================================================================================

Standard_Boolean Adaptor2d_OffsetCurve::IsRational() const
{
  if (myOffset == 0.)
  {
    return myCurve->IsRational();
  }
  return Standard_False;
}

//=================================================================================================

Standard_Integer Adaptor2d_OffsetCurve::NbPoles() const
{
  GeomAbs_CurveType type = myCurve->GetType();
  if ((type == GeomAbs_BezierCurve || type == GeomAbs_BSplineCurve) && myOffset == 0.)
  {
    return myCurve->NbPoles();
  }
  else
  {
    throw Standard_NoSuchObject("Adaptor2d_OffsetCurve::NbPoles");
  }
}

//=================================================================================================

Standard_Integer Adaptor2d_OffsetCurve::NbKnots() const
{
  if (myOffset == 0.)
  {
    return myCurve->NbKnots();
  }
  else
  {
    throw Standard_NoSuchObject("Adaptor2d_OffsetCurve::NbKnots");
  }
}

//=================================================================================================

Handle(Geom2d_BezierCurve) Adaptor2d_OffsetCurve::Bezier() const
{
  Standard_NoSuchObject_Raise_if(myOffset != 0.0e0 || GetType() != GeomAbs_BezierCurve,
                                 "Adaptor2d_OffsetCurve::Bezier() - wrong curve type");
  return myCurve->Bezier();
}

//=================================================================================================

Handle(Geom2d_BSplineCurve) Adaptor2d_OffsetCurve::BSpline() const
{
  Standard_NoSuchObject_Raise_if(myOffset != 0.0e0 || GetType() != GeomAbs_BSplineCurve,
                                 "Adaptor2d_OffsetCurve::BSpline() - wrong curve type");
  return myCurve->BSpline();
}

static Standard_Integer nbPoints(const Handle(Adaptor2d_Curve2d)& theCurve)
{

  Standard_Integer nbs = 20;

  if (theCurve->GetType() == GeomAbs_BezierCurve)
  {
    nbs = Max(nbs, 3 + theCurve->NbPoles());
  }
  else if (theCurve->GetType() == GeomAbs_BSplineCurve)
  {
    nbs = Max(nbs, theCurve->NbKnots() * theCurve->Degree());
  }

  if (nbs > 300)
    nbs = 300;
  return nbs;
}

//=================================================================================================

Standard_Integer Adaptor2d_OffsetCurve::NbSamples() const
{
  return nbPoints(myCurve);
}
