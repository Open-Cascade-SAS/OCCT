// Copyright (c) 1996-1999 Matra Datavision
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


#include <Adaptor3d_Curve.hxx>
#include <Bnd_Box.hxx>
#include <BndLib.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <ElCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_Function.hxx>
#include <math_PSO.hxx>
#include <math_BrentMinimum.hxx>
//
static Standard_Integer NbSamples(const Adaptor3d_Curve& C, 
                                   const Standard_Real Umin,
                                   const Standard_Real Umax);

static Standard_Real  AdjustExtr(const Adaptor3d_Curve& C, 
                                 const Standard_Real UMin,
			                           const Standard_Real UMax,
                                 const Standard_Real Extr0,
                                 const Standard_Integer CoordIndx,                                 
                                 const Standard_Real Tol, 
                                 const Standard_Boolean IsMin);


//=======================================================================
//function : reduceSplineBox
//purpose  : This method intended to reduce box in case of 
//           bezier and bspline curve.
//=======================================================================
static void reduceSplineBox(const Adaptor3d_Curve& theCurve,
                            const Bnd_Box& theOrigBox,
                            Bnd_Box & theReducedBox)
{
  // Guaranteed bounding box based on poles of bspline.
  Bnd_Box aPolesBox;
  Standard_Real aPolesXMin, aPolesYMin, aPolesZMin,
                aPolesXMax, aPolesYMax, aPolesZMax;

  if (theCurve.GetType() == GeomAbs_BSplineCurve)
  {
    Handle(Geom_BSplineCurve) aC = theCurve.BSpline();
    const TColgp_Array1OfPnt& aPoles     = aC->Poles();

    for(Standard_Integer anIdx  = aPoles.Lower();
        anIdx <= aPoles.Upper();
        ++anIdx)
    {
      aPolesBox.Add(aPoles.Value(anIdx));
    }
  }
  if (theCurve.GetType() == GeomAbs_BezierCurve)
  {
    Handle(Geom_BezierCurve) aC = theCurve.Bezier();
    const TColgp_Array1OfPnt& aPoles     = aC->Poles();

    for(Standard_Integer anIdx  = aPoles.Lower();
        anIdx <= aPoles.Upper();
        ++anIdx)
    {
      aPolesBox.Add(aPoles.Value(anIdx));
    }
  }

  aPolesBox.Get(aPolesXMin, aPolesYMin, aPolesZMin,
                aPolesXMax, aPolesYMax, aPolesZMax);

  Standard_Real x, y, z, X, Y, Z;
  theOrigBox.Get(x, y, z, X, Y, Z);

  // Left bound.
  if (aPolesXMin > x)
    x = aPolesXMin;
  if (aPolesYMin > y)
    y = aPolesYMin;
  if (aPolesZMin > z)
    z = aPolesZMin;

  // Right bound.
  if (aPolesXMax < X)
    X = aPolesXMax;
  if (aPolesYMax < Y)
    Y = aPolesYMax;
  if (aPolesZMax < Z)
    Z = aPolesZMax;

  theReducedBox.Update(x, y, z, X, Y, Z);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BndLib_Add3dCurve::Add( const Adaptor3d_Curve& C,
			   const Standard_Real Tol,
			         Bnd_Box&      B )
{
  BndLib_Add3dCurve::Add(C,
			 C.FirstParameter(),
			 C.LastParameter (),
			 Tol,B);
}

//OCC566(apo)->
static Standard_Real FillBox(Bnd_Box& B, const Adaptor3d_Curve& C, 
			     const Standard_Real first, const Standard_Real last, 
			     const Standard_Integer N)
{
  gp_Pnt P1, P2, P3;
  C.D0(first,P1);  B.Add(P1);
  Standard_Real p = first, dp = last-first, tol= 0.;
  if(Abs(dp) > Precision::PConfusion()){
    Standard_Integer i;
    dp /= 2*N; 
    for(i = 1; i <= N; i++){
      p += dp;  C.D0(p,P2);  B.Add(P2);
      p += dp;  C.D0(p,P3);  B.Add(P3);
      gp_Pnt Pc((P1.XYZ()+P3.XYZ())/2.0);
      tol = Max(tol,Pc.Distance(P2));
      P1 = P3;
    }
  }else{
    C.D0(first,P1);  B.Add(P1);
    C.D0(last,P3);  B.Add(P3);
    tol = 0.;
  }
  return tol;
}
//<-OCC566(apo)
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BndLib_Add3dCurve::Add( const Adaptor3d_Curve& C,
			   const Standard_Real U1,
			   const Standard_Real U2,
			   const Standard_Real Tol,
			         Bnd_Box&      B )
{
  static Standard_Real weakness = 1.5;  //OCC566(apo)
  Standard_Real tol = 0.0;
  switch (C.GetType()) {

  case GeomAbs_Line: 
    {
      BndLib::Add(C.Line(),U1,U2,Tol,B);
      break;
    }
  case GeomAbs_Circle: 
    {
      BndLib::Add(C.Circle(),U1,U2,Tol,B);
      break;
    }
  case GeomAbs_Ellipse: 
    {
      BndLib::Add(C.Ellipse(),U1,U2,Tol,B);
      break;
    }
  case GeomAbs_Hyperbola: 
    {
      BndLib::Add(C.Hyperbola(),U1,U2,Tol,B);
      break;
    }
  case GeomAbs_Parabola: 
    {
      BndLib::Add(C.Parabola(),U1,U2,Tol,B);
      break;
    }
  case GeomAbs_BezierCurve: 
    {
      Handle(Geom_BezierCurve) Bz = C.Bezier();
      Standard_Integer N = Bz->Degree();
      GeomAdaptor_Curve GACurve(Bz);
      Bnd_Box B1;
      tol = FillBox(B1,GACurve,U1,U2,N);
      B1.Enlarge(weakness*tol);
      reduceSplineBox(C, B1, B);
      B.Enlarge(Tol);
      break;
    }
  case GeomAbs_BSplineCurve: 
    {
      Handle(Geom_BSplineCurve) Bs = C.BSpline();
      if(Abs(Bs->FirstParameter() - U1) > Precision::Parametric(Tol)||
	 Abs(Bs->LastParameter()  - U2) > Precision::Parametric(Tol)) {

	Handle(Geom_Geometry) G = Bs->Copy();
	Handle(Geom_BSplineCurve) Bsaux (Handle(Geom_BSplineCurve)::DownCast (G));
	Standard_Real u1 = U1, u2 = U2;
	//// modified by jgv, 24.10.01 for BUC61031 ////
	if (Bsaux->IsPeriodic())
	  ElCLib::AdjustPeriodic( Bsaux->FirstParameter(), Bsaux->LastParameter(), Precision::PConfusion(), u1, u2 );
	else {
	  ////////////////////////////////////////////////
	  //  modified by NIZHNY-EAP Fri Dec  3 14:29:14 1999 ___BEGIN___
	  // To avoid exeption in Segment
	  if(Bsaux->FirstParameter() > U1) u1 = Bsaux->FirstParameter();
	  if(Bsaux->LastParameter()  < U2 ) u2  = Bsaux->LastParameter();
	  //  modified by NIZHNY-EAP Fri Dec  3 14:29:18 1999 ___END___
	}
	Bsaux->Segment(u1, u2);
	Bs = Bsaux;
      }
      //OCC566(apo)->
      Bnd_Box B1;
      Standard_Integer k, k1 = Bs->FirstUKnotIndex(), k2 = Bs->LastUKnotIndex(),
                       N = Bs->Degree(), NbKnots = Bs->NbKnots();
      TColStd_Array1OfReal Knots(1,NbKnots);
      Bs->Knots(Knots);
      GeomAdaptor_Curve GACurve(Bs);
      Standard_Real first = Knots(k1), last;
      for(k = k1 + 1; k <= k2; k++){
	last = Knots(k); 
	tol = Max(FillBox(B1,GACurve,first,last,N), tol);
	first = last;
      }
      if (!B1.IsVoid())
      {
        B1.Enlarge(weakness*tol);
        reduceSplineBox(C, B1, B);
        B.Enlarge(Tol);
      }
      //<-OCC566(apo)
      break;
    }
  default:
    {
      Bnd_Box B1;
      static Standard_Integer N = 33;
      tol = FillBox(B1,C,U1,U2,N);
      B1.Enlarge(weakness*tol);
      Standard_Real x, y, z, X, Y, Z;
      B1.Get(x, y, z, X, Y, Z);
      B.Update(x, y, z, X, Y, Z);
      B.Enlarge(Tol);
    }
  }
}

//=======================================================================
//function : AddOptimal
//purpose  : 
//=======================================================================

void BndLib_Add3dCurve::AddOptimal( const Adaptor3d_Curve& C,
			                              const Standard_Real Tol,
			                              Bnd_Box&      B )
{
  BndLib_Add3dCurve::AddOptimal(C,
			                          C.FirstParameter(),
			                          C.LastParameter (),
			                          Tol,B);
}

//=======================================================================
//function : AddOptimal
//purpose  : 
//=======================================================================

void BndLib_Add3dCurve::AddOptimal( const Adaptor3d_Curve& C,
			                              const Standard_Real U1,
			                              const Standard_Real U2,
			                              const Standard_Real Tol,
			                              Bnd_Box&            B)
{
  switch (C.GetType()) {

    case GeomAbs_Line: 
    {
      BndLib::Add(C.Line(),U1,U2,Tol,B);
      break;
    }
    case GeomAbs_Circle: 
    {
      BndLib::Add(C.Circle(),U1,U2,Tol,B);
      break;
    }
    case GeomAbs_Ellipse: 
    {
      BndLib::Add(C.Ellipse(),U1,U2,Tol,B);
      break;
    }
    case GeomAbs_Hyperbola: 
    {
      BndLib::Add(C.Hyperbola(),U1,U2,Tol,B);
      break;
    }
    case GeomAbs_Parabola: 
    {
      BndLib::Add(C.Parabola(),U1,U2,Tol,B);
      break;
    }
    default:
    {
      AddGenCurv(C, U1, U2, Tol, B);
    }
  }
}

//=======================================================================
//function : AddGenCurv
//purpose  : 
//=======================================================================
void BndLib_Add3dCurve::AddGenCurv(const Adaptor3d_Curve& C, 
                                   const Standard_Real UMin,
                                   const Standard_Real UMax,
                                   const Standard_Real Tol,
                                   Bnd_Box& B)
{
  Standard_Integer Nu = NbSamples(C, UMin, UMax);
  //
  Standard_Real CoordMin[3] = {RealLast(), RealLast(), RealLast()}; 
  Standard_Real CoordMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  Standard_Real DeflMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  //
  gp_Pnt P;
  Standard_Integer i, k;
  Standard_Real du = (UMax-UMin)/(Nu-1), du2 = du / 2.;
  NCollection_Array1<gp_XYZ> aPnts(1, Nu);
  Standard_Real u;
  for (i = 1, u = UMin; i <= Nu; i++, u += du)
  {
    C.D0(u,P);
    aPnts(i) = P.XYZ();
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
      gp_XYZ aPm = 0.5 * (aPnts(i-1) + aPnts(i));
      C.D0(u - du2, P);
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
      if(aPnts(i).Coord(k+1) - CMin < d)
      {
        Standard_Real umin, umax;
        umin = UMin + Max(0, i-2) * du;
        umax = UMin + Min(Nu-1, i) * du;
        Standard_Real cmin = AdjustExtr(C, umin, umax,
                                        CMin, k + 1, eps, Standard_True);
        if(cmin < CMin)
        {
          CMin = cmin;
        }
      }
      else if(CMax - aPnts(i).Coord(k+1) < d)
      {
        Standard_Real umin, umax;
        umin = UMin + Max(0, i-2) * du;
        umax = UMin + Min(Nu-1, i) * du;
        Standard_Real cmax = AdjustExtr(C, umin, umax,
                                        CMax, k + 1, eps, Standard_False);
        if(cmax > CMax)
        {
          CMax = cmax;
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
class CurvMaxMinCoordMVar : public math_MultipleVarFunction
{
public:
  CurvMaxMinCoordMVar(const Adaptor3d_Curve& theCurve, 
                      const Standard_Real UMin,
                      const Standard_Real UMax,
                      const Standard_Integer CoordIndx,                                 
                      const Standard_Real Sign)
: myCurve(theCurve),
  myUMin(UMin),
  myUMax(UMax),
  myCoordIndx(CoordIndx),
  mySign(Sign)
  {
  }

  Standard_Boolean Value (const math_Vector& X,
                                Standard_Real& F)
  {
    if (!CheckInputData(X(1)))
    {
      return Standard_False;
    }
    gp_Pnt aP = myCurve.Value(X(1));

    F = mySign * aP.Coord(myCoordIndx);

    return Standard_True;
  }

  

  Standard_Integer NbVariables() const
  {
    return 1;
  }

private:
  CurvMaxMinCoordMVar & operator = (const CurvMaxMinCoordMVar & theOther);

  Standard_Boolean CheckInputData(Standard_Real theParam)
  {
    if (theParam < myUMin || 
        theParam > myUMax)
      return Standard_False;
    return Standard_True;
  }

  const Adaptor3d_Curve& myCurve;
  Standard_Real myUMin;
  Standard_Real myUMax;
  Standard_Integer myCoordIndx;
  Standard_Real mySign;
};
//
class CurvMaxMinCoord : public math_Function
{
public:
  CurvMaxMinCoord(const Adaptor3d_Curve& theCurve, 
                  const Standard_Real UMin,
                  const Standard_Real UMax,
                  const Standard_Integer CoordIndx,                                 
                  const Standard_Real Sign)
: myCurve(theCurve),
  myUMin(UMin),
  myUMax(UMax),
  myCoordIndx(CoordIndx),
  mySign(Sign)
  {
  }

  Standard_Boolean Value (const Standard_Real X,
                                Standard_Real& F)
  {
    if (!CheckInputData(X))
    {
      return Standard_False;
    }
    gp_Pnt aP = myCurve.Value(X);

    F = mySign * aP.Coord(myCoordIndx);

    return Standard_True;
  }

private:
  CurvMaxMinCoord & operator = (const CurvMaxMinCoord & theOther);

  Standard_Boolean CheckInputData(Standard_Real theParam)
  {
    if (theParam < myUMin || 
        theParam > myUMax)
      return Standard_False;
    return Standard_True;
  }

  const Adaptor3d_Curve& myCurve;
  Standard_Real myUMin;
  Standard_Real myUMax;
  Standard_Integer myCoordIndx;
  Standard_Real mySign;
};

//=======================================================================
//function : AdjustExtr
//purpose  : 
//=======================================================================

Standard_Real AdjustExtr(const Adaptor3d_Curve& C, 
                         const Standard_Real UMin,
                         const Standard_Real UMax,
                         const Standard_Real Extr0,
                         const Standard_Integer CoordIndx,                                 
                         const Standard_Real Tol, 
                         const Standard_Boolean IsMin)
{
  Standard_Real aSign = IsMin ? 1.:-1.;
  Standard_Real extr = aSign * Extr0;
  //
  Standard_Real uTol = Max(C.Resolution(Tol), Precision::PConfusion());
  Standard_Real Du = (C.LastParameter() - C.FirstParameter());
  //
  Standard_Real reltol = uTol / Max(Abs(UMin), Abs(UMax));
  if(UMax - UMin < 0.01 * Du)
  {

    math_BrentMinimum anOptLoc(reltol, 100, uTol);
    CurvMaxMinCoord aFunc(C, UMin, UMax, CoordIndx, aSign);
    anOptLoc.Perform(aFunc, UMin, (UMin+UMax)/2., UMax);
    if(anOptLoc.IsDone())
    {
      extr = anOptLoc.Minimum();
      return aSign * extr;
    }
  }
  //
  Standard_Integer aNbParticles = Max(8, RealToInt(32 * (UMax - UMin) / Du));
  Standard_Real maxstep = (UMax - UMin) / (aNbParticles + 1);
  math_Vector aT(1,1);
  math_Vector aLowBorder(1,1);
  math_Vector aUppBorder(1,1);
  math_Vector aSteps(1,1);
  aLowBorder(1) = UMin;
  aUppBorder(1) = UMax;
  aSteps(1) = Min(0.1 * Du, maxstep);

  CurvMaxMinCoordMVar aFunc(C, UMin, UMax, CoordIndx, aSign);
  math_PSO aFinder(&aFunc, aLowBorder, aUppBorder, aSteps, aNbParticles); 
  aFinder.Perform(aSteps, extr, aT);
  //
  math_BrentMinimum anOptLoc(reltol, 100, uTol);
  CurvMaxMinCoord aFunc1(C, UMin, UMax, CoordIndx, aSign);
  anOptLoc.Perform(aFunc1, Max(aT(1) - aSteps(1), UMin), aT(1), Min(aT(1) + aSteps(1), UMax));

  if(anOptLoc.IsDone())
  {
    extr = anOptLoc.Minimum();
    return aSign * extr;
  }

  return aSign * extr;
}

//=======================================================================
//function : NbSamples
//purpose  : 
//=======================================================================

Standard_Integer NbSamples(const Adaptor3d_Curve& C, 
                           const Standard_Real Umin,
                           const Standard_Real Umax) 
{
  Standard_Integer N;
  GeomAbs_CurveType Type = C.GetType();
  switch (Type) {
  case GeomAbs_BezierCurve: 
    {
      N = 2 * C.NbPoles();
      //By default parametric range of Bezier curv is [0, 1]
      Standard_Real du = Umax - Umin;
      if(du < .9)
      {
        N = RealToInt(du*N) + 1;
        N = Max(N, 5);
      }
      break;
    }
  case GeomAbs_BSplineCurve: 
    {
      const Handle(Geom_BSplineCurve)& BC = C.BSpline();
      N = 2 * (BC->Degree() + 1)*(BC->NbKnots() -1);
      Standard_Real umin = BC->FirstParameter(), 
                    umax = BC->LastParameter();
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
  return Min(500, N);
}
