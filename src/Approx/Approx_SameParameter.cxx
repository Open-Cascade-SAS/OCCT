// Created on: 1995-06-06
// Created by: Xavier BENVENISTE
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

#include <Approx_SameParameter.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib_MakeCurvefromApprox.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Geom2dAdaptor.hxx>

//=======================================================================
//class    : Approx_SameParameter_Evaluator
//purpose  : Used in same parameterization curve approximation.
//=======================================================================
class Approx_SameParameter_Evaluator : public AdvApprox_EvaluatorFunction
{
public:
  Approx_SameParameter_Evaluator (const TColStd_Array1OfReal& theFlatKnots,
                                  const TColStd_Array1OfReal& thePoles,
                                  const Handle(Adaptor2d_Curve2d)& theHCurve2d)
    : FlatKnots(theFlatKnots),
      Poles(thePoles),
      HCurve2d(theHCurve2d) {}

  virtual void Evaluate (Standard_Integer *Dimension,
                         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);

private:
  const TColStd_Array1OfReal& FlatKnots;
  const TColStd_Array1OfReal& Poles;
  Handle(Adaptor2d_Curve2d) HCurve2d;
};

//=======================================================================
//function : Evaluate
//purpose  : 
//=======================================================================
void Approx_SameParameter_Evaluator::Evaluate (Standard_Integer *,/*Dimension*/
                                               Standard_Real    /*StartEnd*/[2],
                                               Standard_Real    *Parameter,
                                               Standard_Integer *DerivativeRequest,
                                               Standard_Real    *Result,
                                               Standard_Integer *ReturnCode) 
{
  const Standard_Integer aDegree = 3;
  Standard_Integer extrap_mode[2] = {aDegree, aDegree};
  Standard_Real eval_result[2];
  Standard_Real *PolesArray = (Standard_Real *) &Poles(Poles.Lower()) ;

  // Evaluate the 1D B-Spline that represents the change in parameterization.
  BSplCLib::Eval(*Parameter,
                 Standard_False,
                 *DerivativeRequest,
                 extrap_mode[0],
                 aDegree,
                 FlatKnots,
                 1,
                 PolesArray[0],
                 eval_result[0]);

  gp_Pnt2d aPoint;
  gp_Vec2d aVector;
  if (*DerivativeRequest == 0)
  {
    HCurve2d->D0(eval_result[0], aPoint);
    aPoint.Coord(Result[0],Result[1]);
  }
  else if (*DerivativeRequest == 1)
  {
    HCurve2d->D1(eval_result[0], aPoint, aVector);
    aVector.Multiply(eval_result[1]);
    aVector.Coord(Result[0],Result[1]);
  }

  ReturnCode[0] = 0;
}

//=======================================================================
//function : ProjectPointOnCurve
//purpose  : 
//=======================================================================
static void ProjectPointOnCurve(const Standard_Real      InitValue,
                                const gp_Pnt&            APoint,
                                const Standard_Real      Tolerance,
                                const Standard_Integer   NumIteration,
                                const Adaptor3d_Curve&   Curve,
                                Standard_Boolean&        Status,
                                Standard_Real&           Result)
{
  Standard_Integer num_iter = 0, not_done = 1;

  gp_Pnt a_point;
  gp_Vec vector, d1, d2;
  Standard_Real func, func_derivative,
                param = InitValue;
  Status = Standard_False;
  do
  {
    num_iter++;
    Curve.D2(param, a_point, d1, d2);
    vector = gp_Vec(a_point,APoint);

    func = vector.Dot(d1);
    if ( Abs(func) < Tolerance * d1.Magnitude())
    {
      not_done = 0;
      Status = Standard_True;
    }
    else
    {
      func_derivative = vector.Dot(d2) - d1.Dot(d1);

      // Avoid division by zero.
      const Standard_Real Toler = 1.0e-12;
      if( Abs(func_derivative) > Toler )
        param -= func / func_derivative;

      param = Max(param,Curve.FirstParameter());
      param = Min(param,Curve.LastParameter());
    }
  } while (not_done && num_iter <= NumIteration);

  Result = param;
}

