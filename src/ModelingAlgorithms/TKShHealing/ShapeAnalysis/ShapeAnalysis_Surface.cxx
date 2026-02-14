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

// 06.01.99 pdn private method SurfaceNewton PRO17015: fix against hang in Extrema
// 11.01.99 pdn PRO10109 4517: protect against wrong result
//%12 pdn 11.02.99 PRO9234 project degenerated
// 03.03.99 rln S4135: new algorithms for IsClosed (accepts precision), Degenerated (stores
// precision)
//: p7 abv 10.03.99 PRO18206: adding new method IsDegenerated()
//: p8 abv 11.03.99 PRO7226 #489490: improving ProjectDegenerated() for degenerated edges
//: q1 pdn, abv 15.03.99 PRO7226 #525030: adding maxpreci in NextValueOfUV()
//: q2 abv 16.03.99: PRO7226 #412920: applying Newton algorithm before UVFromIso()
//: q6 abv 19.03.99: ie_soapbox-B.stp #390760: improving projecting point on surface
// #77 rln 15.03.99: S4135: returning singularity which has minimum gap between singular point and
// input 3D point
//: r3 abv 30.03.99: (by smh) S4163: protect against unexpected signals
//: #4 smh 07.04.99: S4163 Zero divide.
// #4  szv           S4163: optimizations
//: r9 abv 09.04.99: id_turbine-C.stp #3865: check degenerated 2d point by recomputing to 3d instead
//: of Resolution s5 abv 22.04.99  Adding debug printouts in catch {} blocks

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Message.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>

#include <cassert>

IMPLEMENT_STANDARD_RTTIEXT(ShapeAnalysis_Surface, Standard_Transient)

namespace
{
inline void RestrictBounds(double& theFirst, double& theLast)
{
  bool isFInf = Precision::IsNegativeInfinite(theFirst);
  bool isLInf = Precision::IsPositiveInfinite(theLast);
  if (isFInf || isLInf)
  {
    if (isFInf && isLInf)
    {
      theFirst = -1000;
      theLast  = 1000;
    }
    else if (isFInf)
    {
      theFirst = theLast - 2000;
    }
    else
    {
      theLast = theFirst + 2000;
    }
  }
}

inline void RestrictBounds(double& theUf, double& theUl, double& theVf, double& theVl)
{
  RestrictBounds(theUf, theUl);
  RestrictBounds(theVf, theVl);
}
} // namespace

// S4135
// S4135
//=================================================================================================

ShapeAnalysis_Surface::ShapeAnalysis_Surface(const occ::handle<Geom_Surface>& S)
    : mySurf(S),
      myNbDeg(-1),
      myIsos(false),
      myIsoBoxes(false),
      myGap(0.),
      myUDelt(0.01),
      myVDelt(0.01),
      myUCloseVal(-1),
      myVCloseVal(-1)
{
  // Bug 33895: Prevent crash when surface is null
  if (mySurf.IsNull())
  {
    Message::SendWarning("ShapeAnalysis_Surface: Cannot create with null surface");
    assert(!mySurf.IsNull());
    return;
  }
  mySurf->Bounds(myUF, myUL, myVF, myVL);
  myAdSur = new GeomAdaptor_Surface(mySurf);
}

//=================================================================================================

void ShapeAnalysis_Surface::Init(const occ::handle<Geom_Surface>& theSurface)
{
  if (mySurf == theSurface)
    return;
  // Bug 33895: Prevent crash when surface is null
  if (theSurface.IsNull())
  {
    Message::SendWarning("ShapeAnalysis_Surface: Cannot initialize with null surface");
    assert(!theSurface.IsNull());
    return;
  }
  myExtPS     = ExtremaPS_Surface(); // Reset to uninitialized state
  mySurf      = theSurface;
  myNbDeg     = -1;
  myUCloseVal = myVCloseVal = -1;
  myGap                     = 0.;
  mySurf->Bounds(myUF, myUL, myVF, myVL);
  myAdSur    = new GeomAdaptor_Surface(mySurf);
  myIsos     = false;
  myIsoBoxes = false;
  myIsoUF.Nullify();
  myIsoUL.Nullify();
  myIsoVF.Nullify();
  myIsoVL.Nullify();
}

//=================================================================================================

void ShapeAnalysis_Surface::Init(const occ::handle<ShapeAnalysis_Surface>& other)
{
  Init(other->Surface());
  myAdSur = other->TrueAdaptor3d();
  myNbDeg = other->myNbDeg; // rln S4135 direct transmission (to avoid computation in <other>)
  for (int i = 0; i < myNbDeg; i++)
  {
    other->Singularity(i + 1,
                       myPreci[i],
                       myP3d[i],
                       myFirstP2d[i],
                       myLastP2d[i],
                       myFirstPar[i],
                       myLastPar[i],
                       myUIsoDeg[i]);
  }
}

//=================================================================================================

const occ::handle<GeomAdaptor_Surface>& ShapeAnalysis_Surface::Adaptor3d()
{
  return myAdSur;
}

//=================================================================================================

