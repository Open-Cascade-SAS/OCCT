// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
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

#include <algorithm>
#include <GeomInt_IntSS.hxx>

#include <Adaptor3d_TopolTool.hxx>
#include <Approx_CurveOnSurface.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtPS.hxx>
#include <Geom2dAdaptor.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomAdaptor.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomInt.hxx>
#include <GeomInt_LineTool.hxx>
#include <GeomInt_WLApprox.hxx>
#include <GeomLib_Check2dBSplineCurve.hxx>
#include <GeomLib_CheckBSplineCurve.hxx>
#include <GeomProjLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_WLine.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <IntSurf_Quadric.hxx>
#include <Precision.hxx>

//=======================================================================
//function : AdjustUPeriodic
//purpose  : 
//=======================================================================
 static void AdjustUPeriodic (const Handle(Geom_Surface)& aS, const Handle(Geom2d_Curve)& aC2D)
{
  if (aC2D.IsNull() || !aS->IsUPeriodic())
    return;
  //
  const Standard_Real aEps=Precision::PConfusion();//1.e-9
  const Standard_Real aEpsilon=Epsilon(10.);//1.77e-15 
  //
  Standard_Real umin,umax,vmin,vmax;
  aS->Bounds(umin,umax,vmin,vmax);
  const Standard_Real aPeriod = aS->UPeriod();
  
  const Standard_Real aT1=aC2D->FirstParameter();
  const Standard_Real aT2=aC2D->LastParameter();
  const Standard_Real aTx=aT1+0.467*(aT2-aT1);
  const gp_Pnt2d aPx=aC2D->Value(aTx);
  //
  Standard_Real aUx=aPx.X();
  if (fabs(aUx)<aEpsilon)
    aUx=0.;
  if (fabs(aUx-aPeriod)<aEpsilon)
    aUx=aPeriod;
  //
  Standard_Real dU=0.;
  while(aUx <(umin-aEps)) {
    aUx+=aPeriod;
    dU+=aPeriod;
  }
  while(aUx>(umax+aEps)) {
    aUx-=aPeriod;
    dU-=aPeriod;
  }
  // 
  if (dU!=0.) {
    gp_Vec2d aV2D(dU, 0.);
    aC2D->Translate(aV2D);
  }
}

 //=======================================================================
