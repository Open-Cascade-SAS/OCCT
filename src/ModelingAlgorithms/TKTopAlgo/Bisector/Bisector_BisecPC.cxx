// Created on: 1994-03-10
// Created by: Yves FRICAUD
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

#include <Bisector.hxx>
#include <Bisector_BisecPC.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Bisector_BisecPC, Bisector_Curve)

//=================================================================================================

Bisector_BisecPC::Bisector_BisecPC()
    : sign(0.0),
      bisInterval(0),
      currentInterval(0),
      shiftParameter(0.0),
      distMax(0.0),
      isEmpty(true),
      isConvex(false),
      extensionStart(false),
      extensionEnd(false)
{
}

//=================================================================================================

Bisector_BisecPC::Bisector_BisecPC(const occ::handle<Geom2d_Curve>& Cu,
                                   const gp_Pnt2d&                  P,
                                   const double                     Side,
                                   const double                     DistMax)
{
  Perform(Cu, P, Side, DistMax);
}

//=================================================================================================

Bisector_BisecPC::Bisector_BisecPC(const occ::handle<Geom2d_Curve>& Cu,
                                   const gp_Pnt2d&                  P,
                                   const double                     Side,
                                   const double                     UMin,
                                   const double                     UMax)

{
  curve = occ::down_cast<Geom2d_Curve>(Cu->Copy());
  point = P;
  sign  = Side;
  startIntervals.Append(UMin);
  endIntervals.Append(UMax);
  bisInterval    = 1;
  extensionStart = false;
  extensionEnd   = false;
  pointStartBis  = Value(UMin);
  pointEndBis    = Value(UMax);
  isConvex       = Bisector::IsConvex(curve, sign);
}

//=================================================================================================

void Bisector_BisecPC::Perform(const occ::handle<Geom2d_Curve>& Cu,
                               const gp_Pnt2d&                  P,
                               const double                     Side,
                               const double                     DistMax)
{
  curve    = occ::down_cast<Geom2d_Curve>(Cu->Copy());
  point    = P;
  distMax  = DistMax;
  sign     = Side;
  isConvex = Bisector::IsConvex(curve, sign);
  //--------------------------------------------
  // Calculate interval of definition.
  //--------------------------------------------
  ComputeIntervals();
  if (isEmpty)
    return;

  //-------------------------
  // Construction extensions.
  //-------------------------
  bisInterval    = 1;
  extensionStart = false;
  extensionEnd   = false;
  pointStartBis  = Value(startIntervals.First());
  pointEndBis    = Value(endIntervals.Last());

  if (!isConvex)
  {
    if (point.IsEqual(curve->Value(curve->FirstParameter()), Precision::Confusion()))
    {
      extensionStart = true;
      double UFirst  = startIntervals.First() - P.Distance(pointStartBis);
      startIntervals.InsertBefore(1, UFirst);
      endIntervals.InsertBefore(1, startIntervals.Value(2));
      bisInterval = 2;
    }
    else if (point.IsEqual(curve->Value(curve->LastParameter()), Precision::Confusion()))
    {
      extensionEnd = true;
      double ULast = endIntervals.Last() + P.Distance(pointEndBis);
      startIntervals.Append(endIntervals.Last());
      endIntervals.Append(ULast);
      bisInterval = 1;
    }
  }
}

//=================================================================================================

bool Bisector_BisecPC::IsExtendAtStart() const
{
  return extensionStart;
}

//=================================================================================================

bool Bisector_BisecPC::IsExtendAtEnd() const
{
  return extensionEnd;
}

//=================================================================================================

void Bisector_BisecPC::Reverse()
{
  throw Standard_NotImplemented();
}

//=================================================================================================

double Bisector_BisecPC::ReversedParameter(const double U) const
{
  return LastParameter() + FirstParameter() - U;
}

//=================================================================================================

occ::handle<Geom2d_Geometry> Bisector_BisecPC::Copy() const
{
  occ::handle<Geom2d_Curve>     CopyC = occ::down_cast<Geom2d_Curve>(curve->Copy());
  occ::handle<Bisector_BisecPC> C     = new Bisector_BisecPC();

  C->Init(CopyC,
          point,
          sign,
          startIntervals,
          endIntervals,
          bisInterval,
          currentInterval,
          shiftParameter,
          distMax,
          isEmpty,
          isConvex,
          extensionStart,
          extensionEnd,
          pointStartBis,
          pointEndBis);
  return C;
}

