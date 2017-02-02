// Created on: 1993-06-24
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepApprox_Approx.hxx>
#include <BRepApprox_ApproxLine.hxx>
#include <BRepTools.hxx>
#include <ElSLib.hxx>
#include <gce_MakeCirc.hxx>
#include <gce_MakeLin.hxx>
#include <gce_MakeLin2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomLib_Check2dBSplineCurve.hxx>
#include <GeomLib_CheckBSplineCurve.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_ProgramError.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
#include <TopOpeBRepTool_GeomTool.hxx>

//#include <Approx.hxx>
#ifdef OCCT_DEBUG
#include <TopOpeBRepTool_KRO.hxx>
TOPKRO KRO_CURVETOOL_APPRO("approximation");
extern Standard_Boolean TopOpeBRepTool_GettraceKRO();
extern Standard_Boolean TopOpeBRepTool_GettracePCURV();
extern Standard_Boolean TopOpeBRepTool_GettraceCHKBSPL();
#endif
//#define DRAW
//#define IFV 
#define CurveImprovement
#ifdef DRAW
#include <DrawTrSurf.hxx>
#include <Geom2d_Curve.hxx>
static Standard_Integer NbCalls = 0;
#endif
//=======================================================================
//function : CurveTool
//purpose  : 
//=======================================================================

TopOpeBRepTool_CurveTool::TopOpeBRepTool_CurveTool()
{}

//=======================================================================
//function : CurveTool
//purpose  : 
//=======================================================================

TopOpeBRepTool_CurveTool::TopOpeBRepTool_CurveTool
(const TopOpeBRepTool_OutCurveType O)
{
  TopOpeBRepTool_GeomTool GT(O);
  SetGeomTool(GT);
}

//=======================================================================
//function : CurveTool
//purpose  : 
//=======================================================================

TopOpeBRepTool_CurveTool::TopOpeBRepTool_CurveTool
(const TopOpeBRepTool_GeomTool& GT)
{
  SetGeomTool(GT);
}

//=======================================================================
//function : ChangeGeomTool
//purpose  : 
//=======================================================================

TopOpeBRepTool_GeomTool& TopOpeBRepTool_CurveTool::ChangeGeomTool()
{
  return myGeomTool;
}

//=======================================================================
//function : GetGeomTool
//purpose  : 
//=======================================================================

const TopOpeBRepTool_GeomTool& TopOpeBRepTool_CurveTool::GetGeomTool()const
{
  return myGeomTool;
}

//=======================================================================
//function : SetGeomTool
//purpose  : 
//=======================================================================

void TopOpeBRepTool_CurveTool::SetGeomTool
(const TopOpeBRepTool_GeomTool& GT)
{
  myGeomTool.Define(GT);
}

//-----------------------------------------------------------------------
//function : MakePCurve
//purpose  : 
//-----------------------------------------------------------------------
Standard_EXPORT Handle(Geom2d_Curve) MakePCurve(const ProjLib_ProjectedCurve& PC)
{
  Handle(Geom2d_Curve) C2D;
  switch (PC.GetType()) {
  case GeomAbs_Line : C2D = new Geom2d_Line(PC.Line()); break;
  case GeomAbs_Circle : C2D = new Geom2d_Circle(PC.Circle()); break;
  case GeomAbs_Ellipse : C2D = new Geom2d_Ellipse(PC.Ellipse()); break;
  case GeomAbs_Parabola : C2D = new Geom2d_Parabola(PC.Parabola()); break;
  case GeomAbs_Hyperbola : C2D = new Geom2d_Hyperbola(PC.Hyperbola()); break;
  case GeomAbs_BSplineCurve : C2D = PC.BSpline(); break;
  default :
    throw Standard_NotImplemented("CurveTool::MakePCurve");
    break;
  }
  return C2D;
}

//------------------------------------------------------------------
static Standard_Boolean CheckApproxResults
  (const BRepApprox_Approx& Approx)
//------------------------------------------------------------------
{
  const AppParCurves_MultiBSpCurve& amc = Approx.Value(1); 
  Standard_Integer np = amc.NbPoles();
  Standard_Integer nc = amc.NbCurves();
  if (np < 2 || nc < 1) return Standard_False;

  // check the knots for coincidence
  const TColStd_Array1OfReal& knots = amc.Knots();
  for (Standard_Integer i = knots.Lower(); i < knots.Upper(); i++) {
    if (knots(i+1) - knots(i) <= Epsilon(knots(i))) {
      return Standard_False;
    }
  }
  return Standard_True;
} 

//------------------------------------------------------------------
static Standard_Boolean CheckPCurve
  (const Handle(Geom2d_Curve)& aPC, const TopoDS_Face& aFace)
