// Created: 1998-06-03
//
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

// abv 06.01.99 fix of misprint
//: p6 abv 26.02.99: make ConvertToPeriodic() return Null if nothing done

#include <ElSLib.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <GeomConvert_CurveToAnaCurve.hxx>
#include <GeomConvert_SurfToAnaSurf.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <Extrema_ExtElC.hxx>
#include <GeomLProp_SLProps.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <math_Vector.hxx>
#include <math_PSO.hxx>
#include <math_Powell.hxx>
#include <GeomConvert_FuncCylinderLSDist.hxx>

//=======================================================================
// function : CheckVTrimForRevSurf
// purpose  :
// static method for checking surface of revolution
// To avoid two-parts cone-like surface
//=======================================================================
void GeomConvert_SurfToAnaSurf::CheckVTrimForRevSurf(
  const occ::handle<Geom_SurfaceOfRevolution>& aRevSurf,
  double&                          V1,
  double&                          V2)
{
  const occ::handle<Geom_Curve>& aBC   = aRevSurf->BasisCurve();
  occ::handle<Geom_Line>         aLine = occ::down_cast<Geom_Line>(aBC);
  if (aLine.IsNull())
    return;
  const gp_Ax1& anAxis = aRevSurf->Axis();

  gp_Lin         anALin(anAxis);
  Extrema_ExtElC anExtLL(aLine->Lin(), anALin, Precision::Angular());
  if (!anExtLL.IsDone() || anExtLL.IsParallel())
    return;
  int aNbExt = anExtLL.NbExt();
  if (aNbExt == 0)
    return;

  int i;
  int imin = 0;
  for (i = 1; i <= aNbExt; ++i)
  {
    if (anExtLL.SquareDistance(i) < Precision::SquareConfusion())
    {
      imin = i;
      break;
    }
  }
  if (imin == 0)
    return;

  Extrema_POnCurv aP1, aP2;
  anExtLL.Points(imin, aP1, aP2);
  double aVExt = aP1.Parameter();
  if (aVExt <= V1 || aVExt >= V2)
    return;

  if (aVExt - V1 > V2 - aVExt)
  {
    V2 = aVExt;
  }
  else
  {
    V1 = aVExt;
  }
}

//=======================================================================
// function : TryCylinderCone
// purpose  :
// static method to try create cylindrical or conical surface
//=======================================================================
occ::handle<Geom_Surface> GeomConvert_SurfToAnaSurf::TryCylinerCone(const occ::handle<Geom_Surface>& theSurf,
                                                               const bool      theVCase,
                                                               const occ::handle<Geom_Curve>& theUmidiso,
                                                               const occ::handle<Geom_Curve>& theVmidiso,
                                                               const double       theU1,
                                                               const double       theU2,
                                                               const double       theV1,
                                                               const double       theV2,
                                                               const double       theToler)
{
  occ::handle<Geom_Surface> aNewSurf;
  double        param1, param2, cf1, cf2, cl1, cl2, aGap1, aGap2;
  occ::handle<Geom_Curve>   firstiso, lastiso;
  occ::handle<Geom_Circle>  firstisocirc, lastisocirc, midisocirc;
  gp_Dir               isoline;
  if (theVCase)
  {
    param1     = theU1;
    param2     = theU2;
    firstiso   = theSurf->VIso(theV1);
    lastiso    = theSurf->VIso(theV2);
    midisocirc = occ::down_cast<Geom_Circle>(theVmidiso);
    isoline    = occ::down_cast<Geom_Line>(theUmidiso)->Lin().Direction();
  }
  else
  {
    param1     = theV1;
    param2     = theV2;
    firstiso   = theSurf->UIso(theU1);
    lastiso    = theSurf->UIso(theU2);
    midisocirc = occ::down_cast<Geom_Circle>(theUmidiso);
    isoline    = occ::down_cast<Geom_Line>(theVmidiso)->Lin().Direction();
  }
  firstisocirc =
    occ::down_cast<Geom_Circle>(GeomConvert_CurveToAnaCurve::ComputeCurve(firstiso,
                                                                            theToler,
                                                                            param1,
                                                                            param2,
                                                                            cf1,
                                                                            cl1,
                                                                            aGap1,
                                                                            GeomConvert_Target,
                                                                            GeomAbs_Circle));
  lastisocirc =
    occ::down_cast<Geom_Circle>(GeomConvert_CurveToAnaCurve::ComputeCurve(lastiso,
                                                                            theToler,
                                                                            param1,
                                                                            param2,
                                                                            cf2,
                                                                            cl2,
                                                                            aGap2,
                                                                            GeomConvert_Target,
                                                                            GeomAbs_Circle));
  if (!firstisocirc.IsNull() || !lastisocirc.IsNull())
  {
    double R1, R2, R3;
    gp_Pnt        P1, P2, P3;
    if (!firstisocirc.IsNull())
    {
      R1 = firstisocirc->Circ().Radius();
      P1 = firstisocirc->Circ().Location();
    }
    else
    {
      R1 = 0;
      P1 = firstiso->Value((firstiso->LastParameter() - firstiso->FirstParameter()) / 2);
    }
    R2 = midisocirc->Circ().Radius();
    P2 = midisocirc->Circ().Location();
    if (!lastisocirc.IsNull())
    {
      R3 = lastisocirc->Circ().Radius();
      P3 = lastisocirc->Circ().Location();
    }
    else
    {
      R3 = 0;
      P3 = lastiso->Value((lastiso->LastParameter() - lastiso->FirstParameter()) / 2);
    }
    // cylinder
    if (((std::abs(R2 - R1)) < theToler) && ((std::abs(R3 - R1)) < theToler)
        && ((std::abs(R3 - R2)) < theToler))
    {
      gp_Ax3 Axes(P1, gp_Dir(gp_Vec(P1, P3)));
      aNewSurf = new Geom_CylindricalSurface(Axes, R1);
    }
    // cone
    else if ((((std::abs(R1)) > (std::abs(R2))) && ((std::abs(R2)) > (std::abs(R3))))
             || (((std::abs(R3)) > (std::abs(R2))) && ((std::abs(R2)) > (std::abs(R1)))))
    {
      double radius;
      gp_Ax3        Axes;
      double semiangle = gp_Vec(isoline).Angle(gp_Vec(P3, P1));
      if (semiangle > M_PI / 2)
        semiangle = M_PI - semiangle;
      if (R1 > R3)
      {
        radius = R3;
        Axes   = gp_Ax3(P3, gp_Dir(gp_Vec(P3, P1)));
      }
      else
      {
        radius = R1;
        Axes   = gp_Ax3(P1, gp_Dir(gp_Vec(P1, P3)));
      }
      aNewSurf = new Geom_ConicalSurface(Axes, semiangle, radius);
    }
  }
  return aNewSurf;
}

