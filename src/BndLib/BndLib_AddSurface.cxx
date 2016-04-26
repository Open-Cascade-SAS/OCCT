// Created on: 1995-07-24
// Created by: Modelistation
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

//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503

#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <BndLib.hxx>
#include <BndLib_AddSurface.hxx>
#include <BSplCLib.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Lin.hxx>
#include <Precision.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_PSO.hxx>
#include <math_Matrix.hxx>
#include <math_Powell.hxx>
//
static Standard_Integer NbUSamples(const Adaptor3d_Surface& S, 
                                   const Standard_Real Umin,
                                   const Standard_Real Umax);
//
static Standard_Integer NbVSamples(const Adaptor3d_Surface& S, 
                                   const Standard_Real Vmin,
                                   const Standard_Real Vmax);
//
static Standard_Real  AdjustExtr(const Adaptor3d_Surface& S, 
                                 const Standard_Real UMin,
			                           const Standard_Real UMax,
			                           const Standard_Real VMin,
			                           const Standard_Real VMax,
                                 const Standard_Real Extr0,
                                 const Standard_Integer CoordIndx,                                 
                                 const Standard_Real Tol, 
                                 const Standard_Boolean IsMin);

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BndLib_AddSurface::Add(const Adaptor3d_Surface& S,
			    const Standard_Real Tol,
			    Bnd_Box& B ) 
{

  BndLib_AddSurface::Add(S,
			 S.FirstUParameter(),
			 S.LastUParameter (),
			 S.FirstVParameter(),
			 S.LastVParameter (),Tol,B);
}
//=======================================================================
//function : NbUSamples
//purpose  : 
//=======================================================================

static Standard_Integer NbUSamples(const Adaptor3d_Surface& S) 
{
  Standard_Integer N;
  GeomAbs_SurfaceType Type = S.GetType();
  switch (Type) {
  case GeomAbs_BezierSurface: 
    {
      N = 2*S.NbUPoles();
      break;
    }
  case GeomAbs_BSplineSurface: 
    {
      const Handle(Geom_BSplineSurface)& BS = S.BSpline();
      N = 2*(BS->UDegree() + 1)*(BS->NbUKnots() -1);
      break;
    }
  default:
    N = 33;
  }
  return Min (50,N);
}

//=======================================================================
//function : NbVSamples
//purpose  : 
//=======================================================================

static Standard_Integer NbVSamples(const Adaptor3d_Surface& S) 
{
  Standard_Integer N;
  GeomAbs_SurfaceType Type = S.GetType();
  switch (Type) {
  case GeomAbs_BezierSurface:
    {
      N = 2*S.NbVPoles();
      break;
    }
  case GeomAbs_BSplineSurface:
    {
      const Handle(Geom_BSplineSurface)& BS = S.BSpline();
      N = 2*(BS->VDegree() + 1)*(BS->NbVKnots() - 1) ;
      break;
    }
  default:
    N = 33;
  }
  return Min(50,N);
}

//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503 Begin
static gp_Pnt BaryCenter(const gp_Pln        &aPlane,
			 const Standard_Real  aUMin,
			 const Standard_Real  aUMax,
			 const Standard_Real  aVMin,
			 const Standard_Real  aVMax)
{
  Standard_Real aU, aV;
  Standard_Boolean isU1Inf = Precision::IsInfinite(aUMin);
  Standard_Boolean isU2Inf = Precision::IsInfinite(aUMax);
  Standard_Boolean isV1Inf = Precision::IsInfinite(aVMin);
  Standard_Boolean isV2Inf = Precision::IsInfinite(aVMax);

  if (isU1Inf && isU2Inf)
    aU = 0;
  else if (isU1Inf)
    aU = aUMax - 10.;
  else if (isU2Inf)
    aU = aUMin + 10.;
  else
    aU = (aUMin + aUMax)/2.;
    
  if (isV1Inf && isV2Inf)
    aV = 0;
  else if (isV1Inf)
    aV = aVMax - 10.;
  else if (isV2Inf)
    aV = aVMin + 10.;
  else
    aV = (aVMin + aVMax)/2.;

  gp_Pnt aCenter = ElSLib::Value(aU, aV, aPlane);

  return aCenter;
}

