// Created on: 2001-04-02
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <BOPTools_Tools2D.ixx>

#include <Standard_NotImplemented.hxx>
#include <Precision.hxx>
#include <gp.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>

#include <Geom_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Surface.hxx>

#include <TopLoc_Location.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>

#include <ProjLib_ProjectedCurve.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Surface.hxx>


static 
  Standard_Boolean CheckEdgeLength (const TopoDS_Edge& E);

//=======================================================================
//function : EdgeTangent
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools2D::EdgeTangent(const TopoDS_Edge& anEdge, 
						 const Standard_Real aT,
						 gp_Vec& aTau)
{
  Standard_Boolean isdgE;
  Standard_Real first, last;
  
  isdgE = BRep_Tool::Degenerated(anEdge); 
  if (isdgE) {
    return Standard_False;
  }
  if (!CheckEdgeLength(anEdge)) {
    return Standard_False;
  }

  Handle(Geom_Curve) aC=BRep_Tool::Curve(anEdge, first, last);
  gp_Pnt aP;
  aC->D1(aT, aP, aTau);
  Standard_Real mod = aTau.Magnitude();
  if(mod > gp::Resolution()) {
    aTau /= mod;
  }
  else {
    return Standard_False;
  }
  //aTau.Normalize(); 
  if (anEdge.Orientation() == TopAbs_REVERSED){
    aTau.Reverse();
  }
  return Standard_True;
}


//=======================================================================
//function : FaceNormal
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::FaceNormal (const TopoDS_Face& aF,
				     const Standard_Real U,
				     const Standard_Real V,
				     gp_Vec& aN)
{
  gp_Pnt aPnt ;
  gp_Vec aD1U, aD1V;
  Handle(Geom_Surface) aSurface;

  aSurface=BRep_Tool::Surface(aF);
  aSurface->D1 (U, V, aPnt, aD1U, aD1V);
  aN=aD1U.Crossed(aD1V);
  aN.Normalize();  
  if (aF.Orientation() == TopAbs_REVERSED){
    aN.Reverse();
  }
  return;
}
//=======================================================================
//function : RemovePCurveForEdgeOnFace
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::RemovePCurveForEdgeOnFace (const TopoDS_Edge& aE,
						    const TopoDS_Face& aF)
{
  BRep_Builder aBB;
  Handle(Geom2d_Curve) aC2D;
  Standard_Real  aTol;

  aTol=BRep_Tool::Tolerance(aE);
  aBB.UpdateEdge(aE, aC2D, aF, aTol);
}
//=======================================================================
//function : BuildPCurveForEdgeOnFace
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::BuildPCurveForEdgeOnFace (const TopoDS_Edge& aE,
						   const TopoDS_Face& aF)
{
  BRep_Builder aBB;
  Handle(Geom2d_Curve) aC2D;
  Standard_Real  aTolPC, aTolFact, aTolEdge, aFirst, aLast;
  
  Standard_Boolean aHasOld;
  aHasOld=BOPTools_Tools2D::HasCurveOnSurface (aE, aF, aC2D, aFirst, aLast, aTolEdge);
  if (aHasOld) {
    return;
  }
  

  BOPTools_Tools2D::CurveOnSurface(aE, aF, aC2D, aTolPC, Standard_True);
  
  aTolEdge=BRep_Tool::Tolerance(aE);

  aTolFact=Max(aTolEdge, aTolPC);

  aBB.UpdateEdge(aE, aC2D, aF, aTolFact);
  return;
}
//=======================================================================
//function : PointOnOnSurface
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::PointOnSurface (const TopoDS_Edge& aE,
					 const TopoDS_Face& aF,
					 const Standard_Real aParameter,
					 Standard_Real& U,
					 Standard_Real& V)
{
  gp_Pnt2d aP2D;
  Handle(Geom2d_Curve) aC2D;
  Standard_Real aToler, aFirst, aLast;

  BOPTools_Tools2D::CurveOnSurface (aE, aF, aC2D, aFirst, aLast, aToler, Standard_True); 
  aC2D->D0(aParameter, aP2D);
  U=aP2D.X();
  V=aP2D.Y();
  return;
}