void ShapeAnalysis_Surface::ComputeSingularities()
{
  // rln S4135
  if (myNbDeg >= 0)
    return;
  //: 51 abv 22 Dec 97: allow forcing:  if (myNbDeg >= 0) return;
  // CKY 27-FEV-98 : en appel direct on peut forcer. En appel interne on optimise
  if (mySurf.IsNull())
    return;

  double su1, sv1, su2, sv2;
  //  mySurf->Bounds(su1, su2, sv1, sv2);
  Bounds(su1, su2, sv1, sv2); // modified by rln on 12/11/97 mySurf-> is deleted

  myNbDeg = 0; //: r3

  if (mySurf->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
  {
    occ::handle<Geom_ConicalSurface> conicS = occ::down_cast<Geom_ConicalSurface>(mySurf);
    double                           vApex  = -conicS->RefRadius() / std::sin(conicS->SemiAngle());
    myPreci[0]                              = 0;
    myP3d[0]                                = conicS->Apex();
    myFirstP2d[0].SetCoord(su1, vApex);
    myLastP2d[0].SetCoord(su2, vApex);
    myFirstPar[0] = su1;
    myLastPar[0]  = su2;
    myUIsoDeg[0]  = false;
    myNbDeg       = 1;
  }
  else if (mySurf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
  {
    occ::handle<Geom_ToroidalSurface> toroidS = occ::down_cast<Geom_ToroidalSurface>(mySurf);
    double                            minorR  = toroidS->MinorRadius();
    double                            majorR  = toroidS->MajorRadius();
    // szv#4:S4163:12Mar99 warning - possible div by zero
    double Ang = std::acos(std::min(1., majorR / minorR));
    myPreci[0] = myPreci[1] = std::max(0., majorR - minorR);
    myP3d[0]                = mySurf->Value(0., M_PI - Ang);
    myFirstP2d[0].SetCoord(su1, M_PI - Ang);
    myLastP2d[0].SetCoord(su2, M_PI - Ang);
    myP3d[1] = mySurf->Value(0., M_PI + Ang);
    myFirstP2d[1].SetCoord(su2, M_PI + Ang);
    myLastP2d[1].SetCoord(su1, M_PI + Ang);
    myFirstPar[0] = myFirstPar[1] = su1;
    myLastPar[0] = myLastPar[1] = su2;
    myUIsoDeg[0] = myUIsoDeg[1] = false;
    myNbDeg                     = (majorR > minorR ? 1 : 2);
  }
  else if (mySurf->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
  {
    myPreci[0] = myPreci[1] = 0;
    myP3d[0]                = mySurf->Value(su1, sv2); // Northern pole is first
    myP3d[1]                = mySurf->Value(su1, sv1);
    myFirstP2d[0].SetCoord(su2, sv2);
    myLastP2d[0].SetCoord(su1, sv2);
    myFirstP2d[1].SetCoord(su1, sv1);
    myLastP2d[1].SetCoord(su2, sv1);
    myFirstPar[0] = myFirstPar[1] = su1;
    myLastPar[0] = myLastPar[1] = su2;
    myUIsoDeg[0] = myUIsoDeg[1] = false;
    myNbDeg                     = 2;
  }
  else if ((mySurf->IsKind(STANDARD_TYPE(Geom_BoundedSurface)))
           || (mySurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) || //: b2 abv 18 Feb 98
           (mySurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface))))
  { // rln S4135

    // rln S4135 //:r3
    myP3d[0] = myAdSur->Value(su1, 0.5 * (sv1 + sv2));
    myFirstP2d[0].SetCoord(su1, sv2);
    myLastP2d[0].SetCoord(su1, sv1);

    myP3d[1] = myAdSur->Value(su2, 0.5 * (sv1 + sv2));
    myFirstP2d[1].SetCoord(su2, sv1);
    myLastP2d[1].SetCoord(su2, sv2);

    myP3d[2] = myAdSur->Value(0.5 * (su1 + su2), sv1);
    myFirstP2d[2].SetCoord(su1, sv1);
    myLastP2d[2].SetCoord(su2, sv1);

    myP3d[3] = myAdSur->Value(0.5 * (su1 + su2), sv2);
    myFirstP2d[3].SetCoord(su2, sv2);
    myLastP2d[3].SetCoord(su1, sv2);

    myFirstPar[0] = myFirstPar[1] = sv1;
    myLastPar[0] = myLastPar[1] = sv2;
    myUIsoDeg[0] = myUIsoDeg[1] = true;

    myFirstPar[2] = myFirstPar[3] = su1;
    myLastPar[2] = myLastPar[3] = su2;
    myUIsoDeg[2] = myUIsoDeg[3] = false;

    gp_Pnt Corner1 = myAdSur->Value(su1, sv1);
    gp_Pnt Corner2 = myAdSur->Value(su1, sv2);
    gp_Pnt Corner3 = myAdSur->Value(su2, sv1);
    gp_Pnt Corner4 = myAdSur->Value(su2, sv2);

    myPreci[0] = std::max(Corner1.Distance(Corner2),
                          std::max(myP3d[0].Distance(Corner1), myP3d[0].Distance(Corner2)));
    myPreci[1] = std::max(Corner3.Distance(Corner4),
                          std::max(myP3d[1].Distance(Corner3), myP3d[1].Distance(Corner4)));
    myPreci[2] = std::max(Corner1.Distance(Corner3),
                          std::max(myP3d[2].Distance(Corner1), myP3d[2].Distance(Corner3)));
    myPreci[3] = std::max(Corner2.Distance(Corner4),
                          std::max(myP3d[3].Distance(Corner2), myP3d[3].Distance(Corner4)));

    myNbDeg = 4;
  }
  SortSingularities();
}

//=================================================================================================

bool ShapeAnalysis_Surface::HasSingularities(const double preci)
{
  return NbSingularities(preci) > 0;
}

//=================================================================================================

int ShapeAnalysis_Surface::NbSingularities(const double preci)
{
  if (myNbDeg < 0)
    ComputeSingularities();
  int Nb = 0;
  for (int i = 1; i <= myNbDeg; i++)
    if (myPreci[i - 1] <= preci)
      Nb++;
  return Nb;
}

//=================================================================================================

bool ShapeAnalysis_Surface::Singularity(const int num,
                                        double&   preci,
                                        gp_Pnt&   P3d,
                                        gp_Pnt2d& firstP2d,
                                        gp_Pnt2d& lastP2d,
                                        double&   firstpar,
                                        double&   lastpar,
                                        bool&     uisodeg)
{
  //  ATTENTION, les champs sont des tableaux C, n0s partent de 0. num part de 1
  if (myNbDeg < 0)
    ComputeSingularities();
  if (num < 1 || num > myNbDeg)
    return false;
  P3d      = myP3d[num - 1];
  preci    = myPreci[num - 1];
  firstP2d = myFirstP2d[num - 1];
  lastP2d  = myLastP2d[num - 1];
  firstpar = myFirstPar[num - 1];
  lastpar  = myLastPar[num - 1];
  uisodeg  = myUIsoDeg[num - 1];
  return true;
}

//=================================================================================================

bool ShapeAnalysis_Surface::IsDegenerated(const gp_Pnt& P3d, const double preci)
{
  if (myNbDeg < 0)
    ComputeSingularities();
  for (int i = 0; i < myNbDeg && myPreci[i] <= preci; i++)
  {
    myGap = myP3d[i].Distance(P3d);
    // rln S4135
    if (myGap <= preci)
      return true;
  }
  return false;
}

//=================================================================================================

bool ShapeAnalysis_Surface::DegeneratedValues(const gp_Pnt& P3d,
                                              const double  preci,
                                              gp_Pnt2d&     firstP2d,
                                              gp_Pnt2d&     lastP2d,
                                              double&       firstPar,
                                              double&       lastPar,
                                              const bool /*forward*/)
{
  if (myNbDeg < 0)
    ComputeSingularities();
  // #77 rln S4135: returning singularity which has minimum gap between singular point and input 3D
  // point
  int    indMin = -1;
  double gapMin = RealLast();
  for (int i = 0; i < myNbDeg && myPreci[i] <= preci; i++)
  {
    myGap = myP3d[i].Distance(P3d);
    // rln S4135
    if (myGap <= preci)
      if (gapMin > myGap)
      {
        gapMin = myGap;
        indMin = i;
      }
  }
  if (indMin >= 0)
  {
    myGap    = gapMin;
    firstP2d = myFirstP2d[indMin];
    lastP2d  = myLastP2d[indMin];
    firstPar = myFirstPar[indMin];
    lastPar  = myLastPar[indMin];
    return true;
  }
  return false;
}

//=================================================================================================

bool ShapeAnalysis_Surface::ProjectDegenerated(const gp_Pnt&   P3d,
                                               const double    preci,
                                               const gp_Pnt2d& neighbour,
                                               gp_Pnt2d&       result)
{
  if (myNbDeg < 0)
    ComputeSingularities();
  // added by rln on 03/12/97
  //: c1 abv 23 Feb 98: preci (3d) -> Resolution (2d)
  // #77 rln S4135
  int    indMin = -1;
  double gapMin = RealLast();
  for (int i = 0; i < myNbDeg && myPreci[i] <= preci; i++)
  {
    double gap2 = myP3d[i].SquareDistance(P3d);
    if (gap2 > preci * preci)
      gap2 = std::min(gap2, myP3d[i].SquareDistance(Value(result)));
    // rln S4135
    if (gap2 <= preci * preci && gapMin > gap2)
    {
      gapMin = gap2;
      indMin = i;
    }
  }
  if (indMin < 0)
    return false;
  myGap = std::sqrt(gapMin);
  if (!myUIsoDeg[indMin])
    result.SetX(neighbour.X());
  else
    result.SetY(neighbour.Y());
  return true;
}

// pdn %12 11.02.99 PRO9234 entity 15402
//=================================================================================================

bool ShapeAnalysis_Surface::ProjectDegenerated(const int                           nbrPnt,
                                               const NCollection_Sequence<gp_Pnt>& points,
                                               NCollection_Sequence<gp_Pnt2d>&     pnt2d,
                                               const double                        preci,
                                               const bool                          direct)
{
  if (myNbDeg < 0)
    ComputeSingularities();

  int step = (direct ? 1 : -1);
  // #77 rln S4135
  int    indMin = -1;
  double gapMin = RealLast(), prec2 = preci * preci;
  int    j = (direct ? 1 : nbrPnt);
  for (int i = 0; i < myNbDeg && myPreci[i] <= preci; i++)
  {
    double gap2 = myP3d[i].SquareDistance(points(j));
    if (gap2 > prec2)
      gap2 = std::min(gap2, myP3d[i].SquareDistance(Value(pnt2d(j))));
    if (gap2 <= prec2 && gapMin > gap2)
    {
      gapMin = gap2;
      indMin = i;
    }
  }
  if (indMin < 0)
    return false;

  myGap = std::sqrt(gapMin);
  gp_Pnt2d pk;

  int k; // svv Jan11 2000 : porting on DEC
  for (k = j + step; k <= nbrPnt && k >= 1; k += step)
  {
    pk        = pnt2d(k);
    gp_Pnt P1 = points(k);
    if (myP3d[indMin].SquareDistance(P1) > prec2 && myP3d[indMin].SquareDistance(Value(pk)) > prec2)
      break;
  }

  //: p8 abv 11 Mar 99: PRO7226 #489490: if whole pcurve is degenerate, distribute evenly
  if (k < 1 || k > nbrPnt)
  {
    double x1 = (myUIsoDeg[indMin] ? pnt2d(1).Y() : pnt2d(1).X());
    double x2 = (myUIsoDeg[indMin] ? pnt2d(nbrPnt).Y() : pnt2d(nbrPnt).X());
    for (j = 1; j <= nbrPnt; j++)
    {
      // szv#4:S4163:12Mar99 warning - possible div by zero
      double x = (x1 * (nbrPnt - j) + x2 * (j - 1)) / (nbrPnt - 1);
      if (!myUIsoDeg[indMin])
        pnt2d(j).SetX(x);
      else
        pnt2d(j).SetY(x);
    }
    return true;
  }

  for (j = k - step; j <= nbrPnt && j >= 1; j -= step)
  {
    if (!myUIsoDeg[indMin])
      pnt2d(j).SetX(pk.X());
    else
      pnt2d(j).SetY(pk.Y());
  }
  return true;
}

//=================================================================================================

bool ShapeAnalysis_Surface::IsDegenerated(const gp_Pnt2d& p2d1,
                                          const gp_Pnt2d& p2d2,
                                          const double    tol,
                                          const double    ratio)
{
  gp_Pnt p1    = Value(p2d1);
  gp_Pnt p2    = Value(p2d2);
  gp_Pnt pm    = Value(0.5 * (p2d1.XY() + p2d2.XY()));
  double max3d = std::max(p1.Distance(p2), std::max(pm.Distance(p1), pm.Distance(p2)));
  if (max3d > tol)
    return false;

  GeomAdaptor_Surface& SA = *Adaptor3d();
  double               RU = SA.UResolution(1.);
  double               RV = SA.VResolution(1.);

  if (RU < Precision::PConfusion() || RV < Precision::PConfusion())
    return false;
  double du = std::abs(p2d1.X() - p2d2.X()) / RU;
  double dv = std::abs(p2d1.Y() - p2d2.Y()) / RV;
  max3d *= ratio;
  return du * du + dv * dv > max3d * max3d;
}

//=================================================================================================

static occ::handle<Geom_Curve> ComputeIso(const occ::handle<Geom_Surface>& surf,
                                          const bool                       utype,
                                          const double                     par)
{
  occ::handle<Geom_Curve> iso;
  try
  {
    OCC_CATCH_SIGNALS
    if (utype)
      iso = surf->UIso(par);
    else
      iso = surf->VIso(par);
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    //: s5
    std::cout << "\nWarning: ShapeAnalysis_Surface, ComputeIso(): Exception in UVIso(): ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
    iso.Nullify();
  }
  return iso;
}

//=================================================================================================

void ShapeAnalysis_Surface::ComputeBoundIsos()
{
  if (myIsos)
    return;
  myIsos  = true;
  myIsoUF = ComputeIso(mySurf, true, myUF);
  myIsoUL = ComputeIso(mySurf, true, myUL);
  myIsoVF = ComputeIso(mySurf, false, myVF);
  myIsoVL = ComputeIso(mySurf, false, myVL);
}

//=================================================================================================

occ::handle<Geom_Curve> ShapeAnalysis_Surface::UIso(const double U)
{
  if (U == myUF)
  {
    ComputeBoundIsos();
    return myIsoUF;
  }
  if (U == myUL)
  {
    ComputeBoundIsos();
    return myIsoUL;
  }
  return ComputeIso(mySurf, true, U);
}

//=================================================================================================

occ::handle<Geom_Curve> ShapeAnalysis_Surface::VIso(const double V)
{
  if (V == myVF)
  {
    ComputeBoundIsos();
    return myIsoVF;
  }
  if (V == myVL)
  {
    ComputeBoundIsos();
    return myIsoVL;
  }
  return ComputeIso(mySurf, false, V);
}

//=================================================================================================

bool ShapeAnalysis_Surface::IsUClosed(const double preci)
{
  double prec      = std::max(preci, Precision::Confusion());
  double anUmidVal = -1.;
  if (myUCloseVal < 0)
  {
    //    Faut calculer : calculs minimaux
    double uf, ul, vf, vl;
    Bounds(uf, ul, vf, vl); // modified by rln on 12/11/97 mySurf-> is deleted
    // mySurf->Bounds (uf,ul,vf,vl);
    RestrictBounds(uf, ul, vf, vl);
    myUDelt = std::abs(ul - uf) / 20; // modified by rln 11/11/97 instead of 10
                                      // because of the example when 10 was not enough
    if (mySurf->IsUClosed())
    {
      myUCloseVal = 0.;
      myUDelt     = 0.;
      myGap       = 0.;
      return true;
    }

    // Calculs adaptes
    // #67 rln S4135
    GeomAdaptor_Surface& SurfAdapt = *Adaptor3d();
    GeomAbs_SurfaceType  surftype  = SurfAdapt.GetType();
    if (mySurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      surftype = GeomAbs_OtherSurface;
    }

    switch (surftype)
    {
      case GeomAbs_Plane: {
        myUCloseVal = RealLast();
        break;
      }
      case GeomAbs_SurfaceOfExtrusion: { //: c8 abv 03 Mar 98: UKI60094 #753: process
                                         //: Geom_SurfaceOfLinearExtrusion
        occ::handle<Geom_SurfaceOfLinearExtrusion> extr =
          occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurf);
        occ::handle<Geom_Curve> crv = extr->BasisCurve();
        double                  f   = crv->FirstParameter();
        double                  l   = crv->LastParameter();
        //: r3 abv (smh) 30 Mar 99: protect against unexpected signals
        if (!Precision::IsInfinite(f) && !Precision::IsInfinite(l))
        {
          gp_Pnt p1   = crv->Value(f);
          gp_Pnt p2   = crv->Value(l);
          myUCloseVal = p1.SquareDistance(p2);
          gp_Pnt pm   = crv->Value((f + l) / 2.);
          anUmidVal   = p1.SquareDistance(pm);
        }
        else
        {
          myUCloseVal = RealLast();
        }
        break;
      }
      case GeomAbs_BSplineSurface: {
        occ::handle<Geom_BSplineSurface> bs      = occ::down_cast<Geom_BSplineSurface>(mySurf);
        int                              nbup    = bs->NbUPoles();
        double                           distmin = RealLast();
        if (bs->IsUPeriodic())
        {
          myUCloseVal = 0;
          myUDelt     = 0;
        }
        else if (nbup < 3)
        { // modified by rln on 12/11/97
          myUCloseVal = RealLast();
        }
        else if (bs->IsURational() ||
                 // #6 rln 20/02/98 ProSTEP ug_exhaust-A.stp entity #18360 (Uclosed BSpline,
                 // but multiplicity of boundary knots != degree + 1)
                 bs->UMultiplicity(1) != bs->UDegree() + 1 || // #6 //:h4: #6 moved
                 bs->UMultiplicity(bs->NbUKnots()) != bs->UDegree() + 1)
        { // #6 //:h4
          int    nbvk = bs->NbVKnots();
          double v    = bs->VKnot(1);
          gp_Pnt p1   = SurfAdapt.Value(uf, v);
          gp_Pnt p2   = SurfAdapt.Value(ul, v);
          myUCloseVal = p1.SquareDistance(p2);
          gp_Pnt pm   = SurfAdapt.Value((uf + ul) / 2., v);
          anUmidVal   = p1.SquareDistance(pm);
          distmin     = myUCloseVal;
          for (int i = 2; i <= nbvk; i++)
          {
            v            = 0.5 * (bs->VKnot(i - 1) + bs->VKnot(i));
            p1           = bs->Value(uf, v);
            p2           = bs->Value(ul, v);
            double aDist = p1.SquareDistance(p2);
            if (aDist > myUCloseVal)
            {
              myUCloseVal = aDist;
              pm          = bs->Value((uf + ul) / 2., v);
              anUmidVal   = p1.SquareDistance(pm);
            }
            else
            {
              distmin = std::min(distmin, aDist);
            }
          }
          distmin = std::sqrt(distmin);
          myUDelt = std::min(myUDelt, 0.5 * SurfAdapt.UResolution(distmin)); // #4 smh
        }
        else
        {
          int nbvp    = bs->NbVPoles();
          myUCloseVal = bs->Pole(1, 1).SquareDistance(bs->Pole(nbup, 1));
          anUmidVal   = bs->Pole(1, 1).SquareDistance(bs->Pole(nbup / 2 + 1, 1));
          distmin     = myUCloseVal;
          for (int i = 2; i <= nbvp; i++)
          {
            double aDist = bs->Pole(1, i).SquareDistance(bs->Pole(nbup, i));
            if (aDist > myUCloseVal)
            {
              myUCloseVal = aDist;
              anUmidVal   = bs->Pole(1, i).SquareDistance(bs->Pole(nbup / 2 + 1, i));
            }
            else
            {
              distmin = std::min(distmin, aDist);
            }
          }
          distmin = std::sqrt(distmin);
          myUDelt = std::min(myUDelt, 0.5 * SurfAdapt.UResolution(distmin)); // #4 smh
        }
        break;
      }
      case GeomAbs_BezierSurface: {
        occ::handle<Geom_BezierSurface> bz      = occ::down_cast<Geom_BezierSurface>(mySurf);
        int                             nbup    = bz->NbUPoles();
        double                          distmin = RealLast();
        if (nbup < 3)
        {
          myUCloseVal = RealLast();
        }
        else
        {
          int nbvp    = bz->NbVPoles();
          myUCloseVal = bz->Pole(1, 1).SquareDistance(bz->Pole(nbup, 1));
          anUmidVal   = bz->Pole(1, 1).SquareDistance(bz->Pole(nbup / 2 + 1, 1));
          distmin     = myUCloseVal;
          for (int i = 1; i <= nbvp; i++)
          {
            double aDist = bz->Pole(1, i).SquareDistance(bz->Pole(nbup, i));
            if (aDist > myUCloseVal)
            {
              myUCloseVal = aDist;
              anUmidVal   = bz->Pole(1, i).SquareDistance(bz->Pole(nbup / 2 + 1, i));
            }
            else
            {
              distmin = std::min(distmin, aDist);
            }
          }
          distmin = std::sqrt(distmin);
          myUDelt = std::min(myUDelt, 0.5 * SurfAdapt.UResolution(distmin)); // #4 smh
        }
        break;
      }
      default: { // Geom_RectangularTrimmedSurface and Geom_OffsetSurface
        double distmin  = RealLast();
        int    nbpoints = 101; // can be revised
        gp_Pnt p1       = SurfAdapt.Value(uf, vf);
        gp_Pnt p2       = SurfAdapt.Value(ul, vf);
        myUCloseVal     = p1.SquareDistance(p2);
        gp_Pnt pm       = SurfAdapt.Value((uf + ul) / 2, vf);
        anUmidVal       = p1.SquareDistance(pm);
        distmin         = myUCloseVal;
        for (int i = 1; i < nbpoints; i++)
        {
          double vparam = vf + (vl - vf) * i / (nbpoints - 1);
          p1            = SurfAdapt.Value(uf, vparam);
          p2            = SurfAdapt.Value(ul, vparam);
          double aDist  = p1.SquareDistance(p2);
          if (aDist > myUCloseVal)
          {
            myUCloseVal = aDist;
            pm          = SurfAdapt.Value((uf + ul) / 2, vparam);
            anUmidVal   = p1.SquareDistance(pm);
          }
          else
          {
            distmin = std::min(distmin, aDist);
          }
        }
        distmin = std::sqrt(distmin);
        myUDelt = std::min(myUDelt, 0.5 * SurfAdapt.UResolution(distmin)); // #4 smh
        break;
      }
    } // switch
    myGap       = sqrt(myUCloseVal);
    myUCloseVal = myGap;
  }

  if (anUmidVal > 0. && myUCloseVal > sqrt(anUmidVal))
  {
    myUCloseVal = RealLast();
    return false;
  }

  return (myUCloseVal <= prec);
}

