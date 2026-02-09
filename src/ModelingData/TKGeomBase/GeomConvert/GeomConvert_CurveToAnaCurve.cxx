// Created: 2001-05-21
//
// Copyright (c) 2001-2013 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS,
// furnished in accordance with the terms and conditions of the contract
// and with the inclusion of this copyright notice.
// This file or any part thereof may not be provided or otherwise
// made available to any third party.
//
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.

#include <ElCLib.hxx>
#include <gce_MakeCirc.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <GeomConvert_CurveToAnaCurve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <GeomAbs_CurveType.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>
#include <math_Gauss.hxx>

#include <array>

GeomConvert_CurveToAnaCurve::GeomConvert_CurveToAnaCurve()
    : myGap(Precision::Infinite()),
      myConvType(GeomConvert_MinGap),
      myTarget(GeomAbs_Line)
{
}

GeomConvert_CurveToAnaCurve::GeomConvert_CurveToAnaCurve(const occ::handle<Geom_Curve>& C)
    : myGap(Precision::Infinite()),
      myConvType(GeomConvert_MinGap),
      myTarget(GeomAbs_Line)
{
  myCurve = C;
}

void GeomConvert_CurveToAnaCurve::Init(const occ::handle<Geom_Curve>& C)
{
  myCurve = C;
  myGap   = Precision::Infinite();
}

//=================================================================================================

bool GeomConvert_CurveToAnaCurve::ConvertToAnalytical(const double             tol,
                                                      occ::handle<Geom_Curve>& theResultCurve,
                                                      const double             F,
                                                      const double             L,
                                                      double&                  NewF,
                                                      double&                  NewL)
{
  if (myCurve.IsNull())
    return false;

  occ::handle<Geom_Curve> aCurve = myCurve;
  while (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    occ::handle<Geom_TrimmedCurve> aTrimmed = occ::down_cast<Geom_TrimmedCurve>(aCurve);
    aCurve                                  = aTrimmed->BasisCurve();
  }

  occ::handle<Geom_Curve> C =
    ComputeCurve(aCurve, tol, F, L, NewF, NewL, myGap, myConvType, myTarget);

  if (C.IsNull())
    return false;
  theResultCurve = C;
  return true;
}

//=================================================================================================

bool GeomConvert_CurveToAnaCurve::IsLinear(const NCollection_Array1<gp_Pnt>& aPoles,
                                           const double                      tolerance,
                                           double&                           Deviation)
{
  int nbPoles = aPoles.Length();
  if (nbPoles < 2)
    return false;

  double dMax  = 0;
  int    iMax1 = 0, iMax2 = 0;

  int i;
  for (i = 1; i < nbPoles; i++)
    for (int j = i + 1; j <= nbPoles; j++)
    {
      double dist = aPoles(i).SquareDistance(aPoles(j));
      if (dist > dMax)
      {
        dMax  = dist;
        iMax1 = i;
        iMax2 = j;
      }
    }

  if (dMax < Precision::SquareConfusion())
    return false;

  double tol2 = tolerance * tolerance;
  gp_Vec avec(aPoles(iMax1), aPoles(iMax2));
  gp_Dir adir(avec);
  gp_Lin alin(aPoles(iMax1), adir);

  double aMax = 0.;
  for (i = 1; i <= nbPoles; i++)
  {
    double dist = alin.SquareDistance(aPoles(i));
    if (dist > tol2)
      return false;
    if (dist > aMax)
      aMax = dist;
  }
  Deviation = sqrt(aMax);

  return true;
}

//=================================================================================================

gp_Lin GeomConvert_CurveToAnaCurve::GetLine(const gp_Pnt& P1,
                                            const gp_Pnt& P2,
                                            double&       cf,
                                            double&       cl)
{
  gp_Vec avec(P1, P2);
  gp_Dir adir(avec);
  gp_Lin alin(P1, adir);
  cf = ElCLib::Parameter(alin, P1);
  cl = ElCLib::Parameter(alin, P2);
  return alin;
}

//=================================================================================================

