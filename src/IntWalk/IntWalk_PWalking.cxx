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


#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Extrema_GenLocateExtPS.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntImp_ComputeTangence.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <IntWalk_PWalking.hxx>
#include <IntWalk_StatusDeflection.hxx>
#include <math_FunctionSetRoot.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

//==================================================================================
// function : ComputePasInit
// purpose  : estimate of max step : To avoid abrupt changes during change of isos 
//==================================================================================
void IntWalk_PWalking::ComputePasInit(const Standard_Real theDeltaU1,
                                      const Standard_Real theDeltaV1,
                                      const Standard_Real theDeltaU2,
                                      const Standard_Real theDeltaV2)
{
  const Standard_Real aRangePart = 0.01;
  const Standard_Real Increment = 2.0*pasMax;
  const Handle(Adaptor3d_HSurface)& 
          Caro1 = myIntersectionOn2S.Function().AuxillarSurface1();
  const Handle(Adaptor3d_HSurface)& 
          Caro2 = myIntersectionOn2S.Function().AuxillarSurface2();

  const Standard_Real aDeltaU1=Abs(UM1-Um1);
  const Standard_Real aDeltaV1=Abs(VM1-Vm1);
  const Standard_Real aDeltaU2=Abs(UM2-Um2);
  const Standard_Real aDeltaV2=Abs(VM2-Vm2);

  //-- limit the reduction of uv box estimate to 0.01 natural box
  //--  theDeltaU1 : On box of Inter
  //-- aDeltaU1 : On parametric space
  if(!Precision::IsInfinite(aDeltaU1))
    pasuv[0]=Max(Increment*Max(theDeltaU1, aRangePart*aDeltaU1), pasuv[0]);
  else
    pasuv[0]=Max(Increment*theDeltaU1, pasuv[0]);

  if(!Precision::IsInfinite(aDeltaV1))
    pasuv[1]=Max(Increment*Max(theDeltaV1, aRangePart*aDeltaV1), pasuv[1]);
  else
    pasuv[1]=Max(Increment*theDeltaV1, pasuv[1]);

  if(!Precision::IsInfinite(aDeltaU2))
    pasuv[2]=Max(Increment*Max(theDeltaU2, aRangePart*aDeltaU2), pasuv[2]);
  else
    pasuv[2]=Max(Increment*theDeltaU2, pasuv[2]);

  if(!Precision::IsInfinite(aDeltaV2))
    pasuv[3]=Max(Increment*Max(theDeltaV2, aRangePart*aDeltaV2), pasuv[3]);
  else
    pasuv[3]=Max(Increment*theDeltaV2, pasuv[3]);

  const Standard_Real ResoU1tol = Adaptor3d_HSurfaceTool::UResolution(Caro1, tolconf);
  const Standard_Real ResoV1tol = Adaptor3d_HSurfaceTool::VResolution(Caro1, tolconf);
  const Standard_Real ResoU2tol = Adaptor3d_HSurfaceTool::UResolution(Caro2, tolconf);
  const Standard_Real ResoV2tol = Adaptor3d_HSurfaceTool::VResolution(Caro2, tolconf);

  myStepMin[0] = Max(myStepMin[0], 2.0*ResoU1tol);
  myStepMin[1] = Max(myStepMin[1], 2.0*ResoV1tol);
  myStepMin[2] = Max(myStepMin[2], 2.0*ResoU2tol);
  myStepMin[3] = Max(myStepMin[3], 2.0*ResoV2tol);

  for(Standard_Integer i  = 0; i < 4; i++)
  {
    pasuv[i]=Max(myStepMin[i], pasuv[i]);
  }
}

//=======================================================================
//function : IsParallel
//purpose  : Checks if theLine is parallel of some boundary of given
//            surface (it is determined by theCheckSurf1 flag). 
//            Parallelism assumes small oscillations (swing is less or 
//            equal than theToler).
//            Small lines (if first and last parameters in the Surface 
//            are almost equal) are classified as parallel (as same as 
//            any point can be considered as parallel of any line).
//=======================================================================
static void IsParallel(const Handle(IntSurf_LineOn2S)& theLine,
                       const Standard_Boolean theCheckSurf1,
                       const Standard_Real theToler,
                       Standard_Boolean& theIsUparallel,
                       Standard_Boolean& theIsVparallel)
{
  const Standard_Integer aNbPointsMAX = 23;

  theIsUparallel = theIsVparallel = Standard_True;

  const Standard_Integer aNbLinePnts = theLine->NbPoints();
  Standard_Integer aNbPoints = aNbLinePnts;
  if(aNbPoints > aNbPointsMAX)
  {
    aNbPoints = aNbPointsMAX;
  }
  else if(aNbPoints < 3)
  {
    //Here we cannot estimate parallelism.
    //Do all same as for small lines 
    return;
  }

  Standard_Real aStep = IntToReal(theLine->NbPoints()) / aNbPoints;
  Standard_Real aNPoint = 1.0;

  Standard_Real aUmin = RealLast(), aUmax = RealFirst(), aVmin = RealLast(), aVmax = RealFirst();
  for(Standard_Integer aNum = 1; aNum <= aNbPoints; aNum++, aNPoint += aStep)
  {
    // Fix possible "out of parameter" case.
    if (aNPoint > aNbLinePnts)
      aNPoint = aNbLinePnts;

    Standard_Real u, v;
    if(theCheckSurf1)
      theLine->Value(RealToInt(aNPoint)).ParametersOnS1(u, v);
    else
      theLine->Value(RealToInt(aNPoint)).ParametersOnS2(u, v);

    if(u < aUmin)
      aUmin = u;

    if(u > aUmax)
      aUmax = u;

    if(v < aVmin)
      aVmin = v;

    if(v > aVmax)
      aVmax = v;
  }

  theIsVparallel = ((aUmax - aUmin) < theToler);
  theIsUparallel = ((aVmax - aVmin) < theToler);
}

//==================================================================================
// function : IntWalk_PWalking::IntWalk_PWalking
// purpose  : 
//==================================================================================
IntWalk_PWalking::IntWalk_PWalking(const Handle(Adaptor3d_HSurface)& Caro1,
                                   const Handle(Adaptor3d_HSurface)& Caro2,
                                   const Standard_Real TolTangency,
                                   const Standard_Real Epsilon,
                                   const Standard_Real Deflection,
                                   const Standard_Real Increment ) 
                                   :

done(Standard_True),
close(Standard_False),
fleche(Deflection),
tolconf(Epsilon),
myTolTang(TolTangency),
sensCheminement(1),
myIntersectionOn2S(Caro1,Caro2,TolTangency),
STATIC_BLOCAGE_SUR_PAS_TROP_GRAND(0),
STATIC_PRECEDENT_INFLEXION(0)
{
  Standard_Real KELARG=20.;
  //
  pasMax=Increment*0.2; //-- June 25 99 after problems with precision 
  Um1 = Adaptor3d_HSurfaceTool::FirstUParameter(Caro1);
  Vm1 = Adaptor3d_HSurfaceTool::FirstVParameter(Caro1);
  UM1 = Adaptor3d_HSurfaceTool::LastUParameter(Caro1);
  VM1 = Adaptor3d_HSurfaceTool::LastVParameter(Caro1);

  Um2 = Adaptor3d_HSurfaceTool::FirstUParameter(Caro2);
  Vm2 = Adaptor3d_HSurfaceTool::FirstVParameter(Caro2);
  UM2 = Adaptor3d_HSurfaceTool::LastUParameter(Caro2);
  VM2 = Adaptor3d_HSurfaceTool::LastVParameter(Caro2);

  ResoU1 = Adaptor3d_HSurfaceTool::UResolution(Caro1,Precision::Confusion());
  ResoV1 = Adaptor3d_HSurfaceTool::VResolution(Caro1,Precision::Confusion());

  ResoU2 = Adaptor3d_HSurfaceTool::UResolution(Caro2,Precision::Confusion());
  ResoV2 = Adaptor3d_HSurfaceTool::VResolution(Caro2,Precision::Confusion());

  Standard_Real NEWRESO;
  Standard_Real MAXVAL;
  Standard_Real MAXVAL2;
  //
  MAXVAL  = Abs(Um1);  MAXVAL2 = Abs(UM1);
  if(MAXVAL2 > MAXVAL) MAXVAL = MAXVAL2;
  NEWRESO = ResoU1 * MAXVAL ;
  if(NEWRESO > ResoU1 &&NEWRESO<10) {    ResoU1 = NEWRESO;  }


  MAXVAL  = Abs(Um2);   MAXVAL2 = Abs(UM2);
  if(MAXVAL2 > MAXVAL) MAXVAL = MAXVAL2;
  NEWRESO = ResoU2 * MAXVAL ;
  if(NEWRESO > ResoU2 && NEWRESO<10) {     ResoU2 = NEWRESO;  }


  MAXVAL  = Abs(Vm1);  MAXVAL2 = Abs(VM1);
  if(MAXVAL2 > MAXVAL) MAXVAL = MAXVAL2;
  NEWRESO = ResoV1 * MAXVAL ;
  if(NEWRESO > ResoV1 && NEWRESO<10) {     ResoV1 = NEWRESO;  }


  MAXVAL  = Abs(Vm2);  MAXVAL2 = Abs(VM2);
  if(MAXVAL2 > MAXVAL) MAXVAL = MAXVAL2;
  NEWRESO = ResoV2 * MAXVAL ;
  if(NEWRESO > ResoV2 && NEWRESO<10) {     ResoV2 = NEWRESO;  }

  pasuv[0]=pasMax*Abs(UM1-Um1);
  pasuv[1]=pasMax*Abs(VM1-Vm1);
  pasuv[2]=pasMax*Abs(UM2-Um2);
  pasuv[3]=pasMax*Abs(VM2-Vm2);

  if(ResoU1>0.0001*pasuv[0]) ResoU1=0.00001*pasuv[0];
  if(ResoV1>0.0001*pasuv[1]) ResoV1=0.00001*pasuv[1];
  if(ResoU2>0.0001*pasuv[2]) ResoU2=0.00001*pasuv[2];
  if(ResoV2>0.0001*pasuv[3]) ResoV2=0.00001*pasuv[3];


  if(Adaptor3d_HSurfaceTool::IsUPeriodic(Caro1)==Standard_False) { 
    //UM1+=KELARG*pasuv[0];  Um1-=KELARG*pasuv[0];
  }
  else { 
    Standard_Real t = UM1-Um1; 
    if(t<Adaptor3d_HSurfaceTool::UPeriod(Caro1)) { 
      t=0.5*(Adaptor3d_HSurfaceTool::UPeriod(Caro1)-t);
      t=(t>KELARG*pasuv[0])? KELARG*pasuv[0] : t;
      UM1+=t;  Um1-=t;
    }
  }

  if(Adaptor3d_HSurfaceTool::IsVPeriodic(Caro1)==Standard_False) { 
    //VM1+=KELARG*pasuv[1];  Vm1-=KELARG*pasuv[1];
  }
  else { 
    Standard_Real t = VM1-Vm1; 
    if(t<Adaptor3d_HSurfaceTool::VPeriod(Caro1)) { 
      t=0.5*(Adaptor3d_HSurfaceTool::VPeriod(Caro1)-t);
      t=(t>KELARG*pasuv[1])? KELARG*pasuv[1] : t;
      VM1+=t;  Vm1-=t;
    }
  }

  if(Adaptor3d_HSurfaceTool::IsUPeriodic(Caro2)==Standard_False) { 
    //UM2+=KELARG*pasuv[2];  Um2-=KELARG*pasuv[2];
  }
  else { 
    Standard_Real t = UM2-Um2; 
    if(t<Adaptor3d_HSurfaceTool::UPeriod(Caro2)) { 
      t=0.5*(Adaptor3d_HSurfaceTool::UPeriod(Caro2)-t);
      t=(t>KELARG*pasuv[2])? KELARG*pasuv[2] : t;
      UM2+=t;  Um2-=t;
    }
  }

  if(Adaptor3d_HSurfaceTool::IsVPeriodic(Caro2)==Standard_False) {   
    //VM2+=KELARG*pasuv[3];  Vm2-=KELARG*pasuv[3];
  }
  else { 
    Standard_Real t = VM2-Vm2; 
    if(t<Adaptor3d_HSurfaceTool::VPeriod(Caro2)) { 
      t=0.5*(Adaptor3d_HSurfaceTool::VPeriod(Caro2)-t);
      t=(t>KELARG*pasuv[3])? KELARG*pasuv[3] : t;
      VM2+=t;  Vm2-=t;
    }
  }

  myStepMin[0] = 100.0*ResoU1;
  myStepMin[1] = 100.0*ResoV1;
  myStepMin[2] = 100.0*ResoU2;
  myStepMin[3] = 100.0*ResoV2;

  //-- ComputePasInit(pasuv,Um1,UM1,Vm1,VM1,Um2,UM2,Vm2,VM2,Caro1,Caro2);

  for (Standard_Integer i = 0; i<=3;i++) {
    if(pasuv[i]>10) 
      pasuv[i] = 10; 
    pasInit[i] = pasSav[i] = pasuv[i]; 
  }


}
//==================================================================================
// function : IntWalk_PWalking
// purpose  : 
//==================================================================================
IntWalk_PWalking::IntWalk_PWalking(const Handle(Adaptor3d_HSurface)& Caro1,
                                   const Handle(Adaptor3d_HSurface)& Caro2,
                                   const Standard_Real TolTangency,
                                   const Standard_Real Epsilon,
                                   const Standard_Real Deflection,
                                   const Standard_Real Increment, 
                                   const Standard_Real U1,
                                   const Standard_Real V1,
                                   const Standard_Real U2, 
                                   const Standard_Real V2)
                                   :