//=======================================================================
//function : CurveOnSurface
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::CurveOnSurface (const TopoDS_Edge& aE,
					 const TopoDS_Face& aF,
					 Handle(Geom2d_Curve)& aC2D,
					 Standard_Real& aToler,
					 const Standard_Boolean trim3d)
{
  Standard_Real aFirst, aLast; 

  BOPTools_Tools2D::CurveOnSurface (aE, aF, aC2D, aFirst, aLast, aToler, trim3d); 

  return;
}
//=======================================================================
//function : CurveOnSurface
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::CurveOnSurface (const TopoDS_Edge& aE,
					 const TopoDS_Face& aF,
					 Handle(Geom2d_Curve)& aC2D,
					 Standard_Real& aFirst,
					 Standard_Real& aLast,
					 Standard_Real& aToler,
					 const Standard_Boolean trim3d)
{
  Standard_Boolean aHasOld;
  Handle(Geom2d_Curve) C2D;

  aHasOld=BOPTools_Tools2D::HasCurveOnSurface (aE, aF, C2D, aFirst, aLast, aToler);
  if (aHasOld) {
    aC2D=C2D;
    return;
  }

  BOPTools_Tools2D::Make2D(aE, aF, C2D, aFirst, aLast, aToler, trim3d);
  aC2D=C2D;
  return;
}

//=======================================================================
//function : HasCurveOnSurface
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools2D::HasCurveOnSurface (const TopoDS_Edge& aE,
							const TopoDS_Face& aF,
							Handle(Geom2d_Curve)& aC2D,
							Standard_Real& aFirst,
							Standard_Real& aLast,
							Standard_Real& aToler)
{
  Standard_Boolean aHasOld;
  
  aToler=BRep_Tool::Tolerance(aE);
  BRep_Tool::Range(aE, aFirst, aLast);

  if((aLast - aFirst) < Precision::PConfusion()) {
    return Standard_False;
  }

  aC2D  =BRep_Tool::CurveOnSurface(aE, aF, aFirst, aLast);
  aHasOld=!aC2D.IsNull();
  return aHasOld;
}
//=======================================================================
//function : HasCurveOnSurface
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools2D::HasCurveOnSurface (const TopoDS_Edge& aE,
							const TopoDS_Face& aF)
						   
{
  Standard_Boolean aHasOld;
  Handle(Geom2d_Curve) aC2D;
  Standard_Real aFirst, aLast;
  BRep_Tool::Range(aE, aFirst, aLast);

  if((aLast - aFirst) < Precision::PConfusion()) {
    return Standard_False;
  }

  aC2D  =BRep_Tool::CurveOnSurface(aE, aF, aFirst, aLast);
  aHasOld=!aC2D.IsNull();
  return aHasOld;
}
//=======================================================================
//function : MakeCurveOnSurface
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::MakeCurveOnSurface (const TopoDS_Edge& aE,
					     const TopoDS_Face& aF,
					     Handle(Geom2d_Curve)& aC2D,
					     Standard_Real& aFirst,
					     Standard_Real& aLast,
					     Standard_Real& aToler,
					     const Standard_Boolean trim3d)
{
  BOPTools_Tools2D::Make2D(aE, aF, aC2D, aFirst, aLast, aToler, trim3d);
}

//=======================================================================
//function : Make2D
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::Make2D (const TopoDS_Edge& aE,
				 const TopoDS_Face& aF,
				 Handle(Geom2d_Curve)& aC2D,
				 Standard_Real& aFirst,
				 Standard_Real& aLast,
				 Standard_Real& aToler,
				 const Standard_Boolean trim3d)
{
  Standard_Boolean aLocIdentity;
  Standard_Real f3d, l3d;
  TopLoc_Location aLoc;

  Handle(Geom2d_Curve) C2D; 
  
  
  C2D=BRep_Tool::CurveOnSurface(aE, aF, aFirst, aLast);
  
  if (!C2D.IsNull()) {
    aC2D=C2D;
    return;
  }

  Handle(Geom_Curve) C3D3, C3D2, C3D;
  C3D = BRep_Tool::Curve(aE, aLoc, f3d, l3d);
  //
  if (C3D.IsNull()) { 
    // aE has no 3D curve, so nothing is done
  }
  //
  aLocIdentity=aLoc.IsIdentity();
    
  if (aLocIdentity) {
    C3D2 = C3D;
  }
  else {
    C3D2 = Handle(Geom_Curve)::
      DownCast(C3D->Transformed(aLoc.Transformation()));
  }
  
  if (trim3d) {
    
    C3D3=C3D2;
  }
  
  else {
    C3D3=C3D2;
  }
  //
  aToler=.5*BRep_Tool::Tolerance(aE);
  BOPTools_Tools2D::MakePCurveOnFace(aF, C3D3, f3d, l3d, aC2D, aToler);
  //
  aFirst = f3d; 
  aLast  = l3d;
}