occ::handle<Geom_Line> GeomConvert_CurveToAnaCurve::ComputeLine(
  const occ::handle<Geom_Curve>& curve,
  const double                   tolerance,
  const double                   c1,
  const double                   c2,
  double&                        cf,
  double&                        cl,
  double&                        Deviation)
{
  occ::handle<Geom_Line> line;
  if (curve.IsNull())
    return line;
  line = occ::down_cast<Geom_Line>(curve); // who knows
  if (!line.IsNull())
  {
    cf        = c1;
    cl        = c2;
    Deviation = 0.;
    return line;
  }

  gp_Pnt P1 = curve->Value(c1);
  gp_Pnt P2 = curve->Value(c2);
  if (P1.SquareDistance(P2) < Precision::SquareConfusion())
    return line;
  cf = c1;
  cl = c2;

  occ::handle<NCollection_HArray1<gp_Pnt>> Poles;
  int                                      nbPoles;
  occ::handle<Geom_BSplineCurve>           bsc = occ::down_cast<Geom_BSplineCurve>(curve);
  if (!bsc.IsNull())
  {
    nbPoles = bsc->NbPoles();
    Poles   = new NCollection_HArray1<gp_Pnt>(bsc->Poles());
  }
  else
  {
    occ::handle<Geom_BezierCurve> bzc = occ::down_cast<Geom_BezierCurve>(curve);
    if (!bzc.IsNull())
    {
      nbPoles = bzc->NbPoles();
      Poles   = new NCollection_HArray1<gp_Pnt>(bzc->Poles());
    }
    else
    {
      nbPoles   = 23;
      Poles     = new NCollection_HArray1<gp_Pnt>(1, nbPoles);
      double dt = (c2 - c1) / (nbPoles - 1);
      Poles->SetValue(1, P1);
      Poles->SetValue(nbPoles, P2);
      int i;
      for (i = 2; i < nbPoles; ++i)
      {
        Poles->SetValue(i, curve->Value(c1 + (i - 1) * dt));
      }
    }
  }
  if (!IsLinear(Poles->Array1(), tolerance, Deviation))
    return line; // non
  gp_Lin alin = GetLine(P1, P2, cf, cl);
  line        = new Geom_Line(alin);
  return line;
}

//=================================================================================================

bool GeomConvert_CurveToAnaCurve::GetCircle(gp_Circ&      crc,
                                            const gp_Pnt& P0,
                                            const gp_Pnt& P1,
                                            const gp_Pnt& P2)
{
  //  Control if points are not aligned (should be done by MakeCirc
  double aMaxCoord = std::sqrt(Precision::Infinite());
  if (std::abs(P0.X()) > aMaxCoord || std::abs(P0.Y()) > aMaxCoord || std::abs(P0.Z()) > aMaxCoord)
    return false;
  if (std::abs(P1.X()) > aMaxCoord || std::abs(P1.Y()) > aMaxCoord || std::abs(P1.Z()) > aMaxCoord)
    return false;
  if (std::abs(P2.X()) > aMaxCoord || std::abs(P2.Y()) > aMaxCoord || std::abs(P2.Z()) > aMaxCoord)
    return false;

  //  Building the circle
  gce_MakeCirc mkc(P0, P1, P2);
  if (!mkc.IsDone())
    return false;
  crc = mkc.Value();
  if (crc.Radius() < gp::Resolution())
    return false;
  //  Recalage sur P0
  gp_Pnt PC  = crc.Location();
  gp_Ax2 axe = crc.Position();
  gp_Vec VX(PC, P0);
  axe.SetXDirection(VX);
  crc.SetPosition(axe);
  return true;
}

//=================================================================================================

