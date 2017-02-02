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

//  Modified by skv - Wed Jun  2 11:49:59 2004 OCC5898

#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <AdvApprox_ApproxAFunction.hxx>
#include <Approx_SameParameter.hxx>
#include <BSplCLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib_MakeCurvefromApprox.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
//class    : Approx_SameParameter_Evaluator
//purpose  : Used in same parameterization curve approximation.
//=======================================================================
class Approx_SameParameter_Evaluator : public AdvApprox_EvaluatorFunction
{
public:
  Approx_SameParameter_Evaluator (const TColStd_Array1OfReal& theFlatKnots,
                                  const TColStd_Array1OfReal& thePoles,
                                  const Handle(Adaptor2d_HCurve2d)& theHCurve2d)
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
  Handle(Adaptor2d_HCurve2d) HCurve2d;
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
                                const gp_Pnt             APoint,
                                const Standard_Real      Tolerance,
                                const Standard_Integer   NumIteration,
                                const Adaptor3d_Curve&   Curve,
                                Standard_Boolean&        Status,
                                Standard_Real&           Result)
{
  Standard_Integer num_iter = 0, not_done = 1, ii;

  gp_Pnt a_point;
  gp_Vec vector, d1, d2;
  Standard_Real func, func_derivative,
                param = InitValue;
  Status = Standard_False;
  do
  {
    num_iter++;
    Curve.D2(param, a_point, d1, d2);
    for (ii = 1 ; ii <= 3 ; ii++) 
      vector.SetCoord(ii, APoint.Coord(ii) - a_point.Coord(ii));

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
static Standard_Real ComputeTolReached(const Handle(Adaptor3d_HCurve)& c3d,
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
    gp_Pnt Pc3d = c3d->Value(u);
    gp_Pnt Pcons = cons.Value(u);
    if (Precision::IsInfinite(Pcons.X()) ||
        Precision::IsInfinite(Pcons.Y()) ||
        Precision::IsInfinite(Pcons.Z()))
    {
        d2=Precision::Infinite();
        break;
    }
    d2 = Max(d2, Pc3d.SquareDistance(Pcons));
  }

  const Standard_Real aMult = 1.5; // To be tolerant to discrete tolerance computing.
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
                              const TColStd_Array1OfReal& pc3d,
                              const TColStd_Array1OfReal& ,
                              const Handle(Adaptor3d_HCurve)& c3d,
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
  Standard_Real aParamFirst = 3.0 * pc3d(1)   - 2.0 * pc3d(nbp);
  Standard_Real aParamLast = 3.0 * pc3d(nbp) - 2.0 * pc3d(1);

  Standard_Real FirstPar = cons.FirstParameter();
  Standard_Real LastPar  = cons.LastParameter();
  if (aParamFirst < FirstPar)
    aParamFirst = FirstPar;
  if (aParamLast > LastPar)
    aParamLast = LastPar;


  Standard_Real d2 = 0.0; // Maximum square deviation on the samples.
  const Standard_Real d = tol;
  const Standard_Integer nn = 2 * nbp;
  const Standard_Real unsurnn = 1.0/nn;
  for(Standard_Integer i = 0; i <= nn; i++)
  {
    // Compute corresponding parameter on 2d curve.
    // It should be inside of 3d curve parameter space.
    Standard_Real t = unsurnn*i;
    Standard_Real tc3d = pc3d(1)*(1.-t) + pc3d(nbp)*t;
    gp_Pnt Pc3d = c3d->Value(tc3d);
    Standard_Real tcons;
    BSplCLib::Eval(tc3d,Standard_False,0,extrap_mode[0],
                   aDegree,FlatKnots,1, (Standard_Real&)Poles(1),tcons);
    if (tcons < aParamFirst ||
        tcons > aParamLast)
    {
      tol = Precision::Infinite();
      return Standard_False;
    }
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
: mySameParameter(Standard_True),
  myDone(Standard_False)
{
  myHCurve2d = new Geom2dAdaptor_HCurve(C2D);
  myC3d      = new GeomAdaptor_HCurve(C3D);
  mySurf     = new GeomAdaptor_HSurface(S);
  Build(Tol);
}

//=======================================================================
//function : Approx_SameParameter
//purpose  : 
//=======================================================================
Approx_SameParameter::Approx_SameParameter(const Handle(Adaptor3d_HCurve)&   C3D,
                                           const Handle(Geom2d_Curve)&       C2D,
                                           const Handle(Adaptor3d_HSurface)& S,
                                           const Standard_Real               Tol)
: mySameParameter(Standard_True),
  myDone(Standard_False)
{
  myC3d = C3D;
  mySurf = S;
  myHCurve2d = new Geom2dAdaptor_HCurve(C2D);
  Build(Tol);
}

//=======================================================================
//function : Approx_SameParameter
//purpose  : 
//=======================================================================
Approx_SameParameter::Approx_SameParameter(const Handle(Adaptor3d_HCurve)&   C3D,
                                           const Handle(Adaptor2d_HCurve2d)& C2D,
                                           const Handle(Adaptor3d_HSurface)& S,
                                           const Standard_Real               Tol)
: mySameParameter(Standard_True),
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
  const Standard_Real anErrorMAX = 1.0e15;
  const Standard_Integer aMaxArraySize = 1000;
  const Standard_Integer NCONTROL = 22;

  Standard_Integer ii ;
  Adaptor3d_CurveOnSurface CurveOnSurface(myHCurve2d,mySurf);
  Standard_Real fcons = CurveOnSurface.FirstParameter();
  Standard_Real lcons = CurveOnSurface.LastParameter();
  Standard_Real fc3d = myC3d->FirstParameter();
  Standard_Real lc3d = myC3d->LastParameter();

  //Control tangents at the extremities to know if the
  //reparametring is possible and calculate the tangents 
  //at the extremities of the function of change of variable.
  Standard_Real tangent[2] = { 0.0, 0.0 };
  gp_Pnt Pcons,Pc3d;
  gp_Vec Vcons,Vc3d;

  const Standard_Real Tol = Tolerance;
  const Standard_Real Tol2 = Tol * Tol;
  Standard_Real deltamin = Precision::PConfusion();

  Standard_Real besttol2 = Tol2;

  // Check tangency on curve border.
  Standard_Boolean extrok = 1;
  CurveOnSurface.D1(fcons,Pcons,Vcons);
  myC3d->D1(fc3d,Pc3d,Vc3d);
  Standard_Real dist2 = Pcons.SquareDistance(Pc3d);
  Standard_Real dmax2 = dist2;

  Standard_Real magVcons = Vcons.Magnitude();
  if (magVcons > 1.e-12)
    tangent[0] = Vc3d.Magnitude() / magVcons;
  else extrok = 0;

  CurveOnSurface.D1(lcons,Pcons,Vcons);
  myC3d->D1(lc3d,Pc3d,Vc3d);
  dist2 = Pcons.SquareDistance(Pc3d);

  dmax2 = Max(dmax2, dist2);
  magVcons = Vcons.Magnitude();
  if (magVcons > 1.e-12)
    tangent[1] = Vc3d.Magnitude() / magVcons;
  else extrok = 0;


  //Take a multiple of the sample pof CheckShape,
  //at least the control points will be correct. No comment!!!

  Standard_Boolean interpolok = 0;
  Standard_Real tolsov = 1.e200;
  //Take parameters with  constant step on the curve on surface
  //and on curve 3d.
  Standard_Real deltacons = lcons - fcons;
  deltacons /= (NCONTROL);
  Standard_Real deltac3d = lc3d - fc3d;
  deltac3d /= (NCONTROL);

  Standard_Real wcons = fcons;
  Standard_Real wc3d  = fc3d;

  Standard_Real qpcons[aMaxArraySize], qnewpcons[aMaxArraySize], 
                 qpc3d[aMaxArraySize], qnewpc3d[aMaxArraySize];
  Standard_Real * pcons = qpcons; Standard_Real * newpcons = qnewpcons;
  Standard_Real * pc3d = qpc3d; Standard_Real * newpc3d = qnewpc3d;

  for ( ii = 0 ; ii < NCONTROL; ii++) {
    pcons[ii] = wcons;
    pc3d[ii]  = wc3d;
    wcons += deltacons;
    wc3d  += deltac3d;
  }
  pcons[NCONTROL] = lcons;
  pc3d[NCONTROL]  = lc3d;

  // Change number of points in case of C0 continuity.
  Standard_Integer New_NCONTROL = NCONTROL;
  GeomAbs_Shape Continuity = myHCurve2d->Continuity();
  if(Continuity > GeomAbs_C1) Continuity = GeomAbs_C1;
  if(Continuity < GeomAbs_C1)
  {
    Standard_Integer NbInt = myHCurve2d->NbIntervals(GeomAbs_C1) + 1;
    TColStd_Array1OfReal Param_de_decoupeC1 (1, NbInt);
    myHCurve2d->Intervals(Param_de_decoupeC1, GeomAbs_C1);
    TColStd_SequenceOfReal new_par;
    Standard_Integer inter = 1;
    ii =1;
    new_par.Append(fcons);

    while(inter <= NbInt && Param_de_decoupeC1(inter) <= fcons + deltamin) inter++;
    while(NbInt > 0 && Param_de_decoupeC1(NbInt) >= lcons - deltamin) NbInt--;

    while(inter <= NbInt || (ii < NCONTROL && inter <= Param_de_decoupeC1.Length()) ) {
      if(Param_de_decoupeC1(inter) < pcons[ii]) {
        new_par.Append(Param_de_decoupeC1(inter));
        if((pcons[ii] - Param_de_decoupeC1(inter)) <= deltamin) {
          ii++;
          if(ii > NCONTROL) {ii = NCONTROL;}
        }
        inter++;
      }
      else {
        if((Param_de_decoupeC1(inter) - pcons[ii]) > deltamin) {
          new_par.Append(pcons[ii]);
        }
        ii++;
      }
    }

    new_par.Append(lcons);
    New_NCONTROL = new_par.Length() - 1;
    // Simple protection if New_NCONTROL > allocated elements in array but one
    // aMaxArraySize - 1 index may be filled after projection.
    if (New_NCONTROL > aMaxArraySize - 1) {
      mySameParameter = Standard_False;
      return;
    }
    for(ii = 1; ii <= New_NCONTROL; ii++){
      pcons[ii] = pc3d[ii] = new_par.Value(ii + 1);
    }
    pc3d[New_NCONTROL]  = lc3d;
  }

  // Check existing same parameter state.
  Extrema_LocateExtPC Projector;
  Projector.Initialize(myC3d->Curve(),fc3d,lc3d,Tol);

  Standard_Integer count = 1;
  Standard_Real previousp = fc3d, initp=0, curp;
  Standard_Real bornesup = lc3d - deltamin;
  Standard_Boolean projok = 0, 
    use_parameter ;
  for (ii = 1; ii < New_NCONTROL; ii++){    
    CurveOnSurface.D0(pcons[ii],Pcons);
    myC3d->D0(pc3d[ii],Pc3d);
    dist2 = Pcons.SquareDistance(Pc3d);
    use_parameter = (dist2 <= Tol2  && (pc3d[ii] > pc3d[count-1] + deltamin)) ;
    Standard_Real aDistMin = RealLast();;
    if(use_parameter) {

      if(dist2 > dmax2) dmax2 = dist2;
      initp = previousp = pc3d[count] = pc3d[ii];
      pcons[count] = pcons[ii];
      count++;
      
    }
    else {
      if(!projok) initp = pc3d[ii];
      projok = mySameParameter = Standard_False;
      Projector.Perform(Pcons, initp);
      if (Projector.IsDone()) {
        curp = Projector.Point().Parameter();
        Standard_Real dist_2 = Projector.SquareDistance();
        projok = Standard_True;
        aDistMin = dist_2; 
      }
      else
      {
        ProjectPointOnCurve(initp,Pcons,Tol,30,myC3d->Curve(),projok,curp);
        if(projok)
        {
          const gp_Pnt& ap1 =myC3d->Value(curp);
          aDistMin = Pcons.SquareDistance(ap1);
        }
      }
      projok = (projok && (curp > previousp + deltamin && curp < bornesup));
      if(projok)
      {
        initp = previousp = pc3d[count] = curp;
        pcons[count] = pcons[ii];
        count++;
       
      }
      else
      {
        Extrema_ExtPC PR(Pcons,myC3d->Curve(),fc3d,lc3d,Tol);
        if(PR.IsDone())
        {
          const Standard_Integer aNbExt = PR.NbExt();
          if(aNbExt > 0)
          {
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
              if( curp > previousp + deltamin && curp < bornesup)
              {
                aDistMin = aCurDistMin;
                initp = previousp = pc3d[count] = curp;
                pcons[count] = pcons[ii];
                count++;
                projok = Standard_True;
                
              }
            }
         
          }
        }
      }
      if(projok && besttol2 < aDistMin)
        besttol2 = aDistMin;
        
      else if(!projok)
      {
        //Projector
#ifdef OCCT_DEBUG
        cout << "Projection not done" << endl;
#endif
      }
    }
  }

  if(mySameParameter){
    myTolReached = 1.5*sqrt(dmax2);
    return;
  }

  if(!extrok)
  {
    // If not already SameP and tangent to mill, abandon.
    mySameParameter = Standard_False;
#ifdef OCCT_DEBUG
    cout<<"SameParameter problem  : zero tangent to extremities"<<endl;
#endif
    return;
  }

  pcons[count] = lcons;
  pc3d[count]  = lc3d;

  // There is at least one point where same parameter is broken.
  // Try to build B-spline interpolation curve with degree 3.
  // The loop is organized over number of poles.
  Standard_Boolean hasCountChanged = Standard_False;
  do
  {
    // The tables and their limits for the interpolation.
    Standard_Integer num_knots = count + 7;
    Standard_Integer num_poles = count + 3;
    TColStd_Array1OfReal    Paramc3d(*pc3d,1,count+1);
    TColStd_Array1OfReal    Paramcons(*pcons,1,count+1);
    TColStd_Array1OfInteger ContactOrder(1,num_poles) ;
    TColStd_Array1OfReal    Poles(1,num_poles) ;
    TColStd_Array1OfReal    InterpolationParameters(1,num_poles) ;
    TColStd_Array1OfReal    FlatKnots(1,num_knots) ; 

    // Fill tables taking attention to end values.
    ContactOrder.Init(0);
    ContactOrder(2) = ContactOrder(num_poles - 1) = 1;

    FlatKnots(1) = FlatKnots(2) = FlatKnots(3) = FlatKnots(4) = fc3d;
    FlatKnots(num_poles + 1) = FlatKnots(num_poles + 2) = 
      FlatKnots(num_poles + 3) = FlatKnots(num_poles + 4) = lc3d;

    Poles(1) = fcons; Poles(num_poles) = lcons;
    Poles(2) = tangent[0]; Poles(num_poles - 1) = tangent[1];

    InterpolationParameters(1) = InterpolationParameters(2) = fc3d;
    InterpolationParameters(num_poles - 1) = InterpolationParameters(num_poles) = lc3d;

    for (ii = 3; ii <= num_poles - 2; ii++) {
      Poles(ii) = Paramcons(ii - 1);
      InterpolationParameters(ii) = FlatKnots(ii+2) = Paramc3d(ii - 1);
    }
    Standard_Integer inversion_problem;
    BSplCLib::Interpolate(3,FlatKnots,InterpolationParameters,ContactOrder,
      1,Poles(1),inversion_problem);
    if(inversion_problem) {
      throw Standard_ConstructionError();
    }

    // Test if par2d(par3d) is monotonous function or not           ----- IFV, Jan 2000
    // and try to insert new point to improve BSpline interpolation

    Standard_Integer extrap_mode[2] ;
    extrap_mode[0] = extrap_mode[1] = 3;
    Standard_Real eval_result[2] ;
    Standard_Integer DerivativeRequest = 0;
    Standard_Real *PolesArray =
      (Standard_Real *) &Poles(Poles.Lower()) ;

    Standard_Integer newcount = 0;
    for (ii = 0; ii < count; ii++) {

      newpcons[newcount] = pcons[ii];
      newpc3d[newcount] = pc3d[ii];
      newcount++;

      if(count - ii + newcount == aMaxArraySize) continue;

      BSplCLib::Eval(.5*(pc3d[ii]+pc3d[ii+1]), Standard_False, DerivativeRequest,
        extrap_mode[0], 3, FlatKnots, 1, PolesArray[0], eval_result[0]);

      if(eval_result[0] < pcons[ii] || eval_result[0] > pcons[ii+1]) {
        Standard_Real ucons = 0.5*(pcons[ii]+pcons[ii+1]);
        Standard_Real uc3d  = 0.5*(pc3d[ii]+pc3d[ii+1]);

        CurveOnSurface.D0(ucons,Pcons);
        Projector.Perform(Pcons, uc3d);
        if (Projector.IsDone()) {
          curp = Projector.Point().Parameter();
          Standard_Real dist_2 = Projector.SquareDistance();
          if(dist_2 > besttol2) besttol2 = dist_2;
          projok = 1;
        }
        else {
          ProjectPointOnCurve(uc3d,Pcons,Tol,30,myC3d->Curve(),projok,curp);
        }
        if(projok){
          if(curp > pc3d[ii] + deltamin && curp < pc3d[ii+1] - deltamin){
            newpc3d[newcount] = curp;
            newpcons[newcount] = ucons;
            newcount ++;
          }
        }
        else {
#ifdef OCCT_DEBUG
          cout << "Projection not done" << endl;
#endif
        }
      }

    }

    newpc3d[newcount] = pc3d[count];
    newpcons[newcount] = pcons[count];
    Standard_Real * temp;
    temp = pc3d;
    pc3d = newpc3d;
    newpc3d = temp;
    temp = pcons;
    pcons = newpcons;
    newpcons = temp;

    if((count != newcount) && newcount < aMaxArraySize)
    {
      hasCountChanged = Standard_True;
      count = newcount;
      continue;
    }

    count = newcount;

    Standard_Real algtol = sqrt(besttol2);

    interpolok = Check (FlatKnots, Poles, count+1, Paramc3d, Paramcons, 
                        myC3d, CurveOnSurface, algtol, tolsov);

    if (Precision::IsInfinite(algtol)) {
      mySameParameter = Standard_False;
#ifdef OCCT_DEBUG
      cout<<"SameParameter problem  : function of interpolation of parametration at mills !!"<<endl;
#endif
      return;
    }

    tolsov = algtol;

    interpolok = (interpolok || // Good result.
                  count >= aMaxArraySize - 1 ); // Number of points.

    if(interpolok) {
      Standard_Real besttol = sqrt(besttol2);

      Handle(TColStd_HArray1OfReal) tol1d,tol2d,tol3d;
      tol1d = new TColStd_HArray1OfReal(1,2) ;
      tol1d->SetValue(1, mySurf->UResolution(besttol));
      tol1d->SetValue(2, mySurf->VResolution(besttol));

      Approx_SameParameter_Evaluator ev (FlatKnots, Poles, myHCurve2d); 
      AdvApprox_ApproxAFunction  anApproximator(2,0,0,tol1d,tol2d,tol3d,fc3d,lc3d,
        Continuity,11,40,ev);

      if (anApproximator.IsDone() || anApproximator.HasResult()) {
        Adaptor3d_CurveOnSurface ACS = CurveOnSurface;
        GeomLib_MakeCurvefromApprox  aCurveBuilder(anApproximator) ;
        Handle(Geom2d_BSplineCurve) aC2d = aCurveBuilder.Curve2dFromTwo1d(1,2) ;
        Handle(Adaptor2d_HCurve2d) aHCurve2d = new Geom2dAdaptor_HCurve(aC2d);
        CurveOnSurface.Load(aHCurve2d);

        myTolReached = ComputeTolReached(myC3d,CurveOnSurface,NCONTROL);

        if(myTolReached > anErrorMAX)
        {
          //This tolerance is too big. Probably, we will not be able to get 
          //edge with sameparameter in this case.

          myDone = Standard_False;
          return;
        }

        if( (myTolReached < 250.0*besttol) || 
            (count >= aMaxArraySize-2) || 
            !hasCountChanged) //if count does not change after adding new point
                              //(else we can have circularity)
        {
          myCurve2d = aC2d;
          myHCurve2d  = aHCurve2d;
          myDone = Standard_True;
        }
        else
        {
          interpolok = Standard_False;
          CurveOnSurface = ACS;
        }
      } 
    }
    
    if(!interpolok)
    {

      newcount = 0;
      for(Standard_Integer n = 0; n < count; n++){
        newpc3d[newcount] = pc3d[n];
        newpcons[newcount] = pcons[n];
        newcount ++;

        if(count - n + newcount == aMaxArraySize) continue;

        Standard_Real ucons = 0.5*(pcons[n]+pcons[n+1]);
        Standard_Real uc3d  = 0.5*(pc3d[n]+pc3d[n+1]);

        CurveOnSurface.D0(ucons,Pcons);
        Projector.Perform(Pcons, uc3d);
        if (Projector.IsDone()) {
          curp = Projector.Point().Parameter();
          Standard_Real dist_2 = Projector.SquareDistance();
          if(dist_2 > besttol2) besttol2 = dist_2;
          projok = 1;
        }
        else {
          ProjectPointOnCurve(uc3d,Pcons,Tol,30,myC3d->Curve(),projok,curp);
        }
        if(projok){
          if(curp > pc3d[n] + deltamin && curp < pc3d[n+1] - deltamin){
            newpc3d[newcount] = curp;
            newpcons[newcount] = ucons;
            newcount ++;
          }
        }
        else {
#ifdef OCCT_DEBUG
          cout << "Projection not done" << endl;
#endif
        }
      }
      newpc3d[newcount] = pc3d[count];
      newpcons[newcount] = pcons[count];
      Standard_Real * tempx;
      tempx = pc3d;
      pc3d = newpc3d;
      newpc3d = tempx;
      tempx = pcons;
      pcons = newpcons;
      newpcons = tempx;
      
      if(count != newcount)
      {
        count = newcount;
        hasCountChanged = Standard_True;
      }
      else
      {
        hasCountChanged = Standard_False;
      }
    }
  } while(!interpolok && hasCountChanged);

  if (!myDone)
  {
    // Loop is finished unsuccessfully. Fix tolerance by maximal deviation,
    // using data from the last loop iteration.
    Standard_Integer num_knots = count + 7;
    Standard_Integer num_poles = count + 3;
    TColStd_Array1OfReal    Paramc3d(*pc3d,1,count + 1);
    TColStd_Array1OfReal    Paramcons(*pcons,1,count + 1);
    TColStd_Array1OfInteger ContactOrder(1,num_poles) ;
    TColStd_Array1OfReal    Poles(1,num_poles) ;
    TColStd_Array1OfReal    InterpolationParameters(1,num_poles) ;
    TColStd_Array1OfReal    FlatKnots(1,num_knots) ; 

    // Fill tables taking attention to end values.
    ContactOrder.Init(0);
    ContactOrder(2) = ContactOrder(num_poles - 1) = 1;

    FlatKnots(1) = FlatKnots(2) = FlatKnots(3) = FlatKnots(4) = fc3d;
    FlatKnots(num_poles + 1) = FlatKnots(num_poles + 2) = 
      FlatKnots(num_poles + 3) = FlatKnots(num_poles + 4) = lc3d;

    Poles(1) = fcons; Poles(num_poles) = lcons;
    Poles(2) = tangent[0]; Poles(num_poles - 1) = tangent[1];

    InterpolationParameters(1) = InterpolationParameters(2) = fc3d;
    InterpolationParameters(num_poles - 1) = InterpolationParameters(num_poles) = lc3d;

    for (ii = 3; ii <= num_poles - 2; ii++)
    {
      Poles(ii) = Paramcons(ii - 1);
      InterpolationParameters(ii) = FlatKnots(ii+2) = Paramc3d(ii - 1);
    }
    Standard_Integer inversion_problem;
    BSplCLib::Interpolate(3,FlatKnots,InterpolationParameters,ContactOrder,
                          1,Poles(1),inversion_problem);
    if(inversion_problem)
    {
      throw Standard_ConstructionError();
    }

    Standard_Real besttol = sqrt(besttol2);
    Handle(TColStd_HArray1OfReal) tol1d,tol2d,tol3d;
    tol1d = new TColStd_HArray1OfReal(1,2) ;
    tol1d->SetValue(1, mySurf->UResolution(besttol));
    tol1d->SetValue(2, mySurf->VResolution(besttol));

    Approx_SameParameter_Evaluator ev (FlatKnots, Poles, myHCurve2d); 
    AdvApprox_ApproxAFunction  anApproximator(2,0,0,tol1d,tol2d,tol3d,fc3d,lc3d,
                                              Continuity,11,40,ev);

    if (!anApproximator.IsDone() &&
        !anApproximator.HasResult() )
    {
      myDone = Standard_False;
      return;
    }

    GeomLib_MakeCurvefromApprox  aCurveBuilder(anApproximator) ;
    Handle(Geom2d_BSplineCurve) aC2d = aCurveBuilder.Curve2dFromTwo1d(1,2) ;
    Handle(Adaptor2d_HCurve2d) aHCurve2d = new Geom2dAdaptor_HCurve(aC2d);
    CurveOnSurface.Load(aHCurve2d);

    myTolReached = ComputeTolReached(myC3d,CurveOnSurface,NCONTROL);

    if(myTolReached > anErrorMAX)
    {
      //This tolerance is too big. Probably, we will not be able to get
      //edge with sameparameter in this case.
      myDone = Standard_False;
      return;
    }

    myCurve2d = aC2d;
    myHCurve2d  = aHCurve2d;
    myDone = Standard_True;
  }
}