done(Standard_True),
close(Standard_False),
fleche(Deflection),
tolconf(Epsilon),
myTolTang(TolTangency),
sensCheminement(1),
myIntersectionOn2S(Caro1,Caro2,TolTangency),
STATIC_BLOCAGE_SUR_PAS_TROP_GRAND(0),
STATIC_PRECEDENT_INFLEXION(0)
{
  Standard_Real KELARG=20.;
  //
  pasMax=Increment*0.2; //-- June 25 99 after problems with precision 
  //
  Um1 = Adaptor3d_HSurfaceTool::FirstUParameter(Caro1);
  Vm1 = Adaptor3d_HSurfaceTool::FirstVParameter(Caro1);
  UM1 = Adaptor3d_HSurfaceTool::LastUParameter(Caro1);
  VM1 = Adaptor3d_HSurfaceTool::LastVParameter(Caro1);

  Um2 = Adaptor3d_HSurfaceTool::FirstUParameter(Caro2);
  Vm2 = Adaptor3d_HSurfaceTool::FirstVParameter(Caro2);
  UM2 = Adaptor3d_HSurfaceTool::LastUParameter(Caro2);
  VM2 = Adaptor3d_HSurfaceTool::LastVParameter(Caro2);

  ResoU1 = Adaptor3d_HSurfaceTool::UResolution(Caro1,Precision::Confusion());
  ResoV1 = Adaptor3d_HSurfaceTool::VResolution(Caro1,Precision::Confusion());

  ResoU2 = Adaptor3d_HSurfaceTool::UResolution(Caro2,Precision::Confusion());
  ResoV2 = Adaptor3d_HSurfaceTool::VResolution(Caro2,Precision::Confusion());
  //
  Standard_Real NEWRESO, MAXVAL, MAXVAL2;
  //
  MAXVAL  = Abs(Um1);  
  MAXVAL2 = Abs(UM1);
  if(MAXVAL2 > MAXVAL) {
    MAXVAL = MAXVAL2;
  }
  NEWRESO = ResoU1 * MAXVAL ;
  if(NEWRESO > ResoU1) {
    ResoU1 = NEWRESO;  
  }
  //
  MAXVAL  = Abs(Um2);   
  MAXVAL2 = Abs(UM2);
  if(MAXVAL2 > MAXVAL){
    MAXVAL = MAXVAL2;
  }  
  NEWRESO = ResoU2 * MAXVAL ;
  if(NEWRESO > ResoU2) {
    ResoU2 = NEWRESO;  
  }
  //
  MAXVAL  = Abs(Vm1);  
  MAXVAL2 = Abs(VM1);
  if(MAXVAL2 > MAXVAL) {
    MAXVAL = MAXVAL2;
  }
  NEWRESO = ResoV1 * MAXVAL ;
  if(NEWRESO > ResoV1) {    
    ResoV1 = NEWRESO; 
  }
  //
  MAXVAL  = Abs(Vm2);  
  MAXVAL2 = Abs(VM2);
  if(MAXVAL2 > MAXVAL){
    MAXVAL = MAXVAL2;
  }  
  NEWRESO = ResoV2 * MAXVAL ;
  if(NEWRESO > ResoV2) {  
    ResoV2 = NEWRESO;
  }
  //
  pasuv[0]=pasMax*Abs(UM1-Um1);
  pasuv[1]=pasMax*Abs(VM1-Vm1);
  pasuv[2]=pasMax*Abs(UM2-Um2);
  pasuv[3]=pasMax*Abs(VM2-Vm2);
  //
  if(Adaptor3d_HSurfaceTool::IsUPeriodic(Caro1)==Standard_False) { 
    UM1+=KELARG*pasuv[0];  
    Um1-=KELARG*pasuv[0];
  }
  else { 
    Standard_Real t = UM1-Um1; 
    if(t<Adaptor3d_HSurfaceTool::UPeriod(Caro1)) { 
      t=0.5*(Adaptor3d_HSurfaceTool::UPeriod(Caro1)-t);
      t=(t>KELARG*pasuv[0])? KELARG*pasuv[0] : t;
      UM1+=t;  
      Um1-=t;
    }
  }
  //
  if(Adaptor3d_HSurfaceTool::IsVPeriodic(Caro1)==Standard_False) { 
    VM1+=KELARG*pasuv[1];
    Vm1-=KELARG*pasuv[1];
  }
  else { 
    Standard_Real t = VM1-Vm1; 
    if(t<Adaptor3d_HSurfaceTool::VPeriod(Caro1)) { 
      t=0.5*(Adaptor3d_HSurfaceTool::VPeriod(Caro1)-t);
      t=(t>KELARG*pasuv[1])? KELARG*pasuv[1] : t;
      VM1+=t;  Vm1-=t;
    }
  }
  //
  if(Adaptor3d_HSurfaceTool::IsUPeriodic(Caro2)==Standard_False) { 
    UM2+=KELARG*pasuv[2];  
    Um2-=KELARG*pasuv[2];
  }
  else { 
    Standard_Real t = UM2-Um2; 
    if(t<Adaptor3d_HSurfaceTool::UPeriod(Caro2)) { 
      t=0.5*(Adaptor3d_HSurfaceTool::UPeriod(Caro2)-t);
      t=(t>KELARG*pasuv[2])? KELARG*pasuv[2] : t;
      UM2+=t;  
      Um2-=t;
    }
  }

  if(Adaptor3d_HSurfaceTool::IsVPeriodic(Caro2)==Standard_False) {   
    VM2+=KELARG*pasuv[3];  
    Vm2-=KELARG*pasuv[3];
  }
  else { 
    Standard_Real t = VM2-Vm2; 
    if(t<Adaptor3d_HSurfaceTool::VPeriod(Caro2)) { 
      t=0.5*(Adaptor3d_HSurfaceTool::VPeriod(Caro2)-t);
      t=(t>KELARG*pasuv[3])? KELARG*pasuv[3] : t;
      VM2+=t;  
      Vm2-=t;
    }
  }
  //-- ComputePasInit(pasuv,Um1,UM1,Vm1,VM1,Um2,UM2,Vm2,VM2,Caro1,Caro2);

  for (Standard_Integer i = 0; i<=3;i++) {
    pasInit[i] = pasSav[i] = pasuv[i]; 
  }  

  if(ResoU1>0.0001*pasuv[0]) ResoU1=0.00001*pasuv[0];
  if(ResoV1>0.0001*pasuv[1]) ResoV1=0.00001*pasuv[1];
  if(ResoU2>0.0001*pasuv[2]) ResoU2=0.00001*pasuv[2];
  if(ResoV2>0.0001*pasuv[3]) ResoV2=0.00001*pasuv[3];
  
  myStepMin[0] = 100.0*ResoU1;
  myStepMin[1] = 100.0*ResoV1;
  myStepMin[2] = 100.0*ResoU2;
  myStepMin[3] = 100.0*ResoV2;

  //
  TColStd_Array1OfReal Par(1,4);
  Par(1) = U1;
  Par(2) = V1;
  Par(3) = U2;
  Par(4) = V2;
  Perform(Par);
}

//==================================================================================
// function : PerformFirstPoint
// purpose  : 
//==================================================================================
Standard_Boolean IntWalk_PWalking::PerformFirstPoint  (const TColStd_Array1OfReal& ParDep,
                                                       IntSurf_PntOn2S& FirstPoint)   
{
  sensCheminement = 1;
  close = Standard_False;
  //
  Standard_Integer i;
  TColStd_Array1OfReal Param(1,4);
  //
  for (i=1; i<=4; ++i) {
    Param(i) = ParDep(i);
  }
  //-- calculate the first solution point
  math_FunctionSetRoot  Rsnld(myIntersectionOn2S.Function());
  //
  myIntersectionOn2S.Perform(Param,Rsnld);
  if (!myIntersectionOn2S.IsDone())  { 
    return Standard_False;
  }

  if (myIntersectionOn2S.IsEmpty()) {
    return Standard_False;
  }

  FirstPoint = myIntersectionOn2S.Point();
  return Standard_True;
}
//==================================================================================
// function : Perform
// purpose  : 
//==================================================================================
void IntWalk_PWalking::Perform(const TColStd_Array1OfReal& ParDep)    
{
  Perform(ParDep,Um1,Vm1,Um2,Vm2,UM1,VM1,UM2,VM2);
}

//=======================================================================
//function : SQDistPointSurface
//purpose  : Returns square distance between thePnt and theSurf.
//            (theU0, theV0) is initial point for extrema
//=======================================================================
static Standard_Real SQDistPointSurface(const gp_Pnt &thePnt,
                                        const Adaptor3d_Surface& theSurf,
                                        const Standard_Real theU0,
                                        const Standard_Real theV0)
{
  Extrema_GenLocateExtPS aExtPS(theSurf);
  aExtPS.Perform(thePnt, theU0, theV0);

  if(!aExtPS.IsDone())
    return RealLast();
  
  return aExtPS.SquareDistance();
}

//==================================================================================
// function : IsTangentExtCheck
// purpose  : Additional check if the surfaces are tangent.
//            Checks if any point in one surface lie in another surface
//            (with given tolerance)
//==================================================================================
static Standard_Boolean IsTangentExtCheck(const Handle(Adaptor3d_HSurface)& theSurf1,
                                          const Handle(Adaptor3d_HSurface)& theSurf2,
                                          const Standard_Real theU10,
                                          const Standard_Real theV10,
                                          const Standard_Real theU20,
                                          const Standard_Real theV20,
                                          const Standard_Real theToler,
                                          const Standard_Real theArrStep[])
{
  {
    gp_Pnt aPt;
    gp_Vec aDu1, aDv1, aDu2, aDv2;
    theSurf1->D1(theU10, theV10, aPt, aDu1, aDv1);
    theSurf2->D1(theU20, theV20, aPt, aDu2, aDv2);

    const gp_Vec  aN1(aDu1.Crossed(aDv1)),
                  aN2(aDu2.Crossed(aDv2));
    const Standard_Real aDP = aN1.Dot(aN2),
                        aSQ1 = aN1.SquareMagnitude(),
                        aSQ2 = aN2.SquareMagnitude();

    if((aSQ1 < RealSmall()) || (aSQ2 < RealSmall()))
      return Standard_True; //Tangent

    if(aDP*aDP < 0.9998*aSQ1*aSQ2)
    {//cos(ang N1<->N2) < 0.9999
      return Standard_False; //Not tangent
    }
  }

  //For two faces (2^2 = 4)
  const Standard_Real aSQToler = 4.0*theToler*theToler;
  const Standard_Integer aNbItems = 4;
  const Standard_Real aParUS1[aNbItems] = { theU10 + theArrStep[0],
                                            theU10 - theArrStep[0],
                                            theU10, theU10};
  const Standard_Real aParVS1[aNbItems] = { theV10, theV10,
                                            theV10 + theArrStep[1],
                                            theV10 - theArrStep[1]};
  const Standard_Real aParUS2[aNbItems] = { theU20 + theArrStep[2],
                                            theU20 - theArrStep[2],
                                            theU20, theU20};
  const Standard_Real aParVS2[aNbItems] = { theV20, theV20,
                                            theV20 + theArrStep[3],
                                            theV20 - theArrStep[3]};

  for(Standard_Integer i = 0; i < aNbItems; i++)
  {
    gp_Pnt aP(theSurf1->Value(aParUS1[i], aParVS1[i]));
    const Standard_Real aSqDist = SQDistPointSurface(aP, theSurf2->Surface(), theU20, theV20);
    if(aSqDist > aSQToler)
      return Standard_False;
  }

  for(Standard_Integer i = 0; i < aNbItems; i++)
  {
    gp_Pnt aP(theSurf2->Value(aParUS2[i], aParVS2[i]));
    const Standard_Real aSqDist = SQDistPointSurface(aP, theSurf1->Surface(), theU10, theV10);
    if(aSqDist > aSQToler)
      return Standard_False;
  }

  return Standard_True;
}