//=======================================================================
//function : MakePCurveOnFace
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::MakePCurveOnFace (const TopoDS_Face& aF,
					   const Handle(Geom_Curve)& aC3D,
					   Handle(Geom2d_Curve)& aC2D, //->
					   Standard_Real& TolReached2d)
{
  Standard_Real aFirst, aLast;

  aFirst = aC3D -> FirstParameter();
  aLast  = aC3D -> LastParameter();
  //
  TolReached2d=0.;
  //
  BOPTools_Tools2D::MakePCurveOnFace (aF, aC3D, aFirst, aLast, aC2D, TolReached2d);
}

//=======================================================================
//function : MakePCurveOnFace
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::MakePCurveOnFace (const TopoDS_Face& aF,
					   const Handle(Geom_Curve)& aC3D,
					   const Standard_Real aFirst,
					   const Standard_Real aLast,
					   Handle(Geom2d_Curve)& aC2D, 
					   Standard_Real& TolReached2d)
{
  Standard_Real aTolR;
  Handle(Geom2d_Curve) aC2DA;

  BRepAdaptor_Surface aBAS(aF, Standard_False);
  Handle(BRepAdaptor_HSurface) aBAHS = new BRepAdaptor_HSurface(aBAS);
  Handle(GeomAdaptor_HCurve)   aBAHC = new GeomAdaptor_HCurve(aC3D, aFirst, aLast);
  
  ProjLib_ProjectedCurve aProjCurv(aBAHS, aBAHC);// 1
  BOPTools_Tools2D::MakePCurveOfType(aProjCurv, aC2D);
  aTolR=aProjCurv.GetTolerance();
  //
  if (aC2D.IsNull()) { 
    ProjLib_ProjectedCurve aProjCurvAgain(aBAHS, aBAHC, TolReached2d);// 2
    BOPTools_Tools2D::MakePCurveOfType(aProjCurvAgain, aC2D);
    aTolR = aProjCurvAgain.GetTolerance();
    //
    if (aC2D.IsNull()) { 
      Standard_Real aTR=0.0001;
      ProjLib_ProjectedCurve aProj3(aBAHS, aBAHC, aTR);// 3
      BOPTools_Tools2D::MakePCurveOfType(aProj3, aC2D);
      aTolR = aProj3.GetTolerance();
    }
  }
  TolReached2d=aTolR;
  
  BOPTools_Tools2D::AdjustPCurveOnFace (aF, aFirst, aLast, aC2D, aC2DA);
  aC2D=aC2DA;
}