//=================================================================================================

void Bisector_BisecPC::Transform(const gp_Trsf2d& T)
{
  curve->Transform(T);
  point.Transform(T);
  pointStartBis.Transform(T);
  pointEndBis.Transform(T);
}

//=================================================================================================

bool Bisector_BisecPC::IsCN(const int N) const
{
  return curve->IsCN(N + 1);
}

//=================================================================================================

double Bisector_BisecPC::FirstParameter() const
{
  return startIntervals.First();
}

//=================================================================================================

double Bisector_BisecPC::LastParameter() const
{
  return endIntervals.Last();
}

//=================================================================================================

GeomAbs_Shape Bisector_BisecPC::Continuity() const
{
  GeomAbs_Shape Cont = curve->Continuity();
  switch (Cont)
  {
    case GeomAbs_C1:
      return GeomAbs_C0;
    case GeomAbs_C2:
      return GeomAbs_C1;
    case GeomAbs_C3:
      return GeomAbs_C2;
    case GeomAbs_CN:
      return GeomAbs_CN;
    default:
      break;
  }
  return GeomAbs_C0;
}

//=================================================================================================

int Bisector_BisecPC::NbIntervals() const
{
  return startIntervals.Length();
}

//=================================================================================================

double Bisector_BisecPC::IntervalFirst(const int I) const
{
  return startIntervals.Value(I);
}

//=================================================================================================

double Bisector_BisecPC::IntervalLast(const int I) const
{
  return endIntervals.Value(I);
}

//=================================================================================================

GeomAbs_Shape Bisector_BisecPC::IntervalContinuity() const
{
  GeomAbs_Shape Cont = curve->Continuity();
  switch (Cont)
  {
    case GeomAbs_C1:
      return GeomAbs_C0;
    case GeomAbs_C2:
      return GeomAbs_C1;
    case GeomAbs_C3:
      return GeomAbs_C2;
    case GeomAbs_CN:
      return GeomAbs_CN;
    default:
      break;
  }
  return GeomAbs_C0;
}

//=================================================================================================

bool Bisector_BisecPC::IsClosed() const
{
  if (curve->IsClosed())
  {
    //-----------------------------------------------------------------------
    // The bisectrice is closed if the curve is closed and the bissectrice
    // has only one domain of continuity equal to the one of the curve.
    // -----------------------------------------------------------------------
    if (startIntervals.First() == curve->FirstParameter()
        && endIntervals.First() == curve->LastParameter())
      return true;
  }
  return false;
}

//=================================================================================================

bool Bisector_BisecPC::IsPeriodic() const
{
  return false;
}

//=================================================================================================

void Bisector_BisecPC::Extension(const double U,
                                 gp_Pnt2d&    P,
                                 gp_Vec2d&    V1,
                                 gp_Vec2d&    V2,
                                 gp_Vec2d&    V3) const
{
  double dU;

  V1.SetCoord(0., 0.);
  V2.SetCoord(0., 0.);
  V3.SetCoord(0., 0.);
  if (U < startIntervals.Value(bisInterval))
  {
    if (pointStartBis.IsEqual(point, Precision::PConfusion()))
      P = pointStartBis;
    else
    {
      dU = U - startIntervals.Value(bisInterval);
      gp_Dir2d DirExt(pointStartBis.X() - point.X(), pointStartBis.Y() - point.Y());
      P.SetCoord(pointStartBis.X() + dU * DirExt.X(), pointStartBis.Y() + dU * DirExt.Y());
      V1.SetCoord(DirExt.X(), DirExt.Y());
    }
  }
  else if (U > endIntervals.Value(bisInterval))
  {
    if (pointEndBis.IsEqual(point, Precision::PConfusion()))
      P = pointEndBis;
    else
    {
      dU = U - endIntervals.Value(bisInterval);
      gp_Dir2d DirExt(point.X() - pointEndBis.X(), point.Y() - pointEndBis.Y());
      P.SetCoord(pointEndBis.X() + dU * DirExt.X(), pointEndBis.Y() + dU * DirExt.Y());
      V1.SetCoord(DirExt.X(), DirExt.Y());
    }
  }
}

