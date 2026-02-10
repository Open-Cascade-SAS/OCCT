// Created by: Bruno DUMORTIER
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

#include <ProjLib_ComputeApproxOnPolarSurface.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <Precision.hxx>
#include <Approx_FitAndDivide2d.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_List.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor.hxx>
#include <GeomAdaptor_Surface.hxx>

#include <gp_Vec2d.hxx>
#include <Extrema_GenLocateExtPS.hxx>
#include <Extrema_ExtPS.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <Standard_DomainError.hxx>
// #include <GeomLib_IsIso.hxx>
// #include <GeomLib_CheckSameParameter.hxx>

#ifdef OCCT_DEBUG
// static int compteur = 0;
#endif

struct aFuncStruct
{
  aFuncStruct() // Empty constructor.
      : mySqProjOrtTol(0.0),
        myTolU(0.0),
        myTolV(0.0)
  {
    memset(myPeriod, 0, sizeof(myPeriod));
  }

  occ::handle<Adaptor3d_Surface> mySurf;         // Surface where to project.
  occ::handle<Adaptor3d_Curve>   myCurve;        // Curve to project.
  occ::handle<Adaptor2d_Curve2d> myInitCurve2d;  // Initial 2dcurve projection.
  double                         mySqProjOrtTol; // Used to filter non-orthogonal projected point.
  double                         myTolU;
  double                         myTolV;
  double                         myPeriod[2]; // U and V period correspondingly.
};

//=======================================================================
// function : computePeriodicity
// purpose  : Compute period information on adaptor.
//=======================================================================
static void computePeriodicity(const occ::handle<Adaptor3d_Surface>& theSurf,
                               double&                               theUPeriod,
                               double&                               theVPeriod)
{
  theUPeriod = 0.0;
  theVPeriod = 0.0;

  // Compute once information about periodicity.
  // Param space may be reduced in case of rectangular trimmed surface,
  // in this case really trimmed bounds should be set as unperiodic.
  double                    aTrimF, aTrimL, aBaseF, aBaseL, aDummyF, aDummyL;
  occ::handle<Geom_Surface> aS = GeomAdaptor::MakeSurface(*theSurf, false); // Not trim.
  // U param space.
  if (theSurf->IsUPeriodic())
  {
    theUPeriod = theSurf->UPeriod();
  }
  else if (theSurf->IsUClosed())
  {
    theUPeriod = theSurf->LastUParameter() - theSurf->FirstUParameter();
  }
  if (theUPeriod != 0.0)
  {
    aTrimF = theSurf->FirstUParameter();          // Trimmed first
    aTrimL = theSurf->LastUParameter();           // Trimmed last
    aS->Bounds(aBaseF, aBaseL, aDummyF, aDummyL); // Non-trimmed values.
    if (std::abs(aBaseF - aTrimF) + std::abs(aBaseL - aTrimL) > Precision::PConfusion())
    {
      // Param space reduced.
      theUPeriod = 0.0;
    }
  }

  // V param space.
  if (theSurf->IsVPeriodic())
  {
    theVPeriod = theSurf->VPeriod();
  }
  else if (theSurf->IsVClosed())
  {
    theVPeriod = theSurf->LastVParameter() - theSurf->FirstVParameter();
  }
  if (theVPeriod != 0.0)
  {
    aTrimF = theSurf->FirstVParameter();          // Trimmed first
    aTrimL = theSurf->LastVParameter();           // Trimmed last
    aS->Bounds(aDummyF, aDummyL, aBaseF, aBaseL); // Non-trimmed values.
    if (std::abs(aBaseF - aTrimF) + std::abs(aBaseL - aTrimL) > Precision::PConfusion())
    {
      // Param space reduced.
      theVPeriod = 0.0;
    }
  }
}

//=======================================================================
// function : aFuncValue
// purpose  : compute functional value in (theU,theV) point
//=======================================================================
static double anOrthogSqValue(const gp_Pnt&                         aBasePnt,
                              const occ::handle<Adaptor3d_Surface>& Surf,
                              const double                          theU,
                              const double                          theV)
{
  // Since find projection, formula is:
  // F1 = Dot(S_U, Vec(aBasePnt, aProjPnt))
  // F2 = Dot(S_V, Vec(aBasePnt, aProjPnt))

  gp_Pnt aProjPnt;
  gp_Vec aSu, aSv;

  Surf->D1(theU, theV, aProjPnt, aSu, aSv);
  gp_Vec aBaseVec(aBasePnt, aProjPnt);

  if (aSu.SquareMagnitude() > Precision::SquareConfusion())
    aSu.Normalize();

  if (aSv.SquareMagnitude() > Precision::SquareConfusion())
    aSv.Normalize();

  double aFirstPart  = aSu.Dot(aBaseVec);
  double aSecondPart = aSv.Dot(aBaseVec);
  return (aFirstPart * aFirstPart + aSecondPart * aSecondPart);
}

//=======================================================================
// function : Value
// purpose  : (OCC217 - apo)- Compute Point2d that project on polar surface(<Surf>) 3D<Curve>
//            <InitCurve2d> use for calculate start 2D point.
//=======================================================================
static gp_Pnt2d Function_Value(const double theU, const aFuncStruct& theData)
{
  gp_Pnt2d p2d          = theData.myInitCurve2d->Value(theU);
  gp_Pnt   p            = theData.myCurve->Value(theU);
  gp_Pnt   aSurfPnt     = theData.mySurf->Value(p2d.X(), p2d.Y());
  double   aSurfPntDist = aSurfPnt.SquareDistance(p);

  double Uinf, Usup, Vinf, Vsup;
  Uinf = theData.mySurf->FirstUParameter();
  Usup = theData.mySurf->LastUParameter();
  Vinf = theData.mySurf->FirstVParameter();
  Vsup = theData.mySurf->LastVParameter();

  // Check case when curve is close to co-parametrized isoline on surf.
  if (std::abs(p2d.X() - Uinf) < Precision::PConfusion()
      || std::abs(p2d.X() - Usup) < Precision::PConfusion())
  {
    // V isoline.
    gp_Pnt aPnt;
    theData.mySurf->D0(p2d.X(), theU, aPnt);
    if (aPnt.SquareDistance(p) < aSurfPntDist)
      p2d.SetY(theU);
  }

  if (std::abs(p2d.Y() - Vinf) < Precision::PConfusion()
      || std::abs(p2d.Y() - Vsup) < Precision::PConfusion())
  {
    // U isoline.
    gp_Pnt aPnt;
    theData.mySurf->D0(theU, p2d.Y(), aPnt);
    if (aPnt.SquareDistance(p) < aSurfPntDist)
      p2d.SetX(theU);
  }

  int    decalU = 0, decalV = 0;
  double U0 = p2d.X(), V0 = p2d.Y();

  GeomAbs_SurfaceType Type = theData.mySurf->GetType();
  if ((Type != GeomAbs_BSplineSurface) && (Type != GeomAbs_BezierSurface)
      && (Type != GeomAbs_OffsetSurface))
  {
    // Analytical cases.
    double S = 0., T = 0.;
    switch (Type)
    {
      case GeomAbs_Cylinder: {
        gp_Cylinder Cylinder = theData.mySurf->Cylinder();
        ElSLib::Parameters(Cylinder, p, S, T);
        if (U0 < Uinf)
          decalU = -int((Uinf - U0) / (2 * M_PI)) - 1;
        if (U0 > Usup)
          decalU = int((U0 - Usup) / (2 * M_PI)) + 1;
        S += decalU * 2 * M_PI;
        break;
      }
      case GeomAbs_Cone: {
        gp_Cone Cone = theData.mySurf->Cone();
        ElSLib::Parameters(Cone, p, S, T);
        if (U0 < Uinf)
          decalU = -int((Uinf - U0) / (2 * M_PI)) - 1;
        if (U0 > Usup)
          decalU = int((U0 - Usup) / (2 * M_PI)) + 1;
        S += decalU * 2 * M_PI;
        break;
      }
      case GeomAbs_Sphere: {
        gp_Sphere Sphere = theData.mySurf->Sphere();
        ElSLib::Parameters(Sphere, p, S, T);
        if (U0 < Uinf)
          decalU = -int((Uinf - U0) / (2 * M_PI)) - 1;
        if (U0 > Usup)
          decalU = int((U0 - Usup) / (2 * M_PI)) + 1;
        S += decalU * 2 * M_PI;
        if (V0 < Vinf)
          decalV = -int((Vinf - V0) / (2 * M_PI)) - 1;
        if (V0 > (Vsup + (Vsup - Vinf)))
          decalV = int((V0 - Vsup + (Vsup - Vinf)) / (2 * M_PI)) + 1;
        T += decalV * 2 * M_PI;
        if (0.4 * M_PI < std::abs(U0 - S) && std::abs(U0 - S) < 1.6 * M_PI)
        {
          T = M_PI - T;
          if (U0 < S)
            S -= M_PI;
          else
            S += M_PI;
        }
        break;
      }
      case GeomAbs_Torus: {
        gp_Torus Torus = theData.mySurf->Torus();
        ElSLib::Parameters(Torus, p, S, T);
        if (U0 < Uinf)
          decalU = -int((Uinf - U0) / (2 * M_PI)) - 1;
        if (U0 > Usup)
          decalU = int((U0 - Usup) / (2 * M_PI)) + 1;
        if (V0 < Vinf)
          decalV = -int((Vinf - V0) / (2 * M_PI)) - 1;
        if (V0 > Vsup)
          decalV = int((V0 - Vsup) / (2 * M_PI)) + 1;
        S += decalU * 2 * M_PI;
        T += decalV * 2 * M_PI;
        break;
      }
      default:
        throw Standard_NoSuchObject("ProjLib_ComputeApproxOnPolarSurface::Value");
    }
    return gp_Pnt2d(S, T);
  }

  // Non-analytical case.
  double Dist2Min = RealLast();
  double uperiod = theData.myPeriod[0], vperiod = theData.myPeriod[1], u, v;

  // U0 and V0 are the points within the initialized period.
  if (U0 < Uinf)
  {
    if (!uperiod)
      U0 = Uinf;
    else
    {
      decalU = int((Uinf - U0) / uperiod) + 1;
      U0 += decalU * uperiod;
    }
  }
  if (U0 > Usup)
  {
    if (!uperiod)
      U0 = Usup;
    else
    {
      decalU = -(int((U0 - Usup) / uperiod) + 1);
      U0 += decalU * uperiod;
    }
  }
  if (V0 < Vinf)
  {
    if (!vperiod)
      V0 = Vinf;
    else
    {
      decalV = int((Vinf - V0) / vperiod) + 1;
      V0 += decalV * vperiod;
    }
  }
  if (V0 > Vsup)
  {
    if (!vperiod)
      V0 = Vsup;
    else
    {
      decalV = -int((V0 - Vsup) / vperiod) - 1;
      V0 += decalV * vperiod;
    }
  }

  // The surface around (U0,V0) is reduced.
  double uLittle = (Usup - Uinf) / 10, vLittle = (Vsup - Vinf) / 10;
  double uInfLi = 0, vInfLi = 0, uSupLi = 0, vSupLi = 0;
  if ((U0 - Uinf) > uLittle)
    uInfLi = U0 - uLittle;
  else
    uInfLi = Uinf;
  if ((V0 - Vinf) > vLittle)
    vInfLi = V0 - vLittle;
  else
    vInfLi = Vinf;
  if ((Usup - U0) > uLittle)
    uSupLi = U0 + uLittle;
  else
    uSupLi = Usup;
  if ((Vsup - V0) > vLittle)
    vSupLi = V0 + vLittle;
  else
    vSupLi = Vsup;

  GeomAdaptor_Surface SurfLittle;
  if (Type == GeomAbs_BSplineSurface)
  {
    occ::handle<Geom_Surface> GBSS(theData.mySurf->BSpline());
    SurfLittle.Load(GBSS, uInfLi, uSupLi, vInfLi, vSupLi);
  }
  else if (Type == GeomAbs_BezierSurface)
  {
    occ::handle<Geom_Surface> GS(theData.mySurf->Bezier());
    SurfLittle.Load(GS, uInfLi, uSupLi, vInfLi, vSupLi);
  }
  else if (Type == GeomAbs_OffsetSurface)
  {
    occ::handle<Geom_Surface> GS = GeomAdaptor::MakeSurface(*theData.mySurf);
    SurfLittle.Load(GS, uInfLi, uSupLi, vInfLi, vSupLi);
  }
  else
  {
    throw Standard_NoSuchObject(
      "ProjLib_ComputeApproxOnPolarSurface::ProjectUsingInitialCurve2d() - unknown surface type");
  }

  // Try to run simple search with initial point (U0, V0).
  Extrema_GenLocateExtPS locext(SurfLittle, theData.myTolU, theData.myTolV);
  locext.Perform(p, U0, V0);
  if (locext.IsDone())
  {
    locext.Point().Parameter(u, v);
    Dist2Min = anOrthogSqValue(p, theData.mySurf, u, v);
    if (Dist2Min < theData.mySqProjOrtTol && // Point is projection.
        locext.SquareDistance()
          < aSurfPntDist + Precision::SquareConfusion()) // Point better than initial.
    {
      gp_Pnt2d pnt(u - decalU * uperiod, v - decalV * vperiod);
      return pnt;
    }
  }

  // Perform whole param space search.
  Extrema_ExtPS ext(p, SurfLittle, theData.myTolU, theData.myTolV, Extrema_ExtFlag_MIN);
  if (ext.IsDone() && ext.NbExt() >= 1)
  {
    Dist2Min      = ext.SquareDistance(1);
    int GoodValue = 1;
    for (int i = 2; i <= ext.NbExt(); i++)
    {
      if (Dist2Min > ext.SquareDistance(i))
      {
        Dist2Min  = ext.SquareDistance(i);
        GoodValue = i;
      }
    }
    ext.Point(GoodValue).Parameter(u, v);
    Dist2Min = anOrthogSqValue(p, theData.mySurf, u, v);
    if (Dist2Min < theData.mySqProjOrtTol && // Point is projection.
        ext.SquareDistance(GoodValue)
          < aSurfPntDist + Precision::SquareConfusion()) // Point better than initial.
    {
      gp_Pnt2d pnt(u - decalU * uperiod, v - decalV * vperiod);
      return pnt;
    }
  }

  // Both searches return bad values, use point from initial 2dcurve.
  return p2d;
}

