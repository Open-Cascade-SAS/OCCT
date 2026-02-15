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

#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepGProp_Face.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <math.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

static const double Epsilon1 = Epsilon(1.);

//=================================================================================================

int BRepGProp_Face::UIntegrationOrder() const
{

  int Nu;
  switch (mySurface.GetType())
  {

    case GeomAbs_Plane:
      Nu = 4;
      break;

    case GeomAbs_BezierSurface: {
      Nu = (*((occ::handle<Geom_BezierSurface>*)&((mySurface.Surface()).Surface())))->UDegree() + 1;
      Nu = std::max(4, Nu);
    }
    break;
    case GeomAbs_BSplineSurface: {
      int a =
        (*((occ::handle<Geom_BSplineSurface>*)&((mySurface.Surface()).Surface())))->UDegree() + 1;
      int b =
        (*((occ::handle<Geom_BSplineSurface>*)&((mySurface.Surface()).Surface())))->NbUKnots() - 1;
      Nu = std::max(4, a * b);
    }
    break;

    default:
      Nu = 9;
      break;
  }
  return std::max(8, 2 * Nu);
}

//=================================================================================================

int BRepGProp_Face::VIntegrationOrder() const
{
  int Nv;
  switch (mySurface.GetType())
  {

    case GeomAbs_Plane:
      Nv = 4;
      break;

    case GeomAbs_BezierSurface: {
      Nv = (*((occ::handle<Geom_BezierSurface>*)&((mySurface.Surface()).Surface())))->VDegree() + 1;
      Nv = std::max(4, Nv);
    }
    break;

    case GeomAbs_BSplineSurface: {
      int a =
        (*((occ::handle<Geom_BSplineSurface>*)&((mySurface.Surface()).Surface())))->VDegree() + 1;
      int b =
        (*((occ::handle<Geom_BSplineSurface>*)&((mySurface.Surface()).Surface())))->NbVKnots() - 1;
      Nv = std::max(4, a * b);
    }
    break;

    default:
      Nv = 9;
      break;
  }
  return std::max(8, 2 * Nv);
}

//=================================================================================================

int BRepGProp_Face::IntegrationOrder() const
{
  int N;

  switch (myCurve.GetType())
  {

    case GeomAbs_Line:
      N = 2;
      break;

    case GeomAbs_Circle:
    case GeomAbs_Ellipse:
    case GeomAbs_Hyperbola:
      N = 9;
      break;

    case GeomAbs_Parabola:
      N = 9;
      break;

    case GeomAbs_BezierCurve: {
      N = (*((occ::handle<Geom2d_BezierCurve>*)&(myCurve.Curve())))->Degree() + 1;
    }
    break;

    case GeomAbs_BSplineCurve: {
      int a = (*((occ::handle<Geom2d_BSplineCurve>*)&(myCurve.Curve())))->Degree() + 1;
      int b = (*((occ::handle<Geom2d_BSplineCurve>*)&(myCurve.Curve())))->NbKnots() - 1;
      N     = a * b;
    }
    break;

    default:
      N = 9;
      break;
  }

  return std::max(4, 2 * N);
}

//=================================================================================================

void BRepGProp_Face::Bounds(double& U1, double& U2, double& V1, double& V2) const
{
  U1 = mySurface.FirstUParameter();
  U2 = mySurface.LastUParameter();
  V1 = mySurface.FirstVParameter();
  V2 = mySurface.LastVParameter();
}

//=================================================================================================

bool BRepGProp_Face::Load(const TopoDS_Edge& E)
{
  double a, b;
  if (!myIsFaceContextReady)
  {
    return false;
  }
  occ::handle<Geom2d_Curve> C = BRep_Tool::CurveOnSurface(E, myFaceSurface, myFaceLocation, a, b);
  if (C.IsNull())
  {
    return false;
  }
  if (E.Orientation() == TopAbs_REVERSED)
  {
    double x = a;
    a        = C->ReversedParameter(b);
    b        = C->ReversedParameter(x);
    C        = C->Reversed();
  }
  myCurve.Load(C, a, b);
  return true;
}

//=================================================================================================

void BRepGProp_Face::Load(const TopoDS_Face& F)
{
  TopoDS_Shape aLocalShape = F.Oriented(TopAbs_FORWARD);
  mySurface.Initialize(TopoDS::Face(aLocalShape));
  myFaceSurface        = BRep_Tool::Surface(mySurface.Face(), myFaceLocation);
  myIsFaceContextReady = !myFaceSurface.IsNull();
  //  mySurface.Initialize(TopoDS::Face(F.Oriented(TopAbs_FORWARD)));
  mySReverse = (F.Orientation() == TopAbs_REVERSED);
}