//------------------------------------------------------------------
// check if points of the pcurve are out of the face bounds
{
  const Standard_Integer NPoints = 23;
  Standard_Real umin,umax,vmin,vmax;

  BRepTools::UVBounds(aFace, umin, umax, vmin, vmax);
  Standard_Real tolU = Max ((umax-umin)*0.01, Precision::Confusion());
  Standard_Real tolV = Max ((vmax-vmin)*0.01, Precision::Confusion());
  Standard_Real fp = aPC->FirstParameter();
  Standard_Real lp = aPC->LastParameter();
  Standard_Real step = (lp-fp)/(NPoints+1);

  // adjust domain for periodic surfaces
  TopLoc_Location aLoc;
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace, aLoc);
  if (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    aSurf = (Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurf))->BasisSurface();

  gp_Pnt2d pnt = aPC->Value((fp+lp)/2);
  Standard_Real u,v;
  pnt.Coord(u,v);

  if (aSurf->IsUPeriodic()) {
    Standard_Real aPer = aSurf->UPeriod();
    Standard_Integer nshift = (Standard_Integer) ((u-umin)/aPer);
    if (u < umin+aPer*nshift) nshift--;
    umin += aPer*nshift;
    umax += aPer*nshift;
  }
  if (aSurf->IsVPeriodic()) {
    Standard_Real aPer = aSurf->VPeriod();
    Standard_Integer nshift = (Standard_Integer) ((v-vmin)/aPer);
    if (v < vmin+aPer*nshift) nshift--;
    vmin += aPer*nshift;
    vmax += aPer*nshift;
  }

  Standard_Integer i;
  for (i=1; i <= NPoints; i++) {
    Standard_Real p = fp + i * step;
    pnt = aPC->Value(p);
    pnt.Coord(u,v);
    if (umin-u > tolU || u-umax > tolU ||
        vmin-v > tolV || v-vmax > tolV)
      return Standard_False;
  }
  return Standard_True;
} 

//------------------------------------------------------------------
static Handle(Geom_Curve) MakeCurve3DfromWLineApprox
(const BRepApprox_Approx& Approx,const Standard_Integer )
//------------------------------------------------------------------
{
  const AppParCurves_MultiBSpCurve& amc = Approx.Value(1); 
  Standard_Integer np = amc.NbPoles();
  //Standard_Integer nc = amc.NbCurves();
  TColgp_Array1OfPnt poles3d(1,np);
  Standard_Integer ic = 1;
  //for (ic=1; ic<=nc; ic++) {
     //if (ic == CI) {
      amc.Curve(ic,poles3d);
     //}
  //}

  const TColStd_Array1OfReal& knots = amc.Knots();
  const TColStd_Array1OfInteger& mults = amc.Multiplicities();
  Standard_Integer degree = amc.Degree();
  Handle(Geom_Curve) C3D = new Geom_BSplineCurve(poles3d,knots,mults,degree);
  return C3D;
} 

//------------------------------------------------------------------
static Handle(Geom2d_Curve) MakeCurve2DfromWLineApproxAndPlane
(const BRepApprox_Approx& Approx,const gp_Pln& Pl) 
//------------------------------------------------------------------
{ 
  const AppParCurves_MultiBSpCurve& amc = Approx.Value(1); 
  Standard_Integer np = amc.NbPoles();
  TColgp_Array1OfPnt2d poles2d(1,np);
  TColgp_Array1OfPnt poles3d(1,np);
  amc.Curve(1,poles3d);
  for(Standard_Integer i=1; i<=np; i++) { 
    Standard_Real U,V; ElSLib::Parameters(Pl,poles3d.Value(i),U,V);
    poles2d.SetValue(i,gp_Pnt2d(U,V));
  }
  const TColStd_Array1OfReal& knots = amc.Knots();
  const TColStd_Array1OfInteger& mults = amc.Multiplicities();
  Standard_Integer degree = amc.Degree();
  Handle(Geom2d_Curve) C2D = new Geom2d_BSplineCurve(poles2d,knots,mults,degree);
  return C2D;
}

//------------------------------------------------------------------
static Handle(Geom2d_Curve) MakeCurve2DfromWLineApprox
(const BRepApprox_Approx& Approx,const Standard_Integer CI)
//------------------------------------------------------------------
{
  const AppParCurves_MultiBSpCurve& amc = Approx.Value(1); 
  Standard_Integer np = amc.NbPoles();
  TColgp_Array1OfPnt2d poles2d(1,np);
  Standard_Integer nc = amc.NbCurves();
  for (Standard_Integer ic=1; ic<=nc; ic++) if (ic == CI) amc.Curve(ic,poles2d);
  const TColStd_Array1OfReal& knots = amc.Knots();
  const TColStd_Array1OfInteger& mults = amc.Multiplicities();
  Standard_Integer degree = amc.Degree();
  Handle(Geom2d_Curve) C2D = new Geom2d_BSplineCurve(poles2d,knots,mults,degree);
  return C2D;
}  