//=======================================================================
// function : GetCylByLS
// purpose  :
// static method to create cylinrical surface using least square method
//=======================================================================
static void GetLSGap(const occ::handle<NCollection_HArray1<gp_XYZ>>& thePoints,
                     const gp_Ax3&                      thePos,
                     const double                theR,
                     double&                     theGap)
{
  theGap = 0.;
  int i;
  gp_XYZ           aLoc = thePos.Location().XYZ();
  gp_Dir           aDir = thePos.Direction();
  for (i = thePoints->Lower(); i <= thePoints->Upper(); ++i)
  {
    gp_Vec aD(thePoints->Value(i) - aLoc);
    aD.Cross(aDir);
    theGap = std::max(theGap, std::abs((aD.Magnitude() - theR)));
  }
}

bool GeomConvert_SurfToAnaSurf::GetCylByLS(const occ::handle<NCollection_HArray1<gp_XYZ>>& thePoints,
                                                       const double                theTol,
                                                       gp_Ax3&                            thePos,
                                                       double&                     theR,
                                                       double&                     theGap)
{

  GetLSGap(thePoints, thePos, theR, theGap);
  if (theGap <= Precision::Confusion())
  {
    return true;
  }

  int i;

  int aNbVar = 4;

  math_Vector aFBnd(1, aNbVar), aLBnd(1, aNbVar), aStartPoint(1, aNbVar);

  double aRelDev = 0.2; // Customer can set parameters of sample surface
                               //  with relative precision about aRelDev.
                               //  For example, if radius of sample surface is R,
                               //  it means, that "exact" value is in interav
                               //[R - aRelDev*R, R + aRelDev*R]. This interval is set
                               //  for R as boundary values for optimization algo.

  aStartPoint(1)      = thePos.Location().X();
  aStartPoint(2)      = thePos.Location().Y();
  aStartPoint(3)      = thePos.Location().Z();
  aStartPoint(4)      = theR;
  double aDR   = aRelDev * theR;
  double aDXYZ = aDR;
  for (i = 1; i <= 3; ++i)
  {
    aFBnd(i) = aStartPoint(i) - aDXYZ;
    aLBnd(i) = aStartPoint(i) + aDXYZ;
  }
  aFBnd(4) = aStartPoint(4) - aDR;
  aLBnd(4) = aStartPoint(4) + aDR;

  //
  constexpr double        aTol = Precision::Confusion();
  math_MultipleVarFunction*      aPFunc;
  GeomConvert_FuncCylinderLSDist aFuncCyl(thePoints, thePos.Direction());
  aPFunc = (math_MultipleVarFunction*)&aFuncCyl;
  //
  math_Vector      aSteps(1, aNbVar);
  int aNbInt = 10;
  for (i = 1; i <= aNbVar; ++i)
  {
    aSteps(i) = (aLBnd(i) - aFBnd(i)) / aNbInt;
  }
  math_PSO      aGlobSolver(aPFunc, aFBnd, aLBnd, aSteps);
  double aLSDist;
  aGlobSolver.Perform(aSteps, aLSDist, aStartPoint);
  //
  gp_Pnt aLoc(aStartPoint(1), aStartPoint(2), aStartPoint(3));
  thePos.SetLocation(aLoc);
  theR = aStartPoint(4);

  GetLSGap(thePoints, thePos, theR, theGap);
  if (theGap <= aTol)
  {
    return true;
  }
  //
  math_Matrix aDirMatrix(1, aNbVar, 1, aNbVar, 0.0);
  for (i = 1; i <= aNbVar; i++)
    aDirMatrix(i, i) = 1.0;

  // Set search direction for location to be perpendicular to axis to avoid
  // searching along axis
  const gp_Dir aDir = thePos.Direction();
  gp_Pln       aPln(thePos.Location(), aDir);
  gp_Dir       aUDir = aPln.Position().XDirection();
  gp_Dir       aVDir = aPln.Position().YDirection();
  for (i = 1; i <= 3; ++i)
  {
    aDirMatrix(i, 1) = aUDir.Coord(i);
    aDirMatrix(i, 2) = aVDir.Coord(i);
    gp_Dir aUVDir(aUDir.XYZ() + aVDir.XYZ());
    aDirMatrix(i, 3) = aUVDir.Coord(i);
  }

  math_Powell aSolver(*aPFunc, aTol);
  aSolver.Perform(*aPFunc, aStartPoint, aDirMatrix);

  if (aSolver.IsDone())
  {
    gp_Ax3 aPos2 = thePos;
    aSolver.Location(aStartPoint);
    aLoc.SetCoord(aStartPoint(1), aStartPoint(2), aStartPoint(3));
    aPos2.SetLocation(aLoc);
    double anR2 = aStartPoint(4), aGap2 = 0.;
    //
    GetLSGap(thePoints, aPos2, anR2, aGap2);
    //
    if (aGap2 < theGap)
    {
      theGap = aGap2;
      thePos = aPos2;
      theR   = anR2;
    }
  }
  if (theGap <= theTol)
  {
    return true;
  }
  return false;
}