static void TreatInfinitePlane(const gp_Pln        &aPlane,
			       const Standard_Real  aUMin,
			       const Standard_Real  aUMax,
			       const Standard_Real  aVMin,
			       const Standard_Real  aVMax,
			       const Standard_Real  aTol,
			             Bnd_Box       &aB)
{
  // Get 3 coordinate axes of the plane.
  const gp_Dir        &aNorm        = aPlane.Axis().Direction();
  const Standard_Real  anAngularTol = RealEpsilon();

  // Get location of the plane as its barycenter
  gp_Pnt aLocation = BaryCenter(aPlane, aUMin, aUMax, aVMin, aVMax);

  if (aNorm.IsParallel(gp::DX(), anAngularTol)) {
    aB.Add(aLocation);
    aB.OpenYmin();
    aB.OpenYmax();
    aB.OpenZmin();
    aB.OpenZmax();
  } else if (aNorm.IsParallel(gp::DY(), anAngularTol)) {
    aB.Add(aLocation);
    aB.OpenXmin();
    aB.OpenXmax();
    aB.OpenZmin();
    aB.OpenZmax();
  } else if (aNorm.IsParallel(gp::DZ(), anAngularTol)) {
    aB.Add(aLocation);
    aB.OpenXmin();
    aB.OpenXmax();
    aB.OpenYmin();
    aB.OpenYmax();
  } else {
    aB.SetWhole();
    return;
  }

  aB.Enlarge(aTol);
}

// Compute start and finish indexes used in convex hull.
// theMinIdx - minimum poles index, that can be used.
// theMaxIdx - maximum poles index, that can be used.
// theShiftCoeff - shift between flatknots array and poles array.
// This vaule should be equal to 1 in case of non periodic BSpline,
// and (degree + 1) - mults(the lowest index).
void ComputePolesIndexes(const TColStd_Array1OfReal &theFlatKnots,
                         const Standard_Integer theDegree,
                         const Standard_Real theMin,
                         const Standard_Real theMax,
                         const Standard_Integer theMinIdx,
                         const Standard_Integer theMaxIdx,
                         const Standard_Integer theShiftCoeff,
                         Standard_Integer &theOutMinIdx,
                         Standard_Integer &theOutMaxIdx)
{
  // Set initial values for the result indexes to handle situation when requested parameter space
  // is slightly greater than B-spline parameter space.
  theOutMinIdx = theFlatKnots.Lower();
  theOutMaxIdx = theFlatKnots.Upper();

  // Compute first and last used flat knots.
  for(Standard_Integer aKnotIdx = theFlatKnots.Lower();
      aKnotIdx < theFlatKnots.Upper();
      aKnotIdx++)
  {
    if (theFlatKnots(aKnotIdx) <= theMin)
      theOutMinIdx = aKnotIdx;

    if (theFlatKnots(theFlatKnots.Upper() - aKnotIdx + theFlatKnots.Lower()) >= theMax)
      theOutMaxIdx = theFlatKnots.Upper() - aKnotIdx + theFlatKnots.Lower();
  }

  theOutMinIdx = Max(theOutMinIdx - 2 * theDegree + 2 - theShiftCoeff, theMinIdx);
  theOutMaxIdx = Min(theOutMaxIdx - 2 + theDegree + 1 - theShiftCoeff, theMaxIdx);
}