//=============================================================================
// function : Values
// purpose : To each point of the curve is associated a point on the
//           bissectrice. The equation of the bissectrice is:
//                              || PP(u)||**2
//           F(u) = P(u) - 1/2* -------------- * N(u)
//                              (N(u)|PP(u))
//
//           N(u) normal to the curve by u.
//           ( | ) designation of the scalar product.
//=============================================================================
void Bisector_BisecPC::Values(const double U,
                              const int    N,
                              gp_Pnt2d&    P,
                              gp_Vec2d&    V1,
                              gp_Vec2d&    V2,
                              gp_Vec2d&    V3) const
{
  if (U < startIntervals.Value(bisInterval))
  {
    Extension(U, P, V1, V2, V3);
    return;
  }
  else if (U > endIntervals.Value(bisInterval))
  {
    Extension(U, P, V1, V2, V3);
    return;
  }
  double UOnCurve = LinkBisCurve(U);

  gp_Vec2d Tu, Tuu, T3u;
  gp_Pnt2d PC;

  switch (N)
  {
    case 0: {
      curve->D1(UOnCurve, PC, Tu);
      break;
    }
    case 1: {
      curve->D2(UOnCurve, PC, Tu, Tuu);
      break;
    }
    case 2: {
      curve->D3(UOnCurve, PC, Tu, Tuu, T3u);
      break;
    }
  }

  gp_Vec2d aPPC(PC.X() - point.X(), PC.Y() - point.Y());
  gp_Vec2d Nor(-Tu.Y(), Tu.X());

  double SquarePPC = aPPC.SquareMagnitude();
  double NorPPC    = Nor.Dot(aPPC);
  double A1;

  if (std::abs(NorPPC) > gp::Resolution() && (NorPPC * sign) < 0.)
  {
    A1 = 0.5 * SquarePPC / NorPPC;
    P.SetCoord(PC.X() - Nor.X() * A1, PC.Y() - Nor.Y() * A1);
  }
  else
  {
    return;
  }

  if (N == 0)
    return; // End Calculation Point;

  gp_Vec2d Nu(-Tuu.Y(), Tuu.X()); // derivative of the normal by U.
  double   NuPPC    = Nu.Dot(aPPC);
  double   TuPPC    = Tu.Dot(aPPC);
  double   NorPPCE2 = NorPPC * NorPPC;
  double   A2       = TuPPC / NorPPC - 0.5 * NuPPC * SquarePPC / NorPPCE2;

  //--------------------------
  V1 = Tu - A1 * Nu - A2 * Nor;
  //--------------------------
  if (N == 1)
    return; // End calculation D1.

  gp_Vec2d Nuu(-T3u.Y(), T3u.X());

  double NorPPCE4 = NorPPCE2 * NorPPCE2;
  double NuuPPC   = Nuu.Dot(aPPC);
  double TuuPPC   = Tuu.Dot(aPPC);

  double A21 = TuuPPC / NorPPC - TuPPC * NuPPC / NorPPCE2;
  double A22 = (0.5 * NuuPPC * SquarePPC + NuPPC * TuPPC) / NorPPCE2
               - NuPPC * SquarePPC * NorPPC * NuPPC / NorPPCE4;
  double A2u = A21 - A22;
  //----------------------------------------
  V2 = Tuu - 2 * A2 * Nu - A1 * Nuu - A2u * Nor;
  //----------------------------------------
}

//=================================================================================================

// Unused :
#ifdef OCCT_DEBUG_CUR
static double Curvature(const occ::handle<Geom2d_Curve>& C, double U, double Tol)
{
  double   K1;
  gp_Vec2d D1, D2;
  gp_Pnt2d P;
  C->D2(U, P, D1, D2);
  double Norm2 = D1.SquareMagnitude();
  if (Norm2 < Tol)
  {
    K1 = 0.0;
  }
  else
  {
    K1 = (D1 ^ D2) / (Norm2 * sqrt(Norm2));
  }
  return K1;
}
#endif