//=======================================================================
//function : MakeCurves
//purpose  : 
//=======================================================================

Standard_Boolean  TopOpeBRepTool_CurveTool::MakeCurves
(const Standard_Real parmin, const Standard_Real parmax,
 const Handle(Geom_Curve)& C3D, 
 const Handle(Geom2d_Curve)& PC1, const Handle(Geom2d_Curve)& PC2, 
 const TopoDS_Shape& S1, const TopoDS_Shape& S2, 
 Handle(Geom_Curve)& C3Dnew,
 Handle(Geom2d_Curve)& PC1new, Handle(Geom2d_Curve)& PC2new,
 Standard_Real& TolReached3d, Standard_Real& TolReached2d) const
{
  Standard_Boolean CompC3D = myGeomTool.CompC3D();

  //cout << "MakeCurves begin" << endl;
  if (!CompC3D) return Standard_False;

  Standard_Boolean CompPC1 = myGeomTool.CompPC1();
  Standard_Boolean CompPC2 = myGeomTool.CompPC2();
  Standard_Real tol3d,tol2d;
  myGeomTool.GetTolerances(tol3d,tol2d);
  Standard_Integer NbPntMax = myGeomTool.NbPntMax();

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceKRO()) KRO_CURVETOOL_APPRO.Start();
#endif

//----------------------------------
///*
#ifdef IFV
  char name[16];
  char *nm = &name[0];
  sprintf(name,"C3D_%d",++NbCalls);
  DrawTrSurf::Set(nm, C3D);
  sprintf(name,"PC1_%d",NbCalls);
  DrawTrSurf::Set(nm, PC1);
  sprintf(name,"PC2_%d",NbCalls);
  DrawTrSurf::Set(nm, PC2);
#endif
//*/
//---------------------------------------------

  Standard_Integer iparmin = (Standard_Integer)parmin;
  Standard_Integer iparmax = (Standard_Integer)parmax;

  Handle(Geom_BSplineCurve) HC3D (Handle(Geom_BSplineCurve)::DownCast (C3D));
  Handle(Geom2d_BSplineCurve) HPC1 (Handle(Geom2d_BSplineCurve)::DownCast (PC1));
  Handle(Geom2d_BSplineCurve) HPC2 (Handle(Geom2d_BSplineCurve)::DownCast (PC2));