//=======================================================================
// function : ProjLib_PolarFunction
// purpose  : (OCC217 - apo)- This class produce interface to call "gp_Pnt2d Function_Value(...)"
//=======================================================================

class ProjLib_PolarFunction : public AppCont_Function
{
  aFuncStruct myStruct;

public:
  ProjLib_PolarFunction(const occ::handle<Adaptor3d_Curve>&   C,
                        const occ::handle<Adaptor3d_Surface>& Surf,
                        const occ::handle<Adaptor2d_Curve2d>& InitialCurve2d,
                        const double                          Tol3d)
  {
    myNbPnt   = 0;
    myNbPnt2d = 1;

    computePeriodicity(Surf, myStruct.myPeriod[0], myStruct.myPeriod[1]);

    myStruct.myCurve        = C;
    myStruct.myInitCurve2d  = InitialCurve2d;
    myStruct.mySurf         = Surf;
    myStruct.mySqProjOrtTol = 10000.0 * Tol3d * Tol3d;
    myStruct.myTolU         = Surf->UResolution(Tol3d);
    myStruct.myTolV         = Surf->VResolution(Tol3d);
  }

  ~ProjLib_PolarFunction() override = default;

  double FirstParameter() const override { return myStruct.myCurve->FirstParameter(); }

  double LastParameter() const override { return myStruct.myCurve->LastParameter(); }

  gp_Pnt2d Value(const double t) const { return Function_Value(t, myStruct); }

  bool Value(const double                  theT,
             NCollection_Array1<gp_Pnt2d>& thePnt2d,
             NCollection_Array1<gp_Pnt>& /*thePnt*/) const override
  {
    thePnt2d(1) = Function_Value(theT, myStruct);
    return true;
  }

  bool D1(const double /*theT*/,
          NCollection_Array1<gp_Vec2d>& /*theVec2d*/,
          NCollection_Array1<gp_Vec>& /*theVec*/) const override
  {
    return false;
  }
};

//=================================================================================================

ProjLib_ComputeApproxOnPolarSurface::ProjLib_ComputeApproxOnPolarSurface()
    : myProjIsDone(false),
      myTolerance(Precision::Approximation()),
      myTolReached(-1.0),
      myDegMin(-1),
      myDegMax(-1),
      myMaxSegments(-1),
      myMaxDist(-1.),
      myBndPnt(AppParCurves_TangencyPoint),
      myDist(0.)
{
}

//=================================================================================================

ProjLib_ComputeApproxOnPolarSurface::ProjLib_ComputeApproxOnPolarSurface(
  const occ::handle<Adaptor2d_Curve2d>& theInitialCurve2d,
  const occ::handle<Adaptor3d_Curve>&   theCurve,
  const occ::handle<Adaptor3d_Surface>& theSurface,
  const double                          theTolerance3D)
    : myProjIsDone(false),
      myTolerance(theTolerance3D),
      myTolReached(-1.0),
      myDegMin(-1),
      myDegMax(-1),
      myMaxSegments(-1),
      myMaxDist(-1.),
      myBndPnt(AppParCurves_TangencyPoint),
      myDist(0.)
{
  myBSpline = Perform(theInitialCurve2d, theCurve, theSurface);
}

//=======================================================================
// function : ProjLib_ComputeApproxOnPolarSurface
// purpose  : case without curve of initialization
//=======================================================================

ProjLib_ComputeApproxOnPolarSurface::ProjLib_ComputeApproxOnPolarSurface(
  const occ::handle<Adaptor3d_Curve>&   theCurve,
  const occ::handle<Adaptor3d_Surface>& theSurface,
  const double                          theTolerance3D)
    : myProjIsDone(false),
      myTolerance(theTolerance3D),
      myTolReached(-1.0),
      myDegMin(-1),
      myDegMax(-1),
      myMaxSegments(-1),
      myMaxDist(-1.),
      myBndPnt(AppParCurves_TangencyPoint),
      myDist(0.)
{
  const occ::handle<Adaptor2d_Curve2d> anInitCurve2d;
  myBSpline = Perform(anInitCurve2d, theCurve, theSurface);
}

//=======================================================================
// function : ProjLib_ComputeApproxOnPolarSurface
// purpose  : Process the case of sewing
//=======================================================================