occ::handle<Geom_Curve> GeomConvert_CurveToAnaCurve::ComputeCircle(
  const occ::handle<Geom_Curve>& c3d,
  const double                   tol,
  const double                   c1,
  const double                   c2,
  double&                        cf,
  double&                        cl,
  double&                        Deviation)
{
  if (c3d->IsKind(STANDARD_TYPE(Geom_Circle)))
  {
    cf                             = c1;
    cl                             = c2;
    Deviation                      = 0.;
    occ::handle<Geom_Circle> aCirc = occ::down_cast<Geom_Circle>(c3d);
    return aCirc;
  }

  occ::handle<Geom_Circle> circ;
  gp_Pnt                   P0, P1, P2;
  double                   ca = (c1 + c1 + c2) / 3;
  double                   cb = (c1 + c2 + c2) / 3;
  P0                          = c3d->Value(c1);
  P1                          = c3d->Value(ca);
  P2                          = c3d->Value(cb);

  gp_Circ crc;
  if (!GetCircle(crc, P0, P1, P2))
    return circ;

  //  Reste a controler que c est bien un cercle : prendre 20 points
  double du = (c2 - c1) / 20;
  int    i;
  double aMax = 0.;
  for (i = 0; i <= 20; i++)
  {
    double u    = c1 + (du * i);
    gp_Pnt PP   = c3d->Value(u);
    double dist = crc.Distance(PP);
    if (dist > tol)
      return circ; // not done
    if (dist > aMax)
      aMax = dist;
  }
  Deviation = aMax;

  // defining the parameters
  double PI2 = 2 * M_PI;

  cf = ElCLib::Parameter(crc, c3d->Value(c1));
  cf = ElCLib::InPeriod(cf, 0., PI2);

  // first parameter should be closed to zero

  if (std::abs(cf) < Precision::PConfusion() || std::abs(PI2 - cf) < Precision::PConfusion())
    cf = 0.;

  double cm = ElCLib::Parameter(crc, c3d->Value((c1 + c2) / 2.));
  cm        = ElCLib::InPeriod(cm, cf, cf + PI2);

  cl = ElCLib::Parameter(crc, c3d->Value(c2));
  cl = ElCLib::InPeriod(cl, cm, cm + PI2);

  circ = new Geom_Circle(crc);
  return circ;
}

//=======================================================================
//              Compute Ellipse
//=======================================================================

//=================================================================================================

static bool IsArrayPntPlanar(const occ::handle<NCollection_HArray1<gp_Pnt>>& HAP,
                             gp_Dir&                                         Norm,
                             const double                                    prec)
{
  int size = HAP->Length();
  if (size < 3)
    return false;
  gp_Pnt P1    = HAP->Value(1);
  gp_Pnt P2    = HAP->Value(2);
  gp_Pnt P3    = HAP->Value(3);
  double dist1 = P1.Distance(P2);
  double dist2 = P1.Distance(P3);
  if (dist1 < prec || dist2 < prec)
    return false;
  gp_Vec V1(P1, P2);
  gp_Vec V2(P1, P3);
  if (V1.IsParallel(V2, prec))
    return false;
  gp_Vec NV = V1.Crossed(V2);

  int i;
  for (i = 1; i <= 3; ++i)
  {
    if (Precision::IsInfinite(NV.Coord(i)))
      return false;
  }

  if (NV.Magnitude() < gp::Resolution())
    return false;

  if (size > 3)
  {
    for (i = 4; i <= size; i++)
    {
      gp_Pnt PN = HAP->Value(i);
      dist1     = P1.Distance(PN);
      if (dist1 < prec || Precision::IsInfinite(dist1))
      {
        return false;
      }
      gp_Vec VN(P1, PN);
      if (!NV.IsNormal(VN, prec))
        return false;
    }
  }
  Norm = NV;
  return true;
}

//=================================================================================================

