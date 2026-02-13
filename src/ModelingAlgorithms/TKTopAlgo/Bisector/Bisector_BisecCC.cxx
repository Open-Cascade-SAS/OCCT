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
#include <Bisector_BisecCC.hxx>
#include <Bisector_BisecPC.hxx>
#include <Bisector_FunctionH.hxx>
#include <Bisector_PointOnBis.hxx>
#include <Bisector_PolyBis.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <math_BissecNewton.hxx>
#include <math_FunctionRoot.hxx>
#include <math_FunctionRoots.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Bisector_BisecCC, Bisector_Curve)

static bool ProjOnCurve(const gp_Pnt2d& P, const occ::handle<Geom2d_Curve>& C, double& theParam);

static double Curvature(const occ::handle<Geom2d_Curve>& C, double U, double Tol);

static bool TestExtension(const occ::handle<Geom2d_Curve>& C1,
                          const occ::handle<Geom2d_Curve>& C2,
                          const int                        Start_End);

static bool DiscretPar(const double DU,
                       const double EpsMin,
                       const double EpsMax,
                       const int    NbMin,
                       const int    NbMax,
                       double&      Eps,
                       int&         Nb);

//=================================================================================================

Bisector_BisecCC::Bisector_BisecCC()
    : sign1(0.0),
      sign2(0.0),
      currentInterval(0),
      shiftParameter(0.0),
      distMax(0.0),
      isEmpty(true),
      isConvex1(false),
      isConvex2(false),
      extensionStart(false),
      extensionEnd(false)
{
}

//=================================================================================================

Bisector_BisecCC::Bisector_BisecCC(const occ::handle<Geom2d_Curve>& Cu1,
                                   const occ::handle<Geom2d_Curve>& Cu2,
                                   const double                     Side1,
                                   const double                     Side2,
                                   const gp_Pnt2d&                  Origin,
                                   const double                     DistMax)
{
  Perform(Cu1, Cu2, Side1, Side2, Origin, DistMax);
}

//=================================================================================================

void Bisector_BisecCC::Perform(const occ::handle<Geom2d_Curve>& Cu1,
                               const occ::handle<Geom2d_Curve>& Cu2,
                               const double                     Side1,
                               const double                     Side2,
                               const gp_Pnt2d&                  Origin,
                               const double                     DistMax)
{
  isEmpty = false;
  distMax = DistMax;

  curve1 = occ::down_cast<Geom2d_Curve>(Cu1->Copy());
  curve2 = occ::down_cast<Geom2d_Curve>(Cu2->Copy());

  sign1 = Side1;
  sign2 = Side2;

  isConvex1 = Bisector::IsConvex(curve1, sign1);
  isConvex2 = Bisector::IsConvex(curve2, sign2);

  double           U, UC1, UC2, Dist, dU, USol;
  gp_Pnt2d         P;
  int              NbPnts    = 21;
  constexpr double EpsMin    = 10 * Precision::Confusion();
  bool             YaPoly    = true;
  bool             OriInPoly = false;
  //---------------------------------------------
  // Calculate first point of the polygon.
  //---------------------------------------------
  bool isProjDone = ProjOnCurve(Origin, curve1, U);

  if (!isProjDone)
  {
    isEmpty = true;
    return;
  }

  P = ValueByInt(U, UC1, UC2, Dist);
  if (Dist < Precision::Confusion())
  {
    gp_Pnt2d aP1   = curve1->Value(UC1);
    gp_Pnt2d aP2   = curve2->Value(UC2);
    double   dp    = (aP1.Distance(P) + aP2.Distance(P));
    double   dorig = (aP1.Distance(Origin) + aP2.Distance(Origin));
    if (dp < dorig)
    {
      isEmpty = true;
      return;
    }
  }

  if (Dist < Precision::Infinite())
  {
    //----------------------------------------------------
    // the parameter of the origin point gives a point
    // on the polygon.
    //----------------------------------------------------
    myPolygon.Append(Bisector_PointOnBis(UC1, UC2, U, Dist, P));
    startIntervals.Append(U);
    if (P.IsEqual(Origin, Precision::Confusion()))
    {
      //----------------------------------------
      // test if the first point is the origin.
      //----------------------------------------
      OriInPoly = true;
    }
  }
  else
  {
    //-------------------------------------------------------
    // The origin point is on the extension.
    // Find the first point of the polygon by dichotomy.
    //-------------------------------------------------------
    dU = (curve1->LastParameter() - U) / (NbPnts - 1);
    U += dU;
    for (int i = 1; i <= NbPnts - 1; i++)
    {
      P = ValueByInt(U, UC1, UC2, Dist);
      if (Dist < Precision::Infinite())
      {
        USol = SearchBound(U - dU, U);
        P    = ValueByInt(USol, UC1, UC2, Dist);
        startIntervals.Append(USol);
        myPolygon.Append(Bisector_PointOnBis(UC1, UC2, USol, Dist, P));
        break;
      }
      U += dU;
    }
  }

  if (myPolygon.Length() != 0)
  {
    SupLastParameter();
    //----------------------------------------------
    // Construction of the polygon of the bissectrice.
    //---------------------------------------------
    U         = FirstParameter();
    double DU = LastParameter() - U;

    if (DU < EpsMin)
    {
      NbPnts = 3;
    }
    dU = DU / (NbPnts - 1);

    U += dU;
    //  modified by NIZHNY-EAP Fri Jan 21 09:33:20 2000 ___BEGIN___
    //  prevent addition of the same point
    gp_Pnt2d prevPnt = P;
    for (int i = 1; i <= NbPnts - 1; i++)
    {
      P = ValueByInt(U, UC1, UC2, Dist);
      if (Dist < Precision::Infinite())
      {
        if (P.Distance(prevPnt) > Precision::Confusion())
          myPolygon.Append(Bisector_PointOnBis(UC1, UC2, U, Dist, P));
      }
      else
      {
        USol = SearchBound(U - dU, U);
        P    = ValueByInt(USol, UC1, UC2, Dist);
        endIntervals.SetValue(1, USol);
        if (P.Distance(prevPnt) > Precision::Confusion())
          myPolygon.Append(Bisector_PointOnBis(UC1, UC2, USol, Dist, P));
        break;
      }
      U += dU;
      prevPnt = P;
      //  modified by NIZHNY-EAP Fri Jan 21 09:33:24 2000 ___END___
    }
  }
  else
  {
    //----------------
    // Empty Polygon.
    //----------------
    YaPoly = false;
  }

  extensionStart = false;
  extensionEnd   = false;
  pointStart     = Origin;

  if (isConvex1 && isConvex2)
  {
    if (YaPoly)
      pointEnd = myPolygon.Last().Point();
  }
  else
  {
    //-----------------------------------------------------------------------------
    // Extension : The curve is extended at the beginning and/or the end if
    //                - one of two curves is concave.
    //                - the curves have a common point at the beginning and/or the end
    //                - the angle of opening at the common point between two curves
    //                  values M_PI.
    // the extension at the beginning  is taken into account if the origin is found above.
    // ie : the origin is not the in the polygon.
    //-----------------------------------------------------------------------------

    //---------------------------------
    // Do the extensions exist ?
    //---------------------------------
    if (OriInPoly)
    {
      extensionStart = false;
    }
    else
    {
      extensionStart = TestExtension(curve1, curve2, 1);
    }
    extensionEnd = TestExtension(curve1, curve2, 2);

    //-----------------
    // Calculate pointEnd.
    //-----------------
    if (extensionEnd)
    {
      pointEnd = curve1->Value(curve1->LastParameter());
    }
    else if (YaPoly)
    {
      pointEnd = myPolygon.Last().Point();
    }
    else
    {
      ComputePointEnd();
    }
    //------------------------------------------------------
    // Update the Limits of intervals of  definition.
    //------------------------------------------------------
    if (YaPoly)
    {
      if (extensionStart)
      {
        gp_Pnt2d P1     = myPolygon.First().Point();
        double   UFirst = startIntervals.First() - pointStart.Distance(P1);
        startIntervals.InsertBefore(1, UFirst);
        endIntervals.InsertBefore(1, startIntervals.Value(2));
      }
      if (extensionEnd)
      {
        gp_Pnt2d P1;
        double   UFirst, ULast;
        P1     = myPolygon.Last().Point();
        UFirst = endIntervals.Last();
        ULast  = UFirst + pointEnd.Distance(P1);
        startIntervals.Append(UFirst);
        endIntervals.Append(ULast);
      }
    }
    else
    {
      //--------------------------------------------------
      // No polygon => the bissectrice is a segment.
      //--------------------------------------------------
      startIntervals.Append(0.);
      endIntervals.Append(pointEnd.Distance(pointStart));
    }
  }
  if (!YaPoly && !extensionStart && !extensionEnd)
    isEmpty = true;
  //  modified by NIZHNY-EAP Mon Jan 17 17:32:40 2000 ___BEGIN___
  if (myPolygon.Length() <= 2)
    isEmpty = true;
  //  modified by NIZHNY-EAP Mon Jan 17 17:32:42 2000 ___END___
}