//=======================================================================
//function : AdjustPCurveOnFace
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::AdjustPCurveOnFace (const TopoDS_Face& aF,
					     const Handle(Geom_Curve)&   aC3D,
					     const Handle(Geom2d_Curve)& aC2D, 
					     Handle(Geom2d_Curve)& aC2DA)
{
  Standard_Real first, last;

  first = aC3D -> FirstParameter();
  last  = aC3D -> LastParameter();
 
  BOPTools_Tools2D::AdjustPCurveOnFace (aF, first, last, aC2D, aC2DA);
} 
//=======================================================================
//function : AdjustPCurveOnFace
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::AdjustPCurveOnFace (const TopoDS_Face& aF,
					     const Standard_Real aFirst,
					     const Standard_Real aLast,
					     const Handle(Geom2d_Curve)& aC2D, 
					     Handle(Geom2d_Curve)& aC2DA)
{
  Standard_Boolean mincond, maxcond, decalu, decalv;
  Standard_Integer k, iCnt;
  Standard_Real UMin, UMax, VMin, VMax, aT, u2, v2, du, dv, aDelta;
  Standard_Real aUPeriod, aUP2, aUP1, aUNew, aDif, aUx;
  //
  aDelta=Precision::PConfusion();
  
  BRepAdaptor_Surface aBAS(aF, Standard_False);
 
  BRepTools::UVBounds(aF, UMin, UMax, VMin, VMax);
  
  aT =.5*(aFirst+aLast);

  gp_Pnt2d pC2D; 
  aC2D->D0(aT, pC2D);

  u2 = pC2D.X();
  v2 = pC2D.Y();
 
  du = 0.;
  if (aBAS.IsUPeriodic()) {
    aUPeriod=aBAS.UPeriod(); 
    mincond = (u2 < UMin-aDelta);
    maxcond = (u2 > UMax+aDelta); 
    
    decalu = mincond || maxcond;
    if (decalu) {
      //modified by NIZNHY-PKV Mon Mar 25 16:44:46 2008f
      //du = ( mincond ) ? UPeriod : -UPeriod;
      //
      iCnt=1;
      aUP2=aUPeriod+aUPeriod+aDelta;
      aUP1=aUPeriod+aDelta;
      //
      if (u2 > aUP2) {
	for(k=1; 1; ++k) {
	  aUx=u2-k*aUPeriod;
	  if (aUx < aUP1) {
	    iCnt=k;
	    break;
	  }
	}
      }
      else if (u2 < -aUP2) {
	for(k=1; 1; ++k) {
	  aUx=u2+k*aUPeriod;
	  if (aUx > -aUP1) {
	    iCnt=k;
	    break;
	  }
	}
      }
      du = ( mincond ) ? aUPeriod : -aUPeriod;
      du=iCnt*du;
      //modified by NIZNHY-PKV Mon Mar 25 16:44:49 2008t
    }
    //
    aUNew=u2+du;
    if (aUNew<(UMin-aDelta) || 
	aUNew>(UMax+aDelta)) {
      // So previous correction was wrong.
      // Try to be closer to UMin or UMax.
      du=0.;
      if (u2>UMax){
	aDif=u2-UMax;
	if (aDif < 4.e-7) {
	  du=-aDif;
	}
      }
    }
  } // if (BAHS->IsUPeriodic())
  //
  // dv
  dv = 0.;
  if (aBAS.IsVPeriodic()) {
    Standard_Real aVPeriod, aVm, aVr, aVmid, dVm, dVr;
    //
    aVPeriod=aBAS.VPeriod();
    mincond = (VMin - v2 > aDelta);
    maxcond = (v2 - VMax > aDelta);
    decalv = mincond || maxcond;
    if (decalv) {
      dv = ( mincond ) ? aVPeriod : -aVPeriod;
    }
    //
    //xf
    if ((VMax-VMin<aVPeriod) && dv) {
      aVm=v2;
      aVr=v2+dv;
      aVmid=0.5*(VMin+VMax);
      dVm=fabs(aVm-aVmid);
      dVr=fabs(aVr-aVmid);
      if (dVm<dVr) {
	dv=0.;
      }
    }
    //xt
  }
  //
  // Translation if necessary
  Handle(Geom2d_Curve) aC2Dx=aC2D;

  if ( du != 0. || dv != 0.) {
    Handle(Geom2d_Curve) PCT = Handle(Geom2d_Curve)::DownCast(aC2Dx->Copy());
    gp_Vec2d aV2D(du,dv);
    PCT->Translate(aV2D);
    aC2Dx = PCT;
  }

  aC2DA=aC2Dx;
}


//=======================================================================
//function : MakePCurveOfType
//purpose  : 
//=======================================================================
  void  BOPTools_Tools2D::MakePCurveOfType(const ProjLib_ProjectedCurve& PC, 
					   Handle(Geom2d_Curve)& C2D)
{
  
  switch (PC.GetType()) {

  case GeomAbs_Line : 
    C2D = new Geom2d_Line(PC.Line()); 
    break;
  case GeomAbs_Circle : 
    C2D = new Geom2d_Circle(PC.Circle());
    break;
  case GeomAbs_Ellipse :
    C2D = new Geom2d_Ellipse(PC.Ellipse());
    break;
  case GeomAbs_Parabola : 
    C2D = new Geom2d_Parabola(PC.Parabola()); 
    break;
  case GeomAbs_Hyperbola : 
    C2D = new Geom2d_Hyperbola(PC.Hyperbola()); 
    break;
  case GeomAbs_BSplineCurve :
    C2D = PC.BSpline(); 
    break;
  case GeomAbs_BezierCurve : 
  case GeomAbs_OtherCurve : 
    default :
    Standard_NotImplemented::Raise("BOPTools_Tools2D::MakePCurveOfType");
    break;
  }
}