//=================================================================================================

void BRepGProp_Face::Normal(const double U, const double V, gp_Pnt& P, gp_Vec& VNor) const
{
  gp_Vec D1U, D1V;
  mySurface.D1(U, V, P, D1U, D1V);
  VNor = D1U.Crossed(D1V);
  if (mySReverse)
    VNor.Reverse();
}

//  APO 17.04.2002 (OCC104)
// This is functions that calculate coeff. to optimize "integration order".
// They had been produced experimentally for some hard example.
static double AS = -0.15, AL = -0.50, B = 1.0, C = 0.75, D = 0.25;

static inline double SCoeff(const double Eps)
{
  return Eps < 0.1 ? AS * (B + std::log10(Eps)) + C : C;
}

static inline double LCoeff(const double Eps)
{
  return Eps < 0.1 ? AL * (B + std::log10(Eps)) + D : D;
}

//=================================================================================================

int BRepGProp_Face::SIntOrder(const double Eps) const
{
  int Nv, Nu;
  switch (mySurface.GetType())
  {
    case GeomAbs_Plane:
      Nu = 1;
      Nv = 1;
      break;
    case GeomAbs_Cylinder:
      Nu = 2;
      Nv = 1;
      break;
    case GeomAbs_Cone:
      Nu = 2;
      Nv = 1;
      break;
    case GeomAbs_Sphere:
      Nu = 2;
      Nv = 2;
      break;
    case GeomAbs_Torus:
      Nu = 2;
      Nv = 2;
      break;
    case GeomAbs_BezierSurface:
      Nv = (*((occ::handle<Geom_BezierSurface>*)&((mySurface.Surface()).Surface())))->VDegree();
      Nu = (*((occ::handle<Geom_BezierSurface>*)&((mySurface.Surface()).Surface())))->UDegree();
      break;
    case GeomAbs_BSplineSurface:
      Nv = (*((occ::handle<Geom_BSplineSurface>*)&((mySurface.Surface()).Surface())))->VDegree();
      Nu = (*((occ::handle<Geom_BSplineSurface>*)&((mySurface.Surface()).Surface())))->UDegree();
      break;
    default:
      Nu = 2;
      Nv = 2;
      break;
  }
  return std::min(RealToInt(std::ceil(SCoeff(Eps) * std::max((Nu + 1), (Nv + 1)))),
                  math::GaussPointsMax());
}

//=================================================================================================

int BRepGProp_Face::SUIntSubs() const
{
  int N;
  switch (mySurface.GetType())
  {
    case GeomAbs_Plane:
      N = 2;
      break;
    case GeomAbs_Cylinder:
      N = 4;
      break;
    case GeomAbs_Cone:
      N = 4;
      break;
    case GeomAbs_Sphere:
      N = 4;
      break;
    case GeomAbs_Torus:
      N = 4;
      break;
    case GeomAbs_BezierSurface:
      N = 2;
      break;
    case GeomAbs_BSplineSurface:
      N = (*((occ::handle<Geom_BSplineSurface>*)&((mySurface.Surface()).Surface())))->NbUKnots();
      break;
    default:
      N = 2;
      break;
  }
  return N - 1;
}

//=================================================================================================

int BRepGProp_Face::SVIntSubs() const
{
  int N;
  switch (mySurface.GetType())
  {
    case GeomAbs_Plane:
      N = 2;
      break;
    case GeomAbs_Cylinder:
      N = 2;
      break;
    case GeomAbs_Cone:
      N = 2;
      break;
    case GeomAbs_Sphere:
      N = 3;
      break;
    case GeomAbs_Torus:
      N = 4;
      break;
    case GeomAbs_BezierSurface:
      N = 2;
      break;
    case GeomAbs_BSplineSurface:
      N = (*((occ::handle<Geom_BSplineSurface>*)&((mySurface.Surface()).Surface())))->NbVKnots();
      break;
    default:
      N = 2;
      break;
  }
  return N - 1;
}

//=================================================================================================

