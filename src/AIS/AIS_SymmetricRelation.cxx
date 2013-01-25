// Created on: 1997-03-03
// Created by: Jean-Pierre COMBE
// Copyright (c) 1997-1999 Matra Datavision
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


#define BUC60915        //GG 05/06/01 Enable to compute the requested arrow size
//                      if any in all dimensions.

#include <Standard_NotImplemented.hxx>

#include <AIS_SymmetricRelation.ixx>
#include <AIS.hxx>
#include <AIS_Drawer.hxx>

#include <gce_MakeLin.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <ElCLib.hxx>
#include <gp_Pln.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>

#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <TopExp_Explorer.hxx>

#include <Precision.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LengthAspect.hxx>

#include <DsgPrs_SymmetricPresentation.hxx>

//=======================================================================
//function : AIS_SymmetricRelation
//purpose  : 
//=======================================================================
AIS_SymmetricRelation::AIS_SymmetricRelation(const TopoDS_Shape& aSymmTool, 
					     const TopoDS_Shape& FirstShape, 
					     const TopoDS_Shape& SecondShape, 
					     const Handle(Geom_Plane)& aPlane)
:AIS_Relation(),
 myTool(aSymmTool)
{
 SetFirstShape(FirstShape);
 SetSecondShape(SecondShape);
 SetPlane(aPlane);
 myPosition = aPlane->Pln().Location();
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_SymmetricRelation::Compute(const Handle(PrsMgr_PresentationManager3d)&, 
				    const Handle(Prs3d_Presentation)& aprs, 
				    const Standard_Integer)
{
  aprs->Clear();

  switch (myFShape.ShapeType()) {
  case TopAbs_FACE :
    {
      // cas symetrie entre deux faces
      ComputeTwoFacesSymmetric(aprs);
    }
    break;
  case TopAbs_EDGE :
    {
      // cas symetrie entre deux edges
      ComputeTwoEdgesSymmetric(aprs);
    }
    break;
  case TopAbs_VERTEX :
    {
      // cas symetrie entre deux vertexs
      ComputeTwoVerticesSymmetric(aprs);
    }
    break;
  default:
    break;
  }
  if (myTool.ShapeType() == TopAbs_EDGE) {
    Handle(Geom_Curve) aCurve,extcurve;
    gp_Pnt p1,p2;
    Standard_Boolean isinfinite,isonplane;
    if (AIS::ComputeGeometry(TopoDS::Edge(myTool),
			     aCurve,p1,p2,
			     extcurve,
			     isinfinite,
			     isonplane,
			     myPlane)) {
      if (!extcurve.IsNull()) { 
	gp_Pnt pf, pl;
	if (!isinfinite) {
	  pf = p1; 
	  pl = p2;
	}
	if (isinfinite) aprs->SetInfiniteState(Standard_True);
	ComputeProjEdgePresentation(aprs,TopoDS::Edge(myTool),aCurve,pf,pl);
      }
    }
  }
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning at compilation (SUN)
//=======================================================================
void AIS_SymmetricRelation::Compute(const Handle(Prs3d_Projector)& /*aProjector*/,
				    const Handle(Prs3d_Presentation)& /*aPresentation*/)
{
// Standard_NotImplemented::Raise("AIS_SymmetricRelation::Compute(const Handle(Prs3d_Projector)&,const Handle(Prs3d_Presentation)&)");
// PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

void AIS_SymmetricRelation::Compute(const Handle_Prs3d_Projector& aProjector,
                                    const Handle_Geom_Transformation& aTransformation,
                                    const Handle_Prs3d_Presentation& aPresentation)
{
 Standard_NotImplemented::Raise("AIS_SymmetricRelation::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_SymmetricRelation::ComputeSelection(const Handle(SelectMgr_Selection)& aSel, 
					     const Standard_Integer)
{
  Handle(Select3D_SensitiveSegment) seg;
  Handle(SelectMgr_EntityOwner) own = new SelectMgr_EntityOwner(this,7);
  Standard_Real F,L;

  Handle(Geom_Line) geom_axis,extcurve;
  gp_Pnt p1,p2;
  Standard_Boolean isinfinite,isonplane;
  if (!AIS::ComputeGeometry(TopoDS::Edge(myTool),
			    geom_axis,p1,p2,
			    extcurve,
			    isinfinite,
			    isonplane,
			    myPlane)) return;

  gp_Lin laxis (geom_axis->Lin());
  
  if(myFShape.ShapeType() != TopAbs_VERTEX){
    BRepAdaptor_Curve cu1(TopoDS::Edge(myFShape));
    
    if(cu1.GetType() == GeomAbs_Line) {
//      gp_Lin L1 (myFAttach,myFDirAttach);
      gp_Pnt PjAttachPnt1  = ElCLib::Value(ElCLib::Parameter(laxis,myFAttach),laxis);
      gp_Pnt PjOffSetPnt   = ElCLib::Value(ElCLib::Parameter(laxis,myPosition),laxis);
      Standard_Real h = fabs(PjOffSetPnt.Distance(PjAttachPnt1)/cos(myAxisDirAttach.Angle(myFDirAttach)));
      gp_Vec VL1(myFDirAttach);
      gp_Vec VLa(PjAttachPnt1,PjOffSetPnt);
      Standard_Real scal = VL1.Dot(VLa);
      if(scal < 0) VL1.Reverse();
      VL1.Multiply(h);
      gp_Pnt P1 = myFAttach.Translated(VL1);
      gp_Pnt ProjAxis = ElCLib::Value(ElCLib::Parameter(laxis,P1),laxis);
      gp_Vec v(P1,ProjAxis);
      gp_Pnt P2 = ProjAxis.Translated(v);
      
      gp_Lin L3;
      
      if (!P1.IsEqual(P2,Precision::Confusion())) {
	L3 = gce_MakeLin(P1,P2);
      }
      else {
	L3 = gce_MakeLin(P1,myFDirAttach);
	Standard_Real size(Min(myVal/100.+1.e-6,myArrowSize+1.e-6));
	Handle(Select3D_SensitiveBox) box =
	  new Select3D_SensitiveBox(own,
				    myPosition.X(),
				    myPosition.Y(),
				    myPosition.Z(),
				    myPosition.X()+size,
				    myPosition.Y()+size,
				    myPosition.Z()+size);
	aSel->Add(box);
      }
      Standard_Real parmin,parmax,parcur;
      parmin = ElCLib::Parameter(L3,P1);
      parmax = parmin;
      
      parcur = ElCLib::Parameter(L3,P2);
      parmin = Min(parmin,parcur);
      parmax = Max(parmax,parcur);
      
      parcur = ElCLib::Parameter(L3,myPosition);
      parmin = Min(parmin,parcur);
      parmax = Max(parmax,parcur);
      
      gp_Pnt PointMin = ElCLib::Value(parmin,L3);
      gp_Pnt PointMax = ElCLib::Value(parmax,L3);
      
      if (!PointMin.IsEqual(PointMax,Precision::Confusion())) {
	seg = new Select3D_SensitiveSegment(own,
					    PointMin,
					    PointMax);
	aSel->Add(seg);
      }
      if (!myFAttach.IsEqual(P1,Precision::Confusion())) {
	seg = new Select3D_SensitiveSegment(own,
					    myFAttach,
					    P1);
	aSel->Add(seg);
      }
      if (!mySAttach.IsEqual(P2,Precision::Confusion())) {
	seg = new Select3D_SensitiveSegment(own,
					    mySAttach,
					    P2);
	aSel->Add(seg);
      }
    }
    
    //=======================Pour les arcs======================    
  if(cu1.GetType() == GeomAbs_Circle) { 
    BRep_Tool::Curve(TopoDS::Edge(myFShape),F,L);
//    Handle(Geom_Circle) geom_circ1 = (Handle(Geom_Circle)&) BRep_Tool::Curve(TopoDS::Edge(myFShape),F,L);
//JR/Hp
    Handle(Geom_Curve) aGeomCurve = BRep_Tool::Curve(TopoDS::Edge(myFShape),F,L);
    Handle(Geom_Circle) geom_circ1 = (Handle(Geom_Circle)&) aGeomCurve ;
//    Handle(Geom_Circle) geom_circ1 = (const Handle(Geom_Circle)&) BRep_Tool::Curve(TopoDS::Edge(myFShape),F,L);
    gp_Circ circ1(geom_circ1->Circ());
    gp_Pnt OffsetPnt(myPosition.X(),myPosition.Y(),myPosition.Z());
    gp_Pnt Center1 = circ1.Location();
    gp_Pnt ProjOffsetPoint = ElCLib::Value(ElCLib::Parameter(laxis,OffsetPnt),laxis);
    gp_Pnt ProjCenter1     = ElCLib::Value(ElCLib::Parameter(laxis,Center1),laxis);
    gp_Vec Vp(ProjCenter1,Center1);
    if (Vp.Magnitude() <= Precision::Confusion()) Vp = gp_Vec(laxis.Direction())^myPlane->Pln().Position().Direction();
    Standard_Real Dt,R,h;
    Dt = ProjCenter1.Distance(ProjOffsetPoint);
    R  = circ1.Radius();
    if (Dt > .999*R) {
      Dt = .999*R;
      gp_Vec Vout(ProjCenter1,ProjOffsetPoint);
      ProjOffsetPoint = ProjCenter1.Translated(Vout.Divided(Vout.Magnitude()).Multiplied(Dt));
      OffsetPnt = ProjOffsetPoint;
    }
    h  = Sqrt(R*R - Dt*Dt);
    gp_Pnt P1 = ProjOffsetPoint.Translated(Vp.Added(Vp.Divided(Vp.Magnitude()).Multiplied(h)));
    gp_Vec v(P1,ProjOffsetPoint);
    gp_Pnt P2 = ProjOffsetPoint.Translated(v);
    
    gp_Lin L3;
    if (!P1.IsEqual(P2,Precision::Confusion())) {
      L3 = gce_MakeLin(P1,P2);
    }
    else {
      L3 = gce_MakeLin(P1,laxis.Direction());
      Standard_Real size(Min(myVal/100.+1.e-6,myArrowSize+1.e-6));
      Handle(Select3D_SensitiveBox) box =
	new Select3D_SensitiveBox(own,
				  myPosition.X(),
				  myPosition.Y(),
				  myPosition.Z(),
				  myPosition.X()+size,
				  myPosition.Y()+size,
				  myPosition.Z()+size);
      aSel->Add(box);
    }
    Standard_Real parmin,parmax,parcur;
    parmin = ElCLib::Parameter(L3,P1);
    parmax = parmin;
    
    parcur = ElCLib::Parameter(L3,P2);
    parmin = Min(parmin,parcur);
    parmax = Max(parmax,parcur);
    
    parcur = ElCLib::Parameter(L3,myPosition);
    parmin = Min(parmin,parcur);
    parmax = Max(parmax,parcur);
    
    gp_Pnt PointMin = ElCLib::Value(parmin,L3);
    gp_Pnt PointMax = ElCLib::Value(parmax,L3);
    
    if (!PointMin.IsEqual(PointMax,Precision::Confusion())) {
      seg = new Select3D_SensitiveSegment(own,
					  PointMin,
					  PointMax);
      aSel->Add(seg);
    }
  }
  }
  //=======================Pour les points======================
  else {
    if (myFAttach.IsEqual(mySAttach,Precision::Confusion())) {
      seg = new Select3D_SensitiveSegment(own,myPosition,myFAttach);
      aSel->Add(seg);
    }
    else{
      gp_Pnt ProjOffsetPoint      = ElCLib::Value(ElCLib::Parameter(laxis,myPosition),laxis);
      gp_Pnt ProjAttachmentPoint1 = ElCLib::Value(ElCLib::Parameter(laxis,myFAttach),laxis);
      gp_Vec PjAtt1_Att1(ProjAttachmentPoint1,myFAttach);
      gp_Pnt P1 = ProjOffsetPoint.Translated(PjAtt1_Att1);
      gp_Pnt P2 = ProjOffsetPoint.Translated(PjAtt1_Att1.Reversed());
      gp_Lin L3;
      
      if (!P1.IsEqual(P2,Precision::Confusion())) {
	L3 = gce_MakeLin(P1,P2);
      }
      else {
	L3 = gce_MakeLin(P1,myFDirAttach);
	Standard_Real size(Min(myVal/100.+1.e-6,myArrowSize+1.e-6));
	Handle(Select3D_SensitiveBox) box =
	  new Select3D_SensitiveBox(own,
				    myPosition.X(),
				    myPosition.Y(),
				    myPosition.Z(),
				    myPosition.X()+size,
				    myPosition.Y()+size,
				    myPosition.Z()+size);
	aSel->Add(box);
      }
      Standard_Real parmin,parmax,parcur;
      parmin = ElCLib::Parameter(L3,P1);
      parmax = parmin;
      
      parcur = ElCLib::Parameter(L3,P2);
      parmin = Min(parmin,parcur);
      parmax = Max(parmax,parcur);
      
      parcur = ElCLib::Parameter(L3,myPosition);
      parmin = Min(parmin,parcur);
      parmax = Max(parmax,parcur);
      
      gp_Pnt PointMin = ElCLib::Value(parmin,L3);
      gp_Pnt PointMax = ElCLib::Value(parmax,L3);
      
      if (!PointMin.IsEqual(PointMax,Precision::Confusion())) {
	seg = new Select3D_SensitiveSegment(own,PointMin,PointMax);
	aSel->Add(seg);
      }
      if (!myFAttach.IsEqual(P1,Precision::Confusion())) {
	seg = new Select3D_SensitiveSegment(own,myFAttach,P1);
	aSel->Add(seg);
      }
      if (!mySAttach.IsEqual(P2,Precision::Confusion())) {
	seg = new Select3D_SensitiveSegment(own,mySAttach,P2);
	aSel->Add(seg);
      }
    }
  }
}

//=======================================================================
//function : ComputeTwoFacesSymmetric
//purpose  : 
//=======================================================================
void AIS_SymmetricRelation::ComputeTwoFacesSymmetric(const Handle(Prs3d_Presentation)&)
{
}

//=======================================================================
//function : ComputeTwoEdgesSymmetric
//purpose  : 
//=======================================================================
void AIS_SymmetricRelation::ComputeTwoEdgesSymmetric(const Handle(Prs3d_Presentation)& aprs)
{
  BRepAdaptor_Curve cu1(TopoDS::Edge(myFShape));
  if (cu1.GetType() != GeomAbs_Line && cu1.GetType() != GeomAbs_Circle) return;
  BRepAdaptor_Curve cu2(TopoDS::Edge(mySShape));
  if (cu2.GetType() != GeomAbs_Line && cu2.GetType() != GeomAbs_Circle) return;
//  gp_Pnt pint3d,ptat11,ptat12,ptat21,ptat22;
  gp_Pnt ptat11,ptat12,ptat21,ptat22;
  Handle(Geom_Curve) geom1,geom2;
  Standard_Boolean isInfinite1,isInfinite2;
  Handle(Geom_Curve) extCurv;
  if (!AIS::ComputeGeometry(TopoDS::Edge(myFShape),
			    TopoDS::Edge(mySShape),
			    myExtShape,
			    geom1,
			    geom2,
			    ptat11,
			    ptat12,
			    ptat21,
			    ptat22,
			    extCurv,
			    isInfinite1,isInfinite2,
			    myPlane)) {
    return;
  } 
  aprs->SetInfiniteState((isInfinite1 || isInfinite2) && (myExtShape !=0));
  Handle(Geom_Line) geom_axis,extcurve;
  gp_Pnt p1,p2;
  Standard_Boolean isinfinite,isonplane;
  if (!AIS::ComputeGeometry(TopoDS::Edge(myTool),
			    geom_axis,p1,p2,
			    extcurve,
			    isinfinite,
			    isonplane,
			    myPlane)) return;

  gp_Lin laxis (geom_axis->Lin());
  myAxisDirAttach = laxis.Direction();

  if(cu1.GetType() == GeomAbs_Line){
    const Handle(Geom_Line)& geom_lin1 = (Handle(Geom_Line)&) geom1;
    gp_Lin l1(geom_lin1->Lin());
    myFDirAttach = l1.Direction();
  }
  gp_Circ circ;
  if(cu1.GetType() == GeomAbs_Circle){
    const Handle(Geom_Circle)& geom_cir1 = (Handle(Geom_Circle)&) geom1;
    gp_Circ c(geom_cir1->Circ());
    circ = c;
  }
  
  // recherche points attache
  gp_Pnt ProjOffset = ElCLib::Value(ElCLib::Parameter(laxis,myPosition),laxis);
  
/*//----------------------------------------------------
  //Quand on fait la symetrie de 2 edges consecutifs:
  //              
  //              :<-- Axe
  //              :
  //             /:\
  // Edge n --->/ : \
  //           /  :  \<-- Edge n+1
  //              :
  //----------------------------------------------------
*/
  Standard_Boolean idem = Standard_False;
  if (isInfinite1 && isInfinite2) { // geom1 et geom2 sont des lignes
    const gp_Lin& line2 = ((Handle(Geom_Line)&) geom2)->Lin();
    if (myAutomaticPosition) {
      myFAttach = ((Handle(Geom_Line)&) geom1)->Lin().Location();      
      mySAttach = ElCLib::Value(ElCLib::Parameter(line2,myFAttach),line2);
    }
    else {
      const gp_Lin& line1 = ((Handle(Geom_Line)&) geom1)->Lin();
      myFAttach = ElCLib::Value(ElCLib::Parameter(line1,myPosition),line1);
      mySAttach = ElCLib::Value(ElCLib::Parameter(line2,myFAttach),line2);
    }
  }
  else if (!isInfinite1 && !isInfinite2) {
    if (ptat11.IsEqual(ptat21,Precision::Confusion())) {
      myFAttach = ptat12;
      mySAttach = ptat22;
      idem = Standard_True;
    }
    if (ptat11.IsEqual(ptat22,Precision::Confusion())) {
      myFAttach = ptat12;
      mySAttach = ptat21;
      idem = Standard_True;
    }
    if (ptat12.IsEqual(ptat21,Precision::Confusion())) {
      myFAttach = ptat11;
      mySAttach = ptat22;
      idem = Standard_True;
    }
    if (ptat12.IsEqual(ptat22,Precision::Confusion())) {
      myFAttach = ptat11;
      mySAttach = ptat21;
      idem = Standard_True;
    }
    if(!idem){
      if( ProjOffset.SquareDistance(ptat11) > ProjOffset.SquareDistance(ptat12)) myFAttach = ptat12;
      else myFAttach = ptat11;
      
      if (ProjOffset.SquareDistance(ptat21) > ProjOffset.SquareDistance(ptat22)) mySAttach = ptat22;
      else mySAttach = ptat21;
    }
  }
  else if (isInfinite1) {// geom1 et geom2 sont des lignes
    mySAttach = ptat21;
    const gp_Lin& line1 = ((Handle(Geom_Line)&) geom1)->Lin();
    myFAttach = ElCLib::Value(ElCLib::Parameter(line1,mySAttach),line1);
  }
  else if (isInfinite2) {// geom1 et geom2 sont des lignes
    myFAttach = ptat11;
    const gp_Lin& line2 = ((Handle(Geom_Line)&) geom2)->Lin();
    mySAttach = ElCLib::Value(ElCLib::Parameter(line2,myFAttach),line2);
  }

#ifdef BUC60915
  if( !myArrowSizeIsDefined )
#endif
    myArrowSize = myFAttach.Distance(mySAttach)/50.;
  //----------------------------------------------------
 
  //----------------------------------------------------
  // Si myFAttach <> mySAttach et PjFAttach = myFAttach
  //----------------------------------------------------
  gp_Pnt PjFAttach = ElCLib::Value(ElCLib::Parameter(laxis,myFAttach),laxis); 
 
  if (PjFAttach.IsEqual(myFAttach,Precision::Confusion())){
    const Handle(Geom_Line)& geom_lin2 = (Handle(Geom_Line)&) geom2;
    gp_Lin l2(geom_lin2->Lin());
    myFDirAttach = l2.Direction();
    gp_Pnt PntTempo;
    PntTempo  = myFAttach;
    myFAttach = mySAttach;
    mySAttach = PntTempo;
    PjFAttach = ElCLib::Value(ElCLib::Parameter(laxis,myFAttach),laxis);
  }
  
  //----------------------------------------------------
//  gp_Pnt curpos;

  if (myAutomaticPosition) {    
    //gp_Pnt PjFAttach = ElCLib::Value(ElCLib::Parameter(laxis,myFAttach),laxis); 
    // offset pour eviter confusion Edge et Dimension
    gp_Vec offset(myAxisDirAttach);
    offset = offset * myArrowSize * (-5);
    gp_Vec Vt(myFAttach, PjFAttach);
    gp_Pnt curpos = PjFAttach.Translated(offset.Added(Vt.Multiplied(.15)));
    myPosition = curpos;  
  }
  
  gp_Pnt Pj1 = ElCLib::Value(ElCLib::Parameter(laxis,myFAttach),laxis);
  gp_Pnt Pj2 = ElCLib::Value(ElCLib::Parameter(laxis,mySAttach),laxis);
  if ((myFAttach.SquareDistance(Pj1)+mySAttach.SquareDistance(Pj2)) <= Precision::Confusion())  myArrowSize = 0.;
  Handle(Prs3d_LengthAspect) la = myDrawer->LengthAspect();
  Handle(Prs3d_ArrowAspect) arr = la->Arrow1Aspect();
  arr->SetLength(myArrowSize);
  arr = la->Arrow2Aspect();
  arr->SetLength(myArrowSize);
  if(cu1.GetType() == GeomAbs_Line)
    DsgPrs_SymmetricPresentation::Add(aprs,
				      myDrawer,
				      myFAttach,
				      mySAttach,
				      myFDirAttach,
				      laxis,
				      myPosition);
  
  if(cu1.GetType() == GeomAbs_Circle)
    DsgPrs_SymmetricPresentation::Add(aprs,
				      myDrawer,
				      myFAttach,
				      mySAttach,
				      circ,
				      laxis,
				      myPosition);
  if ( (myExtShape != 0) &&  !extCurv.IsNull()) {
    gp_Pnt pf, pl;
    if ( myExtShape == 1 ) {
      if (!isInfinite1) {
	pf = ptat11; 
	pl = ptat12;
      }
      ComputeProjEdgePresentation(aprs,TopoDS::Edge(myFShape),geom1,pf,pl);
    }
    else {
      if (!isInfinite2) {
	pf = ptat21; 
	pl = ptat22;
      }
      ComputeProjEdgePresentation(aprs,TopoDS::Edge(mySShape),geom2,pf,pl);
    }
  }
}

//=======================================================================
//function : ComputeTwoVertexsSymmetric
//purpose  : 
//=======================================================================
void AIS_SymmetricRelation::ComputeTwoVerticesSymmetric(const Handle(Prs3d_Presentation)& aprs)
{
  if(myFShape.ShapeType() != TopAbs_VERTEX || mySShape.ShapeType() != TopAbs_VERTEX) return;
  Handle(Geom_Line) geom_axis,extcurve;
  gp_Pnt p1,p2;
  Standard_Boolean isinfinite,isonplane;
  if (!AIS::ComputeGeometry(TopoDS::Edge(myTool),
			    geom_axis,p1,p2,
			    extcurve,
			    isinfinite,
			    isonplane,
			    myPlane)) return;

  Standard_Boolean isOnPlane1, isOnPlane2;

  AIS::ComputeGeometry(TopoDS::Vertex(myFShape), myFAttach, myPlane, isOnPlane1);
  AIS::ComputeGeometry(TopoDS::Vertex(mySShape), mySAttach, myPlane, isOnPlane2);

#ifdef BUC60915
  if( !myArrowSizeIsDefined )
#endif
    myArrowSize = myFAttach.Distance(mySAttach)/50.;
  
  if (isOnPlane1 && isOnPlane2)
    myExtShape = 0;
  else if ( isOnPlane1 && !isOnPlane2)
    myExtShape = 2;
  else if (!isOnPlane1 && isOnPlane2)
    myExtShape = 1;
  else
    return ;
  gp_Lin laxis (geom_axis->Lin());
  myAxisDirAttach = laxis.Direction();

  // recherche points attache
//  gp_Pnt curpos;
  if (myAutomaticPosition) {    
    gp_Pnt PjFAttach = ElCLib::Value(ElCLib::Parameter(laxis,myFAttach),laxis); 
    // offset pour eviter confusion Edge et Dimension
    gp_Vec offset(myAxisDirAttach);
    offset = offset * myArrowSize * (-5);
    gp_Vec Vt(myFAttach, PjFAttach);
    gp_Pnt curpos = PjFAttach.Translated(offset.Added(Vt.Multiplied(.15)));
    myPosition = curpos;
  }
  if (2*(myFAttach.Distance(mySAttach)) <= Precision::Confusion()) myArrowSize = 0.;
  Handle(Prs3d_LengthAspect) la = myDrawer->LengthAspect();
  Handle(Prs3d_ArrowAspect) arr = la->Arrow1Aspect();
  arr->SetLength(myArrowSize);
  arr = la->Arrow2Aspect();
  arr->SetLength(myArrowSize);
  DsgPrs_SymmetricPresentation::Add(aprs,
				    myDrawer,
				    myFAttach,
				    mySAttach,
				    laxis,
				    myPosition);
  if ( myExtShape == 1)
    ComputeProjVertexPresentation(aprs,TopoDS::Vertex(myFShape),myFAttach);
  else if ( myExtShape == 2)
    ComputeProjVertexPresentation(aprs,TopoDS::Vertex(mySShape),mySAttach);
}