//function : GetQuadric
//purpose  : 
//=======================================================================
 static void GetQuadric(const Handle(GeomAdaptor_HSurface)& HS1, IntSurf_Quadric& quad1)
{
  switch (HS1->Surface().GetType())
  {
    case GeomAbs_Plane:    quad1.SetValue(HS1->Surface().Plane()); break;
    case GeomAbs_Cylinder: quad1.SetValue(HS1->Surface().Cylinder()); break;
    case GeomAbs_Cone:     quad1.SetValue(HS1->Surface().Cone()); break;
    case GeomAbs_Sphere:   quad1.SetValue(HS1->Surface().Sphere()); break;
    case GeomAbs_Torus:    quad1.SetValue(HS1->Surface().Torus()); break;
    default: throw Standard_ConstructionError("GeomInt_IntSS::MakeCurve");
  }
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================
 static void Parameters(  const Handle(GeomAdaptor_HSurface)& HS1,
                         const Handle(GeomAdaptor_HSurface)& HS2,
                         const gp_Pnt& Ptref,
                         Standard_Real& U1,
                         Standard_Real& V1,
                         Standard_Real& U2,
                         Standard_Real& V2)
{
  IntSurf_Quadric quad1,quad2;
  //
  GetQuadric(HS1, quad1);
  GetQuadric(HS2, quad2);
  //
  quad1.Parameters(Ptref,U1,V1);
  quad2.Parameters(Ptref,U2,V2);
}

//=======================================================================
//function : ParametersOfNearestPointOnSurface
//purpose  : 
//=======================================================================
static Standard_Boolean ParametersOfNearestPointOnSurface(const Extrema_ExtPS theExtr,
                                                          Standard_Real& theU,
                                                          Standard_Real& theV)
{
  if(!theExtr.IsDone() || !theExtr.NbExt())
    return Standard_False;

  Standard_Integer anIndex = 1;
  Standard_Real aMinSQDist = theExtr.SquareDistance(anIndex);
  for(Standard_Integer i = 2; i <= theExtr.NbExt(); i++)
  {
    Standard_Real aSQD = theExtr.SquareDistance(i);
    if (aSQD < aMinSQDist)
    {
      aMinSQDist = aSQD;
      anIndex = i;
    }
  }

  theExtr.Point(anIndex).Parameter(theU, theV);

  return Standard_True;
}

//=======================================================================
//function : GetSegmentBoundary
//purpose  : 
//=======================================================================
static void GetSegmentBoundary( const IntRes2d_IntersectionSegment& theSegm,
                                const Handle(Geom2d_Curve)& theCurve,
                                GeomInt_VectorOfReal& theArrayOfParameters)
{
  Standard_Real aU1 = theCurve->FirstParameter(), aU2 = theCurve->LastParameter();

  if(theSegm.HasFirstPoint())
  {
    const IntRes2d_IntersectionPoint& anIPF = theSegm.FirstPoint();
    aU1 = anIPF.ParamOnFirst();
  }

  if(theSegm.HasLastPoint())
  {
    const IntRes2d_IntersectionPoint& anIPL = theSegm.LastPoint();
    aU2 = anIPL.ParamOnFirst();
  }

  theArrayOfParameters.Append(aU1);
  theArrayOfParameters.Append(aU2);
}

//=======================================================================
//function : IntersectCurveAndBoundary
//purpose  : 
//=======================================================================
static void IntersectCurveAndBoundary(const Handle(Geom2d_Curve)& theC2d,
                                      const Handle(Geom2d_Curve)* const theArrBounds,
                                      const Standard_Integer theNumberOfCurves,
                                      const Standard_Real theTol,
                                      GeomInt_VectorOfReal& theArrayOfParameters)
{
  if(theC2d.IsNull())
    return;

  Geom2dAdaptor_Curve anAC1(theC2d);
  for(Standard_Integer aCurID = 0; aCurID < theNumberOfCurves; aCurID++)
  {
    if(theArrBounds[aCurID].IsNull())
      continue;

    Geom2dAdaptor_Curve anAC2(theArrBounds[aCurID]);
    Geom2dInt_GInter anIntCC2d(anAC1, anAC2, theTol, theTol);

    if(!anIntCC2d.IsDone() || anIntCC2d.IsEmpty())
      continue;

    for (Standard_Integer aPntID = 1; aPntID <= anIntCC2d.NbPoints(); aPntID++)
    {
      const Standard_Real aParam = anIntCC2d.Point(aPntID).ParamOnFirst();
      theArrayOfParameters.Append(aParam);
    }

    for (Standard_Integer aSegmID = 1; aSegmID <= anIntCC2d.NbSegments(); aSegmID++)
    {
      GetSegmentBoundary(anIntCC2d.Segment(aSegmID), theC2d, theArrayOfParameters);
    }
  }
}

//=======================================================================
//function : isDegenerated
//purpose  : Check if theAHC2d corresponds to a degenerated edge.
//=======================================================================
static Standard_Boolean isDegenerated(const Handle(GeomAdaptor_HSurface)& theGAHS,
                                      const Handle(Adaptor2d_HCurve2d)& theAHC2d,
                                      const Standard_Real theFirstPar,
                                      const Standard_Real theLastPar)
{
  const Standard_Real aSqTol = Precision::Confusion()*Precision::Confusion();
  gp_Pnt2d aP2d;
  gp_Pnt aP1, aP2;

  theAHC2d->D0(theFirstPar, aP2d);
  theGAHS->D0(aP2d.X(), aP2d.Y(), aP1);

  theAHC2d->D0(theLastPar, aP2d);
  theGAHS->D0(aP2d.X(), aP2d.Y(), aP2);

  if(aP1.SquareDistance(aP2) > aSqTol)
    return Standard_False;

  theAHC2d->D0(0.5*(theFirstPar+theLastPar), aP2d);
  theGAHS->D0(aP2d.X(), aP2d.Y(), aP2);

  if(aP1.SquareDistance(aP2) > aSqTol)
    return Standard_False;

  return Standard_True;
}

//=======================================================================
//function : MakeCurve
//purpose  : 
//=======================================================================
void GeomInt_IntSS::MakeCurve(const Standard_Integer Index,
			                        const Handle(Adaptor3d_TopolTool) & dom1,
			                        const Handle(Adaptor3d_TopolTool) & dom2,
			                        const Standard_Real Tol,
			                        const Standard_Boolean Approx,
			                        const Standard_Boolean ApproxS1,
			                        const Standard_Boolean ApproxS2)

{
  Standard_Boolean myApprox1, myApprox2, myApprox;
  Standard_Real Tolpc, myTolApprox;
  IntPatch_IType typl;
  Handle(Geom2d_BSplineCurve) H1;
  Handle(Geom_Surface) aS1, aS2;
  //
  Tolpc = Tol;
  myApprox=Approx;
  myApprox1=ApproxS1;
  myApprox2=ApproxS2;
  myTolApprox=0.0000001;
  //
  aS1=myHS1->ChangeSurface().Surface();
  aS2=myHS2->ChangeSurface().Surface();
  //
  Handle(IntPatch_Line) L = myIntersector.Line(Index);
  typl = L->ArcType();
  //
  if(typl==IntPatch_Walking) {
    Handle(IntPatch_WLine) aWLine (Handle(IntPatch_WLine)::DownCast(L));
    if(aWLine.IsNull()) {
      return;
    }
    L = aWLine;
  }
  //
  // Line Constructor
  myLConstruct.Perform(L);
  if (!myLConstruct.IsDone() || myLConstruct.NbParts() <= 0) {
    return;
  }
  // Do the Curve
  Standard_Boolean ok;
  Standard_Integer i, j,  aNbParts;
  Standard_Real fprm, lprm;
  Handle(Geom_Curve) newc;

  switch (typl) {
    //########################################  
    // Line, Parabola, Hyperbola
    //########################################  
  case IntPatch_Lin:
  case IntPatch_Parabola: 
  case IntPatch_Hyperbola: {
    if (typl == IntPatch_Lin) {
      newc=new Geom_Line (Handle(IntPatch_GLine)::DownCast(L)->Line());
    }
    else if (typl == IntPatch_Parabola) {
      newc=new Geom_Parabola(Handle(IntPatch_GLine)::DownCast(L)->Parabola());
    }
    else if (typl == IntPatch_Hyperbola) {
      newc=new Geom_Hyperbola (Handle(IntPatch_GLine)::DownCast(L)->Hyperbola());
    }
    //
    aNbParts=myLConstruct.NbParts();
    for (i=1; i<=aNbParts; i++) {
      myLConstruct.Part(i, fprm, lprm);

      if (!Precision::IsNegativeInfinite(fprm) && 
        !Precision::IsPositiveInfinite(lprm)) {
          Handle(Geom_TrimmedCurve) aCT3D=new Geom_TrimmedCurve(newc, fprm, lprm);
          sline.Append(aCT3D);
          //
          if(myApprox1) { 
            Handle (Geom2d_Curve) C2d;
            BuildPCurves(fprm, lprm, Tolpc, myHS1->ChangeSurface().Surface(), newc, C2d);
            if(Tolpc>myTolReached2d || myTolReached2d==0.) { 
              myTolReached2d=Tolpc;
            }
            slineS1.Append(new Geom2d_TrimmedCurve(C2d,fprm,lprm));
          }
          else { 
            slineS1.Append(H1);
          }
          //
          if(myApprox2) { 
            Handle (Geom2d_Curve) C2d;
            BuildPCurves(fprm,lprm,Tolpc,myHS2->ChangeSurface().Surface(),newc,C2d);
            if(Tolpc>myTolReached2d || myTolReached2d==0.) { 
              myTolReached2d=Tolpc;
            }
            //
            slineS2.Append(new Geom2d_TrimmedCurve(C2d,fprm,lprm));
          }
          else { 
            slineS2.Append(H1);
          }
      } // if (!Precision::IsNegativeInfinite(fprm) &&  !Precision::IsPositiveInfinite(lprm))

      else {
        GeomAbs_SurfaceType typS1 = myHS1->Surface().GetType();
        GeomAbs_SurfaceType typS2 = myHS2->Surface().GetType();
        if( typS1 == GeomAbs_SurfaceOfExtrusion ||
          typS1 == GeomAbs_OffsetSurface ||
          typS1 == GeomAbs_SurfaceOfRevolution ||
          typS2 == GeomAbs_SurfaceOfExtrusion ||
          typS2 == GeomAbs_OffsetSurface ||
          typS2 == GeomAbs_SurfaceOfRevolution) {
            sline.Append(newc);
            slineS1.Append(H1);
            slineS2.Append(H1);
            continue;
        }
        Standard_Boolean bFNIt, bLPIt;
        Standard_Real aTestPrm, dT=100.;
        Standard_Real u1, v1, u2, v2, TolX;
        //
        bFNIt=Precision::IsNegativeInfinite(fprm);
        bLPIt=Precision::IsPositiveInfinite(lprm);

        aTestPrm=0.;

        if (bFNIt && !bLPIt) {
          aTestPrm=lprm-dT;
        }
        else if (!bFNIt && bLPIt) {
          aTestPrm=fprm+dT;
        }
        //
        gp_Pnt ptref(newc->Value(aTestPrm));
        //
        TolX = Precision::Confusion();
        Parameters(myHS1, myHS2, ptref,  u1, v1, u2, v2);
        ok = (dom1->Classify(gp_Pnt2d(u1, v1), TolX) != TopAbs_OUT);
        if(ok) { 
          ok = (dom2->Classify(gp_Pnt2d(u2,v2),TolX) != TopAbs_OUT); 
        }
        if (ok) {
          sline.Append(newc);
          slineS1.Append(H1);
          slineS2.Append(H1);
        }
      }
    }// end of for (i=1; i<=myLConstruct.NbParts(); i++)
  }// case IntPatch_Lin:  case IntPatch_Parabola:  case IntPatch_Hyperbola:
  break;

                           //########################################  
                           // Circle and Ellipse
                           //########################################  
  case IntPatch_Circle: 
  case IntPatch_Ellipse: {

    if (typl == IntPatch_Circle) {
      newc = new Geom_Circle
        (Handle(IntPatch_GLine)::DownCast(L)->Circle());
    }
    else { 
      newc = new Geom_Ellipse
        (Handle(IntPatch_GLine)::DownCast(L)->Ellipse());
    }
    //
    Standard_Real aPeriod, aRealEpsilon;
    //
    aRealEpsilon=RealEpsilon();
    aPeriod=M_PI+M_PI;
    //
    aNbParts=myLConstruct.NbParts();
    //
    for (i=1; i<=aNbParts; i++) {
      myLConstruct.Part(i, fprm, lprm);
      //
      if (Abs(fprm) > aRealEpsilon || Abs(lprm-aPeriod) > aRealEpsilon) {
        //==============================================
        Handle(Geom_TrimmedCurve) aTC3D=new Geom_TrimmedCurve(newc,fprm,lprm);
        //
        sline.Append(aTC3D);
        //
        fprm=aTC3D->FirstParameter();
        lprm=aTC3D->LastParameter ();
        //// 	
        if(myApprox1) { 
          Handle (Geom2d_Curve) C2d;
          BuildPCurves(fprm,lprm,Tolpc,myHS1->ChangeSurface().Surface(),newc,C2d);
          if(Tolpc>myTolReached2d || myTolReached2d==0.) { 
            myTolReached2d=Tolpc;
          }
          slineS1.Append(C2d);
        }
        else { //// 
          slineS1.Append(H1);
        }
        //
        if(myApprox2) { 
          Handle (Geom2d_Curve) C2d;
          BuildPCurves(fprm,lprm,Tolpc,myHS2->ChangeSurface().Surface(),newc,C2d);
          if(Tolpc>myTolReached2d || myTolReached2d==0) { 
            myTolReached2d=Tolpc;
          }
          slineS2.Append(C2d);
        }
        else { 
          slineS2.Append(H1);  
        }
        //==============================================	
      } //if (Abs(fprm) > RealEpsilon() || Abs(lprm-2.*M_PI) > RealEpsilon())
      //
      else {//  on regarde si on garde
        //
        if (aNbParts==1) {
          if (Abs(fprm) < RealEpsilon() &&  Abs(lprm-2.*M_PI) < RealEpsilon()) {
            Handle(Geom_TrimmedCurve) aTC3D=new Geom_TrimmedCurve(newc,fprm,lprm);
            //
            sline.Append(aTC3D);
            fprm=aTC3D->FirstParameter();
            lprm=aTC3D->LastParameter ();

            if(myApprox1) { 
              Handle (Geom2d_Curve) C2d;
              BuildPCurves(fprm,lprm,Tolpc,myHS1->ChangeSurface().Surface(),newc,C2d);
              if(Tolpc>myTolReached2d || myTolReached2d==0) { 
                myTolReached2d=Tolpc;
              }
              slineS1.Append(C2d);
            }
            else { //// 
              slineS1.Append(H1);
            }

            if(myApprox2) { 
              Handle (Geom2d_Curve) C2d;
              BuildPCurves(fprm,lprm,Tolpc,myHS2->ChangeSurface().Surface(),newc,C2d);
              if(Tolpc>myTolReached2d || myTolReached2d==0) { 
                myTolReached2d=Tolpc;
              }
              slineS2.Append(C2d);
            }
            else { 
              slineS2.Append(H1);
            }
            break;
          }
        }
        //
        Standard_Real aTwoPIdiv17, u1, v1, u2, v2, TolX;
        //
        aTwoPIdiv17=2.*M_PI/17.;
        //
        for (j=0; j<=17; j++) {
          gp_Pnt ptref (newc->Value (j*aTwoPIdiv17));
          TolX = Precision::Confusion();

          Parameters(myHS1, myHS2, ptref, u1, v1, u2, v2);
          ok = (dom1->Classify(gp_Pnt2d(u1,v1),TolX) != TopAbs_OUT);
          if(ok) { 
            ok = (dom2->Classify(gp_Pnt2d(u2,v2),TolX) != TopAbs_OUT);
          }
          if (ok) {
            sline.Append(newc);
            //==============================================
            if(myApprox1) { 
              Handle (Geom2d_Curve) C2d;
              BuildPCurves(fprm, lprm, Tolpc, myHS1->ChangeSurface().Surface(), newc, C2d);
              if(Tolpc>myTolReached2d || myTolReached2d==0) { 
                myTolReached2d=Tolpc;
              }
              slineS1.Append(C2d);
            }
            else { 
              slineS1.Append(H1);  
            }

            if(myApprox2) { 
              Handle (Geom2d_Curve) C2d;
              BuildPCurves(fprm, lprm, Tolpc,myHS2->ChangeSurface().Surface(), newc, C2d);
              if(Tolpc>myTolReached2d || myTolReached2d==0) { 
                myTolReached2d=Tolpc;
              }	
              slineS2.Append(C2d);
            }
            else { 
              slineS2.Append(H1);  
            }
            break;
          }//  end of if (ok) {
        }//  end of for (Standard_Integer j=0; j<=17; j++)
      }//  end of else { on regarde si on garde
    }// for (i=1; i<=myLConstruct.NbParts(); i++)
  }// IntPatch_Circle: IntPatch_Ellipse
  break;

                         //########################################  
                         // Analytic
                         //######################################## 
  case IntPatch_Analytic:
    //This case was processed earlier (in IntPatch_Intersection)

  break;

  //########################################  
  // Walking
  //######################################## 
  case IntPatch_Walking:{
    Handle(IntPatch_WLine) WL = 
      Handle(IntPatch_WLine)::DownCast(L);

#ifdef GEOMINT_INTSS_DEBUG
    WL->Dump(0);
#endif

    //
    Standard_Integer ifprm, ilprm;
    //
    if (!myApprox) {
      aNbParts=myLConstruct.NbParts();
      for (i=1; i<=aNbParts; i++) {
        myLConstruct.Part(i, fprm, lprm);
        ifprm=(Standard_Integer)fprm;
        ilprm=(Standard_Integer)lprm;
        // 	  
        Handle(Geom2d_BSplineCurve) aH1, aH2;

        if(myApprox1) {
          aH1 = MakeBSpline2d(WL, ifprm, ilprm, Standard_True);
        }
        if(myApprox2) {
          aH2 = MakeBSpline2d(WL, ifprm, ilprm, Standard_False);
        }
        //
        Handle(Geom_Curve) aBSp=MakeBSpline(WL, ifprm, ilprm);
        // 	
        sline.Append(aBSp);
        slineS1.Append(aH1);
        slineS2.Append(aH2);
      }
    }
    //
    else {
      Standard_Boolean bIsDecomposited;
      Standard_Integer nbiter, aNbSeqOfL;
      GeomInt_WLApprox theapp3d;
      IntPatch_SequenceOfLine aSeqOfL;
      Standard_Real tol2d, aTolSS;
      // 	
      tol2d = myTolApprox;
      aTolSS=2.e-7;
      theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, 30, myHS1 != myHS2);
      //
      bIsDecomposited = 
        GeomInt_LineTool::DecompositionOfWLine(WL, myHS1, myHS2, aTolSS, myLConstruct, aSeqOfL);
      //
      aNbParts=myLConstruct.NbParts();
      aNbSeqOfL=aSeqOfL.Length();
      //
      nbiter = (bIsDecomposited) ? aNbSeqOfL : aNbParts;
      //
      for(i = 1; i <= nbiter; i++) {
        if(bIsDecomposited) {
          WL = Handle(IntPatch_WLine)::DownCast(aSeqOfL.Value(i));
          ifprm = 1;
          ilprm = WL->NbPnts();
        }
        else {
          myLConstruct.Part(i, fprm, lprm);
          ifprm = (Standard_Integer)fprm;
          ilprm = (Standard_Integer)lprm;
        }
        //-- lbr : 
        //-- Si une des surfaces est un plan , on approxime en 2d
        //-- sur cette surface et on remonte les points 2d en 3d.
        GeomAbs_SurfaceType typs1, typs2;
        typs1 = myHS1->Surface().GetType();
        typs2 = myHS2->Surface().GetType();
        //
        if(typs1 == GeomAbs_Plane) { 
          theapp3d.Perform(myHS1, myHS2, WL, Standard_False,
            Standard_True, myApprox2,
            ifprm,  ilprm);
        }	  
        else if(typs2 == GeomAbs_Plane) { 
          theapp3d.Perform(myHS1,myHS2,WL,Standard_False,
            myApprox1,Standard_True,
            ifprm,  ilprm);
        }
        else { 
          //
          if (myHS1 != myHS2){
            if ((typs1==GeomAbs_BezierSurface || typs1==GeomAbs_BSplineSurface) &&
              (typs2==GeomAbs_BezierSurface || typs2==GeomAbs_BSplineSurface)) {

                theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, 30, Standard_True);
                //Standard_Boolean bUseSurfaces;
                //bUseSurfaces=NotUseSurfacesForApprox(myFace1, myFace2, WL, ifprm,  ilprm);
                //if (bUseSurfaces) {
                //theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_False);
                //}
            }
          }
          //
          theapp3d.Perform(myHS1,myHS2,WL,Standard_True,
            myApprox1,myApprox2,
            ifprm,  ilprm);
        }

        if (!theapp3d.IsDone()) {
          // 	  
          Handle(Geom2d_BSplineCurve) aH1, aH2;
          // 	  
          Handle(Geom_Curve) aBSp=MakeBSpline(WL, ifprm, ilprm);
          if(myApprox1) {
            aH1 = MakeBSpline2d(WL, ifprm, ilprm, Standard_True);
          }
          if(myApprox2) {
            aH2 = MakeBSpline2d(WL, ifprm, ilprm, Standard_False);
          }
          //
          sline.Append(aBSp);
          slineS1.Append(aH1);
          slineS2.Append(aH2);
        }//if (!theapp3d.IsDone())

        else {
          if(myApprox1 || myApprox2 || (typs1==GeomAbs_Plane || typs2==GeomAbs_Plane)) { 
            if( theapp3d.TolReached2d()>myTolReached2d || myTolReached2d==0.) { 
              myTolReached2d = theapp3d.TolReached2d();
            }
          }
          if(typs1==GeomAbs_Plane || typs2==GeomAbs_Plane) { 
            myTolReached3d = myTolReached2d;
          }
          else  if( theapp3d.TolReached3d()>myTolReached3d || myTolReached3d==0.) { 
            myTolReached3d = theapp3d.TolReached3d();
          }

          Standard_Integer aNbMultiCurves, nbpoles;
          //
          aNbMultiCurves=theapp3d.NbMultiCurves(); 
          for (j=1; j<=aNbMultiCurves; j++) {
            if(typs1 == GeomAbs_Plane) {
              const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
              nbpoles = mbspc.NbPoles();

              TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
              TColgp_Array1OfPnt   tpoles(1,nbpoles);

              mbspc.Curve(1,tpoles2d);
              const gp_Pln&  Pln = myHS1->Surface().Plane();
              //
              Standard_Integer ik; 
              for(ik = 1; ik<= nbpoles; ik++) { 
                tpoles.SetValue(ik,
                  ElSLib::Value(tpoles2d.Value(ik).X(),
                  tpoles2d.Value(ik).Y(),
                  Pln));
              }
              //
              Handle(Geom_BSplineCurve) BS = 
                new Geom_BSplineCurve(tpoles,
                mbspc.Knots(),
                mbspc.Multiplicities(),
                mbspc.Degree());
              GeomLib_CheckBSplineCurve Check(BS,myTolCheck,myTolAngCheck);
              Check.FixTangent(Standard_True, Standard_True);
              // 	
              sline.Append(BS);
              //
              if(myApprox1) { 
                Handle(Geom2d_BSplineCurve) BS1 = 
                  new Geom2d_BSplineCurve(tpoles2d,
                  mbspc.Knots(),
                  mbspc.Multiplicities(),
                  mbspc.Degree());
                GeomLib_Check2dBSplineCurve Check1(BS1,myTolCheck,myTolAngCheck);
                Check1.FixTangent(Standard_True,Standard_True);
                // 	
                AdjustUPeriodic (aS1, BS1);  
                //
                slineS1.Append(BS1);
              }
              else {
                slineS1.Append(H1);
              }

              if(myApprox2) { 
                mbspc.Curve(2, tpoles2d);

                Handle(Geom2d_BSplineCurve) BS2 = new Geom2d_BSplineCurve(tpoles2d,
                  mbspc.Knots(),
                  mbspc.Multiplicities(),
                  mbspc.Degree());
                GeomLib_Check2dBSplineCurve newCheck(BS2,myTolCheck,myTolAngCheck);
                newCheck.FixTangent(Standard_True,Standard_True);
                //
                AdjustUPeriodic (aS2, BS2);  
                //
                slineS2.Append(BS2); 
              }
              else { 
                slineS2.Append(H1); 
              }
            }//if(typs1 == GeomAbs_Plane) 
            //
            else if(typs2 == GeomAbs_Plane) { 
              const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
              nbpoles = mbspc.NbPoles();

              TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
              TColgp_Array1OfPnt   tpoles(1,nbpoles);
              mbspc.Curve((myApprox1==Standard_True)? 2 : 1,tpoles2d);
              const gp_Pln&  Pln = myHS2->Surface().Plane();
              //
              Standard_Integer ik; 
              for(ik = 1; ik<= nbpoles; ik++) { 
                tpoles.SetValue(ik,
                  ElSLib::Value(tpoles2d.Value(ik).X(),
                  tpoles2d.Value(ik).Y(),
                  Pln));

              }
              //
              Handle(Geom_BSplineCurve) BS=new Geom_BSplineCurve(tpoles,
                mbspc.Knots(),
                mbspc.Multiplicities(),
                mbspc.Degree());
              GeomLib_CheckBSplineCurve Check(BS,myTolCheck,myTolAngCheck);
              Check.FixTangent(Standard_True,Standard_True);
              // 	
              sline.Append(BS);
              //
              if(myApprox2) {
                Handle(Geom2d_BSplineCurve) BS1=new Geom2d_BSplineCurve(tpoles2d,
                  mbspc.Knots(),
                  mbspc.Multiplicities(),
                  mbspc.Degree());
                GeomLib_Check2dBSplineCurve Check1(BS1,myTolCheck,myTolAngCheck);
                Check1.FixTangent(Standard_True,Standard_True);
                //
                //
                AdjustUPeriodic (aS2, BS1);  
                //
                slineS2.Append(BS1);
              }
              else {
                slineS2.Append(H1);
              }

              if(myApprox1) { 
                mbspc.Curve(1,tpoles2d);
                Handle(Geom2d_BSplineCurve) BS2=new Geom2d_BSplineCurve(tpoles2d,
                  mbspc.Knots(),
                  mbspc.Multiplicities(),
                  mbspc.Degree());
                GeomLib_Check2dBSplineCurve Check2(BS2,myTolCheck,myTolAngCheck);
                Check2.FixTangent(Standard_True,Standard_True);
                // 
                //
                AdjustUPeriodic (aS1, BS2);  
                //	
                slineS1.Append(BS2);
              }
              else { 
                slineS1.Append(H1);
              }
            } // else if(typs2 == GeomAbs_Plane)
            //
            else { // typs1!=GeomAbs_Plane && typs2!=GeomAbs_Plane
              const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
              nbpoles = mbspc.NbPoles();
              TColgp_Array1OfPnt tpoles(1,nbpoles);
              mbspc.Curve(1,tpoles);
              Handle(Geom_BSplineCurve) BS=new Geom_BSplineCurve(tpoles,
                mbspc.Knots(),
                mbspc.Multiplicities(),
                mbspc.Degree());
              GeomLib_CheckBSplineCurve Check(BS,myTolCheck,myTolAngCheck);
              Check.FixTangent(Standard_True,Standard_True);
              // 	
              //Check IsClosed()
              Standard_Real aDist = Max(BS->StartPoint().XYZ().SquareModulus(),
                BS->EndPoint().XYZ().SquareModulus());
              Standard_Real eps = Epsilon(aDist);
              if(BS->StartPoint().SquareDistance(BS->EndPoint()) < 2.*eps)
              {
                // Avoid creating B-splines containing two coincident poles only
                if (mbspc.Degree() == 1 && nbpoles == 2)
                  continue;

                if (!BS->IsClosed() && !BS->IsPeriodic())
                {
                  //force Closed()
                  gp_Pnt aPm((BS->Pole(1).XYZ() + BS->Pole(BS->NbPoles()).XYZ()) / 2.);
                  BS->SetPole(1, aPm);
                  BS->SetPole(BS->NbPoles(), aPm);
                }
              }
              sline.Append(BS);

              if(myApprox1) { 
                TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
                mbspc.Curve(2,tpoles2d);
                Handle(Geom2d_BSplineCurve) BS1=new Geom2d_BSplineCurve(tpoles2d,
                  mbspc.Knots(),
                  mbspc.Multiplicities(),
                  mbspc.Degree());
                GeomLib_Check2dBSplineCurve newCheck(BS1,myTolCheck,myTolAngCheck);
                newCheck.FixTangent(Standard_True,Standard_True);
                //
                AdjustUPeriodic (aS1, BS1);  
                //
                slineS1.Append(BS1);
              }
              else {
                slineS1.Append(H1);
              }
              if(myApprox2) { 
                TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
                mbspc.Curve((myApprox1==Standard_True)? 3 : 2,tpoles2d);
                Handle(Geom2d_BSplineCurve) BS2=new Geom2d_BSplineCurve(tpoles2d,
                  mbspc.Knots(),
                  mbspc.Multiplicities(),
                  mbspc.Degree());
                GeomLib_Check2dBSplineCurve newCheck(BS2,myTolCheck,myTolAngCheck);
                newCheck.FixTangent(Standard_True,Standard_True);
                //
                AdjustUPeriodic (aS2, BS2);  
                //
                slineS2.Append(BS2);
              }
              else { 
                slineS2.Append(H1);
              }
            }// else { // typs1!=GeomAbs_Plane && typs2!=GeomAbs_Plane
          }// for (j=1; j<=aNbMultiCurves; j++
        }
      }
    }
  }
  break;

  case IntPatch_Restriction: 
    {
      Handle(IntPatch_RLine) RL = 
        Handle(IntPatch_RLine)::DownCast(L);
      Handle(Geom_Curve) aC3d;
      Handle(Geom2d_Curve) aC2d1, aC2d2;
      Standard_Real aTolReached;
      TreatRLine(RL, myHS1, myHS2, aC3d,
                  aC2d1, aC2d2, aTolReached);

      if(aC3d.IsNull())
        break;

      Bnd_Box2d aBox1, aBox2;

      const Standard_Real aU1f = myHS1->FirstUParameter(),
                          aV1f = myHS1->FirstVParameter(),
                          aU1l = myHS1->LastUParameter(),
                          aV1l = myHS1->LastVParameter();
      const Standard_Real aU2f = myHS2->FirstUParameter(),
                          aV2f = myHS2->FirstVParameter(),
                          aU2l = myHS2->LastUParameter(),
                          aV2l = myHS2->LastVParameter();

      aBox1.Add(gp_Pnt2d(aU1f, aV1f));
      aBox1.Add(gp_Pnt2d(aU1l, aV1l));
      aBox2.Add(gp_Pnt2d(aU2f, aV2f));
      aBox2.Add(gp_Pnt2d(aU2l, aV2l));

      GeomInt_VectorOfReal anArrayOfParameters;

      //We consider here that the intersection line is same-parameter-line
      anArrayOfParameters.Append(aC3d->FirstParameter());
      anArrayOfParameters.Append(aC3d->LastParameter());

      TrimILineOnSurfBoundaries(aC2d1, aC2d2, aBox1, aBox2, anArrayOfParameters);

      const Standard_Integer aNbIntersSolutionsm1 = anArrayOfParameters.Length() - 1;

      //Trim RLine found.
      for(Standard_Integer anInd = 0; anInd < aNbIntersSolutionsm1; anInd++)
      {
        const Standard_Real aParF = anArrayOfParameters(anInd),
                            aParL = anArrayOfParameters(anInd+1);

        if((aParL - aParF) <= Precision::PConfusion())
          continue;

        const Standard_Real aPar = 0.5*(aParF + aParL);
        gp_Pnt2d aPt;

        Handle(Geom2d_Curve) aCurv2d1, aCurv2d2;
        if(!aC2d1.IsNull())
        {
          aC2d1->D0(aPar, aPt);

          if(aBox1.IsOut(aPt))
            continue;

          if(myApprox1)
            aCurv2d1 = new Geom2d_TrimmedCurve(aC2d1, aParF, aParL);
        }

        if(!aC2d2.IsNull())
        {
          aC2d2->D0(aPar, aPt);

          if(aBox2.IsOut(aPt))
            continue;

          if(myApprox2)
            aCurv2d2 = new Geom2d_TrimmedCurve(aC2d2, aParF, aParL);
        }

        Handle(Geom_Curve) aCurv3d = new Geom_TrimmedCurve(aC3d, aParF, aParL);

        sline.Append(aCurv3d);
        slineS1.Append(aCurv2d1);
        slineS2.Append(aCurv2d2);
      }
    }
    break;
  }
}