//=======================================================================
// function : TryCylinderByGaussField
// purpose  :
// static method to try create cylinrical surface based on its Gauss field
//=======================================================================

occ::handle<Geom_Surface> GeomConvert_SurfToAnaSurf::TryCylinderByGaussField(
  const occ::handle<Geom_Surface>& theSurf,
  const double         theU1,
  const double         theU2,
  const double         theV1,
  const double         theV2,
  const double         theToler,
  const int      theNbU,
  const int      theNbV,
  const bool      theLeastSquare)
{
  occ::handle<Geom_Surface>        aNewSurf;
  double               du = (theU2 - theU1) / theNbU, dv = (theV2 - theV1) / theNbV;
  double               aSigmaR = 0.;
  double               aTol    = 1.e3 * theToler;
  NCollection_Array1<double>        anRs(1, theNbU * theNbV);
  occ::handle<NCollection_HArray1<gp_XYZ>> aPoints;
  if (theLeastSquare)
  {
    aPoints = new NCollection_HArray1<gp_XYZ>(1, theNbU * theNbU);
  }
  //
  GeomLProp_SLProps aProps(theSurf, 2, Precision::Confusion());
  double     anAvMaxCurv = 0., anAvMinCurv = 0., anAvR = 0, aSign = 1.;
  gp_XYZ            anAvDir;
  gp_Dir            aMinD, aMaxD;
  int  i, j, n = 0;
  double     anU, aV;
  for (i = 1, anU = theU1 + du / 2.; i <= theNbU; ++i, anU += du)
  {
    for (j = 1, aV = theV1 + dv / 2.; j <= theNbV; ++j, aV += dv)
    {
      aProps.SetParameters(anU, aV);
      if (!aProps.IsCurvatureDefined())
      {
        return aNewSurf;
      }
      if (aProps.IsUmbilic())
      {
        return aNewSurf;
      }
      ++n;
      double aMinCurv   = aProps.MinCurvature();
      double aMaxCurv   = aProps.MaxCurvature();
      double aGaussCurv = std::abs(aProps.GaussianCurvature());
      double aK1        = std::sqrt(aGaussCurv);
      if (aK1 > theToler)
      {
        return aNewSurf;
      }
      gp_XYZ aD;
      aProps.CurvatureDirections(aMaxD, aMinD);
      aMinCurv = std::abs(aMinCurv);
      aMaxCurv = std::abs(aMaxCurv);
      if (aMinCurv > aMaxCurv)
      {
        // aMinCurv < 0;
        aSign = -1.;
        std::swap(aMinCurv, aMaxCurv);
        gp_Dir aDummy = aMaxD;
        aMaxD         = aMinD;
        aMinD         = aDummy;
      }
      double anR  = 1. / aMaxCurv;
      double anR2 = anR * anR;
      anRs(n)            = anR;
      //
      if (n > 1)
      {
        if (std::abs(aMaxCurv - anAvMaxCurv / (n - 1)) > aTol / anR2)
        {
          return aNewSurf;
        }
        if (std::abs(aMinCurv - anAvMinCurv / (n - 1)) > aTol)
        {
          return aNewSurf;
        }
      }
      aD = aMinD.XYZ();
      anAvR += anR;
      anAvDir += aD;
      anAvMaxCurv += aMaxCurv;
      anAvMinCurv += aMinCurv;
      if (theLeastSquare)
      {
        aPoints->SetValue(n, aProps.Value().XYZ());
      }
    }
  }
  anAvMaxCurv /= n;
  anAvMinCurv /= n;
  anAvR /= n;
  anAvDir /= n;
  //
  if (std::abs(anAvMinCurv) > theToler)
  {
    return aNewSurf;
  }
  //
  for (i = 1; i <= n; ++i)
  {
    double d = (anRs(i) - anAvR);
    aSigmaR += d * d;
  }
  aSigmaR = std::sqrt(aSigmaR / n);
  aSigmaR = 3. * aSigmaR / std::sqrt(n);
  if (aSigmaR > aTol)
  {
    return aNewSurf;
  }
  aProps.SetParameters(theU1, theV1);
  if (!aProps.IsCurvatureDefined())
  {
    return aNewSurf;
  }
  gp_Dir aNorm = aProps.Normal();
  gp_Pnt aLoc  = aProps.Value();
  gp_Dir anAxD(anAvDir);
  gp_Vec aT(aSign * anAvR * aNorm.XYZ());
  aLoc.Translate(aT);
  gp_Ax1      anAx1(aLoc, anAxD);
  gp_Cylinder aCyl;
  aCyl.SetAxis(anAx1);
  aCyl.SetRadius(anAvR);

  if (theLeastSquare)
  {
    gp_Ax3           aPos   = aCyl.Position();
    double    anR    = aCyl.Radius();
    double    aGap   = 0.;
    bool IsDone = GetCylByLS(aPoints, theToler, aPos, anR, aGap);
    if (IsDone)
    {
      aCyl.SetPosition(aPos);
      aCyl.SetRadius(anR);
    }
  }

  aNewSurf = new Geom_CylindricalSurface(aCyl);

  return aNewSurf;
}