//=======================================================================
//function : ComputeTolReached
//purpose  :
//=======================================================================
static Standard_Real ComputeTolReached(const Handle(Adaptor3d_Curve)& c3d,
                                       const Adaptor3d_CurveOnSurface& cons,
                                       const Standard_Integer        nbp)
{
  Standard_Real d2 = 0.0; // Square max discrete deviation.
  const Standard_Real first = c3d->FirstParameter();
  const Standard_Real last  = c3d->LastParameter();
  for(Standard_Integer i = 0; i <= nbp; i++)
  {
    Standard_Real t = IntToReal(i) / IntToReal(nbp);
    Standard_Real u = first * (1.0 - t) + last * t;
    gp_Pnt Pc3d, Pcons;
    try
    {
      Pc3d = c3d->Value(u);
      Pcons = cons.Value(u);
    }
    catch (Standard_Failure const&)
    {
      d2 = Precision::Infinite();
      break;
    }
    if (Precision::IsInfinite(Pcons.X()) ||
        Precision::IsInfinite(Pcons.Y()) ||
        Precision::IsInfinite(Pcons.Z()))
    {
        d2=Precision::Infinite();
        break;
    }
    d2 = Max(d2, Pc3d.SquareDistance(Pcons));
  }

  const Standard_Real aMult = 1. + 0.05; // 
  Standard_Real aDeviation = aMult * sqrt(d2);
  aDeviation = Max(aDeviation, Precision::Confusion()); // Tolerance in modeling space.
  return aDeviation;
}

//=======================================================================
//function : Check
//purpose  : Check current interpolation for validity.
//=======================================================================
static Standard_Boolean Check(const TColStd_Array1OfReal& FlatKnots,
                              const TColStd_Array1OfReal& Poles,
                              const Standard_Integer nbp,
                              const Standard_Real *pc3d,
                              const Handle(Adaptor3d_Curve)& c3d,
                              const Adaptor3d_CurveOnSurface& cons,
                              Standard_Real& tol,
                              const Standard_Real oldtol)
{
  const Standard_Integer aDegree = 3;
  Standard_Integer extrap_mode[2] = {aDegree, aDegree};

  // Correction of the interval of valid values. This condition has no sensible
  // grounds. But it is better then the old one (which is commented out) because
  // it fixes the bug OCC5898. To develop more or less sensible criterion it is
  // necessary to deeply investigate this problem which is not possible in frames
  // of debugging.
  Standard_Real aParamFirst = 3.0 * pc3d[0]   - 2.0 * pc3d[nbp - 1];
  Standard_Real aParamLast = 3.0 * pc3d[nbp - 1] - 2.0 * pc3d[0];

  Standard_Real FirstPar = cons.FirstParameter();
  Standard_Real LastPar  = cons.LastParameter();
  if (aParamFirst < FirstPar)
    aParamFirst = FirstPar;
  if (aParamLast > LastPar)
    aParamLast = LastPar;

  Standard_Real d2 = 0.0; // Maximum square deviation on the samples.
  const Standard_Real d = tol;
  const Standard_Integer nn = 2 * nbp;
  const Standard_Real unsurnn = 1.0 / nn;
  Standard_Real tprev = aParamFirst;
  for(Standard_Integer i = 0; i <= nn; i++)
  {
    // Compute corresponding parameter on 2d curve.
    // It should be inside of 3d curve parameter space.
    Standard_Real t = unsurnn*i;
    Standard_Real tc3d = pc3d[0]*(1.0 - t) + pc3d[nbp - 1] * t; // weight function.
    gp_Pnt Pc3d = c3d->Value(tc3d);
    Standard_Real tcons;
    BSplCLib::Eval(tc3d, Standard_False, 0, extrap_mode[0],
                   aDegree, FlatKnots, 1, (Standard_Real&)Poles(1), tcons);

    if (tcons < tprev ||
        tcons > aParamLast)
    {
      tol = Precision::Infinite();
      return Standard_False;
    }
    tprev = tcons;
    gp_Pnt Pcons = cons.Value(tcons);
    Standard_Real temp = Pc3d.SquareDistance(Pcons);
    if(temp > d2) d2 = temp;
  }
  tol = sqrt(d2);

  // Check poles parameters to be ordered.
  for(Standard_Integer i = Poles.Lower() + 1; i <= Poles.Upper(); ++i)
  {
    const Standard_Real aPreviousParam = Poles(i - 1);
    const Standard_Real aCurrentParam  = Poles(i);

    if (aPreviousParam > aCurrentParam)
      return Standard_False;
  }

  return (tol <= d || tol > 0.8 * oldtol);
}

//=======================================================================
//function : Approx_SameParameter
//purpose  : 
//=======================================================================
Approx_SameParameter::Approx_SameParameter(const Handle(Geom_Curve)&   C3D,
                                           const Handle(Geom2d_Curve)& C2D,
                                           const Handle(Geom_Surface)& S,
                                           const Standard_Real         Tol)