//=================================================================================================

bool ShapeAnalysis_Surface::IsVClosed(const double preci)
{
  double prec     = std::max(preci, Precision::Confusion());
  double aVmidVal = -1.;
  if (myVCloseVal < 0)
  {
    //    Faut calculer : calculs minimaux
    double uf, ul, vf, vl;
    Bounds(uf, ul, vf, vl); // modified by rln on 12/11/97 mySurf-> is deleted
                            //     mySurf->Bounds (uf,ul,vf,vl);
    RestrictBounds(uf, ul, vf, vl);
    myVDelt = std::abs(vl - vf) / 20; // 2; rln S4135
                                      // because of the example when 10 was not enough
    if (mySurf->IsVClosed())
    {
      myVCloseVal = 0.;
      myVDelt     = 0.;
      myGap       = 0.;
      return true;
    }

    //    Calculs adaptes
    // #67 rln S4135
    GeomAdaptor_Surface& SurfAdapt = *Adaptor3d();
    GeomAbs_SurfaceType  surftype  = SurfAdapt.GetType();
    if (mySurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      surftype = GeomAbs_OtherSurface;
    }

    switch (surftype)
    {
      case GeomAbs_Plane:
      case GeomAbs_Cone:
      case GeomAbs_Cylinder:
      case GeomAbs_Sphere:
      case GeomAbs_SurfaceOfExtrusion: {
        myVCloseVal = RealLast();
        break;
      }
      case GeomAbs_SurfaceOfRevolution: {
        occ::handle<Geom_SurfaceOfRevolution> revol =
          occ::down_cast<Geom_SurfaceOfRevolution>(mySurf);
        occ::handle<Geom_Curve> crv = revol->BasisCurve();
        gp_Pnt                  p1  = crv->Value(crv->FirstParameter());
        gp_Pnt                  p2  = crv->Value(crv->LastParameter());
        myVCloseVal                 = p1.SquareDistance(p2);
        break;
      }
      case GeomAbs_BSplineSurface: {
        occ::handle<Geom_BSplineSurface> bs      = occ::down_cast<Geom_BSplineSurface>(mySurf);
        int                              nbvp    = bs->NbVPoles();
        double                           distmin = RealLast();
        if (bs->IsVPeriodic())
        {
          myVCloseVal = 0;
          myVDelt     = 0;
        }
        else if (nbvp < 3)
        { // modified by rln on 12/11/97
          myVCloseVal = RealLast();
        }
        else if (bs->IsVRational() || bs->VMultiplicity(1) != bs->VDegree() + 1 || // #6 //:h4
                 bs->VMultiplicity(bs->NbVKnots()) != bs->VDegree() + 1)
        { // #6 //:h4
          int    nbuk = bs->NbUKnots();
          double u    = bs->UKnot(1);
          gp_Pnt p1   = SurfAdapt.Value(u, vf);
          gp_Pnt p2   = SurfAdapt.Value(u, vl);
          myVCloseVal = p1.SquareDistance(p2);
          gp_Pnt pm   = SurfAdapt.Value(u, (vf + vl) / 2.);
          aVmidVal    = p1.SquareDistance(pm);
          distmin     = myVCloseVal;
          for (int i = 2; i <= nbuk; i++)
          {
            u            = 0.5 * (bs->UKnot(i - 1) + bs->UKnot(i));
            p1           = SurfAdapt.Value(u, vf);
            p2           = SurfAdapt.Value(u, vl);
            double aDist = p1.SquareDistance(p2);
            if (aDist > myVCloseVal)
            {
              myVCloseVal = aDist;
              pm          = SurfAdapt.Value(u, (vf + vl) / 2);
              aVmidVal    = p1.SquareDistance(pm);
            }
            else
            {
              distmin = std::min(distmin, aDist);
            }
          }
          distmin = std::sqrt(distmin);
          myVDelt = std::min(myVDelt, 0.5 * SurfAdapt.VResolution(distmin)); // #4 smh
        }
        else
        {
          int nbup    = bs->NbUPoles();
          myVCloseVal = bs->Pole(1, 1).SquareDistance(bs->Pole(1, nbvp));
          aVmidVal    = bs->Pole(1, 1).SquareDistance(bs->Pole(1, nbvp / 2 + 1));
          distmin     = myVCloseVal;
          for (int i = 2; i <= nbup; i++)
          {
            double aDist = bs->Pole(i, 1).SquareDistance(bs->Pole(i, nbvp));
            if (aDist > myVCloseVal)
            {
              myVCloseVal = aDist;
              aVmidVal    = bs->Pole(i, 1).SquareDistance(bs->Pole(i, nbvp / 2 + 1));
            }
            else
            {
              distmin = std::min(distmin, aDist);
            }
          }
          distmin = std::sqrt(distmin);
          myVDelt = std::min(myVDelt, 0.5 * SurfAdapt.VResolution(distmin)); // #4 smh
        }
        break;
      }
      case GeomAbs_BezierSurface: {
        occ::handle<Geom_BezierSurface> bz      = occ::down_cast<Geom_BezierSurface>(mySurf);
        int                             nbvp    = bz->NbVPoles();
        double                          distmin = RealLast();
        if (nbvp < 3)
        {
          myVCloseVal = RealLast();
        }
        else
        {
          int nbup    = bz->NbUPoles();
          myVCloseVal = bz->Pole(1, 1).SquareDistance(bz->Pole(1, nbvp));
          aVmidVal    = bz->Pole(1, 1).SquareDistance(bz->Pole(1, nbvp / 2 + 1));
          distmin     = myVCloseVal;
          for (int i = 2; i <= nbup; i++)
          {
            double aDist = bz->Pole(i, 1).SquareDistance(bz->Pole(i, nbvp));
            if (aDist > myVCloseVal)
            {
              myVCloseVal = aDist;
              aVmidVal    = bz->Pole(i, 1).SquareDistance(bz->Pole(i, nbvp / 2 + 1));
            }
            else
            {
              distmin = std::min(distmin, aDist);
            }
          }
          distmin = std::sqrt(distmin);
          myVDelt = std::min(myVDelt, 0.5 * SurfAdapt.VResolution(distmin)); // #4 smh
        }
        break;
      }
      default: { // Geom_RectangularTrimmedSurface and Geom_OffsetSurface
        double distmin  = RealLast();
        int    nbpoints = 101; // can be revised
        gp_Pnt p1       = SurfAdapt.Value(uf, vf);
        gp_Pnt p2       = SurfAdapt.Value(uf, vl);
        gp_Pnt pm       = SurfAdapt.Value(uf, (vf + vl) / 2);
        myVCloseVal     = p1.SquareDistance(p2);
        aVmidVal        = p1.SquareDistance(pm);
        distmin         = myVCloseVal;
        for (int i = 1; i < nbpoints; i++)
        {
          double uparam = uf + (ul - uf) * i / (nbpoints - 1);
          p1            = SurfAdapt.Value(uparam, vf);
          p2            = SurfAdapt.Value(uparam, vl);
          double aDist  = p1.SquareDistance(p2);
          if (aDist > myVCloseVal)
          {
            myVCloseVal = aDist;
            pm          = SurfAdapt.Value(uparam, (vf + vl) / 2);
            aVmidVal    = p1.SquareDistance(pm);
          }
          else
          {
            distmin = std::min(distmin, aDist);
          }
        }
        distmin = std::sqrt(distmin);
        myVDelt = std::min(myVDelt, 0.5 * SurfAdapt.VResolution(distmin)); // #4 smh
        break;
      }
    } // switch
    myGap       = std::sqrt(myVCloseVal);
    myVCloseVal = myGap;
  }

  if (aVmidVal > 0. && myVCloseVal > sqrt(aVmidVal))
  {
    myVCloseVal = RealLast();
    return false;
  }

  return (myVCloseVal <= prec);
}