//=======================================================================
// function : TryTorusSphere
// purpose  :
// static method to try create toroidal surface.
// In case <isTryUMajor> = true try to use V isoline radius as minor radaius.
//=======================================================================

occ::handle<Geom_Surface> GeomConvert_SurfToAnaSurf::TryTorusSphere(
  const occ::handle<Geom_Surface>& theSurf,
  const occ::handle<Geom_Circle>&  circle,
  const occ::handle<Geom_Circle>&  otherCircle,
  const double         Param1,
  const double         Param2,
  const double         aParam1ToCrv,
  const double         aParam2ToCrv,
  const double         toler,
  const bool      isTryUMajor)
{
  occ::handle<Geom_Surface> newSurface;
  double        cf, cl;
  occ::handle<Geom_Curve>   IsoCrv1;
  occ::handle<Geom_Curve>   IsoCrv2;
  double        aGap1, aGap2;
  // initial radius
  double R = circle->Circ().Radius();
  // iso lines

  if (isTryUMajor)
  {
    IsoCrv1 = theSurf->VIso(Param1 + ((Param2 - Param1) / 3.));
    IsoCrv2 = theSurf->VIso(Param1 + ((Param2 - Param1) * 2. / 3));
  }
  else
  {
    IsoCrv1 = theSurf->UIso(Param1 + ((Param2 - Param1) / 3.));
    IsoCrv2 = theSurf->UIso(Param1 + ((Param2 - Param1) * 2. / 3));
  }

  occ::handle<Geom_Curve> Crv1 = GeomConvert_CurveToAnaCurve::ComputeCurve(IsoCrv1,
                                                                      toler,
                                                                      aParam1ToCrv,
                                                                      aParam2ToCrv,
                                                                      cf,
                                                                      cl,
                                                                      aGap1,
                                                                      GeomConvert_Target,
                                                                      GeomAbs_Circle);
  occ::handle<Geom_Curve> Crv2 = GeomConvert_CurveToAnaCurve::ComputeCurve(IsoCrv2,
                                                                      toler,
                                                                      aParam1ToCrv,
                                                                      aParam2ToCrv,
                                                                      cf,
                                                                      cl,
                                                                      aGap2,
                                                                      GeomConvert_Target,
                                                                      GeomAbs_Circle);
  if (Crv1.IsNull() || Crv2.IsNull() || !Crv1->IsKind(STANDARD_TYPE(Geom_Circle))
      || !Crv2->IsKind(STANDARD_TYPE(Geom_Circle)))
    return newSurface;

  occ::handle<Geom_Circle> aCircle1 = occ::down_cast<Geom_Circle>(Crv1);
  occ::handle<Geom_Circle> aCircle2 = occ::down_cast<Geom_Circle>(Crv2);
  double       R1       = aCircle1->Circ().Radius();
  double       R2       = aCircle2->Circ().Radius();

  // check radiuses
  if ((std::abs(R - R1) > toler) || (std::abs(R - R2) > toler))
    return newSurface;

  // get centers of the major radius
  gp_Pnt aPnt1, aPnt2, aPnt3;
  aPnt1 = circle->Circ().Location();
  aPnt2 = aCircle1->Circ().Location();
  aPnt3 = aCircle2->Circ().Location();

  // double eps = 1.e-09;  // angular resolution
  double d0 = aPnt1.Distance(aPnt2);
  double d1 = aPnt1.Distance(aPnt3);
  gp_Circ       circ;

  if (d0 < toler || d1 < toler)
  {
    // compute sphere
    gp_Dir                        MainDir = otherCircle->Circ().Axis().Direction();
    gp_Ax3                        Axes(circle->Circ().Location(), MainDir);
    occ::handle<Geom_SphericalSurface> anObject = new Geom_SphericalSurface(Axes, R);
    if (!anObject.IsNull())
      newSurface = anObject;

    return newSurface;
  }

  if (!GeomConvert_CurveToAnaCurve::GetCircle(circ, aPnt1, aPnt2, aPnt3) /*, d0, d1, eps)*/)
    return newSurface;

  double aMajorR = circ.Radius();
  gp_Pnt        aCenter = circ.Location();
  gp_Dir        aDir((aPnt1.XYZ() - aCenter.XYZ()) ^ (aPnt3.XYZ() - aCenter.XYZ()));
  gp_Ax3        anAx3(aCenter, aDir);
  newSurface = new Geom_ToroidalSurface(anAx3, aMajorR, R);
  return newSurface;
}

