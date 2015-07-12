// Created on: 1994-02-03
// Created by: Isabelle GRIGNON
// Copyright (c) 1994-1999 Matra Datavision
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
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ChFiKPart_ComputeData.hxx>
#include <ChFiKPart_ComputeData_Fcts.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <Precision.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>

//=======================================================================
//function : MakeFillet
//purpose  : case cylinder/plane or plane/cylinder.
//=======================================================================
Standard_Boolean ChFiKPart_MakeFillet(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& Pln, 
				      const gp_Cylinder& Cyl, 
				      const Standard_Real fu,
				      const Standard_Real lu,
				      const TopAbs_Orientation Or1,
				      const TopAbs_Orientation Or2,
				      const Standard_Real Radius, 
				      const gp_Lin& Spine, 
				      const Standard_Real First, 
				      const TopAbs_Orientation Ofpl,
				      const Standard_Boolean plandab)
{
  //calculate the cylinder fillet.

  //plane deviated from radius
  gp_Ax3 AxPln  = Pln.Position();
  gp_Dir NorPln = AxPln.XDirection().Crossed(AxPln.YDirection());
  gp_Dir NorF(NorPln);
  gp_Ax3 AxCyl = Cyl.Position();

  if (Or1 == TopAbs_REVERSED) { NorF.Reverse(); } 
  gp_Pln PlanOffset = Pln.Translated(Radius*gp_Vec(NorF));

  // Parallel cylinder
  Standard_Real    ROff = Cyl.Radius();

  if ((Or2 == TopAbs_FORWARD  &&  Cyl.Direct()) ||
      (Or2 == TopAbs_REVERSED && !Cyl.Direct()))
      ROff+=Radius;
  else if (Radius < ROff)  {
    ROff-=Radius;
  }
  else {
#ifdef OCCT_DEBUG
    cout<<"the fillet does not pass"<<endl; 
#endif
    return Standard_False;
  }
  // intersection of the parallel plane and of the parallel cylinder.
  gp_Cylinder CylOffset(Cyl.Position(),ROff);
  IntAna_QuadQuadGeo LInt(PlanOffset,CylOffset,
			  Precision::Angular(),Precision::Confusion());
  gp_Pnt OrSpine = ElCLib::Value(First,Spine);
  gp_Pnt OrFillet;
  gp_Dir DirFillet;
  if (LInt.IsDone()) {
    DirFillet = LInt.Line(1).Direction();
    gp_Pnt P1 = ElCLib::Value(ElCLib::Parameter
			      (LInt.Line(1),OrSpine),LInt.Line(1));
    if(LInt.NbSolutions() == 2){
      gp_Pnt P2 = ElCLib::Value(ElCLib::Parameter
				(LInt.Line(2),OrSpine),LInt.Line(2));
      if (P1.SquareDistance(OrSpine) < P2.SquareDistance(OrSpine)) {
	OrFillet = P1;
      }
      else { OrFillet = P2;}
    }
    else { OrFillet = P1;}
  }
  else {return Standard_False;}

  // Construction fillet
  if (DirFillet.Dot(Spine.Direction()) < 0.) {DirFillet.Reverse();}

  Standard_Real UOnCyl,VOnCyl,UOnPln,VOnPln;
  ElSLib::Parameters(Cyl,OrFillet,UOnCyl,VOnCyl);
  Standard_Real tesp = Precision::Confusion();
  if(UOnCyl < fu - tesp || UOnCyl > lu + tesp) 
    UOnCyl = ElCLib::InPeriod(UOnCyl,fu,fu+2*M_PI);
  ElSLib::Parameters(Pln,OrFillet,UOnPln,VOnPln);

  gp_Vec XDir,OtherDir;
  XDir     = NorF.Reversed();
  OtherDir = gp_Dir(gp_Vec(OrFillet,ElSLib::Value(UOnCyl,VOnCyl,Cyl)));

  if (!plandab) {
    gp_Vec tmp = XDir;
    XDir     = OtherDir;
    OtherDir = tmp;
  }
  gp_Ax3 AxFil (OrFillet,DirFillet,XDir);
  // construction YDir to go from face1 to face2.
  if ((XDir^OtherDir).Dot(DirFillet) < 0.)
    AxFil.YReverse();

  Handle(Geom_CylindricalSurface) 
    Fillet = new Geom_CylindricalSurface(AxFil,Radius);
  Data->ChangeSurf(ChFiKPart_IndexSurfaceInDS(Fillet,DStr));

  // FaceInterferences are loaded with pcurves and curves 3D.
  // edge plane-Fillet
  gp_Pnt2d PPln2d(UOnPln,VOnPln);
  gp_Dir2d VPln2d(DirFillet.Dot(AxPln.XDirection()),
		  DirFillet.Dot(AxPln.YDirection()));
  gp_Lin2d Lin2dPln(PPln2d,VPln2d);
  gp_Pnt   POnPln = ElSLib::Value(UOnPln,VOnPln,Pln);
  gp_Lin   C3d(POnPln,DirFillet);
  Standard_Real UOnFillet,V;
  ElSLib::CylinderParameters(AxFil,Radius,POnPln,UOnFillet,V);
  if(UOnFillet > M_PI) UOnFillet = 0.;
  gp_Lin2d LOnFillet(gp_Pnt2d(UOnFillet,V),gp::DY2d());
  Handle(Geom_Line)   L3d  = new Geom_Line  (C3d);
  Handle(Geom2d_Line) LFac = new Geom2d_Line(Lin2dPln);
  Handle(Geom2d_Line) LFil = new Geom2d_Line(LOnFillet);
  gp_Pnt P;
  gp_Vec deru,derv;
  ElSLib::CylinderD1(UOnFillet,V,AxFil,Radius,P,deru,derv);
  gp_Dir NorFil(deru.Crossed(derv));
  Standard_Boolean toreverse = ( NorFil.Dot(NorPln) <= 0. );
  // It is checked if the orientation of the cylinder is the same as of the plane.
  if (toreverse) {Data->ChangeOrientation() = TopAbs::Reverse(Ofpl);}
  else           {Data->ChangeOrientation() = Ofpl;}

  TopAbs_Orientation trans; 
  if ((toreverse && plandab) || (!toreverse && !plandab) ){ 
    trans = TopAbs_REVERSED; 
  }
  else { 
    trans = TopAbs_FORWARD; 
  }
  if (plandab) 
    Data->ChangeInterferenceOnS1().
    SetInterference(ChFiKPart_IndexCurveInDS(L3d,DStr),
		    trans,LFac,LFil);
  else    
    Data->ChangeInterferenceOnS2().
    SetInterference(ChFiKPart_IndexCurveInDS(L3d,DStr),
		    trans,LFac,LFil);

  // edge cylinder-Fillet.
  gp_Pnt2d PCyl2d(UOnCyl,VOnCyl);
  gp_Dir2d DPC = gp::DY2d();
  if (DirFillet.Dot(AxCyl.Direction()) < 0.) DPC.Reverse();
  gp_Lin2d Lin2dCyl(PCyl2d,DPC);
  gp_Pnt POnCyl = ElSLib::Value(UOnCyl,VOnCyl,Cyl);
  C3d = gp_Lin(POnCyl,DirFillet);
  ElSLib::CylinderParameters(AxFil,Radius,POnCyl,UOnFillet,V);
  if(UOnFillet > M_PI) UOnFillet = 0.;
  LOnFillet = gp_Lin2d(gp_Pnt2d(UOnFillet,V),gp::DY2d());
  L3d  = new Geom_Line  (C3d);
  LFac = new Geom2d_Line(Lin2dCyl);
  LFil = new Geom2d_Line(LOnFillet);  
  
  ElSLib::CylinderD1(UOnFillet,V,AxFil,Radius,P,deru,derv);
  NorFil= gp_Dir(deru.Crossed(derv));
  ElSLib::CylinderD1(UOnCyl,VOnCyl,AxCyl,Cyl.Radius(),P,deru,derv);
  gp_Dir NorCyl(deru.Crossed(derv));

  toreverse = ( NorFil.Dot(NorCyl) <= 0. );
  if ((toreverse && plandab) || (!toreverse && !plandab) ){ 
    trans = TopAbs_FORWARD; 
  }
  else { 
    trans = TopAbs_REVERSED; 
  }
  if (plandab) 
    Data->ChangeInterferenceOnS2().
    SetInterference(ChFiKPart_IndexCurveInDS(L3d,DStr),
		    trans,LFac,LFil);
  else    
    Data->ChangeInterferenceOnS1().
    SetInterference(ChFiKPart_IndexCurveInDS(L3d,DStr),
		    trans,LFac,LFil);
  return Standard_True;
}