//=======================================================================
// function : SurfaceNewton
// purpose  : Newton algo (S4030)
//=======================================================================
int ShapeAnalysis_Surface::SurfaceNewton(const gp_Pnt2d& p2dPrev,
                                         const gp_Pnt&   P3D,
                                         const double    preci,
                                         gp_Pnt2d&       sol)
{
  GeomAdaptor_Surface& SurfAdapt = *Adaptor3d();
  double               uf, ul, vf, vl;
  Bounds(uf, ul, vf, vl);
  double du = SurfAdapt.UResolution(preci);
  double dv = SurfAdapt.VResolution(preci);
  double UF = uf - du, UL = ul + du;
  double VF = vf - dv, VL = vl + dv;

  // int fail = 0;
  constexpr double Tol  = Precision::Confusion();
  constexpr double Tol2 = Tol * Tol; //, rs2p=1e10;
  double           U = p2dPrev.X(), V = p2dPrev.Y();
  gp_Vec           rsfirst = P3D.XYZ() - Value(U, V).XYZ(); // pdn
  for (int i = 0; i < 25; i++)
  {
    gp_Vec ru, rv, ruu, rvv, ruv;
    gp_Pnt pnt;
    SurfAdapt.D2(U, V, pnt, ru, rv, ruu, rvv, ruv);

    // normal
    double ru2 = ru * ru, rv2 = rv * rv;
    gp_Vec n    = ru ^ rv;
    double nrm2 = n.SquareMagnitude();
    if (nrm2 < 1e-10 || Precision::IsPositiveInfinite(nrm2))
      break; // n == 0, use standard

    // descriminant
    gp_Vec rs   = P3D.XYZ() - Value(U, V).XYZ();
    double rSuu = (rs * ruu);
    double rSvv = (rs * rvv);
    double rSuv = (rs * ruv);
    double D = -nrm2 + rv2 * rSuu + ru2 * rSvv - 2 * rSuv * (ru * rv) + rSuv * rSuv - rSuu * rSvv;
    if (fabs(D) < 1e-10)
      break; // bad case; use standard

    // compute step
    double fract = 1. / D;
    du           = (rs * ((n ^ rv) + ru * rSvv - rv * rSuv)) * fract;
    dv           = (rs * ((ru ^ n) + rv * rSuu - ru * rSuv)) * fract;
    U += du;
    V += dv;
    if (U < UF || U > UL || V < VF || V > VL)
      break;
    // check that iterations do not diverge
    // pdn    double rs2 = rs.SquareMagnitude();
    //    if ( rs2 > 4.*rs2p ) break;
    //    rs2p = rs2;

    // test the step by uv and deviation from the solution
    double aResolution = std::max(1e-12, (U + V) * 10e-16);
    if (fabs(du) + fabs(dv) > aResolution)
      continue; // Precision::PConfusion()  continue;

    // if ( U < UF || U > UL || V < VF || V > VL ) break;

    // pdn PRO10109 4517: protect against wrong result
    double rs2 = rs.SquareMagnitude();
    if (rs2 > rsfirst.SquareMagnitude())
      break;

    double rsn = rs * n;
    if (rs2 - rsn * rsn / nrm2 > Tol2)
      break;

    //  if ( rs2 > 100 * preci * preci ) { fail = 6; break; }

    // OK, return the result
    //	std::cout << "Newton: solution found in " << i+1 << " iterations" << std::endl;
    sol.SetCoord(U, V);

    return (nrm2 < 0.01 * ru2 * rv2 ? 2 : 1); //: q6
  }
  //      std::cout << "Newton: failed after " << i+1 << " iterations (fail " << fail << " )" <<
  //      std::endl;
  return false;
}