//=================================================================================================

bool Bisector_BisecCC::IsExtendAtStart() const
{
  return extensionStart;
}

//=================================================================================================

bool Bisector_BisecCC::IsExtendAtEnd() const
{
  return extensionEnd;
}

//=================================================================================================

bool Bisector_BisecCC::IsEmpty() const
{
  return isEmpty;
}

//=================================================================================================

void Bisector_BisecCC::Reverse()
{
  throw Standard_NotImplemented();
}

//=================================================================================================

double Bisector_BisecCC::ReversedParameter(const double U) const
{
  return LastParameter() + FirstParameter() - U;
}

//=================================================================================================

occ::handle<Geom2d_Geometry> Bisector_BisecCC::Copy() const
{
  occ::handle<Geom2d_Curve> CopyCurve1 = occ::down_cast<Geom2d_Curve>(curve1->Copy());
  occ::handle<Geom2d_Curve> CopyCurve2 = occ::down_cast<Geom2d_Curve>(curve2->Copy());

  occ::handle<Bisector_BisecCC> C = new Bisector_BisecCC();

  C->Curve(1, CopyCurve1);
  C->Curve(2, CopyCurve2);
  C->Sign(1, sign1);
  C->Sign(2, sign2);
  C->IsConvex(1, isConvex1);
  C->IsConvex(2, isConvex2);
  C->Polygon(myPolygon);
  C->IsEmpty(isEmpty);
  C->DistMax(distMax);
  C->StartIntervals(startIntervals);
  C->EndIntervals(endIntervals);
  C->ExtensionStart(extensionStart);
  C->ExtensionEnd(extensionEnd);
  C->PointStart(pointStart);
  C->PointEnd(pointEnd);

  return C;
}

//=============================================================================
// function : ChangeGuide
// purpose  : Changet of the guideline for the parameters of the bissectrice
//           ATTENTION : - This can invert the direction  of parameterization.
//                       - This concerns only the part of the curve
//                         corresponding to the polygon.
//=============================================================================
occ::handle<Bisector_BisecCC> Bisector_BisecCC::ChangeGuide() const
{
  occ::handle<Bisector_BisecCC> C = new Bisector_BisecCC();

  C->Curve(1, curve2);
  C->Curve(2, curve1);
  C->Sign(1, sign2);
  C->Sign(2, sign1);
  C->IsConvex(1, isConvex2);
  C->IsConvex(2, isConvex1);

  //-------------------------------------------------------------------------
  // Construction of the new polygon from the initial one.
  // inversion of PointOnBis and Calculation of new parameters on the bissectrice.
  //-------------------------------------------------------------------------
  Bisector_PolyBis Poly;
  if (sign1 == sign2)
  {
    //---------------------------------------------------------------
    // elements of the new polygon are ranked in the other direction.
    //---------------------------------------------------------------
    for (int i = myPolygon.Length(); i >= 1; i--)
    {
      Bisector_PointOnBis P = myPolygon.Value(i);
      Bisector_PointOnBis NewP(P.ParamOnC2(),
                               P.ParamOnC1(),
                               P.ParamOnC2(),
                               P.Distance(),
                               P.Point());
      Poly.Append(NewP);
    }
  }
  else
  {
    for (int i = 1; i <= myPolygon.Length(); i++)
    {
      Bisector_PointOnBis P = myPolygon.Value(i);
      Bisector_PointOnBis NewP(P.ParamOnC2(),
                               P.ParamOnC1(),
                               P.ParamOnC2(),
                               P.Distance(),
                               P.Point());
      Poly.Append(NewP);
    }
  }
  C->Polygon(Poly);
  C->FirstParameter(Poly.First().ParamOnBis());
  C->LastParameter(Poly.Last().ParamOnBis());

  return C;
}

//=================================================================================================

void Bisector_BisecCC::Transform(const gp_Trsf2d& T)
{
  curve1->Transform(T);
  curve2->Transform(T);
  myPolygon.Transform(T);
  pointStart.Transform(T);
  pointEnd.Transform(T);
}

//=================================================================================================

bool Bisector_BisecCC::IsCN(const int N) const
{
  return (curve1->IsCN(N + 1) && curve2->IsCN(N + 1));
}

//=================================================================================================

double Bisector_BisecCC::FirstParameter() const
{
  return startIntervals.First();
}

//=================================================================================================

double Bisector_BisecCC::LastParameter() const
{
  return endIntervals.Last();
}

//=================================================================================================

