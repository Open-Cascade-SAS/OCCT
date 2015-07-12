// Created on: 1998-06-16
// Created by: Philippe NOUAILLE
// Copyright (c) 1998-1999 Matra Datavision
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
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <Precision.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>

//pour tester
//=======================================================================
//function : MakeChAsym
//purpose  : Compute the chamfer in the particular case Plane/Cylinder
//           or Cylinder/Plane
//           Compute the SurfData <Data> of the chamfer build on the <Spine>
//           between the plane <Pln> and the cylinder <Cyl>, with the 
//           distances <Dis> and <Angle> on <Pln> on <Cyl>.
//           <Or1> and <Or2> are the orientations of <Pln> and <Cyl>
//           and <Ofpl> this of the face carried by <Pln>.
//           <First> is the start point on the <Spine>
//           <Plandab> is equal to True if the plane is the surface S1
//           <fu> and <lu> are the first and last u parameters of the
//           cylinder
//           DisOnPlan is equal to True if Dis on plan, else False 
//out      : True if the chanfer has been computed
//           False else
//=======================================================================
Standard_Boolean ChFiKPart_MakeChAsym(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& Pln, 
				      const gp_Cylinder& Cyl, 
				      const Standard_Real fu,
				      const Standard_Real lu,
				      const TopAbs_Orientation Or1,
				      const TopAbs_Orientation Or2,
				      const Standard_Real Dis, 
				      const Standard_Real Angle,
				      const gp_Circ& Spine, 
				      const Standard_Real First, 
				      const TopAbs_Orientation Ofpl,
				      const Standard_Boolean plandab,
                                      const Standard_Boolean DisOnP)
{
  // compute the chamfer surface(cone)

  // compute the normals to the plane surface & to the plane face
  gp_Ax3 PosPl = Pln.Position();
  gp_Dir Dpl   = PosPl.XDirection().Crossed(PosPl.YDirection());
  gp_Dir norf  = Dpl;
  if ( Ofpl == TopAbs_REVERSED) norf.Reverse();
  if (Or1 == TopAbs_REVERSED) Dpl.Reverse();

  // compute the origin Or of the cone
  gp_Pnt Or = Cyl.Location();
  Standard_Real u, v;
  ElSLib::PlaneParameters(PosPl, Or, u, v);
  gp_Pnt2d pt2dPln(u, v);
  ElSLib::PlaneD0(u, v, PosPl, Or);
  gp_Pnt PtPl = Or;  // projection of the cylinder origin 
                     //on the plane 

  gp_Pnt PtSp;//start 3d point on the Spine 
  gp_Vec DSp; //tangent vector to the spine on PtSp
  ElCLib::D1(First, Spine, PtSp, DSp);
  gp_Dir Dx(gp_Vec(Or, PtSp));
  gp_Dir Dy(DSp);
  ElSLib::Parameters(Cyl, PtSp, u, v);
  gp_Pnt PtCyl;//point on the cylinder and on the Spine
  gp_Vec Vu, Vv;
  ElSLib::D1(u, v, Cyl, PtCyl, Vu, Vv);
  gp_Dir Dcyl(Vu.Crossed(Vv));//normal to the cylinder in PtSp
  if (Or2 == TopAbs_REVERSED) Dcyl.Reverse();
  Standard_Boolean dedans = ( Dcyl.Dot(Dx) <= 0.);

  Standard_Boolean pointu = Standard_False;
  Standard_Real ConRad, Rad, SemiAngl;

  //Calculation of distance
  Standard_Real dis1, dis2, cosNPCyl, sinNPCyl;

  if ( (plandab && DisOnP) || (!plandab && !DisOnP) ) {
    dis1     = Dis;
    cosNPCyl = Dpl.Dot(Dcyl);
    sinNPCyl = sqrt(1. - cosNPCyl * cosNPCyl);
    dis2     = Dis / (sinNPCyl / tan(Angle) - cosNPCyl);
  } 
  else {
    dis2     = Dis;
    cosNPCyl = Dpl.Dot(Dcyl);
    sinNPCyl = sqrt(1. - cosNPCyl * cosNPCyl);
    dis1     = Dis / (sinNPCyl / tan(Angle) - cosNPCyl);    
  }

  Or.SetCoord(Or.X() + dis2 * Dpl.X(),
	      Or.Y() + dis2 * Dpl.Y(),
	      Or.Z() + dis2 * Dpl.Z());

      // variables used to compute the semiangle of the cone
  gp_Dir Vec1(Or.X() - PtPl.X(), Or.Y() - PtPl.Y(), Or.Z() - PtPl.Z()); 
  gp_Pnt Pt(Or.X() + dis1*PosPl.XDirection().X(),
	    Or.Y() + dis1*PosPl.XDirection().Y(),
	    Or.Z() + dis1*PosPl.XDirection().Z());
  gp_Dir Vec2( Pt.X() - PtPl.X(), Pt.Y() - PtPl.Y(), Pt.Z() - PtPl.Z());

  // compute the parameters of the conical surface
  if (dedans) {
    Rad = Cyl.Radius() - dis1;
    if ( Abs(Rad) <= Precision::Confusion() ) pointu = Standard_True;
    if(Rad < 0 ) {
#ifdef OCCT_DEBUG
      cout<<"the chamfer can't pass"<<endl;
#endif
      return Standard_False;
    }
  }
  else {
    Rad = Cyl.Radius() + dis1;
    gp_Dir Dplr = Dpl.Reversed();
    Dpl = Dplr;
   }
  ConRad   = Cyl.Radius();
  SemiAngl = Vec1.Angle(Vec2);
  gp_Ax3 ConAx3(Or, Dpl, Dx);

  Handle (Geom_ConicalSurface)
    gcon = new Geom_ConicalSurface( ConAx3, SemiAngl, ConRad );

  // changes due to the fact the parameters of the chamfer must go increasing
  // from surface S1 to surface S2
  if ( (dedans && !plandab) || (!dedans && plandab) ) {
    gcon->VReverse();// be carefull : the SemiAngle was changed
    ConAx3   = gcon->Position();
    SemiAngl = gcon->SemiAngle();
  }

      // changes due to the fact we have reversed the V direction of 
      // parametrization
  if (ConAx3.YDirection().Dot(DSp) <= 0.) {
    ConAx3.YReverse();
    gcon->SetPosition(ConAx3);
  }

  Data->ChangeSurf(ChFiKPart_IndexSurfaceInDS(gcon, DStr));

  // compute the chamfer's orientation according to the orientation
  // of the faces

  //search the normal to the cone
  gp_Vec deru, derv;
  ElSLib::ConeD1(0., 0., ConAx3, ConRad, SemiAngl, Pt, deru, derv);

  gp_Dir norCon(deru.Crossed(derv));
  
  Standard_Boolean toreverse = ( norCon.Dot(norf) <= 0.);
  if (toreverse) {
    Data->ChangeOrientation() = TopAbs_REVERSED; 
  }
  else {
    Data->ChangeOrientation() = TopAbs_FORWARD; 
  }
  
  //we load of the faceInterference with the pcurves and
  // the 3d curves

  // Case of the plane face
  // NB: in the case 'pointu', no pcurve on the plane surface
  // and no intersection plane-chamfer are needed
  Handle(Geom2d_Circle) GCir2dPln;
  Handle(Geom_Circle) GCirPln;
  gp_Ax2 CirAx2 = ConAx3.Ax2();
  CirAx2.SetLocation(PtPl);

  if (!pointu) {
      // intersection plane-chamfer
    gp_Circ CirPln(CirAx2, Rad);
    GCirPln = new Geom_Circle(CirPln);

    //pcurve on the plane
    ElSLib::PlaneParameters(PosPl, Pt, u, v);
    gp_Pnt2d p2dPln(u, v);
    gp_Dir2d d2d(DSp.Dot(PosPl.XDirection()), DSp.Dot(PosPl.YDirection()));
    gp_Ax22d ax2dPln(pt2dPln, gp_Dir2d(gp_Vec2d(pt2dPln, p2dPln)), d2d);
    gp_Circ2d cir2dPln(ax2dPln, Rad);
    GCir2dPln = new Geom2d_Circle(cir2dPln);
  }

  //pcurve on the chamfer
  gp_Pnt2d p2dch;
  if (plandab) 
    v= -sqrt(dis1 * dis1 + dis2 * dis2);
  else 
    v = sqrt(dis1 * dis1 + dis2 * dis2);
  p2dch.SetCoord(0., v);
  ElSLib::ConeD1(0., v, ConAx3, ConRad, SemiAngl, Pt, deru, derv);
  gp_Lin2d lin2dch(p2dch, gp::DX2d());
  Handle(Geom2d_Line) GLin2dCh1 = new Geom2d_Line(lin2dch);

  //orientation
  TopAbs_Orientation trans; 
  gp_Dir norpl = PosPl.XDirection().Crossed(PosPl.YDirection());
  toreverse = ( norCon.Dot(norpl) <= 0. );
  if ((toreverse && plandab) || (!toreverse && !plandab)){ 
    trans = TopAbs_FORWARD;
  }
  else { 
    trans = TopAbs_REVERSED; 
  }


  if(plandab){
    Data->ChangeInterferenceOnS1().
      SetInterference(ChFiKPart_IndexCurveInDS(GCirPln, DStr),
		      trans, GCir2dPln, GLin2dCh1);
  }
  else{
    Data->ChangeInterferenceOnS2().
      SetInterference(ChFiKPart_IndexCurveInDS(GCirPln, DStr),
		      trans,GCir2dPln, GLin2dCh1);
  }

  // Case of the cylindrical face

  //intersection cylinder-chamfer
  CirAx2.SetLocation(Or);
  gp_Circ CirCyl(CirAx2, ConRad);
  Handle(Geom_Circle) GCirCyl = new Geom_Circle(CirCyl);

  //pcurve on the chamfer
  p2dch.SetCoord(0., 0.);
  ElSLib::ConeD1(0., 0., ConAx3, ConRad, SemiAngl, Pt, deru, derv);
  lin2dch.SetLocation(p2dch);
  Handle(Geom2d_Line) GLin2dCh2 = new Geom2d_Line(lin2dch);

  //pcurve on the cylinder
  norCon.SetXYZ (deru.Crossed(derv).XYZ());

  Pt.SetCoord(Or.X() + ConRad * Dx.X(),
	      Or.Y() + ConRad * Dx.Y(),
	      Or.Z() + ConRad * Dx.Z());
  ElSLib::Parameters(Cyl, Pt ,u, v);
  Standard_Real tol = Precision::PConfusion();
  Standard_Boolean careaboutsens = 0;
  if(Abs(lu - fu - 2 * M_PI) < tol) careaboutsens = 1;
  if(u >= fu - tol && u < fu) u = fu;
  if(u <= lu + tol && u > lu) u = lu;
  if(u < fu || u > lu) u = ChFiKPart_InPeriod(u, fu, fu + 2 * M_PI, tol);

  ElSLib::D1(u, v, Cyl, Pt, deru, derv);
  gp_Dir   norcyl = deru.Crossed(derv);
  gp_Dir2d d2dCyl = gp::DX2d();
  if( deru.Dot(Dy) < 0. ){
    d2dCyl.Reverse(); 
    if(careaboutsens && Abs(fu - u)<tol) u = lu;
  }
  else if(careaboutsens && Abs(lu - u)<tol) u = fu;
  gp_Pnt2d p2dCyl(u, v);
  gp_Lin2d lin2dCyl(p2dCyl, d2dCyl);
  Handle(Geom2d_Line) GLin2dCyl = new Geom2d_Line(lin2dCyl);

      //orientation
  toreverse = ( norCon.Dot(norcyl) <= 0. );
  if ((toreverse && plandab) || (!toreverse && !plandab) ) {
    trans = TopAbs_REVERSED;
  }
  else {
    trans = TopAbs_FORWARD;
  }

  if(plandab){
    Data->ChangeInterferenceOnS2().
      SetInterference(ChFiKPart_IndexCurveInDS(GCirCyl, DStr),
		      trans, GLin2dCyl, GLin2dCh2);
  }
  else{
    Data->ChangeInterferenceOnS1().
      SetInterference(ChFiKPart_IndexCurveInDS(GCirCyl, DStr),
		      trans, GLin2dCyl, GLin2dCh2);
  }

  return Standard_True;
}