//=============================================================================
// function : Distance
// purpose  : distance at the square of the point of parameter U to the curve and at point:
//
//            2             ||PP(u)||**4           2
//           d =   1/4* ------------------- ||Nor||
//                         (Nor(u)/PP(u))**2
//
//           where Nor is the normal to the curve by U.
//=============================================================================
double Bisector_BisecPC::Distance(const double U) const
{
  gp_Vec2d Tan;
  gp_Pnt2d PC;

  double UOnCurve = LinkBisCurve(U);

  curve->D1(UOnCurve, PC, Tan);
  gp_Vec2d aPPC(PC.X() - point.X(), PC.Y() - point.Y());
  gp_Vec2d Nor(-Tan.Y(), Tan.X());

  double NorNor       = Nor.SquareMagnitude();
  double SquareMagPPC = aPPC.SquareMagnitude();
  double Prosca       = Nor.Dot(aPPC);

  if (point.IsEqual(PC, Precision::Confusion()))
  {
    if (isConvex)
    {
      return 0.;
    }
    //----------------------------------------------------
    // the point is on a concave curve.
    // The required point is not the common point.
    // This can avoid the discontinuity of the bisectrice.
    //----------------------------------------------------
    else
    {
      return Precision::Infinite();
    }
  }

  if (std::abs(Prosca) < Precision::Confusion() || (Prosca * sign) > 0.)
  {
    return Precision::Infinite();
  }
  else
  {
    double A    = 0.5 * SquareMagPPC / Prosca;
    double Dist = A * A * NorNor;
#ifdef OCCT_DEBUG_CUR
    //----------------------------------------
    // Test Curvature if the curve is concave.
    //----------------------------------------
    if (!isConvex)
    {
      double K = Curvature(curve, UOnCurve, Precision::Confusion());
      if (K != 0.)
      {
        if (Dist > 1 / (K * K))
        {
          Dist = Precision::Infinite();
        }
      }
    }
#endif
    return Dist;
  }
}

//=================================================================================================

std::optional<gp_Pnt2d> Bisector_BisecPC::EvalD0(const double U) const
{
  gp_Pnt2d P = point;
  gp_Vec2d V1, V2, V3;
  Values(U, 0, P, V1, V2, V3);
  return P;
}

//=================================================================================================

std::optional<Geom2d_CurveD1> Bisector_BisecPC::EvalD1(const double U) const
{
  Geom2d_CurveD1 aResult;
  aResult.Point = point;
  aResult.D1.SetCoord(0., 0.);
  gp_Vec2d V2, V3;
  Values(U, 1, aResult.Point, aResult.D1, V2, V3);
  return aResult;
}

//=================================================================================================

std::optional<Geom2d_CurveD2> Bisector_BisecPC::EvalD2(const double U) const
{
  Geom2d_CurveD2 aResult;
  aResult.Point = point;
  aResult.D1.SetCoord(0., 0.);
  aResult.D2.SetCoord(0., 0.);
  gp_Vec2d V3;
  Values(U, 2, aResult.Point, aResult.D1, aResult.D2, V3);
  return aResult;
}

//=================================================================================================

std::optional<Geom2d_CurveD3> Bisector_BisecPC::EvalD3(const double U) const
{
  Geom2d_CurveD3 aResult;
  aResult.Point = point;
  aResult.D1.SetCoord(0., 0.);
  aResult.D2.SetCoord(0., 0.);
  aResult.D3.SetCoord(0., 0.);
  Values(U, 3, aResult.Point, aResult.D1, aResult.D2, aResult.D3);
  return aResult;
}

//=================================================================================================

std::optional<gp_Vec2d> Bisector_BisecPC::EvalDN(const double U, const int N) const
{
  if (N < 1)
    return std::nullopt;
  gp_Pnt2d P = point;
  gp_Vec2d V1(0., 0.);
  gp_Vec2d V2(0., 0.);
  gp_Vec2d V3(0., 0.);
  Values(U, N, P, V1, V2, V3);
  switch (N)
  {
    case 1:
      return V1;
    case 2:
      return V2;
    case 3:
      return V3;
    default: {
      return std::nullopt;
    }
  }
}