//=================================================================================================

double GeomConvert_SurfToAnaSurf::ComputeGap(const occ::handle<Geom_Surface>& theSurf,
                                                    const double         theU1,
                                                    const double         theU2,
                                                    const double         theV1,
                                                    const double         theV2,
                                                    const occ::handle<Geom_Surface>& theNewSurf,
                                                    const double         theTol)
{
  GeomAdaptor_Surface aGAS(theNewSurf);
  GeomAbs_SurfaceType aSType = aGAS.GetType();
  gp_Pln              aPln;
  gp_Cylinder         aCyl;
  gp_Cone             aCon;
  gp_Sphere           aSphere;
  gp_Torus            aTor;
  switch (aSType)
  {
    case GeomAbs_Plane:
      aPln = aGAS.Plane();
      break;
    case GeomAbs_Cylinder:
      aCyl = aGAS.Cylinder();
      break;
    case GeomAbs_Cone:
      aCon = aGAS.Cone();
      break;
    case GeomAbs_Sphere:
      aSphere = aGAS.Sphere();
      break;
    case GeomAbs_Torus:
      aTor = aGAS.Torus();
      break;
    default:
      return Precision::Infinite();
      break;
  }

  double    aGap      = 0.;
  bool onSurface = true;

  double S, T;
  gp_Pnt        P3d, P3d2;

  const int NP = 21;
  double          DU, DV;
  int       j, i;
  DU                = (theU2 - theU1) / (NP - 1);
  DV                = (theV2 - theV1) / (NP - 1);
  double DU2 = DU / 2., DV2 = DV / 2.;
  for (j = 1; (j < NP) && onSurface; j++)
  {
    double V = theV1 + DV * (j - 1) + DV2;
    for (i = 1; i < NP; i++)
    {
      double U = theU1 + DU * (i - 1) + DU2;
      theSurf->D0(U, V, P3d);

      switch (aSType)
      {

        case GeomAbs_Plane: {
          ElSLib::Parameters(aPln, P3d, S, T);
          P3d2 = ElSLib::Value(S, T, aPln);
          break;
        }
        case GeomAbs_Cylinder: {
          ElSLib::Parameters(aCyl, P3d, S, T);
          P3d2 = ElSLib::Value(S, T, aCyl);
          break;
        }
        case GeomAbs_Cone: {
          ElSLib::Parameters(aCon, P3d, S, T);
          P3d2 = ElSLib::Value(S, T, aCon);
          break;
        }
        case GeomAbs_Sphere: {
          ElSLib::Parameters(aSphere, P3d, S, T);
          P3d2 = ElSLib::Value(S, T, aSphere);
          break;
        }
        case GeomAbs_Torus: {
          ElSLib::Parameters(aTor, P3d, S, T);
          P3d2 = ElSLib::Value(S, T, aTor);
          break;
        }
        default:
          S = 0.;
          T = 0.;
          theNewSurf->D0(S, T, P3d2);
          break;
      }

      double dis = P3d.Distance(P3d2);
      if (dis > aGap)
        aGap = dis;

      if (aGap > theTol)
      {
        onSurface = false;
        break;
      }
    }
  }
  return aGap;
}

//=================================================================================================

GeomConvert_SurfToAnaSurf::GeomConvert_SurfToAnaSurf()
    : myGap(-1.),
      myConvType(GeomConvert_Simplest),
      myTarget(GeomAbs_Plane)
{
}

