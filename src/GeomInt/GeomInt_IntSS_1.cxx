// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <GeomInt_IntSS.ixx>

#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pln.hxx>

#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfListOfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <Adaptor3d_TopolTool.hxx>
#include <IntPatch_Line.hxx>
#include <IntPatch_WLine.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_ALineToWLine.hxx>
#include <IntPatch_IType.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_List.hxx>
#include <NCollection_LocalArray.hxx>
#include <NCollection_StdAllocator.hxx>
#include <vector>

#include <AppParCurves_MultiBSpCurve.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_BSplineCurve.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>

#include <GeomLib_CheckBSplineCurve.hxx>
#include <GeomLib_Check2dBSplineCurve.hxx>
#include <GeomProjLib.hxx>


#include <ElSLib.hxx>

#include <GeomInt_WLApprox.hxx>
#include <Extrema_ExtPS.hxx>

static
  void BuildPCurves (Standard_Real f,
		     Standard_Real l,
		     Standard_Real& Tol,
		     const Handle (Geom_Surface)& S,
		     const Handle (Geom_Curve)&   C,
		     Handle (Geom2d_Curve)& C2d);

static
  void Parameters(const Handle(GeomAdaptor_HSurface)& HS1,
		  const Handle(GeomAdaptor_HSurface)& HS2,
		  const gp_Pnt& Ptref,
		  Standard_Real& U1,
		  Standard_Real& V1,
		  Standard_Real& U2,
		  Standard_Real& V2);

static 
  Handle(Geom_Curve) MakeBSpline  (const Handle(IntPatch_WLine)& WL,
				   const Standard_Integer ideb,
				   const Standard_Integer ifin);

static
  Handle(Geom2d_BSplineCurve) MakeBSpline2d(const Handle(IntPatch_WLine)& theWLine,
					    const Standard_Integer                         ideb,
					    const Standard_Integer                         ifin,
					    const Standard_Boolean                         onFirst);

static 
  Handle(IntPatch_WLine) ComputePurgedWLine(const Handle(IntPatch_WLine)& theWLine);


static
  Standard_Boolean DecompositionOfWLine  (const Handle(IntPatch_WLine)& theWLine,
					  const Handle(GeomAdaptor_HSurface)&            theSurface1, 
					  const Handle(GeomAdaptor_HSurface)&            theSurface2,
					  const Standard_Real aTolSum, 
					  const GeomInt_LineConstructor&                 theLConstructor,
					  IntPatch_SequenceOfLine&                       theNewLines);

static
  Standard_Real AdjustPeriodic(const Standard_Real theParameter,
			       const Standard_Real parmin,
			       const Standard_Real parmax,
			       const Standard_Real thePeriod,
			       Standard_Real&      theOffset) ;
static 
  Standard_Boolean IsPointOnBoundary(const Standard_Real theParameter,
				     const Standard_Real theFirstBoundary,
				     const Standard_Real theSecondBoundary,
				     const Standard_Real theResolution,
				     Standard_Boolean&   IsOnFirstBoundary);

static 
  Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
			     const gp_Pnt2d&     theLastPoint,
			     const Standard_Real theUmin, 
			     const Standard_Real theUmax,
			     const Standard_Real theVmin,
			     const Standard_Real theVmax,
			     gp_Pnt2d&           theNewPoint);


static
  void AdjustUPeriodic (const Handle(Geom_Surface)& aS,
			Handle(Geom2d_Curve)& aC2D);
static
  void GetQuadric(const Handle(GeomAdaptor_HSurface)& HS1,
		  IntSurf_Quadric& quad1);


//