void BRepGProp_Face::UKnots(NCollection_Array1<double>& Knots) const
{
  switch (mySurface.GetType())
  {
    case GeomAbs_Plane:
      Knots(1) = mySurface.FirstUParameter();
      Knots(2) = mySurface.LastUParameter();
      break;
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
      Knots(1) = 0.0;
      Knots(2) = M_PI * 2.0 / 3.0;
      Knots(3) = M_PI * 4.0 / 3.0;
      Knots(4) = M_PI * 6.0 / 3.0;
      break;
    case GeomAbs_BSplineSurface: {
      const NCollection_Array1<double>& aSrcKnots =
        (*((occ::handle<Geom_BSplineSurface>*)&((mySurface.Surface()).Surface())))->UKnots();
      for (int i = Knots.Lower(); i <= Knots.Upper(); i++)
        Knots(i) = aSrcKnots(i);
    }
    break;
    default:
      Knots(1) = mySurface.FirstUParameter();
      Knots(2) = mySurface.LastUParameter();
      break;
  }
}

//=================================================================================================

void BRepGProp_Face::VKnots(NCollection_Array1<double>& Knots) const
{
  switch (mySurface.GetType())
  {
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
      Knots(1) = mySurface.FirstUParameter();
      Knots(2) = mySurface.LastUParameter();
      break;
    case GeomAbs_Sphere:
      Knots(1) = -M_PI / 2.0;
      Knots(2) = 0.0;
      Knots(3) = +M_PI / 2.0;
      break;
    case GeomAbs_Torus:
      Knots(1) = 0.0;
      Knots(2) = M_PI * 2.0 / 3.0;
      Knots(3) = M_PI * 4.0 / 3.0;
      Knots(4) = M_PI * 6.0 / 3.0;
      break;
    case GeomAbs_BSplineSurface: {
      const NCollection_Array1<double>& aSrcKnots =
        (*((occ::handle<Geom_BSplineSurface>*)&((mySurface.Surface()).Surface())))->VKnots();
      for (int i = Knots.Lower(); i <= Knots.Upper(); i++)
        Knots(i) = aSrcKnots(i);
    }
    break;
    default:
      Knots(1) = mySurface.FirstUParameter();
      Knots(2) = mySurface.LastUParameter();
      break;
  }
}

//=================================================================================================

int BRepGProp_Face::LIntOrder(const double Eps) const
{
  Bnd_Box2d aBox;

  BndLib_Add2dCurve::Add(myCurve, 1.e-7, aBox);
  double aXmin, aXmax, aYmin, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);
  double aVmin = mySurface.FirstVParameter();
  double aVmax = mySurface.LastVParameter();

  double dv  = (aVmax - aVmin);
  double anR = (dv > Epsilon1 ? std::min((aYmax - aYmin) / dv, 1.) : 1.);

  int anRInt = RealToInt(std::ceil(SVIntSubs() * anR));
  int aLSubs = LIntSubs();

  //  double NL, NS = std::max(SIntOrder(1.0)*anRInt/LIntSubs(), 1);
  double NL, NS = std::max(SIntOrder(1.) * anRInt / aLSubs, 1);
  switch (myCurve.GetType())
  {
    case GeomAbs_Line:
      NL = 1;
      break;
    case GeomAbs_Circle:
      NL = 2 * 3;
      break; // correction for the spans of converted curve
    case GeomAbs_Ellipse:
      NL = 2 * 3;
      break; //
    case GeomAbs_Parabola:
      NL = 2 * 3;
      break;
    case GeomAbs_Hyperbola:
      NL = 3 * 3;
      break;
    case GeomAbs_BezierCurve:
      NL = (*((occ::handle<Geom2d_BezierCurve>*)&(myCurve.Curve())))->Degree();
      break;
    case GeomAbs_BSplineCurve:
      NL = (*((occ::handle<Geom2d_BSplineCurve>*)&(myCurve.Curve())))->Degree();
      break;
    default:
      NL = 3 * 3;
      break;
  }

  NL = std::max(NL, NS);

  int nn = RealToInt(aLSubs <= 4 ? std::ceil(LCoeff(Eps) * (NL + 1)) : NL + 1);

  return std::min(nn, math::GaussPointsMax());
}

//=================================================================================================

int BRepGProp_Face::LIntSubs() const
{
  int N;
  switch (myCurve.GetType())
  {
    case GeomAbs_Line:
      N = 2;
      break;
    case GeomAbs_Circle:
    case GeomAbs_Ellipse:
      N = 4;
      break;
    case GeomAbs_Parabola:
    case GeomAbs_Hyperbola:
      N = 2;
      break;
    case GeomAbs_BSplineCurve:
      N = (*((occ::handle<Geom2d_BSplineCurve>*)&(myCurve.Curve())))->NbKnots();
      break;
    default:
      N = 2;
      break;
  }
  return N - 1;
}

