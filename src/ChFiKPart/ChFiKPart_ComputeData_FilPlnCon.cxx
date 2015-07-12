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
#include <Geom_Line.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Cone.hxx>
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
//purpose  : case cone/plane or plane/cone.
//=======================================================================
Standard_Boolean ChFiKPart_MakeFillet(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& Pln, 
				      const gp_Cone& Con, 
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
//calculate the fillet (torus or sphere).
  Standard_Boolean c1sphere = Standard_False;
  gp_Ax3 PosPl = Pln.Position();
  gp_Dir Dpnat = PosPl.XDirection().Crossed(PosPl.YDirection());
  gp_Dir Dp = Dpnat;
  gp_Dir Df = Dp;
  if (Or1 == TopAbs_REVERSED) { Dp.Reverse(); }
  if (Ofpl == TopAbs_REVERSED) { Df.Reverse(); }

  gp_Pnt Or = Con.Location();
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
  ElCLib::D1(First,Spine,PtSp,DSp);
  IntAna_QuadQuadGeo CInt (Pln,Con,Precision::Angular(),
			   Precision::Confusion());
  gp_Pnt Pv;
  if (CInt.IsDone()) {
    //The origin of the fillet is set at the start point on the  
    //guideline.
    Pv = ElCLib::Value(ElCLib::Parameter(CInt.Circle(1),PtSp),
		       CInt.Circle(1));
  }
  else { return Standard_False; }
  gp_Dir Dx(gp_Vec(cPln,Pv));
  gp_Dir Dy(DSp);
  ElSLib::Parameters(Con,Pv,u,v);
  gp_Pnt PtCon;
  gp_Vec Vu,Vv;
  ElSLib::D1(u,v,Con,PtCon,Vu,Vv);
  gp_Dir Dc(Vu.Crossed(Vv));
  if (Or2 == TopAbs_REVERSED) { Dc.Reverse(); }
  gp_Dir Dz = Dp;

  gp_Pnt pp(Pv.X()+Dc.X(),Pv.Y()+Dc.Y(),Pv.Z()+Dc.Z());
  ElSLib::PlaneParameters(PosPl,pp,u,v);
  ElSLib::PlaneD0(u,v,PosPl,pp);
  gp_Dir ddp(gp_Vec(Pv,pp));
  ElSLib::Parameters(Con,Pv,u,v);
  gp_Vec dcu,dcv;
  ElSLib::D1(u,v,Con,pp,dcu,dcv);
  gp_Dir ddc(dcv);
  if(ddc.Dot(Dp) < 0.) ddc.Reverse();
  Standard_Real Ang = ddp.Angle(ddc);
  Standard_Real Rabio = Radius/Tan(Ang/2);
  Standard_Real Maxrad = cPln.Distance(Pv);
  Standard_Real Rad;
  Standard_Boolean dedans = Dx.Dot(Dc) <= 0. ; 
  if( dedans ){ 
    if (!plandab){ Dz.Reverse(); } 
    Rad = Maxrad - Rabio;
    if(Abs(Rad) <= Precision::Confusion()){ c1sphere = Standard_True; }
    else if(Rad < 0){ 
#ifdef OCCT_DEBUG
      cout<<"the fillet does not pass"<<endl; 
#endif
      return Standard_False;
    }
  }
  else { 
    if (plandab){ Dz.Reverse(); } 
    Rad = Maxrad + Rabio; 
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
  
  // It is checked if the orientation of the fillet is the same  
  // as of the faces.
  gp_Pnt P,PP;
  gp_Vec deru,derv;
  P.SetCoord(cPln.X()+Rad*Dx.X(),
	     cPln.Y()+Rad*Dx.Y(),
	     cPln.Z()+Rad*Dx.Z());
  if(c1sphere){
    ElSLib::SphereParameters(FilAx3,Rad,P,u,v);
    ElSLib::SphereD1(u,v,FilAx3,Rad,PP,deru,derv);
  }  
  else{
    ElSLib::TorusParameters(FilAx3,Rad,Radius,P,u,v);
    ElSLib::TorusD1(u,v,FilAx3,Rad,Radius,PP,deru,derv);
    if(!plandab && Ang < M_PI/2 && dedans) v = v + 2*M_PI;
  }  
  gp_Pnt2d p2dFil(0.,v);
  gp_Dir norFil(deru.Crossed(derv));
  Standard_Boolean toreverse = ( norFil.Dot(Df) <= 0. );
  if (toreverse) { Data->ChangeOrientation() = TopAbs_REVERSED; }
  else { Data->ChangeOrientation() = TopAbs_FORWARD; }

  // FaceInterferences are loaded with pcurves and curves 3d.
  // ---------------------------------------------------------------

  // The plane face.
  // --------------

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

  // The conic face.
  // ----------------

  P.SetCoord(Pv.X()+Rabio*ddc.X(),
	     Pv.Y()+Rabio*ddc.Y(),
	     Pv.Z()+Rabio*ddc.Z());
  if(c1sphere){
    ElSLib::SphereParameters(FilAx3,Radius,P,u,v);
    ElSLib::SphereD1(u,v,FilAx3,Radius,PP,deru,derv);
  }  
  else{
    ElSLib::TorusParameters(FilAx3,Rad,Radius,P,u,v);
    ElSLib::TorusD1(u,v,FilAx3,Rad,Radius,PP,deru,derv);
    if(plandab && Ang < M_PI/2 && dedans) v = v + 2*M_PI;
  }  
  norFil = deru.Crossed(derv);
  p2dFil.SetCoord(0.,v);
  lin2dFil.SetLocation(p2dFil);
  Handle(Geom2d_Line) GLin2dFil2 = new Geom2d_Line(lin2dFil);
  ElSLib::Parameters(Con,P,u,v);
  Standard_Real tol = Precision::PConfusion();
  Standard_Boolean careaboutsens = 0;
  if(Abs(lu - fu - 2*M_PI) < tol) careaboutsens = 1;
  if(u >= fu - tol && u < fu) u = fu;
  if(u <= lu + tol && u > lu) u = lu;
  if(u < fu || u > lu) u = ElCLib::InPeriod(u,fu,fu + 2*M_PI);
  ElSLib::D1(u,v,Con,PP,deru,derv);
  gp_Dir norCon = deru.Crossed(derv);
  gp_Dir2d d2dCon = gp::DX2d();
  if( deru.Dot(Dy) < 0. ){
    d2dCon.Reverse(); 
    if(careaboutsens && Abs(fu-u)<tol) u = lu;
  }
  else if(careaboutsens && Abs(lu-u)<tol) u = fu;
  gp_Pnt2d p2dCon(u,v);
  gp_Lin2d lin2dCon(p2dCon,d2dCon);
  Handle(Geom2d_Line) GLin2dCon = new Geom2d_Line(lin2dCon);
  Standard_Real scal = gp_Vec(Dp).Dot(gp_Vec(Pv,P));
  PP.SetCoord(cPln.X()+scal*Dp.X(),
	      cPln.Y()+scal*Dp.Y(),
	      cPln.Z()+scal*Dp.Z());
  circAx2.SetLocation(PP);
  gp_Circ circCon(circAx2,P.Distance(PP));
  Handle(Geom_Circle) GCircCon = new Geom_Circle(circCon);
  toreverse = ( norFil.Dot(norCon) <= 0. );
  if ((toreverse && plandab) || (!toreverse && !plandab) ){ 
    trans = TopAbs_REVERSED; 
  }
  else { 
    trans = TopAbs_FORWARD; 
  }
  if(plandab){
    Data->ChangeInterferenceOnS2().
      SetInterference(ChFiKPart_IndexCurveInDS(GCircCon,DStr),
		      trans,GLin2dCon,GLin2dFil2);
  }
  else{
    Data->ChangeInterferenceOnS1().
      SetInterference(ChFiKPart_IndexCurveInDS(GCircCon,DStr),
		      trans,GLin2dCon,GLin2dFil2);
  }
  return Standard_True;
}