ProjLib_ComputeApproxOnPolarSurface::ProjLib_ComputeApproxOnPolarSurface(
  const occ::handle<Adaptor2d_Curve2d>& theInitialCurve2d,
  const occ::handle<Adaptor2d_Curve2d>& theInitialCurve2dBis,
  const occ::handle<Adaptor3d_Curve>&   theCurve,
  const occ::handle<Adaptor3d_Surface>& theSurface,
  const double                          theTolerance3D)
    : myProjIsDone(false),
      myTolerance(theTolerance3D),
      myTolReached(-1.0),
      myDegMin(-1),
      myDegMax(-1),
      myMaxSegments(-1),
      myMaxDist(-1.),
      myBndPnt(AppParCurves_TangencyPoint),
      myDist(0.)
{
  // InitialCurve2d and InitialCurve2dBis are two pcurves of the sewing
  occ::handle<Geom2d_BSplineCurve> bsc = Perform(theInitialCurve2d, theCurve, theSurface);

  if (myProjIsDone)
  {
    gp_Pnt2d P2dproj, P2d, P2dBis;
    P2dproj = bsc->StartPoint();
    P2d     = theInitialCurve2d->Value(theInitialCurve2d->FirstParameter());
    P2dBis  = theInitialCurve2dBis->Value(theInitialCurve2dBis->FirstParameter());

    double Dist, DistBis;
    Dist    = P2dproj.Distance(P2d);
    DistBis = P2dproj.Distance(P2dBis);
    if (Dist < DistBis)
    {
      // myBSpline2d is the pcurve that is found. It is translated to obtain myCurve2d
      myBSpline                       = bsc;
      occ::handle<Geom2d_Geometry> GG = myBSpline->Translated(P2d, P2dBis);
      my2ndCurve                      = occ::down_cast<Geom2d_Curve>(GG);
    }
    else
    {
      my2ndCurve                      = bsc;
      occ::handle<Geom2d_Geometry> GG = my2ndCurve->Translated(P2dBis, P2d);
      myBSpline                       = occ::down_cast<Geom2d_BSplineCurve>(GG);
    }
  }
}

//=================================================================================================

static occ::handle<Geom2d_BSplineCurve> Concat(const occ::handle<Geom2d_BSplineCurve>& C1,
                                               const occ::handle<Geom2d_BSplineCurve>& C2,
                                               double                                  theUJump,
                                               double                                  theVJump)
{
  int deg, deg1, deg2;
  deg1 = C1->Degree();
  deg2 = C2->Degree();

  if (deg1 < deg2)
  {
    C1->IncreaseDegree(deg2);
    deg = deg2;
  }
  else if (deg2 < deg1)
  {
    C2->IncreaseDegree(deg1);
    deg = deg1;
  }
  else
    deg = deg1;

  int np1, np2, nk1, nk2, np, nk;
  np1 = C1->NbPoles();
  nk1 = C1->NbKnots();
  np2 = C2->NbPoles();
  nk2 = C2->NbKnots();
  nk  = nk1 + nk2 - 1;
  np  = np1 + np2 - 1;

  const NCollection_Array1<double>&   K1 = C1->Knots();
  const NCollection_Array1<int>&      M1 = C1->Multiplicities();
  const NCollection_Array1<gp_Pnt2d>& P1 = C1->Poles();
  const NCollection_Array1<double>&   K2 = C2->Knots();
  const NCollection_Array1<int>&      M2 = C2->Multiplicities();
  const NCollection_Array1<gp_Pnt2d>& P2 = C2->Poles();

  // Compute the new BSplineCurve
  NCollection_Array1<double>   K(1, nk);
  NCollection_Array1<int>      M(1, nk);
  NCollection_Array1<gp_Pnt2d> P(1, np);

  int i, count = 0;
  // Set Knots and Mults
  for (i = 1; i <= nk1; i++)
  {
    count++;
    K(count) = K1(i);
    M(count) = M1(i);
  }
  M(count) = deg;
  for (i = 2; i <= nk2; i++)
  {
    count++;
    K(count) = K2(i);
    M(count) = M2(i);
  }
  // Set the Poles
  count = 0;
  for (i = 1; i <= np1; i++)
  {
    count++;
    P(count) = P1(i);
  }
  for (i = 2; i <= np2; i++)
  {
    count++;
    P(count).SetX(P2(i).X() + theUJump);
    P(count).SetY(P2(i).Y() + theVJump);
  }

  occ::handle<Geom2d_BSplineCurve> BS = new Geom2d_BSplineCurve(P, K, M, deg);
  return BS;
}

//=================================================================================================