//=======================================================================
//function : TangentOnEdge
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools2D::TangentOnEdge(const Standard_Real par, 
						   const TopoDS_Edge& E, 
						   gp_Vec& Tg)
{
  Standard_Boolean isdgE;
  
  isdgE = BRep_Tool::Degenerated(E); 
  if (isdgE) {
    return Standard_False;
  }
  if (!CheckEdgeLength(E)) {
    return Standard_False;
  }

  BRepAdaptor_Curve BC(E);
  //
  // Body 
  Standard_Real f, l, tolE, tolp;
  Standard_Boolean onf, onl, inbounds;

  f = BC.FirstParameter();
  l = BC.LastParameter();
  tolE = BC.Tolerance(); 
  tolp = BC.Resolution(tolE);
  
  onf = Abs(f-par)<tolp; 
  onl = Abs(l-par)<tolp; 
  inbounds = (f<par) && (par<l);

  if ((!inbounds) && (!onf) && (!onl)) {
    return Standard_False;
  }
  
  
  gp_Pnt aP;

  BC.D1(par, aP, Tg);
  Tg.Normalize(); 
  
  return Standard_True;
}
//=======================================================================
//function : TangentOnEdge
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools2D::TangentOnEdge(const TopoDS_Edge& aE, 
						   gp_Dir& DTg)
{
  Standard_Real aT;
  gp_Vec aTg;

  DTg.SetCoord(1.,0.,0.);

  aT= BOPTools_Tools2D::IntermediatePoint (aE);
  Standard_Boolean bIsFound=BOPTools_Tools2D::TangentOnEdge(aT, aE, aTg);
  if (bIsFound) {
    gp_Dir aDTmp(aTg);
    DTg=aDTmp;
  }
  return bIsFound;
}
  
//=======================================================================
//function : TangentOnVertex
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools2D::TangentOnVertex (const TopoDS_Vertex& v, 
						      const TopoDS_Vertex& vl, 
						      const TopoDS_Edge& e,
						      gp_Vec& aVec)
// tg oriented INSIDE 1d(e)
// vl : last vertex of e
{
  Standard_Boolean ok;
  Standard_Real par;
  gp_Vec tg; 

  par = BRep_Tool::Parameter(v, e);  
  ok =BOPTools_Tools2D::TangentOnEdge (par, e, tg);
  if (!ok) {
    return ok;
  }
  if (v.IsSame(vl)) {
    tg.Reverse();
  }
  aVec=tg;

  return ok;
}

//=======================================================================
//function : EdgeBounds
//purpose  : 
//=======================================================================
  void BOPTools_Tools2D::EdgeBounds (const TopoDS_Edge& aE,
				     Standard_Real& aFirst,
				     Standard_Real& aLast)
{
  BRepAdaptor_Curve aBC(aE);
  aFirst= aBC.FirstParameter();
  aLast = aBC.LastParameter();
}

//=======================================================================
//function : IntermediatePoint
//purpose  : 
//=======================================================================
  Standard_Real BOPTools_Tools2D::IntermediatePoint (const Standard_Real aFirst,
						     const Standard_Real aLast)
{
  //define parameter division number as 10*e^(-M_PI) = 0.43213918
  const Standard_Real PAR_T = 0.43213918;
  Standard_Real aParm;
  aParm=(1.-PAR_T)*aFirst + PAR_T*aLast;
  return aParm;
}
//=======================================================================
//function : IntermediatePoint
//purpose  : 
//=======================================================================
  Standard_Real BOPTools_Tools2D::IntermediatePoint (const TopoDS_Edge& aE)
						
{
  Standard_Real aT, aT1, aT2;

  Handle(Geom_Curve)aC1=BRep_Tool::Curve(aE, aT1, aT2);
  aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
  return aT;
}

//=======================================================================
//function : CheckEdgeLength
//purpose  : 
//=======================================================================
Standard_Boolean CheckEdgeLength (const TopoDS_Edge& E)
{
  BRepAdaptor_Curve BC(E);

  TopTools_IndexedMapOfShape aM;
  TopExp::MapShapes(E, TopAbs_VERTEX, aM);
  Standard_Integer i, anExtent, aN=10;
  Standard_Real ln=0., d, t, f, l, dt; 
  anExtent=aM.Extent();

  if (anExtent!=1) 
    return Standard_True;
    
  gp_Pnt p1, p2;
  f = BC.FirstParameter();
  l = BC.LastParameter();
  dt=(l-f)/aN;
  
  BC.D0(f, p1);
  for (i=1; i<=aN; i++) {
    t=f+i*dt;
    
    if (i==aN) 
      BC.D0(l, p2);
      else 
	BC.D0(t, p2);
    
    d=p1.Distance(p2);
    ln+=d;
    p1=p2;
  }
  
  return (ln > Precision::Confusion()); 
}