class ProjectPointOnSurf
{
 public:
  ProjectPointOnSurf() : myIsDone (Standard_False),myIndex(0) {}
  void Init(const Handle(Geom_Surface)& Surface,
	    const Standard_Real Umin,
	    const Standard_Real Usup,
	    const Standard_Real Vmin,
	    const Standard_Real Vsup);
  void Init ();
  void Perform(const gp_Pnt& P);
  Standard_Boolean IsDone () const { return myIsDone; }
  void LowerDistanceParameters(Standard_Real& U, Standard_Real& V ) const;
  Standard_Real LowerDistance() const;
 protected:
  Standard_Boolean myIsDone;
  Standard_Integer myIndex;
  Extrema_ExtPS myExtPS;
  GeomAdaptor_Surface myGeomAdaptor;
};

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void ProjectPointOnSurf::Init (const Handle(Geom_Surface)& Surface,
				 const Standard_Real       Umin,
				 const Standard_Real       Usup,
				 const Standard_Real       Vmin,
				 const Standard_Real       Vsup )
{
  const Standard_Real Tolerance = Precision::PConfusion();
  //
  myGeomAdaptor.Load(Surface, Umin,Usup,Vmin,Vsup);
  myExtPS.Initialize(myGeomAdaptor, Umin, Usup, Vmin, Vsup, Tolerance, Tolerance);
  myIsDone = Standard_False;
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void ProjectPointOnSurf::Init ()
{
  myIsDone = myExtPS.IsDone() && (myExtPS.NbExt() > 0);
  if (myIsDone) {
    // evaluate the lower distance and its index;
    Standard_Real Dist2Min = myExtPS.SquareDistance(1);
    myIndex = 1;
    for (Standard_Integer i = 2; i <= myExtPS.NbExt(); i++)
    {
      const Standard_Real Dist2 = myExtPS.SquareDistance(i);
      if (Dist2 < Dist2Min) {
        Dist2Min = Dist2;
        myIndex = i;
      }
    }
  }
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void ProjectPointOnSurf::Perform(const gp_Pnt& P)
{
  myExtPS.Perform(P);
  Init ();
}
//=======================================================================
//function : LowerDistanceParameters
//purpose  : 
//=======================================================================
void ProjectPointOnSurf::LowerDistanceParameters (Standard_Real& U,
                                                  Standard_Real& V ) const
{
  StdFail_NotDone_Raise_if(!myIsDone, "GeomInt_IntSS::ProjectPointOnSurf::LowerDistanceParameters");
  (myExtPS.Point(myIndex)).Parameter(U,V);
}
//=======================================================================
//function : LowerDistance
//purpose  : 
//=======================================================================
Standard_Real ProjectPointOnSurf::LowerDistance() const
{
  StdFail_NotDone_Raise_if(!myIsDone, "GeomInt_IntSS::ProjectPointOnSurf::LowerDistance");
  return sqrt(myExtPS.SquareDistance(myIndex));
}
//

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
    Handle(IntPatch_Line) anewL = 
      ComputePurgedWLine(Handle(IntPatch_WLine)::DownCast(L));
    //
    if(anewL.IsNull()) {
      return;
    }
    L = anewL;
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
	// /cto/900/F1
	//if (typl == IntPatch_Lin) {
	  //Standard_Real dPrm=1.3e-6;
	  //fprm=fprm-dPrm;
	  //lprm=lprm+dPrm;
	//}
	//
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
    Standard_Real aPeriod, aNul, aRealEpsilon;
    //
    aRealEpsilon=RealEpsilon();
    aNul=0.;
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
  }// IntPatch_Circle: IntPatch_Ellipse:
    break;
    
    //########################################  
    // Analytic
    //######################################## 
  case IntPatch_Analytic: {
    IntSurf_Quadric quad1,quad2;
    //
    GetQuadric(myHS1, quad1);
    GetQuadric(myHS2, quad2);
    //=========
    IntPatch_ALineToWLine convert (quad1, quad2);
      
    if (!myApprox) {
      Handle(Geom2d_BSplineCurve) aH1, aH2;
      //
      aNbParts=myLConstruct.NbParts();
      for (i=1; i<=aNbParts; i++) {
	myLConstruct.Part(i, fprm, lprm);
	Handle(IntPatch_WLine) WL = 
	  convert.MakeWLine(Handle(IntPatch_ALine)::DownCast(L), fprm, lprm);
	//
	if(myApprox1) {
	  aH1 = MakeBSpline2d(WL, 1, WL->NbPnts(), Standard_True);
	}
	
	if(myApprox2) {
	  aH2 = MakeBSpline2d(WL, 1, WL->NbPnts(), Standard_False);
	}
	sline.Append(MakeBSpline(WL,1,WL->NbPnts()));
	slineS1.Append(aH1);
	slineS2.Append(aH2);
      }
    } // if (!myApprox)

    else { // myApprox=TRUE
      GeomInt_WLApprox theapp3d;
      Standard_Real tol2d = myTolApprox;
      // 	
      theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_True);
      
      aNbParts=myLConstruct.NbParts();
      for (i=1; i<=aNbParts; i++) {
	myLConstruct.Part(i, fprm, lprm);
	Handle(IntPatch_WLine) WL = 
	  convert.MakeWLine(Handle(IntPatch_ALine):: DownCast(L),fprm,lprm);

	theapp3d.Perform(myHS1,myHS2,WL,Standard_True,myApprox1,myApprox2, 1, WL->NbPnts());
	if (!theapp3d.IsDone()) {
	  //
	  Handle(Geom2d_BSplineCurve) aH1, aH2;

	  if(myApprox1) {
	    aH1 = MakeBSpline2d(WL, 1, WL->NbPnts(), Standard_True);
	  }
	  
	  if(myApprox2) {
	    aH2 = MakeBSpline2d(WL, 1, WL->NbPnts(), Standard_False);
	  }
	  sline.Append(MakeBSpline(WL,1,WL->NbPnts()));
	  slineS1.Append(aH1);
	  slineS2.Append(aH1);
	}
	//
	else {
	  if(myApprox1 || myApprox2) { 
	    if( theapp3d.TolReached2d()>myTolReached2d || myTolReached2d==0) { 
	      myTolReached2d = theapp3d.TolReached2d();
	    }
	  }
	  
	  if( theapp3d.TolReached3d()>myTolReached3d || myTolReached3d==0) { 
	    myTolReached3d = theapp3d.TolReached3d();
	  }
	  
	  Standard_Integer aNbMultiCurves, nbpoles;
	  aNbMultiCurves=theapp3d.NbMultiCurves();
	  for (j=1; j<=aNbMultiCurves; j++) {
	    const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
	    
	    nbpoles = mbspc.NbPoles();
	    TColgp_Array1OfPnt tpoles(1, nbpoles);
	    mbspc.Curve(1, tpoles);
	    Handle(Geom_BSplineCurve) BS=new Geom_BSplineCurve(tpoles,
							       mbspc.Knots(),
							       mbspc.Multiplicities(),
							       mbspc.Degree());
	    
	    GeomLib_CheckBSplineCurve Check(BS, myTolCheck, myTolAngCheck);
	    Check.FixTangent(Standard_True,Standard_True);
	    // 
	    sline.Append(BS);
	    //
	    if(myApprox1) { 
	      TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
	      mbspc.Curve(2,tpoles2d);
	      Handle(Geom2d_BSplineCurve) BS2=new Geom2d_BSplineCurve(tpoles2d,
								      mbspc.Knots(),
								      mbspc.Multiplicities(),
								      mbspc.Degree());

	      GeomLib_Check2dBSplineCurve newCheck(BS2,myTolCheck,myTolAngCheck);
	      newCheck.FixTangent(Standard_True,Standard_True);
	      slineS1.Append(BS2); 		
	    }
	    else {
	      slineS1.Append(H1);
	    }
	    
	    if(myApprox2) { 
	      TColgp_Array1OfPnt2d tpoles2d(1, nbpoles);
	      Standard_Integer TwoOrThree;
	      TwoOrThree=myApprox1 ? 3 : 2;
	      mbspc.Curve(TwoOrThree, tpoles2d);
	      Handle(Geom2d_BSplineCurve) BS2 =new Geom2d_BSplineCurve(tpoles2d,
								       mbspc.Knots(),
								       mbspc.Multiplicities(),
								       mbspc.Degree());
		
	      GeomLib_Check2dBSplineCurve newCheck(BS2,myTolCheck,myTolAngCheck);
	      newCheck.FixTangent(Standard_True,Standard_True);
	      // 	
	      slineS2.Append(BS2);
	    }
	    else { 
	      slineS2.Append(H1);
	    }
	    // 
	  }// for (j=1; j<=aNbMultiCurves; j++) {
	}// else from if (!theapp3d.IsDone())
      }// for (i=1; i<=aNbParts; i++) {
    }// else { // myApprox=TRUE
  }// case IntPatch_Analytic:
    break;
    
    //########################################  
    // Walking
    //######################################## 
  case IntPatch_Walking:{
    Handle(IntPatch_WLine) WL = 
      Handle(IntPatch_WLine)::DownCast(L);
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
      if(myHS1 == myHS2) { 
	theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_False);	  
      }
      else { 
	theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_True);
      }
      //
      bIsDecomposited = 
	DecompositionOfWLine(WL, myHS1, myHS2, aTolSS, myLConstruct, aSeqOfL);
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
	     
	      theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_True);
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
        if(BS->StartPoint().SquareDistance(BS->EndPoint()) < 2.*eps &&
          !BS->IsClosed() && !BS->IsPeriodic())
        {
          //force Closed()
          gp_Pnt aPm((BS->Pole(1).XYZ() + BS->Pole(BS->NbPoles()).XYZ()) / 2.);
          BS->SetPole(1, aPm);
          BS->SetPole(BS->NbPoles(), aPm);
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
    break;

  }
}
//
//=======================================================================
//function : AdjustUPeriodic
//purpose  : 
//=======================================================================
 void AdjustUPeriodic (const Handle(Geom_Surface)& aS, Handle(Geom2d_Curve)& aC2D)
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
//
//
//=======================================================================
//function : BuildPCurves
//purpose  : 
//=======================================================================
 void BuildPCurves (Standard_Real f, Standard_Real l,
                    Standard_Real& Tol,
                    const Handle(Geom_Surface)& S,
                    const Handle(Geom_Curve)& C,
                    Handle(Geom2d_Curve)& C2d)
{
  if (C2d.IsNull())
  {
    C2d = GeomProjLib::Curve2d(C,f,l,S,Tol);
    if (S->IsUPeriodic())
    {
      Standard_Real umin,umax,vmin,vmax;
      S->Bounds(umin,umax,vmin,vmax);
      // Recadre dans le domaine UV de la face
      const Standard_Real period = S->UPeriod();
      gp_Pnt2d Pf=C2d->Value(f);
      //
      Standard_Real U0=Pf.X();
      //
      const Standard_Real aEpsilon=Epsilon(10.);//1.77e-15 
      if (fabs(U0)<aEpsilon)
        U0=0.;
      if (fabs(U0-period)<aEpsilon)
        U0=period;
      //
      const Standard_Real aEps=Precision::PConfusion();//1.e-9
      Standard_Real du=0.;
      while(U0 <(umin-aEps)) { 
        U0+=period;
        du+=period;
      }
      while(U0>(umax+aEps)) { 
        U0-=period;
        du-=period;
      }
      if (du!=0.) {
        gp_Vec2d T1(du, 0.);
        C2d->Translate(T1);
      }
    }
  }
}
//
//=======================================================================
//function : GetQuadric
//purpose  : 
//=======================================================================
 void GetQuadric(const Handle(GeomAdaptor_HSurface)& HS1, IntSurf_Quadric& quad1)
{
  switch (HS1->Surface().GetType())
  {
    case GeomAbs_Plane:    quad1.SetValue(HS1->Surface().Plane()); break;
    case GeomAbs_Cylinder: quad1.SetValue(HS1->Surface().Cylinder()); break;
    case GeomAbs_Cone:     quad1.SetValue(HS1->Surface().Cone()); break;
    case GeomAbs_Sphere:   quad1.SetValue(HS1->Surface().Sphere()); break;
    default: Standard_ConstructionError::Raise("GeomInt_IntSS::MakeCurve");
  }
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================
 void Parameters(const Handle(GeomAdaptor_HSurface)& HS1,
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
//function : MakeBSpline
//purpose  : 
//=======================================================================
Handle(Geom_Curve) MakeBSpline  (const Handle(IntPatch_WLine)& WL,
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
Handle(Geom2d_BSplineCurve) MakeBSpline2d(const Handle(IntPatch_WLine)& theWLine,
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

//=========================================================================
// static function : ComputePurgedWLine
// purpose : Removes equal points (leave one of equal points) from theWLine
//           and recompute vertex parameters.
//           Returns new WLine or null WLine if the number
//           of the points is less than 2.
//=========================================================================
Handle(IntPatch_WLine) ComputePurgedWLine(const Handle(IntPatch_WLine)& theWLine) 
{
  Handle(IntPatch_WLine) aResult;
  Handle(IntPatch_WLine) aLocalWLine;
  Handle(IntPatch_WLine) aTmpWLine = theWLine;

  Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
  aLocalWLine = new IntPatch_WLine(aLineOn2S, Standard_False);
  Standard_Integer i, k, v, nb, nbvtx;
  nbvtx = theWLine->NbVertex();
  nb = theWLine->NbPnts();

  for(i = 1; i <= nb; i++) {
    aLineOn2S->Add(theWLine->Point(i));
  }

  for(v = 1; v <= nbvtx; v++) {
    aLocalWLine->AddVertex(theWLine->Vertex(v));
  }
  
  for(i = 1; i <= aLineOn2S->NbPoints(); i++) {
    Standard_Integer aStartIndex = i + 1;
    Standard_Integer anEndIndex = i + 5;
    nb = aLineOn2S->NbPoints();
    anEndIndex = (anEndIndex > nb) ? nb : anEndIndex;

    if((aStartIndex >= nb) || (anEndIndex <= 1)) {
      continue;
    }
    k = aStartIndex;

    while(k <= anEndIndex) {
      
      if(i != k) {
	IntSurf_PntOn2S p1 = aLineOn2S->Value(i);
	IntSurf_PntOn2S p2 = aLineOn2S->Value(k);
	
	if(p1.Value().IsEqual(p2.Value(), gp::Resolution())) {
	  aTmpWLine = aLocalWLine;
	  aLocalWLine = new IntPatch_WLine(aLineOn2S, Standard_False);

	  for(v = 1; v <= aTmpWLine->NbVertex(); v++) {
	    IntPatch_Point aVertex = aTmpWLine->Vertex(v);
	    Standard_Integer avertexindex = (Standard_Integer)aVertex.ParameterOnLine();

	    if(avertexindex >= k) {
	      aVertex.SetParameter(aVertex.ParameterOnLine() - 1.);
	    }
	    aLocalWLine->AddVertex(aVertex);
	  }
	  aLineOn2S->RemovePoint(k);
	  anEndIndex--;
	  continue;
	}
      }
      k++;
    }
  }

  if(aLineOn2S->NbPoints() > 1) {
    aResult = aLocalWLine;
  }
  return aResult;
}
//=======================================================================
//function : DecompositionOfWLine
//purpose  : 
//=======================================================================
Standard_Boolean DecompositionOfWLine(const Handle(IntPatch_WLine)& theWLine,
                                      const Handle(GeomAdaptor_HSurface)&            theSurface1, 
                                      const Handle(GeomAdaptor_HSurface)&            theSurface2,
                                      const Standard_Real aTolSum, 
                                      const GeomInt_LineConstructor&                 theLConstructor,
                                      IntPatch_SequenceOfLine&                       theNewLines)
{
  typedef NCollection_List<Standard_Integer> ListOfInteger;
  //have to use std::vector, not NCollection_Vector in order to use copy constructor of
  //ListOfInteger which will be created with specific allocator instance
  typedef std::vector<ListOfInteger, NCollection_StdAllocator<
      ListOfInteger> > ArrayOfListOfInteger;

  Standard_Boolean bIsPrevPointOnBoundary, bIsCurrentPointOnBoundary;
  Standard_Integer nblines, aNbPnts, aNbParts, pit, i, j, aNbListOfPointIndex;
  Standard_Real aTol, umin, umax, vmin, vmax;

  //an inc allocator, it will contain wasted space (upon list's Clear()) but it should
  //still be faster than the standard allocator, and wasted memory should not be
  //significant and will be limited by time span of this function;
  //this is a separate allocator from the anIncAlloc below what provides better data
  //locality in the latter (by avoiding wastes which will only be in anIdxAlloc)
  Handle(NCollection_IncAllocator) anIdxAlloc = new NCollection_IncAllocator();
  ListOfInteger aListOfPointIndex (anIdxAlloc);

  //GeomAPI_ProjectPointOnSurf aPrj1, aPrj2;
  ProjectPointOnSurf aPrj1, aPrj2;
  Handle(Geom_Surface) aSurf1,  aSurf2;
  //
  aNbParts=theLConstructor.NbParts();
  aNbPnts=theWLine->NbPnts();
  //
  if((!aNbPnts) || (!aNbParts)){
    return Standard_False;
  }
  //
  Handle(NCollection_IncAllocator) anIncAlloc = new NCollection_IncAllocator();
  NCollection_StdAllocator<ListOfInteger> anAlloc (anIncAlloc);
  const ListOfInteger aDummy (anIncAlloc); //empty list to be copy constructed from
  ArrayOfListOfInteger anArrayOfLines (aNbPnts + 1, aDummy, anAlloc);

  NCollection_LocalArray<Standard_Integer> anArrayOfLineTypeArr (aNbPnts + 1);
  Standard_Integer* anArrayOfLineType = anArrayOfLineTypeArr;
  //
  nblines = 0;
  aTol = Precision::Confusion();
  //
  aSurf1 = theSurface1->ChangeSurface().Surface();
  aSurf1->Bounds(umin, umax, vmin, vmax);
  aPrj1.Init(aSurf1, umin, umax, vmin, vmax);
  //
  aSurf2 = theSurface2->ChangeSurface().Surface();
  aSurf2->Bounds(umin, umax, vmin, vmax);
  aPrj2.Init(aSurf2, umin, umax, vmin, vmax);
  //
  //
  bIsPrevPointOnBoundary=Standard_False;
  for(pit=1; pit<=aNbPnts; pit++) {
    const IntSurf_PntOn2S& aPoint = theWLine->Point(pit);
    bIsCurrentPointOnBoundary=Standard_False;
    //
    // whether aPoint is on boundary or not
    //
    for(i=0; i<2; i++) {// exploration Surface 1,2 
      Handle(GeomAdaptor_HSurface) aGASurface = (!i) ? theSurface1 : theSurface2;
      aGASurface->ChangeSurface().Surface()->Bounds(umin, umax, vmin, vmax);
      //
      for(j=0; j<2; j++) {// exploration of coordinate U,V
	Standard_Boolean isperiodic;
	//
	isperiodic = (!j) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();
	if(!isperiodic) {
	  continue;
	}
	//
	Standard_Real aResolution, aPeriod, alowerboundary, aupperboundary, U, V;
	Standard_Real aParameter, anoffset, anAdjustPar;
	Standard_Boolean bIsOnFirstBoundary, bIsPointOnBoundary;
	//
	aResolution = (!j) ? aGASurface->UResolution(aTol) : aGASurface->VResolution(aTol);
	aPeriod     = (!j) ? aGASurface->UPeriod() : aGASurface->VPeriod();
	alowerboundary = (!j) ? umin : vmin;
	aupperboundary = (!j) ? umax : vmax;
	U=0.;V=0.;//?
	//
	if(!i){
	  aPoint.ParametersOnS1(U, V);
	}
	else{
	  aPoint.ParametersOnS2(U, V);
	}
	//
	aParameter = (!j) ? U : V;
	anoffset=0.;
	anAdjustPar=AdjustPeriodic(aParameter, alowerboundary, aupperboundary, aPeriod, anoffset);
	//
	bIsOnFirstBoundary=Standard_True;
	//
	bIsPointOnBoundary=
	  IsPointOnBoundary(anAdjustPar, alowerboundary, aupperboundary, aResolution, bIsOnFirstBoundary);
	
	if(bIsPointOnBoundary) {
	  bIsCurrentPointOnBoundary = Standard_True;
	  break;
	}
      }// for(j=0; j<2; j++)
      
      if(bIsCurrentPointOnBoundary){
	break;
      }
    }// for(i=0; i<2; i++) 
    //
    if((bIsCurrentPointOnBoundary != bIsPrevPointOnBoundary)) {

      if(!aListOfPointIndex.IsEmpty()) {
	nblines++;
	anArrayOfLines[nblines] = aListOfPointIndex;
	anArrayOfLineType[nblines] = bIsPrevPointOnBoundary;
	aListOfPointIndex.Clear();
      }
      bIsPrevPointOnBoundary = bIsCurrentPointOnBoundary;
    }
    aListOfPointIndex.Append(pit);
  } // for(pit=1; pit<=aNbPnts; pit++)
  //
  aNbListOfPointIndex=aListOfPointIndex.Extent();
  if(aNbListOfPointIndex) {
    nblines++;
    anArrayOfLines[nblines] = aListOfPointIndex;
    anArrayOfLineType[nblines] = bIsPrevPointOnBoundary;
    aListOfPointIndex.Clear();
  }
  //
  if(nblines <= 1){
    return Standard_False;
  }
  //
  // Correct wlines.begin
  Standard_Integer aLineType;
  TColStd_Array1OfListOfInteger anArrayOfLineEnds(1, nblines);
  Handle(IntSurf_LineOn2S) aSeqOfPntOn2S = new IntSurf_LineOn2S (new NCollection_IncAllocator());
  //
  for(i = 1; i <= nblines; i++) {
    aLineType=anArrayOfLineType[i];
    if(aLineType) {
      continue;
    }
    //
    const ListOfInteger& aListOfIndex = anArrayOfLines[i];
    if(aListOfIndex.Extent() < 2) {
      continue;
    }
    //
    TColStd_ListOfInteger aListOfFLIndex;
    Standard_Integer aneighbourindex, aLineTypeNeib;
    //
    for(j = 0; j < 2; j++) {// neighbour line choice 
      aneighbourindex = (!j) ? (i-1) : (i+1);
      if((aneighbourindex < 1) || (aneighbourindex > nblines)){
	continue;
      }
      //
      aLineTypeNeib=anArrayOfLineType[aneighbourindex];
      if(!aLineTypeNeib){
	continue;
      }
      //
      const ListOfInteger& aNeighbour = anArrayOfLines[aneighbourindex];
      Standard_Integer anIndex = (!j) ? aNeighbour.Last() : aNeighbour.First();
      const IntSurf_PntOn2S& aPoint = theWLine->Point(anIndex);
      // check if need use derivative.begin .end [absence]
      //
      IntSurf_PntOn2S aNewP = aPoint;
      Standard_Integer surfit, parit;
      //
      for(surfit = 0; surfit < 2; ++surfit) {

	Handle(GeomAdaptor_HSurface) aGASurface = (!surfit) ? theSurface1 : theSurface2;
	
	aGASurface->ChangeSurface().Surface()->Bounds(umin, umax, vmin, vmax);
	Standard_Real U=0., V=0.;

	if(!surfit) {
	  aNewP.ParametersOnS1(U, V);
	}
	else {
	  aNewP.ParametersOnS2(U, V);
	}
	//
	Standard_Integer nbboundaries = 0;
	Standard_Integer bIsUBoundary = Standard_False; // use if nbboundaries == 1
	Standard_Integer bIsFirstBoundary = Standard_False; // use if nbboundaries == 1
	//
	for(parit = 0; parit < 2; parit++) {
	  Standard_Boolean isperiodic = (!parit) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();

	  Standard_Real aResolution = (!parit) ? aGASurface->UResolution(aTol) : aGASurface->VResolution(aTol);
	  Standard_Real alowerboundary = (!parit) ? umin : vmin;
	  Standard_Real aupperboundary = (!parit) ? umax : vmax;

	  Standard_Real aParameter = (!parit) ? U : V;
	  Standard_Boolean bIsOnFirstBoundary = Standard_True;
  
	  if(!isperiodic) {
	    if(IsPointOnBoundary(aParameter, alowerboundary, aupperboundary, aResolution, bIsOnFirstBoundary)) {
	      bIsUBoundary = (!parit);
	      bIsFirstBoundary = bIsOnFirstBoundary;
	      nbboundaries++;
	    }
	  }
	  else {
	    Standard_Real aPeriod     = (!parit) ? aGASurface->UPeriod() : aGASurface->VPeriod();
	    Standard_Real anoffset = 0.;
	    Standard_Real anAdjustPar = AdjustPeriodic(aParameter, alowerboundary, aupperboundary, aPeriod, anoffset);

	    if(IsPointOnBoundary(anAdjustPar, alowerboundary, aupperboundary, aResolution, bIsOnFirstBoundary)) {
	      bIsUBoundary = (parit == 0);
	      bIsFirstBoundary = bIsOnFirstBoundary;
	      nbboundaries++;
	    }
	  }
	}
	//
	Standard_Boolean bComputeLineEnd = Standard_False;
	
	if(nbboundaries == 2) {
	  bComputeLineEnd = Standard_True;
	}
	else if(nbboundaries == 1) {
	  Standard_Boolean isperiodic = (bIsUBoundary) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();

	  if(isperiodic) {
	    Standard_Real alowerboundary = (bIsUBoundary) ? umin : vmin;
	    Standard_Real aupperboundary = (bIsUBoundary) ? umax : vmax;
	    Standard_Real aPeriod     = (bIsUBoundary) ? aGASurface->UPeriod() : aGASurface->VPeriod();
	    Standard_Real aParameter = (bIsUBoundary) ? U : V;
	    Standard_Real anoffset = 0.;
	    Standard_Real anAdjustPar = AdjustPeriodic(aParameter, alowerboundary, aupperboundary, aPeriod, anoffset);

	    Standard_Real adist = (bIsFirstBoundary) ? fabs(anAdjustPar - alowerboundary) : fabs(anAdjustPar - aupperboundary);
	    Standard_Real anotherPar = (bIsFirstBoundary) ? (aupperboundary - adist) : (alowerboundary + adist);
	    anotherPar += anoffset;
	    Standard_Integer aneighbourpointindex = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
	    const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex);
	    Standard_Real nU1, nV1;

	    if(surfit == 0)
	      aNeighbourPoint.ParametersOnS1(nU1, nV1);
	    else
	      aNeighbourPoint.ParametersOnS2(nU1, nV1);
	    
	    Standard_Real adist1 = (bIsUBoundary) ? fabs(nU1 - U) : fabs(nV1 - V);
	    Standard_Real adist2 = (bIsUBoundary) ? fabs(nU1 - anotherPar) : fabs(nV1 - anotherPar);
	    bComputeLineEnd = Standard_True;
	    Standard_Boolean bCheckAngle1 = Standard_False;
	    Standard_Boolean bCheckAngle2 = Standard_False;
	    gp_Vec2d aNewVec;
	    Standard_Real anewU = (bIsUBoundary) ? anotherPar : U;
	    Standard_Real anewV = (bIsUBoundary) ? V : anotherPar;
	    //
	    if(((adist1 - adist2) > Precision::PConfusion()) && 
	       (adist2 < (aPeriod / 4.))) {
	      bCheckAngle1 = Standard_True;
	      aNewVec = gp_Vec2d(gp_Pnt2d(nU1, nV1), gp_Pnt2d(anewU, anewV));

	      if(aNewVec.SquareMagnitude() < (gp::Resolution() * gp::Resolution())) {
		aNewP.SetValue((surfit == 0), anewU, anewV);
		bCheckAngle1 = Standard_False;
	      }
	    }
	    else if(adist1 < (aPeriod / 4.)) {
	      bCheckAngle2 = Standard_True;
	      aNewVec = gp_Vec2d(gp_Pnt2d(nU1, nV1), gp_Pnt2d(U, V));

	      if(aNewVec.SquareMagnitude() < (gp::Resolution() * gp::Resolution())) {
		bCheckAngle2 = Standard_False;
	      }
	    }
	    //
	    if(bCheckAngle1 || bCheckAngle2) {
	      // assume there are at least two points in line (see "if" above)
	      Standard_Integer anindexother = aneighbourpointindex;
	      
	      while((anindexother <= aListOfIndex.Last()) && (anindexother >= aListOfIndex.First())) {
		anindexother = (j == 0) ? (anindexother + 1) : (anindexother - 1);
		const IntSurf_PntOn2S& aPrevNeighbourPoint = theWLine->Point(anindexother);
		Standard_Real nU2, nV2;
		
		if(surfit == 0)
		  aPrevNeighbourPoint.ParametersOnS1(nU2, nV2);
		else
		  aPrevNeighbourPoint.ParametersOnS2(nU2, nV2);
		gp_Vec2d aVecOld(gp_Pnt2d(nU2, nV2), gp_Pnt2d(nU1, nV1));

		if(aVecOld.SquareMagnitude() <= (gp::Resolution() * gp::Resolution())) {
		  continue;
		}
		else {
		  Standard_Real anAngle = aNewVec.Angle(aVecOld);

		  if((fabs(anAngle) < (M_PI * 0.25)) && (aNewVec.Dot(aVecOld) > 0.)) {

		    if(bCheckAngle1) {
		      Standard_Real U1, U2, V1, V2;
		      IntSurf_PntOn2S atmppoint = aNewP;
		      atmppoint.SetValue((surfit == 0), anewU, anewV);
		      atmppoint.Parameters(U1, V1, U2, V2);
		      gp_Pnt P1 = theSurface1->Value(U1, V1);
		      gp_Pnt P2 = theSurface2->Value(U2, V2);
		      gp_Pnt P0 = aPoint.Value();

		      if(P0.IsEqual(P1, aTol) &&
			 P0.IsEqual(P2, aTol) &&
			 P1.IsEqual(P2, aTol)) {
			bComputeLineEnd = Standard_False;
			aNewP.SetValue((surfit == 0), anewU, anewV);
		      }
		    }
		    
		    if(bCheckAngle2) {
		      bComputeLineEnd = Standard_False;
		    }
		  }
		  break;
		}
	      } // end while(anindexother...)
	    }
	  }
	}
	//
	if(bComputeLineEnd) {
	  Standard_Integer aneighbourpointindex1 = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
	  const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex1);
	  Standard_Real nU1, nV1;

	  if(surfit == 0)
	    aNeighbourPoint.ParametersOnS1(nU1, nV1);
	  else
	    aNeighbourPoint.ParametersOnS2(nU1, nV1);
	  gp_Pnt2d ap1(nU1, nV1);
	  gp_Pnt2d ap2(nU1, nV1);
	  Standard_Integer aneighbourpointindex2 = aneighbourpointindex1;

	  while((aneighbourpointindex2 <= aListOfIndex.Last()) && (aneighbourpointindex2 >= aListOfIndex.First())) {
	    aneighbourpointindex2 = (j == 0) ? (aneighbourpointindex2 + 1) : (aneighbourpointindex2 - 1);
	    const IntSurf_PntOn2S& aPrevNeighbourPoint = theWLine->Point(aneighbourpointindex2);
	    Standard_Real nU2, nV2;

	    if(surfit == 0)
	      aPrevNeighbourPoint.ParametersOnS1(nU2, nV2);
	    else
	      aPrevNeighbourPoint.ParametersOnS2(nU2, nV2);
	    ap2.SetX(nU2);
	    ap2.SetY(nV2);

	    if(ap1.SquareDistance(ap2) > (gp::Resolution() * gp::Resolution())) {
	      break;
	    }
	  }	  
	  gp_Pnt2d anewpoint;
	  Standard_Boolean found = FindPoint(ap2, ap1, umin, umax, vmin, vmax, anewpoint);

	  if(found) {
	    // check point
	    Standard_Real aCriteria =aTolSum;// BRep_Tool::Tolerance(theFace1) + BRep_Tool::Tolerance(theFace2);
	    //GeomAPI_ProjectPointOnSurf& aProjector = (surfit == 0) ? aPrj2 : aPrj1;
	    ProjectPointOnSurf& aProjector = (surfit == 0) ? aPrj2 : aPrj1;
	    Handle(GeomAdaptor_HSurface) aSurface = (surfit == 0) ? theSurface1 : theSurface2;

	    gp_Pnt aP3d = aSurface->Value(anewpoint.X(), anewpoint.Y());
	    aProjector.Perform(aP3d);

	    if(aProjector.IsDone()) {
	      if(aProjector.LowerDistance() < aCriteria) {
		Standard_Real foundU = U, foundV = V;
		aProjector.LowerDistanceParameters(foundU, foundV);

		if(surfit == 0)
		  aNewP.SetValue(aP3d, anewpoint.X(), anewpoint.Y(), foundU, foundV);
		else
		  aNewP.SetValue(aP3d, foundU, foundV, anewpoint.X(), anewpoint.Y());
	      }
	    }
	  }
	}
      }
      aSeqOfPntOn2S->Add(aNewP);
      aListOfFLIndex.Append(aSeqOfPntOn2S->NbPoints());
    }
    anArrayOfLineEnds.SetValue(i, aListOfFLIndex);
  }
  // Correct wlines.end

  // Split wlines.begin
  for(j = 1; j <= theLConstructor.NbParts(); j++) {
    Standard_Real fprm = 0., lprm = 0.;
    theLConstructor.Part(j, fprm, lprm);
    Standard_Integer ifprm = (Standard_Integer)fprm;
    Standard_Integer ilprm = (Standard_Integer)lprm;
    //
    Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
    //
    for(i = 1; i <= nblines; i++) {
      if(anArrayOfLineType[i] != 0) {
	continue;
      }
      const ListOfInteger& aListOfIndex = anArrayOfLines[i];

      if(aListOfIndex.Extent() < 2) {
	continue;
      }
      const TColStd_ListOfInteger& aListOfFLIndex = anArrayOfLineEnds.Value(i);
      Standard_Boolean bhasfirstpoint = (aListOfFLIndex.Extent() == 2);
      Standard_Boolean bhaslastpoint = (aListOfFLIndex.Extent() == 2);

      if(!bhasfirstpoint && !aListOfFLIndex.IsEmpty()) {
	bhasfirstpoint = (i != 1);
      }

      if(!bhaslastpoint && !aListOfFLIndex.IsEmpty()) {
	bhaslastpoint = (i != nblines);
      }
      Standard_Boolean bIsFirstInside = ((ifprm >= aListOfIndex.First()) && (ifprm <= aListOfIndex.Last()));
      Standard_Boolean bIsLastInside =  ((ilprm >= aListOfIndex.First()) && (ilprm <= aListOfIndex.Last()));

      if(!bIsFirstInside && !bIsLastInside) {
	if((ifprm < aListOfIndex.First()) && (ilprm > aListOfIndex.Last())) {
	  // append whole line, and boundaries if neccesary
	  if(bhasfirstpoint) {
	    const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.First());
	    aLineOn2S->Add(aP);
	  }
	  ListOfInteger::Iterator anIt(aListOfIndex);

	  for(; anIt.More(); anIt.Next()) {
	    const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
	    aLineOn2S->Add(aP);
	  }

	  if(bhaslastpoint) {
	    const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.Last());
	    aLineOn2S->Add(aP);
	  }

	  // check end of split line (end is almost always)
	  Standard_Integer aneighbour = i + 1;
	  Standard_Boolean bIsEndOfLine = Standard_True;

	  if(aneighbour <= nblines) {
	    const ListOfInteger& aListOfNeighbourIndex = anArrayOfLines[aneighbour];

	    if((anArrayOfLineType[aneighbour] != 0) &&
	       (aListOfNeighbourIndex.IsEmpty())) {
	      bIsEndOfLine = Standard_False;
	    }
	  }

	  if(bIsEndOfLine) {
	    if(aLineOn2S->NbPoints() > 1) {
	      Handle(IntPatch_WLine) aNewWLine = 
		new IntPatch_WLine(aLineOn2S, Standard_False);
	      theNewLines.Append(aNewWLine);
	    }
	    aLineOn2S = new IntSurf_LineOn2S();
	  }
	}
	continue;
      }
      // end if(!bIsFirstInside && !bIsLastInside)

      if(bIsFirstInside && bIsLastInside) {
	// append inside points between ifprm and ilprm
	ListOfInteger::Iterator anIt(aListOfIndex);

	for(; anIt.More(); anIt.Next()) {
	  if((anIt.Value() < ifprm) || (anIt.Value() > ilprm))
	    continue;
	  const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
	  aLineOn2S->Add(aP);
	}
      }
      else {

	if(bIsFirstInside) {
	  // append points from ifprm to last point + boundary point
	  ListOfInteger::Iterator anIt(aListOfIndex);

	  for(; anIt.More(); anIt.Next()) {
	    if(anIt.Value() < ifprm)
	      continue;
	    const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
	    aLineOn2S->Add(aP);
	  }

	  if(bhaslastpoint) {
	    const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.Last());
	    aLineOn2S->Add(aP);
	  }
	  // check end of split line (end is almost always)
	  Standard_Integer aneighbour = i + 1;
	  Standard_Boolean bIsEndOfLine = Standard_True;

	  if(aneighbour <= nblines) {
	    const ListOfInteger& aListOfNeighbourIndex = anArrayOfLines[aneighbour];

	    if((anArrayOfLineType[aneighbour] != 0) &&
	       (aListOfNeighbourIndex.IsEmpty())) {
	      bIsEndOfLine = Standard_False;
	    }
	  }

	  if(bIsEndOfLine) {
	    if(aLineOn2S->NbPoints() > 1) {
	      Handle(IntPatch_WLine) aNewWLine = 
		new IntPatch_WLine(aLineOn2S, Standard_False);
	      theNewLines.Append(aNewWLine);
	    }
	    aLineOn2S = new IntSurf_LineOn2S();
	  }
	}
	// end if(bIsFirstInside)

	if(bIsLastInside) {
	  // append points from first boundary point to ilprm
	  if(bhasfirstpoint) {
	    const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.First());
	    aLineOn2S->Add(aP);
	  }
	  ListOfInteger::Iterator anIt(aListOfIndex);

	  for(; anIt.More(); anIt.Next()) {
	    if(anIt.Value() > ilprm)
	      continue;
	    const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
	    aLineOn2S->Add(aP);
	  }
	}
	//end if(bIsLastInside)
      }
    }

    if(aLineOn2S->NbPoints() > 1) {
      Handle(IntPatch_WLine) aNewWLine = 
	new IntPatch_WLine(aLineOn2S, Standard_False);
      theNewLines.Append(aNewWLine);
    }
  }
  // Split wlines.end
  //
  // cda002/I3
  Standard_Real fprm, lprm;
  Standard_Integer ifprm, ilprm, aNbPoints, aIndex;
  //
  aNbParts=theLConstructor.NbParts();
  //
  for(j = 1; j <= aNbParts; j++) {
    theLConstructor.Part(j, fprm, lprm);
    ifprm=(Standard_Integer)fprm;
    ilprm=(Standard_Integer)lprm;
    //
    if ((ilprm-ifprm)==1) {
      for(i = 1; i <= nblines; i++) {
	aLineType=anArrayOfLineType[i];
	if(aLineType) {
	  continue;
	}
	//
	const ListOfInteger& aListOfIndex = anArrayOfLines[i];
	aNbPoints=aListOfIndex.Extent();
	if(aNbPoints==1) {
	  aIndex=aListOfIndex.First();
	  if (aIndex==ifprm || aIndex==ilprm) {
	    Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
	    const IntSurf_PntOn2S& aP1 = theWLine->Point(ifprm);
	    const IntSurf_PntOn2S& aP2 = theWLine->Point(ilprm);
	    aLineOn2S->Add(aP1);
	    aLineOn2S->Add(aP2);
	    Handle(IntPatch_WLine) aNewWLine = 
	      new IntPatch_WLine(aLineOn2S, Standard_False);
	    theNewLines.Append(aNewWLine);
	  }
	}
      }
    }
  }
  //
  return Standard_True;
}

