// Created on: 1994-06-27
// Created by: Laurent BOURESCHE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <ChFiKPart_ComputeData.jxx>
#include <ChFiKPart_ComputeData_Fcts.hxx>

#include <Precision.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Lin.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>

#include <ElCLib.hxx>
#include <ElSLib.hxx>

#include <Geom2d_Line.hxx>
#include <Geom_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ToroidalSurface.hxx>

#include <IntAna_QuadQuadGeo.hxx>

//=======================================================================
//function : MakeRotule
//Purpose  : cas plan/plan.
//=======================================================================

Standard_Boolean ChFiKPart_MakeRotule(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& pl, 
				      const gp_Pln& pl1, 
				      const gp_Pln& pl2, 
				      const TopAbs_Orientation opl,
				      const TopAbs_Orientation opl1,
				      const TopAbs_Orientation opl2,
				      const Standard_Real r, 
				      const TopAbs_Orientation ofpl)
{

  //calcul du tore.
  //---------------
  gp_Ax3 pos = pl.Position();
  gp_Dir dpl = pos.XDirection().Crossed(pos.YDirection());
  gp_Dir dfpl = dpl;
  gp_Dir dplnat = dpl;
  if (opl == TopAbs_REVERSED) { dpl.Reverse(); }
  if (ofpl == TopAbs_REVERSED) { dfpl.Reverse(); }
  pos = pl1.Position();
  gp_Dir dpl1 = pos.XDirection().Crossed(pos.YDirection());
  if (opl1 == TopAbs_REVERSED) { dpl1.Reverse(); }
  pos = pl2.Position();
  gp_Dir dpl2 = pos.XDirection().Crossed(pos.YDirection());
  if (opl2 == TopAbs_REVERSED) { dpl2.Reverse(); }

  Standard_Real alpha = dpl1.Angle(dpl2);

  IntAna_QuadQuadGeo LInt (pl1,pl2,Precision::Angular(),
			   Precision::Confusion());
  gp_Pnt ptor,pcirc;
  if (LInt.IsDone()) {

    pcirc = ElCLib::Value(ElCLib::Parameter(LInt.Line(1),pl.Location()),
			 LInt.Line(1));
    ptor.SetCoord(pcirc.X()+r*dpl.X(),
		  pcirc.Y()+r*dpl.Y(),
		  pcirc.Z()+r*dpl.Z());
  }
  else { return Standard_False; }

  gp_Ax3 ppos(ptor,dpl.Reversed(),dpl1);
  if(ppos.YDirection().Dot(dpl2) < 0.) ppos.YReverse();
  Handle(Geom_ToroidalSurface) 
    gtor = new Geom_ToroidalSurface(ppos,r,r);
  Data->ChangeSurf(ChFiKPart_IndexSurfaceInDS(gtor,DStr));

  //on compare l orientation du tore a celle de la face en bout.
  //------------------------------------------------------------
  gp_Pnt pp;
  gp_Vec du,dv;
  ElSLib::TorusD1(0.,M_PI/2,ppos,r,r,pp,du,dv);
  gp_Dir drot(du.Crossed(dv));
  Standard_Boolean reversecur = ( drot.Dot(dplnat) <= 0. );
  Standard_Boolean reversefil = ( drot.Dot(dfpl) <= 0. );
  if (reversefil) { Data->ChangeOrientation() = TopAbs_REVERSED; }
  else { Data->ChangeOrientation() = TopAbs_FORWARD; }

  //on charge les FaceInterferences avec les pcurves et courbes 3d.
  //-----------------------------------------------------------------
  
  //du cote du plan
  //---------------
  gp_Ax2 circAx2 = ppos.Ax2();
  circAx2.SetLocation(pcirc);
  Handle(Geom_Circle) GC = new Geom_Circle(circAx2,r);
  Standard_Real u,v;
  ElSLib::Parameters(pl,pcirc,u,v);
  gp_Pnt2d p2dcirc(u,v);
  gp_Dir2d dx2d(dpl1.Dot(pl.Position().XDirection()),
		dpl1.Dot(pl.Position().YDirection()));
  gp_Dir2d dy2d(ppos.YDirection().Dot(pl.Position().XDirection()),
		ppos.YDirection().Dot(pl.Position().YDirection()));
  gp_Ax22d circ2dax(p2dcirc,dx2d,dy2d);
  Handle(Geom2d_Circle) GC2d = new Geom2d_Circle(circ2dax,r);
  gp_Pnt2d p2dlin(0.,M_PI/2);
  Handle(Geom2d_Line) GL2d = new Geom2d_Line(p2dlin,gp::DX2d());
  TopAbs_Orientation trans = TopAbs_REVERSED; 
  if (reversecur) trans = TopAbs_FORWARD; 
  Data->ChangeInterferenceOnS1().
    SetInterference(ChFiKPart_IndexCurveInDS(GC,DStr),trans,GC2d,GL2d);
  
  //du cote pointu
  //--------------
  Handle(Geom_Curve) bid;
  Handle(Geom2d_Curve) bid2d;
  p2dlin.SetCoord(0.,M_PI);
  Handle(Geom2d_Line) GL2dcoin = new Geom2d_Line(p2dlin,gp::DX2d());
  Data->ChangeInterferenceOnS2().
    SetInterference(ChFiKPart_IndexCurveInDS(bid,DStr),trans,bid2d,GL2dcoin);

  //et les points
  //-------------
  Data->ChangeVertexFirstOnS1().SetPoint(pp);
  ElSLib::TorusD0(alpha,M_PI/2,ppos,r,r,pp);
  Data->ChangeVertexLastOnS1().SetPoint(pp);
  Data->ChangeInterferenceOnS1().SetFirstParameter(0.);
  Data->ChangeInterferenceOnS1().SetLastParameter(alpha);
  Data->ChangeInterferenceOnS2().SetFirstParameter(0.);
  Data->ChangeInterferenceOnS2().SetLastParameter(alpha);

  return Standard_True;
}