static bool ConicDefinition(const double a,
                            const double b1,
                            const double c,
                            const double d1,
                            const double e1,
                            const double f,
                            const bool   IsParab,
                            const bool   IsEllip,
                            gp_Pnt&      Center,
                            gp_Dir&      MainAxis,
                            double&      Rmin,
                            double&      Rmax)
{
  double Xcen = 0., Ycen = 0., Xax = 0., Yax = 0.;
  double b, d, e;
  //  conic : a*x2 + 2*b*x*y + c*y2 + 2*d*x + 2*e*y + f = 0.
  // Equation (a,b,c,d,e,f);
  b = b1 / 2.;
  d = d1 / 2.;
  e = e1 / 2.; // chgt de variable

  double eps = 1.E-08; // ?? comme ComputedForm

  if (IsParab)
  {
  }
  else
  {
    //   -> Conique a centre, cas general
    //  On utilise les Determinants des matrices :
    //               | a b d |
    //  gdet (3x3) = | b c e |  et pdet (2X2) = | a b |
    //               | d e f |                  | b c |

    double gdet = a * c * f + 2 * b * d * e - c * d * d - a * e * e - b * b * f;
    double pdet = a * c - b * b;

    Xcen = (b * e - c * d) / pdet;
    Ycen = (b * d - a * e) / pdet;

    double term1 = a - c;
    double term2 = 2 * b;
    double cos2t;
    double auxil;

    if (std::abs(term2) <= eps && std::abs(term1) <= eps)
    {
      cos2t = 1.;
      auxil = 0.;
    }
    else
    {
      if (std::abs(term1) < eps)
      {
        return false;
      }
      double t2d = term2 / term1; // skl 21.11.2001
      cos2t      = 1. / sqrt(1 + t2d * t2d);
      auxil      = sqrt(term1 * term1 + term2 * term2);
    }

    double cost = sqrt((1 + cos2t) / 2.);
    double sint = sqrt((1 - cos2t) / 2.);

    double aprim = (a + c + auxil) / 2.;
    double cprim = (a + c - auxil) / 2.;

    if (std::abs(aprim) < gp::Resolution() || std::abs(cprim) < gp::Resolution())
      return false;

    term1 = -gdet / (aprim * pdet);
    term2 = -gdet / (cprim * pdet);

    if (IsEllip)
    {
      if (term1 <= eps || term2 <= eps)
        return false;
      Xax  = cost;
      Yax  = sint;
      Rmin = sqrt(term1);
      Rmax = sqrt(term2);
      if (Rmax < Rmin)
      {
        Rmax = sqrt(term1);
        Rmin = sqrt(term2);
      }
    }
    else if (term1 <= eps)
    {
      if (-term1 <= eps || term2 <= eps)
        return false;
      Xax  = -sint;
      Yax  = cost;
      Rmin = sqrt(-term1);
      Rmax = sqrt(term2);
    }
    else
    {
      if (term1 <= eps || -term2 <= eps)
        return false;
      Xax  = cost;
      Yax  = sint;
      Rmin = sqrt(-term2);
      Rmax = sqrt(term1);
    }
  }
  Center.SetCoord(Xcen, Ycen, 0.);
  MainAxis.SetCoord(Xax, Yax, 0.);
  return true;
}

//=================================================================================================