: myDeltaMin(Precision::PConfusion()),
  mySameParameter(Standard_True),
  myDone(Standard_False)
{
  myHCurve2d = new Geom2dAdaptor_Curve(C2D);
  myC3d      = new GeomAdaptor_Curve(C3D);
  mySurf     = new GeomAdaptor_Surface(S);
  Build(Tol);
}

//=======================================================================
//function : Approx_SameParameter
//purpose  : 
//=======================================================================
Approx_SameParameter::Approx_SameParameter(const Handle(Adaptor3d_Curve)&   C3D,
                                           const Handle(Geom2d_Curve)&       C2D,
                                           const Handle(Adaptor3d_Surface)& S,
                                           const Standard_Real               Tol)
: myDeltaMin(Precision::PConfusion()),
  mySameParameter(Standard_True),
  myDone(Standard_False)
{
  myC3d = C3D;
  mySurf = S;
  myHCurve2d = new Geom2dAdaptor_Curve(C2D);
  Build(Tol);
}

//=======================================================================
//function : Approx_SameParameter
//purpose  : 
//=======================================================================
Approx_SameParameter::Approx_SameParameter(const Handle(Adaptor3d_Curve)&   C3D,
                                           const Handle(Adaptor2d_Curve2d)& C2D,
                                           const Handle(Adaptor3d_Surface)& S,
                                           const Standard_Real               Tol)