//--------------------- IFV - "improving" initial curves
#ifdef CurveImprovement
  Standard_Integer nbpol = HC3D->NbPoles();
  //cout <<"nbpol = " << nbpol << endl;
  if(nbpol > 100) {
    TColgp_Array1OfPnt PolC3D(1, nbpol);
    TColgp_Array1OfPnt2d PolPC1(1, nbpol);
    TColgp_Array1OfPnt2d PolPC2(1, nbpol);
    TColStd_Array1OfBoolean IsValid(1, nbpol);
    IsValid.Init(Standard_True);
    Standard_Real tol = Max(1.e-10, 100.*tol3d*tol3d); //tol *= tol; - square distance
    Standard_Real tl2d = tol*(tol2d*tol2d)/(tol3d*tol3d);
    Standard_Real def = tol;
    Standard_Real def2d = tol2d;
    HC3D->Poles(PolC3D);
    if(CompPC1) HPC1->Poles(PolPC1);
    if(CompPC2) HPC2->Poles(PolPC2);
    
    Standard_Integer ip = 1, NbPol = 1;
    Standard_Real d, d1, d2;
    gp_Pnt P = PolC3D(ip);
    gp_Pnt2d P1, P2;
    if(CompPC1) P1 = PolPC1(ip);
    if(CompPC2) P2 = PolPC2(ip);

    for(ip = 2; ip <= nbpol; ip++) {
      if( IsValid(ip) ) {
	d = P.SquareDistance(PolC3D(ip));
	if(CompPC1) {d1 = P1.SquareDistance(PolPC1(ip));} else {d1 = 0.;}
	if(CompPC2) {d2 = P2.SquareDistance(PolPC2(ip));} else {d2 = 0.;}
	if(d > tol || d1 > tl2d || d2 > tl2d ) {
	  Standard_Real dd=0.;
	  if(ip < nbpol ) dd = P.Distance(PolC3D(ip+1));
	  if(ip < nbpol && dd < 10.*tol) {
	    gce_MakeLin mkL(P, PolC3D(ip+1));
	    if(mkL.IsDone()) {
	      gp_Lin L = mkL.Value();
	      d = L.SquareDistance(PolC3D(ip));
	      if(CompPC1) {
		gp_Lin2d L1 = gce_MakeLin2d(P1, PolPC1(ip+1));
		d1 = L1.SquareDistance(PolPC1(ip));
	      }
	      else d1 = 0.;
	      if(CompPC2) {
		gp_Lin2d L2 = gce_MakeLin2d(P2, PolPC2(ip+1));
		d2 = L2.SquareDistance(PolPC2(ip));
	      }
	      else d2 = 0.;
	    
	      if(d > def || d1 > def2d || d2 > def2d ) {
		NbPol++;
		P = PolC3D(ip);
		if(CompPC1) P1 = PolPC1(ip);
		if(CompPC2) P2 = PolPC2(ip);
	      }
	      else {
		IsValid(ip) = Standard_False;
	      }
	    }
	    else {
	      IsValid(ip+1) = Standard_False;
	      NbPol++;
	      P = PolC3D(ip);
	      if(CompPC1) P1 = PolPC1(ip);
	      if(CompPC2) P2 = PolPC2(ip);
	    }
	  }
	  else {
	    NbPol++;
	    P = PolC3D(ip);
	    if(CompPC1) P1 = PolPC1(ip);
	    if(CompPC2) P2 = PolPC2(ip);
	  }
	}
	else {
	  IsValid(ip) = Standard_False;
	}
      }
    }
 
    if(NbPol < 2) {IsValid(nbpol) = Standard_True; NbPol++;}
    
    if(NbPol < nbpol) {
#ifdef IFV
      cout << "NbPol < nbpol : " << NbPol << " " <<  nbpol << endl;
#endif
      TColgp_Array1OfPnt Polc3d(1, NbPol);
      TColgp_Array1OfPnt2d Polpc1(1, NbPol);
      TColgp_Array1OfPnt2d Polpc2(1, NbPol);
      TColStd_Array1OfReal knots(1,NbPol);
      TColStd_Array1OfInteger mults(1,NbPol);
      mults.Init(1); mults(1) = 2; mults(NbPol) = 2;
      Standard_Integer count = 0;
      for(ip = 1; ip <= nbpol; ip++) {
	if(IsValid(ip)) {
	  count++;
	  Polc3d(count) = PolC3D(ip);
	  if(CompPC1) Polpc1(count) = PolPC1(ip);
	  if(CompPC2) Polpc2(count) = PolPC2(ip);
	  knots(count) = count;
	}
      }

      Polc3d(NbPol) = PolC3D(nbpol);
      if(CompPC1) Polpc1(NbPol) = PolPC1(nbpol);
      if(CompPC2) Polpc2(NbPol) = PolPC2(nbpol);
      
      const_cast<Handle(Geom_Curve)&>(C3D) = new Geom_BSplineCurve(Polc3d, knots, mults, 1);
      if(CompPC1) const_cast<Handle(Geom2d_Curve)&>(PC1) = new Geom2d_BSplineCurve(Polpc1, knots, mults, 1);
      if(CompPC2) const_cast<Handle(Geom2d_Curve)&>(PC2) = new Geom2d_BSplineCurve(Polpc2, knots, mults, 1);
      iparmax = NbPol;

#ifdef IFV
      sprintf(name,"C3Dmod_%d",NbCalls);
      nm = &name[0];
      DrawTrSurf::Set(nm, C3D);
      sprintf(name,"PC1mod_%d",NbCalls);
      nm = &name[0];
      DrawTrSurf::Set(nm, PC1);
      sprintf(name,"PC2mod_%d",NbCalls);
      nm = &name[0];
      DrawTrSurf::Set(nm, PC2);
#endif

    }
  }