//=======================================================================
//function : AdjustPeriodic
//purpose  : 
//=======================================================================
Standard_Real AdjustPeriodic(const Standard_Real theParameter,
                             const Standard_Real parmin,
                             const Standard_Real parmax,
                             const Standard_Real thePeriod,
                             Standard_Real&      theOffset)
{
  Standard_Real aresult = theParameter;
  theOffset = 0.;
  while(aresult < parmin) {
    aresult += thePeriod;
    theOffset += thePeriod;
  }
  while(aresult > parmax) {
    aresult -= thePeriod;
    theOffset -= thePeriod;
  }
  return aresult;
}

//=======================================================================
//function : IsPointOnBoundary
//purpose  : 
//=======================================================================
Standard_Boolean IsPointOnBoundary(const Standard_Real theParameter,
                                   const Standard_Real theFirstBoundary,
                                   const Standard_Real theSecondBoundary,
                                   const Standard_Real theResolution,
                                   Standard_Boolean&   IsOnFirstBoundary)
{
  IsOnFirstBoundary = Standard_True;
  if(fabs(theParameter - theFirstBoundary) < theResolution)
    return Standard_True;
  if(fabs(theParameter - theSecondBoundary) < theResolution)
  {
    IsOnFirstBoundary = Standard_False;
    return Standard_True;
  }
  return Standard_False;
}
//=======================================================================
//function : FindPoint
//purpose  : 
//=======================================================================
Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
                           const gp_Pnt2d&     theLastPoint,
                           const Standard_Real theUmin,
                           const Standard_Real theUmax,
                           const Standard_Real theVmin,
                           const Standard_Real theVmax,
                           gp_Pnt2d&           theNewPoint)
{
  gp_Vec2d aVec(theFirstPoint, theLastPoint);
  Standard_Integer i = 0, j = 0;

  for(i = 0; i < 4; i++) {
    gp_Vec2d anOtherVec;
    gp_Vec2d anOtherVecNormal;
    gp_Pnt2d aprojpoint = theLastPoint;    

    if((i % 2) == 0) {
      anOtherVec.SetX(0.);
      anOtherVec.SetY(1.);
      anOtherVecNormal.SetX(1.);
      anOtherVecNormal.SetY(0.);

      if(i < 2)
	aprojpoint.SetX(theUmin);
      else
	aprojpoint.SetX(theUmax);
    }
    else {
      anOtherVec.SetX(1.);
      anOtherVec.SetY(0.);
      anOtherVecNormal.SetX(0.);
      anOtherVecNormal.SetY(1.);

      if(i < 2)
	aprojpoint.SetY(theVmin);
      else
	aprojpoint.SetY(theVmax);
    }
    gp_Vec2d anormvec = aVec;
    anormvec.Normalize();
    Standard_Real adot1 = anormvec.Dot(anOtherVecNormal);

    if(fabs(adot1) < Precision::Angular())
      continue;
    Standard_Real adist = 0.;

    if((i % 2) == 0) {
      adist = (i < 2) ? fabs(theLastPoint.X() - theUmin) : fabs(theLastPoint.X() - theUmax);
    }
    else {
      adist = (i < 2) ? fabs(theLastPoint.Y() - theVmin) : fabs(theLastPoint.Y() - theVmax);
    }
    Standard_Real anoffset = adist * anOtherVec.Dot(anormvec) / adot1;

    for(j = 0; j < 2; j++) {
      anoffset = (j == 0) ? anoffset : -anoffset;
      gp_Pnt2d acurpoint(aprojpoint.XY() + (anOtherVec.XY()*anoffset));
      gp_Vec2d acurvec(theLastPoint, acurpoint);

      //
      Standard_Real aDotX, anAngleX, aPC;
      //
      aDotX=aVec.Dot(acurvec);
      anAngleX=aVec.Angle(acurvec);
      aPC=Precision::PConfusion();
      //
      if(aDotX > 0. && fabs(anAngleX) < aPC) {
      //
	if((i % 2) == 0) {
	  if((acurpoint.Y() >= theVmin) &&
	     (acurpoint.Y() <= theVmax)) {
	    theNewPoint = acurpoint;
	    return Standard_True;
	  }
	}
	else {
	  if((acurpoint.X() >= theUmin) &&
	     (acurpoint.X() <= theUmax)) {
	    theNewPoint = acurpoint;
	    return Standard_True;
	  }
	}
      }
    }
  }
  return Standard_False;
}

/*
static
  void DumpWLine (const Handle(IntPatch_WLine)& theWLine);
//=======================================================================
//function : DumpWLine
//purpose  : 
//=======================================================================
  void DumpWLine (const Handle(IntPatch_WLine)& theWLine)
{
  Standard_Integer i, nbp;
  Standard_Real u1,v1,u2,v2;
  //
  nbp=theWLine->NbPnts();
  printf(" nbp=%d\n", nbp);
  for(i=1;i<=nbp;i++) {
    const IntSurf_PntOn2S& aPoint = theWLine->Point(i);
    const gp_Pnt& aP=aPoint.Value();
    aPoint.Parameters(u1,v1,u2,v2);
    printf("point i%d %lf %lf %lf [ %lf %lf ] [ %lf %lf ]\n", 
	   i, aP.X(), aP.Y(), aP.Z(), u1, v1, u2, v2);
  }
}
*/
