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


#include <Adaptor2d_Curve2d.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_Function.hxx>
#include <math_BrentMinimum.hxx>
#include <math_PSO.hxx>

//=======================================================================
//function : BndLib_Box2dCurve
//purpose  : 
//=======================================================================
class BndLib_Box2dCurve  {
 public:
  BndLib_Box2dCurve();

  virtual ~BndLib_Box2dCurve();

  void SetCurve(const Handle(Geom2d_Curve)& aC);

  const Handle(Geom2d_Curve)& Curve() const;

  void SetRange(const Standard_Real aT1,
		const Standard_Real aT2);

  void Range(Standard_Real& aT1,
	     Standard_Real& aT2) const;

  const Bnd_Box2d& Box() const;

  void Perform();

  void PerformOptimal(const Standard_Real Tol);

  void Clear();

  Standard_Integer ErrorStatus() const;
  //
  //-----------------------------
 protected:
  void CheckData();
  void GetInfoBase();
  void PerformLineConic();
  void PerformBezier();
  void PerformBSpline();
  void PerformOther();
  void D0(const Standard_Real, gp_Pnt2d&);
  //
  void Compute(const Handle(Geom2d_Conic)&,
	       const GeomAbs_CurveType,
	       const Standard_Real,
	       const Standard_Real,
	       Bnd_Box2d& aBox2D);
  //
  static
    Standard_Integer Compute(const Handle(Geom2d_Conic)&,
			     const GeomAbs_CurveType,
			     Standard_Real *);
  static
    Standard_Boolean IsTypeBase(const Handle(Geom2d_Curve)& ,
			      GeomAbs_CurveType& );
  static
    Standard_Real AdjustToPeriod(const Standard_Real ,
				 const Standard_Real );
  //
  void PerformOnePoint();
  //
  void PerformGenCurv(const Standard_Real Tol = Precision::PConfusion());
  //
  Standard_Integer NbSamples();
  //
  Standard_Real AdjustExtr(const Standard_Real UMin,
                           const Standard_Real UMax,
                           const Standard_Real Extr0,
                           const Standard_Integer CoordIndx,
                           const Standard_Real Tol, 
                           const Standard_Boolean IsMin);
  //-----------------------------
 protected:
  Handle(Geom2d_Curve) myCurve;
  Bnd_Box2d myBox;
  Standard_Integer myErrorStatus;
  Handle(Geom2d_Curve) myCurveBase;
  Standard_Real myOffsetBase;
  Standard_Boolean myOffsetFlag;
  Standard_Real myT1;
  Standard_Real myT2;
  GeomAbs_CurveType myTypeBase;
};
//
class Curv2dMaxMinCoordMVar : public math_MultipleVarFunction
{
public:
  Curv2dMaxMinCoordMVar(const Handle(Geom2d_Curve)& theCurve, 
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
    gp_Pnt2d aP = myCurve->Value(X(1));

    F = mySign * aP.Coord(myCoordIndx);

    return Standard_True;
  }

  

  Standard_Integer NbVariables() const
  {
    return 1;
  }

private:
  Curv2dMaxMinCoordMVar & operator = (const Curv2dMaxMinCoordMVar & theOther);

  Standard_Boolean CheckInputData(Standard_Real theParam)
  {
    if (theParam < myUMin || 
        theParam > myUMax)
      return Standard_False;
    return Standard_True;
  }

  const Handle(Geom2d_Curve)& myCurve;
  Standard_Real myUMin;
  Standard_Real myUMax;
  Standard_Integer myCoordIndx;
  Standard_Real mySign;
};
//
class Curv2dMaxMinCoord : public math_Function
{
public:
  Curv2dMaxMinCoord(const Handle(Geom2d_Curve)& theCurve, 
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
    gp_Pnt2d aP = myCurve->Value(X);

    F = mySign * aP.Coord(myCoordIndx);

    return Standard_True;
  }

private:
  Curv2dMaxMinCoord & operator = (const Curv2dMaxMinCoord & theOther);

  Standard_Boolean CheckInputData(Standard_Real theParam)
  {
    if (theParam < myUMin || 
        theParam > myUMax)
      return Standard_False;
    return Standard_True;
  }