//=================================================================================================

double Bisector_BisecPC::SearchBound(const double U1, const double U2) const
{
  double           Dist1, DistMid, U11, U22;
  double           UMid     = 0.;
  constexpr double Tol      = Precision::PConfusion();
  double           DistMax2 = distMax * distMax;
  U11                       = U1;
  U22                       = U2;
  Dist1                     = Distance(U11);

  while ((U22 - U11) > Tol)
  {
    UMid    = 0.5 * (U22 + U11);
    DistMid = Distance(UMid);
    if ((Dist1 > DistMax2) == (DistMid > DistMax2))
    {
      U11   = UMid;
      Dist1 = DistMid;
    }
    else
    {
      U22 = UMid;
    }
  }
  return UMid;
}

//=================================================================================================

void Bisector_BisecPC::CuspFilter()
{
  throw Standard_NotImplemented();
}

//=================================================================================================

void Bisector_BisecPC::ComputeIntervals()
{
  double U1 = 0., U2 = 0., UProj = 0.;
  double UStart = 0., UEnd = 0.;
  double Dist1, Dist2, DistProj;
  isEmpty         = false;
  shiftParameter  = 0.;
  bool   YaProj   = false;
  double DistMax2 = distMax * distMax;

  U1       = curve->FirstParameter();
  U2       = curve->LastParameter();
  Dist1    = Distance(U1);
  Dist2    = Distance(U2);
  DistProj = Precision::Infinite();

  Geom2dAPI_ProjectPointOnCurve Proj(point, curve, U1, U2);
  if (Proj.NbPoints() > 0)
  {
    UProj    = Proj.LowerDistanceParameter();
    DistProj = Distance(UProj);
    YaProj   = true;
  }

  if (Dist1 < DistMax2 && Dist2 < DistMax2)
  {
    if (DistProj > DistMax2 && YaProj)
    {
      isEmpty = true;
    }
    else
    {
      startIntervals.Append(U1);
      endIntervals.Append(U2);
    }
    return;
  }
  else if (Dist1 > DistMax2 && Dist2 > DistMax2)
  {
    if (DistProj < DistMax2)
    {
      UStart = SearchBound(U1, UProj);
      UEnd   = SearchBound(UProj, U2);
    }
    else
    {
      isEmpty = true;
      return;
    }
  }
  else if (Dist1 < DistMax2)
  {
    UStart = U1;
    UEnd   = SearchBound(U1, U2);
  }
  else if (Dist2 < DistMax2)
  {
    UEnd   = U2;
    UStart = SearchBound(U1, U2);
  }
  startIntervals.Append(UStart);
  endIntervals.Append(UEnd);

  //------------------------------------------------------------------------
  // Eventual offset of the parameter on the curve correspondingly to the one
  // on the curve. The offset can be done if the curve is periodical and the
  // point of initial parameter is less then the interval of continuity.
  //------------------------------------------------------------------------
  if (curve->IsPeriodic())
  {
    if (startIntervals.Length() > 1)
    { // Plusieurs intervals.
      if (endIntervals.Last() == curve->LastParameter()
          && startIntervals.First() == curve->FirstParameter())
      {
        //---------------------------------------------------------------
        // the bissectrice is defined at the origin.
        // => Fusion of the first and the last interval.
        // => 0 on the bisectrice becomes the start of the first interval
        // => offset of parameter on all limits of intervals.
        //---------------------------------------------------------------
        startIntervals.Remove(1);
        endIntervals.Remove(endIntervals.Length());

        shiftParameter = Period() - startIntervals.First();
        for (int k = 1; k <= startIntervals.Length(); k++)
        {
          endIntervals.ChangeValue(k) += shiftParameter;
          startIntervals.ChangeValue(k) += shiftParameter;
        }
        startIntervals.ChangeValue(1) = 0.;
      }
    }
  }
}

//=================================================================================================

double Bisector_BisecPC::LinkBisCurve(const double U) const
{
  return (U - shiftParameter);
}

//=================================================================================================

double Bisector_BisecPC::LinkCurveBis(const double U) const
{
  return (U + shiftParameter);
}

//=================================================================================================