GeomAbs_Shape Bisector_BisecCC::Continuity() const
{
  GeomAbs_Shape Cont = curve1->Continuity();
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

int Bisector_BisecCC::NbIntervals() const
{
  return startIntervals.Length();
}

//=================================================================================================

double Bisector_BisecCC::IntervalFirst(const int Index) const
{
  return startIntervals.Value(Index);
}

//=================================================================================================

double Bisector_BisecCC::IntervalLast(const int Index) const
{
  return endIntervals.Value(Index);
}

//=================================================================================================

GeomAbs_Shape Bisector_BisecCC::IntervalContinuity() const
{
  GeomAbs_Shape Cont = curve1->Continuity();
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

bool Bisector_BisecCC::IsClosed() const
{
  if (curve1->IsClosed())
  {
    if (startIntervals.First() == curve1->FirstParameter()
        && endIntervals.Last() == curve1->LastParameter())
      return true;
  }
  return false;
}

//=================================================================================================

bool Bisector_BisecCC::IsPeriodic() const
{
  return false;
}

//=================================================================================================

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

//=============================================================================
// function : Value
// purpose  : CALCULATE THE CURRENT POINT BY ITERATIVE METHOD.
//           ----------------------------------------------
//           Calculate the current point, the distance from the current point to
//           both curves, the parameters on each curve of the projection
//           of the current point.
//
// method  : - Find start parameter by using <myPolygon>.
//          - Calculate parameter U2 on curve C2 solution of H(U,V)= 0
//          - P(U) = F(U,U2)
//
//          or :
//                                 ||P2(v0)P1(u)||**2
//           F(u,v) = P1(u) - 1/2 *----------------* N(u)
//                                (N(u).P2(v0)P1(u))
//
//           H(u,v) = (Tu.P1(u)P2(v))**2||Tv||**2 - (Tv.P1(u)P2(v))**2||Tu||**2
//
//=============================================================================
gp_Pnt2d Bisector_BisecCC::ValueAndDist(const double U, double& U1, double& U2, double& Dist) const
{
  gp_Vec2d T;

  //-----------------------------------------------
  // is the polygon reduced to a point or empty?
  //-----------------------------------------------
  if (myPolygon.Length() <= 1)
  {
    return Extension(U, U1, U2, Dist, T);
  }

  //-----------------------------------------------
  // test U out of the limits of the polygon.
  //-----------------------------------------------
  if (U < myPolygon.First().ParamOnBis())
  {
    return Extension(U, U1, U2, Dist, T);
  }
  if (U > myPolygon.Last().ParamOnBis())
  {
    return Extension(U, U1, U2, Dist, T);
  }

  //-------------------------------------------------------
  // Find start parameter by using <myPolygon>.
  //-------------------------------------------------------
  int    IntervalIndex = myPolygon.Interval(U);
  double UMin          = myPolygon.Value(IntervalIndex).ParamOnBis();
  double UMax          = myPolygon.Value(IntervalIndex + 1).ParamOnBis();
  double VMin          = myPolygon.Value(IntervalIndex).ParamOnC2();
  double VMax          = myPolygon.Value(IntervalIndex + 1).ParamOnC2();
  double Alpha, VInit;

  if (std::abs(UMax - UMin) < gp::Resolution())
  {
    VInit = VMin;
  }
  else
  {
    Alpha = (U - UMin) / (UMax - UMin);
    VInit = VMin + Alpha * (VMax - VMin);
  }

  U1           = LinkBisCurve(U);
  double VTemp = std::min(VMin, VMax);
  VMax         = std::max(VMin, VMax);
  VMin         = VTemp;
  bool Valid   = true;
  //---------------------------------------------------------------
  // Calculate parameter U2 on curve C2 solution of H(u,v)=0
  //---------------------------------------------------------------
  gp_Pnt2d P1;
  gp_Vec2d T1;
  double   EpsH    = 1.E-9;
  double   EpsH100 = 1.E-7;
  curve1->D1(U1, P1, T1);
  gp_Vec2d N1(T1.Y(), -T1.X());

  if ((VMax - VMin) < Precision::PConfusion())
  {
    U2 = VInit;
  }
  else
  {
    Bisector_FunctionH H(curve2, P1, sign1 * sign2 * T1);
    double             FInit;
    H.Value(VInit, FInit);
    if (std::abs(FInit) < EpsH)
    {
      U2 = VInit;
    }
    else
    {
      math_BissecNewton aNewSolution(EpsH);
      aNewSolution.Perform(H, VMin - EpsH100, VMax + EpsH100, 10);

      if (aNewSolution.IsDone())
      {
        U2 = aNewSolution.Root();
      }
      else
      {
        math_FunctionRoot SolRoot(H, VInit, EpsH, VMin - EpsH100, VMax + EpsH100);

        if (SolRoot.IsDone())
          U2 = SolRoot.Root();
        else
          Valid = false;
      }
    }
  }

  gp_Pnt2d PBis = pointStart;
  //----------------
  // P(U) = F(U1,U2)
  //----------------
  if (Valid)
  {
    gp_Pnt2d     P2 = curve2->Value(U2);
    gp_Vec2d     P2P1(P1.X() - P2.X(), P1.Y() - P2.Y());
    double       SquareP2P1 = P2P1.SquareMagnitude();
    double       N1P2P1     = N1.Dot(P2P1);
    const double anEps      = Epsilon(1);

    if (P1.IsEqual(P2, Precision::Confusion()))
    {
      PBis = P1;
      Dist = 0.0;
    }
    else if (N1P2P1 * sign1 < anEps)
    {
      Valid = false;
    }
    else
    {
      PBis = P1.Translated(-(0.5 * SquareP2P1 / N1P2P1) * N1);
      Dist = P1.SquareDistance(PBis);
    }
  }

  //----------------------------------------------------------------
  // If the point is not valid
  // calculate by intersection.
  //----------------------------------------------------------------
  if (!Valid)
  {
    //--------------------------------------------------------------------
    // Construction of the bisectrice point curve and of the straight line passing
    // by P1 and carried by the normal. curve2 is limited by VMin and VMax.
    //--------------------------------------------------------------------
    double                        DMin = Precision::Infinite();
    gp_Pnt2d                      P;
    occ::handle<Bisector_BisecPC> BisPC = new Bisector_BisecPC(curve2, P1, sign2, VMin, VMax);
    occ::handle<Geom2d_Line>      NorLi = new Geom2d_Line(P1, N1);

    Geom2dAdaptor_Curve ABisPC(BisPC);
    Geom2dAdaptor_Curve ANorLi(NorLi);
    //-------------------------------------------------------------------------
    Geom2dInt_GInter Intersect(ABisPC, ANorLi, Precision::Confusion(), Precision::Confusion());
    //-------------------------------------------------------------------------

    if (Intersect.IsDone() && !Intersect.IsEmpty())
    {
      for (int i = 1; i <= Intersect.NbPoints(); i++)
      {
        if (Intersect.Point(i).ParamOnSecond() * sign1 < Precision::PConfusion())
        {
          P = Intersect.Point(i).Value();
          if (P.SquareDistance(P1) < DMin)
          {
            DMin = P.SquareDistance(P1);
            PBis = P;
            U2   = BisPC->LinkBisCurve(Intersect.Point(i).ParamOnFirst());
            Dist = DMin;
          }
        }
      }
    }
  }
  return PBis;
}

//=============================================================================
// function : ValueByInt
// purpose  : CALCULATE THE CURRENT POINT BY INTERSECTION.
//           -------------------------------------------
//           Calculate the current point, the distance from the current point
//           to two curves, the parameters on each curve of the projection of the
//           current point.
//           the current point with parameter U is the intersection of the
//           bissectrice point curve (P1,curve2) and of the straight line
//           passing through P1 of director vector N1.
//           P1 is the current point of parameter U on curve1 and N1 the
//           normal at this point.
//=============================================================================
gp_Pnt2d Bisector_BisecCC::ValueByInt(const double U, double& U1, double& U2, double& Dist) const
{
  //------------------------------------------------------------------
  // Return point, tangent, normal on C1 at parameter U.
  //-------------------------------------------------------------------
  U1 = LinkBisCurve(U);

  gp_Pnt2d P1, P2, P, PSol;
  gp_Vec2d Tan1, Tan2;
  curve1->D1(U1, P1, Tan1);
  gp_Vec2d N1(Tan1.Y(), -Tan1.X());

  //--------------------------------------------------------------------------
  // test confusion of P1 with extremity of curve2.
  //--------------------------------------------------------------------------
  if (P1.Distance(curve2->Value(curve2->FirstParameter())) < Precision::Confusion())
  {
    U2 = curve2->FirstParameter();
    curve2->D1(U2, P2, Tan2);
    if (isConvex1 && isConvex2)
    {
      Dist = 0.;
      return P1;
    }
    if (!Tan1.IsParallel(Tan2, Precision::Angular()))
    {
      Dist = 0.;
      return P1;
    }
  }
  if (P1.Distance(curve2->Value(curve2->LastParameter())) < Precision::Confusion())
  {
    U2 = curve2->LastParameter();
    curve2->D1(U2, P2, Tan2);
    if (isConvex1 && isConvex2)
    {
      Dist = 0.;
      return P1;
    }
    if (!Tan1.IsParallel(Tan2, Precision::Angular()))
    {
      Dist = 0.;
      return P1;
    }
  }

  bool   YaSol = false;
  double DMin  = Precision::Infinite();
  double USol;
  double EpsMax = 1.E-6;
  double EpsX;
  double EpsH = 1.E-8;
  double DistPP1;
  int    NbSamples  = 20;
  double UFirstOnC2 = curve2->FirstParameter();
  double ULastOnC2  = curve2->LastParameter();

  if (!myPolygon.IsEmpty())
  {
    if (sign1 == sign2)
    {
      ULastOnC2 = myPolygon.Last().ParamOnC2();
    }
    else
    {
      UFirstOnC2 = myPolygon.Last().ParamOnC2();
    }
  }

  if (std::abs(ULastOnC2 - UFirstOnC2) < Precision::PConfusion() / 100.)
  {
    Dist = Precision::Infinite();
    return P1;
  }

  DiscretPar(std::abs(ULastOnC2 - UFirstOnC2), EpsH, EpsMax, 2, 20, EpsX, NbSamples);

  Bisector_FunctionH H(curve2, P1, sign1 * sign2 * Tan1);
  math_FunctionRoots SolRoot(H, UFirstOnC2, ULastOnC2, NbSamples, EpsX, EpsH, EpsH);
  if (SolRoot.IsDone())
  {
    for (int j = 1; j <= SolRoot.NbSolutions(); j++)
    {
      USol              = SolRoot.Value(j);
      gp_Pnt2d P2Curve2 = curve2->Value(USol);
      gp_Vec2d P2P1(P1.X() - P2Curve2.X(), P1.Y() - P2Curve2.Y());
      double   SquareP2P1 = P2P1.SquareMagnitude();
      double   N1P2P1     = N1.Dot(P2P1);

      // Test if the solution is at the proper side of the curves.
      if (N1P2P1 * sign1 > 0)
      {
        P       = P1.Translated(-(0.5 * SquareP2P1 / N1P2P1) * N1);
        DistPP1 = P1.SquareDistance(P);
        if (DistPP1 < DMin)
        {
          DMin  = DistPP1;
          PSol  = P;
          U2    = USol;
          YaSol = true;
        }
      }
    }
  }

  /*
  if (!YaSol) {
  //--------------------------------------------------------------------
  // Construction de la bisectrice point courbe et de la droite passant
  // par P1 et portee par la normale.
  //--------------------------------------------------------------------
  occ::handle<Bisector_BisecPC> BisPC
  = new Bisector_BisecPC(curve2,P1,sign2,2*distMax);
  //-------------------------------
  // Test si la bissectrice existe.
  //-------------------------------
  if (BisPC->IsEmpty()) {
  Dist = Precision::Infinite();
  PSol = P1;
  return PSol;
  }

  occ::handle<Geom2d_Line>      NorLi  = new Geom2d_Line (P1,N1);
  Geom2dAdaptor_Curve      NorLiAd;
  if (sign1 < 0.) {NorLiAd.Load(NorLi,0.       ,distMax);}
  else            {NorLiAd.Load(NorLi,- distMax,0.     );}

  //-------------------------------------------------------------------------
  Geom2dInt_GInter  Intersect(BisPC,NorLiAd,
  Precision::Confusion(),Precision::Confusion());
  //-------------------------------------------------------------------------
  if (Intersect.IsDone() && !Intersect.IsEmpty()) {
  for (int i = 1; i <= Intersect.NbPoints(); i++) {
  if (Intersect.Point(i).ParamOnSecond()*sign1< Precision::PConfusion()) {
  P       = Intersect.Point(i).Value();
  DistPP1 = P.SquareDistance(P1);
  if (DistPP1  < DMin) {
  DMin  = DistPP1;
  PSol  = P;
  U2   = Intersect.Point(i).ParamOnFirst();
  YaSol = true;
  }
  }
  }
  }
  }
  */

  if (YaSol)
  {
    Dist = DMin;
    //--------------------------------------------------------------
    // Point found => Test curve distance + Angular Test
    //---------------------------------------------------------------
    P2 = curve2->Value(U2);
    gp_Vec2d PP1(P1.X() - PSol.X(), P1.Y() - PSol.Y());
    gp_Vec2d PP2(P2.X() - PSol.X(), P2.Y() - PSol.Y());

    //-----------------------------------------------
    // Dist = product of norms = distance at the square.
    //-----------------------------------------------
    if (PP1.Dot(PP2) > (1. - Precision::Angular()) * Dist)
    {
      YaSol = false;
    }
    else
    {
      if (!isConvex1)
      {
        double K1 = Curvature(curve1, U1, Precision::Confusion());
        if (K1 != 0.)
        {
          if (Dist > 1 / (K1 * K1))
            YaSol = false;
        }
      }
      if (YaSol)
      {
        if (!isConvex2)
        {
          double K2 = Curvature(curve2, U2, Precision::Confusion());
          if (K2 != 0.)
          {
            if (Dist > 1 / (K2 * K2))
              YaSol = false;
          }
        }
      }
    }
  }
  if (!YaSol)
  {
    Dist = Precision::Infinite();
    PSol = P1;
  }
  return PSol;
}

//=================================================================================================

std::optional<gp_Pnt2d> Bisector_BisecCC::EvalD0(const double U) const
{
  double U1, U2, Dist;

  return ValueAndDist(U, U1, U2, Dist);
}

//=================================================================================================

std::optional<Geom2d_Curve::ResD1> Bisector_BisecCC::EvalD1(const double U) const
{
  Geom2d_Curve::ResD1 aResult;
  aResult.D1.SetCoord(0., 0.);
  gp_Vec2d V2, V3;
  Values(U, 1, aResult.Point, aResult.D1, V2, V3);
  return aResult;
}

//=================================================================================================

std::optional<Geom2d_Curve::ResD2> Bisector_BisecCC::EvalD2(const double U) const
{
  Geom2d_Curve::ResD2 aResult;
  aResult.D1.SetCoord(0., 0.);
  aResult.D2.SetCoord(0., 0.);
  gp_Vec2d V3;
  Values(U, 2, aResult.Point, aResult.D1, aResult.D2, V3);
  return aResult;
}

//=================================================================================================

std::optional<Geom2d_Curve::ResD3> Bisector_BisecCC::EvalD3(const double U) const
{
  Geom2d_Curve::ResD3 aResult;
  aResult.D1.SetCoord(0., 0.);
  aResult.D2.SetCoord(0., 0.);
  aResult.D3.SetCoord(0., 0.);
  Values(U, 3, aResult.Point, aResult.D1, aResult.D2, aResult.D3);
  return aResult;
}

//=================================================================================================

std::optional<gp_Vec2d> Bisector_BisecCC::EvalDN(const double U, const int N) const
{
  if (N < 1)
    return std::nullopt;
  gp_Pnt2d P;
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

//=============================================================================
// function : Values
// purpose : the curve can be described by the following equations:
//
//           B(u) = F(u,v0)
//           where v0 = Phi(u) is given by H (u,v) = 0.
//
//           with :
//                                 ||P2(v0)P1(u)||**2
//           F(u,v) = P1(u) - 1/2 *----------------* N(u)
//                                (N(u).P2(v0)P1(u))
//
//           H(u,v) = (Tu.P1(u)P2(v))**2||Tv||**2 - (Tv.P1(u)P2(v))**2||Tu||**2
//
//           => dB(u)/du = dF/du + dF/dv(- dH/du:dH/dv)
//
//           Note : tangent to the bisectrice is bissectrice at
//                      tangents  T1(u) and T2(v0)
//
//=============================================================================
void Bisector_BisecCC::Values(const double U,
                              const int    N,
                              gp_Pnt2d&    P,
                              gp_Vec2d&    V1,
                              gp_Vec2d&    V2,
                              gp_Vec2d&    V3) const
{
  V1 = gp_Vec2d(0., 0.);
  V2 = gp_Vec2d(0., 0.);
  V3 = gp_Vec2d(0., 0.);
  //-------------------------------------------------------------------------
  // Calculate the current point on the bisectrice and the parameters on each
  // curve.
  //-------------------------------------------------------------------------
  double U0, V0, Dist;

  //-----------------------------------------------
  // is the polygon reduced to a point or empty?
  //-----------------------------------------------
  if (myPolygon.Length() <= 1)
  {
    P = Extension(U, U0, V0, Dist, V1);
  }
  if (U < myPolygon.First().ParamOnBis())
  {
    P = Extension(U, U0, V0, Dist, V1);
    return;
  }
  if (U > myPolygon.Last().ParamOnBis())
  {
    P = Extension(U, U0, V0, Dist, V1);
    return;
  }
  P = ValueAndDist(U, U0, V0, Dist);

  if (N == 0)
    return;
  //------------------------------------------------------------------
  // Return point, tangent, normal to C1 by parameter U0.
  //-------------------------------------------------------------------
  gp_Pnt2d P1;  // point on C1.
  gp_Vec2d Tu;  // tangent to C1 by U0.
  gp_Vec2d Tuu; // second derivative to C1 by U0.
  curve1->D2(U0, P1, Tu, Tuu);
  gp_Vec2d Nor(-Tu.Y(), Tu.X());  // Normal by U0.
  gp_Vec2d Nu(-Tuu.Y(), Tuu.X()); // derivative of the normal by U0.

  //-------------------------------------------------------------------
  // Return point, tangent, normale to C2 by parameter V0.
  //-------------------------------------------------------------------
  gp_Pnt2d P2;  // point on C2.
  gp_Vec2d Tv;  // tangent to C2 by V.
  gp_Vec2d Tvv; // second derivative to C2 by V.
  curve2->D2(V0, P2, Tv, Tvv);

  gp_Vec2d PuPv(P2.X() - P1.X(), P2.Y() - P1.Y());

  //-----------------------------
  // Calculate dH/du and dH/dv.
  //-----------------------------
  double TuTu, TvTv, TuTv;
  double TuPuPv, TvPuPv;
  double TuuPuPv, TuTuu;
  double TvvPuPv, TvTvv;

  TuTu    = Tu.Dot(Tu);
  TvTv    = Tv.Dot(Tv);
  TuTv    = Tu.Dot(Tv);
  TuPuPv  = Tu.Dot(PuPv);
  TvPuPv  = Tv.Dot(PuPv);
  TuuPuPv = Tuu.Dot(PuPv);
  TuTuu   = Tu.Dot(Tuu);
  TvvPuPv = Tvv.Dot(PuPv);
  TvTvv   = Tv.Dot(Tvv);

  double dHdu =
    2 * (TuPuPv * (TuuPuPv - TuTu) * TvTv + TvPuPv * TuTv * TuTu - TuTuu * TvPuPv * TvPuPv);
  double dHdv =
    2 * (TuPuPv * TuTv * TvTv + TvTvv * TuPuPv * TuPuPv - TvPuPv * (TvvPuPv + TvTv) * TuTu);

  //-----------------------------
  // Calculate dF/du and dF/dv.
  //-----------------------------
  double NorPuPv, NuPuPv, NorTv;
  double A, B, dAdu, dAdv, dBdu, dBdv, BB;

  NorPuPv = Nor.Dot(PuPv);
  NuPuPv  = Nu.Dot(PuPv);
  NorTv   = Nor.Dot(Tv);

  A    = 0.5 * PuPv.SquareMagnitude();
  B    = -NorPuPv;
  BB   = B * B;
  dAdu = -TuPuPv;
  dBdu = -NuPuPv;
  dAdv = TvPuPv;
  dBdv = -NorTv;

  //---------------------------------------
  // F(u,v) = Pu - (A(u,v)/B(u,v))*Nor(u)
  //----------------------------------------
  if (BB < gp::Resolution())
  {
    V1 = Tu.Normalized() + Tv.Normalized();
    V1 = 0.5 * Tu.SquareMagnitude() * V1;
  }
  else
  {
    gp_Vec2d dFdu = Tu - (dAdu / B - dBdu * A / BB) * Nor - (A / B) * Nu;
    gp_Vec2d dFdv = (-dAdv / B + dBdv * A / BB) * Nor;

    if (std::abs(dHdv) > gp::Resolution())
    {
      V1 = dFdu + dFdv * (-dHdu / dHdv);
    }
    else
    {
      V1 = Tu;
    }
  }
  if (N == 1)
    return;
}

//=============================================================================
// function : Extension
// purpose : Calculate the current point on the  extensions
//           by tangence of the curve.
//============================================================================
gp_Pnt2d Bisector_BisecCC::Extension(const double U,
                                     double&      U1,
                                     double&      U2,
                                     double&      Dist,
                                     gp_Vec2d&    T) const
{
  Bisector_PointOnBis PRef;
  gp_Pnt2d            P, P1, P2, PBis;
  gp_Vec2d            T1, Tang;
  double              dU               = 0.;
  bool                ExtensionTangent = false;

  if (myPolygon.Length() == 0)
  {
    //---------------------------------------------
    // Empty Polygon => segment (pointStart,pointEnd)
    //---------------------------------------------
    dU = U - startIntervals.First();
    P  = pointStart;
    P1 = pointEnd;
    U1 = curve1->LastParameter();
    if (sign1 == sign2)
    {
      U2 = curve2->FirstParameter();
    }
    else
    {
      U2 = curve2->LastParameter();
    }
    Tang.SetCoord(P1.X() - P.X(), P1.Y() - P.Y());
  }
  else if (U < myPolygon.First().ParamOnBis())
  {
    PRef = myPolygon.First();
    P    = PRef.Point();
    dU   = U - PRef.ParamOnBis();
    if (extensionStart)
    {
      //------------------------------------------------------------
      // extension = segment (pointstart, first point of the polygon.)
      //------------------------------------------------------------
      P1 = pointStart;
      U1 = curve1->FirstParameter();
      if (sign1 == sign2)
      {
        U2 = curve2->LastParameter();
      }
      else
      {
        U2 = curve2->FirstParameter();
      }
      Tang.SetCoord(P.X() - P1.X(), P.Y() - P1.Y());
    }
    else
    {
      ExtensionTangent = true;
    }
  }
  else if (U > myPolygon.Last().ParamOnBis())
  {
    PRef = myPolygon.Last();
    P    = PRef.Point();
    dU   = U - PRef.ParamOnBis();
    if (extensionEnd)
    {
      //------------------------------------------------------------
      // extension = segment (last point of the polygon.pointEnd)
      //------------------------------------------------------------
      P1 = pointEnd;
      U1 = curve1->LastParameter();
      if (sign1 == sign2)
      {
        U2 = curve2->LastParameter();
      }
      else
      {
        U2 = curve2->FirstParameter();
      }
      Tang.SetCoord(P1.X() - P.X(), P1.Y() - P.Y());
    }
    else
    {
      ExtensionTangent = true;
    }
  }

  if (ExtensionTangent)
  {
    //-----------------------------------------------------------
    // If the la curve has no a extension, it is extended by tangency
    //------------------------------------------------------------
    U1 = PRef.ParamOnC1();
    U2 = PRef.ParamOnC2();
    P2 = curve2->Value(U2);
    curve1->D1(U1, P1, T1);
    Tang.SetCoord(2 * P.X() - P1.X() - P2.X(), 2 * P.Y() - P1.Y() - P2.Y());
    if (Tang.Magnitude() < Precision::Confusion())
    {
      Tang = T1;
    }
    if (T1.Dot(Tang) < 0.)
      Tang = -Tang;
  }

  T = Tang.Normalized();
  PBis.SetCoord(P.X() + dU * T.X(), P.Y() + dU * T.Y());
  Dist = P1.Distance(PBis);
  return PBis;
}

//=================================================================================================

static bool PointByInt(const occ::handle<Geom2d_Curve>& CA,
                       const occ::handle<Geom2d_Curve>& CB,
                       const double                     SignA,
                       const double                     SignB,
                       const double                     UOnA,
                       double&                          UOnB,
                       double&                          Dist)
{
  //------------------------------------------------------------------
  // Return point,tangent, normal on CA with parameter UOnA.
  //-------------------------------------------------------------------
  gp_Pnt2d P1, P2, P, PSol;
  gp_Vec2d Tan1, Tan2;
  bool     IsConvexA = Bisector::IsConvex(CA, SignA);
  bool     IsConvexB = Bisector::IsConvex(CB, SignB);

  CA->D1(UOnA, P1, Tan1);
  gp_Vec2d N1(Tan1.Y(), -Tan1.X());

  //--------------------------------------------------------------------------
  // test of confusion of P1 with extremity of curve2.
  //--------------------------------------------------------------------------
  if (P1.Distance(CB->Value(CB->FirstParameter())) < Precision::Confusion())
  {
    UOnB = CB->FirstParameter();
    CB->D1(UOnB, P2, Tan2);
    if (IsConvexA && IsConvexB)
    {
      Dist = 0.;
      return true;
    }
    if (!Tan1.IsParallel(Tan2, Precision::Angular()))
    {
      Dist = 0.;
      return false;
    }
  }
  if (P1.Distance(CB->Value(CB->LastParameter())) < Precision::Confusion())
  {
    UOnB = CB->LastParameter();
    CB->D1(UOnB, P2, Tan2);
    if (IsConvexA && IsConvexB)
    {
      Dist = 0.;
      return true;
    }
    if (!Tan1.IsParallel(Tan2, Precision::Angular()))
    {
      Dist = 0.;
      return false;
    }
  }

  double DMin = Precision::Infinite();
  double UPC;
  bool   YaSol = false;
  //--------------------------------------------------------------------
  // Construction of the bisectrice point curve and of the straight line passing
  // through P1 and carried by the normal.
  //--------------------------------------------------------------------
  occ::handle<Bisector_BisecPC> BisPC = new Bisector_BisecPC(CB, P1, SignB);
  //-------------------------------
  // Test if the bissectrice exists.
  //-------------------------------
  if (BisPC->IsEmpty())
  {
    Dist = Precision::Infinite();
    PSol = P1;
    return false;
  }

  occ::handle<Geom2d_Line> NorLi = new Geom2d_Line(P1, N1);

  Geom2dAdaptor_Curve ABisPC(BisPC);
  Geom2dAdaptor_Curve ANorLi(NorLi);
  //-------------------------------------------------------------------------
  Geom2dInt_GInter Intersect(ABisPC, ANorLi, Precision::Confusion(), Precision::Confusion());
  //-------------------------------------------------------------------------

  if (Intersect.IsDone() && !Intersect.IsEmpty())
  {
    for (int i = 1; i <= Intersect.NbPoints(); i++)
    {
      if (Intersect.Point(i).ParamOnSecond() * SignA < Precision::PConfusion())
      {
        P = Intersect.Point(i).Value();
        if (P.SquareDistance(P1) < DMin)
        {
          DMin  = P.SquareDistance(P1);
          PSol  = P;
          UPC   = Intersect.Point(i).ParamOnFirst();
          UOnB  = BisPC->LinkBisCurve(UPC);
          Dist  = DMin;
          YaSol = true;
        }
      }
    }
  }
  if (YaSol)
  {
    //--------------------------------------------------------------
    // Point found => Test distance curvature + Angular test
    //---------------------------------------------------------------
    P2 = CB->Value(UOnB);
    if (P1.SquareDistance(PSol) < 1.e-32)
    {
      YaSol = false;
      return YaSol;
    }
    if (P2.SquareDistance(PSol) < 1.e-32)
    {
      YaSol = false;
      return YaSol;
    }

    gp_Dir2d PP1Unit(P1.X() - PSol.X(), P1.Y() - PSol.Y());
    gp_Dir2d PP2Unit(P2.X() - PSol.X(), P2.Y() - PSol.Y());

    if (PP1Unit * PP2Unit > 1. - Precision::Angular())
    {
      YaSol = false;
    }
    else
    {
      Dist = sqrt(Dist);
      if (!IsConvexA)
      {
        double K1 = Curvature(CA, UOnA, Precision::Confusion());
        if (K1 != 0.)
        {
          if (Dist > std::abs(1 / K1))
            YaSol = false;
        }
      }
      if (YaSol)
      {
        if (!IsConvexB)
        {
          double K2 = Curvature(CB, UOnB, Precision::Confusion());
          if (K2 != 0.)
          {
            if (Dist > std::abs(1 / K2))
              YaSol = false;
          }
        }
      }
    }
  }
  return YaSol;
}

//=================================================================================================

void Bisector_BisecCC::SupLastParameter()
{
  endIntervals.Append(curve1->LastParameter());
  // ----------------------------------------------------------------------
  // Calculate parameter on curve1 associated to one or the other of the extremities
  // of curve2 following the values of sign1 and sign2.
  // the bissectrice is limited by the obtained parameters.
  //------------------------------------------------------------------------
  double UOnC1, UOnC2, Dist;
  if (sign1 == sign2)
  {
    UOnC2 = curve2->FirstParameter();
  }
  else
  {
    UOnC2 = curve2->LastParameter();
  }
  bool YaSol = PointByInt(curve2, curve1, sign2, sign1, UOnC2, UOnC1, Dist);
  if (YaSol)
  {
    if (UOnC1 > startIntervals.First() && UOnC1 < endIntervals.Last())
    {
      endIntervals.SetValue(1, UOnC1);
    }
  }
}

//=================================================================================================

occ::handle<Geom2d_Curve> Bisector_BisecCC::Curve(const int I) const
{
  if (I == 1)
    return curve1;
  else if (I == 2)
    return curve2;
  else
    throw Standard_OutOfRange();
}

//=================================================================================================

double Bisector_BisecCC::LinkBisCurve(const double U) const
{
  return (U - shiftParameter);
}

//=================================================================================================

double Bisector_BisecCC::LinkCurveBis(const double U) const
{
  return (U + shiftParameter);
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

const Bisector_PolyBis& Bisector_BisecCC::Polygon() const
{
  return myPolygon;
}

//=================================================================================================

double Bisector_BisecCC::Parameter(const gp_Pnt2d& P) const
{
  double UOnCurve;

  if (P.IsEqual(Value(FirstParameter()), Precision::Confusion()))
  {
    UOnCurve = FirstParameter();
  }
  else if (P.IsEqual(Value(LastParameter()), Precision::Confusion()))
  {
    UOnCurve = LastParameter();
  }
  else
  {
    ProjOnCurve(P, curve1, UOnCurve);
  }

  return UOnCurve;
}

//=================================================================================================

// void Bisector_BisecCC::Dump(const int Deep,
void Bisector_BisecCC::Dump(const int, const int Offset) const
{
  Indent(Offset);
  std::cout << "Bisector_BisecCC :" << std::endl;
  Indent(Offset);
  //  std::cout <<"Curve1 :"<<curve1<<std::endl;
  //  std::cout <<"Curve2 :"<<curve2<<std::endl;
  std::cout << "Sign1  :" << sign1 << std::endl;
  std::cout << "Sign2  :" << sign2 << std::endl;

  std::cout << "Number Of Intervals :" << startIntervals.Length() << std::endl;
  for (int i = 1; i <= startIntervals.Length(); i++)
  {
    std::cout << "Interval number :" << i << "Start :" << startIntervals.Value(i)
              << "  end :" << endIntervals.Value(i) << std::endl;
  }
  std::cout << "Index Current Interval :" << currentInterval << std::endl;
}

//=================================================================================================

void Bisector_BisecCC::Curve(const int I, const occ::handle<Geom2d_Curve>& C)
{
  if (I == 1)
    curve1 = C;
  else if (I == 2)
    curve2 = C;
  else
    throw Standard_OutOfRange();
}

//=================================================================================================

void Bisector_BisecCC::Sign(const int I, const double S)
{
  if (I == 1)
    sign1 = S;
  else if (I == 2)
    sign2 = S;
  else
    throw Standard_OutOfRange();
}

//=================================================================================================

void Bisector_BisecCC::Polygon(const Bisector_PolyBis& P)
{
  myPolygon = P;
}

//=================================================================================================

void Bisector_BisecCC::DistMax(const double D)
{
  distMax = D;
}

//=================================================================================================

void Bisector_BisecCC::IsConvex(const int I, const bool IsConvex)
{
  if (I == 1)
    isConvex1 = IsConvex;
  else if (I == 2)
    isConvex2 = IsConvex;
  else
    throw Standard_OutOfRange();
}

//=================================================================================================

void Bisector_BisecCC::IsEmpty(const bool IsEmpty)
{
  isEmpty = IsEmpty;
}

//=================================================================================================

void Bisector_BisecCC::ExtensionStart(const bool ExtensionStart)
{
  extensionStart = ExtensionStart;
}

//=================================================================================================

void Bisector_BisecCC::ExtensionEnd(const bool ExtensionEnd)
{
  extensionEnd = ExtensionEnd;
}

//=================================================================================================

void Bisector_BisecCC::PointStart(const gp_Pnt2d& Point)
{
  pointStart = Point;
}

//=================================================================================================

void Bisector_BisecCC::PointEnd(const gp_Pnt2d& Point)
{
  pointEnd = Point;
}

//=================================================================================================

void Bisector_BisecCC::StartIntervals(const NCollection_Sequence<double>& StartIntervals)
{
  startIntervals = StartIntervals;
}

//=================================================================================================

void Bisector_BisecCC::EndIntervals(const NCollection_Sequence<double>& EndIntervals)
{
  endIntervals = EndIntervals;
}

//=================================================================================================

void Bisector_BisecCC::FirstParameter(const double U)
{
  startIntervals.Append(U);
}

//=================================================================================================

void Bisector_BisecCC::LastParameter(const double U)
{
  endIntervals.Append(U);
}

//=================================================================================================

double Bisector_BisecCC::SearchBound(const double U1, const double U2) const
{
  double           UMid, Dist1, Dist2, DistMid, U11, U22;
  double           UC1, UC2;
  gp_Pnt2d         PBis, PBisPrec;
  constexpr double TolPnt = Precision::Confusion();
  constexpr double TolPar = Precision::PConfusion();
  U11                     = U1;
  U22                     = U2;
  PBisPrec                = ValueByInt(U11, UC1, UC2, Dist1);
  PBis                    = ValueByInt(U22, UC1, UC2, Dist2);

  while ((U22 - U11) > TolPar
         || ((Dist1 < Precision::Infinite() && Dist2 < Precision::Infinite()
              && !PBis.IsEqual(PBisPrec, TolPnt))))
  {
    PBisPrec = PBis;
    UMid     = 0.5 * (U22 + U11);
    PBis     = ValueByInt(UMid, UC1, UC2, DistMid);
    if ((Dist1 < Precision::Infinite()) == (DistMid < Precision::Infinite()))
    {
      U11   = UMid;
      Dist1 = DistMid;
    }
    else
    {
      U22   = UMid;
      Dist2 = DistMid;
    }
  }
  PBis = ValueByInt(U11, UC1, UC2, Dist1);
  if (Dist1 < Precision::Infinite())
  {
    UMid = U11;
  }
  else
  {
    UMid = U22;
  }
  return UMid;
}

//=================================================================================================

static bool ProjOnCurve(const gp_Pnt2d& P, const occ::handle<Geom2d_Curve>& C, double& theParam)
{
  // double UOnCurve =0.;
  theParam = 0.0;
  gp_Pnt2d PF, PL;
  gp_Vec2d TF, TL;

  C->D1(C->FirstParameter(), PF, TF);
  C->D1(C->LastParameter(), PL, TL);

  if (P.IsEqual(PF, Precision::Confusion()))
  {
    theParam = C->FirstParameter();
    return true;
  }

  if (P.IsEqual(PL, Precision::Confusion()))
  {
    theParam = C->LastParameter();
    return true;
  }

  gp_Vec2d PPF(PF.X() - P.X(), PF.Y() - P.Y());
  TF.Normalize();

  if (std::abs(PPF.Dot(TF)) < Precision::Confusion())
  {
    theParam = C->FirstParameter();
    return true;
  }
  gp_Vec2d PPL(PL.X() - P.X(), PL.Y() - P.Y());
  TL.Normalize();
  if (std::abs(PPL.Dot(TL)) < Precision::Confusion())
  {
    theParam = C->LastParameter();
    return true;
  }
  Geom2dAPI_ProjectPointOnCurve Proj(P, C, C->FirstParameter(), C->LastParameter());
  if (Proj.NbPoints() > 0)
  {
    theParam = Proj.LowerDistanceParameter();
  }
  else
  {
    return false;
  }

  return true;
}

//=================================================================================================

static bool TestExtension(const occ::handle<Geom2d_Curve>& C1,
                          const occ::handle<Geom2d_Curve>& C2,
                          const int                        Start_End)
{
  gp_Pnt2d P1, P2;
  gp_Vec2d T1, T2;
  bool     Test = false;
  if (Start_End == 1)
  {
    C1->D1(C1->FirstParameter(), P1, T1);
  }
  else
  {
    C1->D1(C1->LastParameter(), P1, T1);
  }
  C2->D1(C2->FirstParameter(), P2, T2);
  if (P1.IsEqual(P2, Precision::Confusion()))
  {
    T1.Normalize();
    T2.Normalize();
    if (T1.Dot(T2) > 1.0 - Precision::Confusion())
    {
      Test = true;
    }
  }
  else
  {
    C2->D1(C2->LastParameter(), P2, T2);
    if (P1.IsEqual(P2, Precision::Confusion()))
    {
      T2.Normalize();
      if (T1.Dot(T2) > 1.0 - Precision::Confusion())
      {
        Test = true;
      }
    }
  }
  return Test;
}

//=================================================================================================

void Bisector_BisecCC::ComputePointEnd()
{
  double U1, U2;
  double KC, RC;
  U1 = curve1->FirstParameter();
  if (sign1 == sign2)
  {
    U2 = curve2->LastParameter();
  }
  else
  {
    U2 = curve2->FirstParameter();
  }
  double K1 = Curvature(curve1, U1, Precision::Confusion());
  double K2 = Curvature(curve2, U2, Precision::Confusion());
  if (!isConvex1 && !isConvex2)
  {
    if (K1 < K2)
    {
      KC = K1;
    }
    else
    {
      KC = K2;
    }
  }
  else if (!isConvex1)
  {
    KC = K1;
  }
  else
  {
    KC = K2;
  }

  gp_Pnt2d PF;
  gp_Vec2d TF;
  curve1->D1(U1, PF, TF);
  TF.Normalize();
  if (KC != 0.)
  {
    RC = std::abs(1 / KC);
  }
  else
  {
    RC = Precision::Infinite();
  }
  pointEnd.SetCoord(PF.X() - sign1 * RC * TF.Y(), PF.Y() + sign1 * RC * TF.X());
}

//=================================================================================================

static bool DiscretPar(const double DU,
                       const double EpsMin,
                       const double EpsMax,
                       const int    NbMin,
                       const int    NbMax,
                       double&      Eps,
                       int&         Nb)
{
  if (DU <= NbMin * EpsMin)
  {
    Eps = DU / (NbMin + 1);
    Nb  = NbMin;
    return false;
  }

  Eps = std::min(EpsMax, DU / NbMax);

  if (Eps < EpsMin)
  {
    Eps = EpsMin;
    Nb  = int(DU / EpsMin);
  }
  else
  {
    Nb = NbMax;
  }

  return true;
}