occ::handle<Geom_Curve> GeomConvert_CurveToAnaCurve::ComputeEllipse(
  const occ::handle<Geom_Curve>& c3d,
  const double                   tol,
  const double                   c1,
  const double                   c2,
  double&                        cf,
  double&                        cl,
  double&                        Deviation)
{
  if (c3d->IsKind(STANDARD_TYPE(Geom_Ellipse)))
  {
    cf                                = c1;
    cl                                = c2;
    Deviation                         = 0.;
    occ::handle<Geom_Ellipse> anElips = occ::down_cast<Geom_Ellipse>(c3d);
    return anElips;
  }

  occ::handle<Geom_Curve> res;
  constexpr double        prec = Precision::PConfusion();

  double AF, BF, CF, DF, EF, Q1, Q2, Q3, c2n;
  int    i;

  gp_Pnt PStart = c3d->Value(c1);
  gp_Pnt PEnd   = c3d->Value(c2);

  const bool IsClos = PStart.Distance(PEnd) < prec;
  if (IsClos)
  {
    c2n = c2 - (c2 - c1) / 5;
  }
  else
    c2n = c2;
  //
  gp_XYZ                                   aBC;
  occ::handle<NCollection_HArray1<gp_Pnt>> AP = new NCollection_HArray1<gp_Pnt>(1, 5);
  AP->SetValue(1, PStart);
  aBC += PStart.XYZ();
  double dc = (c2n - c1) / 4;
  for (i = 1; i < 5; i++)
  {
    gp_Pnt aP = c3d->Value(c1 + dc * i);
    AP->SetValue(i + 1, aP);
    aBC += aP.XYZ();
  }
  aBC /= 5;
  aBC *= -1;
  gp_Vec aTrans(aBC);
  for (i = 1; i <= 5; ++i)
  {
    AP->ChangeValue(i).Translate(aTrans);
  }
  gp_Dir ndir;
  if (!IsArrayPntPlanar(AP, ndir, prec))
    return res;

  if (std::abs(ndir.X()) < gp::Resolution() && std::abs(ndir.Y()) < gp::Resolution()
      && std::abs(ndir.Z()) < gp::Resolution())
    return res;

  gp_Ax3  AX(gp_Pnt(0, 0, 0), ndir);
  gp_Trsf Tr;
  Tr.SetTransformation(AX);
  gp_Trsf Tr2 = Tr.Inverted();

  math_Matrix Dt(1, 5, 1, 5);
  math_Vector F(1, 5), Sl(1, 5);

  double XN, YN, ZN = 0.;
  gp_Pnt PT, PP;
  for (i = 1; i <= 5; i++)
  {
    PT = AP->Value(i).Transformed(Tr);
    PT.Coord(XN, YN, ZN);
    Dt(i, 1) = XN * XN;
    Dt(i, 2) = XN * YN;
    Dt(i, 3) = YN * YN;
    Dt(i, 4) = XN;
    Dt(i, 5) = YN;
    F(i)     = -1.;
  }

  math_Gauss aSolver(Dt);
  if (!aSolver.IsDone())
    return res;

  aSolver.Solve(F, Sl);

  AF = Sl(1);
  BF = Sl(2);
  CF = Sl(3);
  DF = Sl(4);
  EF = Sl(5);

  Q1 = AF * CF + BF * EF * DF / 4 - CF * DF * DF / 4 - BF * BF / 4 - AF * EF * EF / 4;
  Q2 = AF * CF - BF * BF / 4;
  Q3 = AF + CF;

  double Rmax, Rmin;
  gp_Pnt Center;
  gp_Dir MainAxis;
  bool   IsParab = false, IsEllip = false;

  if (Q2 > 0 && Q1 * Q3 < 0)
  {
    // ellipse
    IsEllip = true;
    if (ConicDefinition(AF, BF, CF, DF, EF, 1., IsParab, IsEllip, Center, MainAxis, Rmin, Rmax))
    {
      // create ellipse
      if (Rmax - Rmin < Precision::Confusion())
      {
        return res; // really it is circle, which must be recognized in other method
      }
      aTrans *= -1;
      Center.SetZ(ZN);
      gp_Pnt NewCenter = Center.Transformed(Tr2);
      gp_Pnt Ptmp(Center.X() + MainAxis.X() * 10,
                  Center.Y() + MainAxis.Y() * 10,
                  Center.Z() + MainAxis.Z() * 10);
      gp_Pnt NewPtmp = Ptmp.Transformed(Tr2);
      gp_Dir NewMainAxis(NewPtmp.X() - NewCenter.X(),
                         NewPtmp.Y() - NewCenter.Y(),
                         NewPtmp.Z() - NewCenter.Z());
      gp_Ax2 ax2(NewCenter, ndir, NewMainAxis);

      gp_Elips anEllipse(ax2, Rmax, Rmin);
      anEllipse.Translate(aTrans);
      occ::handle<Geom_Ellipse> gell = new Geom_Ellipse(anEllipse);

      // test for 20 points
      double param2 = 0;
      dc            = (c2 - c1) / 20;
      for (i = 1; i <= 20; i++)
      {
        PP          = c3d->Value(c1 + i * dc);
        double aPar = ElCLib::Parameter(anEllipse, PP);
        double dist = gell->Value(aPar).Distance(PP);
        if (dist > tol)
          return res; // not done
        if (dist > param2)
          param2 = dist;
      }

      Deviation = param2;

      double PI2 = 2 * M_PI;
      cf         = ElCLib::Parameter(anEllipse, c3d->Value(c1));
      cf         = ElCLib::InPeriod(cf, 0., PI2);

      // first parameter should be closed to zero

      if (std::abs(cf) < Precision::PConfusion() || std::abs(PI2 - cf) < Precision::PConfusion())
        cf = 0.;

      double cm = ElCLib::Parameter(anEllipse, c3d->Value((c1 + c2) / 2.));
      cm        = ElCLib::InPeriod(cm, cf, cf + PI2);

      cl = ElCLib::Parameter(anEllipse, c3d->Value(c2));
      cl = ElCLib::InPeriod(cl, cm, cm + PI2);

      res = gell;
    }
  }
  /*
  if (Q2 < 0 && Q1 != 0) {
    // hyberbola
  }

  if (Q2 == 0 && Q1 != 0) {
    // parabola
  }
  */
  return res;
}

//=================================================================================================