//--------------- IFV - end "improving"
#endif


  BRepApprox_Approx Approx;

  Standard_Integer degmin = 4;
  Standard_Integer degmax = 8;
  Approx_ParametrizationType parametrization = Approx_ChordLength;

  Standard_Integer npol = HC3D->NbPoles();
  TColgp_Array1OfPnt Polc3d(1, npol);
  TColStd_Array1OfReal par(1, npol);
  HC3D->Poles(Polc3d);
  gp_Pnt P = Polc3d(1);

  Standard_Boolean IsBad = Standard_False;
  Standard_Integer ip;
  Standard_Real ksi = 0., kc, kcprev = 0.;
  Standard_Real dist;
  par(1) = 0.;
  for(ip = 2; ip <= npol; ip++) {
    dist = P.Distance(Polc3d(ip));

    if(dist < Precision::Confusion()) {
      IsBad = Standard_True;
      break;
    }

    par(ip) = par(ip-1) + dist; 
    P = Polc3d(ip);
  }

  if(!IsBad) {

    TColStd_Array1OfReal Curvature(1, npol);

    if(npol > 3) {
      Standard_Integer ic = 1;
      for(ip = 2; ip <= npol-1; ip += npol-3) {
	gp_Vec v1(Polc3d(ip-1),Polc3d(ip));
	gp_Vec v2(Polc3d(ip),Polc3d(ip+1));
	if(!v1.IsParallel(v2, 1.e-4)) {
	  gce_MakeCirc mc(Polc3d(ip-1),Polc3d(ip),Polc3d(ip+1));
	  gp_Circ cir = mc.Value();
	  kc = 1./cir.Radius();
	}
	else kc = 0.;
    
	Curvature(ic) = kc;
	ic = npol;
      }
    }
    else if(npol == 3) {
      ip = 2;
      gp_Vec v1(Polc3d(ip-1),Polc3d(ip));
      gp_Vec v2(Polc3d(ip),Polc3d(ip+1));
      if(!v1.IsParallel(v2, 1.e-4)) {
	gce_MakeCirc mc(Polc3d(ip-1),Polc3d(ip),Polc3d(ip+1));
	gp_Circ cir = mc.Value();
	kc = 1./cir.Radius();
      }
      else kc = 0.;
      Curvature(1) = Curvature(npol) = kc;
    }
    else {
      Curvature(1) = Curvature(npol) = 0.;
    }
    
    ip = 1;
    Standard_Real dt = par(ip+1) - par(ip);
    Standard_Real dx = (Polc3d(ip+1).X() - Polc3d(ip).X())/dt,
                  dy = (Polc3d(ip+1).Y() - Polc3d(ip).Y())/dt,
                  dz = (Polc3d(ip+1).Z() - Polc3d(ip).Z())/dt;
    Standard_Real dx1, dy1, dz1, d2x, d2y, d2z, d2t;

    for(ip = 2; ip <= npol-1; ip++) {
      dt = par(ip+1) - par(ip);
      dx1 = (Polc3d(ip+1).X() - Polc3d(ip).X())/dt;
      dy1 = (Polc3d(ip+1).Y() - Polc3d(ip).Y())/dt,
      dz1 = (Polc3d(ip+1).Z() - Polc3d(ip).Z())/dt;
      d2t = 2./(par(ip+1) - par(ip-1));
      d2x = d2t*(dx1 - dx);
      d2y = d2t*(dy1 - dy);
      d2z = d2t*(dz1 - dz);
      Curvature(ip) = Sqrt(d2x*d2x + d2y*d2y + d2z*d2z);
      dx = dx1; dy = dy1; dz = dz1;
    }

    Standard_Real crit = 1000.; // empirical criterion !!!

    dt = par(2) - par(1);
    kcprev = (Curvature(2) - Curvature(1))/dt; 
    for(ip = 2; ip <= npol-1; ip++) {
      dt = par(ip+1) - par(ip);
      kc = (Curvature(ip+1) - Curvature(ip))/dt;
      ksi = ksi + Abs(kc - kcprev);
      if(ksi > crit) {IsBad = Standard_True;break;}
      kc = kcprev; 
    }

  }
  //cout << NbCalls << " ksi = " << ksi << endl;
  //cout << "IsBad = " << IsBad << endl;

  if(IsBad){
    Standard_Real tt = Min(10.*tol3d,Precision::Approximation());
    tol2d = tt * tol2d / tol3d;
    tol3d = tt;
    NbPntMax = 40;
    degmax = 4;
    parametrization = Approx_Centripetal;
  }
 
  Standard_Integer nitmax = 0; // use projection only
  Standard_Boolean withtangency = Standard_True;
  
  Standard_Boolean compminmaxUV = Standard_True;
  BRepAdaptor_Surface BAS1(TopoDS::Face(S1),compminmaxUV);
  BRepAdaptor_Surface BAS2(TopoDS::Face(S2),compminmaxUV);


  Handle(BRepApprox_ApproxLine) AL;
  AL = new BRepApprox_ApproxLine(HC3D,HPC1,HPC2);

    Approx.SetParameters(tol3d,tol2d,degmin,degmax,nitmax,NbPntMax,withtangency,
			 parametrization);

    if     (CompC3D && CompPC1 && BAS1.GetType() == GeomAbs_Plane) { 
      //-- The curve X,Y,Z and U2,V2 is approximated
      Approx.Perform(BAS1,BAS2,AL,CompC3D,Standard_False,CompPC2,iparmin,iparmax);
    }
    
    else if(CompC3D && CompPC2 && BAS2.GetType() == GeomAbs_Plane) {
      //-- The curve X,Y,Z and U1,V1 is approximated
      Approx.Perform(BAS1,BAS2,AL,CompC3D,CompPC1,Standard_False,iparmin,iparmax);
    }

    else { 
      Approx.Perform(BAS1,BAS2,AL,CompC3D,CompPC1,CompPC2,iparmin,iparmax);
    }

  // MSV Nov 9, 2001: do not raise exception in the case of failure,
  //                  but return status

  Standard_Boolean done = Approx.IsDone();
  done = done && CheckApproxResults(Approx);

  if (done) {
    if     (CompC3D && CompPC1 && BAS1.GetType() == GeomAbs_Plane) { 
      C3Dnew = ::MakeCurve3DfromWLineApprox(Approx,1);
      PC1new = ::MakeCurve2DfromWLineApproxAndPlane(Approx,BAS1.Plane());
      if (CompPC2) PC2new = ::MakeCurve2DfromWLineApprox(Approx,2);
    }
    else if(CompC3D && CompPC2 && BAS2.GetType() == GeomAbs_Plane) {
      C3Dnew = ::MakeCurve3DfromWLineApprox(Approx,1);
      if (CompPC1) PC1new = ::MakeCurve2DfromWLineApprox(Approx,2);
      PC2new = ::MakeCurve2DfromWLineApproxAndPlane(Approx,BAS2.Plane());
    }
    else { 
      if (CompC3D) C3Dnew = ::MakeCurve3DfromWLineApprox(Approx,1);
      if (CompPC1) PC1new = ::MakeCurve2DfromWLineApprox(Approx,2);
      if (CompPC2) {
        Standard_Integer i32 = (CompPC1) ? 3 : 2;
        PC2new = ::MakeCurve2DfromWLineApprox(Approx,i32);
      }
    }

    // check the pcurves relatively the faces bounds
    if (CompPC1)
      done = done && CheckPCurve(PC1new,TopoDS::Face(S1));
    if (CompPC2)
      done = done && CheckPCurve(PC2new,TopoDS::Face(S2));
  }

  if (!done) {
    if (CompC3D) C3Dnew.Nullify();
    if (CompPC1) PC1new.Nullify();
    if (CompPC2) PC2new.Nullify();
    return Standard_False;
  }