//=======================================================================
//function : MakeFillet
//purpose  : case cylinder/plane or plane/cylinder.
//=======================================================================

Standard_Boolean ChFiKPart_MakeFillet(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& Pln, 
				      const gp_Cylinder& Cyl, 
				      const Standard_Real fu,
				      const Standard_Real lu,
				      const TopAbs_Orientation Or1,
				      const TopAbs_Orientation Or2,
				      const Standard_Real Radius, 
				      const gp_Circ& Spine, 
				      const Standard_Real First, 
				      const TopAbs_Orientation Ofpl,
				      const Standard_Boolean plandab)
{

//calculation of the fillet (torus or sphere).
  Standard_Boolean c1sphere = Standard_False;
  gp_Ax3 PosPl = Pln.Position();
  gp_Dir Dpnat = PosPl.XDirection().Crossed(PosPl.YDirection());
  gp_Dir Dp = Dpnat;
  gp_Dir Df = Dp;
  if (Or1 == TopAbs_REVERSED) { Dp.Reverse(); }
  if (Ofpl == TopAbs_REVERSED) { Df.Reverse(); }

  gp_Pnt Or = Cyl.Location();
  Standard_Real u,v;
  ElSLib::PlaneParameters(PosPl,Or,u,v);
  gp_Pnt2d c2dPln(u,v);
  ElSLib::PlaneD0(u,v,PosPl,Or);
  gp_Pnt cPln = Or;
  Or.SetCoord(Or.X()+Radius*Dp.X(),
	      Or.Y()+Radius*Dp.Y(),
	      Or.Z()+Radius*Dp.Z());
  gp_Pnt PtSp;
  gp_Vec DSp;
 // Modification for the PtSp found at the wrong side of the sewing edge.
  gp_Pnt PtSp2;
  gp_Vec DSp2;
  Standard_Real acote = 1e-7;
  ElCLib::D1(First,Spine,PtSp,DSp);
  ElSLib::Parameters(Cyl,PtSp,u,v);
  if ((Abs(u)<acote) || (Abs(u-(2*M_PI))<acote)){
    ElCLib::D1(First+0.2,Spine,PtSp2,DSp2);
    Standard_Real u2,v2;
    ElSLib::Parameters(Cyl,PtSp2,u2,v2);
    if (Abs(u2-u)>M_PI){
      u = (2*M_PI)-u;
      PtSp = ElSLib::Value(u,v,Cyl);
      Standard_Real PR;
      PR = ElCLib::Parameter(Spine,PtSp);
      ElCLib::D1(PR,Spine,PtSp2,DSp);
    }
  }
  // end of modif
  gp_Dir Dx(gp_Vec(Or,PtSp));
  Dx = Dp.Crossed(Dx.Crossed(Dp));
  gp_Dir Dy(DSp);
  gp_Pnt PtCyl;
  gp_Vec Vu,Vv;
  ElSLib::D1(u,v,Cyl,PtCyl,Vu,Vv);
  gp_Dir Dc(Vu.Crossed(Vv));
  if (Or2 == TopAbs_REVERSED) { Dc.Reverse(); }
  gp_Dir Dz = Dp;
  Standard_Real Rad,cylrad = Cyl.Radius();
  Standard_Boolean dedans = (Dx.Dot(Dc) <= 0.);
  if(dedans){ 
    if (!plandab){ Dz.Reverse(); } 
    Rad = cylrad - Radius;
    if(Abs(Rad) <= Precision::Confusion()){ c1sphere = Standard_True; }
    else if(Rad < 0){ 
#ifdef OCCT_DEBUG
      cout<<"the fillet can't pass"<<endl; 
#endif
      return Standard_False;
    }
  }
  else { 
    if (plandab){ Dz.Reverse(); } 
    Rad = cylrad + Radius; 
  }
  gp_Ax3 FilAx3(Or,Dz,Dx);
  if (FilAx3.YDirection().Dot(Dy) <= 0.){ FilAx3.YReverse(); }

  if(c1sphere) {
    Handle(Geom_SphericalSurface) 
      gsph = new Geom_SphericalSurface(FilAx3,Radius);
    Data->ChangeSurf(ChFiKPart_IndexSurfaceInDS(gsph,DStr));
  }
  else{
    Handle(Geom_ToroidalSurface) 
      gtor = new Geom_ToroidalSurface(FilAx3,Rad,Radius);
    Data->ChangeSurf(ChFiKPart_IndexSurfaceInDS(gtor,DStr));
  }
  
  // It is checked if the orientation of the fillet is the same as of faces.
  gp_Pnt P,PP;
  gp_Vec deru,derv;
  P.SetCoord(cPln.X()+Rad*Dx.X(),
	     cPln.Y()+Rad*Dx.Y(),
	     cPln.Z()+Rad*Dx.Z());
  u = 0.;
  if ((dedans && plandab) || (!dedans && !plandab)){ 
    if (c1sphere) { v = - M_PI / 2; }
    else { v = 3 * M_PI / 2; }
  }
  else { v = M_PI / 2; }
  gp_Dir norFil;
  if(c1sphere){
    ElSLib::SphereD1(u,v,FilAx3,cylrad,PP,deru,derv);
    norFil = FilAx3.XDirection().Crossed(FilAx3.YDirection());
    if(v < 0.) norFil.Reverse();
  }  
  else{
    ElSLib::TorusD1(u,v,FilAx3,Rad,Radius,PP,deru,derv);
    norFil = gp_Dir(deru.Crossed(derv));
  }  
  gp_Pnt2d p2dFil(0.,v);
  Standard_Boolean toreverse = ( norFil.Dot(Df) <= 0. );
  if (toreverse) { Data->ChangeOrientation() = TopAbs_REVERSED; }
  else { Data->ChangeOrientation() = TopAbs_FORWARD; }

  // FaceInterferences are loaded with pcurves and curves 3d.

  // The plane face.
  Handle(Geom2d_Circle) GCirc2dPln;
  Handle(Geom_Circle) GCircPln;
  gp_Ax2 circAx2 = FilAx3.Ax2();
  if(!c1sphere){
    ElSLib::PlaneParameters(PosPl,P,u,v);
    gp_Pnt2d p2dPln(u,v);
    gp_Dir2d d2d(DSp.Dot(PosPl.XDirection()),DSp.Dot(PosPl.YDirection()));
    gp_Ax22d ax2dPln(c2dPln,gp_Dir2d(gp_Vec2d(c2dPln,p2dPln)),d2d);
    gp_Circ2d circ2dPln(ax2dPln,Rad);
    GCirc2dPln = new Geom2d_Circle(circ2dPln);
    circAx2.SetLocation(cPln);
    gp_Circ circPln(circAx2,Rad);
    GCircPln = new Geom_Circle(circPln);
  }
  else {
    ElSLib::PlaneParameters(PosPl,P,u,v);
    gp_Pnt2d p2dPln(u,v),pbid;
    if(plandab){
      Data->Set2dPoints(p2dPln,p2dPln,pbid,pbid);
    }
    else {
      Data->Set2dPoints(pbid,pbid,p2dPln,p2dPln);
    }
  }
  gp_Lin2d lin2dFil(p2dFil,gp::DX2d());
  Handle(Geom2d_Line) GLin2dFil1 = new Geom2d_Line(lin2dFil);
  toreverse = ( norFil.Dot(Dpnat) <= 0. );
  TopAbs_Orientation trans; 
  if ((toreverse && plandab) || (!toreverse && !plandab) ){ 
    trans = TopAbs_FORWARD; 
  }
  else { 
    trans = TopAbs_REVERSED; 
  }
  if(plandab){
    Data->ChangeInterferenceOnS1().
      SetInterference(ChFiKPart_IndexCurveInDS(GCircPln,DStr),
		      trans,GCirc2dPln,GLin2dFil1);
  }
  else{
    Data->ChangeInterferenceOnS2().
      SetInterference(ChFiKPart_IndexCurveInDS(GCircPln,DStr),
		      trans,GCirc2dPln,GLin2dFil1);
  }

  // The cylindrical face.
  P.SetCoord(Or.X()+cylrad*Dx.X(),
	     Or.Y()+cylrad*Dx.Y(),
	     Or.Z()+cylrad*Dx.Z());
  u = 0.;
  if (dedans) {
    if (plandab && !c1sphere) { v = 2 * M_PI; }
    else { v = 0. ; }
  }
  else { v = M_PI; }
  p2dFil.SetCoord(u,v);
  if(c1sphere){
    ElSLib::SphereD1(u,v,FilAx3,cylrad,PP,deru,derv);
  }  
  else{
    ElSLib::TorusD1(u,v,FilAx3,Rad,Radius,PP,deru,derv);
  }  
  norFil = deru.Crossed(derv);
  lin2dFil.SetLocation(p2dFil);
  Handle(Geom2d_Line) GLin2dFil2 = new Geom2d_Line(lin2dFil);
  ElSLib::Parameters(Cyl,P,u,v);
  Standard_Real tol = Precision::PConfusion();
  Standard_Boolean careaboutsens = 0;
  if(Abs(lu - fu - 2*M_PI) < tol) careaboutsens = 1;
  if(u >= fu - tol && u < fu) u = fu;
  if(u <= lu + tol && u > lu) u = lu;
  if(u < fu || u > lu) u = ChFiKPart_InPeriod(u,fu,fu + 2*M_PI,tol);
  ElSLib::D1(u,v,Cyl,PP,deru,derv);
  gp_Dir norcyl = deru.Crossed(derv);
  gp_Dir2d d2dCyl = gp::DX2d();
  if( deru.Dot(Dy) < 0. ){
    d2dCyl.Reverse(); 
    if(careaboutsens && Abs(fu-u)<tol) u = lu;
  }
  else if(careaboutsens && Abs(lu-u)<tol) u = fu;
  gp_Pnt2d p2dCyl(u,v);
  gp_Lin2d lin2dCyl(p2dCyl,d2dCyl);
  Handle(Geom2d_Line) GLin2dCyl = new Geom2d_Line(lin2dCyl);
  circAx2.SetLocation(Or);
  gp_Circ circCyl(circAx2,cylrad);
  Handle(Geom_Circle) GCircCyl = new Geom_Circle(circCyl);
  toreverse = ( norFil.Dot(norcyl) <= 0. );
  if ((toreverse && plandab) || (!toreverse && !plandab) ){ 
    trans = TopAbs_REVERSED; 
  }
  else { 
    trans = TopAbs_FORWARD; 
  }
  if(plandab){
    Data->ChangeInterferenceOnS2().
      SetInterference(ChFiKPart_IndexCurveInDS(GCircCyl,DStr),
		      trans,GLin2dCyl,GLin2dFil2);
    
  }
  else{
    Data->ChangeInterferenceOnS1().
      SetInterference(ChFiKPart_IndexCurveInDS(GCircCyl,DStr),
		      trans,GLin2dCyl,GLin2dFil2);
  }
  return Standard_True;
}