//==================================================================================
// function : Perform
// purpose  : 
//==================================================================================
void IntWalk_PWalking::Perform(const TColStd_Array1OfReal& ParDep,
                               const Standard_Real u1min,
                               const Standard_Real v1min,
                               const Standard_Real u2min,
                               const Standard_Real v2min,
                               const Standard_Real u1max,
                               const Standard_Real v1max,
                               const Standard_Real u2max,
                               const Standard_Real v2max)
{
  const Standard_Real aSQDistMax = 1.0e-14;
  //xf

  Standard_Integer NbPasOKConseq=0;
  TColStd_Array1OfReal Param(1,4);
  IntImp_ConstIsoparametric ChoixIso;
  //xt
  //
  done = Standard_False;
  //
  // Caro1 and Caro2
  const Handle(Adaptor3d_HSurface)& Caro1 =myIntersectionOn2S.Function().AuxillarSurface1();
  const Handle(Adaptor3d_HSurface)& Caro2 =myIntersectionOn2S.Function().AuxillarSurface2();
  //
  const Standard_Real UFirst1 = Adaptor3d_HSurfaceTool::FirstUParameter(Caro1);
  const Standard_Real VFirst1 = Adaptor3d_HSurfaceTool::FirstVParameter(Caro1);
  const Standard_Real ULast1  = Adaptor3d_HSurfaceTool::LastUParameter (Caro1);
  const Standard_Real VLast1  = Adaptor3d_HSurfaceTool::LastVParameter (Caro1);

  const Standard_Real UFirst2 = Adaptor3d_HSurfaceTool::FirstUParameter(Caro2);
  const Standard_Real VFirst2 = Adaptor3d_HSurfaceTool::FirstVParameter(Caro2);
  const Standard_Real ULast2  = Adaptor3d_HSurfaceTool::LastUParameter (Caro2);
  const Standard_Real VLast2  = Adaptor3d_HSurfaceTool::LastVParameter (Caro2);
  //
  ComputePasInit(u1max - u1min,v1max - v1min,u2max - u2min,v2max - v2min);

  for (Standard_Integer i=0; i<4; ++i)
  {
    if(pasuv[i]>10)
    {
      pasuv[i] = 10;
    }

    pasInit[i] = pasSav[i] = pasuv[i]; 
  }
  //
  line = new IntSurf_LineOn2S ();
  //
  for (Standard_Integer i=1; i<=4; ++i)
  {
    Param(i)=ParDep(i);
  }
  //-- reproduce steps uv connected to surfaces Caro1 and Caro2
  //-- pasuv[] and pasSav[] are modified during the marching
  for(Standard_Integer i = 0; i < 4; ++i)
  {
    pasSav[i] = pasuv[i] = pasInit[i];
  }

  //-- calculate the first solution point
  math_FunctionSetRoot  Rsnld(myIntersectionOn2S.Function());
  //
  ChoixIso = myIntersectionOn2S.Perform(Param,Rsnld);
  if (!myIntersectionOn2S.IsDone())
  {
    return;
  }

  //
  if (myIntersectionOn2S.IsEmpty())
  {
    return;
  }
  //
  if(myIntersectionOn2S.IsTangent())
  {
    return;
  }
  //
  Standard_Boolean Arrive, DejaReparti;
  const Standard_Integer RejectIndexMAX = 250000;
  Standard_Integer IncKey, RejectIndex;
  gp_Pnt pf,pl;
  //
  DejaReparti = Standard_False;
  IncKey = 0;
  RejectIndex = 0;
  //
  previousPoint = myIntersectionOn2S.Point();
  previoustg = Standard_False;
  previousd  = myIntersectionOn2S.Direction();
  previousd1 = myIntersectionOn2S.DirectionOnS1();
  previousd2 = myIntersectionOn2S.DirectionOnS2();
  indextg = 1;
  tgdir   = previousd;
  firstd1 = previousd1;
  firstd2 = previousd2;
  tgfirst = tglast = Standard_False;
  choixIsoSav  =  ChoixIso;
  //------------------------------------------------------------
  //-- Test if the first point of marching corresponds 
  //-- to a point on borders. 
  //-- In this case, DejaReparti is initialized as True
  //-- 
  pf = previousPoint.Value();
  Standard_Boolean bTestFirstPoint = Standard_True;

  previousPoint.Parameters(Param(1),Param(2),Param(3),Param(4));

  if(IsTangentExtCheck(Caro1, Caro2, Param(1), Param(2), Param(3), Param(4), myTolTang, pasuv))
    return;

  AddAPoint(line,previousPoint);
  //
  IntWalk_StatusDeflection aStatus = IntWalk_OK, aPrevStatus = IntWalk_OK;
  Standard_Boolean NoTestDeflection = Standard_False;
  Standard_Real SvParam[4], f;
  Standard_Integer LevelOfEmptyInmyIntersectionOn2S=0;
  Standard_Integer LevelOfPointConfondu = 0; 
  Standard_Integer LevelOfIterWithoutAppend = -1;
  //

  const Standard_Real aTol[4] = { Epsilon(u1max - u1min),
                                  Epsilon(v1max - v1min),
                                  Epsilon(u2max - u2min),
                                  Epsilon(v2max - v2min)};
  Arrive = Standard_False;
  while(!Arrive) //010
  {
    aPrevStatus = aStatus;

    LevelOfIterWithoutAppend++;
    if(LevelOfIterWithoutAppend>20)
    {
      Arrive = Standard_True; 
      if(DejaReparti) {
        break;
      }
      RepartirOuDiviser(DejaReparti,ChoixIso,Arrive);
      LevelOfIterWithoutAppend = 0;
    }
    //
    // compute f
    f = 0.;
    switch (ChoixIso) { 
      case IntImp_UIsoparametricOnCaro1: f = Abs(previousd1.X()); break;
      case IntImp_VIsoparametricOnCaro1: f = Abs(previousd1.Y()); break;
      case IntImp_UIsoparametricOnCaro2: f = Abs(previousd2.X()); break;
      case IntImp_VIsoparametricOnCaro2: f = Abs(previousd2.Y()); break;
      default:break;
    }
    //
    if(f<0.1) {
      f=0.1;
    }
    //
    previousPoint.Parameters(Param(1),Param(2),Param(3),Param(4));
    //
    //--ofv.begin
    Standard_Real aIncKey, aEps, dP1, dP2, dP3, dP4;
    //
    dP1 = sensCheminement * pasuv[0] * previousd1.X() /f;
    dP2 = sensCheminement * pasuv[1] * previousd1.Y() /f;
    dP3 = sensCheminement * pasuv[2] * previousd2.X() /f; 
    dP4 = sensCheminement * pasuv[3] * previousd2.Y() /f;
    //
    aIncKey=5.*(Standard_Real)IncKey;
    aEps=1.e-7;
    if(ChoixIso == IntImp_UIsoparametricOnCaro1 && Abs(dP1) < aEps)
    {
      dP1 *= aIncKey;
    }

    if(ChoixIso == IntImp_VIsoparametricOnCaro1 && Abs(dP2) < aEps)
    {
      dP2 *= aIncKey;
    }

    if(ChoixIso == IntImp_UIsoparametricOnCaro2 && Abs(dP3) < aEps)
    {
      dP3 *= aIncKey;
    }

    if(ChoixIso == IntImp_VIsoparametricOnCaro2 && Abs(dP4) < aEps)
    {
      dP4 *= aIncKey;
    }
    //--ofv.end
    //
    Param(1) += dP1;
    Param(2) += dP2;
    Param(3) += dP3; 
    Param(4) += dP4;
    //==========================
    SvParam[0]=Param(1); 
    SvParam[1]=Param(2);
    SvParam[2]=Param(3);
    SvParam[3]=Param(4);
    //
    Standard_Integer aTryNumber = 0;
    Standard_Real    isBadPoint = Standard_False;
    IntImp_ConstIsoparametric aBestIso = ChoixIso;
    do
    {
      isBadPoint = Standard_False;

      ChoixIso= myIntersectionOn2S.Perform(Param, Rsnld, aBestIso);

      if (myIntersectionOn2S.IsDone() && !myIntersectionOn2S.IsEmpty())
      {
        //If we go along any surface boundary then it is possible 
        //to find "outboundaried" point.
        //Nevertheless, if this deflection is quite small, we will be
        //able to adjust this point to the boundary.

        Standard_Real aNewPnt[4], anAbsParamDist[4];
        myIntersectionOn2S.Point().Parameters(aNewPnt[0], aNewPnt[1], aNewPnt[2], aNewPnt[3]);
        const Standard_Real aParMin[4] = {u1min, v1min, u2min, v2min};
        const Standard_Real aParMax[4] = {u1max, v1max, u2max, v2max};

        for(Standard_Integer i = 0; i < 4; i++)
        {
          if(Abs(aNewPnt[i] - aParMin[i]) < aTol[i])
            aNewPnt[i] = aParMin[i];
          else if(Abs(aNewPnt[i] - aParMax[i]) < aTol[i])
            aNewPnt[i] = aParMax[i];
        }

        if (aNewPnt[0] < u1min || aNewPnt[0] > u1max ||
            aNewPnt[1] < v1min || aNewPnt[1] > v1max ||
            aNewPnt[2] < u2min || aNewPnt[2] > u2max ||
            aNewPnt[3] < v2min || aNewPnt[3] > v2max)
        {
          break; // Out of borders, handle this later.
        }

        myIntersectionOn2S.ChangePoint().SetValue(aNewPnt[0],
                                                  aNewPnt[1],
                                                  aNewPnt[2],
                                                  aNewPnt[3]);

        anAbsParamDist[0] = Abs(Param(1) - dP1 - aNewPnt[0]);
        anAbsParamDist[1] = Abs(Param(2) - dP2 - aNewPnt[1]);
        anAbsParamDist[2] = Abs(Param(3) - dP3 - aNewPnt[2]);
        anAbsParamDist[3] = Abs(Param(4) - dP4 - aNewPnt[3]);
        if (anAbsParamDist[0] < ResoU1 &&
            anAbsParamDist[1] < ResoV1 &&
            anAbsParamDist[2] < ResoU2 &&
            anAbsParamDist[3] < ResoV2 &&
            aStatus != IntWalk_PasTropGrand)
        {
          isBadPoint = Standard_True;
          aBestIso = IntImp_ConstIsoparametric((aBestIso + 1) % 4);
        }
      }
    } while (isBadPoint && ++aTryNumber <= 4);
    //
    if (!myIntersectionOn2S.IsDone())
    {
      //end of line, division
      Arrive = Standard_False;
      Param(1)=SvParam[0]; 
      Param(2)=SvParam[1]; 
      Param(3)=SvParam[2];
      Param(4)=SvParam[3];
      RepartirOuDiviser(DejaReparti, ChoixIso, Arrive);
    }
    else  //009 
    {
      //== Calculation of exact point from Param(.) is possible
      if (myIntersectionOn2S.IsEmpty())
      {
        Standard_Real u1,v1,u2,v2;
        previousPoint.Parameters(u1,v1,u2,v2);
        //
        Arrive = Standard_False;
        if(u1<UFirst1 || u1>ULast1)
        {
          Arrive=Standard_True;
        }	

        if(u2<UFirst2 || u2>ULast2)
        {
          Arrive=Standard_True;
        }

        if(v1<VFirst1 || v1>VLast1)
        {
          Arrive=Standard_True;
        }

        if(v2<VFirst2 || v2>VLast2)
        {
          Arrive=Standard_True;
        }

        RepartirOuDiviser(DejaReparti,ChoixIso,Arrive);
        LevelOfEmptyInmyIntersectionOn2S++;
        //
        if(LevelOfEmptyInmyIntersectionOn2S>10)
        {
          pasuv[0]=pasSav[0]; 
          pasuv[1]=pasSav[1]; 
          pasuv[2]=pasSav[2]; 
          pasuv[3]=pasSav[3];
        }
      }
      else //008
      {
        //============================================================
        //== A point has been found :  T E S T   D E F L E C T I O N 
        //============================================================
        if(NoTestDeflection)
        {
          NoTestDeflection = Standard_False;
        }		  
        else
        {
          if(--LevelOfEmptyInmyIntersectionOn2S<=0)
          {
            LevelOfEmptyInmyIntersectionOn2S=0;
            if(LevelOfIterWithoutAppend < 10)
            {
              aStatus = TestDeflection(ChoixIso);
            }			
            else
            {
              pasuv[0]*=0.5; 
              pasuv[1]*=0.5; 
              pasuv[2]*=0.5; 
              pasuv[3]*=0.5;
            }
          }
        }

        //============================================================
        //==       T r a i t e m e n t   s u r   S t a t u s        ==
        //============================================================
        if(LevelOfPointConfondu > 5)
        { 
          aStatus = IntWalk_ArretSurPoint;
          LevelOfPointConfondu = 0;  
        }
        //
        if(aStatus==IntWalk_OK)
        { 
          NbPasOKConseq++;
          if(NbPasOKConseq >= 5)
          {
            NbPasOKConseq=0;
            Standard_Boolean pastroppetit;
            Standard_Real t;
            //
            do
            {
              pastroppetit=Standard_True;
              //
              if(pasuv[0]<pasInit[0])
              {
                t = (pasInit[0]-pasuv[0])*0.25;
                if(t>0.1*pasInit[0])
                {
                  t=0.1*pasuv[0];
                }

                pasuv[0]+=t; 
                pastroppetit=Standard_False;
              }

              if(pasuv[1]<pasInit[1])
              {
                t = (pasInit[1]-pasuv[1])*0.25;
                if(t>0.1*pasInit[1]) {
                  t=0.1*pasuv[1];
                }		

                pasuv[1]+=t; 
                pastroppetit=Standard_False;
              }

              if(pasuv[2]<pasInit[2])
              {
                t = (pasInit[2]-pasuv[2])*0.25;
                if(t>0.1*pasInit[2])
                {
                  t=0.1*pasuv[2];
                }

                pasuv[2]+=t; 
                pastroppetit=Standard_False;
              }

              if(pasuv[3]<pasInit[3])
              {
                t = (pasInit[3]-pasuv[3])*0.25;
                if(t>0.1*pasInit[3]) {
                  t=0.1*pasuv[3];
                }
                pasuv[3]+=t; 
                pastroppetit=Standard_False;
              }
              if(pastroppetit)
              {
                if(pasMax<0.1)
                {
                  pasMax*=1.1;
                  pasInit[0]*=1.1; 
                  pasInit[1]*=1.1; 
                  pasInit[2]*=1.1; 
                  pasInit[3]*=1.1; 
                }
                else
                {
                  pastroppetit=Standard_False;
                }
              }
            }
            while(pastroppetit);
          }
        }//aStatus==IntWalk_OK
        else
          NbPasOKConseq=0;

        //
        switch(aStatus)//007
        {
        case IntWalk_ArretSurPointPrecedent:
          {
            Arrive = Standard_False;
            RepartirOuDiviser(DejaReparti, ChoixIso, Arrive);
            break;
          }
        case IntWalk_PasTropGrand:
          {
            Param(1)=SvParam[0];
            Param(2)=SvParam[1]; 
            Param(3)=SvParam[2]; 
            Param(4)=SvParam[3];

            if(LevelOfIterWithoutAppend > 5)
            {
              for (Standard_Integer i = 0; i < 4; i++)
              {
                if (pasSav[i] > pasInit[i])
                  continue;

                const Standard_Real aDelta = (pasInit[i]-pasSav[i])*0.25;

                if(aDelta > Epsilon(pasInit[i]))
                {
                  pasInit[i] -= aDelta;
                  LevelOfIterWithoutAppend=0;
                }
              }
            }

            break;
          }
        case IntWalk_PointConfondu:
          {
            LevelOfPointConfondu++;

            if(LevelOfPointConfondu>5)
            {
              Standard_Boolean pastroppetit;
              //
              do
              {
                pastroppetit=Standard_True;

                for(Standard_Integer i = 0; i < 4; i++)
                {
                  if(pasuv[i]<pasInit[i])
                  {
                    pasuv[i]+=(pasInit[i]-pasuv[i])*0.25;
                    pastroppetit=Standard_False;
                  }
                }

                if(pastroppetit)
                {
                  if(pasMax<0.1)
                  {
                    pasMax*=1.1;
                    pasInit[0]*=1.1;
                    pasInit[1]*=1.1;
                    pasInit[2]*=1.1;
                    pasInit[3]*=1.1; 
                  }
                  else
                  {
                    pastroppetit=Standard_False;
                  }
                }
              }
              while(pastroppetit);
            }

            break;
          }
        case IntWalk_StepTooSmall:
          {
            Standard_Boolean hasStepBeenIncreased = Standard_False;

            for(Standard_Integer i = 0; i < 4; i++)
            {
              const Standard_Real aNewStep = Min(1.5*pasuv[i], pasInit[i]);
              if(aNewStep > pasuv[i])
              {
                pasuv[i] = aNewStep;
                hasStepBeenIncreased = Standard_True;
              }
            }

            if(hasStepBeenIncreased)
            {
              Param(1)=SvParam[0];
              Param(2)=SvParam[1];
              Param(3)=SvParam[2];
              Param(4)=SvParam[3];

              // In order to avoid cyclic changes
              // (PasTropGrand --> Decrease step --> 
              // StepTooSmall --> Increase step --> PasTropGrand...)
              // nullify LevelOfIterWithoutAppend only if the condition
              // is satisfied:
              if (aPrevStatus != IntWalk_PasTropGrand)
                LevelOfIterWithoutAppend = 0;

              break;
            }
          }
        case IntWalk_OK:
        case IntWalk_ArretSurPoint://006
          {
            //=======================================================
            //== Stop Test t   :  Frame on Param(.)     ==
            //=======================================================
            //xft arrive here
            Arrive = TestArret(DejaReparti,Param,ChoixIso); 
            // JMB 30th December 1999. 
            // Some statement below should not be put in comment because they are useful.
            // See grid CTO 909 A1 which infinitely loops 
            if(Arrive==Standard_False && aStatus==IntWalk_ArretSurPoint)
            {
              Arrive=Standard_True;
#ifdef OCCT_DEBUG
              cout << "IntWalk_PWalking_1.gxx: Problems with intersection"<<endl;
#endif
            }

            if(Arrive)
            {
              NbPasOKConseq = -10;
            }

            if(!Arrive)//005
            {
              //=====================================================
              //== Param(.) is in the limits                       ==
              //==  and does not end a closed  line                ==
              //=====================================================
              //== Check on the current point of myInters
              Standard_Boolean pointisvalid = Standard_False;
              {
                Standard_Real u1,v1,u2,v2; 
                myIntersectionOn2S.Point().Parameters(u1,v1,u2,v2);

                //
                if(u1 <= UM1  && u2 <= UM2 && v1 <= VM1 && 
                  v2 <= VM2  && u1 >= Um1 && u2 >= Um2 &&
                  v1 >= Vm1  && v2 >= Vm2)
                {
                  pointisvalid=Standard_True;
                }
              }

              //
              if(pointisvalid)
              {
                previousPoint = myIntersectionOn2S.Point();
                previoustg = myIntersectionOn2S.IsTangent();

                if(!previoustg)
                {
                  previousd  = myIntersectionOn2S.Direction();
                  previousd1 = myIntersectionOn2S.DirectionOnS1();
                  previousd2 = myIntersectionOn2S.DirectionOnS2();
                }
                //=====================================================
                //== Check on the previous Point
                {
                  Standard_Real u1,v1,u2,v2;
                  previousPoint.Parameters(u1,v1,u2,v2); 
                  if( u1 <= UM1  && u2 <= UM2 && v1 <= VM1 &&
                    v2 <= VM2  && u1 >= Um1 && u2 >= Um2 &&
                    v1 >= Vm1  && v2 >= Vm2)
                  {
                    pl = previousPoint.Value();
                    if(bTestFirstPoint)
                    {
                      if(pf.SquareDistance(pl) < aSQDistMax)
                      {
                        IncKey++;
                        if(IncKey == 5000)
                          return;
                        else
                          continue;
                      }
                      else
                      {
                        bTestFirstPoint = Standard_False;
                      }
                    }
                    //
                    AddAPoint(line,previousPoint);
                    RejectIndex++;

                    if(RejectIndex >= RejectIndexMAX)
                    {
                      Arrive = Standard_True;
                      break;
                    }

                    //
                    LevelOfIterWithoutAppend = 0;
                  }
                }
              }//pointisvalid
              //====================================================

              if (aStatus == IntWalk_ArretSurPoint)
              {
                RepartirOuDiviser(DejaReparti,ChoixIso,Arrive);
              }
              else
              {
                if (line->NbPoints() == 2)
                {
                  pasSav[0] = pasuv[0];
                  pasSav[1] = pasuv[1];
                  pasSav[2] = pasuv[2];
                  pasSav[3] = pasuv[3];
                }
              }
            }//005 if(!Arrive)
            else  //004
            {
              if(close)
              {
                //================= la ligne est fermee ===============
                AddAPoint(line,line->Value(1)); //ligne fermee
                LevelOfIterWithoutAppend=0;
              }
              else    //$$$
              {
                //====================================================
                //== Param was not in the limits (was reframed)
                //====================================================
                Standard_Boolean bPrevNotTangent = !previoustg || !myIntersectionOn2S.IsTangent();

                IntImp_ConstIsoparametric SauvChoixIso = ChoixIso;
                ChoixIso = myIntersectionOn2S.Perform(Param,Rsnld,ChoixIso);
                //
                if(!myIntersectionOn2S.IsEmpty()) //002
                {
                  // mutially outpasses in the square or intersection in corner

                  if(TestArret(Standard_True,Param,ChoixIso))
                  {
                    NbPasOKConseq = -10;
                    ChoixIso = myIntersectionOn2S.Perform(Param,Rsnld,ChoixIso);

                    if(!myIntersectionOn2S.IsEmpty())
                    {
                      previousPoint = myIntersectionOn2S.Point();
                      previoustg = myIntersectionOn2S.IsTangent();

                      if (!previoustg)
                      {
                        previousd  = myIntersectionOn2S.Direction();
                        previousd1 = myIntersectionOn2S.DirectionOnS1();
                        previousd2 = myIntersectionOn2S.DirectionOnS2();
                      }

                      pl = previousPoint.Value();

                      if(bTestFirstPoint)
                      {
                        if(pf.SquareDistance(pl) < aSQDistMax)
                        {
                          IncKey++;
                          if(IncKey == 5000)
                            return;
                          else
                            continue;
                        }
                        else
                        {
                          bTestFirstPoint = Standard_False;
                        }
                      }
                      //
                      AddAPoint(line,previousPoint);
                      RejectIndex++;

                      if(RejectIndex >= RejectIndexMAX)
                      {
                        Arrive = Standard_True;
                        break;
                      }

                      //
                      LevelOfIterWithoutAppend=0;
                      RepartirOuDiviser(DejaReparti,ChoixIso,Arrive);
                    }
                    else
                    {
                      //fail framing divides the step
                      Arrive = Standard_False;
                      RepartirOuDiviser(DejaReparti,ChoixIso,Arrive);
                      NoTestDeflection = Standard_True;
                      ChoixIso = SauvChoixIso;
                    }
                  }//if(TestArret())
                  else
                  {
                    // save the last point
                    // to revert to it if the current point is out of bounds

                    IntSurf_PntOn2S previousPointSave = previousPoint;
                    Standard_Boolean previoustgSave   = previoustg;
                    gp_Dir previousdSave              = previousd;
                    gp_Dir2d previousd1Save           = previousd1;
                    gp_Dir2d previousd2Save           = previousd2;

                    previousPoint = myIntersectionOn2S.Point();
                    previoustg = myIntersectionOn2S.IsTangent();
                    Arrive = Standard_False;

                    if(!previoustg)
                    {
                      previousd  = myIntersectionOn2S.Direction();
                      previousd1 = myIntersectionOn2S.DirectionOnS1();
                      previousd2 = myIntersectionOn2S.DirectionOnS2();
                    }

                    //========================================
                    //== Check on PreviousPoint @@

                    {
                      Standard_Real u1,v1,u2,v2;
                      previousPoint.Parameters(u1,v1,u2,v2);

                      //To save initial 2d points
                      gp_Pnt2d ParamPntOnS1(Param(1), Param(2));
                      gp_Pnt2d ParamPntOnS2(Param(3), Param(4));

                      ///////////////////////////
                      Param(1) = u1;
                      Param(2) = v1;
                      Param(3) = u2;
                      Param(4) = v2;
                      //

                      //xf
                      Standard_Boolean bFlag1, bFlag2;
                      Standard_Real aTol2D=1.e-11;
                      //
                      bFlag1=u1 >= Um1-aTol2D && v1 >= Vm1-aTol2D && u1 <= UM1+aTol2D && v1 <= VM1+aTol2D;
                      bFlag2=u2 >= Um2-aTol2D && v2 >= Vm2-aTol2D && u2 <= UM2+aTol2D && v2 <= VM2+aTol2D;
                      if (bFlag1 && bFlag2)
                      {
                        if (line->NbPoints() > 1)
                        {
                          IntSurf_PntOn2S prevprevPoint = line->Value(line->NbPoints()-1);
                          Standard_Real ppU1, ppV1, ppU2, ppV2;
                          prevprevPoint.Parameters(ppU1, ppV1, ppU2, ppV2);
                          Standard_Real pU1, pV1, pU2, pV2;
                          previousPointSave.Parameters(pU1, pV1, pU2, pV2);
                          gp_Vec2d V1onS1(gp_Pnt2d(ppU1, ppV1), gp_Pnt2d(pU1, pV1));
                          gp_Vec2d V2onS1(gp_Pnt2d(pU1, pV1), gp_Pnt2d(u1, v1));
                          gp_Vec2d V1onS2(gp_Pnt2d(ppU2, ppV2), gp_Pnt2d(pU2, pV2));
                          gp_Vec2d V2onS2(gp_Pnt2d(pU2, pV2), gp_Pnt2d(u2, v2));

                          const Standard_Real aDot1 = V1onS1 * V2onS1;
                          const Standard_Real aDot2 = V1onS2 * V2onS2;

                          if ((aDot1 < 0.0) || (aDot2 < 0.0))
                          {
                            Arrive = Standard_True;
                            break;
                          }
                        }
                        /*
                        if(u1 <= UM1  && u2 <= UM2 && v1 <= VM1 &&
                        v2 <= VM2  && u1 >= Um1 && u2 >= Um2 &&
                        v1 >= Vm1  && v2 >= Vm2)  {
                        */			
                        //xt
                        pl = previousPoint.Value();

                        if(bTestFirstPoint)
                        {
                          if(pf.SquareDistance(pl) < aSQDistMax)
                          {
                            IncKey++;

                            if(IncKey == 5000)
                              return;
                            else
                              continue;
                          }
                          else
                          {
                            bTestFirstPoint = Standard_False;
                          }
                        }

                        //To avoid walking around the same point
                        //in the tangent zone near a border

                        if (previoustg)
                        {
                          //There are three consecutive points:
                          //previousPointSave -> ParamPnt -> curPnt.

                          Standard_Real prevU1, prevV1, prevU2, prevV2;
                          previousPointSave.Parameters(prevU1, prevV1, prevU2, prevV2);
                          gp_Pnt2d prevPntOnS1(prevU1, prevV1), prevPntOnS2(prevU2, prevV2);
                          gp_Pnt2d curPntOnS1(u1, v1), curPntOnS2(u2, v2);
                          gp_Vec2d PrevToParamOnS1(prevPntOnS1, ParamPntOnS1);
                          gp_Vec2d PrevToCurOnS1(prevPntOnS1, curPntOnS1);
                          gp_Vec2d PrevToParamOnS2(prevPntOnS2, ParamPntOnS2);
                          gp_Vec2d PrevToCurOnS2(prevPntOnS2, curPntOnS2);
                          Standard_Real MaxAngle = 3*M_PI/4;
                          Standard_Real anAngleS1 = 0.0, anAngleS2 = 0.0;
                          const Standard_Real aSQMParS1 = PrevToParamOnS1.SquareMagnitude();
                          const Standard_Real aSQMParS2 = PrevToParamOnS2.SquareMagnitude();
                          const Standard_Real aSQMCurS1 = PrevToCurOnS1.SquareMagnitude();
                          const Standard_Real aSQMCurS2 = PrevToCurOnS2.SquareMagnitude();

                          if(aSQMCurS1 < gp::Resolution())
                          {
                            //We came back to the one of previos point.
                            //Therefore, we must break;

                            anAngleS1 = M_PI;
                          }
                          else if(aSQMParS1 < gp::Resolution())
                          {
                            //We are walking along tangent zone.
                            //It should be continued.
                            anAngleS1 = 0.0;
                          }
                          else
                          {
                            anAngleS1 = Abs(PrevToParamOnS1.Angle(PrevToCurOnS1));
                          }

                          if(aSQMCurS2 < gp::Resolution())
                          {
                            //We came back to the one of previos point.
                            //Therefore, we must break;

                            anAngleS2 = M_PI;
                          }
                          else if(aSQMParS2 < gp::Resolution())
                          {
                            //We are walking along tangent zone.
                            //It should be continued;
                            anAngleS2 = 0.0;
                          }
                          else
                          {
                            anAngleS2 = Abs(PrevToParamOnS2.Angle(PrevToCurOnS2));
                          }

                          if ((anAngleS1 > MaxAngle) && (anAngleS2 > MaxAngle))
                          {
                            Arrive = Standard_True;
                            break;
                          }

                          {
                            //Check singularity.
                            //I.e. check if we are walking along direction, which does not
                            //result in comming to any point (i.e. derivative 
                            //3D-intersection curve along this direction is equal to 0).
                            //A sphere with direction {dU=1, dV=0} from point
                            //(U=0, V=M_PI/2) can be considered as example for
                            //this case (we cannot find another 3D-point if we go thus).

                            //Direction chosen along 1st and 2nd surface correspondingly
                            const gp_Vec2d  aDirS1(prevPntOnS1, curPntOnS1),
                                            aDirS2(prevPntOnS2, curPntOnS2);

                            gp_Pnt aPtemp;
                            gp_Vec aDuS1, aDvS1, aDuS2, aDvS2;

                            myIntersectionOn2S.Function().AuxillarSurface1()->
                                  D1(curPntOnS1.X(), curPntOnS1.Y(), aPtemp, aDuS1, aDvS1);
                            myIntersectionOn2S.Function().AuxillarSurface2()->
                                  D1(curPntOnS2.X(), curPntOnS2.Y(), aPtemp, aDuS2, aDvS2);

                            //Derivative WLine along (it is vector-function indeed)
                            //directions chosen
                            //(https://en.wikipedia.org/wiki/Directional_derivative#Variation_using_only_direction_of_vector).
                            //F1 - on the 1st surface, F2 - on the 2nd surface.
                            //x, y, z - coordinates of derivative vector.
                            const Standard_Real aF1x =  aDuS1.X()*aDirS1.X() + 
                                                        aDvS1.X()*aDirS1.Y();
                            const Standard_Real aF1y =  aDuS1.Y()*aDirS1.X() +
                                                        aDvS1.Y()*aDirS1.Y();
                            const Standard_Real aF1z =  aDuS1.Z()*aDirS1.X() +
                                                        aDvS1.Z()*aDirS1.Y();
                            const Standard_Real aF2x =  aDuS2.X()*aDirS2.X() +
                                                        aDvS2.X()*aDirS2.Y();
                            const Standard_Real aF2y =  aDuS2.Y()*aDirS2.X() +
                                                        aDvS2.Y()*aDirS2.Y();
                            const Standard_Real aF2z =  aDuS2.Z()*aDirS2.X() +
                                                        aDvS2.Z()*aDirS2.Y();

                            const Standard_Real aF1 = aF1x*aF1x + aF1y*aF1y + aF1z*aF1z;
                            const Standard_Real aF2 = aF2x*aF2x + aF2y*aF2y + aF2z*aF2z;

                            if((aF1 < gp::Resolution()) && (aF2 < gp::Resolution()))
                            {
                              //All derivative are equal to 0. Therefore, there is
                              //no point in going along direction chosen.
                              Arrive = Standard_True;
                              break;
                            }
                          }
                        }//if (previoustg) cond.

                        ////////////////////////////////////////
                        AddAPoint(line,previousPoint);
                        RejectIndex++;

                        if(RejectIndex >= RejectIndexMAX)
                        {
                          Arrive = Standard_True;
                          break;
                        }

                        //

                        LevelOfIterWithoutAppend=0;
                        Arrive = Standard_True;
                      }
                      else
                      {
                        // revert to the last correctly calculated point
                        previousPoint = previousPointSave;
                        previoustg    = previoustgSave;
                        previousd     = previousdSave;
                        previousd1    = previousd1Save;
                        previousd2    = previousd2Save;
                      }
                    }

                    //
                    Standard_Boolean wasExtended = Standard_False;

                    if(Arrive && myIntersectionOn2S.IsTangent() && bPrevNotTangent)
                    {
                      if(ExtendLineInCommonZone(SauvChoixIso, DejaReparti))
                      {
                        wasExtended = Standard_True;
                        Arrive = Standard_False;
                        ChoixIso = SauvChoixIso;
                      }
                    }

                    RepartirOuDiviser(DejaReparti,ChoixIso,Arrive);

                    if(Arrive && 
                      myIntersectionOn2S.IsDone() && !myIntersectionOn2S.IsEmpty() &&
                      myIntersectionOn2S.IsTangent() && bPrevNotTangent &&
                      !wasExtended)
                    {
                      if(ExtendLineInCommonZone(SauvChoixIso, DejaReparti))
                      {
                        wasExtended = Standard_True;
                        Arrive = Standard_False;
                        ChoixIso = SauvChoixIso;
                      }
                    }
                  }//else !TestArret() $
                }//$$ end successful framing on border (!myIntersectionOn2S.IsEmpty())
                else
                {
                  //echec framing on border; division of step 
                  Arrive = Standard_False;
                  NoTestDeflection = Standard_True;
                  RepartirOuDiviser(DejaReparti,ChoixIso,Arrive);
                }
              }//$$$ end framing on border (!close)
            }//004 fin TestArret return Arrive = True
          } // 006case IntWalk_ArretSurPoint:  end Processing Status = OK  or ArretSurPoint 
        } //007  switch(aStatus)
      } //008 end processing point  (TEST DEFLECTION)
    } //009 end processing line (else if myIntersectionOn2S.IsDone())
  }  //010 end if first departure point allows marching  while (!Arrive)

  done = Standard_True;
}
// ===========================================================================================================
// function: ExtendLineInCommonZone
// purpose:  Extends already computed line inside tangent zone in the direction given by theChoixIso.
//           Returns Standard_True if the line was extended through tangent zone and the last computed point 
//           is outside the tangent zone (but it is not put into the line). Otherwise returns Standard_False.
// ===========================================================================================================
Standard_Boolean IntWalk_PWalking::ExtendLineInCommonZone(const IntImp_ConstIsoparametric theChoixIso,
                                                          const Standard_Boolean          theDirectionFlag) 
{
  Standard_Boolean bOutOfTangentZone = Standard_False;
  Standard_Boolean bStop = !myIntersectionOn2S.IsTangent();
  Standard_Integer dIncKey = 1;
  TColStd_Array1OfReal Param(1,4);
  IntWalk_StatusDeflection aStatus = IntWalk_OK;
  Standard_Integer nbIterWithoutAppend = 0;
  Standard_Integer nbEqualPoints = 0;
  Standard_Integer parit = 0;
  Standard_Integer uvit = 0;
  IntSurf_SequenceOfPntOn2S aSeqOfNewPoint;

  while (!bStop) {
    nbIterWithoutAppend++;

    if((nbIterWithoutAppend > 20) || (nbEqualPoints > 20)) {
#ifdef OCCT_DEBUG
      cout<<"Infinite loop detected. Stop iterations (IntWalk_PWalking_1.gxx)" << endl;
#endif
      bStop = Standard_True;
      break;
    }
    Standard_Real f = 0.;

    switch (theChoixIso)
    { 
    case IntImp_UIsoparametricOnCaro1: f = Abs(previousd1.X()); break;
    case IntImp_VIsoparametricOnCaro1: f = Abs(previousd1.Y()); break;
    case IntImp_UIsoparametricOnCaro2: f = Abs(previousd2.X()); break;
    case IntImp_VIsoparametricOnCaro2: f = Abs(previousd2.Y()); break;
    }

    if(f<0.1) f=0.1;

    previousPoint.Parameters(Param(1),Param(2),Param(3),Param(4));

    Standard_Real dP1 = sensCheminement * pasuv[0] * previousd1.X() /f;
    Standard_Real dP2 = sensCheminement * pasuv[1] * previousd1.Y() /f;
    Standard_Real dP3 = sensCheminement * pasuv[2] * previousd2.X() /f; 
    Standard_Real dP4 = sensCheminement * pasuv[3] * previousd2.Y() /f;

    if(theChoixIso == IntImp_UIsoparametricOnCaro1 && Abs(dP1) < 1.e-7) dP1 *= (5. * (Standard_Real)dIncKey);
    if(theChoixIso == IntImp_VIsoparametricOnCaro1 && Abs(dP2) < 1.e-7) dP2 *= (5. * (Standard_Real)dIncKey);
    if(theChoixIso == IntImp_UIsoparametricOnCaro2 && Abs(dP3) < 1.e-7) dP3 *= (5. * (Standard_Real)dIncKey);
    if(theChoixIso == IntImp_VIsoparametricOnCaro2 && Abs(dP4) < 1.e-7) dP4 *= (5. * (Standard_Real)dIncKey);

    Param(1) += dP1;
    Param(2) += dP2;
    Param(3) += dP3; 
    Param(4) += dP4;
    Standard_Real SvParam[4];
    IntImp_ConstIsoparametric ChoixIso = theChoixIso;

    for(parit = 0; parit < 4; parit++) {
      SvParam[parit] = Param(parit+1);
    }
    math_FunctionSetRoot  Rsnld(myIntersectionOn2S.Function());
    ChoixIso = myIntersectionOn2S.Perform(Param,Rsnld, theChoixIso);

    if (!myIntersectionOn2S.IsDone()) {
      return bOutOfTangentZone;
    }
    else {
      if (myIntersectionOn2S.IsEmpty()) {
        return bOutOfTangentZone;
      }

      aStatus = TestDeflection(ChoixIso);

      if(aStatus == IntWalk_OK) {

        for(uvit = 0; uvit < 4; uvit++) {
          if(pasuv[uvit] < pasInit[uvit]) {
            pasuv[uvit] = pasInit[uvit];
          }
        }
      }

      switch(aStatus) {
      case  IntWalk_ArretSurPointPrecedent:
        {
          bStop = Standard_True;
          bOutOfTangentZone = !myIntersectionOn2S.IsTangent();
          break;
        }
      case IntWalk_PasTropGrand:
        {
          for(parit = 0; parit < 4; parit++) {
            Param(parit+1) = SvParam[parit];
          }
          Standard_Boolean bDecrease = Standard_False;

          for(uvit = 0; uvit < 4; uvit++) {
            if(pasSav[uvit] < pasInit[uvit]) { 
              pasInit[uvit] -= (pasInit[uvit] - pasSav[uvit]) * 0.1;
              bDecrease = Standard_True;
            }
          }

          if(bDecrease) nbIterWithoutAppend--;
          break;
        }
      case IntWalk_PointConfondu:
        {
          for(uvit = 0; uvit < 4; uvit++) {
            if(pasuv[uvit] < pasInit[uvit]) {
              pasuv[uvit] += (pasInit[uvit] - pasuv[uvit]) * 0.1;
            }
          }
          break;
        }
      case IntWalk_OK:
      case IntWalk_ArretSurPoint:
        {
          //
          bStop = TestArret(theDirectionFlag, Param, ChoixIso);
          //

          //
          if(!bStop) {
            Standard_Real u11,v11,u12,v12; 
            myIntersectionOn2S.Point().Parameters(u11,v11,u12,v12); 
            Standard_Real u21,v21,u22,v22;
            previousPoint.Parameters(u21,v21,u22,v22); 

            if(((fabs(u11-u21) < ResoU1) && (fabs(v11-v21) < ResoV1)) ||
              ((fabs(u12-u22) < ResoU2) && (fabs(v12-v22) < ResoV2))) {
                nbEqualPoints++;
            }
            else {
              nbEqualPoints = 0;
            }
          }
          //

          bStop = bStop || !myIntersectionOn2S.IsTangent();
          bOutOfTangentZone = !myIntersectionOn2S.IsTangent();

          if(!bStop) {
            Standard_Boolean pointisvalid = Standard_False;
            Standard_Real u1,v1,u2,v2; 
            myIntersectionOn2S.Point().Parameters(u1,v1,u2,v2); 

            if(u1 <= UM1  && u2 <= UM2 && v1 <= VM1 && 
              v2 <= VM2  && u1 >= Um1 && u2 >= Um2 &&
              v1 >= Vm1  && v2 >= Vm2) 
              pointisvalid = Standard_True;

            if(pointisvalid) {
              previousPoint = myIntersectionOn2S.Point();
              previoustg = myIntersectionOn2S.IsTangent();

              if(!previoustg) {
                previousd  = myIntersectionOn2S.Direction();
                previousd1 = myIntersectionOn2S.DirectionOnS1();
                previousd2 = myIntersectionOn2S.DirectionOnS2();
              }
              Standard_Boolean bAddPoint = Standard_True;

              if(line->NbPoints() >= 1) {
                gp_Pnt pf = line->Value(1).Value();
                gp_Pnt pl = previousPoint.Value(); 

                if(pf.Distance(pl) < Precision::Confusion()) { 
                  dIncKey++; 
                  if(dIncKey == 5000) return bOutOfTangentZone; 
                  else bAddPoint = Standard_False;
                }
              }

              if(bAddPoint) {
                aSeqOfNewPoint.Append(previousPoint);
                nbIterWithoutAppend = 0;
              }
            }

            if (line->NbPoints() == 2) {
              for(uvit = 0; uvit < 4; uvit++) {
                pasSav[uvit] = pasuv[uvit]; 
              }
            }

            if ( !pointisvalid ) {
              // decrease step if out of bounds
              // otherwise the same calculations will be 
              // repeated several times
              if ( ( u1 > UM1 ) || ( u1 < Um1 ) )
                pasuv[0] *= 0.5;

              if ( ( v1 > VM1 ) || ( v1 < Vm1 ) ) 
                pasuv[1] *= 0.5;

              if ( ( u2 > UM2 ) || ( u2 < Um2 ) )
                pasuv[2] *= 0.5;

              if ( ( v2 > VM2 ) || ( v2 < Vm2 ) )
                pasuv[3] *= 0.5;
            }
          } // end if(!bStop)
          else { //if(bStop)
            if(close && (line->NbPoints() >= 1)) { 

              if(!bOutOfTangentZone) {
                aSeqOfNewPoint.Append(line->Value(1)); // line end
              }
              nbIterWithoutAppend = 0;
            }
            else {
              ChoixIso = myIntersectionOn2S.Perform(Param, Rsnld, theChoixIso);

              if(myIntersectionOn2S.IsEmpty()) { 
                bStop = Standard_True;// !myIntersectionOn2S.IsTangent();
                bOutOfTangentZone = Standard_False; // !myIntersectionOn2S.IsTangent();
              }
              else {
                Standard_Boolean bAddPoint = Standard_True;
                Standard_Boolean pointisvalid = Standard_False;

                previousPoint = myIntersectionOn2S.Point();
                Standard_Real u1,v1,u2,v2; 
                previousPoint.Parameters(u1,v1,u2,v2); 

                if(u1 <= UM1  && u2 <= UM2 && v1 <= VM1 && 
                  v2 <= VM2  && u1 >= Um1 && u2 >= Um2 &&
                  v1 >= Vm1  && v2 >= Vm2) 
                  pointisvalid = Standard_True;

                if(pointisvalid) {

                  if(line->NbPoints() >= 1) {
                    gp_Pnt pf = line->Value(1).Value();
                    gp_Pnt pl = previousPoint.Value(); 

                    if(pf.Distance(pl) < Precision::Confusion()) { 
                      dIncKey++; 
                      if(dIncKey == 5000) return bOutOfTangentZone; 
                      else bAddPoint = Standard_False;
                    }
                  }

                  if(bAddPoint && !bOutOfTangentZone) {
                    aSeqOfNewPoint.Append(previousPoint);
                    nbIterWithoutAppend = 0;
                  }
                }
              }
            }
          }
          break;
        }
      default:
        {
          break;
        }
      }
    }
  }
  Standard_Boolean bExtendLine = Standard_False;
  Standard_Real u1 = 0., v1 = 0., u2 = 0., v2 = 0.; 

  Standard_Integer pit = 0;

  for(pit = 0; !bExtendLine && (pit < 2); pit++) {
    if(pit == 0)
      previousPoint.Parameters(u1,v1,u2,v2); 
    else {
      if(aSeqOfNewPoint.Length() > 0)
        aSeqOfNewPoint.Value(aSeqOfNewPoint.Length()).Parameters(u1,v1,u2,v2); 
      else
        break;
    }

    if(((u1 - Um1) < ResoU1) ||
      ((UM1 - u1) < ResoU1) ||
      ((u2 - Um2) < ResoU2) ||
      ((UM2 - u2) < ResoU2) ||
      ((v1 - Vm1) < ResoV1) ||
      ((VM1 - v1) < ResoV1) ||
      ((v2 - Vm2) < ResoV2) ||
      ((VM2 - v2) < ResoV2))
      bExtendLine = Standard_True;
  }

  if(!bExtendLine) {
    //    if(aStatus == IntWalk_OK || aStatus == IntWalk_ArretSurPoint) {
    if(aStatus == IntWalk_OK) {
      bExtendLine = Standard_True;

      if(aSeqOfNewPoint.Length() > 1) {
        TColStd_Array1OfReal FirstParams(0, 3), LastParams(0, 3), Resolutions(0, 3);
        Resolutions(0) = ResoU1; Resolutions(1) = ResoV1; Resolutions(2) = ResoU2; Resolutions(3) = ResoV2;

        aSeqOfNewPoint(1).Parameters(FirstParams.ChangeValue(0), FirstParams.ChangeValue(1),
          FirstParams.ChangeValue(2), FirstParams.ChangeValue(3));
        aSeqOfNewPoint(aSeqOfNewPoint.Length()).Parameters(LastParams.ChangeValue(0), 
          LastParams.ChangeValue(1),
          LastParams.ChangeValue(2), 
          LastParams.ChangeValue(3)); 
        Standard_Integer indexofiso = 0;

        if(theChoixIso == IntImp_UIsoparametricOnCaro1) indexofiso = 0;
        if(theChoixIso == IntImp_VIsoparametricOnCaro1) indexofiso = 1;
        if(theChoixIso == IntImp_UIsoparametricOnCaro2) indexofiso = 2;
        if(theChoixIso == IntImp_VIsoparametricOnCaro2) indexofiso = 3;

        Standard_Integer afirstindex = (indexofiso < 2) ? 0 : 2;
        gp_Vec2d aTangentZoneDir(gp_Pnt2d(FirstParams.Value(afirstindex), FirstParams.Value(afirstindex + 1)),
          gp_Pnt2d(LastParams.Value(afirstindex), LastParams.Value(afirstindex + 1)));

        gp_Dir2d anIsoDir(0, 1);

        if((indexofiso == 1) || (indexofiso == 3))
          anIsoDir = gp_Dir2d(1, 0);

        if(aTangentZoneDir.SquareMagnitude() > gp::Resolution()) {
          Standard_Real piquota = M_PI*0.25;

          if(fabs(aTangentZoneDir.Angle(anIsoDir)) > piquota) {
            Standard_Integer ii = 1, nextii = 2;
            gp_Vec2d d1(0, 0);
            Standard_Real asqresol = gp::Resolution();
            asqresol *= asqresol;

            do {
              aSeqOfNewPoint(ii).Parameters(FirstParams.ChangeValue(0), FirstParams.ChangeValue(1),
                FirstParams.ChangeValue(2), FirstParams.ChangeValue(3));
              aSeqOfNewPoint(ii + 1).Parameters(LastParams.ChangeValue(0), LastParams.ChangeValue(1),
                LastParams.ChangeValue(2), LastParams.ChangeValue(3));
              d1 = gp_Vec2d(gp_Pnt2d(FirstParams.Value(afirstindex),
                FirstParams.Value(afirstindex + 1)),
                gp_Pnt2d(LastParams.Value(afirstindex),
                LastParams.Value(afirstindex + 1)));
              ii++;
            }
            while((d1.SquareMagnitude() < asqresol) &&
              (ii < aSeqOfNewPoint.Length()));

            nextii = ii;

            while(nextii < aSeqOfNewPoint.Length()) {

              gp_Vec2d nextd1(0, 0);
              Standard_Integer jj = nextii;

              do {
                aSeqOfNewPoint(jj).Parameters(FirstParams.ChangeValue(0), FirstParams.ChangeValue(1),
                  FirstParams.ChangeValue(2), FirstParams.ChangeValue(3));
                aSeqOfNewPoint(jj + 1).Parameters(LastParams.ChangeValue(0), LastParams.ChangeValue(1),
                  LastParams.ChangeValue(2), LastParams.ChangeValue(3));
                nextd1 = gp_Vec2d(gp_Pnt2d(FirstParams.Value(afirstindex),
                  FirstParams.Value(afirstindex + 1)),
                  gp_Pnt2d(LastParams.Value(afirstindex),
                  LastParams.Value(afirstindex + 1)));
                jj++;

              }
              while((nextd1.SquareMagnitude() < asqresol) &&
                (jj < aSeqOfNewPoint.Length()));
              nextii = jj;

              if(fabs(d1.Angle(nextd1)) > piquota) {
                bExtendLine = Standard_False;
                break;
              }
              d1 = nextd1;
            }
          }
          // end if(fabs(aTangentZoneDir.Angle(anIsoDir)
        }
      }
    }
  }

  if(!bExtendLine) {
    return Standard_False;
  }
  Standard_Integer i = 0;

  for(i = 1; i <= aSeqOfNewPoint.Length(); i++) {
    AddAPoint(line, aSeqOfNewPoint.Value(i));
  }

  return bOutOfTangentZone;
}

