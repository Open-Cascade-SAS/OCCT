// Created on: 1998-06-18
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
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>

//=======================================================================
//function : MakeChAsym
//purpose  : Compute the chamfer in the particular case Plane/Cone or 
//           Cylinder/Plane
//           Compute the SurfData <Data> of the chamfer build on the <Spine>
//           between the plane <Pln> and the cone <Con>, with the 
//           distances <Dis1> on <Pln> and <Dis2> on <Con>.
//           <Or1> and <Or2> are the orientations of <Pln> and <Con>
//           and <Ofpl> this of the face carried by <Pln>.
//           <First> is the start point on the <Spine>
//           <Plandab> is equal to True if the plane is the surface S1
//           <fu> and <lu> are the first and last u parameters of the
//           cone
//out      : True if the chanfer has been computed
//           False else
//=======================================================================
Standard_Boolean ChFiKPart_MakeChAsym(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& Pln, 
				      const gp_Cone& Con, 
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
  // Compute the chamfer surface(cone)
  gp_Ax3 PosPl = Pln.Position();
  gp_Dir Dpl = PosPl.XDirection().Crossed(PosPl.YDirection());
  gp_Dir norf = Dpl;
  if (Ofpl == TopAbs_REVERSED ) norf.Reverse();
  if ( Or1 == TopAbs_REVERSED ) Dpl.Reverse();

  // compute the origin of the conical chamfer PtPl
  gp_Pnt Or = Con.Location();
  Standard_Real u,v;
  ElSLib::PlaneParameters(PosPl,Or,u,v);
  gp_Pnt2d pt2dPln(u,v);
  ElSLib::PlaneD0(u,v,PosPl,Or);
  gp_Pnt PtPl = Or;

  gp_Pnt PtSp;
  gp_Vec DSp;
  ElCLib::D1(First,Spine,PtSp,DSp);
  gp_Dir Dx(gp_Vec(PtPl,PtSp));

  //compute the normal to the cone in PtSp
  gp_Vec deru,derv;
  gp_Pnt PtCon;
  ElSLib::Parameters(Con,PtSp,u,v);
  ElSLib::D1(u,v,Con,PtCon ,deru,derv);
  gp_Dir Dcon( deru.Crossed(derv) );
  if ( Or2 == TopAbs_REVERSED ) Dcon.Reverse();
  
  Standard_Boolean dedans = ( Dx.Dot(Dcon) <= 0.);
  Standard_Boolean ouvert = ( Dpl.Dot(Dcon) >= 0.);

    // variables used to compute the semiangle of the chamfer
  Standard_Real angCon = Con.SemiAngle();
  Standard_Real move;
  Standard_Real ChamfRad,SemiAngl;
  Standard_Boolean pointu = Standard_False;
  Standard_Real dis;
  Standard_Boolean iscylinder = Standard_False;
  Standard_Boolean isConPar   = Standard_False;

  if ( (plandab && DisOnP) || (!plandab && !DisOnP) ) {
    Standard_Real tgang = Tan(Angle), Dis11;
    Standard_Real tgCon = Abs(Tan(angCon));
    if (ouvert) { 
      move  = Dis * tgang / (1. - tgCon * tgang);
      Dis11 = move * tgCon;
      dis   = Dis + Dis11;
    }
    else {
      move  = Dis * tgang / (1. + tgCon * tgang);
      Dis11 = move * tgCon;
      dis   = Dis - Dis11;
    }

    // compute the parameters of the conical chamfer
    if (dedans) {
      ChamfRad = Spine.Radius() - Dis;
      if ( Abs(ChamfRad) < Precision::Confusion() ) pointu = Standard_True;
      if( ChamfRad < 0 ) {
#ifdef OCCT_DEBUG
	cout<<"the chamfer can't pass"<<endl;
#endif
	return Standard_False;
      }
      SemiAngl = M_PI / 2. - Angle;      
    }
    else {
      ChamfRad = Spine.Radius() + Dis;
      SemiAngl = Angle - M_PI / 2.;
    }

    if (ouvert) {
      if (Abs(angCon) - Abs(SemiAngl) > -Precision::Confusion() ) {
#ifdef OCCT_DEBUG
	cout<<"wrong choice of angle for the chamfer"<<endl;
#endif
	return Standard_False; 
      } 
    }
  } 
  else {
    Standard_Real Dis1;
    move = Dis * Cos(angCon);
    if (ouvert) {
      SemiAngl =  Abs(angCon) + Angle;

      if ( (M_PI / 2. - SemiAngl) < Precision::Confusion() ) {
#ifdef OCCT_DEBUG
	cout <<"wrong choice of angle for the chamfer"<<endl;
#endif
	return Standard_False;
      }
      Dis1 =  move * Tan(SemiAngl) - Dis * Abs(Sin(angCon));

      if (!dedans) SemiAngl = -SemiAngl;

    }
    else {
      SemiAngl =  Abs(angCon) - Angle;

      if ( Abs(SemiAngl) < Precision::Confusion() ) {
        iscylinder = Standard_True;
        Dis1       = Dis * Abs(Sin(angCon));
      }
      else {        
        Dis1 = Dis * Abs(Sin(angCon)) - move * Tan(SemiAngl);
      }

      if (SemiAngl > Precision::Confusion())
        isConPar = Standard_True;

      if (dedans) SemiAngl = -SemiAngl;

    }

    // compute the parameters of the conical chamfer
    if (dedans) {
       ChamfRad = Spine.Radius() - Dis1;

      if ( Abs(ChamfRad) < Precision::Confusion() ) pointu = Standard_True;
      if( ChamfRad < 0 ) {
#ifdef OCCT_DEBUG
	cout<<"the chamfer can't pass"<<endl;
#endif
	return Standard_False;
      }
    }
    else {
      ChamfRad = Spine.Radius() + Dis1;
      //Dpl.Reverse();
    }

    if (ouvert) 
      dis = Dis1 + Dis * Abs(Sin(angCon));
    else 
      dis = Dis1 - Dis * Abs(Sin(angCon));
  }

  Or.SetCoord( Or.X()+ move*Dpl.X(),
	       Or.Y()+ move*Dpl.Y(),
	       Or.Z()+ move*Dpl.Z());
  gp_Pnt Pt(Or.X()+dis*PosPl.XDirection().X(),
	    Or.Y()+dis*PosPl.XDirection().Y(),
	    Or.Z()+dis*PosPl.XDirection().Z());

  gp_Ax3 ChamfAx3(PtPl,Dpl,Dx);


  if (iscylinder) {
    Handle (Geom_CylindricalSurface)
      gcyl = new Geom_CylindricalSurface( ChamfAx3, ChamfRad );
    
    // changes due to the fact the parameters of the chamfer must go increasing
    // from surface S1 to surface S2
    if (!plandab) {
      gcyl->VReverse();// be carefull : the SemiAngle was changed
      ChamfAx3 = gcyl->Position();
    }

    // changes due to the fact we have reversed the V direction of 
    // parametrization
    if (ChamfAx3.YDirection().Dot(DSp) <= 0.) {
      ChamfAx3.YReverse();
      gcyl->SetPosition(ChamfAx3);
    }

    Data->ChangeSurf(ChFiKPart_IndexSurfaceInDS(gcyl,DStr));

    Standard_Boolean torevcha = !ChamfAx3.Direct();
    gp_Dir cylaxe = (ChamfAx3.Axis()).Direction();
    torevcha = ( (torevcha  && !plandab) || (!torevcha  && plandab));

    if (torevcha) cylaxe.Reverse();
    Standard_Boolean toreverse = (norf.Dot(cylaxe) < 0.);
   
    if ((toreverse && dedans) || (!toreverse && !dedans))
      Data->ChangeOrientation() = TopAbs_REVERSED;
    else
      Data->ChangeOrientation() = TopAbs_FORWARD; 
        
    //we load the faceInterference with the pcurves and
    // the 3d curves
    
    // Case of the plane face
    // NB: in the case 'pointu', no pcurve on the plane surface
    // and no intersection plane-chamfer are needed
    
    // intersection plane-chamfer
    Handle(Geom_Circle) GCirPln;
    Handle(Geom2d_Circle) GCir2dPln;
    gp_Ax2 CirAx2 = ChamfAx3.Ax2();
    CirAx2.SetLocation(PtPl);

    Pt.SetCoord(PtPl.X()+ChamfRad*Dx.X(),
		PtPl.Y()+ChamfRad*Dx.Y(),
		PtPl.Z()+ChamfRad*Dx.Z());
    gp_Circ CirPln(CirAx2,ChamfRad);
    GCirPln = new Geom_Circle(CirPln);
    
    //pcurve on the plane
    ElSLib::PlaneParameters(PosPl,Pt ,u,v);
    gp_Pnt2d p2dPln(u,v);
    gp_Dir2d d2d(DSp.Dot(PosPl.XDirection()),DSp.Dot(PosPl.YDirection()));
    gp_Ax22d ax2dPln(pt2dPln, gp_Dir2d(gp_Vec2d(pt2dPln,p2dPln)),d2d);
    gp_Circ2d cir2dPln(ax2dPln,ChamfRad);
    GCir2dPln = new Geom2d_Circle(cir2dPln);
    
    //pcurve on chamfer
    gp_Pnt2d p2dch;
    p2dch.SetCoord(0.,0.);
    // ElSLib::CylinderD1(0.,0.,ChamfAx3,ChamfRad,Pt,deru,derv);
    gp_Lin2d lin2dch(p2dch,gp::DX2d());
    Handle(Geom2d_Line) GLin2dCh1 = new Geom2d_Line(lin2dch);
    
    //orientation
    TopAbs_Orientation trans; 
    gp_Dir norpl = PosPl.XDirection().Crossed(PosPl.YDirection());
    toreverse = (norpl.Dot(cylaxe) < 0.);

   toreverse = (toreverse && plandab) || (!toreverse && !plandab);
 
   if ((toreverse && dedans) || (!toreverse && !dedans)) { 
      trans = TopAbs_FORWARD;
    }
    else { 
      trans = TopAbs_REVERSED; 
    }

    if (plandab) { 
      Data->ChangeInterferenceOnS1().
	SetInterference(ChFiKPart_IndexCurveInDS(GCirPln,DStr),
			trans,GCir2dPln,GLin2dCh1);
    }
    else {
      Data->ChangeInterferenceOnS2().
	SetInterference(ChFiKPart_IndexCurveInDS(GCirPln,DStr),
			trans,GCir2dPln,GLin2dCh1);
    }

    // Case of the conical face
    
    //intersection cone-chamfer
    Standard_Real Rad;
    if (dedans)
      Rad = ChamfRad + dis;
    else
      Rad = ChamfRad - dis;
    
    CirAx2.SetLocation(Or);
    gp_Circ CirCon(CirAx2, Rad);
    Handle(Geom_Circle) GCirCon = new Geom_Circle(CirCon);  
    
    //pcurve on chamfer
    if (plandab)
      v = sqrt(dis*dis + move*move);
    else
      v = - sqrt(dis*dis + move*move);
    p2dch.SetCoord(0.,v);
    ElSLib::CylinderD1(0.,v,ChamfAx3,ChamfRad,Pt,deru,derv);
    lin2dch.SetLocation(p2dch);
    Handle(Geom2d_Line) GLin2dCh2 = new Geom2d_Line(lin2dch);
    
    //pcurve on cone
    Pt.SetCoord(Or.X()+Rad*Dx.X(),
		Or.Y()+Rad*Dx.Y(),
		Or.Z()+Rad*Dx.Z());
    ElSLib::Parameters(Con,Pt ,u,v);
    Standard_Real tol = Precision::PConfusion();
    if(u >= 2*M_PI - tol && u <= 2*M_PI) u = 0.;
    if(u >= fu - tol && u < fu) u = fu;
    if(u <= lu + tol && u > lu) u = lu;
    if(u < fu || u > lu) u = ElCLib::InPeriod(u,fu,fu + 2*M_PI);
    ElSLib::D1(u,v,Con,Pt,deru,derv);
    gp_Pnt2d p2dCon(u,v);
    gp_Dir2d d2dCon;
    if ( deru.Dot(DSp)<=0. )
      d2dCon = - gp::DX2d();
    else
      d2dCon = gp::DX2d();
    gp_Lin2d lin2dCon(p2dCon,d2dCon);
    Handle(Geom2d_Line) GLin2dCon = new Geom2d_Line(lin2dCon);
    
    //orientation
    gp_Dir norcon = deru.Crossed(derv);

    gp_Dir DirCon   = (Con.Axis()).Direction();
    if (angCon > Precision::Confusion()) DirCon.Reverse();
    Standard_Boolean torevcon = ( norcon.Dot(DirCon) < 0. );

    if ((torevcon && dedans) || (!torevcon && !dedans) ) {
      trans = TopAbs_REVERSED;
    }
    else {
      trans = TopAbs_FORWARD;
    }
    
    if(plandab){
      Data->ChangeInterferenceOnS2().
	SetInterference(ChFiKPart_IndexCurveInDS(GCirCon,DStr),
			trans,GLin2dCon,GLin2dCh2);
    }
    else {
      Data->ChangeInterferenceOnS1().
	SetInterference(ChFiKPart_IndexCurveInDS(GCirCon,DStr),
			trans,GLin2dCon,GLin2dCh2);
    }
  }
  else { 
    Handle (Geom_ConicalSurface)
      gcon = new Geom_ConicalSurface( ChamfAx3, SemiAngl, ChamfRad );
    
    // changes due to the fact the parameters of the chamfer must go increasing
    // from surface S1 to surface S2
    if (!plandab) {
      gcon->VReverse();// be carefull : the SemiAngle was changed
      ChamfAx3 = gcon->Position();
      SemiAngl = gcon->SemiAngle();
    }

    // changes due to the fact we have reversed the V direction of 
    // parametrization
    if (ChamfAx3.YDirection().Dot(DSp) <= 0.) {
      ChamfAx3.YReverse();
      gcon->SetPosition(ChamfAx3);
    }

    Data->ChangeSurf(ChFiKPart_IndexSurfaceInDS(gcon,DStr));

    //compute the chamfer's orientation according to the orientation
    // of the faces
    
    //search the normal to the conical chamfer
    gp_Pnt P;
    u = 0.;
    if (plandab)
      v = sqrt(dis*dis + move*move);
    else
      v = - sqrt(dis*dis + move*move);

    ElSLib::ConeD1(u,v,ChamfAx3,ChamfRad,SemiAngl,P,deru,derv);
    gp_Dir norchamf(deru.Crossed(derv));
    
    Standard_Boolean toreverse = (norf.Dot(norchamf) < 0.);

    if (isConPar)  toreverse = !toreverse;
    
    if (toreverse) 
      Data->ChangeOrientation() = TopAbs_REVERSED;
    else
      Data->ChangeOrientation() = TopAbs_FORWARD;

    //we load the faceInterference with the pcurves and
    // the 3d curves
    
    // Case of the plane face
    // NB: in the case 'pointu', no pcurve on the plane surface
    // and no intersection plane-chamfer are needed
    
    // intersection plane-chamfer
    Handle(Geom_Circle) GCirPln;
    Handle(Geom2d_Circle) GCir2dPln;
    gp_Ax2 CirAx2 = ChamfAx3.Ax2();
    CirAx2.SetLocation(PtPl);
    
    if (!pointu) {
      Pt.SetCoord(PtPl.X()+ChamfRad*Dx.X(),
		  PtPl.Y()+ChamfRad*Dx.Y(),
		  PtPl.Z()+ChamfRad*Dx.Z());
      gp_Circ CirPln(CirAx2,ChamfRad);
      GCirPln = new Geom_Circle(CirPln);
      
      //pcurve on the plane
      ElSLib::PlaneParameters(PosPl,Pt ,u,v);
      gp_Pnt2d p2dPln(u,v);
      gp_Dir2d d2d(DSp.Dot(PosPl.XDirection()),DSp.Dot(PosPl.YDirection()));
      gp_Ax22d ax2dPln(pt2dPln, gp_Dir2d(gp_Vec2d(pt2dPln,p2dPln)),d2d);
      gp_Circ2d cir2dPln(ax2dPln,ChamfRad);
      GCir2dPln = new Geom2d_Circle(cir2dPln);
    }

    //pcurve on chamfer
    gp_Pnt2d p2dch;
    p2dch.SetCoord(0.,0.);
    ElSLib::ConeD1(0.,0.,ChamfAx3,ChamfRad,SemiAngl,Pt,deru,derv);
    gp_Lin2d lin2dch(p2dch,gp::DX2d());
    Handle(Geom2d_Line) GLin2dCh1 = new Geom2d_Line(lin2dch);
    
    //orientation
    TopAbs_Orientation trans; 
    gp_Dir norpl = PosPl.XDirection().Crossed(PosPl.YDirection());
    if (!pointu)
      norchamf.SetXYZ (deru.Crossed(derv).XYZ());
    toreverse = ( norchamf.Dot(norpl) <= 0. );

    if (isConPar)  toreverse = !toreverse;

    if ((toreverse && plandab) || (!toreverse && !plandab)){ 
      trans = TopAbs_FORWARD;
    }
    else { 
      trans = TopAbs_REVERSED; 
    }

    if (plandab) { 
      Data->ChangeInterferenceOnS1().
	SetInterference(ChFiKPart_IndexCurveInDS(GCirPln,DStr),
			trans,GCir2dPln,GLin2dCh1);
    }
    else {
      Data->ChangeInterferenceOnS2().
	SetInterference(ChFiKPart_IndexCurveInDS(GCirPln,DStr),
			trans,GCir2dPln,GLin2dCh1);
    }

    // Case of the conical face
    
    //intersection cone-chamfer
    Standard_Real Rad;
    if (dedans)
      Rad = ChamfRad + dis;
    else
      Rad = ChamfRad - dis;
    
    CirAx2.SetLocation(Or);
    gp_Circ CirCon(CirAx2, Rad);
    Handle(Geom_Circle) GCirCon = new Geom_Circle(CirCon);  
    
    //pcurve on chamfer
    if (plandab)
      v = sqrt(dis*dis + move*move);
    else
      v = - sqrt(dis*dis + move*move);
    p2dch.SetCoord(0.,v);
    ElSLib::ConeD1(0.,v,ChamfAx3,ChamfRad,SemiAngl,Pt,deru,derv);
    lin2dch.SetLocation(p2dch);
    Handle(Geom2d_Line) GLin2dCh2 = new Geom2d_Line(lin2dch);
    
    //pcurve on cone
    norchamf.SetXYZ (deru.Crossed(derv).XYZ());

    Pt.SetCoord(Or.X()+Rad*Dx.X(),
		Or.Y()+Rad*Dx.Y(),
		Or.Z()+Rad*Dx.Z());
    ElSLib::Parameters(Con,Pt ,u,v);
    Standard_Real tol = Precision::PConfusion();
    if (u >= 2*M_PI - tol && u <= 2*M_PI) u = 0.;
    if (u >= fu - tol && u < fu) u = fu;
    if (u <= lu + tol && u > lu) u = lu;
    if (u < fu || u > lu) u = ElCLib::InPeriod(u,fu,fu + 2*M_PI);
    ElSLib::D1(u,v,Con,Pt,deru,derv);
    gp_Pnt2d p2dCon(u,v);
    gp_Dir2d d2dCon;
    if ( deru.Dot(DSp)<=0. )
      d2dCon = - gp::DX2d();
    else
      d2dCon = gp::DX2d();
    gp_Lin2d lin2dCon(p2dCon,d2dCon);
    Handle(Geom2d_Line) GLin2dCon = new Geom2d_Line(lin2dCon);
    
    //orientation
    gp_Dir norcon = deru.Crossed(derv);

    gp_Dir DirCon   = (Con.Axis()).Direction();
    gp_Dir DirChamf = (gcon->Axis()).Direction();
    if (angCon > Precision::Confusion()) DirCon.Reverse();
    if (SemiAngl > Precision::Confusion()) DirChamf.Reverse();   

    Standard_Boolean torevcon = ( norcon.Dot(DirCon) > 0. );
    Standard_Boolean torevcha = ( norchamf.Dot(DirChamf) > 0. );

    toreverse = ( (torevcon && !torevcha) || (!torevcon && torevcha) );

    if ((toreverse && plandab) || (!toreverse && !plandab) ) {
      trans = TopAbs_REVERSED;
    }
    else {
      trans = TopAbs_FORWARD;
    } 

    if(plandab){
      Data->ChangeInterferenceOnS2().
	SetInterference(ChFiKPart_IndexCurveInDS(GCirCon,DStr),
			trans,GLin2dCon,GLin2dCh2);
    }
    else {
      Data->ChangeInterferenceOnS1().
	SetInterference(ChFiKPart_IndexCurveInDS(GCirCon,DStr),
			trans,GLin2dCon,GLin2dCh2);
    }
    
  }    
  return Standard_True;
}