//=======================================================================
//function : TreatRLine
//purpose  : Approx of Restriction line
//=======================================================================
void GeomInt_IntSS::TreatRLine(const Handle(IntPatch_RLine)& theRL, 
                const Handle(GeomAdaptor_HSurface)& theHS1, 
                const Handle(GeomAdaptor_HSurface)& theHS2, 
                Handle(Geom_Curve)& theC3d,
                Handle(Geom2d_Curve)& theC2d1, 
                Handle(Geom2d_Curve)& theC2d2, 
                Standard_Real& theTolReached)
{
  Handle(GeomAdaptor_HSurface) aGAHS;
  Handle(Adaptor2d_HCurve2d) anAHC2d;
  Standard_Real tf, tl;
  gp_Lin2d aL;
  // It is assumed that 2d curve is 2d line (rectangular surface domain)
  if(theRL->IsArcOnS1())
  {
    aGAHS = theHS1;
    anAHC2d = theRL->ArcOnS1();
    theRL->ParamOnS1(tf, tl);
    theC2d1 = Geom2dAdaptor::MakeCurve(anAHC2d->Curve2d());
    tf = Max(tf, theC2d1->FirstParameter());
    tl = Min(tl, theC2d1->LastParameter());
    theC2d1 = new Geom2d_TrimmedCurve(theC2d1, tf, tl);
  }
  else if (theRL->IsArcOnS2())
  {
    aGAHS = theHS2;
    anAHC2d = theRL->ArcOnS2();
    theRL->ParamOnS2(tf, tl);
    theC2d2 = Geom2dAdaptor::MakeCurve(anAHC2d->Curve2d());
    tf = Max(tf, theC2d2->FirstParameter());
    tl = Min(tl, theC2d2->LastParameter());
    theC2d2 = new Geom2d_TrimmedCurve(theC2d2, tf, tl);
  }
  else
  {
    return;
  }

  //Restriction line can correspond to a degenerated edge.
  //In this case we return null-curve.
  if(isDegenerated(aGAHS, anAHC2d, tf, tl))
    return;

  //
  //To provide sameparameter it is necessary to get 3d curve as
  //approximation of curve on surface.
  Standard_Integer aMaxDeg = 8;
  Standard_Integer aMaxSeg = 1000;
  Approx_CurveOnSurface anApp(anAHC2d, aGAHS, tf, tl, Precision::Confusion(),
                              GeomAbs_C1, aMaxDeg, aMaxSeg, 
                              Standard_True, Standard_False);
  if(!anApp.HasResult())
    return;

  theC3d = anApp.Curve3d();
  theTolReached = anApp.MaxError3d();
  Standard_Real aTol = Precision::Confusion();
  if(theRL->IsArcOnS1())
  {
    Handle(Geom_Surface) aS = GeomAdaptor::MakeSurface(theHS2->Surface());
    BuildPCurves (tf, tl, aTol, 
                  aS, theC3d, theC2d2);
  }
  if(theRL->IsArcOnS2())
  {
    Handle(Geom_Surface) aS = GeomAdaptor::MakeSurface(theHS1->Surface());
    BuildPCurves (tf, tl, aTol, 
                  aS, theC3d, theC2d1);
  }
  theTolReached = Max(theTolReached, aTol);
}