//=======================================================================
//function : DistanceMinimizeByGradient
//purpose  : 
//
// ATTENTION!!!
//  theInit should be initialized before function calling.
//=======================================================================
Standard_Boolean IntWalk_PWalking::
DistanceMinimizeByGradient( const Handle(Adaptor3d_HSurface)& theASurf1,
                           const Handle(Adaptor3d_HSurface)& theASurf2,
                           TColStd_Array1OfReal& theInit,
                           const Standard_Real* theStep0)
{
  const Standard_Integer aNbIterMAX = 60;
  const Standard_Real aTol = 1.0e-14;
  const Standard_Real aTolNul = 1.0 / Precision::Infinite();

  // I.e. if theU1 = 0.0 then Epsilon(theU1) = DBL_MIN (~1.0e-308).
  // Work with this number is impossible: there is a dangerous to 
  // obtain Floating-point-overflow. Therefore, we limit this value.
  const Standard_Real aMinAddValU1 = Max(Epsilon(theInit(1)), aTolNul);
  const Standard_Real aMinAddValV1 = Max(Epsilon(theInit(2)), aTolNul);
  const Standard_Real aMinAddValU2 = Max(Epsilon(theInit(3)), aTolNul);
  const Standard_Real aMinAddValV2 = Max(Epsilon(theInit(4)), aTolNul);

  Handle(Geom_Surface) aS1, aS2;

  if (theASurf1->GetType() != GeomAbs_BezierSurface &&
      theASurf1->GetType() != GeomAbs_BSplineSurface)
      return Standard_True;
  if (theASurf2->GetType() != GeomAbs_BezierSurface &&
      theASurf2->GetType() != GeomAbs_BSplineSurface)
      return Standard_True;

  Standard_Boolean aStatus = Standard_False;

  gp_Pnt aP1, aP2;
  gp_Vec aD1u, aD1v, aD2U, aD2V;

  theASurf1->D1(theInit(1), theInit(2), aP1, aD1u, aD1v);
  theASurf2->D1(theInit(3), theInit(4), aP2, aD2U, aD2V);

  Standard_Real aSQDistPrev = aP1.SquareDistance(aP2);

  gp_Vec aP12(aP1, aP2);

  Standard_Real aGradFu(-aP12.Dot(aD1u));
  Standard_Real aGradFv(-aP12.Dot(aD1v));
  Standard_Real aGradFU( aP12.Dot(aD2U));
  Standard_Real aGradFV( aP12.Dot(aD2V));

  Standard_Real aStepU1 = 1.0e-6, aStepV1 = 1.0e-6,
                aStepU2 = 1.0e-6, aStepV2 = 1.0e-6;
    
  if (theStep0)
  {
    aStepU1 = theStep0[0];
    aStepV1 = theStep0[1];
    aStepU2 = theStep0[2];
    aStepV2 = theStep0[3];
  }

  Standard_Boolean flRepeat = Standard_True;
  Standard_Integer aNbIter = aNbIterMAX;

  while(flRepeat)
  {
    Standard_Real anAdd = aGradFu*aStepU1;
    const Standard_Real aPARu = theInit(1) - Sign(Max(Abs(anAdd), aMinAddValU1), anAdd);
    
    anAdd = aGradFv*aStepV1;
    const Standard_Real aPARv = theInit(2) - Sign(Max(Abs(anAdd), aMinAddValV1), anAdd);

    anAdd = aGradFU*aStepU2;
    const Standard_Real aParU = theInit(3) - Sign(Max(Abs(anAdd), aMinAddValU2), anAdd);

    anAdd = aGradFV*aStepV2;
    const Standard_Real aParV = theInit(4) - Sign(Max(Abs(anAdd), aMinAddValV2), anAdd);

    gp_Pnt aPt1, aPt2;

    theASurf1->D1(aPARu, aPARv, aPt1, aD1u, aD1v);
    theASurf2->D1(aParU, aParV, aPt2, aD2U, aD2V);

    Standard_Real aSQDist = aPt1.SquareDistance(aPt2);

    if(aSQDist < aSQDistPrev)
    {
      aSQDistPrev = aSQDist;
      theInit(1) = aPARu;
      theInit(2) = aPARv;
      theInit(3) = aParU;
      theInit(4) = aParV;

      aStatus = aSQDistPrev < aTol;
      aStepU1 *= 1.2;
      aStepV1 *= 1.2;
      aStepU2 *= 1.2;
      aStepV2 *= 1.2;
    }
    else
    {
      if(--aNbIter < 0)
      {
        flRepeat = Standard_False;
      }
      else
      {
        theASurf1->D1(theInit(1), theInit(2), aPt1, aD1u, aD1v);
        theASurf2->D1(theInit(3), theInit(4), aPt2, aD2U, aD2V);

        gp_Vec aPt12(aPt1, aPt2);
        aGradFu = -aPt12.Dot(aD1u);
        aGradFv = -aPt12.Dot(aD1v);
        aGradFU = aPt12.Dot(aD2U);
        aGradFV = aPt12.Dot(aD2V);

        if (theStep0)
        {
          aStepU1 = theStep0[0];
          aStepV1 = theStep0[1];
          aStepU2 = theStep0[2];
          aStepV2 = theStep0[3];
        }
        else
        {
          aStepU1 = aStepV1 = aStepU2 = aStepV2 = 1.0e-6;
        }
      }
    }
  }

  return aStatus;
}