void ProjLib_ComputeApproxOnPolarSurface::Perform(const occ::handle<Adaptor3d_Curve>&   Curve,
                                                  const occ::handle<Adaptor3d_Surface>& S)
{
  const occ::handle<Adaptor2d_Curve2d> anInitCurve2d;
  myBSpline = Perform(anInitCurve2d, Curve, S);
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> ProjLib_ComputeApproxOnPolarSurface::Perform(
  const occ::handle<Adaptor2d_Curve2d>& InitialCurve2d,
  const occ::handle<Adaptor3d_Curve>&   Curve,
  const occ::handle<Adaptor3d_Surface>& S)
{
  // OCC217
  double           Tol3d    = myTolerance;
  constexpr double ParamTol = Precision::PApproximation();

  occ::handle<Adaptor2d_Curve2d> AHC2d = InitialCurve2d;
  occ::handle<Adaptor3d_Curve>   AHC   = Curve;

  // if the curve 3d is a BSpline with degree C0, it is cut into sections with degree C1
  // -> bug cts18237
  GeomAbs_CurveType typeCurve = Curve->GetType();
  if (typeCurve == GeomAbs_BSplineCurve)
  {
    NCollection_List<occ::handle<Standard_Transient>> LOfBSpline2d;
    occ::handle<Geom_BSplineCurve>                    BSC     = Curve->BSpline();
    int                                               nbInter = Curve->NbIntervals(GeomAbs_C1);
    if (nbInter > 1)
    {
      int                              i, j;
      occ::handle<Geom_TrimmedCurve>   GTC;
      occ::handle<Geom2d_TrimmedCurve> G2dTC;
      NCollection_Array1<double>       Inter(1, nbInter + 1);
      Curve->Intervals(Inter, GeomAbs_C1);
      double firstinter = Inter.Value(1), secondinter = Inter.Value(2);
      // initialization 3d
      GTC = new Geom_TrimmedCurve(BSC, firstinter, secondinter);
      AHC = new GeomAdaptor_Curve(GTC);

      // if there is an initialization curve:
      // - either this is a BSpline C0, with discontinuity at the same parameters of nodes
      // and the sections C1 are taken
      // - or this is a curve C1 and the sections of interest are taken otherwise the curve is
      // created.

      // initialization 2d
      int  nbInter2d;
      bool C2dIsToCompute;
      C2dIsToCompute = InitialCurve2d.IsNull();
      occ::handle<Geom2d_BSplineCurve> BSC2d;
      occ::handle<Geom2d_Curve>        G2dC;

      if (!C2dIsToCompute)
      {
        nbInter2d = InitialCurve2d->NbIntervals(GeomAbs_C1);
        NCollection_Array1<double> Inter2d(1, nbInter2d + 1);
        InitialCurve2d->Intervals(Inter2d, GeomAbs_C1);
        j = 1;
        for (i = 1, j = 1; i <= nbInter; i++)
          if (std::abs(Inter.Value(i) - Inter2d.Value(j)) < ParamTol)
          {
            if (j > nbInter2d)
              break;
            j++;
          }
        if (j != (nbInter2d + 1))
        {
          C2dIsToCompute = true;
        }
      }

      if (C2dIsToCompute)
      {
        AHC2d = BuildInitialCurve2d(AHC, S);
      }
      else
      {
        typeCurve = InitialCurve2d->GetType();
        switch (typeCurve)
        {
          case GeomAbs_Line: {
            G2dC = new Geom2d_Line(InitialCurve2d->Line());
            break;
          }
          case GeomAbs_Circle: {
            G2dC = new Geom2d_Circle(InitialCurve2d->Circle());
            break;
          }
          case GeomAbs_Ellipse: {
            G2dC = new Geom2d_Ellipse(InitialCurve2d->Ellipse());
            break;
          }
          case GeomAbs_Hyperbola: {
            G2dC = new Geom2d_Hyperbola(InitialCurve2d->Hyperbola());
            break;
          }
          case GeomAbs_Parabola: {
            G2dC = new Geom2d_Parabola(InitialCurve2d->Parabola());
            break;
          }
          case GeomAbs_BezierCurve: {
            G2dC = InitialCurve2d->Bezier();
            break;
          }
          case GeomAbs_BSplineCurve: {
            G2dC = InitialCurve2d->BSpline();
            break;
          }
          case GeomAbs_OtherCurve:
          default:
            break;
        }
        gp_Pnt2d fp2d = G2dC->Value(firstinter), lp2d = G2dC->Value(secondinter);
        gp_Pnt   fps, lps, fpc, lpc;
        S->D0(fp2d.X(), fp2d.Y(), fps);
        S->D0(lp2d.X(), lp2d.Y(), lps);
        Curve->D0(firstinter, fpc);
        Curve->D0(secondinter, lpc);
        // OCC217
        if ((fps.IsEqual(fpc, Tol3d)) && (lps.IsEqual(lpc, Tol3d)))
        {
          // if((fps.IsEqual(fpc, myTolerance)) &&
          //    (lps.IsEqual(lpc, myTolerance))) {
          G2dTC = new Geom2d_TrimmedCurve(G2dC, firstinter, secondinter);
          Geom2dAdaptor_Curve G2dAC(G2dTC);
          AHC2d        = new Geom2dAdaptor_Curve(G2dAC);
          myProjIsDone = true;
        }
        else
        {
          AHC2d          = BuildInitialCurve2d(AHC, S);
          C2dIsToCompute = true;
        }
      }

      if (myProjIsDone)
      {
        BSC2d = ProjectUsingInitialCurve2d(AHC, S, AHC2d);
        if (BSC2d.IsNull())
        {
          return occ::handle<Geom2d_BSplineCurve>();
        }
        LOfBSpline2d.Append(BSC2d);
      }
      else
      {
        return occ::handle<Geom2d_BSplineCurve>();
      }

      int nbK2d = BSC2d->NbKnots();
      int deg   = BSC2d->Degree();

      for (i = 2; i <= nbInter; i++)
      {
        double iinter   = Inter.Value(i);
        double ip1inter = Inter.Value(i + 1);
        // general case 3d
        GTC->SetTrim(iinter, ip1inter);
        AHC = new GeomAdaptor_Curve(GTC);

        // general case 2d
        if (C2dIsToCompute)
        {
          AHC2d = BuildInitialCurve2d(AHC, S);
        }
        else
        {
          gp_Pnt2d fp2d = G2dC->Value(iinter), lp2d = G2dC->Value(ip1inter);
          gp_Pnt   fps, lps, fpc, lpc;
          S->D0(fp2d.X(), fp2d.Y(), fps);
          S->D0(lp2d.X(), lp2d.Y(), lps);
          Curve->D0(iinter, fpc);
          Curve->D0(ip1inter, lpc);
          // OCC217
          if ((fps.IsEqual(fpc, Tol3d)) && (lps.IsEqual(lpc, Tol3d)))
          {
            // if((fps.IsEqual(fpc, myTolerance)) &&
            //    (lps.IsEqual(lpc, myTolerance))) {
            G2dTC->SetTrim(iinter, ip1inter);
            Geom2dAdaptor_Curve G2dAC(G2dTC);
            AHC2d        = new Geom2dAdaptor_Curve(G2dAC);
            myProjIsDone = true;
          }
          else
          {
            AHC2d = BuildInitialCurve2d(AHC, S);
          }
        }
        if (myProjIsDone)
        {
          BSC2d = ProjectUsingInitialCurve2d(AHC, S, AHC2d);
          if (BSC2d.IsNull())
          {
            return occ::handle<Geom2d_BSplineCurve>();
          }
          LOfBSpline2d.Append(BSC2d);

          (void)nbK2d; // unused but set for debug
          nbK2d += BSC2d->NbKnots() - 1;
          deg = std::max(deg, BSC2d->Degree());
        }
        else
        {
          return occ::handle<Geom2d_BSplineCurve>();
        }
      }

      double anUPeriod, anVPeriod;
      computePeriodicity(S, anUPeriod, anVPeriod);
      int                              NbC = LOfBSpline2d.Extent();
      occ::handle<Geom2d_BSplineCurve> CurBS;
      CurBS = occ::down_cast<Geom2d_BSplineCurve>(LOfBSpline2d.First());
      LOfBSpline2d.RemoveFirst();
      for (int ii = 2; ii <= NbC; ii++)
      {
        occ::handle<Geom2d_BSplineCurve> BS =
          occ::down_cast<Geom2d_BSplineCurve>(LOfBSpline2d.First());

        // Check for period jump in point of contact.
        gp_Pnt2d aC1End  = CurBS->Pole(CurBS->NbPoles()); // End of C1.
        gp_Pnt2d aC2Beg  = BS->Pole(1);                   // Beginning of C2.
        double   anUJump = 0.0, anVJump = 0.0;

        if (anUPeriod > 0.0 && std::abs(aC1End.X() - aC2Beg.X()) > (anUPeriod) / 2.01)
        {
          double aMultCoeff = aC2Beg.X() < aC1End.X() ? 1.0 : -1.0;
          anUJump           = (anUPeriod)*aMultCoeff;
        }

        if (anVPeriod && std::abs(aC1End.Y() - aC2Beg.Y()) > (anVPeriod) / 2.01)
        {
          double aMultCoeff = aC2Beg.Y() < aC1End.Y() ? 1.0 : -1.0;
          anVJump           = (anVPeriod)*aMultCoeff;
        }

        CurBS = Concat(CurBS, BS, anUJump, anVJump);
        LOfBSpline2d.RemoveFirst();
      }
      return CurBS;
    }
  }

  if (InitialCurve2d.IsNull())
  {
    AHC2d = BuildInitialCurve2d(Curve, S);
    if (!myProjIsDone)
      return occ::handle<Geom2d_BSplineCurve>();
  }
  return ProjectUsingInitialCurve2d(AHC, S, AHC2d);
}

//=================================================================================================

occ::handle<Adaptor2d_Curve2d> ProjLib_ComputeApproxOnPolarSurface::BuildInitialCurve2d(
  const occ::handle<Adaptor3d_Curve>&   Curve,
  const occ::handle<Adaptor3d_Surface>& Surf)
{
  //  discretize the Curve with quasiuniform deflection
  //  density at least NbOfPnts points
  myProjIsDone = false;

  // OCC217
  double Tol3d = myTolerance;
  double TolU = Surf->UResolution(Tol3d), TolV = Surf->VResolution(Tol3d);
  double DistTol3d = 100.0 * Tol3d;
  if (myMaxDist > 0.)
  {
    DistTol3d = myMaxDist;
  }
  double DistTol3d2 = DistTol3d * DistTol3d;
  double uperiod = 0.0, vperiod = 0.0;
  computePeriodicity(Surf, uperiod, vperiod);

  // NO myTol is Tol2d !!!!
  // double TolU = myTolerance, TolV = myTolerance;
  // double Tol3d = 100*myTolerance; // At random Balthazar.

  int                         NbOfPnts = 61;
  GCPnts_QuasiUniformAbscissa QUA(*Curve, NbOfPnts);
  NbOfPnts = QUA.NbPoints();
  NCollection_Array1<gp_Pnt> Pts(1, NbOfPnts);
  NCollection_Array1<double> Param(1, NbOfPnts);
  int                        i, j;
  for (i = 1; i <= NbOfPnts; i++)
  {
    Param(i) = QUA.Parameter(i);
    Pts(i)   = Curve->Value(Param(i));
  }

  NCollection_Array1<gp_Pnt2d> Pts2d(1, NbOfPnts);
  NCollection_Array1<int>      Mult(1, NbOfPnts);
  Mult.Init(1);
  Mult(1) = Mult(NbOfPnts) = 2;

  double Uinf, Usup, Vinf, Vsup;
  Uinf                     = Surf->FirstUParameter();
  Usup                     = Surf->LastUParameter();
  Vinf                     = Surf->FirstVParameter();
  Vsup                     = Surf->LastVParameter();
  GeomAbs_SurfaceType Type = Surf->GetType();
  if ((Type != GeomAbs_BSplineSurface) && (Type != GeomAbs_BezierSurface)
      && (Type != GeomAbs_OffsetSurface))
  {
    double S, T;
    //    int usens = 0, vsens = 0;
    // to know the position relatively to the period
    switch (Type)
    {
        //    case GeomAbs_Plane:
        //      {
        //	gp_Pln Plane = Surf->Plane();
        //	for ( i = 1 ; i <= NbOfPnts ; i++) {
        //	  ElSLib::Parameters( Plane, Pts(i), S, T);
        //	  Pts2d(i).SetCoord(S,T);
        //	}
        //	myProjIsDone = true;
        //	break;
        //      }
      case GeomAbs_Cylinder: {
        //	double Sloc, Tloc;
        double      Sloc;
        int         usens    = 0;
        gp_Cylinder Cylinder = Surf->Cylinder();
        ElSLib::Parameters(Cylinder, Pts(1), S, T);
        Pts2d(1).SetCoord(S, T);
        for (i = 2; i <= NbOfPnts; i++)
        {
          Sloc = S;
          ElSLib::Parameters(Cylinder, Pts(i), S, T);
          if (std::abs(Sloc - S) > M_PI)
          {
            if (Sloc > S)
              usens++;
            else
              usens--;
          }
          Pts2d(i).SetCoord(S + usens * 2 * M_PI, T);
        }
        myProjIsDone = true;
        break;
      }
      case GeomAbs_Cone: {
        //	double Sloc, Tloc;
        double  Sloc;
        int     usens = 0;
        gp_Cone Cone  = Surf->Cone();
        ElSLib::Parameters(Cone, Pts(1), S, T);
        Pts2d(1).SetCoord(S, T);
        for (i = 2; i <= NbOfPnts; i++)
        {
          Sloc = S;
          ElSLib::Parameters(Cone, Pts(i), S, T);
          if (std::abs(Sloc - S) > M_PI)
          {
            if (Sloc > S)
              usens++;
            else
              usens--;
          }
          Pts2d(i).SetCoord(S + usens * 2 * M_PI, T);
        }
        myProjIsDone = true;
        break;
      }
      case GeomAbs_Sphere: {
        double    Sloc, Tloc;
        int       usens = 0, vsens = 0; // usens steps by half-period
        bool      vparit = false;
        gp_Sphere Sphere = Surf->Sphere();
        ElSLib::Parameters(Sphere, Pts(1), S, T);
        Pts2d(1).SetCoord(S, T);
        for (i = 2; i <= NbOfPnts; i++)
        {
          Sloc = S;
          Tloc = T;
          ElSLib::Parameters(Sphere, Pts(i), S, T);
          if (1.6 * M_PI < std::abs(Sloc - S))
          {
            if (Sloc > S)
              usens += 2;
            else
              usens -= 2;
          }
          if (1.6 * M_PI > std::abs(Sloc - S) && std::abs(Sloc - S) > 0.4 * M_PI)
          {
            vparit = !vparit;
            if (Sloc > S)
              usens++;
            else
              usens--;
            if (std::abs(Tloc - Vsup) < (Vsup - Vinf) / 5)
              vsens++;
            else
              vsens--;
          }
          if (vparit)
          {
            Pts2d(i).SetCoord(S + usens * M_PI, (M_PI - T) * (vsens - 1));
          }
          else
          {
            Pts2d(i).SetCoord(S + usens * M_PI, T + vsens * M_PI);
          }
        }
        myProjIsDone = true;
        break;
      }
      case GeomAbs_Torus: {
        double   Sloc, Tloc;
        int      usens = 0, vsens = 0;
        gp_Torus Torus = Surf->Torus();
        ElSLib::Parameters(Torus, Pts(1), S, T);
        Pts2d(1).SetCoord(S, T);
        for (i = 2; i <= NbOfPnts; i++)
        {
          Sloc = S;
          Tloc = T;
          ElSLib::Parameters(Torus, Pts(i), S, T);
          if (std::abs(Sloc - S) > M_PI)
          {
            if (Sloc > S)
              usens++;
            else
              usens--;
          }
          if (std::abs(Tloc - T) > M_PI)
          {
            if (Tloc > T)
              vsens++;
            else
              vsens--;
          }
          Pts2d(i).SetCoord(S + usens * 2 * M_PI, T + vsens * 2 * M_PI);
        }
        myProjIsDone = true;
        break;
      }
      default:
        throw Standard_NoSuchObject("ProjLib_ComputeApproxOnPolarSurface::BuildInitialCurve2d");
    }
  }
  else
  {
    myProjIsDone    = false;
    double Dist2Min = 1.e+200, u = 0., v = 0.;
    myDist = 0.;
    gp_Pnt pntproj;

    NCollection_Sequence<gp_Pnt2d> Sols;
    bool                           areManyZeros = false;

    pntproj = Pts(1);
    Extrema_ExtPS aExtPS(pntproj, *Surf, TolU, TolV, Extrema_ExtFlag_MIN);
    double        aMinSqDist = RealLast();
    if (aExtPS.IsDone())
    {
      for (i = 1; i <= aExtPS.NbExt(); i++)
      {
        double aSqDist = aExtPS.SquareDistance(i);
        if (aSqDist < aMinSqDist)
          aMinSqDist = aSqDist;
      }
    }
    if (aMinSqDist > DistTol3d2) // try to project with less tolerance
    {
      TolU = std::min(TolU, Precision::PConfusion());
      TolV = std::min(TolV, Precision::PConfusion());
      aExtPS.Initialize(*Surf,
                        Surf->FirstUParameter(),
                        Surf->LastUParameter(),
                        Surf->FirstVParameter(),
                        Surf->LastVParameter(),
                        TolU,
                        TolV);
      aExtPS.Perform(pntproj);
    }

    if (aExtPS.IsDone() && aExtPS.NbExt() >= 1)
    {

      int GoodValue = 1;

      for (i = 1; i <= aExtPS.NbExt(); i++)
      {
        if (aExtPS.SquareDistance(i) < DistTol3d2)
        {
          if (aExtPS.SquareDistance(i) <= 1.e-18)
          {
            aExtPS.Point(i).Parameter(u, v);
            gp_Pnt2d p2d(u, v);
            bool     isSame = false;
            for (j = 1; j <= Sols.Length(); j++)
            {
              if (p2d.SquareDistance(Sols.Value(j)) <= 1.e-18)
              {
                isSame = true;
                break;
              }
            }
            if (!isSame)
              Sols.Append(p2d);
          }
          if (Dist2Min > aExtPS.SquareDistance(i))
          {
            Dist2Min  = aExtPS.SquareDistance(i);
            GoodValue = i;
          }
        }
      }

      if (Sols.Length() > 1)
        areManyZeros = true;

      if (Dist2Min <= DistTol3d2)
      {
        if (!areManyZeros)
        {
          aExtPS.Point(GoodValue).Parameter(u, v);
          Pts2d(1).SetCoord(u, v);
          myProjIsDone = true;
        }
        else
        {
          int    nbSols   = Sols.Length();
          double Dist2Max = -1.e+200;
          for (i = 1; i <= nbSols; i++)
          {
            const gp_Pnt2d& aP1 = Sols.Value(i);
            for (j = i + 1; j <= nbSols; j++)
            {
              const gp_Pnt2d& aP2    = Sols.Value(j);
              double          aDist2 = aP1.SquareDistance(aP2);
              if (aDist2 > Dist2Max)
                Dist2Max = aDist2;
            }
          }
          double aMaxT2 = std::max(TolU, TolV);
          aMaxT2 *= aMaxT2;
          if (Dist2Max > aMaxT2)
          {
            int tPp = 0;
            for (i = 1; i <= 5; i++)
            {
              int nbExtOk = 0;
              int indExt  = 0;
              int iT      = 1 + (NbOfPnts - 1) / 5 * i;
              pntproj     = Pts(iT);
              Extrema_ExtPS aTPS(pntproj, *Surf, TolU, TolV, Extrema_ExtFlag_MIN);
              Dist2Min = 1.e+200;
              if (aTPS.IsDone() && aTPS.NbExt() >= 1)
              {
                for (j = 1; j <= aTPS.NbExt(); j++)
                {
                  if (aTPS.SquareDistance(j) < DistTol3d2)
                  {
                    nbExtOk++;
                    if (aTPS.SquareDistance(j) < Dist2Min)
                    {
                      Dist2Min = aTPS.SquareDistance(j);
                      indExt   = j;
                    }
                  }
                }
              }
              if (nbExtOk == 1)
              {
                tPp = iT;
                aTPS.Point(indExt).Parameter(u, v);
                break;
              }
            }

            if (tPp != 0 && tPp != NbOfPnts)
            {
              gp_Pnt2d aPp = gp_Pnt2d(u, v);
              gp_Pnt2d aPn;
              bool     isFound = false;
              for (j = tPp + 1; j <= NbOfPnts; ++j)
              {
                pntproj = Pts(j);
                Extrema_ExtPS aTPS(pntproj, *Surf, TolU, TolV, Extrema_ExtFlag_MIN);
                Dist2Min = RealLast();
                if (aTPS.IsDone() && aTPS.NbExt() >= 1)
                {
                  int indExt = 0;
                  for (i = 1; i <= aTPS.NbExt(); i++)
                  {
                    if (aTPS.SquareDistance(i) < DistTol3d2 && aTPS.SquareDistance(i) < Dist2Min)
                    {
                      Dist2Min = aTPS.SquareDistance(i);
                      indExt   = i;
                    }
                  }
                  if (indExt > 0)
                  {
                    aTPS.Point(indExt).Parameter(u, v);
                    aPn     = gp_Pnt2d(u, v);
                    isFound = true;
                    break;
                  }
                }
              }

              if (isFound)
              {
                gp_Dir2d atV(gp_Vec2d(aPp, aPn));
                bool     isChosen = false;
                for (i = 1; i <= nbSols; i++)
                {
                  const gp_Pnt2d& aP1 = Sols.Value(i);
                  gp_Dir2d        asV(gp_Vec2d(aP1, aPp));
                  if (asV.Dot(atV) > 0.)
                  {
                    isChosen = true;
                    u        = aP1.X();
                    v        = aP1.Y();
                    Pts2d(1).SetCoord(u, v);
                    myProjIsDone = true;
                    break;
                  }
                }
                if (!isChosen)
                {
                  aExtPS.Point(GoodValue).Parameter(u, v);
                  Pts2d(1).SetCoord(u, v);
                  myProjIsDone = true;
                }
              }
              else
              {
                aExtPS.Point(GoodValue).Parameter(u, v);
                Pts2d(1).SetCoord(u, v);
                myProjIsDone = true;
              }
            }
            else
            {
              aExtPS.Point(GoodValue).Parameter(u, v);
              Pts2d(1).SetCoord(u, v);
              myProjIsDone = true;
            }
          }
          else
          {
            aExtPS.Point(GoodValue).Parameter(u, v);
            Pts2d(1).SetCoord(u, v);
            myProjIsDone = true;
          }
        }
      }

      //  calculate the following points with GenLocate_ExtPS
      // (and store the result and each parameter in a sequence)
      int usens = 0, vsens = 0;
      // to know the position relatively to the period
      double U0 = u, V0 = v, U1 = u, V1 = v;
      // U0 and V0 are the points in the initialized period
      // (period with u and v),
      // U1 and V1 are the points for construction of poles
      myDist = Dist2Min;
      for (i = 2; i <= NbOfPnts; i++)
        if (myProjIsDone)
        {
          myProjIsDone = false;
          Dist2Min     = RealLast();
          pntproj      = Pts(i);
          Extrema_GenLocateExtPS aLocateExtPS(*Surf, TolU, TolV);
          aLocateExtPS.Perform(pntproj, U0, V0);

          if (aLocateExtPS.IsDone())
          {
            if (aLocateExtPS.SquareDistance() < DistTol3d2)
            { // OCC217
              // if (aLocateExtPS.SquareDistance() < Tol3d * Tol3d) {
              if (aLocateExtPS.SquareDistance() > myDist)
              {
                myDist = aLocateExtPS.SquareDistance();
              }
              (aLocateExtPS.Point()).Parameter(U0, V0);
              U1 = U0 + usens * uperiod;
              V1 = V0 + vsens * vperiod;
              Pts2d(i).SetCoord(U1, V1);
              myProjIsDone = true;
            }
            else
            {
              Extrema_ExtPS aGlobalExtr(pntproj, *Surf, TolU, TolV, Extrema_ExtFlag_MIN);
              if (aGlobalExtr.IsDone())
              {
                double LocalMinSqDist = RealLast();
                int    imin           = 0;
                for (int isol = 1; isol <= aGlobalExtr.NbExt(); isol++)
                {
                  double aSqDist = aGlobalExtr.SquareDistance(isol);
                  if (aSqDist < LocalMinSqDist)
                  {
                    LocalMinSqDist = aSqDist;
                    imin           = isol;
                  }
                }
                if (LocalMinSqDist < DistTol3d2)
                {
                  if (LocalMinSqDist > myDist)
                  {
                    myDist = LocalMinSqDist;
                  }
                  double LocalU, LocalV;
                  aGlobalExtr.Point(imin).Parameter(LocalU, LocalV);
                  if (uperiod > 0. && std::abs(U0 - LocalU) >= uperiod / 2.)
                  {
                    if (LocalU > U0)
                      usens = -1;
                    else
                      usens = 1;
                  }
                  if (vperiod > 0. && std::abs(V0 - LocalV) >= vperiod / 2.)
                  {
                    if (LocalV > V0)
                      vsens = -1;
                    else
                      vsens = 1;
                  }
                  U0 = LocalU;
                  V0 = LocalV;
                  U1 = U0 + usens * uperiod;
                  V1 = V0 + vsens * vperiod;
                  Pts2d(i).SetCoord(U1, V1);
                  myProjIsDone = true;

                  if ((i == 2) && (!IsEqual(uperiod, 0.0) || !IsEqual(vperiod, 0.0)))
                  { // Make 1st point more precise for periodic surfaces
                    const int      aSize = 3;
                    const gp_Pnt2d aP(Pts2d(2));
                    double         aUpar[aSize], aVpar[aSize];
                    Pts2d(1).Coord(aUpar[1], aVpar[1]);
                    aUpar[0] = aUpar[1] - uperiod;
                    aUpar[2] = aUpar[1] + uperiod;
                    aVpar[0] = aVpar[1] - vperiod;
                    aVpar[2] = aVpar[1] + vperiod;

                    double    aSQdistMin = RealLast();
                    int       aBestUInd = 1, aBestVInd = 1;
                    const int aSizeU = IsEqual(uperiod, 0.0) ? 1 : aSize,
                              aSizeV = IsEqual(vperiod, 0.0) ? 1 : aSize;
                    for (int uInd = 0; uInd < aSizeU; uInd++)
                    {
                      for (int vInd = 0; vInd < aSizeV; vInd++)
                      {
                        double aSQdist = aP.SquareDistance(gp_Pnt2d(aUpar[uInd], aVpar[vInd]));
                        if (aSQdist < aSQdistMin)
                        {
                          aSQdistMin = aSQdist;
                          aBestUInd  = uInd;
                          aBestVInd  = vInd;
                        }
                      }
                    }

                    Pts2d(1).SetCoord(aUpar[aBestUInd], aVpar[aBestVInd]);
                  } // if(i == 2) condition
                }
              }
            }
          }
          if (!myProjIsDone && uperiod)
          {
            double aUinf, aUsup, Uaux;
            aUinf = Surf->FirstUParameter();
            aUsup = Surf->LastUParameter();
            if ((aUsup - U0) > (U0 - aUinf))
              Uaux = 2 * aUinf - U0 + uperiod;
            else
              Uaux = 2 * aUsup - U0 - uperiod;

            Extrema_GenLocateExtPS locext(*Surf, TolU, TolV);
            locext.Perform(pntproj, Uaux, V0);

            if (locext.IsDone())
              if (locext.SquareDistance() < DistTol3d2)
              { // OCC217
                // if (locext.SquareDistance() < Tol3d * Tol3d) {
                if (locext.SquareDistance() > myDist)
                {
                  myDist = locext.SquareDistance();
                }
                (locext.Point()).Parameter(u, v);
                if ((aUsup - U0) > (U0 - aUinf))
                  usens--;
                else
                  usens++;
                U0 = u;
                V0 = v;
                U1 = U0 + usens * uperiod;
                V1 = V0 + vsens * vperiod;
                Pts2d(i).SetCoord(U1, V1);
                myProjIsDone = true;
              }
          }
          if (!myProjIsDone && vperiod)
          {
            double aVinf, aVsup, Vaux;
            aVinf = Surf->FirstVParameter();
            aVsup = Surf->LastVParameter();
            if ((aVsup - V0) > (V0 - aVinf))
              Vaux = 2 * aVinf - V0 + vperiod;
            else
              Vaux = 2 * aVsup - V0 - vperiod;

            Extrema_GenLocateExtPS locext(*Surf, TolU, TolV);
            locext.Perform(pntproj, U0, Vaux);

            if (locext.IsDone())
              if (locext.SquareDistance() < DistTol3d2)
              { // OCC217
                // if (locext.SquareDistance() < Tol3d * Tol3d) {
                if (locext.SquareDistance() > myDist)
                {
                  myDist = locext.SquareDistance();
                }
                (locext.Point()).Parameter(u, v);
                if ((aVsup - V0) > (V0 - aVinf))
                  vsens--;
                else
                  vsens++;
                U0 = u;
                V0 = v;
                U1 = U0 + usens * uperiod;
                V1 = V0 + vsens * vperiod;
                Pts2d(i).SetCoord(U1, V1);
                myProjIsDone = true;
              }
          }
          if (!myProjIsDone && uperiod && vperiod)
          {
            double Uaux, Vaux;
            if ((Usup - U0) > (U0 - Uinf))
              Uaux = 2 * Uinf - U0 + uperiod;
            else
              Uaux = 2 * Usup - U0 - uperiod;
            if ((Vsup - V0) > (V0 - Vinf))
              Vaux = 2 * Vinf - V0 + vperiod;
            else
              Vaux = 2 * Vsup - V0 - vperiod;

            Extrema_GenLocateExtPS locext(*Surf, TolU, TolV);
            locext.Perform(pntproj, Uaux, Vaux);

            if (locext.IsDone())
              if (locext.SquareDistance() < DistTol3d2)
              {
                // if (locext.SquareDistance() < Tol3d * Tol3d) {
                if (locext.SquareDistance() > myDist)
                {
                  myDist = locext.SquareDistance();
                }
                (locext.Point()).Parameter(u, v);
                if ((Usup - U0) > (U0 - Uinf))
                  usens--;
                else
                  usens++;
                if ((Vsup - V0) > (V0 - Vinf))
                  vsens--;
                else
                  vsens++;
                U0 = u;
                V0 = v;
                U1 = U0 + usens * uperiod;
                V1 = V0 + vsens * vperiod;
                Pts2d(i).SetCoord(U1, V1);
                myProjIsDone = true;
              }
          }
          if (!myProjIsDone)
          {
            Extrema_ExtPS ext(pntproj, *Surf, TolU, TolV, Extrema_ExtFlag_MIN);
            if (ext.IsDone())
            {
              Dist2Min       = ext.SquareDistance(1);
              int aGoodValue = 1;
              for (j = 2; j <= ext.NbExt(); j++)
                if (Dist2Min > ext.SquareDistance(j))
                {
                  Dist2Min   = ext.SquareDistance(j);
                  aGoodValue = j;
                }
              if (Dist2Min < DistTol3d2)
              {
                // if (Dist2Min < Tol3d * Tol3d) {
                if (Dist2Min > myDist)
                {
                  myDist = Dist2Min;
                }
                (ext.Point(aGoodValue)).Parameter(u, v);
                if (uperiod)
                {
                  if ((U0 - u) > (2 * uperiod / 3))
                  {
                    usens++;
                  }
                  else if ((u - U0) > (2 * uperiod / 3))
                  {
                    usens--;
                  }
                }
                if (vperiod)
                {
                  if ((V0 - v) > (vperiod / 2))
                  {
                    vsens++;
                  }
                  else if ((v - V0) > (vperiod / 2))
                  {
                    vsens--;
                  }
                }
                U0 = u;
                V0 = v;
                U1 = U0 + usens * uperiod;
                V1 = V0 + vsens * vperiod;
                Pts2d(i).SetCoord(U1, V1);
                myProjIsDone = true;
              }
            }
          }
        }
        else
          break;
    }
  }
  // -- Pnts2d is transformed into Geom2d_BSplineCurve, with the help of Param and Mult
  if (myProjIsDone)
  {
    myBSpline = new Geom2d_BSplineCurve(Pts2d, Param, Mult, 1);
    // jgv: put the curve into parametric range
    gp_Pnt2d MidPoint =
      myBSpline->Value(0.5 * (myBSpline->FirstParameter() + myBSpline->LastParameter()));
    double TestU = MidPoint.X(), TestV = MidPoint.Y();
    double sense = 0.;
    if (uperiod)
    {
      if (TestU < Uinf - TolU)
        sense = 1.;
      else if (TestU > Usup + TolU)
        sense = -1;
      while (TestU < Uinf - TolU || TestU > Usup + TolU)
        TestU += sense * uperiod;
    }
    if (vperiod)
    {
      sense = 0.;
      if (TestV < Vinf - TolV)
        sense = 1.;
      else if (TestV > Vsup + TolV)
        sense = -1.;
      while (TestV < Vinf - TolV || TestV > Vsup + TolV)
        TestV += sense * vperiod;
    }
    gp_Vec2d Offset(TestU - MidPoint.X(), TestV - MidPoint.Y());
    if (std::abs(Offset.X()) > gp::Resolution() || std::abs(Offset.Y()) > gp::Resolution())
      myBSpline->Translate(Offset);
    //////////////////////////////////////////
    Geom2dAdaptor_Curve            GAC(myBSpline);
    occ::handle<Adaptor2d_Curve2d> IC2d = new Geom2dAdaptor_Curve(GAC);
#ifdef OCCT_DEBUG
//    char name [100];
//    Sprintf(name,"%s_%d","build",compteur++);
//    DrawTrSurf::Set(name,myBSpline);
#endif
    return IC2d;
  }
  else
  {
    //  Modified by Sergey KHROMOV - Thu Apr 18 10:57:50 2002 Begin
    //     Standard_NoSuchObject_Raise_if(1,"ProjLib_Compu: build echec");
    //  Modified by Sergey KHROMOV - Thu Apr 18 10:57:51 2002 End
    return occ::handle<Adaptor2d_Curve2d>();
  }
  //  myProjIsDone = false;
  //  Modified by Sergey KHROMOV - Thu Apr 18 10:58:01 2002 Begin
  //   Standard_NoSuchObject_Raise_if(1,"ProjLib_ComputeOnPS: build echec");
  //  Modified by Sergey KHROMOV - Thu Apr 18 10:58:02 2002 End
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> ProjLib_ComputeApproxOnPolarSurface::ProjectUsingInitialCurve2d(
  const occ::handle<Adaptor3d_Curve>&   Curve,
  const occ::handle<Adaptor3d_Surface>& Surf,
  const occ::handle<Adaptor2d_Curve2d>& InitCurve2d)
{
  // OCC217
  double Tol3d     = myTolerance;
  double DistTol3d = 100.0 * Tol3d;
  if (myMaxDist > 0.)
  {
    DistTol3d = myMaxDist;
  }
  double DistTol3d2 = DistTol3d * DistTol3d;
  double TolU = Surf->UResolution(Tol3d), TolV = Surf->VResolution(Tol3d);
  double Tol2d = std::max(std::sqrt(TolU * TolU + TolV * TolV), Precision::PConfusion());

  int                 i;
  GeomAbs_SurfaceType TheTypeS = Surf->GetType();
  GeomAbs_CurveType   TheTypeC = Curve->GetType();
  if (TheTypeS == GeomAbs_Plane)
  {
    double S, T;
    gp_Pln Plane = Surf->Plane();
    if (TheTypeC == GeomAbs_BSplineCurve)
    {
      myTolReached                       = Precision::Confusion();
      occ::handle<Geom_BSplineCurve> BSC = Curve->BSpline();
      NCollection_Array1<gp_Pnt2d>   Poles2d(1, Curve->NbPoles());
      for (i = 1; i <= Curve->NbPoles(); i++)
      {
        ElSLib::Parameters(Plane, BSC->Pole(i), S, T);
        Poles2d(i).SetCoord(S, T);
      }
      const NCollection_Array1<double>& Knots = BSC->Knots();
      const NCollection_Array1<int>&    Mults = BSC->Multiplicities();
      if (BSC->IsRational())
      {
        const NCollection_Array1<double>* pWeights = BSC->Weights();
        if (pWeights)
        {
          return new Geom2d_BSplineCurve(Poles2d,
                                         *pWeights,
                                         Knots,
                                         Mults,
                                         BSC->Degree(),
                                         BSC->IsPeriodic());
        }
      }
      return new Geom2d_BSplineCurve(Poles2d, Knots, Mults, BSC->Degree(), BSC->IsPeriodic());
    }
    if (TheTypeC == GeomAbs_BezierCurve)
    {
      myTolReached                     = Precision::Confusion();
      occ::handle<Geom_BezierCurve> BC = Curve->Bezier();
      NCollection_Array1<gp_Pnt2d>  Poles2d(1, Curve->NbPoles());
      for (i = 1; i <= Curve->NbPoles(); i++)
      {
        ElSLib::Parameters(Plane, BC->Pole(i), S, T);
        Poles2d(i).SetCoord(S, T);
      }
      NCollection_Array1<double> Knots(1, 2);
      Knots.SetValue(1, 0.0);
      Knots.SetValue(2, 1.0);
      NCollection_Array1<int> Mults(1, 2);
      Mults.Init(BC->NbPoles());
      if (BC->IsRational())
      {
        const NCollection_Array1<double>* pWeights = BC->Weights();
        if (pWeights)
        {
          return new Geom2d_BSplineCurve(Poles2d,
                                         *pWeights,
                                         Knots,
                                         Mults,
                                         BC->Degree(),
                                         BC->IsPeriodic());
        }
      }
      return new Geom2d_BSplineCurve(Poles2d, Knots, Mults, BC->Degree(), BC->IsPeriodic());
    }
  }
  if (TheTypeS == GeomAbs_BSplineSurface)
  {
    occ::handle<Geom_BSplineSurface> BSS = Surf->BSpline();
    if ((BSS->MaxDegree() == 1) && (BSS->NbUPoles() == 2) && (BSS->NbVPoles() == 2))
    {
      gp_Pnt p11 = BSS->Pole(1, 1);
      gp_Pnt p12 = BSS->Pole(1, 2);
      gp_Pnt p21 = BSS->Pole(2, 1);
      gp_Pnt p22 = BSS->Pole(2, 2);
      gp_Vec V1(p11, p12);
      gp_Vec V2(p21, p22);
      if (V1.IsEqual(V2, Tol3d, Tol3d / (p11.Distance(p12) * 180 / M_PI)))
      {
        int    Dist2Min = IntegerLast();
        double u, v;
        if (TheTypeC == GeomAbs_BSplineCurve)
        {
          myTolReached                       = Tol3d;
          occ::handle<Geom_BSplineCurve> BSC = Curve->BSpline();
          NCollection_Array1<gp_Pnt2d>   Poles2d(1, Curve->NbPoles());
          for (i = 1; i <= Curve->NbPoles(); i++)
          {
            myProjIsDone = false;
            Dist2Min     = IntegerLast();

            Extrema_GenLocateExtPS extrloc(*Surf, TolU, TolV);
            extrloc.Perform(BSC->Pole(i), (p11.X() + p22.X()) / 2, (p11.Y() + p22.Y()) / 2);

            if (extrloc.IsDone())
            {
              Dist2Min = (int)extrloc.SquareDistance();
              if (Dist2Min < DistTol3d2)
              {
                (extrloc.Point()).Parameter(u, v);
                Poles2d(i).SetCoord(u, v);
                myProjIsDone = true;
              }
              else
                break;
            }
            else
              break;
            if (!myProjIsDone)
              break;
          }
          if (myProjIsDone)
          {
            const NCollection_Array1<double>& Knots = BSC->Knots();
            const NCollection_Array1<int>&    Mults = BSC->Multiplicities();
            if (BSC->IsRational())
            {
              return new Geom2d_BSplineCurve(Poles2d,
                                             BSC->WeightsArray(),
                                             Knots,
                                             Mults,
                                             BSC->Degree(),
                                             BSC->IsPeriodic());
            }
            return new Geom2d_BSplineCurve(Poles2d, Knots, Mults, BSC->Degree(), BSC->IsPeriodic());
          }
        }
        if (TheTypeC == GeomAbs_BezierCurve)
        {
          myTolReached                     = Tol3d;
          occ::handle<Geom_BezierCurve> BC = Curve->Bezier();
          NCollection_Array1<gp_Pnt2d>  Poles2d(1, Curve->NbPoles());
          for (i = 1; i <= Curve->NbPoles(); i++)
          {
            Dist2Min = IntegerLast();

            Extrema_GenLocateExtPS extrloc(*Surf, TolU, TolV);
            extrloc.Perform(BC->Pole(i), 0.5, 0.5);

            if (extrloc.IsDone())
            {
              Dist2Min = (int)extrloc.SquareDistance();
              if (Dist2Min < DistTol3d2)
              {
                (extrloc.Point()).Parameter(u, v);
                Poles2d(i).SetCoord(u, v);
                myProjIsDone = true;
              }
              else
                break;
            }
            else
              break;
            if (myProjIsDone)
              myProjIsDone = false;
            else
              break;
          }
          if (myProjIsDone)
          {
            NCollection_Array1<double> Knots(1, 2);
            Knots.SetValue(1, 0.0);
            Knots.SetValue(2, 1.0);
            NCollection_Array1<int> Mults(1, 2);
            Mults.Init(BC->NbPoles());
            if (BC->IsRational())
            {
              return new Geom2d_BSplineCurve(Poles2d,
                                             BC->WeightsArray(),
                                             Knots,
                                             Mults,
                                             BC->Degree(),
                                             BC->IsPeriodic());
            }
            return new Geom2d_BSplineCurve(Poles2d, Knots, Mults, BC->Degree(), BC->IsPeriodic());
          }
        }
      }
    }
  }
  else if (TheTypeS == GeomAbs_BezierSurface)
  {
    occ::handle<Geom_BezierSurface> BS = Surf->Bezier();
    if ((BS->MaxDegree() == 1) && (BS->NbUPoles() == 2) && (BS->NbVPoles() == 2))
    {
      gp_Pnt p11 = BS->Pole(1, 1);
      gp_Pnt p12 = BS->Pole(1, 2);
      gp_Pnt p21 = BS->Pole(2, 1);
      gp_Pnt p22 = BS->Pole(2, 2);
      gp_Vec V1(p11, p12);
      gp_Vec V2(p21, p22);
      if (V1.IsEqual(V2, Tol3d, Tol3d / (p11.Distance(p12) * 180 / M_PI)))
      {
        int    Dist2Min = IntegerLast();
        double u, v;

        //	gp_Pnt pntproj;
        if (TheTypeC == GeomAbs_BSplineCurve)
        {
          myTolReached                       = Tol3d;
          occ::handle<Geom_BSplineCurve> BSC = Curve->BSpline();
          NCollection_Array1<gp_Pnt2d>   Poles2d(1, Curve->NbPoles());
          for (i = 1; i <= Curve->NbPoles(); i++)
          {
            myProjIsDone = false;
            Dist2Min     = IntegerLast();

            Extrema_GenLocateExtPS extrloc(*Surf, TolU, TolV);
            extrloc.Perform(BSC->Pole(i), (p11.X() + p22.X()) / 2, (p11.Y() + p22.Y()) / 2);

            if (extrloc.IsDone())
            {
              Dist2Min = (int)extrloc.SquareDistance();
              if (Dist2Min < DistTol3d2)
              {
                (extrloc.Point()).Parameter(u, v);
                Poles2d(i).SetCoord(u, v);
                myProjIsDone = true;
              }
              else
                break;
            }
            else
              break;
            if (!myProjIsDone)
              break;
          }
          if (myProjIsDone)
          {
            const NCollection_Array1<double>& Knots = BSC->Knots();
            const NCollection_Array1<int>&    Mults = BSC->Multiplicities();
            if (BSC->IsRational())
            {
              return new Geom2d_BSplineCurve(Poles2d,
                                             BSC->WeightsArray(),
                                             Knots,
                                             Mults,
                                             BSC->Degree(),
                                             BSC->IsPeriodic());
            }
            return new Geom2d_BSplineCurve(Poles2d, Knots, Mults, BSC->Degree(), BSC->IsPeriodic());
          }
        }
        if (TheTypeC == GeomAbs_BezierCurve)
        {
          myTolReached                     = Tol3d;
          occ::handle<Geom_BezierCurve> BC = Curve->Bezier();
          NCollection_Array1<gp_Pnt2d>  Poles2d(1, Curve->NbPoles());
          for (i = 1; i <= Curve->NbPoles(); i++)
          {
            Dist2Min = IntegerLast();

            Extrema_GenLocateExtPS extrloc(*Surf, TolU, TolV);
            extrloc.Perform(BC->Pole(i), 0.5, 0.5);

            if (extrloc.IsDone())
            {
              Dist2Min = (int)extrloc.SquareDistance();
              if (Dist2Min < DistTol3d2)
              {
                (extrloc.Point()).Parameter(u, v);
                Poles2d(i).SetCoord(u, v);
                myProjIsDone = true;
              }
              else
                break;
            }
            else
              break;
            if (myProjIsDone)
              myProjIsDone = false;
            else
              break;
          }
          if (myProjIsDone)
          {
            NCollection_Array1<double> Knots(1, 2);
            Knots.SetValue(1, 0.0);
            Knots.SetValue(2, 1.0);
            NCollection_Array1<int> Mults(1, 2);
            Mults.Init(BC->NbPoles());
            if (BC->IsRational())
            {
              return new Geom2d_BSplineCurve(Poles2d,
                                             BC->WeightsArray(),
                                             Knots,
                                             Mults,
                                             BC->Degree(),
                                             BC->IsPeriodic());
            }
            return new Geom2d_BSplineCurve(Poles2d, Knots, Mults, BC->Degree(), BC->IsPeriodic());
          }
        }
      }
    }
  }

  ProjLib_PolarFunction F(Curve, Surf, InitCurve2d, Tol3d);

#ifdef OCCT_DEBUG
  int Nb = 50;

  double U, U1, U2;
  U1 = F.FirstParameter();
  U2 = F.LastParameter();

  NCollection_Array1<gp_Pnt2d> DummyPoles(1, Nb + 1);
  NCollection_Array1<double>   DummyKnots(1, Nb + 1);
  NCollection_Array1<int>      DummyMults(1, Nb + 1);
  DummyMults.Init(1);
  DummyMults(1)      = 2;
  DummyMults(Nb + 1) = 2;
  for (int ij = 0; ij <= Nb; ij++)
  {
    U = (Nb - ij) * U1 + ij * U2;
    U /= Nb;
    DummyPoles(ij + 1) = F.Value(U);
    DummyKnots(ij + 1) = ij;
  }
  occ::handle<Geom2d_BSplineCurve> DummyC2d =
    new Geom2d_BSplineCurve(DummyPoles, DummyKnots, DummyMults, 1);
  //  DrawTrSurf::Set((const char* ) "bs2d",DummyC2d);
  occ::handle<Geom2dAdaptor_Curve> DDD = occ::down_cast<Geom2dAdaptor_Curve>(InitCurve2d);

//  DrawTrSurf::Set((const char* ) "initc2d",DDD->ChangeCurve2d().Curve());
#endif

  int Deg1, Deg2;
  Deg1 = 2;
  Deg2 = 8;
  if (myDegMin > 0)
  {
    Deg1 = myDegMin;
  }
  if (myDegMax > 0)
  {
    Deg2 = myDegMax;
  }
  int aMaxSegments = 1000;
  if (myMaxSegments > 0)
  {
    aMaxSegments = myMaxSegments;
  }
  AppParCurves_Constraint aFistC = AppParCurves_TangencyPoint, aLastC = AppParCurves_TangencyPoint;
  if (myBndPnt != AppParCurves_TangencyPoint)
  {
    aFistC = myBndPnt;
    aLastC = myBndPnt;
  }

  if (myDist > 10. * Tol3d)
  {
    aFistC = AppParCurves_PassPoint;
    aLastC = AppParCurves_PassPoint;
  }

  Approx_FitAndDivide2d Fit(Deg1, Deg2, Tol3d, Tol2d, true, aFistC, aLastC);
  Fit.SetMaxSegments(aMaxSegments);
  if (InitCurve2d->GetType() == GeomAbs_Line)
  {
    Fit.SetInvOrder(false);
  }
  Fit.Perform(F);

  double anOldTol2d = Tol2d;
  double aNewTol2d  = 0;
  if (Fit.IsAllApproximated())
  {
    int j;
    int NbCurves = Fit.NbMultiCurves();
    int MaxDeg   = 0;
    // To transform the MultiCurve into BSpline, it is required that all
    // Bezier constituing it have the same degree -> Calculation of MaxDeg
    int NbPoles = 1;
    for (j = 1; j <= NbCurves; j++)
    {
      int Deg = Fit.Value(j).Degree();
      MaxDeg  = std::max(MaxDeg, Deg);
      Fit.Error(j, Tol3d, Tol2d);
      aNewTol2d = std::max(aNewTol2d, Tol2d);
    }
    //
    myTolReached = std::max(myTolReached, myTolerance * (aNewTol2d / anOldTol2d));
    //
    NbPoles = MaxDeg * NbCurves + 1; // Tops on the BSpline
    NCollection_Array1<gp_Pnt2d> Poles(1, NbPoles);

    NCollection_Array1<gp_Pnt2d> TempPoles(1, MaxDeg + 1); // to augment the degree

    NCollection_Array1<double> Knots(1, NbCurves + 1); // Nodes of the BSpline

    int Compt = 1;
    for (i = 1; i <= NbCurves; i++)
    {
      Fit.Parameters(i, Knots(i), Knots(i + 1));
      AppParCurves_MultiCurve      MC = Fit.Value(i);           // Load the Ith Curve
      NCollection_Array1<gp_Pnt2d> Poles2d(1, MC.Degree() + 1); // Retrieve the tops
      MC.Curve(1, Poles2d);

      // Eventual augmentation of the degree
      int Inc = MaxDeg - MC.Degree();
      if (Inc > 0)
      {
        //	BSplCLib::IncreaseDegree( Inc, Poles2d, PLib::NoWeights(),
        BSplCLib::IncreaseDegree(MaxDeg,
                                 Poles2d,
                                 BSplCLib::NoWeights(),
                                 TempPoles,
                                 BSplCLib::NoWeights());
        // update of tops of the PCurve
        for (int k = 1; k <= MaxDeg + 1; k++)
        {
          Poles.SetValue(Compt, TempPoles(k));
          Compt++;
        }
      }
      else
      {
        // update of tops of the PCurve
        for (int k = 1; k <= MaxDeg + 1; k++)
        {
          Poles.SetValue(Compt, Poles2d(k));
          Compt++;
        }
      }

      Compt--;
    }

    // update of fields of  ProjLib_Approx
    int NbKnots = NbCurves + 1;

    NCollection_Array1<int> Mults(1, NbKnots);
    Mults.Init(MaxDeg);
    Mults.SetValue(1, MaxDeg + 1);
    Mults.SetValue(NbKnots, MaxDeg + 1);
    myProjIsDone                           = true;
    occ::handle<Geom2d_BSplineCurve> Dummy = new Geom2d_BSplineCurve(Poles, Knots, Mults, MaxDeg);

    // try to smoother the Curve GeomAbs_C1.

    bool   OK         = true;
    double aSmoothTol = std::max(Precision::Confusion(), aNewTol2d);
    if (myBndPnt == AppParCurves_PassPoint)
    {
      aSmoothTol *= 10.;
    }
    for (int ij = 2; ij < NbKnots; ij++)
    {
      OK = OK && Dummy->RemoveKnot(ij, MaxDeg - 1, aSmoothTol);
    }
#ifdef OCCT_DEBUG
    if (!OK)
    {
      std::cout << "ProjLib_ComputeApproxOnPolarSurface : Smoothing echoue" << std::endl;
    }
#endif
    return Dummy;
  }
  return occ::handle<Geom2d_BSplineCurve>();
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> ProjLib_ComputeApproxOnPolarSurface::BSpline() const

{
  return myBSpline;
}

//=================================================================================================

occ::handle<Geom2d_Curve> ProjLib_ComputeApproxOnPolarSurface::Curve2d() const

{
  Standard_NoSuchObject_Raise_if(!myProjIsDone, "ProjLib_ComputeApproxOnPolarSurface:2ndCurve2d");
  return my2ndCurve;
}

//=================================================================================================

bool ProjLib_ComputeApproxOnPolarSurface::IsDone() const

{
  return myProjIsDone;
}

//=================================================================================================

void ProjLib_ComputeApproxOnPolarSurface::SetTolerance(const double theTol)

{
  myTolerance = theTol;
}

//=================================================================================================

void ProjLib_ComputeApproxOnPolarSurface::SetDegree(const int theDegMin, const int theDegMax)
{
  myDegMin = theDegMin;
  myDegMax = theDegMax;
}

//=================================================================================================

void ProjLib_ComputeApproxOnPolarSurface::SetMaxSegments(const int theMaxSegments)
{
  myMaxSegments = theMaxSegments;
}

//=================================================================================================

void ProjLib_ComputeApproxOnPolarSurface::SetBndPnt(const AppParCurves_Constraint theBndPnt)
{
  myBndPnt = theBndPnt;
}

//=================================================================================================

void ProjLib_ComputeApproxOnPolarSurface::SetMaxDist(const double theMaxDist)
{
  myMaxDist = theMaxDist;
}

//=================================================================================================

double ProjLib_ComputeApproxOnPolarSurface::Tolerance() const

{
  return myTolReached;
}