//=======================================================================
// function : NextValueOfUV
// purpose  : optimizing projection by Newton algo (S4030)
//=======================================================================

gp_Pnt2d ShapeAnalysis_Surface::NextValueOfUV(const gp_Pnt2d& p2dPrev,
                                              const gp_Pnt&   P3D,
                                              const double    preci,
                                              const double    maxpreci)
{
  GeomAdaptor_Surface& SurfAdapt = *Adaptor3d();
  GeomAbs_SurfaceType  surftype  = SurfAdapt.GetType();

  switch (surftype)
  {
    case GeomAbs_BezierSurface:
    case GeomAbs_BSplineSurface:
    case GeomAbs_SurfaceOfExtrusion:
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_OffsetSurface:

    {
      if (surftype == GeomAbs_BSplineSurface)
      {
        occ::handle<Geom_BSplineSurface> aBSpline = SurfAdapt.BSpline();

        // Check near to knot position ~ near to C0 points on U isoline.
        if (SurfAdapt.UContinuity() == GeomAbs_C0)
        {
          int aMinIndex = aBSpline->FirstUKnotIndex();
          int aMaxIndex = aBSpline->LastUKnotIndex();
          for (int anIdx = aMinIndex; anIdx <= aMaxIndex; ++anIdx)
          {
            double aKnot = aBSpline->UKnot(anIdx);
            if (std::abs(aKnot - p2dPrev.X()) < Precision::Confusion())
              return ValueOfUV(P3D, preci);
          }
        }

        // Check near to knot position ~ near to C0 points on U isoline.
        if (SurfAdapt.VContinuity() == GeomAbs_C0)
        {
          int aMinIndex = aBSpline->FirstVKnotIndex();
          int aMaxIndex = aBSpline->LastVKnotIndex();
          for (int anIdx = aMinIndex; anIdx <= aMaxIndex; ++anIdx)
          {
            double aKnot = aBSpline->VKnot(anIdx);
            if (std::abs(aKnot - p2dPrev.Y()) < Precision::Confusion())
              return ValueOfUV(P3D, preci);
          }
        }
      }

      gp_Pnt2d sol;
      int      res = SurfaceNewton(p2dPrev, P3D, preci, sol);
      if (res != 0)
      {
        double gap = P3D.Distance(Value(sol));
        if (res == 2 || //: q6 abv 19 Mar 99: protect against strange attractors
            (maxpreci > 0. && gap - maxpreci > Precision::Confusion()))
        { //: q1: check with maxpreci
          double U = sol.X(), V = sol.Y();
          myGap = UVFromIso(P3D, preci, U, V);
          //	  gp_Pnt2d p = ValueOfUV ( P3D, preci );
          if (gap >= myGap)
            return gp_Pnt2d(U, V);
        }
        myGap = gap;
        return sol;
      }
    }
    break;
    default:
      break;
  }
  return ValueOfUV(P3D, preci);
}