//=======================================================================
//function : DistanceMinimizeByExtrema
//purpose  : 
//
// ATTENTION!!!
//  theP0, theU0 and theV0 parameters should be initialized
//    before the function calling.
//=======================================================================
Standard_Boolean IntWalk_PWalking::
DistanceMinimizeByExtrema(const Handle(Adaptor3d_HSurface)& theASurf, 
                          const gp_Pnt& theP0,
                          Standard_Real& theU0,
                          Standard_Real& theV0,
                          const Standard_Real* theStep0)
{
  const Standard_Real aTol = 1.0e-14;
  gp_Pnt aPS;
  gp_Vec aD1Su, aD1Sv, aD2Su, aD2Sv, aD2SuvTemp;
  Standard_Real aSQDistPrev = RealLast();
  Standard_Real aU = theU0, aV = theV0;

  const Standard_Real aStep0[2] = { theStep0 ? theStep0[0] : 1.0,
                                    theStep0 ? theStep0[1] : 1.0 };

  Standard_Integer aNbIter = 10;
  do
  {
    theASurf->D2(aU, aV, aPS, aD1Su, aD1Sv, aD2Su, aD2Sv, aD2SuvTemp);

    gp_Vec aVec(theP0, aPS);

    Standard_Real aSQDist = aVec.SquareMagnitude();

    if(aSQDist >= aSQDistPrev)
      break;

    aSQDistPrev = aSQDist;
    theU0 = aU;
    theV0 = aV;
    aNbIter--;

    if(aSQDistPrev < aTol)
      break;

    //Functions
    const Standard_Real aF1 = aD1Su.Dot(aVec), aF2 = aD1Sv.Dot(aVec);

    //Derivatives
    const Standard_Real aDf1u = aD2Su.Dot(aVec) + aD1Su.Dot(aD1Su),
      aDf1v = aD2Su.Dot(aD1Sv),
      aDf2u = aDf1v,
      aDf2v = aD2Sv.Dot(aVec) + aD1Sv.Dot(aD1Sv);

    const Standard_Real aDet = aDf1u*aDf2v - aDf1v*aDf2u;
    aU -= aStep0[0]*(aDf2v*aF1 - aDf1v*aF2) / aDet;
    aV += aStep0[1]*(aDf2u*aF1 - aDf1u*aF2) / aDet;
  }
  while(aNbIter > 0);

  return (aSQDistPrev < aTol);
}