#ifdef IFV
    sprintf(name,"C3Dnew_%d", NbCalls);
    nm = &name[0];
    DrawTrSurf::Set(nm, C3Dnew);
    if (CompPC1) {
      sprintf(name,"PC1new_%d", NbCalls);
      nm = &name[0];
      DrawTrSurf::Set(nm, PC1new);
    }
    if (CompPC2) {
      sprintf(name,"PC2new_%d", NbCalls);
      nm = &name[0];
      DrawTrSurf::Set(nm, PC2new);
    }

#endif

  TolReached3d = Approx.TolReached3d();
  TolReached2d = Approx.TolReached2d();
#ifdef IFV
  cout << NbCalls << " : Tol = " << TolReached3d << " " << TolReached2d << endl;
#endif

  Standard_Boolean bf, bl;

  Handle(Geom_BSplineCurve) Curve (Handle(Geom_BSplineCurve)::DownCast(C3Dnew));
  if(!Curve.IsNull()) {
    GeomLib_CheckBSplineCurve cbsc(Curve, 1.e-7, 0.1);
    cbsc.NeedTangentFix(bf, bl);

#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceCHKBSPL()) {
      if(bf || bl) {
	cout<<"Problem orientation GeomLib_CheckBSplineCurve : First = "<<bf;
	cout<<" Last = "<<bl<<endl;
      }
    }
#endif
    cbsc.FixTangent(bf, bl);
  }
  
  Handle(Geom2d_BSplineCurve) Curve2df (Handle(Geom2d_BSplineCurve)::DownCast(PC1new));
  if(!Curve2df.IsNull()) {
    GeomLib_Check2dBSplineCurve cbsc2df(Curve2df, 1.e-7, 0.1);
    cbsc2df.NeedTangentFix(bf, bl);
#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceCHKBSPL()) {
      if(bf || bl) {
	cout<<"Problem orientation GeomLib_CheckBSplineCurve : First = "<<bf;
	cout<<" Last = "<<bl<<endl;
      }
    }
#endif
    cbsc2df.FixTangent(bf, bl);
  }
  
  Handle(Geom2d_BSplineCurve) Curve2ds (Handle(Geom2d_BSplineCurve)::DownCast(PC2new));
  if(!Curve2ds.IsNull()) {
    GeomLib_Check2dBSplineCurve cbsc2ds(Curve2ds, 1.e-7, 0.1);
    cbsc2ds.NeedTangentFix(bf, bl);
#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceCHKBSPL()) {
      if(bf || bl) {
	cout<<"Problem orientation GeomLib_CheckBSplineCurve : First = "<<bf;
	cout<<" Last = "<<bl<<endl;
      }
    }
#endif
    cbsc2ds.FixTangent(bf, bl);
  }
  
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceKRO()) KRO_CURVETOOL_APPRO.Stop();
#endif
//  cout << "MakeCurves end" << endl;

  return Standard_True;
}


//=======================================================================
//function : MakeBSpline1fromPnt
//purpose  : 
//=======================================================================