//=================================================================================================

gp_Pnt2d ShapeAnalysis_Surface::ValueOfUV(const gp_Pnt& P3D, const double preci)
{
  GeomAdaptor_Surface& SurfAdapt = *Adaptor3d();
  double               S = 0., T = 0.;
  myGap = -1.; // devra etre calcule

  double uf, ul, vf, vl;
  Bounds(uf, ul, vf, vl);

  try
  {
    OCC_CATCH_SIGNALS

    // Handle special case for extrusion surfaces with infinite bounds
    GeomAbs_SurfaceType surftype = SurfAdapt.GetType();
    if ((surftype == GeomAbs_SurfaceOfExtrusion) && Precision::IsInfinite(uf)
        && Precision::IsInfinite(ul))
    {
      // conic case - try Newton first
      gp_Pnt2d prev(0., (vf + vl) / 2);
      gp_Pnt2d solution;
      if (SurfaceNewton(prev, P3D, preci, solution) != 0)
      {
        myGap = P3D.Distance(SurfAdapt.Value(solution.X(), solution.Y()));
        return solution;
      }
      uf = -500;
      ul = 500;
    }

    RestrictBounds(uf, ul, vf, vl);

    // Initialize ExtremaPS_Surface if not yet done
    if (!myExtPS.IsInitialized())
    {
      // Ensure IsU-VClosed has been called
      if (myUCloseVal < 0)
        IsUClosed();
      if (myVCloseVal < 0)
        IsVClosed();
      double du = 0., dv = 0.;
      // Extension of the surface range is limited to non-offset surfaces as the latter
      // can throw exception (e.g. Geom_UndefinedValue) when computing value - see id23943
      if (!mySurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
      {
        du = std::min(myUDelt, SurfAdapt.UResolution(preci));
        dv = std::min(myVDelt, SurfAdapt.VResolution(preci));
      }
      ExtremaPS::Domain2D aDomain(uf - du, ul + du, vf - dv, vl + dv);
      myExtPS = ExtremaPS_Surface(SurfAdapt, aDomain);
    }

    const ExtremaPS::Result& aResult = myExtPS.Perform(P3D, preci, ExtremaPS::SearchMode::Min);
    int                      nPSurf  = aResult.IsDone() ? aResult.NbExt() : 0;

    if (nPSurf > 0)
    {
      int aMinIdx = aResult.MinIndex();
      S           = aResult[aMinIdx].U;
      T           = aResult[aMinIdx].V;

      // Verify distance (workaround for OCC486)
      gp_Pnt aCheckPnt = SurfAdapt.Value(S, T);
      double disSurf   = P3D.Distance(aCheckPnt);

      // For non-analytical surfaces, try to improve the result if distance exceeds precision
      if (disSurf > preci && surftype != GeomAbs_Plane && surftype != GeomAbs_Cylinder
          && surftype != GeomAbs_Cone && surftype != GeomAbs_Sphere && surftype != GeomAbs_Torus)
      {
        double UU = S, VV = T;
        double DistMinOnIso = RealLast();
        bool   possLocal    = false;

        // Try Newton refinement
        gp_Pnt2d pp(UU, VV);
        if (SurfaceNewton(pp, P3D, preci, pp) != 0)
        {
          double dist = P3D.Distance(Value(pp));
          if (dist < disSurf)
          {
            disSurf = dist;
            S = UU = pp.X();
            T = VV = pp.Y();
          }
        }

        // Check if point is locally on surface (normal projection)
        if (disSurf < 10 * preci && mySurf->Continuity() != GeomAbs_C0)
        {
          constexpr double Tol = Precision::Confusion();
          gp_Vec           D1U, D1V;
          gp_Pnt           pnt;
          SurfAdapt.D1(UU, VV, pnt, D1U, D1V);
          gp_Vec b = D1U.Crossed(D1V);
          gp_Vec a(pnt, P3D);
          double ab   = a.Dot(b);
          double nrm2 = b.SquareMagnitude();
          if (nrm2 > 1e-10)
          {
            double dist = a.SquareMagnitude() - (ab * ab) / nrm2;
            possLocal   = (dist < Tol * Tol);
          }
        }

        // Try iso-line projection as fallback
        if (!possLocal)
        {
          DistMinOnIso = UVFromIso(P3D, preci, UU, VV);
        }

        if (disSurf > DistMinOnIso)
        {
          S     = UU;
          T     = VV;
          myGap = DistMinOnIso;
        }
        else
        {
          myGap = disSurf;
        }
      }
      else
      {
        myGap = disSurf;
      }
    }
    else
    {
      // Fall back to iso-line projection
      S     = (uf + ul) / 2;
      T     = (vf + vl) / 2;
      myGap = UVFromIso(P3D, preci, S, T);
    }
  }
  catch (Standard_Failure const&)
  {
    S = (Precision::IsInfinite(uf)) ? 0 : (uf + ul) / 2.;
    T = (Precision::IsInfinite(vf)) ? 0 : (vf + vl) / 2.;
  }

  if (myGap <= 0)
    myGap = P3D.Distance(SurfAdapt.Value(S, T));

  return gp_Pnt2d(S, T);
}

//=================================================================================================

double ShapeAnalysis_Surface::UVFromIso(const gp_Pnt& P3d, const double preci, double& U, double& V)
{
  //  Projection qui considere les isos ... comme suit :
  //  Les 4 bords, plus les isos en U et en V
  //  En effet, souvent, un des deux est bon ...
  double theMin = RealLast();

  gp_Pnt pntres;
  double Cf, Cl, UU, VV;

  //  Initialisation des recherches : point deja trouve (?)
  UU            = U;
  VV            = V;
  gp_Pnt depart = myAdSur->Value(U, V);
  theMin        = depart.Distance(P3d);

  if (theMin < preci / 10)
    return theMin; // c etait deja OK
  ComputeBoxes();
  if (myIsoUF.IsNull() || myIsoUL.IsNull() || myIsoVF.IsNull() || myIsoVL.IsNull())
  {
    // no isolines
    // no more precise computation
    return theMin;
  }
  try
  { // RAJOUT
    OCC_CATCH_SIGNALS
    // pdn Create BndBox containing point;
    Bnd_Box aPBox;
    aPBox.Set(P3d);

    // std::cout<<"Adaptor3d()->Surface().GetType() =
    // "<<Adaptor3d()->Surface().GetType()<<std::endl;

    // modified by rln on 04/12/97 in order to use these variables later
    bool                    UV  = true;
    double                  par = 0., other = 0., dist = 0.;
    occ::handle<Geom_Curve> iso;
    Adaptor3d_IsoCurve      anIsoCurve(Adaptor3d());
    for (int num = 0; num < 6; num++)
    {

      UV = (num < 3); // 0-1-2 : iso-U  3-4-5 : iso-V
      if (!(Adaptor3d()->GetType() == GeomAbs_OffsetSurface))
      {
        const Bnd_Box* anIsoBox = nullptr;
        switch (num)
        {
          case 0:
            par      = myUF;
            iso      = myIsoUF;
            anIsoBox = &myBndUF;
            break;
          case 1:
            par      = myUL;
            iso      = myIsoUL;
            anIsoBox = &myBndUL;
            break;
          case 2:
            par = U;
            iso = UIso(U);
            break;
          case 3:
            par      = myVF;
            iso      = myIsoVF;
            anIsoBox = &myBndVF;
            break;
          case 4:
            par      = myVL;
            iso      = myIsoVL;
            anIsoBox = &myBndVL;
            break;
          case 5:
            par = V;
            iso = VIso(V);
            break;
          default:
            break;
        }

        //    On y va la-dessus
        if (!Precision::IsInfinite(par) && !iso.IsNull())
        {
          if (anIsoBox && anIsoBox->Distance(aPBox) > theMin)
            continue;

          Cf = iso->FirstParameter();
          Cl = iso->LastParameter();

          RestrictBounds(Cf, Cl);
          dist = ShapeAnalysis_Curve().Project(iso, P3d, preci, pntres, other, Cf, Cl, false);
          if (dist < theMin)
          {
            theMin = dist;
            //: q6	if (UV) VV = other;  else UU = other;
            //  Selon une isoU, on calcule le meilleur V; et lycee de Versailles
            UU = (UV ? par : other);
            VV = (UV ? other : par); //: q6: uncommented
          }
        }
      }

      else
      {
        Adaptor3d_Curve*  anAdaptor = nullptr;
        GeomAdaptor_Curve aGeomCurve;

        const Bnd_Box* anIsoBox = nullptr;
        switch (num)
        {
          case 0:
            par = myUF;
            aGeomCurve.Load(myIsoUF);
            anAdaptor = &aGeomCurve;
            anIsoBox  = &myBndUF;
            break;
          case 1:
            par = myUL;
            aGeomCurve.Load(myIsoUL);
            anAdaptor = &aGeomCurve;
            anIsoBox  = &myBndUL;
            break;
          case 2:
            par = U;
            anIsoCurve.Load(GeomAbs_IsoU, U);
            anAdaptor = &anIsoCurve;
            break;
          case 3:
            par = myVF;
            aGeomCurve.Load(myIsoVF);
            anAdaptor = &aGeomCurve;
            anIsoBox  = &myBndVF;
            break;
          case 4:
            par = myVL;
            aGeomCurve.Load(myIsoVL);
            anAdaptor = &aGeomCurve;
            anIsoBox  = &myBndVL;
            break;
          case 5:
            par = V;
            anIsoCurve.Load(GeomAbs_IsoV, V);
            anAdaptor = &anIsoCurve;
            break;
          default:
            break;
        }
        if (anIsoBox && anIsoBox->Distance(aPBox) > theMin)
          continue;
        dist = ShapeAnalysis_Curve().Project(*anAdaptor, P3d, preci, pntres, other);
        if (dist < theMin)
        {
          theMin = dist;
          UU     = (UV ? par : other);
          VV     = (UV ? other : par); //: q6: uncommented
        }
      }
    }

    // added by rln on 04/12/97 iterational process
    double PrevU = U, PrevV = V;
    int    MaxIters = 5, Iters = 0;
    if (!(Adaptor3d()->GetType() == GeomAbs_OffsetSurface))
    {
      while (((PrevU != UU) || (PrevV != VV)) && (Iters < MaxIters) && (theMin > preci))
      {
        PrevU = UU;
        PrevV = VV;
        if (UV)
        {
          par = UU;
          iso = UIso(UU);
        }
        else
        {
          par = VV;
          iso = VIso(VV);
        }
        if (!iso.IsNull())
        {
          Cf = iso->FirstParameter();
          Cl = iso->LastParameter();
          RestrictBounds(Cf, Cl);
          dist = ShapeAnalysis_Curve().Project(iso, P3d, preci, pntres, other, Cf, Cl, false);
          if (dist < theMin)
          {
            theMin = dist;
            if (UV)
              VV = other;
            else
              UU = other;
          }
        }
        UV = !UV;
        if (UV)
        {
          par = UU;
          iso = UIso(UU);
        }
        else
        {
          par = VV;
          iso = VIso(VV);
        }
        if (!iso.IsNull())
        {
          Cf = iso->FirstParameter();
          Cl = iso->LastParameter();
          RestrictBounds(Cf, Cl);
          dist = ShapeAnalysis_Curve().Project(iso, P3d, preci, pntres, other, Cf, Cl, false);
          if (dist < theMin)
          {
            theMin = dist;
            if (UV)
              VV = other;
            else
              UU = other;
          }
        }
        UV = !UV;
        Iters++;
      }
    }

    else
    {
      while (((PrevU != UU) || (PrevV != VV)) && (Iters < MaxIters) && (theMin > preci))
      {
        PrevU = UU;
        PrevV = VV;
        if (UV)
        {
          par = UU;
          anIsoCurve.Load(GeomAbs_IsoU, UU);
        }
        else
        {
          par = VV;
          anIsoCurve.Load(GeomAbs_IsoV, VV);
        }
        Cf = anIsoCurve.FirstParameter();
        Cl = anIsoCurve.LastParameter();
        RestrictBounds(Cf, Cl);
        dist = ShapeAnalysis_Curve().Project(anIsoCurve, P3d, preci, pntres, other);
        if (dist < theMin)
        {
          theMin = dist;
          if (UV)
            VV = other;
          else
            UU = other;
        }
        UV = !UV;
        if (UV)
        {
          par = UU;
          anIsoCurve.Load(GeomAbs_IsoU, UU);
        }
        else
        {
          par = VV;
          anIsoCurve.Load(GeomAbs_IsoV, VV);
        }
        Cf = anIsoCurve.FirstParameter();
        Cl = anIsoCurve.LastParameter();
        RestrictBounds(Cf, Cl);
        dist = ShapeAnalysis_Curve().ProjectAct(anIsoCurve, P3d, preci, pntres, other);
        if (dist < theMin)
        {
          theMin = dist;
          if (UV)
            VV = other;
          else
            UU = other;
        }
        UV = !UV;
        Iters++;
      }
    }

    U = UU;
    V = VV;

  } // fin try RAJOUT
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    //: s5
    std::cout << "\nWarning: ShapeAnalysis_Curve::UVFromIso(): Exception: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
    theMin = RealLast(); // theMin de depart
  }
  return theMin;
}