//=======================================================================
//function : HandleSingleSingularPoint
//purpose  : 
//=======================================================================
Standard_Boolean IntWalk_PWalking::HandleSingleSingularPoint(const Handle(Adaptor3d_HSurface)& theASurf1,
                                                             const Handle(Adaptor3d_HSurface)& theASurf2,
                                                             const Standard_Real the3DTol,
                                                             TColStd_Array1OfReal &thePnt)
{
  // u1, v1, u2, v2 order is used.
  Standard_Real aLowBorder[4] = {theASurf1->FirstUParameter(),
                                 theASurf1->FirstVParameter(),
                                 theASurf2->FirstUParameter(),
                                 theASurf2->FirstVParameter()};
  Standard_Real aUppBorder[4] = {theASurf1->LastUParameter(),
                                 theASurf1->LastVParameter(),
                                 theASurf2->LastUParameter(),
                                 theASurf2->LastVParameter()};
  IntImp_ConstIsoparametric aLockedDir[4] = {IntImp_UIsoparametricOnCaro1,
                                             IntImp_VIsoparametricOnCaro1,
                                             IntImp_UIsoparametricOnCaro2,
                                             IntImp_VIsoparametricOnCaro2};

  // Create new intersector with new tolerance.
  IntWalk_TheInt2S anInt(theASurf1, theASurf2, the3DTol);
  math_FunctionSetRoot aRsnld(anInt.Function());

  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    if ( Abs(thePnt(i) - aLowBorder[i - 1]) < Precision::PConfusion() ||
         Abs(thePnt(i) - aUppBorder[i - 1]) < Precision::PConfusion())
    {

      anInt.Perform(thePnt,aRsnld, aLockedDir[i - 1]);

      if (!anInt.IsDone())
        continue;

      if (anInt.IsEmpty())
        continue;

      anInt.Point().Parameters(thePnt(1), thePnt(2), thePnt(3), thePnt(4));
      return Standard_True;
    }
  }

  return Standard_False;
}

//=======================================================================
//function : SeekPointOnBoundary
//purpose  : 
//=======================================================================
Standard_Boolean IntWalk_PWalking::
SeekPointOnBoundary(const Handle(Adaptor3d_HSurface)& theASurf1,
                    const Handle(Adaptor3d_HSurface)& theASurf2,
                    const Standard_Real theU1,
                    const Standard_Real theV1,
                    const Standard_Real theU2,
                    const Standard_Real theV2,
                    const Standard_Boolean isTheFirst)
{
  Standard_Boolean isOK = Standard_False;

  // Tune solution tolerance according with object size.
  const Standard_Real aRes1 = Max(Precision::PConfusion() / theASurf1->UResolution(1.0),
                                  Precision::PConfusion() / theASurf1->VResolution(1.0));
  const Standard_Real aRes2 = Max(Precision::PConfusion() / theASurf2->UResolution(1.0),
                                  Precision::PConfusion() / theASurf2->VResolution(1.0));
  const Standard_Real a3DTol = Max(aRes1, aRes2);
  const Standard_Real aTol = Max(Precision::Confusion(), a3DTol);

  // u1, v1, u2, v2 order is used.
  TColStd_Array1OfReal aPnt(1,4);
  aPnt(1) = theU1; aPnt(2) = theV1; aPnt(3) = theU2; aPnt(4) = theV2;
  TColStd_Array1OfReal aSingularPnt(aPnt);

  Standard_Integer aNbIter = 20;
  Standard_Boolean aStatus = Standard_False;
  do
  {
    aNbIter--;
    aStatus = DistanceMinimizeByGradient(theASurf1, theASurf2, aPnt);
    if(aStatus)
      break;

    aStatus = DistanceMinimizeByExtrema(theASurf1, theASurf2->Value(aPnt(3), aPnt(4)), aPnt(1), aPnt(2));
    if(aStatus)
      break;

    aStatus = DistanceMinimizeByExtrema(theASurf2, theASurf1->Value(aPnt(1), aPnt(2)), aPnt(3), aPnt(4));
    if(aStatus)
      break;
  }
  while(!aStatus && (aNbIter > 0));

  // Handle singular points.
  Standard_Boolean aSingularStatus = HandleSingleSingularPoint(theASurf1, theASurf2, aTol, aSingularPnt);
  if (aSingularStatus)
    aPnt = aSingularPnt;

  if (!aStatus && !aSingularStatus)
  {
    return isOK;
  }

  gp_Pnt aP1 = theASurf1->Value(aPnt(1), aPnt(2));
  gp_Pnt aP2 = theASurf2->Value(aPnt(3), aPnt(4));
  const gp_Pnt aPInt(0.5*(aP1.XYZ() + aP2.XYZ()));

  const Standard_Real aSQDist = aPInt.SquareDistance(aP1);
  if (aSQDist > aTol * aTol)
  {
    return isOK;
  }

  //Found point is true intersection point
  IntSurf_PntOn2S anIP;
  anIP.SetValue(aPInt, aPnt(1), aPnt(2), aPnt(3), aPnt(4));

  //The main idea of checks below is to define if insertion of
  //addition point (on the boundary) does not lead to invalid
  //intersection curve (e.g. having a loop).
  //
  //Loops are detected with rotation angle of the Walking-line (WL).
  //If there is hairpin bend then insertion is forbidden.

  //There are at least two possible problems:
  //  1. There are some cases when two neighbor points of the WL
  //      are almost coincident (the distance between them is less
  //      than Precision::Confusion). It is impossible to define
  //      rotation angle in these cases. Therefore, points with
  //      "good" distances should be selected.

  //  2. Intersection point on the surface boundary has highest
  //      priority in compare with other "middle" points. Therefore,
  //      if insertion of new point will result in a bend then some
  //      "middle" points should be deleted in order to provide
  //      correct insertion.

  //Problem test cases:
  //  test bugs modalg_5 bug24585_1
  //  test boolean bcut_complex G7
  //  test bugs moddata_2 bug469

  if (isTheFirst)
  {
    while (line->NbPoints() > 1)
    {
      const Standard_Integer aNbPnts = line->NbPoints();

      Standard_Integer aPInd = 1;
      for (; aPInd <= aNbPnts; aPInd++)
      {
        aP1.SetXYZ(line->Value(aPInd).Value().XYZ());
        if (aP1.SquareDistance(aPInt) > Precision::SquareConfusion())
          break;
      }

      for (++aPInd; aPInd <= aNbPnts; aPInd++)
      {
        aP2.SetXYZ(line->Value(aPInd).Value().XYZ());
        if (aP1.SquareDistance(aP2) > Precision::SquareConfusion())
          break;
      }

      if (aPInd > aNbPnts)
      {
        return isOK;
      }

      const gp_XYZ aDir01(aP1.XYZ() - aPInt.XYZ());
      const gp_XYZ aDir12(aP2.XYZ() - aP1.XYZ());

      if (aDir01.Dot(aDir12) > 0.0)
      {
        break;
      }

      line->RemovePoint(1);
    }

    line->InsertBefore(1, anIP);
    isOK = Standard_True;
  }
  else
  {
    while (line->NbPoints() > 1)
    {
      const Standard_Integer aNbPnts = line->NbPoints();

      gp_Pnt aPPrev, aPCurr;
      Standard_Integer aPInd = aNbPnts;
      for (; aPInd > 0; aPInd--)
      {
        aPCurr.SetXYZ(line->Value(aPInd).Value().XYZ());
        if (aPCurr.SquareDistance(aPInt) > Precision::SquareConfusion())
          break;
      }

      for (--aPInd; aPInd > 0; aPInd--)
      {
        aPPrev.SetXYZ(line->Value(aPInd).Value().XYZ());
        if (aPCurr.SquareDistance(aPPrev) > Precision::SquareConfusion())
          break;
      }

      if (aPInd < 1)
      {
        return isOK;
      }

      const gp_XYZ aDirPC(aPCurr.XYZ() - aPPrev.XYZ());
      const gp_XYZ aDirCN(aPInt.XYZ() - aPCurr.XYZ());

      if (aDirPC.Dot(aDirCN) > 0.0)
      {
        break;
      }

      line->RemovePoint(aNbPnts);
    }

    line->Add(anIP);
    isOK = Standard_True;
  }

  return isOK;
}

//=======================================================================
//function : PutToBoundary
//purpose  : 
//=======================================================================
Standard_Boolean IntWalk_PWalking::
PutToBoundary(const Handle(Adaptor3d_HSurface)& theASurf1,
              const Handle(Adaptor3d_HSurface)& theASurf2)
{
  const Standard_Real aTolMin = Precision::Confusion();

  Standard_Boolean hasBeenAdded = Standard_False;

  const Standard_Real aU1bFirst = theASurf1->FirstUParameter();
  const Standard_Real aU1bLast = theASurf1->LastUParameter();
  const Standard_Real aU2bFirst = theASurf2->FirstUParameter();
  const Standard_Real aU2bLast = theASurf2->LastUParameter();
  const Standard_Real aV1bFirst = theASurf1->FirstVParameter();
  const Standard_Real aV1bLast = theASurf1->LastVParameter();
  const Standard_Real aV2bFirst = theASurf2->FirstVParameter();
  const Standard_Real aV2bLast = theASurf2->LastVParameter();

  Standard_Real aTol = 1.0;
  aTol = Min(aTol, aU1bLast - aU1bFirst);
  aTol = Min(aTol, aU2bLast - aU2bFirst);
  aTol = Min(aTol, aV1bLast - aV1bFirst);
  aTol = Min(aTol, aV2bLast - aV2bFirst)*1.0e-3;

  if(aTol <= 2.0*aTolMin)
    return hasBeenAdded;

  Standard_Boolean isNeedAdding = Standard_False;
  Standard_Boolean isU1parallel = Standard_False, isV1parallel = Standard_False;
  Standard_Boolean isU2parallel = Standard_False, isV2parallel = Standard_False;
  IsParallel(line, Standard_True, aTol, isU1parallel, isV1parallel);
  IsParallel(line, Standard_False, aTol, isU2parallel, isV2parallel);

  Standard_Real u1, v1, u2, v2;
  line->Value(1).Parameters(u1, v1, u2, v2);
  Standard_Real aDelta = 0.0;

  if(!isV1parallel)
  {
    aDelta = u1 - aU1bFirst;
    if((aTolMin < aDelta) && (aDelta < aTol))
    {
      u1 = aU1bFirst;
      isNeedAdding = Standard_True;
    }
    else
    {
      aDelta = aU1bLast - u1;
      if((aTolMin < aDelta) && (aDelta < aTol))
      {
        u1 = aU1bLast;
        isNeedAdding = Standard_True;
      }
    }
  }

  if(!isV2parallel)
  {
    aDelta = u2 - aU2bFirst;
    if((aTolMin < aDelta) && (aDelta < aTol))
    {
      u2 = aU2bFirst;
      isNeedAdding = Standard_True;
    }
    else
    {
      aDelta = aU2bLast - u2;
      if((aTolMin < aDelta) && (aDelta < aTol))
      {
        u2 = aU2bLast;
        isNeedAdding = Standard_True;
      }
    }
  }

  if(!isU1parallel)
  {
    aDelta = v1 - aV1bFirst;
    if((aTolMin < aDelta) && (aDelta < aTol))
    {
      v1 = aV1bFirst;
      isNeedAdding = Standard_True;
    }
    else
    {
      aDelta = aV1bLast - v1;
      if((aTolMin < aDelta) && (aDelta < aTol))
      {
        v1 = aV1bLast;
        isNeedAdding = Standard_True;
      }
    }
  }

  if(!isU2parallel)
  {
    aDelta = v2 - aV2bFirst;
    if((aTolMin < aDelta) && (aDelta < aTol))
    {
      v2 = aV2bFirst;
      isNeedAdding = Standard_True;
    }
    else
    {
      aDelta = aV2bLast - v2;
      if((aTolMin < aDelta) && (aDelta < aTol))
      {
        v2 = aV2bLast;
        isNeedAdding = Standard_True;
      }
    }
  }

  if(isNeedAdding)
  {
    hasBeenAdded = 
      SeekPointOnBoundary(theASurf1, theASurf2, u1, 
      v1, u2, v2, Standard_True);
  }

  const Standard_Integer aNbPnts = line->NbPoints();
  isNeedAdding = Standard_False;
  line->Value(aNbPnts).Parameters(u1, v1, u2, v2);

  if(!isV1parallel)
  {
    aDelta = u1 - aU1bFirst;
    if((aTolMin < aDelta) && (aDelta < aTol))
    {
      u1 = aU1bFirst;
      isNeedAdding = Standard_True;
    }
    else
    {
      aDelta = aU1bLast - u1;
      if((aTolMin < aDelta) && (aDelta < aTol))
      {
        u1 = aU1bLast;
        isNeedAdding = Standard_True;
      }
    }
  }

  if(!isV2parallel)
  {
    aDelta = u2 - aU2bFirst;
    if((aTolMin < aDelta) && (aDelta < aTol))
    {
      u2 = aU2bFirst;
      isNeedAdding = Standard_True;
    }
    else
    {
      aDelta = aU2bLast - u2;
      if((aTolMin < aDelta) && (aDelta < aTol))
      {
        u2 = aU2bLast;
        isNeedAdding = Standard_True;
      }
    }
  }

  if(!isU1parallel)
  {
    aDelta = v1 - aV1bFirst;
    if((aTolMin < aDelta) && (aDelta < aTol))
    {
      v1 = aV1bFirst;
      isNeedAdding = Standard_True;
    }
    else
    {
      aDelta = aV1bLast - v1;
      if((aTolMin < aDelta) && (aDelta < aTol))
      {
        v1 = aV1bLast;
        isNeedAdding = Standard_True;
      }
    }
  }

  if(!isU2parallel)
  {
    aDelta = v2 - aV2bFirst;
    if((aTolMin < aDelta) && (aDelta < aTol))
    {
      v2 = aV2bFirst;
      isNeedAdding = Standard_True;
    }
    else
    {
      aDelta = aV2bLast - v2;
      if((aTolMin < aDelta) && (aDelta < aTol))
      {
        v2 = aV2bLast;
        isNeedAdding = Standard_True;
      }
    }
  }

  if(isNeedAdding)
  {
    hasBeenAdded = 
      SeekPointOnBoundary(theASurf1, theASurf2, u1, 
      v1, u2, v2, Standard_False);
  }

  return hasBeenAdded;
}

//=======================================================================
//function : SeekAdditionalPoints
//purpose  : 
//=======================================================================
Standard_Boolean IntWalk_PWalking::
SeekAdditionalPoints( const Handle(Adaptor3d_HSurface)& theASurf1,
                     const Handle(Adaptor3d_HSurface)& theASurf2,
                     const Standard_Integer theMinNbPoints)
{
  const Standard_Real aTol = 1.0e-14;
  Standard_Integer aNbPoints = line->NbPoints();
  if(aNbPoints > theMinNbPoints)
    return Standard_True;

  const Standard_Real aU1bFirst = theASurf1->FirstUParameter();
  const Standard_Real aU1bLast = theASurf1->LastUParameter();
  const Standard_Real aU2bFirst = theASurf2->FirstUParameter();
  const Standard_Real aU2bLast = theASurf2->LastUParameter();
  const Standard_Real aV1bFirst = theASurf1->FirstVParameter();
  const Standard_Real aV1bLast = theASurf1->LastVParameter();
  const Standard_Real aV2bFirst = theASurf2->FirstVParameter();
  const Standard_Real aV2bLast = theASurf2->LastVParameter();


  Standard_Boolean isPrecise = Standard_False;

  TColStd_Array1OfReal aPnt(1, 4);
  aPnt.Init(0.0);

  Standard_Integer aNbPointsPrev = 0;
  while(aNbPoints < theMinNbPoints && (aNbPoints != aNbPointsPrev))
  {
    aNbPointsPrev = aNbPoints;
    for(Standard_Integer fp = 1, lp = 2; fp < aNbPoints; fp = lp + 1)
    {
      Standard_Real U1f, V1f, U2f, V2f; //first point in 1st and 2nd surafaces
      Standard_Real U1l, V1l, U2l, V2l; //last  point in 1st and 2nd surafaces

      lp = fp+1;
      line->Value(fp).Parameters(U1f, V1f, U2f, V2f);
      line->Value(lp).Parameters(U1l, V1l, U2l, V2l);

      aPnt(1) = 0.5*(U1f + U1l);
      if(aPnt(1) < aU1bFirst)
        aPnt(1) = aU1bFirst;
      if(aPnt(1) > aU1bLast)
        aPnt(1) = aU1bLast;

      aPnt(2) = 0.5*(V1f+V1l);
      if(aPnt(2) < aV1bFirst)
        aPnt(2) = aV1bFirst;
      if(aPnt(2) > aV1bLast)
        aPnt(2) = aV1bLast;

      aPnt(3) = 0.5*(U2f+U2l);
      if(aPnt(3) < aU2bFirst)
        aPnt(3) = aU2bFirst;
      if(aPnt(3) > aU2bLast)
        aPnt(3) = aU2bLast;

      aPnt(4) = 0.5*(V2f+V2l);
      if(aPnt(4) < aV2bFirst)
        aPnt(4) = aV2bFirst;
      if(aPnt(4) > aV2bLast)
        aPnt(4) = aV2bLast;

      Standard_Boolean aStatus = Standard_False;
      Standard_Integer aNbIter = 5;
      do
      {
        aStatus = DistanceMinimizeByGradient(theASurf1, theASurf2, aPnt);
        if(aStatus)
        {
          break;
        }

        aStatus = DistanceMinimizeByExtrema(theASurf1, theASurf2->Value(aPnt(3), aPnt(4)), aPnt(1), aPnt(2));
        if(aStatus)
        {
          break;
        }

        aStatus = DistanceMinimizeByExtrema(theASurf2, theASurf1->Value(aPnt(1), aPnt(2)), aPnt(3), aPnt(4));
        if(aStatus)
        {
          break;
        }
      }
      while(!aStatus && (--aNbIter > 0));

      if(aStatus)
      {
        gp_Pnt  aP1 = theASurf1->Value(aPnt(1), aPnt(2)),
          aP2 = theASurf2->Value(aPnt(3), aPnt(4));
        gp_Pnt aPInt(0.5*(aP1.XYZ() + aP2.XYZ()));

        const Standard_Real aSQDist1 = aPInt.SquareDistance(aP1),
          aSQDist2 = aPInt.SquareDistance(aP2);

        if((aSQDist1 < aTol) && (aSQDist2 < aTol))
        {
          IntSurf_PntOn2S anIP;
          anIP.SetValue(aPInt, aPnt(1), aPnt(2), aPnt(3), aPnt(4));
          line->InsertBefore(lp, anIP);

          isPrecise = Standard_True;

          if(++aNbPoints >= theMinNbPoints)
            break;
        }
        else
        {
          lp--;
        }
      }
    }
  }

  return isPrecise;
}