Handle(Geom_Curve) TopOpeBRepTool_CurveTool::MakeBSpline1fromPnt
(const TColgp_Array1OfPnt& Points)
{
  Standard_Integer Degree = 1;
  
  Standard_Integer i,nbpoints = Points.Length();
  Standard_Integer nbknots = nbpoints - Degree +1;
  
  //  First compute the parameters
  //  Standard_Real length = 0.;
  //  TColStd_Array1OfReal parameters(1,nbpoints);
  //  for (i = 1; i < nbpoints; i++) {
  //    parameters(i) = length;
  //    Standard_Real dist = Points(i).Distance(Points(i+1));
  //    length += dist;
  //  }
  //  parameters(nbpoints) = length;
  
  // knots and multiplicities
  TColStd_Array1OfReal knots(1,nbknots);
  TColStd_Array1OfInteger mults(1,nbknots);
  mults.Init(1);
  mults(1) = mults(nbknots) = Degree + 1;
  
  //  knots(1) = 0;
  //  for (i=2;i<nbknots;i++) knots(i) = (parameters(i) + parameters(i+1)) /2.;
  //  knots(nbknots) = length;
  
  // take point index as parameter : JYL 01/AUG/94
  for (i = 1; i <= nbknots; i++) knots(i) = (Standard_Real)i;
  Handle(Geom_Curve) C = new Geom_BSplineCurve(Points,knots,mults,Degree);
  return C;
}


//=======================================================================
//function : MakeBSpline1fromPnt2d
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) TopOpeBRepTool_CurveTool::MakeBSpline1fromPnt2d
(const TColgp_Array1OfPnt2d& Points)
{
  Standard_Integer Degree = 1;
  
  Standard_Integer i,nbpoints = Points.Length();
  Standard_Integer nbknots = nbpoints - Degree +1;
  
  //  First compute the parameters
  //  Standard_Real length = 0;
  //  TColStd_Array1OfReal parameters(1,nbpoints);
  //  for (i = 1; i < nbpoints; i++) {
  //    parameters(i) = length;
  //    Standard_Real dist = Points(i).Distance(Points(i+1));
  //    length += dist;
  //  }
  //  parameters(nbpoints) = length;
  
  // knots and multiplicities
  TColStd_Array1OfReal knots(1,nbknots);
  TColStd_Array1OfInteger mults(1,nbknots);
  mults.Init(1);
  mults(1) = mults(nbknots) = Degree + 1;
  
  //  knots(1) = 0;
  //  for (i=2;i<nbknots;i++) knots(i) = (parameters(i) + parameters(i+1)) /2.;
  //  knots(nbknots) = length;
  
  // take point index as parameter : JYL 01/AUG/94
  for (i = 1; i <= nbknots; i++) knots(i) = (Standard_Real)i;
  Handle(Geom2d_Curve) C = new Geom2d_BSplineCurve(Points,knots,mults,Degree);
  return C;
}

//=======================================================================
//function : IsProjectable
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CurveTool::IsProjectable
(const TopoDS_Shape& S, const Handle(Geom_Curve)& C3D)
{
  const TopoDS_Face& F = TopoDS::Face(S);
  Standard_Boolean compminmaxUV = Standard_False;
  BRepAdaptor_Surface BAS(F,compminmaxUV);
  GeomAbs_SurfaceType suty = BAS.GetType();
  GeomAdaptor_Curve GAC(C3D);
  GeomAbs_CurveType cuty = GAC.GetType();

  // --------
  // avoid projection of 3d curve on surface in case
  // of a quadric (ellipse,hyperbola,parabola) on a cone.
  // Projection fails when the curve in not fully inside the UV domain 
  // of the cone : only part of 2d curve is built.
  // NYI : projection of quadric on cone (crossing cone domain)
  // --------
  
  Standard_Boolean projectable = Standard_True;
  if ( suty == GeomAbs_Cone ) {
    if( (cuty == GeomAbs_Ellipse) || 
       ( cuty == GeomAbs_Hyperbola) || 
       ( cuty == GeomAbs_Parabola) ) {
      projectable = Standard_False;
    }
  }
  else if ( suty == GeomAbs_Cylinder ) {
    if (cuty == GeomAbs_Ellipse) {
      projectable = Standard_False;
    }
  }
  else if ( suty == GeomAbs_Sphere ) {
    if (cuty == GeomAbs_Circle) {
      projectable = Standard_False;
    }
  }
  else if ( suty == GeomAbs_Torus ) {
    if (cuty == GeomAbs_Circle) {
      projectable = Standard_False;
    }
  }
  
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettracePCURV()) {
    cout<<"--- IsProjectable : "; 
    if (projectable) cout<<"projectable"<<endl;
    else cout<<"NOT projectable"<<endl;
  }
#endif
  
  return projectable;
}