//=================================================================================================

void ShapeAnalysis_Surface::SortSingularities()
{
  for (int i = 0; i < myNbDeg - 1; i++)
  {
    double minPreci = myPreci[i];
    int    minIndex = i;
    for (int j = i + 1; j < myNbDeg; j++)
      if (minPreci > myPreci[j])
      {
        minPreci = myPreci[j];
        minIndex = j;
      }
    if (minIndex != i)
    {
      myPreci[minIndex]    = myPreci[i];
      myPreci[i]           = minPreci;
      gp_Pnt tmpP3d        = myP3d[minIndex];
      myP3d[minIndex]      = myP3d[i];
      myP3d[i]             = tmpP3d;
      gp_Pnt2d tmpP2d      = myFirstP2d[minIndex];
      myFirstP2d[minIndex] = myFirstP2d[i];
      myFirstP2d[i]        = tmpP2d;
      tmpP2d               = myLastP2d[minIndex];
      myLastP2d[minIndex]  = myLastP2d[i];
      myLastP2d[i]         = tmpP2d;
      double tmpPar        = myFirstPar[minIndex];
      myFirstPar[minIndex] = myFirstPar[i];
      myFirstPar[i]        = tmpPar;
      tmpPar               = myLastPar[minIndex];
      myLastPar[minIndex]  = myLastPar[i];
      myLastPar[i]         = tmpPar;
      bool tmpUIsoDeg      = myUIsoDeg[minIndex];
      myUIsoDeg[minIndex]  = myUIsoDeg[i];
      myUIsoDeg[i]         = tmpUIsoDeg;
    }
  }
}