//=================================================================================================

void BRepGProp_Face::LKnots(NCollection_Array1<double>& Knots) const
{
  switch (myCurve.GetType())
  {
    case GeomAbs_Line:
      Knots(1) = myCurve.FirstParameter();
      Knots(2) = myCurve.LastParameter();
      break;
    case GeomAbs_Circle:
    case GeomAbs_Ellipse:
      Knots(1) = 0.0;
      Knots(2) = M_PI * 2.0 / 3.0;
      Knots(3) = M_PI * 4.0 / 3.0;
      Knots(4) = M_PI * 6.0 / 3.0;
      break;
    case GeomAbs_Parabola:
    case GeomAbs_Hyperbola:
      Knots(1) = myCurve.FirstParameter();
      Knots(2) = myCurve.LastParameter();
      break;
    case GeomAbs_BSplineCurve: {
      const NCollection_Array1<double>& aSrcKnots =
        (*((occ::handle<Geom2d_BSplineCurve>*)&(myCurve.Curve())))->Knots();
      for (int i = Knots.Lower(); i <= Knots.Upper(); i++)
        Knots(i) = aSrcKnots(i);
    }
    break;
    default:
      Knots(1) = myCurve.FirstParameter();
      Knots(2) = myCurve.LastParameter();
      break;
  }
}

//=================================================================================================

void BRepGProp_Face::Load(const bool IsFirstParam, const GeomAbs_IsoType theIsoType)
{
  double   aLen;
  double   aU1;
  double   aU2;
  double   aV1;
  double   aV2;
  gp_Pnt2d aLoc;
  gp_Dir2d aDir;

  Bounds(aU1, aU2, aV1, aV2);

  if (theIsoType == GeomAbs_IsoU)
  {
    aLen = aV2 - aV1;

    if (IsFirstParam)
    {
      aLoc.SetCoord(aU1, aV2);
      aDir.SetCoord(0., -1.);
    }
    else
    {
      aLoc.SetCoord(aU2, aV1);
      aDir.SetCoord(0., 1.);
    }
  }
  else if (theIsoType == GeomAbs_IsoV)
  {
    aLen = aU2 - aU1;

    if (IsFirstParam)
    {
      aLoc.SetCoord(aU1, aV1);
      aDir.SetCoord(1., 0.);
    }
    else
    {
      aLoc.SetCoord(aU2, aV2);
      aDir.SetCoord(-1., 0.);
    }
  }
  else
    return;

  occ::handle<Geom2d_Curve> aLin = new Geom2d_Line(aLoc, aDir);

  myCurve.Load(aLin, 0., aLen);
}

//=================================================================================================

static void GetRealKnots(const double                                    theMin,
                         const double                                    theMax,
                         const occ::handle<NCollection_HArray1<double>>& theKnots,
                         occ::handle<NCollection_HArray1<double>>&       theRealKnots)
{
  int              i       = theKnots->Lower() - 1;
  int              iU      = theKnots->Upper();
  int              aStartI = 0;
  int              aEndI   = 0;
  constexpr double aTol    = Precision::Confusion();

  while (++i < iU)
  {
    if (aStartI == 0 && theKnots->Value(i) > theMin + aTol)
      aStartI = i;

    if (aEndI == 0 && theKnots->Value(i + 1) > theMax - aTol)
      aEndI = i;

    if (aStartI != 0 && aEndI != 0)
      break;
  }

  if (aStartI == 0)
    aStartI = iU;

  int aNbNode = std::max(0, aEndI - aStartI + 1) + 2;
  int j;

  theRealKnots = new NCollection_HArray1<double>(1, aNbNode);
  theRealKnots->SetValue(1, theMin);
  theRealKnots->SetValue(aNbNode, theMax);

  for (i = 2, j = aStartI; j <= aEndI; i++, j++)
    theRealKnots->SetValue(i, theKnots->Value(j));
}

//=================================================================================================