bool Bisector_BisecPC::IsEmpty() const
{
  return isEmpty;
}

//=================================================================================================

double Bisector_BisecPC::Parameter(const gp_Pnt2d& P) const
{
  constexpr double Tol = Precision::Confusion();

  if (P.IsEqual(pointStartBis, Tol))
  {
    return startIntervals.Value(bisInterval);
  }
  if (P.IsEqual(pointEndBis, Tol))
  {
    return endIntervals.Value(bisInterval);
  }

  if (extensionStart)
  {
    gp_Ax2d  Axe(pointStartBis, gp_Dir2d(pointStartBis.X() - P.X(), pointStartBis.Y() - P.Y()));
    double   U    = ElCLib::LineParameter(Axe, P);
    gp_Pnt2d Proj = ElCLib::LineValue(U, Axe);
    if (Proj.IsEqual(P, Tol) && U < 0.)
    {
      return U + startIntervals.Value(bisInterval);
    }
  }
  if (extensionEnd)
  {
    gp_Ax2d  Axe(pointEndBis, gp_Dir2d(P.X() - pointEndBis.X(), P.Y() - pointEndBis.Y()));
    double   U    = ElCLib::LineParameter(Axe, P);
    gp_Pnt2d Proj = ElCLib::LineValue(U, Axe);
    if (Proj.IsEqual(P, Tol) && U > 0.)
    {
      return U + endIntervals.Value(bisInterval);
    }
  }
  double                        UOnCurve = 0.;
  Geom2dAPI_ProjectPointOnCurve Proj(P, curve, curve->FirstParameter(), curve->LastParameter());
  if (Proj.NbPoints() > 0)
  {
    UOnCurve = Proj.LowerDistanceParameter();
  }
  return LinkCurveBis(UOnCurve);
}

//=================================================================================================

static void Indent(const int Offset)
{
  if (Offset > 0)
  {
    for (int i = 0; i < Offset; i++)
    {
      std::cout << " ";
    }
  }
}

//=================================================================================================

void Bisector_BisecPC::Init(const occ::handle<Geom2d_Curve>&    Curve,
                            const gp_Pnt2d&                     Point,
                            const double                        Sign,
                            const NCollection_Sequence<double>& StartIntervals,
                            const NCollection_Sequence<double>& EndIntervals,
                            const int                           BisInterval,
                            const int                           CurrentInterval,
                            const double                        ShiftParameter,
                            const double                        DistMax,
                            const bool                          IsEmpty,
                            const bool                          IsConvex,
                            const bool                          ExtensionStart,
                            const bool                          ExtensionEnd,
                            const gp_Pnt2d&                     PointStartBis,
                            const gp_Pnt2d&                     PointEndBis)
{
  curve           = Curve;
  point           = Point;
  sign            = Sign;
  startIntervals  = StartIntervals;
  endIntervals    = EndIntervals;
  bisInterval     = BisInterval;
  currentInterval = CurrentInterval;
  shiftParameter  = ShiftParameter;
  distMax         = DistMax;
  isEmpty         = IsEmpty;
  isConvex        = IsConvex;
  extensionStart  = ExtensionStart;
  extensionEnd    = ExtensionEnd;
  pointStartBis   = PointStartBis;
  pointEndBis     = PointEndBis;
}

//=================================================================================================

// void Bisector_BisecPC::Dump(const int Deep,
void Bisector_BisecPC::Dump(const int, const int Offset) const
{
  Indent(Offset);
  std::cout << "Bisector_BisecPC :" << std::endl;
  Indent(Offset);
  std::cout << "Point :" << std::endl;
  std::cout << " X = " << point.X() << std::endl;
  std::cout << " Y = " << point.Y() << std::endl;
  std::cout << "Sign  :" << sign << std::endl;
  std::cout << "Number Of Intervals :" << startIntervals.Length() << std::endl;
  for (int i = 1; i <= startIntervals.Length(); i++)
  {
    std::cout << "Interval number :" << i << "Start :" << startIntervals.Value(i)
              << "  end :" << endIntervals.Value(i) << std::endl;
  }
  std::cout << "Index Current Interval :" << currentInterval << std::endl;
}