occ::handle<Geom_Curve> GeomConvert_CurveToAnaCurve::ComputeCurve(
  const occ::handle<Geom_Curve>& theC3d,
  const double                   tolerance,
  const double                   c1,
  const double                   c2,
  double&                        cf,
  double&                        cl,
  double&                        theGap,
  const GeomConvert_ConvType     theConvType,
  const GeomAbs_CurveType        theTarget)
{
  cf                                            = c1;
  cl                                            = c2;
  std::array<occ::handle<Geom_Curve>, 3> newc3d = {};
  occ::handle<Geom_Curve>                c3d    = theC3d;
  if (c3d.IsNull())
    return c3d;
  gp_Pnt                P1 = c3d->Value(c1);
  gp_Pnt                P2 = c3d->Value(c2);
  gp_Pnt                P3 = c3d->Value(c1 + (c2 - c1) / 2);
  std::array<double, 3> d  = {RealLast(), RealLast(), RealLast()};
  std::array<double, 3> fp = {};
  std::array<double, 3> lp = {};

  if (c3d->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    occ::handle<Geom_TrimmedCurve> aTc = occ::down_cast<Geom_TrimmedCurve>(c3d);
    c3d                                = aTc->BasisCurve();
  }

  if (theConvType == GeomConvert_Target)
  {
    theGap = RealLast();
    if (theTarget == GeomAbs_Line)
    {
      newc3d[0] = ComputeLine(c3d, tolerance, c1, c2, fp[0], lp[0], theGap);
      cf        = fp[0];
      cl        = lp[0];
      return newc3d[0];
    }
    if (theTarget == GeomAbs_Circle)
    {
      newc3d[1] = ComputeCircle(c3d, tolerance, c1, c2, fp[1], lp[1], theGap);
      cf        = fp[1];
      cl        = lp[1];
      return newc3d[1];
    }
    if (theTarget == GeomAbs_Ellipse)
    {
      newc3d[2] = ComputeEllipse(c3d, tolerance, c1, c2, fp[2], lp[2], theGap);
      cf        = fp[2];
      cl        = lp[2];
      return newc3d[2];
    }
  }
  //
  if (theConvType == GeomConvert_Simplest)
  {
    theGap    = RealLast();
    newc3d[0] = ComputeLine(c3d, tolerance, c1, c2, fp[0], lp[0], theGap);
    if (!newc3d[0].IsNull())
    {
      cf = fp[0];
      cl = lp[0];
      return newc3d[0];
    }
    theGap    = RealLast();
    newc3d[1] = ComputeCircle(c3d, tolerance, c1, c2, fp[1], lp[1], theGap);
    if (!newc3d[1].IsNull())
    {
      cf = fp[1];
      cl = lp[1];
      return newc3d[1];
    }
    theGap    = RealLast();
    newc3d[2] = ComputeEllipse(c3d, tolerance, c1, c2, fp[2], lp[2], theGap);
    if (!newc3d[2].IsNull())
    {
      cf = fp[2];
      cl = lp[2];
      return newc3d[2];
    }
    // Conversion failed, returns null curve
    return newc3d[0];
  }

  //  theConvType == GeomConvert_MinGap
  // recognition in case of small curve
  int imin = -1;
  if ((P1.Distance(P2) < 2 * tolerance) && (P1.Distance(P3) < 2 * tolerance))
  {
    newc3d[1] = ComputeCircle(c3d, tolerance, c1, c2, fp[1], lp[1], d[1]);
    newc3d[0] = ComputeLine(c3d, tolerance, c1, c2, fp[0], lp[0], d[0]);
    imin      = 1;
    if (newc3d[1].IsNull() || d[0] < d[1])
    {
      imin = 0;
    }
  }
  else
  {
    d[0]       = RealLast();
    newc3d[0]  = ComputeLine(c3d, tolerance, c1, c2, fp[0], lp[0], d[0]);
    double tol = std::min(tolerance, d[0]);
    if (!Precision::IsInfinite(c1) && !Precision::IsInfinite(c2))
    {
      d[1]      = RealLast();
      newc3d[1] = ComputeCircle(c3d, tol, c1, c2, fp[1], lp[1], d[1]);
      tol       = std::min(tol, d[1]);
      d[2]      = RealLast();
      newc3d[2] = ComputeEllipse(c3d, tol, c1, c2, fp[2], lp[2], d[2]);
    }
    double dd = RealLast();
    for (int i = 0; i < 3; ++i)
    {
      if (newc3d[i].IsNull())
        continue;
      if (d[i] < dd)
      {
        dd   = d[i];
        imin = i;
      }
    }
  }

  if (imin >= 0)
  {
    cf     = fp[imin];
    cl     = lp[imin];
    theGap = d[imin];
    return newc3d[imin];
  }
  else
  {
    cf     = c1;
    cl     = c2;
    theGap = -1.;
    return newc3d[0]; // must be null curve;
  }
}
