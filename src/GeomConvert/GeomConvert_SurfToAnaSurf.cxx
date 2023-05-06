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

//abv 06.01.99 fix of misprint
//:p6 abv 26.02.99: make ConvertToPeriodic() return Null if nothing done

#include <BRepTopAdaptor_TopolTool.hxx>
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
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Extrema_ExtElC.hxx>
#include <GeomLProp_SLProps.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <math_Vector.hxx>
#include <math_PSO.hxx>
#include <math_Powell.hxx>
#include <GeomConvert_FuncCylinderLSDist.hxx>

//=======================================================================
//function : CheckVTrimForRevSurf
//purpose  : 
//static method for checking surface of revolution
//To avoid two-parts cone-like surface
//=======================================================================
void GeomConvert_SurfToAnaSurf::CheckVTrimForRevSurf(const Handle(Geom_SurfaceOfRevolution)& aRevSurf,
  Standard_Real& V1, Standard_Real& V2)
{
  const Handle(Geom_Curve)& aBC = aRevSurf->BasisCurve();
  Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast(aBC);
  if (aLine.IsNull())
    return;
  const gp_Ax1& anAxis = aRevSurf->Axis();

  gp_Lin anALin(anAxis);
  Extrema_ExtElC anExtLL(aLine->Lin(), anALin, Precision::Angular());
  if (!anExtLL.IsDone() || anExtLL.IsParallel())
    return;
  Standard_Integer aNbExt = anExtLL.NbExt();
  if (aNbExt == 0)
    return;

  Standard_Integer i;
  Standard_Integer imin = 0;
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
  Standard_Real aVExt = aP1.Parameter();
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
//function : TryCylinderCone
//purpose  : 
//static method to try create cylindrical or conical surface
//=======================================================================
Handle(Geom_Surface) GeomConvert_SurfToAnaSurf::TryCylinerCone(const Handle(Geom_Surface)& theSurf, const Standard_Boolean theVCase,
  const Handle(Geom_Curve)& theUmidiso, const Handle(Geom_Curve)& theVmidiso,
  const Standard_Real theU1, const Standard_Real theU2, const Standard_Real theV1, const Standard_Real theV2,
  const Standard_Real theToler)
{
  Handle(Geom_Surface) aNewSurf;
  Standard_Real param1, param2, cf1, cf2, cl1, cl2, aGap1, aGap2;
  Handle(Geom_Curve) firstiso, lastiso;
  Handle(Geom_Circle) firstisocirc, lastisocirc, midisocirc;
  gp_Dir isoline;
  if (theVCase) {
    param1 = theU1; param2 = theU2;
    firstiso = theSurf->VIso(theV1);
    lastiso = theSurf->VIso(theV2);
    midisocirc = Handle(Geom_Circle)::DownCast(theVmidiso);
    isoline = Handle(Geom_Line)::DownCast(theUmidiso)->Lin().Direction();
  }
  else {
    param1 = theV1; param2 = theV2;
    firstiso = theSurf->UIso(theU1);
    lastiso = theSurf->UIso(theU2);
    midisocirc = Handle(Geom_Circle)::DownCast(theUmidiso);
    isoline = Handle(Geom_Line)::DownCast(theVmidiso)->Lin().Direction();
  }
  firstisocirc = Handle(Geom_Circle)::DownCast(GeomConvert_CurveToAnaCurve::ComputeCurve(firstiso, theToler, 
    param1, param2, cf1, cl1, aGap1, GeomConvert_Target, GeomAbs_Circle));
  lastisocirc = Handle(Geom_Circle)::DownCast(GeomConvert_CurveToAnaCurve::ComputeCurve(lastiso, theToler, 
    param1, param2, cf2, cl2, aGap2, GeomConvert_Target, GeomAbs_Circle));
  if (!firstisocirc.IsNull() || !lastisocirc.IsNull()) {
    Standard_Real R1, R2, R3;
    gp_Pnt P1, P2, P3;
    if (!firstisocirc.IsNull()) {
      R1 = firstisocirc->Circ().Radius();
      P1 = firstisocirc->Circ().Location();
    }
    else {
      R1 = 0;
      P1 = firstiso->Value((firstiso->LastParameter() - firstiso->FirstParameter()) / 2);
    }
    R2 = midisocirc->Circ().Radius();
    P2 = midisocirc->Circ().Location();
    if (!lastisocirc.IsNull()) {
      R3 = lastisocirc->Circ().Radius();
      P3 = lastisocirc->Circ().Location();
    }
    else {
      R3 = 0;
      P3 = lastiso->Value((lastiso->LastParameter() - lastiso->FirstParameter()) / 2);
    }
    //cylinder
    if (((Abs(R2 - R1)) < theToler) && ((Abs(R3 - R1)) < theToler) &&
      ((Abs(R3 - R2)) < theToler)) 
    {
      gp_Ax3 Axes(P1, gp_Dir(gp_Vec(P1, P3)));
      aNewSurf = new Geom_CylindricalSurface(Axes, R1);
    }
    //cone
    else if ((((Abs(R1)) > (Abs(R2))) && ((Abs(R2)) > (Abs(R3)))) ||
      (((Abs(R3)) > (Abs(R2))) && ((Abs(R2)) > (Abs(R1))))) {
      Standard_Real radius;
      gp_Ax3 Axes;
      Standard_Real semiangle =
        gp_Vec(isoline).Angle(gp_Vec(P3, P1));
      if (semiangle > M_PI / 2)  semiangle = M_PI - semiangle;
      if (R1 > R3) {
        radius = R3;
        Axes = gp_Ax3(P3, gp_Dir(gp_Vec(P3, P1)));
      }
      else {
        radius = R1;
        Axes = gp_Ax3(P1, gp_Dir(gp_Vec(P1, P3)));
      }
      aNewSurf = new Geom_ConicalSurface(Axes, semiangle, radius);
    }
  }
  return aNewSurf;
}

//=======================================================================
//function : GetCylByLS
//purpose  : 
//static method to create cylinrical surface using least square method
//=======================================================================
static void GetLSGap(const Handle(TColgp_HArray1OfXYZ)& thePoints, const gp_Ax3& thePos, 
  const Standard_Real theR, Standard_Real& theGap)
{
  theGap = 0.;
  Standard_Integer i;
  gp_XYZ aLoc = thePos.Location().XYZ();
  gp_Dir aDir = thePos.Direction();
  for (i = thePoints->Lower(); i <= thePoints->Upper(); ++i)
  {
    gp_Vec aD(thePoints->Value(i) - aLoc);
    aD.Cross(aDir);
    theGap = Max(theGap, Abs((aD.Magnitude() - theR)));
  }

}
Standard_Boolean GeomConvert_SurfToAnaSurf::GetCylByLS(const Handle(TColgp_HArray1OfXYZ)& thePoints,
  const Standard_Real theTol,
  gp_Ax3& thePos, Standard_Real& theR,
  Standard_Real& theGap)
{

  GetLSGap(thePoints, thePos, theR, theGap);
  if (theGap <= Precision::Confusion())
  {
    return Standard_True;
  }

  Standard_Integer i;

  Standard_Integer aNbVar = 4;

  math_Vector aFBnd(1, aNbVar), aLBnd(1, aNbVar), aStartPoint(1, aNbVar);

  Standard_Real aRelDev = 0.2; //Customer can set parameters of sample surface
                               // with relative precision about aRelDev.
                               // For example, if radius of sample surface is R,
                               // it means, that "exact" vaue is in interav 
                               //[R - aRelDev*R, R + aRelDev*R]. This intrrval is set
                               // for R as boundary values for dptimization algo.

  aStartPoint(1) = thePos.Location().X();
  aStartPoint(2) = thePos.Location().Y();
  aStartPoint(3) = thePos.Location().Z();
  aStartPoint(4) = theR;
  Standard_Real aDR = aRelDev * theR;
  Standard_Real aDXYZ = aDR;
  for (i = 1; i <= 3; ++i)
  {
    aFBnd(i) = aStartPoint(i) - aDXYZ;
    aLBnd(i) = aStartPoint(i) + aDXYZ;
  }
  aFBnd(4) = aStartPoint(4) - aDR;
  aLBnd(4) = aStartPoint(4) + aDR;

  //
  Standard_Real aTol = Precision::Confusion();
  math_MultipleVarFunction* aPFunc;
  GeomConvert_FuncCylinderLSDist aFuncCyl(thePoints, thePos.Direction());
  aPFunc = (math_MultipleVarFunction*)&aFuncCyl;
  //
  math_Vector aSteps(1, aNbVar);
  Standard_Integer aNbInt = 10;
  for (i = 1; i <= aNbVar; ++i)
  {
    aSteps(i) = (aLBnd(i) - aFBnd(i)) / aNbInt;
  }
  math_PSO aGlobSolver(aPFunc, aFBnd, aLBnd, aSteps);
  Standard_Real aLSDist;
  aGlobSolver.Perform(aSteps, aLSDist, aStartPoint);
  //
  gp_Pnt aLoc(aStartPoint(1), aStartPoint(2), aStartPoint(3));
  thePos.SetLocation(aLoc);
  theR = aStartPoint(4);

  GetLSGap(thePoints, thePos, theR, theGap);
  if (theGap <= aTol)
  {
    return Standard_True;
  }
  //
  math_Matrix aDirMatrix(1, aNbVar, 1, aNbVar, 0.0);
  for (i = 1; i <= aNbVar; i++)
    aDirMatrix(i, i) = 1.0;

  //Set search direction for location to be perpendicular to axis to avoid
  //seaching along axis
  const gp_Dir aDir = thePos.Direction();
  gp_Pln aPln(thePos.Location(), aDir);
  gp_Dir aUDir = aPln.Position().XDirection();
  gp_Dir aVDir = aPln.Position().YDirection();
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
    Standard_Real anR2 = aStartPoint(4), aGap2 = 0.;
    //
    GetLSGap(thePoints, aPos2, anR2, aGap2);
    //
    if (aGap2 < theGap)
    {
      theGap = aGap2;
      thePos = aPos2;
      theR = anR2;
    }
  }
  if (theGap <= theTol)
  {
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : TryCylinderByGaussField
//purpose  : 
//static method to try create cylinrical surface based on its Gauss field
//=======================================================================

Handle(Geom_Surface) GeomConvert_SurfToAnaSurf::TryCylinderByGaussField(const Handle(Geom_Surface)& theSurf,
  const Standard_Real theU1, const Standard_Real theU2, const Standard_Real theV1, const Standard_Real theV2,
  const Standard_Real theToler, const Standard_Integer theNbU, const Standard_Integer theNbV,
  const Standard_Boolean theLeastSquare)
{
  Handle(Geom_Surface) aNewSurf;
  Standard_Real du = (theU2 - theU1) / theNbU, dv = (theV2 - theV1) / theNbV;
  Standard_Real aSigmaR = 0.;
  Standard_Real aTol = 1.e3 * theToler;
  TColStd_Array1OfReal anRs(1, theNbU*theNbV);
  Handle(TColgp_HArray1OfXYZ) aPoints;
  if (theLeastSquare)
  {
    aPoints = new TColgp_HArray1OfXYZ(1, theNbU*theNbU);
  }
  //
  GeomLProp_SLProps aProps(theSurf, 2, Precision::Confusion());
  Standard_Real anAvMaxCurv = 0., anAvMinCurv = 0., anAvR = 0, aSign = 1.;
  gp_XYZ anAvDir;
  gp_Dir aMinD, aMaxD;
  Standard_Integer i, j, n = 0;
  Standard_Real anU, aV;
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
      Standard_Real aMinCurv = aProps.MinCurvature();
      Standard_Real aMaxCurv = aProps.MaxCurvature();
      Standard_Real aGaussCurv = Abs(aProps.GaussianCurvature());
      Standard_Real aK1 = Sqrt(aGaussCurv);
      if (aK1 > theToler )
      {
        return aNewSurf;
      }
      gp_XYZ aD;
      aProps.CurvatureDirections(aMaxD, aMinD);
      aMinCurv = Abs(aMinCurv);
      aMaxCurv = Abs(aMaxCurv);
      if (aMinCurv > aMaxCurv)
      {
        //aMinCurv < 0;
        aSign = -1.;
        std::swap(aMinCurv, aMaxCurv);
        gp_Dir aDummy = aMaxD;
        aMaxD = aMinD;
        aMinD = aDummy;
      }
      Standard_Real anR = 1. / aMaxCurv;
      Standard_Real anR2 = anR * anR;
      anRs(n) = anR;
      //
      if (n > 1)
      {
        if (Abs(aMaxCurv - anAvMaxCurv / (n - 1)) > aTol / anR2)
        {
          return aNewSurf;
        }
        if (Abs(aMinCurv - anAvMinCurv / (n - 1)) > aTol)
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
  if (Abs(anAvMinCurv) > theToler)
  {
    return aNewSurf;
  }
  //
  for (i = 1; i <= n; ++i)
  {
    Standard_Real d = (anRs(i) - anAvR);
    aSigmaR += d * d;
  }
  aSigmaR = Sqrt(aSigmaR / n);
  aSigmaR = 3.*aSigmaR / Sqrt(n);
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
  gp_Pnt aLoc = aProps.Value();
  gp_Dir anAxD(anAvDir);
  gp_Vec aT(aSign*anAvR*aNorm.XYZ());
  aLoc.Translate(aT);
  gp_Ax1 anAx1(aLoc, anAxD);
  gp_Cylinder aCyl;
  aCyl.SetAxis(anAx1);
  aCyl.SetRadius(anAvR);

  if (theLeastSquare)
  {
    gp_Ax3 aPos = aCyl.Position();
    Standard_Real anR = aCyl.Radius();
    Standard_Real aGap = 0.;
    Standard_Boolean IsDone = GetCylByLS(aPoints, theToler, aPos, anR, aGap);
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
//function : TryTorusSphere
//purpose  : 
// static method to try create toroidal surface.
// In case <isTryUMajor> = Standard_True try to use V isoline radius as minor radaius.
//=======================================================================

Handle(Geom_Surface) GeomConvert_SurfToAnaSurf::TryTorusSphere(const Handle(Geom_Surface)& theSurf,
  const Handle(Geom_Circle)& circle,
  const Handle(Geom_Circle)& otherCircle,
  const Standard_Real Param1,
  const Standard_Real Param2,
  const Standard_Real aParam1ToCrv,
  const Standard_Real aParam2ToCrv,
  const Standard_Real toler,
  const Standard_Boolean isTryUMajor)
{
  Handle(Geom_Surface) newSurface;
  Standard_Real cf, cl;
  Handle(Geom_Curve) IsoCrv1;
  Handle(Geom_Curve) IsoCrv2;
  Standard_Real aGap1, aGap2;
  // initial radius
  Standard_Real R = circle->Circ().Radius();
  // iso lines

  if (isTryUMajor)
  {
    IsoCrv1 = theSurf->VIso(Param1 + ((Param2 - Param1) / 3.));
    IsoCrv2 = theSurf->VIso(Param1 + ((Param2 - Param1)*2. / 3));
  }
  else
  {
    IsoCrv1 = theSurf->UIso(Param1 + ((Param2 - Param1) / 3.));
    IsoCrv2 = theSurf->UIso(Param1 + ((Param2 - Param1)*2. / 3));
  }

  Handle(Geom_Curve) Crv1 = GeomConvert_CurveToAnaCurve::ComputeCurve(IsoCrv1, toler, aParam1ToCrv, aParam2ToCrv, cf, cl,
    aGap1, GeomConvert_Target, GeomAbs_Circle);
  Handle(Geom_Curve) Crv2 = GeomConvert_CurveToAnaCurve::ComputeCurve(IsoCrv2, toler, aParam1ToCrv, aParam2ToCrv, cf, cl,
    aGap2, GeomConvert_Target, GeomAbs_Circle);
  if (Crv1.IsNull() || Crv2.IsNull() ||
    !Crv1->IsKind(STANDARD_TYPE(Geom_Circle)) ||
    !Crv2->IsKind(STANDARD_TYPE(Geom_Circle)))
    return newSurface;

  Handle(Geom_Circle) aCircle1 = Handle(Geom_Circle)::DownCast(Crv1);
  Handle(Geom_Circle) aCircle2 = Handle(Geom_Circle)::DownCast(Crv2);
  Standard_Real R1 = aCircle1->Circ().Radius();
  Standard_Real R2 = aCircle2->Circ().Radius();

  // check radiuses
  if ((Abs(R - R1) > toler) || (Abs(R - R2) > toler))
    return newSurface;

  // get centers of the major radius
  gp_Pnt aPnt1, aPnt2, aPnt3;
  aPnt1 = circle->Circ().Location();
  aPnt2 = aCircle1->Circ().Location();
  aPnt3 = aCircle2->Circ().Location();

  //Standard_Real eps = 1.e-09;  // angular resolution
  Standard_Real d0 = aPnt1.Distance(aPnt2);  Standard_Real d1 = aPnt1.Distance(aPnt3);
  gp_Circ circ;

  if (d0 < toler || d1 < toler)
  {
    // compute sphere
    gp_Dir MainDir = otherCircle->Circ().Axis().Direction();
    gp_Ax3 Axes(circle->Circ().Location(), MainDir);
    Handle(Geom_SphericalSurface) anObject =
      new Geom_SphericalSurface(Axes, R);
    if (!anObject.IsNull())
      newSurface = anObject;

    return newSurface;
  }

  if (!GeomConvert_CurveToAnaCurve::GetCircle(circ, aPnt1, aPnt2, aPnt3) /*, d0, d1, eps)*/)
    return newSurface;

  Standard_Real aMajorR = circ.Radius();
  gp_Pnt aCenter = circ.Location();
  gp_Dir aDir((aPnt1.XYZ() - aCenter.XYZ()) ^ (aPnt3.XYZ() - aCenter.XYZ()));
  gp_Ax3 anAx3(aCenter, aDir);
  newSurface = new Geom_ToroidalSurface(anAx3, aMajorR, R);
  return newSurface;
}

//=======================================================================
//function : ComputeGap
//purpose  : 
//=======================================================================

Standard_Real GeomConvert_SurfToAnaSurf::ComputeGap(const Handle(Geom_Surface)& theSurf,
  const Standard_Real theU1, const Standard_Real theU2, const Standard_Real theV1, const Standard_Real theV2,
  const Handle(Geom_Surface)& theNewSurf, const Standard_Real theTol)
{
  GeomAdaptor_Surface aGAS(theNewSurf);
  GeomAbs_SurfaceType aSType = aGAS.GetType();
  gp_Pln aPln;
  gp_Cylinder aCyl;
  gp_Cone aCon;
  gp_Sphere aSphere;
  gp_Torus aTor;
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

  Standard_Real aGap = 0.;
  Standard_Boolean onSurface = Standard_True;

  Standard_Real S, T;
  gp_Pnt P3d, P3d2;

  const Standard_Integer NP = 21;
  Standard_Real DU, DV;
  Standard_Integer j, i;
  DU = (theU2 - theU1) / (NP - 1);
  DV = (theV2 - theV1) / (NP - 1);
  Standard_Real DU2 = DU / 2., DV2 = DV / 2.;
  for (j = 1; (j < NP) && onSurface; j++) {
    Standard_Real V = theV1 + DV*(j - 1) + DV2;
    for (i = 1; i < NP; i++) {
      Standard_Real U = theU1 + DU*(i - 1) + DU2;
      theSurf->D0(U, V, P3d);

      switch (aSType) {

      case GeomAbs_Plane:
      {
        ElSLib::Parameters(aPln, P3d, S, T);
        P3d2 = ElSLib::Value(S, T, aPln);
        break;
      }
      case GeomAbs_Cylinder:
      {
        ElSLib::Parameters(aCyl, P3d, S, T);
        P3d2 = ElSLib::Value(S, T, aCyl);
        break;
      }
      case GeomAbs_Cone:
      {
        ElSLib::Parameters(aCon, P3d, S, T);
        P3d2 = ElSLib::Value(S, T, aCon);
        break;
      }
      case GeomAbs_Sphere:
      {
        ElSLib::Parameters(aSphere, P3d, S, T);
        P3d2 = ElSLib::Value(S, T, aSphere);
        break;
      }
      case GeomAbs_Torus:
      {
        ElSLib::Parameters(aTor, P3d, S, T);
        P3d2 = ElSLib::Value(S, T, aTor);
        break;
      }
      default:
        S = 0.; T = 0.;
        theNewSurf->D0(S, T, P3d2);
        break;
      }

      Standard_Real dis = P3d.Distance(P3d2);
      if (dis > aGap) aGap = dis;

      if (aGap > theTol) {
        onSurface = Standard_False;
        break;
      }
    }
  }
  return aGap;
}
//=======================================================================
//function : GeomConvert_SurfToAnaSurf
//purpose  : 
//=======================================================================
GeomConvert_SurfToAnaSurf::GeomConvert_SurfToAnaSurf() :
       myGap (-1.), 
       myConvType (GeomConvert_Simplest),
       myTarget (GeomAbs_Plane)
{
}

//=======================================================================
//function : GeomConvert_SurfToAnaSurf
//purpose  : 
//=======================================================================

GeomConvert_SurfToAnaSurf::GeomConvert_SurfToAnaSurf (const Handle(Geom_Surface)& S) :
  myGap(-1.),
  myConvType(GeomConvert_Simplest),
  myTarget(GeomAbs_Plane)
{
  Init ( S );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomConvert_SurfToAnaSurf::Init (const Handle(Geom_Surface)& S) 
{
  mySurf = S;
}

//=======================================================================
//function : ConvertToAnalytical
//purpose  : 
//=======================================================================

Handle(Geom_Surface) GeomConvert_SurfToAnaSurf::ConvertToAnalytical (const Standard_Real InitialToler) 
{
  Standard_Real U1, U2, V1, V2;
  mySurf->Bounds(U1, U2, V1, V2);
  if(Precision::IsInfinite(U1) && Precision::IsInfinite(U2) ) {
    U1 = -1.;
    U2 = 1.;
  }
  if(Precision::IsInfinite(V1) && Precision::IsInfinite(V2) ) {
    V1 = -1.;
    V2 = 1.;
    Handle(Geom_SurfaceOfRevolution) aRevSurf = Handle(Geom_SurfaceOfRevolution)::DownCast(mySurf);
    if (!aRevSurf.IsNull())
    {
      CheckVTrimForRevSurf(aRevSurf, V1, V2);
    }
  }
  return ConvertToAnalytical(InitialToler, U1, U2, V1, V2); 
}

//=======================================================================
//function : ConvertToAnalytical
//purpose  : 
//=======================================================================
Handle(Geom_Surface) GeomConvert_SurfToAnaSurf::ConvertToAnalytical(const Standard_Real InitialToler,
  const Standard_Real Umin, const Standard_Real Umax,
  const Standard_Real Vmin, const Standard_Real Vmax)
{
  //
  GeomAdaptor_Surface aGAS(mySurf);
  GeomAbs_SurfaceType aSType = aGAS.GetType();
  switch (aSType)
  {
    case GeomAbs_Plane:
    {
      myGap = 0.;
      return new Geom_Plane(aGAS.Plane());
    }
    case GeomAbs_Cylinder:
    {
      myGap = 0.;
      return new Geom_CylindricalSurface(aGAS.Cylinder());
    }
    case GeomAbs_Cone:
    {
      myGap = 0.;
      return new Geom_ConicalSurface(aGAS.Cone());
    }
    case GeomAbs_Sphere:
    {
      myGap = 0.;
      return new Geom_SphericalSurface(aGAS.Sphere());
    }
    case GeomAbs_Torus:
    {
      myGap = 0.;
      return new Geom_ToroidalSurface(aGAS.Torus());
    }
    default:
      break;
  }
  //
  Standard_Real toler = InitialToler;
  Handle(Geom_Surface) newSurf[5];
  Standard_Real dd[5] = { RealLast(), RealLast(), RealLast(), RealLast(), RealLast() };
  GeomAbs_SurfaceType aSTypes[5] = { GeomAbs_Plane, GeomAbs_Cylinder,
    GeomAbs_Cone, GeomAbs_Sphere, GeomAbs_Torus };

  //Check boundaries
  Standard_Real U1, U2, V1, V2;
  mySurf->Bounds(U1, U2, V1, V2);
  Standard_Boolean aDoSegment = Standard_False;
  Standard_Real aTolBnd = Precision::PConfusion();
  Standard_Integer isurf = 0;
  if (Umin < U1 || Umax > U2 || Vmin < V1 || Vmax > V2)
  {
    return newSurf[isurf];
  }
  else 
  {
    if (Umin - U1 > aTolBnd)
    {
      U1 = Umin;
      aDoSegment = Standard_True;
    }
    if (U2 - Umax > aTolBnd)
    {
      U2 = Umax;
      aDoSegment = Standard_True;
    }
    if (Vmin - V1 > aTolBnd)
    {
      V1 = Vmin;
      aDoSegment = Standard_True;
    }
    if (V2 - Vmax > aTolBnd)
    {
      V2 = Vmax;
      aDoSegment = Standard_True;
    }
     
  }

  Standard_Boolean IsBz = aSType == GeomAbs_BezierSurface;
  Standard_Boolean IsBs = aSType == GeomAbs_BSplineSurface;

  Handle(Geom_Surface) aTempS = mySurf;
  if (IsBs)
  {
    Handle(Geom_BSplineSurface) aBs = Handle(Geom_BSplineSurface)::DownCast(mySurf->Copy());
    if (aDoSegment)
    {
      aBs->Segment(U1, U2, V1, V2);
    }
    aTempS = aBs;
  }
  else if (IsBz)
  {
    Handle(Geom_BezierSurface) aBz = Handle(Geom_BezierSurface)::DownCast(mySurf->Copy());
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
  if (GeomIsPlanar.IsPlanar()) {
    gp_Pln newPln = GeomIsPlanar.Plan();
    newSurf[isurf] = new Geom_Plane(newPln);
    dd[isurf] = ComputeGap(aTempS, U1, U2, V1, V2, newSurf[isurf]);
    if ( myConvType == GeomConvert_Simplest || 
        (myConvType == GeomConvert_Target && myTarget == GeomAbs_Plane))
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

  Standard_Real diagonal = mySurf->Value(U1, V1).Distance(mySurf->Value((U1 + U2), (V1 + V2) / 2));
  Standard_Real twist = 1000;
  if (toler > diagonal / twist) toler = diagonal / twist;

  isurf = 1; // set cylinder
  Standard_Boolean aCylinderConus = Standard_False;
  Standard_Boolean aToroidSphere = Standard_False;

  //convert middle uiso and viso to canonical representation
  Standard_Real VMid = 0.5*(V1 + V2);
  Standard_Real UMid = 0.5*(U1 + U2);
 // Handle(Geom_Surface) TrSurf = aTempS;

  Handle(Geom_Curve) UIso = aTempS->UIso(UMid);
  Handle(Geom_Curve) VIso = aTempS->VIso(VMid);

  Standard_Real cuf, cul, cvf, cvl, aGap1, aGap2;
  Standard_Boolean aLineIso = Standard_False;
  Handle(Geom_Curve) umidiso = GeomConvert_CurveToAnaCurve::ComputeCurve(UIso, toler, V1, V2, cuf, cul, aGap1,
    GeomConvert_Simplest);
  if (!umidiso.IsNull())
  {
    aLineIso = umidiso->IsKind(STANDARD_TYPE(Geom_Line));
  }
  Handle(Geom_Curve) vmidiso = GeomConvert_CurveToAnaCurve::ComputeCurve(VIso, toler, U1, U2, cvf, cvl, aGap2,
    GeomConvert_Simplest);
  if (!vmidiso.IsNull() && !aLineIso)
  {
    aLineIso = vmidiso->IsKind(STANDARD_TYPE(Geom_Line));
  }
  if (!umidiso.IsNull() && !vmidiso.IsNull())
  {
    //
    Standard_Boolean VCase = Standard_False;

    if (umidiso->IsKind(STANDARD_TYPE(Geom_Circle)) && vmidiso->IsKind(STANDARD_TYPE(Geom_Circle)))
    {
      aToroidSphere = Standard_True;
      if (myConvType == GeomConvert_Target && (myTarget == GeomAbs_Cylinder || myTarget == GeomAbs_Cone))
      {
        isurf = 1;
        myGap = dd[isurf];
        return newSurf[isurf];
      }
      isurf = 3; // set sphere
    }
    else if (umidiso->IsKind(STANDARD_TYPE(Geom_Line)) && vmidiso->IsKind(STANDARD_TYPE(Geom_Circle))) {
      aCylinderConus = Standard_True; VCase = Standard_True;
      if (myConvType == GeomConvert_Target && (myTarget == GeomAbs_Sphere || myTarget == GeomAbs_Torus))
      {
        isurf = 3;
        myGap = dd[isurf];
        return newSurf[isurf];
      }
      isurf = 1;// set cylinder
    }
    else if (umidiso->IsKind(STANDARD_TYPE(Geom_Circle)) && vmidiso->IsKind(STANDARD_TYPE(Geom_Line)))
    {
      aCylinderConus = Standard_True;
      if (myConvType == GeomConvert_Target && (myTarget == GeomAbs_Sphere || myTarget == GeomAbs_Torus))
      {
        isurf = 3;
        myGap = dd[isurf];
        return newSurf[isurf];
      }
      isurf = 1;// set cylinder
    }


    //case of torus-sphere
    if (aToroidSphere) {

      isurf = 3; // Set spherical surface

      Handle(Geom_Circle) Ucircle = Handle(Geom_Circle)::DownCast(umidiso);
      Handle(Geom_Circle) Vcircle = Handle(Geom_Circle)::DownCast(vmidiso);
      // torus
      // try when V isolines is with same radius
      Handle(Geom_Surface) anObject =
        TryTorusSphere(mySurf, Vcircle, Ucircle, V1, V2, U1, U2, toler, Standard_True);
      if (anObject.IsNull()) // try when U isolines is with same radius
        anObject = TryTorusSphere(mySurf, Ucircle, Vcircle, U1, U2, V1, V2, toler, Standard_False);

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
    //case of cone - cylinder
    else if (aCylinderConus) {
      isurf = 1; //set cylindrical  surface
      Handle(Geom_Surface) anObject = TryCylinerCone(aTempS, VCase, umidiso, vmidiso,
        U1, U2, V1, V2, toler);
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
        aCylinderConus = Standard_False;
        myGap = dd[isurf];
      }
    }
  }
  //Additional checking for case of cylinder
  if (!aCylinderConus && !aToroidSphere && aLineIso)
  {
    //Try cylinder using Gauss field
    Standard_Integer aNbU = 7, aNbV = 7;
    Standard_Boolean aLeastSquare = Standard_True;
    Handle(Geom_Surface) anObject = TryCylinderByGaussField(aTempS, U1, U2, V1, V2, toler,
      aNbU, aNbV, aLeastSquare);
    if (!anObject.IsNull())
    {
      isurf = 1;
      newSurf[isurf] = anObject;
    }
  }

  //
  //---------------------------------------------------------------------
  //                 verification
  //---------------------------------------------------------------------
  Standard_Integer imin = -1;
  Standard_Real aDmin = RealLast();
  for (isurf = 0; isurf < 5; ++isurf)
  {
    if (newSurf[isurf].IsNull())
      continue;
    dd[isurf] = ComputeGap(aTempS, U1, U2, V1, V2, newSurf[isurf], toler);
    if (dd[isurf] <= toler)
    {
      if (myConvType == GeomConvert_Simplest || (myConvType == GeomConvert_Target && myTarget == aSTypes[isurf]))
      {
        myGap = dd[isurf];
        return newSurf[isurf];
      }
      else if (myConvType == GeomConvert_MinGap)
      {
        if (dd[isurf] < aDmin)
        {
          aDmin = dd[isurf];
          imin = isurf;
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

//=======================================================================
//function : IsSame
//purpose  : 
//=======================================================================

Standard_Boolean GeomConvert_SurfToAnaSurf::IsSame(const Handle(Geom_Surface)& S1,
                                             const Handle(Geom_Surface)& S2,
                                             const Standard_Real tol)
{
  // only elementary surfaces are processed
  if(!S1->IsKind(STANDARD_TYPE(Geom_ElementarySurface)) ||
     !S2->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    return Standard_False;
  
  Handle(GeomAdaptor_Surface) anAdaptor1 = new GeomAdaptor_Surface(S1);
  Handle(GeomAdaptor_Surface) anAdaptor2 = new GeomAdaptor_Surface(S2);

  GeomAbs_SurfaceType aST1 = anAdaptor1->GetType();
  GeomAbs_SurfaceType aST2 = anAdaptor2->GetType();

  if (aST1 != aST2)
  {
    return Standard_False;
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
    return Standard_False;
  
  IntAna_ResultType aTypeRes = interii.TypeInter();

  return aTypeRes == IntAna_Same;

}


//=======================================================================
//function : IsCanonical
//purpose  : 
//=======================================================================

Standard_Boolean GeomConvert_SurfToAnaSurf::IsCanonical(const Handle(Geom_Surface)& S)
{
  if(S.IsNull())
    return Standard_False;
  
  if (   S->IsKind(STANDARD_TYPE(Geom_Plane))
      || S->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))
      || S->IsKind(STANDARD_TYPE(Geom_ConicalSurface))
      || S->IsKind(STANDARD_TYPE(Geom_SphericalSurface)) 
      || S->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
    return Standard_True;
  
  return Standard_False;
}