//=================================================================================================

GeomConvert_SurfToAnaSurf::GeomConvert_SurfToAnaSurf(const occ::handle<Geom_Surface>& S)
    : myGap(-1.),
      myConvType(GeomConvert_Simplest),
      myTarget(GeomAbs_Plane)
{
  Init(S);
}

//=================================================================================================

void GeomConvert_SurfToAnaSurf::Init(const occ::handle<Geom_Surface>& S)
{
  mySurf = S;
}

//=================================================================================================

occ::handle<Geom_Surface> GeomConvert_SurfToAnaSurf::ConvertToAnalytical(
  const double InitialToler)
{
  double U1, U2, V1, V2;
  mySurf->Bounds(U1, U2, V1, V2);
  if (Precision::IsInfinite(U1) && Precision::IsInfinite(U2))
  {
    U1 = -1.;
    U2 = 1.;
  }
  if (Precision::IsInfinite(V1) && Precision::IsInfinite(V2))
  {
    V1                                        = -1.;
    V2                                        = 1.;
    occ::handle<Geom_SurfaceOfRevolution> aRevSurf = occ::down_cast<Geom_SurfaceOfRevolution>(mySurf);
    if (!aRevSurf.IsNull())
    {
      CheckVTrimForRevSurf(aRevSurf, V1, V2);
    }
  }
  return ConvertToAnalytical(InitialToler, U1, U2, V1, V2);
}

//=================================================================================================