//=======================================================================
//function : MakeChAsym
//purpose  : case cylinder/plane or plane/cylinder.
//=======================================================================

Standard_Boolean ChFiKPart_MakeChAsym(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& Pln, 
				      const gp_Cylinder& Cyl, 
				      const Standard_Real /*fu*/,
				      const Standard_Real /*lu*/,
				      const TopAbs_Orientation Or1,
				      const TopAbs_Orientation Or2,
				      const Standard_Real Dis, 
				      const Standard_Real Angle,
				      const gp_Lin& Spine, 
				      const Standard_Real First, 
				      const TopAbs_Orientation Ofpl,
				      const Standard_Boolean plandab,
				      const Standard_Boolean DisOnP)
{
  // calculation of the fillet plane.
  // or1 and or2 permit to determine in which of four sides created by
  // intersection of 2 surfaces we are
  //        _|_          Ofpl is orientation of the plane face allowing
  //         |4          to determine the side of the material

  gp_Pnt OrSpine = ElCLib::Value(First, Spine);
  gp_Pnt POnCyl, POnPln, OrCyl;

  gp_Dir XDir = Spine.Direction();
  gp_Ax3 AxPln  = Pln.Position();
  gp_Dir NorPln = AxPln.XDirection().Crossed(AxPln.YDirection());
  gp_Dir NorF(NorPln);
  if (Or1 == TopAbs_REVERSED)
    {NorF.Reverse();} 

  gp_Ax3 AxCyl = Cyl.Position();
  // OrCyl is the point on axis of cylinder in the plane normal to the
  // axis containing OrSpine
  gp_Pnt Loc = AxCyl.Location();
  gp_Vec LocSp(Loc, OrSpine);
  gp_XYZ temp = AxCyl.Direction().XYZ();
  temp = temp.Multiplied(LocSp.XYZ().Multiplied(temp) );
  OrCyl.SetXYZ( (Loc.XYZ()).Added(temp) );
 
  //construction of POnPln
  gp_Vec VecTranslPln,tmp;

 tmp = gp_Vec(OrSpine,OrCyl);
  if ((Or2 == TopAbs_FORWARD && Cyl.Direct()) ||
      (Or2 == TopAbs_REVERSED && !Cyl.Direct()))
    {tmp.Reverse();}

  VecTranslPln = gp_Vec( XDir.Crossed(NorPln) );
  if( VecTranslPln.Dot(tmp) <= 0. )
     {VecTranslPln.Reverse();}

  gp_Vec VecTranslCyl;
  VecTranslCyl = gp_Vec(OrSpine,OrCyl);

  // Calculation of distances dis1 and dis2, depending on Dis and Angle
  gp_Vec DirSOrC = VecTranslCyl.Normalized();
  Standard_Real cosA1 = DirSOrC.Dot(VecTranslPln.Normalized());
  Standard_Real sinA1 = Sqrt(1. - cosA1 * cosA1);
  Standard_Real dis1 = 0.;
  Standard_Real dis2, ray = Cyl.Radius(); 
  Standard_Boolean IsDisOnP = ( (plandab && DisOnP) || (!plandab && !DisOnP) ); 
 
  if (IsDisOnP) {
    dis1 = Dis;
    Standard_Real sinAl = Sin(Angle), cosAl = Cos(Angle);
    Standard_Real h = dis1 * sinAl; 
    Standard_Real cosAhOC = cosA1 * sinAl + sinA1 * cosAl;
    Standard_Real sinAhOC = sinA1 * sinAl - cosA1 * cosAl;
    if (cosA1 > 0) sinAhOC = -sinAhOC;
    Standard_Real temp1 = h / ray,
                  temp2 = sinAhOC * sinAhOC + temp1 * cosAhOC;

    dis2 = temp2 + temp1 * (cosAhOC - temp1);

    if (dis2 < -1.E-09) {
#ifdef OCCT_DEBUG
      cout<<"too great angle of chamfer"<<endl;
#endif
      return Standard_False;
    }
    else if (dis2 < 1.E-09) {
      dis2 = ray * Sqrt(2. * temp2);
    }
    else {
      dis2 = ray * Sqrt(2. * (temp2 - sinAhOC * Sqrt(dis2)) );
    }
  }
  else {
    dis2 = Dis;
  }

  //construction of POnCyl 
  Standard_Real alpha = ( 2*ASin(dis2*0.5/ray) );
  gp_Vec VecTemp = VecTranslCyl.Reversed();

  if ( ( XDir.Crossed(gp_Dir(VecTranslCyl)) ).Dot(NorF) < 0.) {
    VecTemp.Rotate(gp_Ax1(OrCyl,XDir),alpha);
  }
  else { 
    VecTemp.Rotate(gp_Ax1(OrCyl,XDir.Reversed()),alpha);}

  POnCyl.SetXYZ( OrCyl.XYZ().Added(VecTemp.XYZ()) );
  Standard_Real UOnCyl,VOnCyl,UOnPln,VOnPln;
  gp_Vec DUOnCyl, DVOnCyl;
  ElSLib::Parameters(Cyl,POnCyl,UOnCyl,VOnCyl);
  ElSLib::CylinderD1(UOnCyl, VOnCyl, AxCyl, Cyl.Radius(),
                     POnCyl, DUOnCyl, DVOnCyl);		

  // Construction of the point on the plane
  if (!IsDisOnP) {
    gp_Vec Corde(POnCyl, OrSpine);
    gp_Vec TCyl = DUOnCyl.Crossed(DVOnCyl);

    TCyl =  XDir.Crossed(TCyl);
    TCyl.Normalize();
    Corde.Normalize();
    Standard_Real cosCorTan = TCyl.Dot(Corde);
    Standard_Real tgCorTan = 1. / (cosCorTan * cosCorTan);
    tgCorTan = Sqrt(tgCorTan - 1.);

    Standard_Real tgAng  = tan(Angle);
    tgAng = (tgAng + tgCorTan) / (1. - tgAng * tgCorTan);

    Standard_Real cosA11 = dis2 / (2. * ray);
    Standard_Real sinA11 = Sqrt(1. - cosA11 * cosA11);
    if (cosA1 > 0) sinA11 = -sinA11;
    dis1  = (sinA1 + cosA1 * tgAng) * cosA11;
    dis1 -= (cosA1 - sinA1 * tgAng) * sinA11;
    dis1  = (dis2 * tgAng) / dis1;
  }  

  VecTranslPln.Multiply(dis1);

  POnPln.SetXYZ( (OrSpine.XYZ()).Added(VecTranslPln.XYZ()) );

  //construction of the chamfer  
  ElSLib::Parameters(Pln,POnPln,UOnPln,VOnPln);
  POnPln = ElSLib::PlaneValue(UOnPln,VOnPln,AxPln);

  //construction of YDir to go from face1 to face2.
  gp_Vec YDir(POnPln,POnCyl);
  if (!plandab){ 
    YDir.Reverse();
  }
  gp_Ax3 AxCh(POnPln,XDir.Crossed(YDir),XDir);

  Handle(Geom_Plane) Chamfer = new Geom_Plane(AxCh);
  Data->ChangeSurf(ChFiKPart_IndexSurfaceInDS(Chamfer,DStr));

  // FaceInterferences are loaded with pcurves and curves 3d.
     //----------- edge plan-Chamfer
  gp_Pnt2d PPln2d(UOnPln,VOnPln);
  gp_Dir2d VPln2d(XDir.Dot(AxPln.XDirection()),
		  XDir.Dot(AxPln.YDirection()));
  gp_Lin2d Lin2dPln(PPln2d,VPln2d);

  POnPln = ElSLib::Value(UOnPln,VOnPln,Pln);
  gp_Lin   C3d(POnPln,XDir);

  Standard_Real U,VOnChamfer;
  ElSLib::PlaneParameters(AxCh,POnPln,U,VOnChamfer);
  gp_Lin2d LOnChamfer(gp_Pnt2d(U,VOnChamfer),gp::DX2d());

  Handle(Geom_Line)   L3d  = new Geom_Line  (C3d);
  Handle(Geom2d_Line) LFac = new Geom2d_Line(Lin2dPln);
  Handle(Geom2d_Line) LFil = new Geom2d_Line(LOnChamfer);

  gp_Dir NorFil=AxCh.Direction();
  Standard_Boolean toreverse = ( NorFil.Dot(NorPln) <= 0. );
  gp_Dir DirPlnCyl(gp_Vec(POnPln, POnCyl));
  gp_Dir DirSPln(gp_Vec(OrSpine, POnPln));
  Standard_Boolean PosChamfPln = DirPlnCyl.Dot(DirSPln) > 0;

  if ( !IsDisOnP && PosChamfPln )
    toreverse = !toreverse;
  // It is checked if the orientation of the Chamfer is the same as of the plane
  if (toreverse)
    {Data->ChangeOrientation() = TopAbs::Reverse(Ofpl);}
  else          
    {Data->ChangeOrientation() = Ofpl;}

  TopAbs_Orientation trans = TopAbs_FORWARD;
  if ((!plandab && toreverse) || (plandab && !toreverse))
    {trans=TopAbs_REVERSED;}
  
  //trans allows to determine the "material" side on S1(2) limited by L3d
  if (plandab) 
    {Data->ChangeInterferenceOnS1().
     SetInterference(ChFiKPart_IndexCurveInDS(L3d,DStr),trans,LFac,LFil);}
  else    
    {Data->ChangeInterferenceOnS2().
     SetInterference(ChFiKPart_IndexCurveInDS(L3d,DStr),trans,LFac,LFil);}
  
     //------------edge cylindre-Chamfer	
  gp_Pnt2d PCyl2d(UOnCyl,VOnCyl);
  gp_Dir2d VCyl2d=gp::DY2d();
  if ( XDir.Dot(AxCyl.Direction())<0 )
    {VCyl2d.Reverse();}
  gp_Lin2d Lin2dCyl(PCyl2d,VCyl2d);

  POnCyl = ElSLib::Value(UOnCyl,VOnCyl,Cyl);
  C3d = gp_Lin(POnCyl,XDir);

  ElSLib::PlaneParameters(AxCh,POnCyl,U,VOnChamfer);
  LOnChamfer = gp_Lin2d(gp_Pnt2d(U,VOnChamfer),gp::DX2d());

  L3d  = new Geom_Line  (C3d);
  LFac = new Geom2d_Line(Lin2dCyl);
  LFil = new Geom2d_Line(LOnChamfer);

  gp_Vec deru,derv;	       
  ElSLib::CylinderD1(UOnCyl,VOnCyl,AxCyl,Cyl.Radius(),POnCyl,deru,derv);
  gp_Dir NorCyl(deru.Crossed(derv));	
  gp_Dir DirSCyl(gp_Vec(OrSpine, POnCyl));
  Standard_Boolean PosChamfCyl = DirPlnCyl.Dot(DirSCyl) < 0;
  toreverse = ( NorFil.Dot(NorCyl) <= 0. );


  if ( IsDisOnP && PosChamfCyl) 
     toreverse = !toreverse; 
  trans = TopAbs_REVERSED;
  if ((!plandab && toreverse) || (plandab && !toreverse))
    {trans=TopAbs_FORWARD;}

  if (plandab) 
    Data->ChangeInterferenceOnS2().
    SetInterference(ChFiKPart_IndexCurveInDS(L3d,DStr),trans,LFac,LFil);
  else    
    Data->ChangeInterferenceOnS1().
    SetInterference(ChFiKPart_IndexCurveInDS(L3d,DStr),trans,LFac,LFil);
  return Standard_True;
} 	