//=======================================================================
//function : BuildPCurves
//purpose  : 
//=======================================================================
void GeomInt_IntSS::BuildPCurves (Standard_Real f,
                                  Standard_Real l,
                                  Standard_Real& Tol,
                                  const Handle (Geom_Surface)& S,
                                  const Handle (Geom_Curve)&   C,
                                  Handle (Geom2d_Curve)& C2d)
{
  if (!C2d.IsNull()) {
    return;
  }
  //
  Standard_Real umin,umax,vmin,vmax;
  // 
  S->Bounds(umin, umax, vmin, vmax);
  // in class ProjLib_Function the range of parameters is shrank by 1.e-09
  if((l - f) > 2.e-09) {
    C2d = GeomProjLib::Curve2d(C,f,l,S,umin,umax,vmin,vmax,Tol);
    if (C2d.IsNull()) {
      // proj. a circle that goes through the pole on a sphere to the sphere     
      Tol += Precision::Confusion();
      C2d = GeomProjLib::Curve2d(C,f,l,S,Tol);
    }
    const Handle(Standard_Type)& aType = C2d->DynamicType();
    if ( aType == STANDARD_TYPE(Geom2d_BSplineCurve)) 
    { 
      //Check first, last knots to avoid problems with trimming
      //First, last knots can differ from f, l because of numerical error 
      //of projection and approximation
      //The same checking as in Geom2d_TrimmedCurve
      if((C2d->FirstParameter() - f > Precision::PConfusion()) ||
        (l - C2d->LastParameter() > Precision::PConfusion()))   
      {
        Handle(Geom2d_BSplineCurve) aBspl = Handle(Geom2d_BSplineCurve)::DownCast(C2d);
        TColStd_Array1OfReal aKnots(1, aBspl->NbKnots());
        aBspl->Knots(aKnots);
        BSplCLib::Reparametrize(f, l, aKnots);
        aBspl->SetKnots(aKnots);
      }
    }
  }
  else {
    if((l - f) > Epsilon(Abs(f)))
    {
      //The domain of C2d is [Epsilon(Abs(f)), 2.e-09]
      //On this small range C2d can be considered as segment 
      //of line.

      Standard_Real aU=0., aV=0.;
      GeomAdaptor_Surface anAS;
      anAS.Load(S);
      Extrema_ExtPS anExtr;
      const gp_Pnt aP3d1 = C->Value(f);
      const gp_Pnt aP3d2 = C->Value(l);

      anExtr.SetAlgo(Extrema_ExtAlgo_Grad);
      anExtr.Initialize(anAS, umin, umax, vmin, vmax,
                                Precision::Confusion(), Precision::Confusion());
      anExtr.Perform(aP3d1);

      if(ParametersOfNearestPointOnSurface(anExtr, aU, aV))
      {
        const gp_Pnt2d aP2d1(aU, aV);

        anExtr.Perform(aP3d2);

        if(ParametersOfNearestPointOnSurface(anExtr, aU, aV))
        {
          const gp_Pnt2d aP2d2(aU, aV);

          if(aP2d1.Distance(aP2d2) > gp::Resolution())
          {
            TColgp_Array1OfPnt2d poles(1,2);
            TColStd_Array1OfReal knots(1,2);
            TColStd_Array1OfInteger mults(1,2);
            poles(1) = aP2d1;
            poles(2) = aP2d2;
            knots(1) = f;
            knots(2) = l;
            mults(1) = mults(2) = 2;

            C2d = new Geom2d_BSplineCurve(poles,knots,mults,1);

            //Check same parameter in middle point .begin
            const gp_Pnt PMid(C->Value(0.5*(f+l)));
            const gp_Pnt2d pmidcurve2d(0.5*(aP2d1.XY() + aP2d2.XY()));
            const gp_Pnt aPC(anAS.Value(pmidcurve2d.X(), pmidcurve2d.Y()));
            const Standard_Real aDist = PMid.Distance(aPC);
            Tol = Max(aDist, Tol);
            //Check same parameter in middle point .end
          }
        }
      }
    }
  }
  //
  if (S->IsUPeriodic() && !C2d.IsNull()) {
    // Recadre dans le domaine UV de la face
    Standard_Real aTm, U0, aEps, period, du, U0x;
    Standard_Boolean bAdjust;
    //
    aEps = Precision::PConfusion();
    period = S->UPeriod();
    //
    aTm = .5*(f + l);
    gp_Pnt2d pm = C2d->Value(aTm);
    U0 = pm.X();
    //
    bAdjust = 
      GeomInt::AdjustPeriodic(U0, umin, umax, period, U0x, du, aEps);
    if (bAdjust) {
      gp_Vec2d T1(du, 0.);
      C2d->Translate(T1);
    }
  }
}