occ::handle<Geom_Surface> GeomConvert_SurfToAnaSurf::ConvertToAnalytical(
  const double InitialToler,
  const double Umin,
  const double Umax,
  const double Vmin,
  const double Vmax)
{
  //
  GeomAdaptor_Surface aGAS(mySurf);
  GeomAbs_SurfaceType aSType = aGAS.GetType();
  switch (aSType)
  {
    case GeomAbs_Plane: {
      myGap = 0.;
      return new Geom_Plane(aGAS.Plane());
    }
    case GeomAbs_Cylinder: {
      myGap = 0.;
      return new Geom_CylindricalSurface(aGAS.Cylinder());
    }
    case GeomAbs_Cone: {
      myGap = 0.;
      return new Geom_ConicalSurface(aGAS.Cone());
    }
    case GeomAbs_Sphere: {
      myGap = 0.;
      return new Geom_SphericalSurface(aGAS.Sphere());
    }
    case GeomAbs_Torus: {
      myGap = 0.;
      return new Geom_ToroidalSurface(aGAS.Torus());
    }
    default:
      break;
  }
  //
  double        toler = InitialToler;
  occ::handle<Geom_Surface> newSurf[5];
  double        dd[5]      = {RealLast(), RealLast(), RealLast(), RealLast(), RealLast()};
  GeomAbs_SurfaceType  aSTypes[5] = {GeomAbs_Plane,
                                     GeomAbs_Cylinder,
                                     GeomAbs_Cone,
                                     GeomAbs_Sphere,
                                     GeomAbs_Torus};

  // Check boundaries
  double U1, U2, V1, V2;
  mySurf->Bounds(U1, U2, V1, V2);
  bool        aDoSegment = false;
  constexpr double aTolBnd    = Precision::PConfusion();
  int        isurf      = 0;
  if (Umin < U1 || Umax > U2 || Vmin < V1 || Vmax > V2)
  {
    return newSurf[isurf];
  }
  else
  {
    if (Umin - U1 > aTolBnd)
    {
      U1         = Umin;
      aDoSegment = true;
    }
    if (U2 - Umax > aTolBnd)
    {
      U2         = Umax;
      aDoSegment = true;
    }
    if (Vmin - V1 > aTolBnd)
    {
      V1         = Vmin;
      aDoSegment = true;
    }
    if (V2 - Vmax > aTolBnd)
    {
      V2         = Vmax;
      aDoSegment = true;
    }
  }

  bool IsBz = aSType == GeomAbs_BezierSurface;
  bool IsBs = aSType == GeomAbs_BSplineSurface;

  occ::handle<Geom_Surface> aTempS = mySurf;
  if (IsBs)
  {
    occ::handle<Geom_BSplineSurface> aBs = occ::down_cast<Geom_BSplineSurface>(mySurf->Copy());
    if (aDoSegment)
    {
      aBs->Segment(U1, U2, V1, V2);
    }
    aTempS = aBs;
  }
  else if (IsBz)
  {
    occ::handle<Geom_BezierSurface> aBz = occ::down_cast<Geom_BezierSurface>(mySurf->Copy());
    if (aDoSegment)
    {
      aBz->Segment(U1, U2, V1, V2);
    }
    aTempS = aBz;
  }
  // check the planarity first
  if (!IsBs && !IsBz)
  {
    aTempS = new Geom_RectangularTrimmedSurface(aTempS, U1, U2, V1, V2);
  }
  isurf = 0; // set plane
  GeomLib_IsPlanarSurface GeomIsPlanar(aTempS, toler);
  if (GeomIsPlanar.IsPlanar())
  {
    gp_Pln newPln  = GeomIsPlanar.Plan();
    newSurf[isurf] = new Geom_Plane(newPln);
    dd[isurf]      = ComputeGap(aTempS, U1, U2, V1, V2, newSurf[isurf]);
    if (myConvType == GeomConvert_Simplest
        || (myConvType == GeomConvert_Target && myTarget == GeomAbs_Plane))
    {
      myGap = dd[isurf];
      return newSurf[isurf];
    }
  }
  else
  {
    if (myConvType == GeomConvert_Target && myTarget == GeomAbs_Plane)
    {
      myGap = dd[isurf];
      return newSurf[isurf];
    }
  }

  double diagonal = mySurf->Value(U1, V1).Distance(mySurf->Value((U1 + U2), (V1 + V2) / 2));
  double twist    = 1000;
  if (toler > diagonal / twist)
    toler = diagonal / twist;

  isurf                           = 1; // set cylinder
  bool aCylinderConus = false;
  bool aToroidSphere  = false;

  // convert middle uiso and viso to canonical representation
  double VMid = 0.5 * (V1 + V2);
  double UMid = 0.5 * (U1 + U2);
  // occ::handle<Geom_Surface> TrSurf = aTempS;

  occ::handle<Geom_Curve> UIso = aTempS->UIso(UMid);
  occ::handle<Geom_Curve> VIso = aTempS->VIso(VMid);

  double      cuf, cul, cvf, cvl, aGap1, aGap2;
  bool   aLineIso = false;
  occ::handle<Geom_Curve> umidiso  = GeomConvert_CurveToAnaCurve::ComputeCurve(UIso,
                                                                         toler,
                                                                         V1,
                                                                         V2,
                                                                         cuf,
                                                                         cul,
                                                                         aGap1,
                                                                         GeomConvert_Simplest);
  if (!umidiso.IsNull())
  {
    aLineIso = umidiso->IsKind(STANDARD_TYPE(Geom_Line));
  }
  occ::handle<Geom_Curve> vmidiso = GeomConvert_CurveToAnaCurve::ComputeCurve(VIso,
                                                                         toler,
                                                                         U1,
                                                                         U2,
                                                                         cvf,
                                                                         cvl,
                                                                         aGap2,
                                                                         GeomConvert_Simplest);
  if (!vmidiso.IsNull() && !aLineIso)
  {
    aLineIso = vmidiso->IsKind(STANDARD_TYPE(Geom_Line));
  }
  if (!umidiso.IsNull() && !vmidiso.IsNull())
  {
    //
    bool VCase = false;

    if (umidiso->IsKind(STANDARD_TYPE(Geom_Circle)) && vmidiso->IsKind(STANDARD_TYPE(Geom_Circle)))
    {
      aToroidSphere = true;
      if (myConvType == GeomConvert_Target
          && (myTarget == GeomAbs_Cylinder || myTarget == GeomAbs_Cone))
      {
        isurf = 1;
        myGap = dd[isurf];
        return newSurf[isurf];
      }
      isurf = 3; // set sphere
    }
    else if (umidiso->IsKind(STANDARD_TYPE(Geom_Line))
             && vmidiso->IsKind(STANDARD_TYPE(Geom_Circle)))
    {
      aCylinderConus = true;
      VCase          = true;
      if (myConvType == GeomConvert_Target
          && (myTarget == GeomAbs_Sphere || myTarget == GeomAbs_Torus))
      {
        isurf = 3;
        myGap = dd[isurf];
        return newSurf[isurf];
      }
      isurf = 1; // set cylinder
    }
    else if (umidiso->IsKind(STANDARD_TYPE(Geom_Circle))
             && vmidiso->IsKind(STANDARD_TYPE(Geom_Line)))
    {
      aCylinderConus = true;
      if (myConvType == GeomConvert_Target
          && (myTarget == GeomAbs_Sphere || myTarget == GeomAbs_Torus))
      {
        isurf = 3;
        myGap = dd[isurf];
        return newSurf[isurf];
      }
      isurf = 1; // set cylinder
    }

    // case of torus-sphere
    if (aToroidSphere)
    {

      isurf = 3; // Set spherical surface

      occ::handle<Geom_Circle> Ucircle = occ::down_cast<Geom_Circle>(umidiso);
      occ::handle<Geom_Circle> Vcircle = occ::down_cast<Geom_Circle>(vmidiso);
      // torus
      // try when V isolines is with same radius
      occ::handle<Geom_Surface> anObject =
        TryTorusSphere(mySurf, Vcircle, Ucircle, V1, V2, U1, U2, toler, true);
      if (anObject.IsNull()) // try when U isolines is with same radius
        anObject = TryTorusSphere(mySurf, Ucircle, Vcircle, U1, U2, V1, V2, toler, false);

      if (!anObject.IsNull())
      {
        if (anObject->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
        {
          isurf = 4; // set torus
        }
        newSurf[isurf] = anObject;
        if (myConvType == GeomConvert_Target && (myTarget != aSTypes[isurf]))
        {
          myGap = RealLast();
          return NULL;
        }
      }
      else
      {
        myGap = dd[isurf];
      }
    }
    // case of cone - cylinder
    else if (aCylinderConus)
    {
      isurf = 1; // set cylindrical  surface
      occ::handle<Geom_Surface> anObject =
        TryCylinerCone(aTempS, VCase, umidiso, vmidiso, U1, U2, V1, V2, toler);
      if (!anObject.IsNull())
      {
        if (anObject->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
        {
          isurf = 2; // set conical surface
        }
        if (myConvType == GeomConvert_Target && (myTarget != aSTypes[isurf]))
        {
          myGap = RealLast();
          return NULL;
        }
        newSurf[isurf] = anObject;
      }
      else
      {
        aCylinderConus = false;
        myGap          = dd[isurf];
      }
    }
  }
  // Additional checking for case of cylinder
  if (!aCylinderConus && !aToroidSphere && aLineIso)
  {
    // Try cylinder using Gauss field
    int     aNbU = 7, aNbV = 7;
    bool     aLeastSquare = true;
    occ::handle<Geom_Surface> anObject =
      TryCylinderByGaussField(aTempS, U1, U2, V1, V2, toler, aNbU, aNbV, aLeastSquare);
    if (!anObject.IsNull())
    {
      isurf          = 1;
      newSurf[isurf] = anObject;
    }
  }

  //
  //---------------------------------------------------------------------
  //                 verification
  //---------------------------------------------------------------------
  int imin  = -1;
  double    aDmin = RealLast();
  for (isurf = 0; isurf < 5; ++isurf)
  {
    if (newSurf[isurf].IsNull())
      continue;
    dd[isurf] = ComputeGap(aTempS, U1, U2, V1, V2, newSurf[isurf], toler);
    if (dd[isurf] <= toler)
    {
      if (myConvType == GeomConvert_Simplest
          || (myConvType == GeomConvert_Target && myTarget == aSTypes[isurf]))
      {
        myGap = dd[isurf];
        return newSurf[isurf];
      }
      else if (myConvType == GeomConvert_MinGap)
      {
        if (dd[isurf] < aDmin)
        {
          aDmin = dd[isurf];
          imin  = isurf;
        }
      }
    }
  }
  //
  if (imin >= 0)
  {
    myGap = dd[imin];
    return newSurf[imin];
  }

  return NULL;
}

//=================================================================================================

bool GeomConvert_SurfToAnaSurf::IsSame(const occ::handle<Geom_Surface>& S1,
                                                   const occ::handle<Geom_Surface>& S2,
                                                   const double         tol)
{
  // only elementary surfaces are processed
  if (!S1->IsKind(STANDARD_TYPE(Geom_ElementarySurface))
      || !S2->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    return false;

  occ::handle<GeomAdaptor_Surface> anAdaptor1 = new GeomAdaptor_Surface(S1);
  occ::handle<GeomAdaptor_Surface> anAdaptor2 = new GeomAdaptor_Surface(S2);

  GeomAbs_SurfaceType aST1 = anAdaptor1->GetType();
  GeomAbs_SurfaceType aST2 = anAdaptor2->GetType();

  if (aST1 != aST2)
  {
    return false;
  }

  IntAna_QuadQuadGeo interii;
  if (aST1 == GeomAbs_Plane)
  {
    interii.Perform(anAdaptor1->Plane(), anAdaptor2->Plane(), tol, tol);
  }
  else if (aST1 == GeomAbs_Cylinder)
  {
    interii.Perform(anAdaptor1->Cylinder(), anAdaptor2->Cylinder(), tol);
  }
  else if (aST1 == GeomAbs_Cone)
  {
    interii.Perform(anAdaptor1->Cone(), anAdaptor2->Cone(), tol);
  }
  else if (aST1 == GeomAbs_Sphere)
  {
    interii.Perform(anAdaptor1->Sphere(), anAdaptor2->Sphere(), tol);
  }
  else if (aST1 == GeomAbs_Torus)
  {
    interii.Perform(anAdaptor1->Torus(), anAdaptor2->Torus(), tol);
  }

  if (!interii.IsDone())
    return false;

  IntAna_ResultType aTypeRes = interii.TypeInter();

  return aTypeRes == IntAna_Same;
}

//=================================================================================================

bool GeomConvert_SurfToAnaSurf::IsCanonical(const occ::handle<Geom_Surface>& S)
{
  if (S.IsNull())
    return false;

  if (S->IsKind(STANDARD_TYPE(Geom_Plane)) || S->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))
      || S->IsKind(STANDARD_TYPE(Geom_ConicalSurface))
      || S->IsKind(STANDARD_TYPE(Geom_SphericalSurface))
      || S->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
    return true;

  return false;
}