//=======================================================================
//function : MakePCurveOnFace
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) TopOpeBRepTool_CurveTool::MakePCurveOnFace
(const TopoDS_Shape& S,
 const Handle(Geom_Curve)& C3D,
 Standard_Real& TolReached2d,
 const Standard_Real first, const Standard_Real last)
 
{
  Standard_Boolean trim = Standard_False;
  if (first < last)
    trim = Standard_True;

  const TopoDS_Face& F = TopoDS::Face(S);
  Standard_Boolean compminmaxUV = Standard_False;
  BRepAdaptor_Surface BAS(F,compminmaxUV);
  GeomAdaptor_Curve GAC;
  if (trim) GAC.Load(C3D,first,last);
  else      GAC.Load(C3D);
  Handle(BRepAdaptor_HSurface) BAHS = new BRepAdaptor_HSurface(BAS);
  Handle(GeomAdaptor_HCurve) BAHC = new GeomAdaptor_HCurve(GAC);
  ProjLib_ProjectedCurve projcurv(BAHS,BAHC);
  Handle(Geom2d_Curve) C2D = ::MakePCurve(projcurv);
  TolReached2d = projcurv.GetTolerance();

  Standard_Real UMin, UMax, VMin, VMax;
  BRepTools::UVBounds(F,UMin, UMax, VMin, VMax);

  Standard_Real f = GAC.FirstParameter();
  Standard_Real l = GAC.LastParameter();
  Standard_Real t =(f+l)*.5;
  gp_Pnt2d pC2D; C2D->D0(t,pC2D);
  Standard_Real u2 = pC2D.X();
  Standard_Real v2 = pC2D.Y();

  if (BAS.GetType() == GeomAbs_Sphere) {
    // MSV: consider quasiperiodic shift of pcurve
    Standard_Real VFirst = BAS.FirstVParameter();
    Standard_Real VLast = BAS.LastVParameter();
    Standard_Boolean mincond = v2 < VFirst;
    Standard_Boolean maxcond = v2 > VLast;
    if (mincond || maxcond) {
      Handle(Geom2d_Curve) PCT = Handle(Geom2d_Curve)::DownCast(C2D->Copy());
      // make mirror relative to the isoline of apex -PI/2 or PI/2
      gp_Trsf2d aTrsf;
      gp_Pnt2d po(0,-M_PI/2);
      if (maxcond) po.SetY(M_PI/2);
      aTrsf.SetMirror(gp_Ax2d(po, gp_Dir2d(1,0)));
      PCT->Transform(aTrsf);
      // add translation along U direction on PI
      gp_Vec2d vec(M_PI,0);
      Standard_Real UFirst = BAS.FirstUParameter();
      if (u2-UFirst-M_PI > -1e-7) vec.Reverse();
      PCT->Translate(vec);
      C2D = PCT;
      // recompute the test point
      C2D->D0(t,pC2D);
      u2 = pC2D.X();
      v2 = pC2D.Y();
    }
  }

  Standard_Real du = 0.;
  if (BAHS->IsUPeriodic()) {
    //modified by NIZHNY-MZV  Thu Mar 30 10:03:15 2000
    Standard_Boolean mincond = (UMin - u2 > 1e-7) ? Standard_True : Standard_False;
    Standard_Boolean maxcond = (u2 - UMax > 1e-7) ? Standard_True : Standard_False;
    Standard_Boolean decalu = mincond || maxcond;
    if (decalu) du = ( mincond ) ? BAHS->UPeriod() : -BAHS->UPeriod();
    //Standard_Boolean decalu = ( u2 < UMin || u2 > UMax);
    //if (decalu) du = ( u2 < UMin ) ? BAHS->UPeriod() : -BAHS->UPeriod();
  }
  Standard_Real dv = 0.;
  if (BAHS->IsVPeriodic()) {
    //modified by NIZHNY-MZV  Thu Mar 30 10:06:24 2000
    Standard_Boolean mincond = (VMin - v2 > 1e-7) ? Standard_True : Standard_False;
    Standard_Boolean maxcond = (v2 - VMax > 1e-7) ? Standard_True : Standard_False;
    Standard_Boolean decalv = mincond || maxcond;
    if (decalv) dv = ( mincond ) ? BAHS->VPeriod() : -BAHS->VPeriod();
    //Standard_Boolean decalv = ( v2 < VMin || v2 > VMax);
    //if (decalv) dv = ( v2 < VMin ) ? BAHS->VPeriod() : -BAHS->VPeriod();
  }
  
  if ( du != 0. || dv != 0.) {
    Handle(Geom2d_Curve) PCT = Handle(Geom2d_Curve)::DownCast(C2D->Copy());
    PCT->Translate(gp_Vec2d(du,dv));
    C2D = PCT;
  }

  return C2D;
}