  const Handle(Geom2d_Curve)& myCurve;
  Standard_Real myUMin;
  Standard_Real myUMax;
  Standard_Integer myCoordIndx;
  Standard_Real mySign;
};

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BndLib_Box2dCurve::BndLib_Box2dCurve()
{
  Clear();
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BndLib_Box2dCurve::~BndLib_Box2dCurve()
{
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::Clear()
{
  myBox.SetVoid();
  //
  myErrorStatus=-1;
  myTypeBase=GeomAbs_OtherCurve;
  myOffsetBase=0.;
  myOffsetFlag=Standard_False;
}
//=======================================================================
//function : SetCurve
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::SetCurve(const Handle(Geom2d_Curve)& aC2D)
{
  myCurve=aC2D;
}
//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================
const Handle(Geom2d_Curve)& BndLib_Box2dCurve::Curve()const
{
  return myCurve;
}
//=======================================================================
//function : SetRange
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::SetRange(const Standard_Real aT1,
				 const Standard_Real aT2)
{
  myT1=aT1;
  myT2=aT2;
}
//=======================================================================
//function : tRange
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::Range(Standard_Real& aT1,
			      Standard_Real& aT2) const
{
  aT1=myT1;
  aT2=myT2;
}
//=======================================================================
//function : ErrorStatus
//purpose  : 
//=======================================================================
Standard_Integer BndLib_Box2dCurve::ErrorStatus()const
{
  return myErrorStatus;
}
//=======================================================================
//function : Box
//purpose  : 
//=======================================================================
const Bnd_Box2d& BndLib_Box2dCurve::Box()const
{
  return myBox;
}
//=======================================================================
//function : CheckData
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::CheckData()
{
  myErrorStatus=0;
  //
  if(myCurve.IsNull()) {
    myErrorStatus=10;
    return;
  }
  //
  if(myT1>myT2) {
    myErrorStatus=12; // invalid range
    return;
  }
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::Perform()
{
  Clear();
  // 
  myErrorStatus=0;
  //
  CheckData();
  if(myErrorStatus) {
    return;
  }
  //
  if (myT1==myT2) {
    PerformOnePoint();
    return;
  }
  //
  GetInfoBase();
  if(myErrorStatus) {
    return;
  }
  // 
  if (myTypeBase==GeomAbs_Line ||
      myTypeBase==GeomAbs_Circle ||
      myTypeBase==GeomAbs_Ellipse ||
      myTypeBase==GeomAbs_Parabola ||
      myTypeBase==GeomAbs_Hyperbola) { // LineConic
    PerformLineConic();
  }
  else if (myTypeBase==GeomAbs_BezierCurve) { // Bezier
    PerformBezier();
  }
  else if (myTypeBase==GeomAbs_BSplineCurve) { //B-Spline
    PerformBSpline();
  }
  else {
    myErrorStatus=11; // unknown type base
  }
}
//=======================================================================
//function : PerformOptimal
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::PerformOptimal(const Standard_Real Tol)
{
  Clear();
  myErrorStatus=0;
  CheckData();

  if(myErrorStatus) {
    return;
  }
  
  if (myT1==myT2) {
    PerformOnePoint();
    return;
  }
  
  GetInfoBase();
  if(myErrorStatus) {
    return;
  }
  
  if (myTypeBase==GeomAbs_Line ||
      myTypeBase==GeomAbs_Circle ||
      myTypeBase==GeomAbs_Ellipse ||
      myTypeBase==GeomAbs_Parabola ||
      myTypeBase==GeomAbs_Hyperbola) { // LineConic
    PerformLineConic();
  }
  else {
    PerformGenCurv(Tol);
  }
}
//=======================================================================
//function : PerformOnePoint
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::PerformOnePoint()
{
  gp_Pnt2d aP2D;
  //
  myCurve->D0(myT1, aP2D);
  myBox.Add(aP2D);
}
//=======================================================================
//function : PerformBezier
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::PerformBezier()
{
  if (myOffsetFlag) {
    PerformOther();
    return;
  }
  //
  Standard_Integer i, aNbPoles;
  Standard_Real aT1, aT2, aTb[2];
  gp_Pnt2d aP2D;
  Handle(Geom2d_Geometry) aG;
  Handle(Geom2d_BezierCurve) aCBz, aCBzSeg;
  //
  myErrorStatus=0;
  Bnd_Box2d& aBox2D=myBox;
  //
  aCBz=Handle(Geom2d_BezierCurve)::DownCast(myCurveBase);
  aT1=aCBz->FirstParameter();
  aT2=aCBz->LastParameter();
  //
  aTb[0]=myT1;
  if (aTb[0]<aT1) {
    aTb[0]=aT1;
  }
  //
  aTb[1]=myT2;
  if (aTb[1]>aT2) {
    aTb[1]=aT2;
  }
  //
  if (!(aT1==aTb[0] && aT2==aTb[1])) {  
    aG=aCBz->Copy();
    //
    aCBzSeg=Handle(Geom2d_BezierCurve)::DownCast(aG);
    aCBzSeg->Segment(aTb[0], aTb[1]);
    aCBz=aCBzSeg;
  }
  //
  aNbPoles=aCBz->NbPoles();
  for (i=1; i<=aNbPoles; ++i) {
    aP2D=aCBz->Pole(i);
    aBox2D.Add(aP2D);
  }
}
//=======================================================================
//function : PerformBSpline
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::PerformBSpline()
{
  if (myOffsetFlag) {
    PerformOther();
    return;
  }
  //
  Standard_Integer i, aNbPoles;
  Standard_Real  aT1, aT2, aTb[2];
  gp_Pnt2d aP2D;
  Handle(Geom2d_Geometry) aG;
  Handle(Geom2d_BSplineCurve) aCBS, aCBSs;
  //
  myErrorStatus=0;
  Bnd_Box2d& aBox2D=myBox;
  //
  aCBS=Handle(Geom2d_BSplineCurve)::DownCast(myCurveBase);
  aT1=aCBS->FirstParameter();
  aT2=aCBS->LastParameter();
  //
  aTb[0]=myT1;
  if (aTb[0]<aT1) {
    aTb[0]=aT1;
  } 
  aTb[1]=myT2;
  if (aTb[1]>aT2) {
    aTb[1]=aT2;
  }

  if(aTb[1] < aTb[0])
  {
    aTb[0]=aT1;
    aTb[1]=aT2;
  }

  //
  const Standard_Real eps = Precision::PConfusion();
  if (fabs(aT1-aTb[0]) > eps || fabs(aT2-aTb[1]) > eps) {
    aG=aCBS->Copy();
    //
    aCBSs=Handle(Geom2d_BSplineCurve)::DownCast(aG);
    aCBSs->Segment(aTb[0], aTb[1]);
    aCBS=aCBSs;
  }
  //
  aNbPoles=aCBS->NbPoles();
  for (i=1; i<=aNbPoles; ++i) {
    aP2D=aCBS->Pole(i);
    aBox2D.Add(aP2D);
  }
}
//=======================================================================
//function : PerformOther
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::PerformOther()
{
  Standard_Integer j, aNb;
  Standard_Real aT, dT;
  gp_Pnt2d aP2D;
  //
  aNb=33;
  dT=(myT2-myT1)/(aNb-1);
  //
  for (j=0; j<aNb; ++j) {
    aT=myT1+j*dT;
    myCurve->D0(aT, aP2D);
    myBox.Add(aP2D);
  }
  myCurve->D0(myT2, aP2D);
  myBox.Add(aP2D);
}
//=======================================================================
//function : NbSamples
//purpose  : 
//=======================================================================
Standard_Integer BndLib_Box2dCurve::NbSamples()
{
  Standard_Integer N;
  switch (myTypeBase) {
  case GeomAbs_BezierCurve: 
    {
      Handle(Geom2d_BezierCurve) aCBz=Handle(Geom2d_BezierCurve)::DownCast(myCurveBase);
      N = aCBz->NbPoles();
      //By default parametric range of Bezier curv is [0, 1]
      Standard_Real du = myT2 - myT1;
      if(du < .9)
      {
        N = RealToInt(du*N) + 1;
        N = Max(N, 5);
      }
      break;
    }
  case GeomAbs_BSplineCurve: 
    {
      Handle(Geom2d_BSplineCurve) aCBS=Handle(Geom2d_BSplineCurve)::DownCast(myCurveBase);
      N = (aCBS->Degree() + 1)*(aCBS->NbKnots() -1);
      Standard_Real umin = aCBS->FirstParameter(), 
                    umax = aCBS->LastParameter();
      Standard_Real du = (myT2 - myT1) / (umax - umin);
      if(du < .9)
      {
        N = RealToInt(du*N) + 1;
        N = Max(N, 5);
      }
      break;
    }
  default:
    N = 17;
  }
  return Min (23,N);
}
//=======================================================================
//function : AdjustExtr
//purpose  : 
//=======================================================================
Standard_Real BndLib_Box2dCurve::AdjustExtr(const Standard_Real UMin,
                                            const Standard_Real UMax,
                                            const Standard_Real Extr0,
                                            const Standard_Integer CoordIndx,                                 
                                            const Standard_Real Tol, 
                                            const Standard_Boolean IsMin)
{
  Standard_Real aSign = IsMin ? 1.:-1.;
  Standard_Real extr = aSign * Extr0;
  //
  Standard_Real Du = (myCurve->LastParameter() - myCurve->FirstParameter());
  //
  Geom2dAdaptor_Curve aGAC(myCurve);
  Standard_Real UTol = Max(aGAC.Resolution(Tol), Precision::PConfusion());
  Standard_Real reltol = UTol / Max(Abs(UMin), Abs(UMax));
  if(UMax - UMin < 0.01 * Du)
  {
    //It is suggested that function has one extremum on small interval
    math_BrentMinimum anOptLoc(reltol, 100, UTol);
    Curv2dMaxMinCoord aFunc(myCurve, UMin, UMax, CoordIndx, aSign);
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

  Curv2dMaxMinCoordMVar aFunc(myCurve, UMin, UMax, CoordIndx, aSign);
  math_PSO aFinder(&aFunc, aLowBorder, aUppBorder, aSteps, aNbParticles); 
  aFinder.Perform(aSteps, extr, aT);
  //
  math_BrentMinimum anOptLoc(reltol, 100, UTol);
  Curv2dMaxMinCoord aFunc1(myCurve, UMin, UMax, CoordIndx, aSign);
  anOptLoc.Perform(aFunc1, Max(aT(1) - aSteps(1), UMin), aT(1), Min(aT(1) + aSteps(1), UMax));

  if(anOptLoc.IsDone())
  {
    extr = anOptLoc.Minimum();
    return aSign * extr;
  }

  return aSign * extr;
}

//=======================================================================
//function : PerformGenCurv
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::PerformGenCurv(const Standard_Real Tol)
{
  //
  Standard_Integer Nu = NbSamples();
  //
  Standard_Real CoordMin[2] = {RealLast(), RealLast()}; 
  Standard_Real CoordMax[2] = {-RealLast(), -RealLast()};
  Standard_Real DeflMax[2] = {-RealLast(), -RealLast()};
  //
  gp_Pnt2d P;
  Standard_Integer i, k;
  Standard_Real du = (myT2 - myT1)/(Nu-1), du2 = du / 2.;
  NCollection_Array1<gp_XY> aPnts(1, Nu);
  Standard_Real u;
  for (i = 1, u = myT1; i <= Nu; i++, u += du)
  {
    D0(u,P);
    aPnts(i) = P.XY();
    //
    for(k = 0; k < 2; ++k)
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
      gp_XY aPm = 0.5 * (aPnts(i-1) + aPnts(i));
      D0(u - du2, P);
      gp_XY aD = (P.XY() - aPm);
      for(k = 0; k < 2; ++k)
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
  for(k = 0; k < 2; ++k)
  {
    Standard_Real d = DeflMax[k];
    if(d <= Tol)
    {
      continue;
    }
    Standard_Real CMin = CoordMin[k];
    Standard_Real CMax = CoordMax[k];
    for(i = 1; i <= Nu; ++i)
    {
      if(aPnts(i).Coord(k+1) - CMin < d)
      {
        Standard_Real tmin, tmax;
        tmin = myT1 + Max(0, i-2) * du;
        tmax = myT1 + Min(Nu-1, i) * du;
        Standard_Real cmin = AdjustExtr(tmin, tmax,
                                        CMin, k + 1, Tol, Standard_True);
        if(cmin < CMin)
        {
          CMin = cmin;
        }
      }
      else if(CMax - aPnts(i).Coord(k+1) < d)
      {
        Standard_Real tmin, tmax;
        tmin = myT1 + Max(0, i-2) * du;
        tmax = myT1 + Min(Nu-1, i) * du;
        Standard_Real cmax = AdjustExtr(tmin, tmax,
                                        CMax, k + 1, Tol, Standard_False);
        if(cmax > CMax)
        {
          CMax = cmax;
        }
      }
    }
    CoordMin[k] = CMin;
    CoordMax[k] = CMax;
  }

  myBox.Add(gp_Pnt2d(CoordMin[0], CoordMin[1]));
  myBox.Add(gp_Pnt2d(CoordMax[0], CoordMax[1]));
  myBox.Enlarge(Tol);
}
//=======================================================================
//function : D0
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::D0(const Standard_Real aU,
			    gp_Pnt2d& aP2D)  
{
  gp_Vec2d aV1;
  //
  myCurveBase->D1(aU, aP2D, aV1);
  //
  if (myOffsetFlag) {
    Standard_Integer aIndex, aMaxDegree;
    Standard_Real aA, aB, aR, aRes;
    //
    aMaxDegree=9;
    aIndex = 2;
    aRes=gp::Resolution();
    //
    while (aV1.Magnitude() <= aRes && aIndex <= aMaxDegree) {
      aV1=myCurveBase->DN(aU, aIndex);
      ++aIndex;
    }
    //
    aA=aV1.Y();
    aB=-aV1.X();
    aR=sqrt(aA*aA+aB*aB);
    if(aR<=aRes) {
      myErrorStatus=13;
      return;
    } 
    //
    aR=myOffsetBase/aR;
    aA=aA*aR;
    aB=aB*aR;
    aP2D.SetCoord(aP2D.X()+aA, aP2D.Y()+aB);
  }
  //
}
//=======================================================================
//function : GetInfoBase
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::GetInfoBase()
{
  Standard_Boolean bIsTypeBase;
  Standard_Integer  iTrimmed, iOffset;
  GeomAbs_CurveType aTypeB;
  Handle(Geom2d_Curve) aC2DB;
  Handle(Geom2d_TrimmedCurve) aCT2D;
  Handle(Geom2d_OffsetCurve) aCF2D;
  //
  myErrorStatus=0;
  myTypeBase=GeomAbs_OtherCurve;
  myOffsetBase=0;
  //
  aC2DB=myCurve;
  bIsTypeBase=IsTypeBase(aC2DB, aTypeB);
  if (bIsTypeBase) {
    myTypeBase=aTypeB;
    myCurveBase=myCurve;
    return;
  }
  //
  while(!bIsTypeBase) {
    iTrimmed=0;
    iOffset=0;
    aCT2D=Handle(Geom2d_TrimmedCurve)::DownCast(aC2DB);
    if (!aCT2D.IsNull()) {
      aC2DB=aCT2D->BasisCurve();
      ++iTrimmed;
    }
    //
    aCF2D=Handle(Geom2d_OffsetCurve)::DownCast(aC2DB);
    if (!aCF2D.IsNull()) {
      Standard_Real aOffset;
      //
      aOffset=aCF2D->Offset();
      myOffsetBase=myOffsetBase+aOffset;
      myOffsetFlag=Standard_True;
      //
      aC2DB=aCF2D->BasisCurve();
      ++iOffset;
    }
    //
    if (!(iTrimmed || iOffset)) {
      break;
    }
    //
    bIsTypeBase=IsTypeBase(aC2DB, aTypeB);
    if (bIsTypeBase) {
      myTypeBase=aTypeB;
      myCurveBase=aC2DB;
      return;
    }
  }
  //
  myErrorStatus=11; // unknown type base
}
//=======================================================================
//function : IsTypeBase
//purpose  : 
//=======================================================================
Standard_Boolean BndLib_Box2dCurve::IsTypeBase
  (const Handle(Geom2d_Curve)& aC2D,
   GeomAbs_CurveType& aTypeB)
{
  Standard_Boolean bRet; 
  Handle(Standard_Type) aType;
  //
  bRet=Standard_True;
  //
  aType=aC2D->DynamicType();
  if (aType==STANDARD_TYPE(Geom2d_Line)) {
    aTypeB=GeomAbs_Line;
  }
  else if (aType==STANDARD_TYPE(Geom2d_Circle)) {
    aTypeB=GeomAbs_Circle;
  }
  else if (aType==STANDARD_TYPE(Geom2d_Ellipse)) {
    aTypeB=GeomAbs_Ellipse;
  }
  else if (aType==STANDARD_TYPE(Geom2d_Parabola)) {
    aTypeB=GeomAbs_Parabola;
  }
  else if (aType==STANDARD_TYPE(Geom2d_Hyperbola)) {
    aTypeB=GeomAbs_Hyperbola;
  }
  else if (aType==STANDARD_TYPE(Geom2d_BezierCurve)) {
    aTypeB=GeomAbs_BezierCurve;
  }
  else if (aType==STANDARD_TYPE(Geom2d_BSplineCurve)) {
    aTypeB=GeomAbs_BSplineCurve;
  }
  else {
    aTypeB=GeomAbs_OtherCurve;
    bRet=!bRet;
  }
  return bRet;
}
//=======================================================================
//function : PerformLineConic
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::PerformLineConic()
{
  Standard_Integer i, iInf[2];
  Standard_Real  aTb[2];
  gp_Pnt2d aP2D;
  //
  myErrorStatus=0;
  //
  Bnd_Box2d& aBox2D=myBox;
  //
  iInf[0]=0;
  iInf[1]=0;
  aTb[0]=myT1;
  aTb[1]=myT2;
  //
  for (i=0; i<2; ++i) {
    if (Precision::IsNegativeInfinite(aTb[i])) {
      D0(aTb[i], aP2D);
      aBox2D.Add(aP2D);
      ++iInf[0];
    }
    else if (Precision::IsPositiveInfinite(aTb[i])) {
      D0(aTb[i], aP2D);
      aBox2D.Add(aP2D);
      ++iInf[1];
    }
    else {
      D0(aTb[i], aP2D);
      aBox2D.Add(aP2D);
    }
  } 
  //
  if (myTypeBase==GeomAbs_Line) {
    return;
  }
  //
  if (iInf[0] && iInf[1]) {
    return;
  }
  //-------------
  Handle(Geom2d_Conic) aConic2D;
  //
  aConic2D=Handle(Geom2d_Conic)::DownCast(myCurveBase);
  Compute(aConic2D, myTypeBase, aTb[0], aTb[1], aBox2D);
  
}
//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void BndLib_Box2dCurve::Compute(const Handle(Geom2d_Conic)& aConic2D,
				const GeomAbs_CurveType aType,
				const Standard_Real aT1,
				const Standard_Real aT2,
				Bnd_Box2d& aBox2D)
{
  Standard_Integer i, aNbT;
  Standard_Real pT[10], aT, aTwoPI, dT, aEps;
  gp_Pnt2d aP2D;
  //
  aNbT=Compute(aConic2D, aType, pT);
  //
  if (aType==GeomAbs_Parabola ||  aType==GeomAbs_Hyperbola) {
    for (i=0; i<aNbT; ++i) {
      aT=pT[i];
      if (aT>aT1 && aT<aT2) {
	D0(aT, aP2D);
	aBox2D.Add(aP2D);
      }
    }
    return;
  }
  //
  //aType==GeomAbs_Circle ||  aType==GeomAbs_Ellipse
  aEps=1.e-14;
  aTwoPI=2.*M_PI;
  dT=aT2-aT1;
  //
  Standard_Real aT1z = AdjustToPeriod (aT1, aTwoPI);
  if (fabs(aT1z)<aEps) {
    aT1z=0.;
  }
  //
  Standard_Real aT2z = aT1z + dT;
  if (fabs(aT2z-aTwoPI)<aEps) {
    aT2z=aTwoPI;
  }
  //
  for (i=0; i<aNbT; ++i) {
    aT = pT[i];
    // adjust aT to range [aT1z, aT1z + 2*PI]; note that pT[i] and aT1z
    // are adjusted to range [0, 2*PI], but aT2z can be greater than 2*PI
    aT = (aT < aT1z ? aT + aTwoPI : aT);
    if (aT <= aT2z) {
      D0(aT, aP2D);
      aBox2D.Add(aP2D);
    }
  }
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
Standard_Integer BndLib_Box2dCurve::Compute
  (const Handle(Geom2d_Conic)& aConic2D,
   const GeomAbs_CurveType aType,
   Standard_Real *pT)
{
  Standard_Integer iRet, i, j;
  Standard_Real aCosBt, aSinBt, aCosGm, aSinGm;
  Standard_Real aLx, aLy;
  //
  iRet=0;
  //
  const gp_Ax22d& aPos=aConic2D->Position();
  const gp_XY& aXDir=aPos.XDirection().XY();
  const gp_XY& aYDir=aPos.YDirection().XY();
  //
  aCosBt=aXDir.X();
  aSinBt=aXDir.Y();
  aCosGm=aYDir.X();
  aSinGm=aYDir.Y();
  //
  if (aType==GeomAbs_Circle ||  aType==GeomAbs_Ellipse) {
    Standard_Real aR1 = 0.0, aR2 = 0.0, aTwoPI = M_PI+M_PI;
    Standard_Real aA11 = 0.0, aA12 = 0.0, aA21 = 0.0, aA22 = 0.0;
    Standard_Real aBx = 0.0, aBy = 0.0, aB = 0.0, aCosFi = 0.0, aSinFi = 0.0, aFi = 0.0;
    //
    if(aType==GeomAbs_Ellipse) {
      Handle(Geom2d_Ellipse) aEL2D;
      //
      aEL2D=Handle(Geom2d_Ellipse)::DownCast(aConic2D);
      aR1=aEL2D->MajorRadius();
      aR2=aEL2D->MinorRadius();
    }
    else if(aType==GeomAbs_Circle) {
      Handle(Geom2d_Circle) aCR2D;
      //
      aCR2D=Handle(Geom2d_Circle)::DownCast(aConic2D);
      aR1=aCR2D->Radius();
      aR2=aR1;
    }
    //
    aA11=-aR1*aCosBt;
    aA12= aR2*aCosGm;
    aA21=-aR1*aSinBt;
    aA22= aR2*aSinGm;
    //
    for (i=0; i<2; ++i) {
      aLx=(!i) ? 0. : 1.;
      aLy=(!i) ? 1. : 0.;
      aBx=aLx*aA21-aLy*aA11;
      aBy=aLx*aA22-aLy*aA12;
      aB=sqrt(aBx*aBx+aBy*aBy);
      //
      aCosFi=aBx/aB;
      aSinFi=aBy/aB;
      //
      aFi=acos(aCosFi);
      if (aSinFi<0.) {
	aFi=aTwoPI-aFi;
      }
      //
      j=2*i;
      pT[j]=aTwoPI-aFi;
      pT[j]=AdjustToPeriod(pT[j], aTwoPI);
      //
      pT[j+1]=M_PI-aFi;
      pT[j+1]=AdjustToPeriod(pT[j+1], aTwoPI);
    }
    iRet=4;
  }//if (aType==GeomAbs_Ellipse) {
  //
  else if (aType==GeomAbs_Parabola) {
    Standard_Real aFc, aEps;
    Standard_Real aA1, aA2;
    Handle(Geom2d_Parabola) aPR2D;
    //
    aEps=1.e-12;
    //
    aPR2D=Handle(Geom2d_Parabola)::DownCast(aConic2D);
    aFc=aPR2D->Focal();
    //
    j=0;
    for (i=0; i<2; i++) {
      aLx=(!i) ? 0. : 1.;
      aLy=(!i) ? 1. : 0.;
      //
      aA2=aLx*aSinBt-aLy*aCosBt;
      if (fabs(aA2)<aEps) {
	continue;
      } 
      //
      aA1=aLy*aCosGm-aLx*aSinGm;
      //
      pT[j]=2.*aFc*aA1/aA2;
      ++j;
    }
    iRet=j;
  }// else if (aType==GeomAbs_Parabola) {
  //
  else if (aType==GeomAbs_Hyperbola) {
    Standard_Integer k;
    Standard_Real aR1, aR2; 
    Standard_Real aEps, aB1, aB2, aB12, aB22, aZ, aD;
    Handle(Geom2d_Hyperbola) aHP2D;
    //
    aEps=1.e-12;
    //
    aHP2D=Handle(Geom2d_Hyperbola)::DownCast(aConic2D);
    aR1=aHP2D->MajorRadius();
    aR2=aHP2D->MinorRadius();
    //
    j=0;
    for (i=0; i<2; i++) {
      aLx=(!i) ? 0. : 1.;
      aLy=(!i) ? 1. : 0.;
      //
      aB1=aR1*(aLx*aSinBt-aLy*aCosBt);
      aB2=aR2*(aLx*aSinGm-aLy*aCosGm);
      // 
      if (fabs(aB1)<aEps) {
	continue;
      } 
      //
      if (fabs(aB2)<aEps) {
	pT[j]=0.;
	++j;
      } 
      else {
	aB12=aB1*aB1;
	aB22=aB2*aB2;
	if (!(aB12>aB22)) {
	  continue;
	}
	//
	aD=sqrt(aB12-aB22);
	//-------------
	for (k=-1; k<2; k+=2) {
	  aZ=(aB1+k*aD)/aB2;
	  if (fabs(aZ)<1.) {
	    pT[j]=-log((1.+aZ)/(1.-aZ));
	    ++j;
	  }
	}
      }
    }
    iRet=j;
  }// else if (aType==GeomAbs_Hyperbola) {
  //
  return iRet;
}
//=======================================================================
//function : AdjustToPeriod
//purpose  : 
//=======================================================================
Standard_Real BndLib_Box2dCurve::AdjustToPeriod(const Standard_Real aT,
						const Standard_Real aPeriod)
{
  Standard_Integer k;
  Standard_Real aTRet;
  //
  aTRet=aT;
  if (aT<0.) {
    k=1+(Standard_Integer)(-aT/aPeriod);
    aTRet=aT+k*aPeriod;
  }
  else if (aT>aPeriod) {
    k=(Standard_Integer)(aT/aPeriod);
    aTRet=aT-k*aPeriod;
  }
  if (aTRet==aPeriod) {
    aTRet=0.;
  }
  return aTRet;
}
//
// myErrorStatus:
//
// -1 - object is just initialized
// 10 - myCurve is Null
// 12 - invalid range myT1 >  myT2l
// 11 - unknown type of base curve
// 13 - offset curve can not be computed
//NMTTest

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BndLib_Add2dCurve::Add(const Adaptor2d_Curve2d& aC,
			     const Standard_Real aTol,
			     Bnd_Box2d& aBox2D) 
{
  BndLib_Add2dCurve::Add(aC,
			  aC.FirstParameter(),
			  aC.LastParameter (),
			  aTol,
			  aBox2D);
}
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BndLib_Add2dCurve::Add(const Adaptor2d_Curve2d& aC,
			     const Standard_Real aU1,
			     const Standard_Real aU2,
			     const Standard_Real aTol,
			     Bnd_Box2d& aBox2D)
{
  Adaptor2d_Curve2d *pC=(Adaptor2d_Curve2d *)&aC;
  Geom2dAdaptor_Curve *pA=dynamic_cast<Geom2dAdaptor_Curve*>(pC);
  if (!pA) {
    Standard_Real U, DU;
    Standard_Integer N, j;
    gp_Pnt2d P;
    N = 33;
    U  = aU1;
    DU = (aU2-aU1)/(N-1);
    for (j=1; j<N; j++) {
      aC.D0(U,P);
      U+=DU;
      aBox2D.Add(P);
    }
    aC.D0(aU2,P);
    aBox2D.Add(P);
    aBox2D.Enlarge(aTol);
    return;
  }
  //
  const Handle(Geom2d_Curve)& aC2D=pA->Curve();
  //
  BndLib_Add2dCurve::Add(aC2D, aU1, aU2, aTol, aBox2D);
}
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BndLib_Add2dCurve::Add(const Handle(Geom2d_Curve)& aC2D,
			     const Standard_Real aTol,
			     Bnd_Box2d& aBox2D)
{
  Standard_Real aT1, aT2;
  //
  aT1=aC2D->FirstParameter();
  aT2=aC2D->LastParameter();
  //
  BndLib_Add2dCurve::Add(aC2D, aT1, aT2, aTol, aBox2D);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BndLib_Add2dCurve::Add(const Handle(Geom2d_Curve)& aC2D,
			     const Standard_Real aT1,
			     const Standard_Real aT2,
			     const Standard_Real aTol,
			     Bnd_Box2d& aBox2D)
{
  BndLib_Box2dCurve aBC;
  //
  aBC.SetCurve(aC2D);
  aBC.SetRange(aT1, aT2);
  //
  aBC.Perform();
  //
  const Bnd_Box2d& aBoxC=aBC.Box();
  aBox2D.Add(aBoxC);
  aBox2D.Enlarge(aTol);
}
//=======================================================================
//function : AddOptimal
//purpose  : 
//=======================================================================
void BndLib_Add2dCurve::AddOptimal(const Handle(Geom2d_Curve)& aC2D,
                                   const Standard_Real aT1,
                                   const Standard_Real aT2,
                                   const Standard_Real aTol,
                                   Bnd_Box2d& aBox2D)
{
  BndLib_Box2dCurve aBC;
  //
  aBC.SetCurve(aC2D);
  aBC.SetRange(aT1, aT2);
  //
  aBC.PerformOptimal(aTol);
  //
  const Bnd_Box2d& aBoxC=aBC.Box();
  aBox2D.Add(aBoxC);
  aBox2D.Enlarge(aTol);
}