: myDeltaMin(Precision::PConfusion()),
  mySameParameter(Standard_True),
  myDone(Standard_False)
{
  myC3d = C3D;
  mySurf = S;
  myHCurve2d = C2D;
  Build(Tol);
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================
void Approx_SameParameter::Build(const Standard_Real Tolerance)
{
  // Algorithm:
  // 1) Build initial distribution. Increase number of samples in case of C0 continuity.
  // 2.1) Check same parameter state on samples.
  // 2.2) Compute parameters in 2d space if not same parameter.
  // 3) Loop over poles number and try to interpolate 2d curve.
  // 4) If loop is failed build 2d curve forcibly or use original pcurve.
  Standard_Real qpcons[myMaxArraySize], qnewpcons[myMaxArraySize],
                 qpc3d[myMaxArraySize], qnewpc3d[myMaxArraySize];

  // Create and fill data structure.
  Approx_SameParameter_Data aData;
  aData.myCOnS = Adaptor3d_CurveOnSurface(myHCurve2d,mySurf);
  aData.myC2dPF = aData.myCOnS.FirstParameter();
  aData.myC2dPL = aData.myCOnS.LastParameter();
  aData.myC3dPF = myC3d->FirstParameter();
  aData.myC3dPL = myC3d->LastParameter();
  aData.myNbPnt = 0; // No points initially.
  aData.myPC2d = qpcons;
  aData.myPC3d = qpc3d;
  aData.myNewPC2d = qnewpcons;
  aData.myNewPC3d = qnewpc3d;
  aData.myTol = Tolerance;

  // Build initial distribution.
  if (!BuildInitialDistribution(aData))
  {
    mySameParameter = Standard_False;
    myDone = Standard_False;
    return;
  }

  // Check same parameter state on distribution.
  Standard_Real aMaxSqDeviation = 0.0;
  const Standard_Real aPercentOfBadProj = 0.3;
  Standard_Integer aNbPnt = aData.myNbPnt - RealToInt(aPercentOfBadProj * aData.myNbPnt);
  mySameParameter = CheckSameParameter(aData, aMaxSqDeviation);
  if(mySameParameter)
  {
    myTolReached = ComputeTolReached(myC3d, aData.myCOnS, 2 * myNbSamples);
    myDone = Standard_True;
    return;
  }
  else
  {
    // Control number of sample points after checking sameparameter
    // If number of points is less then initial one, it means that there are 
    // problems with projection
    if(aData.myNbPnt < aNbPnt )
    {
      myTolReached = ComputeTolReached(myC3d,aData.myCOnS, 2 * myNbSamples);
      myCurve2d = Geom2dAdaptor::MakeCurve (*myHCurve2d);
      myDone = Standard_False;
      return;
    }
  }

  // Control tangents at the extremities to know if the
  // reparametring is possible and calculate the tangents
  // at the extremities of the function of change of variable.
  Standard_Real tangent[2] = { 0.0, 0.0 };
  if(!ComputeTangents(aData.myCOnS, tangent[0], tangent[1]))
  {
    // Cannot compute tangents.
    mySameParameter = Standard_False;
    myDone = Standard_False;
    myTolReached = ComputeTolReached(myC3d, aData.myCOnS, 2 * myNbSamples);
    return;
  }

  // There is at least one point where same parameter is broken.
  // Try to build B-spline approximation curve using interpolation with degree 3.
  // The loop is organized over number of poles.
  GeomAbs_Shape aContinuity = myHCurve2d->Continuity();
  if(aContinuity > GeomAbs_C1) aContinuity = GeomAbs_C1;

  Standard_Real besttol2 = aData.myTol * aData.myTol,
                tolsov = Precision::Infinite();
  Standard_Boolean interpolok = Standard_False,
                   hasCountChanged = Standard_False;
  do
  {
    // Interpolation data.
    Standard_Integer num_knots = aData.myNbPnt + 7;
    Standard_Integer num_poles = aData.myNbPnt + 3;
    TColStd_Array1OfReal    Poles(1, num_poles);
    TColStd_Array1OfReal    FlatKnots(1 ,num_knots);

    if (!Interpolate(aData, tangent[0], tangent[1], Poles, FlatKnots))
    {
      // Interpolation fails.
      mySameParameter = Standard_False;
      myDone = Standard_False;
      myTolReached = ComputeTolReached(myC3d, aData.myCOnS, 2 * myNbSamples);
      return;
    }

    Standard_Real algtol = sqrt(besttol2);
    interpolok = Check (FlatKnots, Poles, aData.myNbPnt+1, aData.myPC3d,
                        myC3d, aData.myCOnS, algtol, tolsov);
    tolsov = algtol;

    // Try to build 2d curve and check it for validity.
    if(interpolok)
    {
      Standard_Real besttol = sqrt(besttol2);

      Handle(TColStd_HArray1OfReal) tol1d,tol2d,tol3d;
      tol1d = new TColStd_HArray1OfReal(1,2);
      tol1d->SetValue(1, mySurf->UResolution(besttol));
      tol1d->SetValue(2, mySurf->VResolution(besttol));

      Approx_SameParameter_Evaluator ev (FlatKnots, Poles, myHCurve2d);
      Standard_Integer aMaxDeg = 11, aMaxSeg = 1000;
      AdvApprox_ApproxAFunction  anApproximator(2,0,0,tol1d,tol2d,tol3d,aData.myC3dPF,aData.myC3dPL,
                                                aContinuity,aMaxDeg,aMaxSeg,ev);

      if (anApproximator.IsDone() || anApproximator.HasResult())
      {
        Adaptor3d_CurveOnSurface ACS = aData.myCOnS;
        GeomLib_MakeCurvefromApprox  aCurveBuilder(anApproximator);
        Handle(Geom2d_BSplineCurve) aC2d = aCurveBuilder.Curve2dFromTwo1d(1,2);
        Handle(Adaptor2d_Curve2d) aHCurve2d = new Geom2dAdaptor_Curve(aC2d);
        aData.myCOnS.Load(aHCurve2d);
        myTolReached = ComputeTolReached(myC3d,aData.myCOnS, 2 * myNbSamples);

        const Standard_Real aMult = 250.0; // To be tolerant with discrete tolerance.
        if (myTolReached < aMult * besttol ) 
        {
          myCurve2d = aC2d;
          myHCurve2d = aHCurve2d;
          myDone = Standard_True;
          break;
        }
        else if(aData.myNbPnt < myMaxArraySize - 1)
        {
          interpolok = Standard_False;
          aData.myCOnS = ACS;
        }
        else
        {
          break;
        }
      }
    }

    if (!interpolok)
      hasCountChanged = IncreaseNbPoles(Poles, FlatKnots, aData, besttol2);

  } while(!interpolok && hasCountChanged);

  if (!myDone)
  {
    // Loop is finished unsuccessfully. Fix tolerance by maximal deviation,
    // using data from the last loop iteration or initial data. Use data set with minimal deflection.

    // Original 2d curve.
    aData.myCOnS.Load(myHCurve2d);
    myTolReached = ComputeTolReached(myC3d,aData.myCOnS, 2 * myNbSamples);
    myCurve2d = Geom2dAdaptor::MakeCurve (*myHCurve2d);

    // Approximation curve.
    Standard_Integer num_knots = aData.myNbPnt + 7;
    Standard_Integer num_poles = aData.myNbPnt + 3;
    TColStd_Array1OfReal    Poles(1, num_poles);
    TColStd_Array1OfReal    FlatKnots(1 ,num_knots);

    Interpolate(aData, tangent[0], tangent[1],
                Poles, FlatKnots);

    Standard_Real besttol = sqrt(besttol2);
    Handle(TColStd_HArray1OfReal) tol1d,tol2d,tol3d;
    tol1d = new TColStd_HArray1OfReal(1,2) ;
    tol1d->SetValue(1, mySurf->UResolution(besttol));
    tol1d->SetValue(2, mySurf->VResolution(besttol));

    Approx_SameParameter_Evaluator ev(FlatKnots, Poles, myHCurve2d);
    AdvApprox_ApproxAFunction  anApproximator(2,0,0,tol1d,tol2d,tol3d,aData.myC3dPF,aData.myC3dPL,
                                              aContinuity,11,40,ev);

    if (!anApproximator.IsDone() &&
        !anApproximator.HasResult() )
    {
      myDone = Standard_False;
      return;
    }

    GeomLib_MakeCurvefromApprox  aCurveBuilder(anApproximator);
    Handle(Geom2d_BSplineCurve) aC2d = aCurveBuilder.Curve2dFromTwo1d(1,2);
    Handle(Adaptor2d_Curve2d) aHCurve2d = new Geom2dAdaptor_Curve(aC2d);
    aData.myCOnS.Load(aHCurve2d);

    Standard_Real anApproxTol = ComputeTolReached(myC3d,aData.myCOnS,2 * myNbSamples);
    if (anApproxTol < myTolReached)
    {
      myTolReached = anApproxTol;
      myCurve2d = aC2d;
      myHCurve2d  = aHCurve2d;
    }
    myDone = Standard_True;
  }
  
  myCurveOnSurface = Handle(Adaptor3d_CurveOnSurface)::DownCast(aData.myCOnS.ShallowCopy());
}

//=======================================================================
//function : BuildInitialDistribution
//purpose  : Sub-method in Build.
//=======================================================================
Standard_Boolean Approx_SameParameter::BuildInitialDistribution(Approx_SameParameter_Data &theData) const
{
  // Take a multiple of the sample pof CheckShape,
  // at least the control points will be correct.
  // Take parameters with constant step on the curve on surface
  // and on curve 3d.
  const Standard_Real deltacons = (theData.myC2dPL - theData.myC2dPF) / myNbSamples;
  const Standard_Real deltac3d  = (theData.myC3dPL - theData.myC3dPF) / myNbSamples;
  Standard_Real wcons = theData.myC2dPF;
  Standard_Real wc3d  = theData.myC3dPF;
  for (Standard_Integer ii = 0 ; ii < myNbSamples; ii++)
  {
    theData.myPC2d[ii] = wcons;
    theData.myPC3d[ii]  = wc3d;
    wcons += deltacons;
    wc3d  += deltac3d;
  }
  theData.myNbPnt = myNbSamples;
  theData.myPC2d[theData.myNbPnt] = theData.myC2dPL;
  theData.myPC3d[theData.myNbPnt]  = theData.myC3dPL;

  // Change number of points in case of C0 continuity.
  GeomAbs_Shape Continuity = myHCurve2d->Continuity();
  if(Continuity < GeomAbs_C1)
  {
    if (!IncreaseInitialNbSamples(theData))
    {
      // Number of samples is too big.
      return Standard_False;
    }
  }

  return Standard_True;
}

//=======================================================================
//function : IncreaseInitialNbSamples
//purpose  : Get number of C1 intervals and build new distribution on them.
//           Sub-method in BuildInitialDistribution.
//=======================================================================
Standard_Boolean Approx_SameParameter::IncreaseInitialNbSamples(Approx_SameParameter_Data &theData) const
{
  Standard_Integer NbInt = myHCurve2d->NbIntervals(GeomAbs_C1) + 1;
  TColStd_Array1OfReal aC1Intervals (1, NbInt);
  myHCurve2d->Intervals(aC1Intervals, GeomAbs_C1);

  Standard_Integer inter = 1;
  while(inter <= NbInt && aC1Intervals(inter) <= theData.myC3dPF + myDeltaMin) inter++;
  while(NbInt > 0 && aC1Intervals(NbInt) >= theData.myC3dPL - myDeltaMin) NbInt--;

  // Compute new parameters.
  TColStd_SequenceOfReal aNewPar;
  aNewPar.Append(theData.myC3dPF);
  Standard_Integer ii = 1;
  while(inter <= NbInt || (ii < myNbSamples && inter <= aC1Intervals.Length()) )
  {
    if(aC1Intervals(inter) < theData.myPC2d[ii])
    {
      aNewPar.Append(aC1Intervals(inter));
      if((theData.myPC2d[ii] - aC1Intervals(inter)) <= myDeltaMin)
      {
        ii++;
        if(ii > myNbSamples)
        {
          ii = myNbSamples;
        }
      }
      inter++;
    }
    else
    {
      if((aC1Intervals(inter) - theData.myPC2d[ii]) > myDeltaMin)
      {
        aNewPar.Append(theData.myPC2d[ii]);
      }
      ii++;
    }
  }
  // Simple protection if theNewNbPoints > allocated elements in array but one
  // myMaxArraySize - 1 index may be filled after projection.
  theData.myNbPnt = aNewPar.Length();
  if (theData.myNbPnt > myMaxArraySize - 1)
  {
    return Standard_False;
  }

  for(ii = 1; ii < theData.myNbPnt; ii++)
  {
    // Copy only internal points.
    theData.myPC2d[ii] = theData.myPC3d[ii] = aNewPar.Value(ii + 1);
  }
  theData.myPC3d[theData.myNbPnt]  = theData.myC3dPL;
  theData.myPC2d[theData.myNbPnt] = theData.myC2dPL;

  return Standard_True;
}

//=======================================================================
//function : CheckSameParameter
//purpose  : Sub-method in Build.
//=======================================================================
Standard_Boolean Approx_SameParameter::CheckSameParameter(Approx_SameParameter_Data &theData,
                                                          Standard_Real &theSqDist) const
{
  const Standard_Real Tol2 = theData.myTol * theData.myTol;
  Standard_Boolean isSameParam = Standard_True;

  // Compute initial distance on boundary points.
  gp_Pnt Pcons, Pc3d;
  theData.myCOnS.D0(theData.myC2dPF, Pcons);
  myC3d->D0(theData.myC3dPF, Pc3d);
  Standard_Real dist2 = Pcons.SquareDistance(Pc3d);
  Standard_Real dmax2 = dist2;

  theData.myCOnS.D0(theData.myC2dPL, Pcons);
  myC3d->D0(theData.myC3dPL, Pc3d);
  dist2 = Pcons.SquareDistance(Pc3d);
  dmax2 = Max(dmax2, dist2);

  Extrema_LocateExtPC Projector;
  Projector.Initialize (*myC3d, theData.myC3dPF, theData.myC3dPL, theData.myTol);

  Standard_Integer count = 1;
  Standard_Real previousp = theData.myC3dPF, initp=0, curp;
  Standard_Real bornesup = theData.myC3dPL - myDeltaMin;
  Standard_Boolean isProjOk = Standard_False;
  for (Standard_Integer ii = 1; ii < theData.myNbPnt; ii++)
  {
    theData.myCOnS.D0(theData.myPC2d[ii],Pcons);
    myC3d->D0(theData.myPC3d[ii],Pc3d);
    dist2 = Pcons.SquareDistance(Pc3d);

    // Same parameter point.
    Standard_Boolean isUseParam = (dist2 <= Tol2  && // Good distance.
      (theData.myPC3d[ii] > theData.myPC3d[count-1] + myDeltaMin)); // Point is separated from previous.
    if(isUseParam)
    {
      if(dmax2 < dist2)
        dmax2 = dist2;
      initp = previousp = theData.myPC3d[count] = theData.myPC3d[ii];
      theData.myPC2d[count] = theData.myPC2d[ii];
      count++;
      continue;
    }

    // Local search: local extrema and iterative projection algorithm.
    if(!isProjOk)
      initp = theData.myPC3d[ii];
    isProjOk = isSameParam = Standard_False;
    Projector.Perform(Pcons, initp);
    if (Projector.IsDone())
    {
      // Local extrema is found.
      curp = Projector.Point().Parameter();
      isProjOk = Standard_True;
    }
    else
    {
      ProjectPointOnCurve(initp,Pcons,theData.myTol,30, *myC3d,isProjOk,curp);
    }
    isProjOk = isProjOk && // Good projection.
               curp > previousp + myDeltaMin && // Point is separated from previous.
               curp < bornesup; // Inside of parameter space.
    if(isProjOk)
    {
      initp = previousp = theData.myPC3d[count] = curp;
      theData.myPC2d[count] = theData.myPC2d[ii];
      count++;
      continue;
    }

    // Whole parameter space search using general extrema.
    Extrema_ExtPC PR(Pcons, *myC3d, theData.myC3dPF, theData.myC3dPL, theData.myTol);
    if (!PR.IsDone() || PR.NbExt() == 0) // Lazy evaluation is used.
      continue;

    const Standard_Integer aNbExt = PR.NbExt();
    Standard_Integer anIndMin = 0;
    Standard_Real aCurDistMin = RealLast();
    for(Standard_Integer i = 1; i <= aNbExt; i++)
    {
      const gp_Pnt &aP = PR.Point(i).Value();
      Standard_Real aDist2 = aP.SquareDistance(Pcons);
      if(aDist2 < aCurDistMin)
      {
        aCurDistMin = aDist2;
        anIndMin = i;
      }
    }
    if(anIndMin)
    {
      curp = PR.Point(anIndMin).Parameter();
      if( curp > previousp + myDeltaMin && curp < bornesup)
      {
        initp = previousp = theData.myPC3d[count] = curp;
        theData.myPC2d[count] = theData.myPC2d[ii];
        count++;
        isProjOk = Standard_True;
      }
    }
  }
  theData.myNbPnt = count;
  theData.myPC2d[theData.myNbPnt] = theData.myC2dPL;
  theData.myPC3d[theData.myNbPnt] = theData.myC3dPL;

  theSqDist = dmax2;
  return isSameParam;
}

//=======================================================================
//function : ComputeTangents
//purpose  : Sub-method in Build.
//=======================================================================
Standard_Boolean Approx_SameParameter::ComputeTangents(const Adaptor3d_CurveOnSurface & theCOnS,
                                                       Standard_Real &theFirstTangent,
                                                       Standard_Real &theLastTangent) const
{
  const Standard_Real aSmallMagnitude = 1.0e-12;
  // Check tangency on curve border.
  gp_Pnt aPnt, aPntCOnS;
  gp_Vec aVec, aVecConS;

  // First point.
  const Standard_Real aParamFirst = myC3d->FirstParameter();
  theCOnS.D1(aParamFirst, aPntCOnS, aVecConS);
  myC3d->D1(aParamFirst, aPnt, aVec);
  Standard_Real aMagnitude = aVecConS.Magnitude();
  if (aMagnitude > aSmallMagnitude)
    theFirstTangent = aVec.Magnitude() / aMagnitude;
  else
    return Standard_False;

  // Last point.
  const Standard_Real aParamLast = myC3d->LastParameter();
  theCOnS.D1(aParamLast,aPntCOnS,aVecConS);
  myC3d->D1(aParamLast, aPnt, aVec);

  aMagnitude = aVecConS.Magnitude();
  if (aMagnitude > aSmallMagnitude)
    theLastTangent = aVec.Magnitude() / aMagnitude;
  else
    return Standard_False;

  return Standard_True;
}

//=======================================================================
//function : Interpolate
//purpose  : Sub-method in Build.
//=======================================================================
Standard_Boolean Approx_SameParameter::Interpolate(const Approx_SameParameter_Data & theData,
                                       const Standard_Real aTangFirst,
                                       const Standard_Real aTangLast,
                                       TColStd_Array1OfReal & thePoles,
                                       TColStd_Array1OfReal & theFlatKnots) const
{
  Standard_Integer num_poles = theData.myNbPnt + 3;
  TColStd_Array1OfInteger ContactOrder(1,num_poles);
  TColStd_Array1OfReal    aParameters(1, num_poles);

  // Fill tables taking attention to end values.
  ContactOrder.Init(0);
  ContactOrder(2) = ContactOrder(num_poles - 1) = 1;

  theFlatKnots(1) = theFlatKnots(2) = theFlatKnots(3) = theFlatKnots(4) = theData.myC3dPF;
  theFlatKnots(num_poles + 1) = theFlatKnots(num_poles + 2) = 
    theFlatKnots(num_poles + 3) = theFlatKnots(num_poles + 4) = theData.myC3dPL;

  thePoles(1) = theData.myC2dPF; thePoles(num_poles) = theData.myC2dPL;
  thePoles(2) = aTangFirst; thePoles(num_poles - 1) = aTangLast;

  aParameters(1) = aParameters(2) = theData.myC3dPF;
  aParameters(num_poles - 1) = aParameters(num_poles) = theData.myC3dPL;

  for (Standard_Integer ii = 3; ii <= num_poles - 2; ii++)
  {
    thePoles(ii) = theData.myPC2d[ii - 2];
    aParameters(ii) = theFlatKnots(ii+2) = theData.myPC3d[ii - 2];
  }
  Standard_Integer inversion_problem;
  BSplCLib::Interpolate(3,theFlatKnots,aParameters,ContactOrder,
                        1,thePoles(1),inversion_problem);
  if(inversion_problem)
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : IncreaseNbPoles
//purpose  : Sub-method in Build.
//=======================================================================
Standard_Boolean Approx_SameParameter::IncreaseNbPoles(const TColStd_Array1OfReal & thePoles,
                                                       const TColStd_Array1OfReal & theFlatKnots,
                                                       Approx_SameParameter_Data & theData,
                                                       Standard_Real &theBestSqTol) const
{
  Extrema_LocateExtPC Projector;
  Projector.Initialize (*myC3d, myC3d->FirstParameter(), myC3d->LastParameter(), theData.myTol);
  Standard_Real curp = 0.0;
  Standard_Boolean projok = Standard_False;

  // Project middle point to fix parameterization and check projection existence.
  const Standard_Integer aDegree = 3;
  const Standard_Integer DerivativeRequest = 0;
  Standard_Integer extrap_mode[2] = {aDegree, aDegree};
  Standard_Real eval_result;
  Standard_Real *PolesArray = (Standard_Real *) &thePoles(thePoles.Lower());
  Standard_Integer newcount = 0;
  for (Standard_Integer ii = 0; ii < theData.myNbPnt; ii++)
  {
    theData.myNewPC2d[newcount] = theData.myPC2d[ii];
    theData.myNewPC3d[newcount] = theData.myPC3d[ii];
    newcount++;

    if(theData.myNbPnt - ii + newcount == myMaxArraySize) continue;

    BSplCLib::Eval(0.5*(theData.myPC3d[ii]+theData.myPC3d[ii+1]), Standard_False, DerivativeRequest,
      extrap_mode[0], 3, theFlatKnots, 1, PolesArray[0], eval_result);

    if(eval_result < theData.myPC2d[ii] || eval_result > theData.myPC2d[ii+1])
    {
      Standard_Real ucons = 0.5*(theData.myPC2d[ii]+theData.myPC2d[ii+1]);
      Standard_Real uc3d  = 0.5*(theData.myPC3d[ii]+theData.myPC3d[ii+1]);

      gp_Pnt Pcons;
      theData.myCOnS.D0(ucons,Pcons);
      Projector.Perform(Pcons, uc3d);
      if (Projector.IsDone())
      {
        curp = Projector.Point().Parameter();
        Standard_Real dist_2 = Projector.SquareDistance();
        if(dist_2 > theBestSqTol) theBestSqTol = dist_2;
        projok = 1;
      }
      else
      {
        ProjectPointOnCurve(uc3d,Pcons,theData.myTol,30, *myC3d,projok,curp);
      }
      if(projok)
      {
        if(curp > theData.myPC3d[ii] + myDeltaMin && curp < theData.myPC3d[ii+1] - myDeltaMin)
        {
          theData.myNewPC3d[newcount] = curp;
          theData.myNewPC2d[newcount] = ucons;
          newcount ++;
        }
      }
    }
  } //     for (ii = 0; ii < count; ii++)
  theData.myNewPC3d[newcount] = theData.myPC3d[theData.myNbPnt];
  theData.myNewPC2d[newcount] = theData.myPC2d[theData.myNbPnt];

  if((theData.myNbPnt != newcount) && newcount < myMaxArraySize - 1)
  {
    // Distribution is changed.
    theData.Swap(newcount);
    return Standard_True;
  }

  // Increase number of samples in two times.
  newcount = 0;
  for(Standard_Integer n = 0; n < theData.myNbPnt; n++)
  {
    theData.myNewPC3d[newcount] = theData.myPC3d[n];
    theData.myNewPC2d[newcount] = theData.myPC2d[n];
    newcount ++;

    if(theData.myNbPnt - n + newcount == myMaxArraySize) continue;

    Standard_Real ucons = 0.5*(theData.myPC2d[n]+theData.myPC2d[n+1]);
    Standard_Real uc3d  = 0.5*(theData.myPC3d[n]+theData.myPC3d[n+1]);

    gp_Pnt Pcons;
    theData.myCOnS.D0(ucons,Pcons);
    Projector.Perform(Pcons, uc3d);
    if (Projector.IsDone()) 
    {
      curp = Projector.Point().Parameter();
      Standard_Real dist_2 = Projector.SquareDistance();
      if(dist_2 > theBestSqTol) theBestSqTol = dist_2;
      projok = 1;
    }
    else 
    {
      ProjectPointOnCurve(uc3d,Pcons,theData.myTol,30, *myC3d,projok,curp);
    }
    if(projok)
    {
      if(curp > theData.myPC3d[n] + myDeltaMin && curp < theData.myPC3d[n+1] - myDeltaMin)
      {
        theData.myNewPC3d[newcount] = curp;
        theData.myNewPC2d[newcount] = ucons;
        newcount ++;
      }
    }
  }
  theData.myNewPC3d[newcount] = theData.myPC3d[theData.myNbPnt];
  theData.myNewPC2d[newcount] = theData.myPC2d[theData.myNbPnt];

  if(theData.myNbPnt != newcount)
  {
    // Distribution is changed.
    theData.Swap(newcount);
    return Standard_True;
  }

  return Standard_False;
}