void IntWalk_PWalking::
RepartirOuDiviser(Standard_Boolean& DejaReparti,
                  IntImp_ConstIsoparametric& ChoixIso,
                  Standard_Boolean& Arrive) 

                  // at the neighborhood of a point, there is a fail of marching 
                  // it is required to divide the steps to try to continue
                  // if the step is too small if we are on border
                  // restart in another direction if it was not done, otherwise stop

{
  //  Standard_Integer i;
  if (Arrive) {    //restart in the other direction
    if (!DejaReparti ) {
      Arrive        = Standard_False; 
      DejaReparti   = Standard_True;
      previousPoint = line->Value(1);
      previoustg    = Standard_False;
      previousd1    = firstd1;
      previousd2    = firstd2;
      previousd     = tgdir;
      indextg       = line->NbPoints();
      tgdir.Reverse();
      line->Reverse();

      //-- printf("\nIntWalk_PWalking_2.gxx Reverse %3d\n",indextg);
      sensCheminement = -1;
      tgfirst      = tglast;
      tglast       = Standard_False;
      ChoixIso     = choixIsoSav;
#if 0
      pasuv[0]=pasSav[0];
      pasuv[1]=pasSav[1];
      pasuv[2]=pasSav[2];
      pasuv[3]=pasSav[3];
#else 
      Standard_Real u1,v1,u2,v2;
      Standard_Real U1,V1,U2,V2;
      Standard_Integer nn=line->NbPoints();
      if(nn>2) { 
        line->Value(nn).Parameters(u1,v1,u2,v2);
        line->Value(nn-1).Parameters(U1,V1,U2,V2);
        pasuv[0]=Abs(u1-U1);
        pasuv[1]=Abs(v1-V1);
        pasuv[2]=Abs(u2-U2);
        pasuv[3]=Abs(v2-V2);
      }
#endif

    }
  }  
  else  {
    if (    pasuv[0]*0.5 < ResoU1
      &&  pasuv[1]*0.5 < ResoV1
      &&  pasuv[2]*0.5 < ResoU2
      &&  pasuv[3]*0.5 < ResoV2
      ) {
        if (!previoustg) {
          tglast = Standard_True;      // IS IT ENOUGH ????
        }

        if (!DejaReparti) {  //restart in the other direction
          DejaReparti       = Standard_True;
          previousPoint     = line->Value(1);
          previoustg        = Standard_False;
          previousd1        = firstd1;
          previousd2        = firstd2;
          previousd         = tgdir;
          indextg           = line->NbPoints();
          tgdir.Reverse();
          line->Reverse();

          //-- printf("\nIntWalk_PWalking_2.gxx Reverse %3d\n",indextg);

          sensCheminement   = -1;
          tgfirst           = tglast;
          tglast            = Standard_False;
          ChoixIso          = choixIsoSav;

#if 0 
          pasuv[0]=pasSav[0];
          pasuv[1]=pasSav[1];
          pasuv[2]=pasSav[2];
          pasuv[3]=pasSav[3];
#else 
          Standard_Real u1,v1,u2,v2;
          Standard_Real U1,V1,U2,V2;
          Standard_Integer nn=line->NbPoints();
          if(nn>2) { 
            line->Value(nn).Parameters(u1,v1,u2,v2);
            line->Value(nn-1).Parameters(U1,V1,U2,V2);
            pasuv[0]=Abs(u1-U1);
            pasuv[1]=Abs(v1-V1);
            pasuv[2]=Abs(u2-U2);
            pasuv[3]=Abs(v2-V2);
          }
#endif
        }
        else Arrive = Standard_True;
    }
    else {
      pasuv[0]*=0.5;
      pasuv[1]*=0.5;
      pasuv[2]*=0.5;
      pasuv[3]*=0.5; 
    }
  }
}

namespace {
  //OCC431(apo): modified ->
  static const Standard_Real CosRef2D =  Cos(M_PI/9.0),  AngRef2D = M_PI/2.0; 

  static const Standard_Real d = 7.0;
}

IntWalk_StatusDeflection  IntWalk_PWalking::TestDeflection(const IntImp_ConstIsoparametric choixIso)

// test if vector is observed by calculating an increase of vector 
//     or the previous point and its tangent, the new calculated point and its  
//     tangent; it is possible to find a cube passing by the 2 points and having as a 
//     derivative the tangents of the intersection
//     calculate the point with parameter 0.5 on cube=p1 
//     calculate the medium point of 2 points of intersection=p2
//   if arrow/2<=||p1p2||<= arrow consider that the vector is observed
//   otherwise adjust the step depending on the ratio ||p1p2||/vector
//   and the previous step 
// test if in  2 tangent planes of surfaces there is no too great angle2d 
// grand : if yes divide the step
// test if there is no change of side
//  
{
  if(line->NbPoints() ==1 ) { 
    STATIC_BLOCAGE_SUR_PAS_TROP_GRAND=STATIC_PRECEDENT_INFLEXION=0;
  }

  IntWalk_StatusDeflection aStatus = IntWalk_OK;
  Standard_Real FlecheCourante , Ratio = 1.0;

  // Caro1 and Caro2
  const Handle(Adaptor3d_HSurface)& Caro1 = myIntersectionOn2S.Function().AuxillarSurface1();
  const Handle(Adaptor3d_HSurface)& Caro2 = myIntersectionOn2S.Function().AuxillarSurface2();

  const IntSurf_PntOn2S& CurrentPoint = myIntersectionOn2S.Point(); 
  //==================================================================================
  //=========               S t o p   o n   p o i n t                 ============
  //================================================================================== 
  if (myIntersectionOn2S.IsTangent())  { 
    return IntWalk_ArretSurPoint;  
  }

  const gp_Dir& TgCourante = myIntersectionOn2S.Direction();

  const Standard_Real aCosBetweenTangent = TgCourante.Dot(previousd);

  //==================================================================================
  //=========   R i s k   o f    i n f l e x i o n   p o i n t  ============
  //==================================================================================  
  if (aCosBetweenTangent < 0) {
    //------------------------------------------------------------
    //-- Risk of inflexion point : Divide the step by 2
    //-- Initialize STATIC_PRECEDENT_INFLEXION so that 
    //-- at the next call to return Pas_OK if there is no 
    //-- more risk of the point of inflexion
    //------------------------------------------------------------

    pasuv[0]*=0.5;
    pasuv[1]*=0.5;
    pasuv[2]*=0.5;
    pasuv[3]*=0.5;
    STATIC_PRECEDENT_INFLEXION+=3; 
    if (pasuv[0] < ResoU1 && pasuv[1] <ResoV1 && pasuv[2] <ResoU2 && pasuv[3] < ResoV2)
      return IntWalk_ArretSurPointPrecedent;
    else 
      return IntWalk_PasTropGrand;
  }
  else {
    if(STATIC_PRECEDENT_INFLEXION  > 0) { 
      STATIC_PRECEDENT_INFLEXION -- ;
      return IntWalk_OK;
    }
  }

  //==================================================================================
  //=========  D e t e c t    c o n f u s e d    P o in t s       ===========
  //==================================================================================

  const Standard_Real aSqDist = previousPoint.Value().
    SquareDistance(CurrentPoint.Value());


  if (aSqDist < Precision::SquareConfusion()) { 
    pasInit[0] = Max(pasInit[0], 5.0*ResoU1);
    pasInit[1] = Max(pasInit[1], 5.0*ResoV1);
    pasInit[2] = Max(pasInit[2], 5.0*ResoU2);
    pasInit[3] = Max(pasInit[3], 5.0*ResoV2);

    for(Standard_Integer i = 0; i < 4; i++)
    {
      pasuv[i] = Max(pasuv[i], Min(1.5*pasuv[i], pasInit[i]));
    }
    //Compute local resolution: for OCC26717
    if (Abs(pasuv[choixIso] - pasInit[choixIso]) <= Precision::Confusion())
    {
      Standard_Real CurU, CurV;
      if (choixIso == IntImp_UIsoparametricOnCaro1 ||
          choixIso == IntImp_VIsoparametricOnCaro1)
        previousPoint.ParametersOnS1(CurU, CurV);
      else
        previousPoint.ParametersOnS2(CurU, CurV);
      gp_Pnt CurPnt = (choixIso == IntImp_UIsoparametricOnCaro1 ||
                       choixIso == IntImp_VIsoparametricOnCaro1)?
        Adaptor3d_HSurfaceTool::Value(Caro1, CurU, CurV) :
        Adaptor3d_HSurfaceTool::Value(Caro2, CurU, CurV);
      gp_Pnt OffsetPnt;
      switch(choixIso)
      {
      case IntImp_UIsoparametricOnCaro1:
        OffsetPnt =
          Adaptor3d_HSurfaceTool::Value(Caro1,
                                        CurU + sensCheminement*pasuv[0],
                                        CurV);
        break;
      case IntImp_VIsoparametricOnCaro1:
        OffsetPnt =
          Adaptor3d_HSurfaceTool::Value(Caro1,
                                        CurU,
                                        CurV + sensCheminement*pasuv[1]);
        break;
      case IntImp_UIsoparametricOnCaro2:
        OffsetPnt =
          Adaptor3d_HSurfaceTool::Value(Caro2,
                                        CurU + sensCheminement*pasuv[2],
                                        CurV);
        break;
      case IntImp_VIsoparametricOnCaro2:
        OffsetPnt =
          Adaptor3d_HSurfaceTool::Value(Caro2,
                                        CurU,
                                        CurV + sensCheminement*pasuv[3]);
        break;
      default:break;
      }
      Standard_Real RefDist = CurPnt.Distance(OffsetPnt);
      Standard_Real LocalResol = 0.;
      if (RefDist > gp::Resolution())
        LocalResol = pasuv[choixIso] * tolconf / RefDist;
      if (pasuv[choixIso] < 2*LocalResol)
        pasuv[choixIso] = pasInit[choixIso] = 2*LocalResol;
    }
    ////////////////////////////////////////
    aStatus = IntWalk_PointConfondu;
  }

  //==================================================================================
  Standard_Real Up1,Vp1,Uc1,Vc1,Du1,Dv1,AbsDu1,AbsDu2,AbsDv1,AbsDv2;
  Standard_Real Up2,Vp2,Uc2,Vc2,Du2,Dv2;

  previousPoint.Parameters(Up1,Vp1,Up2,Vp2);
  CurrentPoint.Parameters(Uc1,Vc1,Uc2,Vc2);               

  Du1 = Uc1 - Up1;   Dv1 = Vc1 - Vp1;
  Du2 = Uc2 - Up2;   Dv2 = Vc2 - Vp2;

  AbsDu1 = Abs(Du1);
  AbsDu2 = Abs(Du2);
  AbsDv1 = Abs(Dv1);
  AbsDv2 = Abs(Dv2);
  //=================================================================================
  //====   S t e p   o f   p  r o g r e s s i o n (between previous and Current)   =======
  //=================================================================================
  if (   AbsDu1 < ResoU1 && AbsDv1 < ResoV1 
    && AbsDu2 < ResoU2 && AbsDv2 < ResoV2) {
      pasuv[0] = ResoU1; pasuv[1] = ResoV1; pasuv[2] = ResoU2; pasuv[3] = ResoV2;
      return(IntWalk_ArretSurPointPrecedent);
  }
  //==================================================================================

  Standard_Real tolArea = 100.0;
  if (ResoU1 < Precision::PConfusion() ||
    ResoV1 < Precision::PConfusion() ||
    ResoU2 < Precision::PConfusion() ||
    ResoV2 < Precision::PConfusion() )
    tolArea =  tolArea*2.0;

  Standard_Real Cosi1, CosRef1, Ang1, AngRef1, ResoUV1, Duv1, d1, tolCoeff1;   
  Standard_Real Cosi2, CosRef2, Ang2, AngRef2, ResoUV2, Duv2, d2, tolCoeff2;   
  Cosi1 = Du1*previousd1.X() + Dv1*previousd1.Y();
  Cosi2 = Du2*previousd2.X() + Dv2*previousd2.Y();
  Duv1 = Du1*Du1 + Dv1*Dv1;
  Duv2 = Du2*Du2 + Dv2*Dv2;
  ResoUV1 = ResoU1*ResoU1 + ResoV1*ResoV1;
  ResoUV2 = ResoU2*ResoU2 + ResoV2*ResoV2;
  //
  //modified by NIZNHY-PKV Wed Nov 13 12:25:44 2002 f
  //
  Standard_Real aMinDiv2=Precision::Confusion();
  aMinDiv2=aMinDiv2*aMinDiv2;
  //
  d1=d;
  if (Duv1>aMinDiv2)  {
    d1 = Abs(ResoUV1/Duv1);
    d1 = Min(Sqrt(d1)*tolArea, d);  
  } 
  //d1 = Abs(ResoUV1/Duv1); 
  //d1 = Min(Sqrt(d1)*tolArea,d);  
  //modified by NIZNHY-PKV Wed Nov 13 12:34:30 2002 t
  tolCoeff1 = Exp(d1);
  //
  //modified by NIZNHY-PKV Wed Nov 13 12:34:43 2002 f
  d2=d;
  if (Duv2>aMinDiv2) {
    d2 = Abs(ResoUV2/Duv2); 
    d2 = Min(Sqrt(d2)*tolArea,d); 
  }
  //d2 = Abs(ResoUV2/Duv2); 
  //d2 = Min(Sqrt(d2)*tolArea,d);  
  //modified by NIZNHY-PKV Wed Nov 13 12:34:53 2002 t
  tolCoeff2 = Exp(d2);
  CosRef1 = CosRef2D/tolCoeff1;
  CosRef2 = CosRef2D/tolCoeff2;
  //
  //==================================================================================
  //== The points are not confused :                                           ==
  //== D e t e c t    t h e   S t o p   a  t   p r e v i o u s  p o i n t ==
  //==                           N o t    T o o    G r e a t (angle in space UV)    ==
  //==                           C h a n g e    o f    s i d e                ==
  //==================================================================================
  if (aStatus != IntWalk_PointConfondu) {
    if(Cosi1*Cosi1 < CosRef1*Duv1 || Cosi2*Cosi2 < CosRef2*Duv2) {
      pasuv[0]*=0.5;  pasuv[1]*=0.5;  pasuv[2]*=0.5;  pasuv[3]*=0.5;
      if (pasuv[0]<ResoU1 && pasuv[1]<ResoV1 && pasuv[2]<ResoU2 && pasuv[3]<ResoV2) { 
        return(IntWalk_ArretSurPointPrecedent);
      }
      else {
        pasuv[0]*=0.5; pasuv[1]*=0.5; pasuv[2]*=0.5; pasuv[3]*=0.5;
        return(IntWalk_PasTropGrand);
      }
    }
    const gp_Dir2d& Tg2dcourante1 = myIntersectionOn2S.DirectionOnS1();
    const gp_Dir2d& Tg2dcourante2 = myIntersectionOn2S.DirectionOnS2();
    Cosi1 = Du1*Tg2dcourante1.X() + Dv1*Tg2dcourante1.Y();
    Cosi2 = Du2*Tg2dcourante2.X() + Dv2*Tg2dcourante2.Y();
    Ang1 = Abs(previousd1.Angle(Tg2dcourante1));  
    Ang2 = Abs(previousd2.Angle(Tg2dcourante2));  
    AngRef1 = AngRef2D*tolCoeff1;
    AngRef2 = AngRef2D*tolCoeff2;
    //-------------------------------------------------------
    //-- Test : Angle too great in space UV       -----
    //--        Change of  side                      -----
    //-------------------------------------------------------
    if(Cosi1*Cosi1 < CosRef1*Duv1 || Cosi2*Cosi2 < CosRef2*Duv2 || Ang1 > AngRef1 || Ang2 > AngRef2) {
      pasuv[0]*=0.5;  pasuv[1]*=0.5;  pasuv[2]*=0.5;  pasuv[3]*=0.5;
      if (pasuv[0]<ResoU1 && pasuv[1]<ResoV1 && pasuv[2]<ResoU2 && pasuv[3]<ResoV2) 
        return(IntWalk_ArretSurPoint);
      else 
        return(IntWalk_PasTropGrand);
    }
  }
  //<-OCC431(apo)
  //==================================================================================
  //== D e t e c t i o n   o f    :  Step Too Small 
  //==                               STEP TOO Great 
  //==================================================================================

  //---------------------------------------
  //-- Estimate of the vector           --
  //---------------------------------------
  FlecheCourante =
    Sqrt(Abs((previousd.XYZ()-TgCourante.XYZ()).SquareModulus()*aSqDist))/8.;

  if ( FlecheCourante<= fleche*0.5) {     //-- Current step too small
    if(FlecheCourante>1e-16) { 
      Ratio = 0.5*(fleche/FlecheCourante);
    }
    else { 
      Ratio = 10.0;
    }
    Standard_Real pasSu1 = pasuv[0];
    Standard_Real pasSv1 = pasuv[1];
    Standard_Real pasSu2 = pasuv[2];
    Standard_Real pasSv2 = pasuv[3];

    //-- In  case if 
    //-- a point at U+DeltaU is required, ....
    //-- return a point at U + Epsilon
    //-- Epsilon << DeltaU.

    if(pasuv[0]< AbsDu1) pasuv[0] = AbsDu1;
    if(pasuv[1]< AbsDv1) pasuv[1] = AbsDv1;
    if(pasuv[2]< AbsDu2) pasuv[2] = AbsDu2;
    if(pasuv[3]< AbsDv2) pasuv[3] = AbsDv2;

    if(pasuv[0]<ResoU1) pasuv[0]=ResoU1;
    if(pasuv[1]<ResoV1) pasuv[1]=ResoV1;
    if(pasuv[2]<ResoU2) pasuv[2]=ResoU2;
    if(pasuv[3]<ResoV2) pasuv[3]=ResoV2;
    //-- if(Ratio>10.0 ) { Ratio=10.0; } 
    Standard_Real R1,R = pasInit[0]/pasuv[0];
    R1= pasInit[1]/pasuv[1];     if(R1<R) R=R1;
    R1= pasInit[2]/pasuv[2];     if(R1<R) R=R1;
    R1= pasInit[3]/pasuv[3];     if(R1<R) R=R1;
    if(Ratio > R) Ratio=R;
    pasuv[0] = Min(Ratio*pasuv[0],pasInit[0]);
    pasuv[1] = Min(Ratio*pasuv[1],pasInit[1]);
    pasuv[2] = Min(Ratio*pasuv[2],pasInit[2]);
    pasuv[3] = Min(Ratio*pasuv[3],pasInit[3]);
    if (pasuv[0] != pasSu1 || pasuv[2] != pasSu2|| 
      pasuv[1] != pasSv1 || pasuv[3] != pasSv2) {
        if(++STATIC_BLOCAGE_SUR_PAS_TROP_GRAND > 5) {
          STATIC_BLOCAGE_SUR_PAS_TROP_GRAND = 0;
          return IntWalk_PasTropGrand; 
        }
    }
    if(aStatus == IntWalk_OK) {
      STATIC_BLOCAGE_SUR_PAS_TROP_GRAND=0;
      //-- Try to increase the step
    }
    return aStatus;
  }
  else {                                //-- CurrentVector > vector*0.5 
    if (FlecheCourante > fleche) {      //-- Current step too Great
      Ratio = fleche/FlecheCourante; 
      pasuv[0] = Ratio*pasuv[0];
      pasuv[1] = Ratio*pasuv[1];
      pasuv[2] = Ratio*pasuv[2];
      pasuv[3] = Ratio*pasuv[3];
      //if(++STATIC_BLOCAGE_SUR_PAS_TROP_GRAND > 5) {
      //	STATIC_BLOCAGE_SUR_PAS_TROP_GRAND = 0;
      return IntWalk_PasTropGrand; 
      //}
    }
    else {                             //-- vector/2  <  CurrentVector <= vector   
      Ratio = 0.75 * (fleche / FlecheCourante);
    }
  }

  if(aStatus != IntWalk_PointConfondu)
  {
    //Here, aCosBetweenTangent >= 0.0 definitely.

    /*
    Brief algorithm description.
    We have two (not-coincindent) intersection point (P1 and P2). In every point,
    vector of tangent (to the intersection curve) is known (vectors T1 and T2).
    Basing on these data, we create osculating circle.

                                    * - arc of osculating circle
                                *      * 
                           P1 x----------x P2
                             /            \
                            /              \
                          Vec(T1)         Vec(T2)

    Let me draw your attention to the following facts:
      1. Vectors T1 and T2 direct FROM (not TO) points P1 and P2. Therefore,
        one of previously computed vector should be reversed.

    In this case, the absolute (!) value of the deflection between the arc of
    the osculating circle and the P1P2 segment can be computed as follows:
          e = d*(1-sin(B/2))/(2*cos(B/2)),      (1)
    where d is the length of P1P2 segment, B is the angle between vectors T1 and T2.
    At that,
              pi/2 <= B <= pi,
              cos(B/2) >= 0,
              sin(B/2) > 0,
              sin(B) > 0,
              cos(B) < 0.

    Later, the normal state of algorithm work is (as we apply) 
              tolconf/2 <= e <= tolconf.
    In this case, we keep previous step.

    If e < tolconf/2 then the local curvature of the intersection curve is small.
    As result, the step should be increased.

    If e > tolconf then the step is too big. Therefore, we should decrease one.

    Condition (1) is equivalent to
              sin(B/2) = 1 - 2/(1+(d/(2*e))^2) = Fs(e),
              cos(B) = 1 - 2*Fs(e)^2 = Fd(e),
    where Fs(e)and Fd(e) are some function with parameter "deflection".

    Let mean that Fs(e) is decreasing function. Fd(e) is increasing function,
    in the range, where Fs(e) > 0.0 (i.e. when e < d/2).

    Now, let substitute required deflection (tolconf or tolconf/2) to z. Then
    it is necessary to check if e < z or if e > z.

    In this case, it is enough to comapare Fs(e) and Fs(z).
    At that Fs(e) > 0 because sin(B/2) > 0 always.

    Therefore, if Fs(z) < 0.0 then Fs(e) > Fs(z) ==> e < z definitely.
    If Fs(z) > 0.0 then we can compare Fs(z)^2 and Fs(e)^2 or, in substance,
    values Fd(e) and Fd(z). If Fd(e) > Fd(z) then e > z and vice versa.
    */

    //Fd(e) is already known (Fd(e) == -aCosBetweenTangent)

    const Standard_Real anInvSqAbsArcDeflMax = 0.25*aSqDist/(tolconf*tolconf);
    const Standard_Real aSinB2Max = 1.0 - 2.0/(1.0 + anInvSqAbsArcDeflMax);

    if(aSinB2Max >= 0.0 && (aCosBetweenTangent <= 2.0 * aSinB2Max * aSinB2Max - 1.0))
    {//Real deflection is greater or equal than tolconf
      aStatus = IntWalk_PasTropGrand;
    }
    else
    {//Real deflection is less than tolconf
      const Standard_Real anInvSqAbsArcDeflMin = 4.0*anInvSqAbsArcDeflMax;
      const Standard_Real aSinB2Min = 1.0 - 2.0/(1.0 + anInvSqAbsArcDeflMin);

      if((aSinB2Min < 0.0) || (aCosBetweenTangent >= 2.0 * aSinB2Min * aSinB2Min - 1.0))
      {//Real deflection is less than tolconf/2.0
        aStatus = IntWalk_StepTooSmall;
      }
    }

    if(aStatus == IntWalk_PasTropGrand)
    {
      pasuv[0]*=0.5; pasuv[1]*=0.5; pasuv[2]*=0.5; pasuv[3]*=0.5;
      return aStatus;
    }

    if(aStatus == IntWalk_StepTooSmall)
    {
      pasuv[0] = Max(pasuv[0], AbsDu1);
      pasuv[1] = Max(pasuv[1], AbsDv1);
      pasuv[2] = Max(pasuv[2], AbsDu2);
      pasuv[3] = Max(pasuv[3], AbsDv2);

      pasInit[0] = Max(pasInit[0], AbsDu1);
      pasInit[1] = Max(pasInit[1], AbsDv1);
      pasInit[2] = Max(pasInit[2], AbsDu2);
      pasInit[3] = Max(pasInit[3], AbsDv2);

      return aStatus;
    }
  }

  pasuv[0] = Max(myStepMin[0],Min(Min(Ratio*AbsDu1,pasuv[0]),pasInit[0]));
  pasuv[1] = Max(myStepMin[1],Min(Min(Ratio*AbsDv1,pasuv[1]),pasInit[1]));
  pasuv[2] = Max(myStepMin[2],Min(Min(Ratio*AbsDu2,pasuv[2]),pasInit[2]));
  pasuv[3] = Max(myStepMin[3],Min(Min(Ratio*AbsDv2,pasuv[3]),pasInit[3]));

  if(aStatus == IntWalk_OK) STATIC_BLOCAGE_SUR_PAS_TROP_GRAND=0;
  return aStatus;
}