//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503 End
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BndLib_AddSurface::Add(const Adaptor3d_Surface& S,
			    const Standard_Real UMin,
			    const Standard_Real UMax,
			    const Standard_Real VMin,
			    const Standard_Real VMax,
			    const Standard_Real Tol,
			    Bnd_Box& B ) 
{
  GeomAbs_SurfaceType Type = S.GetType(); // skv OCC6503

  if (Precision::IsInfinite(VMin) ||
      Precision::IsInfinite(VMax) ||
      Precision::IsInfinite(UMin) ||
      Precision::IsInfinite(UMax)   ) {
//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503 Begin
//     B.SetWhole();
//     return;
    switch (Type) {
    case GeomAbs_Plane: 
      {
	TreatInfinitePlane(S.Plane(), UMin, UMax, VMin, VMax, Tol, B);
	return;
      }
    default: 
      {
	B.SetWhole();
	return;
      }
    }
//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503 End
  }

//   GeomAbs_SurfaceType Type = S.GetType(); // skv OCC6503

  switch (Type) {

  case GeomAbs_Plane: 
    {
      gp_Pln Plan = S.Plane();
      B.Add(ElSLib::Value(UMin,VMin,Plan)); 
      B.Add(ElSLib::Value(UMin,VMax,Plan)); 
      B.Add(ElSLib::Value(UMax,VMin,Plan)); 
      B.Add(ElSLib::Value(UMax,VMax,Plan)); 
      B.Enlarge(Tol);
      break;
    }
  case GeomAbs_Cylinder: 
    {
      BndLib::Add(S.Cylinder(),UMin,UMax,VMin,VMax,Tol,B);
      break;
    }
  case GeomAbs_Cone: 
    {
      BndLib::Add(S.Cone(),UMin,UMax,VMin,VMax,Tol,B);
      break;
    }
  case GeomAbs_Torus: 
    {
      BndLib::Add(S.Torus(),UMin,UMax,VMin,VMax,Tol,B);
      break;
    }
  case GeomAbs_Sphere: 
    {
      if (Abs(UMin) < Precision::Angular() &&
          Abs(UMax - 2.*M_PI) < Precision::Angular() &&
          Abs(VMin + M_PI/2.) < Precision::Angular() &&
          Abs(VMax - M_PI/2.) < Precision::Angular()) // a whole sphere
        BndLib::Add(S.Sphere(),Tol,B);
      else
        BndLib::Add(S.Sphere(),UMin,UMax,VMin,VMax,Tol,B);
      break;
    }
  case GeomAbs_OffsetSurface: 
    {
      Handle(Adaptor3d_HSurface) HS = S.BasisSurface();
      Add (HS->Surface(),UMin,UMax,VMin,VMax,Tol,B);
      B.Enlarge(S.OffsetValue());
      B.Enlarge(Tol);
      break;
    } 
  case GeomAbs_BezierSurface:
  case GeomAbs_BSplineSurface: 
    {
      Standard_Boolean isUseConvexHullAlgorithm = Standard_True;
      Standard_Real PTol = Precision::Parametric(Precision::Confusion());
      // Borders of underlying geometry.
      Standard_Real anUMinParam = UMin, anUMaxParam = UMax,// BSpline case.
                     aVMinParam = VMin,  aVMaxParam = VMax;
      if (Type == GeomAbs_BezierSurface)
      {
        // Bezier surface:
        // All of poles used for any parameter,
        // thats why in case of trimmed parameters handled by grid algorithm.

        if (Abs(UMin-S.FirstUParameter()) > PTol ||
            Abs(VMin-S.FirstVParameter()) > PTol ||
            Abs(UMax-S.LastUParameter ()) > PTol ||
            Abs(VMax-S.LastVParameter ()) > PTol )
        {
          // Borders not equal to topology borders.
          isUseConvexHullAlgorithm = Standard_False;
        }
      }
      else
      {
        // BSpline:
        // If Umin, Vmin, Umax, Vmax lies inside geometry bounds then:
        // use convex hull algorithm,
        // if Umin, VMin, Umax, Vmax lies outside then:
        // use grid algorithm on analytic continuation (default case).
        S.BSpline()->Bounds(anUMinParam, anUMaxParam, aVMinParam, aVMaxParam);

        if ( (UMin - anUMinParam) < -PTol ||
             (VMin -  aVMinParam) < -PTol ||
             (UMax - anUMaxParam) >  PTol ||
             (VMax -  aVMaxParam) >  PTol )
        {
          // Out of geometry borders.
          isUseConvexHullAlgorithm = Standard_False;
        }
      }

      if (isUseConvexHullAlgorithm)
      {
          TColgp_Array2OfPnt Tp(1,S.NbUPoles(),1,S.NbVPoles());
          Standard_Integer UMinIdx = 0, UMaxIdx = 0;
          Standard_Integer VMinIdx = 0, VMaxIdx = 0;
          if (Type == GeomAbs_BezierSurface)
          {
            S.Bezier()->Poles(Tp);

            UMinIdx = Tp.LowerRow();
            UMaxIdx = Tp.UpperRow();
            VMinIdx = Tp.LowerCol();
            VMaxIdx = Tp.UpperCol();
          }
          else
          {
            S.BSpline()->Poles(Tp);

            UMinIdx = Tp.LowerRow();
            UMaxIdx = Tp.UpperRow();
            VMinIdx = Tp.LowerCol();
            VMaxIdx = Tp.UpperCol();

            if (UMin > anUMinParam ||
                UMax < anUMaxParam)
            {
              Standard_Integer anUFlatKnotsCount = S.BSpline()->NbUPoles() + S.BSpline()->UDegree() + 1;
              Standard_Integer aShift = 1;

              if (S.BSpline()->IsUPeriodic())
              {
                TColStd_Array1OfInteger aMults(1, S.BSpline()->NbUKnots());
                S.BSpline()->UMultiplicities(aMults);
                anUFlatKnotsCount = BSplCLib::KnotSequenceLength(aMults, S.BSpline()->UDegree(), Standard_True);

                aShift = S.BSpline()->UDegree() + 1 - S.BSpline()->UMultiplicity(1);
              }

              TColStd_Array1OfReal anUFlatKnots(1, anUFlatKnotsCount);
              S.BSpline()->UKnotSequence(anUFlatKnots);

              ComputePolesIndexes(anUFlatKnots,
                                  S.BSpline()->UDegree(),
                                  UMin, UMax,
                                  UMinIdx, UMaxIdx,  // Min and Max Indexes
                                  aShift,
                                  UMinIdx, UMaxIdx); // the Output indexes
            }

            if (VMin > aVMinParam ||
                VMax < aVMaxParam)
            {
              Standard_Integer anVFlatKnotsCount = S.BSpline()->NbVPoles() + S.BSpline()->VDegree() + 1;
              Standard_Integer aShift = 1;

              if (S.BSpline()->IsVPeriodic())
              {
                TColStd_Array1OfInteger aMults(1, S.BSpline()->NbVKnots());
                S.BSpline()->VMultiplicities(aMults);
                anVFlatKnotsCount = BSplCLib::KnotSequenceLength(aMults, S.BSpline()->VDegree(), Standard_True);

                aShift = S.BSpline()->VDegree() + 1 - S.BSpline()->VMultiplicity(1);
              }

              TColStd_Array1OfReal anVFlatKnots(1, anVFlatKnotsCount);
              S.BSpline()->VKnotSequence(anVFlatKnots);

              ComputePolesIndexes(anVFlatKnots,
                                  S.BSpline()->VDegree(),
                                  VMin, VMax,
                                  VMinIdx, VMaxIdx,  // Min and Max Indexes
                                  aShift,
                                  VMinIdx, VMaxIdx); // the Output indexes
            }

          }

          // Use poles to build convex hull.
          for (Standard_Integer i = UMinIdx; i <= UMaxIdx; i++)
          {
            for (Standard_Integer j = VMinIdx; j <= VMaxIdx; j++)
            {
              B.Add(Tp(i,j));
            }
          }

          B.Enlarge(Tol);
          break;
      }
    }
  default: 
    {
      Standard_Integer Nu = NbUSamples(S);
      Standard_Integer Nv = NbVSamples(S);
      gp_Pnt P;
      for (Standard_Integer i =1 ;i<=Nu;i++){
        Standard_Real U = UMin + ((UMax-UMin)*(i-1)/(Nu-1));
        for (Standard_Integer j=1 ;j<=Nv;j++){
          Standard_Real V = VMin + ((VMax-VMin)*(j-1)/(Nv-1));
          S.D0(U,V,P);
          B.Add(P);
        }
      }
      B.Enlarge(Tol);
    }
  }
}
//----- Methods for AddOptimal ---------------------------------------