static void GetCurveKnots(const double                              theMin,
                          const double                              theMax,
                          const Geom2dAdaptor_Curve&                theCurve,
                          occ::handle<NCollection_HArray1<double>>& theKnots)
{
  bool isSBSpline = theCurve.GetType() == GeomAbs_BSplineCurve;

  if (isSBSpline)
  {
    occ::handle<Geom2d_BSplineCurve>         aCrv;
    occ::handle<NCollection_HArray1<double>> aCrvKnots;

    aCrv      = occ::down_cast<Geom2d_BSplineCurve>(theCurve.Curve());
    aCrvKnots = new NCollection_HArray1<double>(aCrv->Knots());
    GetRealKnots(theMin, theMax, aCrvKnots, theKnots);
  }
  else
  {
    theKnots = new NCollection_HArray1<double>(1, 2);
    theKnots->SetValue(1, theMin);
    theKnots->SetValue(2, theMax);
  }
}

//=================================================================================================

void BRepGProp_Face::GetUKnots(const double                              theUMin,
                               const double                              theUMax,
                               occ::handle<NCollection_HArray1<double>>& theUKnots) const
{
  bool isSBSpline = mySurface.GetType() == GeomAbs_BSplineSurface;
  bool isCBSpline = false;

  if (!isSBSpline)
  {
    // Check the basis curve of the surface of linear extrusion.
    if (mySurface.GetType() == GeomAbs_SurfaceOfExtrusion)
    {
      GeomAdaptor_Curve         aCurve;
      occ::handle<Geom_Surface> aSurf = mySurface.Surface().Surface();

      aCurve.Load(occ::down_cast<Geom_SurfaceOfLinearExtrusion>(aSurf)->BasisCurve());
      isCBSpline = aCurve.GetType() == GeomAbs_BSplineCurve;
    }
  }

  if (myIsUseSpan && (isSBSpline || isCBSpline))
  {
    // Using span decomposition for BSpline.
    occ::handle<NCollection_HArray1<double>> aKnots;

    if (isSBSpline)
    {
      // Get U knots of BSpline surface.
      occ::handle<Geom_Surface>        aSurf = mySurface.Surface().Surface();
      occ::handle<Geom_BSplineSurface> aBSplSurf;

      aBSplSurf = occ::down_cast<Geom_BSplineSurface>(aSurf);
      aKnots    = new NCollection_HArray1<double>(aBSplSurf->UKnots());
    }
    else
    {
      // Get U knots of BSpline curve - basis curve of
      // the surface of linear extrusion.
      GeomAdaptor_Curve              aCurve;
      occ::handle<Geom_Surface>      aSurf = mySurface.Surface().Surface();
      occ::handle<Geom_BSplineCurve> aBSplCurve;

      aCurve.Load(occ::down_cast<Geom_SurfaceOfLinearExtrusion>(aSurf)->BasisCurve());
      aBSplCurve = aCurve.BSpline();
      aKnots     = new NCollection_HArray1<double>(aBSplCurve->Knots());
    }

    // Compute number of knots inside theUMin and theUMax.
    GetRealKnots(theUMin, theUMax, aKnots, theUKnots);
  }
  else
  {
    // No span decomposition.
    theUKnots = new NCollection_HArray1<double>(1, 2);
    theUKnots->SetValue(1, theUMin);
    theUKnots->SetValue(2, theUMax);
  }
}

//=================================================================================================

void BRepGProp_Face::GetTKnots(const double                              theTMin,
                               const double                              theTMax,
                               occ::handle<NCollection_HArray1<double>>& theTKnots) const
{
  bool isBSpline = mySurface.GetType() == GeomAbs_BSplineSurface;

  if (myIsUseSpan && isBSpline)
  {
    // Using span decomposition for BSpline.
    occ::handle<NCollection_HArray1<double>> aSurfKnots;

    // Get V knots of BSpline surface.
    occ::handle<Geom_Surface>        aSurf = mySurface.Surface().Surface();
    occ::handle<Geom_BSplineSurface> aBSplSurf;

    aBSplSurf  = occ::down_cast<Geom_BSplineSurface>(aSurf);
    aSurfKnots = new NCollection_HArray1<double>(aBSplSurf->VKnots());

    //     occ::handle<NCollection_HArray1<double>> aCurveKnots;

    //     GetCurveKnots(theTMin, theTMax, myCurve, aCurveKnots);
    //    GetRealCurveKnots(aCurveKnots, aSurfKnots, myCurve, theTKnots);
    GetCurveKnots(theTMin, theTMax, myCurve, theTKnots);
  }
  else
  {
    theTKnots = new NCollection_HArray1<double>(1, 2);
    theTKnots->SetValue(1, theTMin);
    theTKnots->SetValue(2, theTMax);
  }
}
