// File:	ChFiKPart_ComputeData_Sphere.cxx
// Created:	Tue Nov 22 14:41:44 1994
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#include <ChFiKPart_ComputeData.jxx>
#include <Precision.hxx>
#include <gp.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gce_MakeCirc.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_SphericalSurface.hxx>
#include <ElSLib.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <ChFiKPart_ComputeData_Fcts.hxx>

//=======================================================================
//function : ChFiKPart_Sphere
//purpose  : Construction d un conge spherique dont les contours ne sont
//           pas tous des isos, a partir de ses trois sommets.
//=======================================================================

Standard_Boolean ChFiKPart_Sphere(TopOpeBRepDS_DataStructure& DStr,
				  const Handle(ChFiDS_SurfData)& Data, 
				  const Handle(Adaptor3d_HSurface)& S1, 
				  const Handle(Adaptor3d_HSurface)& S2,
				  const TopAbs_Orientation OrFace1,
				  const TopAbs_Orientation OrFace2,
				  const TopAbs_Orientation Or1,
				  const TopAbs_Orientation,
				  const Standard_Real Rad, 
				  const gp_Pnt2d& PS1,
				  const gp_Pnt2d& P1S2,
				  const gp_Pnt2d& P2S2)
{
  // Construction de la sphere :
  // - pole sud sur PS1
  // - origine des u donnee par P1S2
  // - u+ vers P2S2

  Standard_Real ptol = Precision::Confusion();
  gp_Pnt p1,p2,p3;
  gp_Vec v1,v2;
  S1->D1(PS1.X(),PS1.Y(),p1,v1,v2);
  gp_Dir ds1(v1.Crossed(v2));
  gp_Dir df1 = ds1;
  gp_Dir dnat1 = ds1;
  if(Or1 == TopAbs_REVERSED) ds1.Reverse();
  if(OrFace1 == TopAbs_REVERSED) df1.Reverse();
  S2->D0(P1S2.X(),P1S2.Y(),p2);
  S2->D0(P2S2.X(),P2S2.Y(),p3);
  gp_Circ ci = gce_MakeCirc(p1,p2,p3);
  gp_Dir di = ci.Axis().Direction();
  gp_Pnt pp = ci.Location();
  Standard_Real rr = ci.Radius();
  Standard_Real delta = sqrt(Rad*Rad-rr*rr);
  gp_Pnt cen(pp.X()+delta*di.X(),
	     pp.Y()+delta*di.Y(),
	     pp.Z()+delta*di.Z());
  gp_Dir dz(gp_Vec(p1,cen));
  if(Abs(ds1.Dot(dz)-1.)>ptol){
    cen.SetCoord(pp.X()-delta*di.X(),
		 pp.Y()-delta*di.Y(),
		 pp.Z()-delta*di.Z());
    dz = gp_Dir(gp_Vec(p1,cen));
    if(Abs(ds1.Dot(dz)-1.)>ptol){
#ifdef DEB
      cout<<"centre du coin spherique non trouve"<<endl;
#endif
      return Standard_False;
    }
  }
  gp_Dir ddx(gp_Vec(cen,p2));
  gp_Dir dddx = ddx;
  gp_Dir ddy(gp_Vec(cen,p3));
  gp_Dir dx = dz.Crossed(ddx.Crossed(dz));
  gp_Dir ddz = dz.Reversed();
  gp_Ax3 FilAx3(cen,dz,dx);
  if (FilAx3.YDirection().Dot(ddy) <= 0.){ 
    FilAx3.YReverse(); 
    ddz.Reverse();
    dddx.Reverse();
  }
  Handle(Geom_SphericalSurface) 
    gsph = new Geom_SphericalSurface(FilAx3,Rad);
  Data->ChangeSurf(ChFiKPart_IndexSurfaceInDS(gsph,DStr));

  // on compare la normale de la sphere a celle de la face
  // orientee pour determiner l orientation finale du conge.
  Standard_Boolean toreverse = ( ddz.Dot(df1) <= 0. );
  if (toreverse) { Data->ChangeOrientation() = TopAbs_REVERSED; }
  else { Data->ChangeOrientation() = TopAbs_FORWARD; }

  // On calcule les parametres de p2 et p3 sur la Sphere pour avoir
  // les ranges des courbes.
  Standard_Real uu1,vv1,uu2,vv2;
  ElSLib::SphereParameters(FilAx3,Rad,p2,uu1,vv1);
  uu1 = 0.;
  ElSLib::SphereParameters(FilAx3,Rad,p3,uu2,vv2);

  // On charge les FaceInterferences avec les pcurves et courbes 3d.

  // Le cote pointu.
  
  Handle(Geom_Curve) C;
  Handle(Geom2d_Curve) C2d;
  gp_Pnt2d p2dFil(0.,-PI/2.);
  gp_Lin2d lin2dFil(p2dFil,gp::DX2d());
  Handle(Geom2d_Curve) C2dFil = new Geom2d_Line(lin2dFil);
  toreverse = ( ddz.Dot(dnat1) <= 0. );
  TopAbs_Orientation trans = TopAbs_REVERSED; 
  if (toreverse) trans = TopAbs_FORWARD; 
  Data->ChangeInterferenceOnS1().
    SetInterference(ChFiKPart_IndexCurveInDS(C,DStr),
		    trans,C2d,C2dFil);

  // L autre cote.

  Standard_Real ang = ddx.Angle(ddy);
  gp_Dir dci = ddx.Crossed(ddy);
  gp_Ax2 axci(cen,dci,ddx);
  gp_Circ ci2(axci,Rad);
  C = new Geom_Circle(ci2);
  GeomAdaptor_Surface AS(gsph);
  GeomAdaptor_Curve AC(C,0.,ang);
  ChFiKPart_ProjPC(AC,AS,C2dFil);
  gp_Pnt2d p2dbid = C2dFil->Value(0.);
  gp_Pnt2d pp2dbid(uu1,vv1);
  if(!pp2dbid.IsEqual(p2dbid,ptol)){
    gp_Vec2d v2dbid(p2dbid,pp2dbid);
    C2dFil->Translate(v2dbid);
  }
  gp_Vec2d v2d(P1S2,P2S2);
  gp_Dir2d d2d(v2d);
  if(Abs(v2d.Magnitude()-ang) <= ptol){
    gp_Lin2d l2d(P1S2,d2d);
    C2d = new Geom2d_Line(l2d);
  }
  else C2d = ChFiKPart_PCurve(P1S2,P2S2,0.,ang);
  gp_Pnt pp1;
  S2->D1(P1S2.X(),P1S2.Y(),pp1,v1,v2);
  gp_Dir ds2(v1.Crossed(v2));
  toreverse = (ds2.Dot(dddx) <= 0.);
  trans = TopAbs_REVERSED; 
  if (!toreverse) trans = TopAbs_FORWARD; 
  Data->ChangeInterferenceOnS2().
    SetInterference(ChFiKPart_IndexCurveInDS(C,DStr),
		    trans,C2d,C2dFil);
  
  Data->ChangeVertexFirstOnS1().SetPoint(p1);
  Data->ChangeVertexLastOnS1().SetPoint(p1);
  Data->ChangeVertexFirstOnS2().SetPoint(p2);
  Data->ChangeVertexLastOnS2().SetPoint(p3);
  Data->ChangeInterferenceOnS1().SetFirstParameter(0.);
  Data->ChangeInterferenceOnS1().SetLastParameter(uu2);
  Data->ChangeInterferenceOnS2().SetFirstParameter(0.);
  Data->ChangeInterferenceOnS2().SetLastParameter(ang);

  return Standard_True;
}