//=======================================================================
//function : AddOptimal
//purpose  : 
//=======================================================================
void BndLib_AddSurface::AddOptimal(const Adaptor3d_Surface& S,
			                             const Standard_Real Tol,
			                             Bnd_Box& B ) 
{

  BndLib_AddSurface::AddOptimal(S,
			                          S.FirstUParameter(),
			                          S.LastUParameter (),
			                          S.FirstVParameter(),
			                          S.LastVParameter (),Tol,B);
}
//=======================================================================
//function : AddOptimal
//purpose  : 
//=======================================================================

void BndLib_AddSurface::AddOptimal(const Adaptor3d_Surface& S,
			    const Standard_Real UMin,
			    const Standard_Real UMax,
			    const Standard_Real VMin,
			    const Standard_Real VMax,
			    const Standard_Real Tol,
			    Bnd_Box& B ) 
{
  GeomAbs_SurfaceType Type = S.GetType(); 

  if (Precision::IsInfinite(VMin) ||
      Precision::IsInfinite(VMax) ||
      Precision::IsInfinite(UMin) ||
      Precision::IsInfinite(UMax)   ) {
    switch (Type) {
      case GeomAbs_Plane: 
      {
        TreatInfinitePlane(S.Plane(), UMin, UMax, VMin, VMax, Tol, B);
        return;
      }
      default: 
      {
	      B.SetWhole();
	      return;
      }
    }
  }

  switch (Type) {
    
    case GeomAbs_Plane: 
    {
      gp_Pln Plan = S.Plane();
      B.Add(ElSLib::Value(UMin,VMin,Plan)); 
      B.Add(ElSLib::Value(UMin,VMax,Plan)); 
      B.Add(ElSLib::Value(UMax,VMin,Plan)); 
      B.Add(ElSLib::Value(UMax,VMax,Plan)); 
      B.Enlarge(Tol);
      break;
    }
    case GeomAbs_Cylinder: 
    {
      BndLib::Add(S.Cylinder(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    }
    case GeomAbs_Cone: 
    {
      BndLib::Add(S.Cone(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    }
    case GeomAbs_Sphere: 
    {
      BndLib::Add(S.Sphere(), UMin, UMax, VMin, VMax, Tol, B); 
      break;
    }
    default: 
    {
      AddGenSurf(S, UMin, UMax, VMin, VMax, Tol, B);
    }
  }
}
//=======================================================================
//function : AddGenSurf
//purpose  : 
//=======================================================================
void BndLib_AddSurface::AddGenSurf(const Adaptor3d_Surface& S, 
                                   const Standard_Real UMin,
                                   const Standard_Real UMax,
                                   const Standard_Real VMin,
                                   const Standard_Real VMax,
                                   const Standard_Real Tol,
                                   Bnd_Box& B)
{
  Standard_Integer Nu = NbUSamples(S, UMin, UMax);
  Standard_Integer Nv = NbVSamples(S, VMin, VMax);
  //
  Standard_Real CoordMin[3] = {RealLast(), RealLast(), RealLast()}; 
  Standard_Real CoordMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  Standard_Real DeflMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  //
  //
  Standard_Real du = (UMax-UMin)/(Nu-1), du2 = du / 2.;
  Standard_Real dv = (VMax-VMin)/(Nv-1), dv2 = dv / 2.;
  NCollection_Array2<gp_XYZ> aPnts(1, Nu, 1, Nv);
  Standard_Real u, v;
  Standard_Integer i, j, k;
  gp_Pnt P;
  for (i = 1, u = UMin; i <= Nu; i++, u += du){
    for (j = 1, v = VMin;j <= Nv; j++, v += dv){
      S.D0(u,v,P);
      aPnts(i, j) = P.XYZ();
      //
      for(k = 0; k < 3; ++k)
      {
        if(CoordMin[k] > P.Coord(k+1))
        {
          CoordMin[k] = P.Coord(k+1);
        }
        if(CoordMax[k] < P.Coord(k+1))
        {
          CoordMax[k] = P.Coord(k+1);
        }
      }
      //
      if(i > 1)
      {
        gp_XYZ aPm = 0.5 * (aPnts(i-1,j) + aPnts(i, j));
        S.D0(u - du2, v, P);
        gp_XYZ aD = (P.XYZ() - aPm);
        for(k = 0; k < 3; ++k)
        {
          if(CoordMin[k] > P.Coord(k+1))
          {
            CoordMin[k] = P.Coord(k+1);
          }
          if(CoordMax[k] < P.Coord(k+1))
          {
            CoordMax[k] = P.Coord(k+1);
          }
          Standard_Real d = Abs(aD.Coord(k+1));
          if(DeflMax[k] < d)
          {
            DeflMax[k] = d;
          }
        }
      }
      if(j > 1)
      {
        gp_XYZ aPm = 0.5 * (aPnts(i,j-1) + aPnts(i, j));
        S.D0(u , v - dv2, P);
        gp_XYZ aD = (P.XYZ() - aPm);
        for(k = 0; k < 3; ++k)
        {
          if(CoordMin[k] > P.Coord(k+1))
          {
            CoordMin[k] = P.Coord(k+1);
          }
          if(CoordMax[k] < P.Coord(k+1))
          {
            CoordMax[k] = P.Coord(k+1);
          }
          Standard_Real d = Abs(aD.Coord(k+1));
          if(DeflMax[k] < d)
          {
            DeflMax[k] = d;
          }
        }
      }
    }
  }
  //
  //Adjusting minmax 
  Standard_Real eps = Max(Tol, Precision::Confusion()); 
  for(k = 0; k < 3; ++k)
  {
    Standard_Real d = DeflMax[k];
    if(d <= eps)
    {
      continue;
    }

    Standard_Real CMin = CoordMin[k];
    Standard_Real CMax = CoordMax[k];
    for(i = 1; i <= Nu; ++i)
    {
      for(j = 1; j <= Nv; ++j)
      {
        if(aPnts(i,j).Coord(k+1) - CMin < d)
        {
          Standard_Real umin, umax, vmin, vmax;
          umin = UMin + Max(0, i-2) * du;
          umax = UMin + Min(Nu-1, i) * du;
          vmin = VMin + Max(0, j-2) * dv;
          vmax = VMin + Min(Nv-1, j) * dv;
          Standard_Real cmin = AdjustExtr(S, umin, umax, vmin, vmax,
                                          CMin, k + 1, eps, Standard_True);
          if(cmin < CMin)
          {
            CMin = cmin;
          }
        }
        else if(CMax - aPnts(i,j).Coord(k+1) < d)
        {
          Standard_Real umin, umax, vmin, vmax;
          umin = UMin + Max(0, i-2) * du;
          umax = UMin + Min(Nu-1, i) * du;
          vmin = VMin + Max(0, j-2) * dv;
          vmax = VMin + Min(Nv-1, j) * dv;
          Standard_Real cmax = AdjustExtr(S, umin, umax, vmin, vmax,
                                          CMax, k + 1, eps, Standard_False);
          if(cmax > CMax)
          {
            CMax = cmax;
          }
        }
      }
    }
    CoordMin[k] = CMin;
    CoordMax[k] = CMax;

  }

  B.Add(gp_Pnt(CoordMin[0], CoordMin[1], CoordMin[2]));
  B.Add(gp_Pnt(CoordMax[0], CoordMax[1], CoordMax[2]));
  B.Enlarge(eps);
}
//

//
class SurfMaxMinCoord : public math_MultipleVarFunction
{
public:
  SurfMaxMinCoord(const Adaptor3d_Surface& theSurf, 
              const Standard_Real UMin,
              const Standard_Real UMax,
              const Standard_Real VMin,
              const Standard_Real VMax,
              const Standard_Integer CoordIndx,                                 
              const Standard_Real Sign)
: mySurf(theSurf),
  myUMin(UMin),
  myUMax(UMax),
  myVMin(VMin),
  myVMax(VMax),
  myCoordIndx(CoordIndx),
  mySign(Sign)
  {
    math_Vector X(1,2);
    X(1) = UMin;
    X(2) = (VMin + VMax) / 2.;
    Standard_Real F1, F2;
    Value(X, F1);
    X(1) = UMax;
    Value(X, F2);
    Standard_Real DU = Abs((F2 - F1) / (UMax - UMin));
    X(1) = (UMin + UMax) / 2.;
    X(2) = VMin;
    Value(X, F1);
    X(2) = VMax;
    Value(X, F2);
    Standard_Real DV = Abs((F2 - F1) / (VMax - VMin));
    myPenalty = 10. * Max(DU, DV);
    myPenalty = Max(myPenalty, 1.);
  }

  Standard_Boolean Value (const math_Vector& X,
                                  Standard_Real& F)
  {
    if (CheckInputData(X))
    {
      gp_Pnt aP = mySurf.Value(X(1), X(2));
      F = mySign * aP.Coord(myCoordIndx);
    }
    else
    {
      Standard_Real UPen = 0., VPen = 0., u0, v0;
      if(X(1) < myUMin)
      {
        UPen = myPenalty * (myUMin - X(1));
        u0 = myUMin;
      }
      else if(X(1) > myUMax)
      {
        UPen = myPenalty * (X(1) - myUMax);
        u0 = myUMax;
      }
      else
      {
        u0 = X(1);
      }
      //
      if(X(2) < myVMin)
      {
        VPen = myPenalty * (myVMin - X(2));
        v0 = myVMin;
      }
      else if(X(2) > myVMax)
      {
        VPen = myPenalty * (X(2) - myVMax);
        v0 = myVMax;
      }
      else
      {
        v0 = X(2);
      }
      //
      gp_Pnt aP = mySurf.Value(u0, v0);
      F = mySign * aP.Coord(myCoordIndx) + UPen + VPen;
    }

    return Standard_True;
  }

  

  Standard_Integer NbVariables() const
  {
    return 2;
  }

private:
  SurfMaxMinCoord & operator = (const SurfMaxMinCoord & theOther);

  Standard_Boolean CheckInputData(const math_Vector theParams)
  {
    if (theParams(1) < myUMin || 
        theParams(1) > myUMax || 
        theParams(2) < myVMin || 
        theParams(2) > myVMax)
      return Standard_False;
    return Standard_True;
  }

  const Adaptor3d_Surface& mySurf;
  Standard_Real myUMin;
  Standard_Real myUMax;
  Standard_Real myVMin;
  Standard_Real myVMax;
  Standard_Integer myCoordIndx;
  Standard_Real mySign;
  Standard_Real myPenalty;
};

//=======================================================================
//function : AdjustExtr
//purpose  : 
//=======================================================================

Standard_Real AdjustExtr(const Adaptor3d_Surface& S, 
                         const Standard_Real UMin,
                         const Standard_Real UMax,
                         const Standard_Real VMin,
                         const Standard_Real VMax,
                         const Standard_Real Extr0,
                         const Standard_Integer CoordIndx,                                 
                         const Standard_Real Tol, 
                         const Standard_Boolean IsMin)
{
  Standard_Real aSign = IsMin ? 1.:-1.;
  Standard_Real extr = aSign * Extr0;
  Standard_Real relTol = 2.*Tol;
  if(Abs(extr) > Tol)
  {
    relTol /= Abs(extr);
  }
  Standard_Real Du = (S.LastUParameter() - S.FirstUParameter());
  Standard_Real Dv = (S.LastVParameter() - S.FirstVParameter());
  //
  math_Vector aT(1,2);
  math_Vector aLowBorder(1,2);
  math_Vector aUppBorder(1,2);
  math_Vector aSteps(1,2);
  aLowBorder(1) = UMin;
  aUppBorder(1) = UMax;
  aLowBorder(2) = VMin;
  aUppBorder(2) = VMax;

  Standard_Integer aNbU = Max(8, RealToInt(32 * (UMax - UMin) / Du));
  Standard_Integer aNbV = Max(8, RealToInt(32 * (VMax - VMin) / Dv));
  Standard_Integer aNbParticles = aNbU * aNbV;
  Standard_Real aMaxUStep = (UMax - UMin) / (aNbU + 1);
  aSteps(1) = Min(0.1 * Du, aMaxUStep);  
  Standard_Real aMaxVStep = (VMax - VMin) / (aNbV + 1);
  aSteps(2) = Min(0.1 * Dv, aMaxVStep);
 
  SurfMaxMinCoord aFunc(S, UMin, UMax, VMin, VMax, CoordIndx, aSign);
  math_PSO aFinder(&aFunc, aLowBorder, aUppBorder, aSteps, aNbParticles); 
  aFinder.Perform(aSteps, extr, aT);
  
  //Refinement of extremal value
  math_Matrix aDir(1, 2, 1, 2, 0.0);
  aDir(1, 1) = 1.;
  aDir(2, 1) = 0.;
  aDir(1, 2) = 0.;
  aDir(2, 2) = 1.;

  Standard_Integer aNbIter = 200;
  math_Powell powell(aFunc, relTol, aNbIter, Tol);
  powell.Perform(aFunc, aT, aDir);

  if (powell.IsDone())
  {
    powell.Location(aT);
    extr = powell.Minimum();
  }

  return aSign * extr;
}

//=======================================================================
//function : NbUSamples
//purpose  : 
//=======================================================================

Standard_Integer NbUSamples(const Adaptor3d_Surface& S, 
                            const Standard_Real Umin,
                            const Standard_Real Umax) 
{
  Standard_Integer N;
  GeomAbs_SurfaceType Type = S.GetType();
  switch (Type) {
  case GeomAbs_BezierSurface: 
    {
      N = 2*S.NbUPoles();
      //By default parametric range of Bezier surf is [0, 1] [0, 1]
      Standard_Real du = Umax - Umin;
      if(du < .9)
      {
        N = RealToInt(du*N) + 1;
        N = Max(N, 5);
      }
      break;
    }
  case GeomAbs_BSplineSurface: 
    {
      const Handle(Geom_BSplineSurface)& BS = S.BSpline();
      N = 2*(BS->UDegree() + 1)*(BS->NbUKnots() -1);
      Standard_Real umin, umax, vmin, vmax;
      BS->Bounds(umin, umax, vmin, vmax);
      Standard_Real du = (Umax - Umin) / (umax - umin);
      if(du < .9)
      {
        N = RealToInt(du*N) + 1;
        N = Max(N, 5);
      }
      break;
    }
  default:
    N = 33;
  }
  return Min (50,N);
}

//=======================================================================
//function : NbVSamples
//purpose  : 
//=======================================================================

Standard_Integer NbVSamples(const Adaptor3d_Surface& S, 
                            const Standard_Real Vmin,
                            const Standard_Real Vmax) 
{
  Standard_Integer N;
  GeomAbs_SurfaceType Type = S.GetType();
  switch (Type) {
  case GeomAbs_BezierSurface:
    {
      N = 2*S.NbVPoles();
      //By default parametric range of Bezier surf is [0, 1] [0, 1]
      Standard_Real dv = Vmax - Vmin;
      if(dv < .9)
      {
        N = RealToInt(dv*N) + 1;
        N = Max(N, 5);
      }
      break;
    }
  case GeomAbs_BSplineSurface:
    {
      const Handle(Geom_BSplineSurface)& BS = S.BSpline();
      N = 2*(BS->VDegree() + 1)*(BS->NbVKnots() - 1) ;
     Standard_Real umin, umax, vmin, vmax;
      BS->Bounds(umin, umax, vmin, vmax);
      Standard_Real dv = (Vmax - Vmin) / (vmax - vmin);
      if(dv < .9)
      {
        N = RealToInt(dv*N) + 1;
        N = Max(N, 5);
      }
      break;
    }
  default:
    N = 33;
  }
  return Min(50,N);
}