//=======================================================================
//function : TrimILineOnSurfBoundaries
//purpose  : This function finds intersection points of given curves with
//            surface boundaries and fills theArrayOfParameters by parameters
//            along the given curves corresponding of these points.
//=======================================================================
void GeomInt_IntSS::TrimILineOnSurfBoundaries(const Handle(Geom2d_Curve)& theC2d1,
                                              const Handle(Geom2d_Curve)& theC2d2,
                                              const Bnd_Box2d& theBound1,
                                              const Bnd_Box2d& theBound2,
                                              GeomInt_VectorOfReal& theArrayOfParameters)
{
  //Rectangular boundaries of two surfaces: [0]:U=Ufirst, [1]:U=Ulast,
  //                                        [2]:V=Vfirst, [3]:V=Vlast 
  const Standard_Integer aNumberOfCurves = 4;
  Handle(Geom2d_Curve) aCurS1Bounds[aNumberOfCurves];
  Handle(Geom2d_Curve) aCurS2Bounds[aNumberOfCurves];

  Standard_Real aU1f=0.0, aV1f=0.0, aU1l=0.0, aV1l=0.0;
  Standard_Real aU2f=0.0, aV2f=0.0, aU2l=0.0, aV2l=0.0;

  theBound1.Get(aU1f, aV1f, aU1l, aV1l);
  theBound2.Get(aU2f, aV2f, aU2l, aV2l);

  Standard_Real aDelta = aV1l-aV1f;
  if(Abs(aDelta) > RealSmall())
  {
    if(!Precision::IsInfinite(aU1f))
    {
      aCurS1Bounds[0] = new Geom2d_Line(gp_Pnt2d(aU1f, aV1f), gp_Dir2d(0.0, 1.0));

      if(!Precision::IsInfinite(aDelta))
        aCurS1Bounds[0] = new Geom2d_TrimmedCurve(aCurS1Bounds[0], 0, aDelta);
    }

    if(!Precision::IsInfinite(aU1l))
    {
      aCurS1Bounds[1] = new Geom2d_Line(gp_Pnt2d(aU1l, aV1f), gp_Dir2d(0.0, 1.0));
      if(!Precision::IsInfinite(aDelta))
        aCurS1Bounds[1] = new Geom2d_TrimmedCurve(aCurS1Bounds[1], 0, aDelta);
    }
  }

  aDelta = aU1l-aU1f;
  if(Abs(aDelta) > RealSmall())
  {
    if(!Precision::IsInfinite(aV1f))
    {
      aCurS1Bounds[2] = new Geom2d_Line(gp_Pnt2d(aU1f, aV1f), gp_Dir2d(1.0, 0.0));
      if(!Precision::IsInfinite(aDelta))
        aCurS1Bounds[2] = new Geom2d_TrimmedCurve(aCurS1Bounds[2], 0, aDelta);
    }

    if(!Precision::IsInfinite(aV1l))
    {
      aCurS1Bounds[3] = new Geom2d_Line(gp_Pnt2d(aU1l, aV1l), gp_Dir2d(1.0, 0.0));
      if(!Precision::IsInfinite(aDelta))
        aCurS1Bounds[3] = new Geom2d_TrimmedCurve(aCurS1Bounds[3], 0, aDelta);
    }
  }

  aDelta = aV2l-aV2f;
  if(Abs(aDelta) > RealSmall())
  {
    if(!Precision::IsInfinite(aU2f))
    {
      aCurS2Bounds[0] = new Geom2d_Line(gp_Pnt2d(aU2f, aV2f), gp_Dir2d(0.0, 1.0));
      if(!Precision::IsInfinite(aDelta))
        aCurS2Bounds[0] = new Geom2d_TrimmedCurve(aCurS2Bounds[0], 0, aDelta);
    }

    if(!Precision::IsInfinite(aU2l))
    {
      aCurS2Bounds[1] = new Geom2d_Line(gp_Pnt2d(aU2l, aV2f), gp_Dir2d(0.0, 1.0));
      if(!Precision::IsInfinite(aDelta))
        aCurS2Bounds[1] = new Geom2d_TrimmedCurve(aCurS2Bounds[1], 0, aDelta);
    }
  }

  aDelta = aU2l-aU2f;
  if(Abs(aDelta) > RealSmall())
  {
    if(!Precision::IsInfinite(aV2f))
    {
      aCurS2Bounds[2] = new Geom2d_Line(gp_Pnt2d(aU2f, aV2f), gp_Dir2d(1.0, 0.0));
      if(!Precision::IsInfinite(aDelta))
        aCurS2Bounds[2] = new Geom2d_TrimmedCurve(aCurS2Bounds[2], 0, aDelta);
    }

    if(!Precision::IsInfinite(aV2l))
    {
      aCurS2Bounds[3] = new Geom2d_Line(gp_Pnt2d(aU2l, aV2l), gp_Dir2d(1.0, 0.0));
      if(!Precision::IsInfinite(aDelta))
        aCurS2Bounds[3] = new Geom2d_TrimmedCurve(aCurS2Bounds[3], 0, aDelta);
    }
  }

  const Standard_Real anIntTol = 10.0*Precision::Confusion();

  IntersectCurveAndBoundary(theC2d1, aCurS1Bounds,
                        aNumberOfCurves, anIntTol, theArrayOfParameters);

  IntersectCurveAndBoundary(theC2d2, aCurS2Bounds,
                        aNumberOfCurves, anIntTol, theArrayOfParameters);

  std::sort(theArrayOfParameters.begin(), theArrayOfParameters.end());
}