//=================================================================================================

void ShapeAnalysis_Surface::SetDomain(const double U1,
                                      const double U2,
                                      const double V1,
                                      const double V2)
{
  myUF = U1;
  myUL = U2;
  myVF = V1;
  myVL = V2;
}

void ShapeAnalysis_Surface::ComputeBoxes()
{
  if (myIsoBoxes)
    return;
  myIsoBoxes = true;
  ComputeBoundIsos();
  if (!myIsoUF.IsNull())
    BndLib_Add3dCurve::Add(GeomAdaptor_Curve(myIsoUF), Precision::Confusion(), myBndUF);
  if (!myIsoUL.IsNull())
    BndLib_Add3dCurve::Add(GeomAdaptor_Curve(myIsoUL), Precision::Confusion(), myBndUL);
  if (!myIsoVF.IsNull())
    BndLib_Add3dCurve::Add(GeomAdaptor_Curve(myIsoVF), Precision::Confusion(), myBndVF);
  if (!myIsoVL.IsNull())
    BndLib_Add3dCurve::Add(GeomAdaptor_Curve(myIsoVL), Precision::Confusion(), myBndVL);
}

const Bnd_Box& ShapeAnalysis_Surface::GetBoxUF()
{
  ComputeBoxes();
  return myBndUF;
}

const Bnd_Box& ShapeAnalysis_Surface::GetBoxUL()
{
  ComputeBoxes();
  return myBndUL;
}

const Bnd_Box& ShapeAnalysis_Surface::GetBoxVF()
{
  ComputeBoxes();
  return myBndVF;
}

const Bnd_Box& ShapeAnalysis_Surface::GetBoxVL()
{
  ComputeBoxes();
  return myBndVL;
}