Standard_Boolean IntWalk_PWalking::
TestArret(const Standard_Boolean DejaReparti,
          TColStd_Array1OfReal& Param,
          IntImp_ConstIsoparametric&  ChoixIso)

          //
          // test if the point of intersection set by these parameters remains in the 
          // natural domain of each square.
          // if the point outpasses reframe to find the best iso (border)
          // that intersects easiest the other square
          // otherwise test if closed line is present  
          // 
{
  Standard_Real Uvd[4],Uvf[4],Epsuv[4],Duv[4],Uvp[4],dv,dv2,ParC[4];
  Standard_Real DPc,DPb;
  Standard_Integer i = 0, k = 0;
  Epsuv[0] = ResoU1;
  Epsuv[1] = ResoV1;
  Epsuv[2] = ResoU2;
  Epsuv[3] = ResoV2;
  previousPoint.Parameters(Uvp[0],Uvp[1],Uvp[2],Uvp[3]);

  Standard_Real SolParam[4];
  myIntersectionOn2S.Point().Parameters(SolParam[0],SolParam[1],SolParam[2],SolParam[3]);

  Standard_Boolean Trouve = Standard_False;

  Uvd[0]=Um1;   Uvf[0]=UM1;   Uvd[1]=Vm1;   Uvf[1]=VM1;
  Uvd[2]=Um2;   Uvf[2]=UM2;   Uvd[3]=Vm2;   Uvf[3]=VM2;

  Standard_Integer im1;
  for ( i = 1,im1 = 0;i<=4;i++,im1++) {
    switch(i) { 
    case 1: k=2; break;
    case 2: k=1; break;
    case 3: k=4; break;
    case 4: k=3; break;
    }
    if (Param(i) < (Uvd[im1]-Epsuv[im1]) ||
      SolParam[im1] < (Uvd[im1]-Epsuv[im1]))     //--     Current -----  Bound Inf -----  Previous
    {
      Trouve    = Standard_True;                   //-- 
      DPc       = Uvp[im1]-Param(i);               //--     Previous  - Current
      DPb       = Uvp[im1]-Uvd[im1];               //--     Previous  - Bound Inf
      ParC[im1] = Uvd[im1];                        //--     ParamCorrige
      dv        = Param(k)-Uvp[k-1];               //--     Current   - Previous (other Direction)
      dv2       = dv*dv;         
      if(dv2>RealEpsilon()) {                       //--    Progress at the other Direction ?
        Duv[im1]  = DPc*DPb + dv2;
        Duv[im1]  = Duv[im1]*Duv[im1]/(DPc*DPc+dv2)/(DPb*DPb+dv2);
      }
      else {
        Duv[im1]=-1.0;                              //--    If no progress, do not change  
      }                                             //--    the choice of iso 
    }   
    else if (Param(i) > (Uvf[im1] + Epsuv[im1]) ||
      SolParam[im1] > (Uvf[im1] + Epsuv[im1]))//--    Previous -----  Bound Sup -----  Current
    {
      Trouve    = Standard_True;                    //-- 
      DPc       = Param(i)-Uvp[im1];                //--     Current   - Previous
      DPb       = Uvf[im1]-Uvp[im1];                //--     Bound Sup - Previous 
      ParC[im1] = Uvf[im1];                         //--     Param Corrige
      dv        = Param(k)-Uvp[k-1];                //--     Current   - Previous (other Direction)
      dv2       = dv*dv;
      if(dv2>RealEpsilon()) {                       //--     Progress in other Direction ?
        Duv[im1]  =  DPc*DPb + dv2;
        Duv[im1]  = Duv[im1]*Duv[im1]/(DPc*DPc+dv2)/(DPb*DPb+dv2);
      }
      else {
        Duv[im1]=-1.0;                              //--    If no progress, do not change 
      }                                             //--    the choice of iso 
    }
    else { 
      Duv[im1]= -1.;
      ParC[im1]=Param(i);
    }
  }

  if (Trouve) {
    //--------------------------------------------------
    //-- One of Parameters u1,v1,u2,v2 is outside of  --
    //-- the natural limits.                          -- 
    //-- Find the best direction of                   -- 
    //-- progress and reframe the parameters.        --
    //--------------------------------------------------
    Standard_Real ddv = -1.0;
    k=-1;
    for (i=0;i<=3;i++) {
      Param(i+1) = ParC[i];
      if(Duv[i]>ddv) { 
        ddv = Duv[i];
        k=i;
      }
    }
    if(k!=-1) { 
      ChoixIso   = ChoixRef[k];
    }
    else { 
      if((ParC[0]<=Uvd[0]+Epsuv[0]) || (ParC[0]>=Uvf[0]-Epsuv[0])) {
        ChoixIso = IntImp_UIsoparametricOnCaro1;
      }
      else if((ParC[1]<=Uvd[1]+Epsuv[1]) || (ParC[1]>=Uvf[1]-Epsuv[1])) {
        ChoixIso = IntImp_VIsoparametricOnCaro1;
      }
      else if((ParC[2]<=Uvd[2]+Epsuv[2]) || (ParC[2]>=Uvf[2]-Epsuv[2])) {
        ChoixIso = IntImp_UIsoparametricOnCaro2;
      }
      else if((ParC[3]<=Uvd[3]+Epsuv[3]) || (ParC[3]>=Uvf[3]-Epsuv[3])) {
        ChoixIso = IntImp_VIsoparametricOnCaro2;
      }
    }
    close = Standard_False;
    return Standard_True;
  }
  else 
  {  
    if (!DejaReparti) { // find if line closed

      Standard_Real u,v;
      const IntSurf_PntOn2S& POn2S1=line->Value(1);
      //On S1
      POn2S1.ParametersOnS1(u,v);
      gp_Pnt2d P1uvS1(u,v);
      previousPoint.ParametersOnS1(u,v);
      gp_Pnt2d PrevuvS1(u,v);
      myIntersectionOn2S.Point().ParametersOnS1(u,v);
      gp_Pnt2d myIntersuvS1(u,v);
      Standard_Boolean close2dS1 = (P1uvS1.XY()-PrevuvS1.XY())*
        (P1uvS1.XY()-myIntersuvS1.XY()) < 0.0;
      //On S2
      POn2S1.ParametersOnS2(u,v);
      gp_Pnt2d P1uvS2(u,v);
      previousPoint.ParametersOnS2(u,v);
      gp_Pnt2d PrevuvS2(u,v);
      myIntersectionOn2S.Point().ParametersOnS2(u,v);
      gp_Pnt2d myIntersuvS2(u,v);
      Standard_Boolean close2dS2 = (P1uvS2.XY()-PrevuvS2.XY())*
        (P1uvS2.XY()-myIntersuvS2.XY()) < 0.0;

      close = close2dS1 && close2dS2;
      return close;
    }
    else return Standard_False;
  }
}