//=======================================================================
//function : MakeBSpline
//purpose  : 
//=======================================================================
Handle(Geom_Curve) GeomInt_IntSS::MakeBSpline  (const Handle(IntPatch_WLine)& WL,
                                                const Standard_Integer ideb,
                                                const Standard_Integer ifin)
{
  const Standard_Integer nbpnt = ifin-ideb+1;
  TColgp_Array1OfPnt poles(1,nbpnt);
  TColStd_Array1OfReal knots(1,nbpnt);
  TColStd_Array1OfInteger mults(1,nbpnt);
  Standard_Integer i = 1, ipidebm1 = ideb;
  for(; i<=nbpnt; ipidebm1++, i++)
  {
    poles(i) = WL->Point(ipidebm1).Value();
    mults(i) = 1;
    knots(i) = i-1;
  }
  mults(1) = mults(nbpnt) = 2;
  return new Geom_BSplineCurve(poles,knots,mults,1);
}

//=======================================================================
//function : MakeBSpline2d
//purpose  : 
//=======================================================================
Handle(Geom2d_BSplineCurve) GeomInt_IntSS::
      MakeBSpline2d(const Handle(IntPatch_WLine)& theWLine,
                    const Standard_Integer ideb,
                    const Standard_Integer ifin,
                    const Standard_Boolean onFirst)
{
  const Standard_Integer nbpnt = ifin-ideb+1;
  TColgp_Array1OfPnt2d poles(1,nbpnt);
  TColStd_Array1OfReal knots(1,nbpnt);
  TColStd_Array1OfInteger mults(1,nbpnt);
  Standard_Integer i = 1, ipidebm1 = ideb;
  for(; i <= nbpnt; ipidebm1++, i++)
  {
    Standard_Real U, V;
    if(onFirst)
	  theWLine->Point(ipidebm1).ParametersOnS1(U, V);
    else
	  theWLine->Point(ipidebm1).ParametersOnS2(U, V);
    poles(i).SetCoord(U, V);
    mults(i) = 1;
    knots(i) = i-1;
  }

  mults(1) = mults(nbpnt) = 2;
  return new Geom2d_BSplineCurve(poles,knots,mults,1);
}
