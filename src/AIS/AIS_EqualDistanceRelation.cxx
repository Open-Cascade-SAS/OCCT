// Created on: 1998-01-24
// Created by: Julia GERASIMOVA
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


#include <AIS_EqualDistanceRelation.hxx>
#include <AIS_LengthDimension.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <DsgPrs_EqualDistancePresentation.hxx>
#include <ElCLib.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Transformation.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_EqualDistanceRelation,AIS_Relation)

//=======================================================================
//function : AIS_EqualDistanceRelation
//purpose  : 
//=======================================================================
AIS_EqualDistanceRelation::AIS_EqualDistanceRelation( const TopoDS_Shape& aShape1,
						      const TopoDS_Shape& aShape2,
						      const TopoDS_Shape& aShape3,
						      const TopoDS_Shape& aShape4,
						      const Handle( Geom_Plane )& aPlane )
     :AIS_Relation()
{
  myFShape = aShape1;
  mySShape = aShape2;
  myShape3 = aShape3;
  myShape4 = aShape4;
  myPlane  = aPlane;

  //Temporary
  myArrowSize = 3.0; //set the concrete value
  mySymbolPrs = DsgPrs_AS_BOTHAR;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_EqualDistanceRelation::Compute( const Handle( PrsMgr_PresentationManager3d )&,
					 const Handle( Prs3d_Presentation )& aPresentation,
					 const Standard_Integer ) 
{
  gp_Pnt Position12 =  myPosition, Position34 = myPosition;

  Handle(Prs3d_DimensionAspect) la = myDrawer->DimensionAspect();
  Handle(Prs3d_ArrowAspect) arr = la->ArrowAspect();
  arr->SetLength(myArrowSize);
// -- ota -- begin
  if (!myAutomaticPosition ){
    gp_Pnt aMiddle12 ((myPoint1.XYZ() + myPoint2.XYZ())*0.5);
    gp_Pnt aMiddle34 ((myPoint3.XYZ() + myPoint4.XYZ())*0.5);

    if (myPosition.Distance(aMiddle12) > myPosition.Distance(aMiddle34))
      Position12.SetXYZ((myPoint1.XYZ() + myPoint2.XYZ())*0.5);
    else 	
      Position34.SetXYZ((myPoint3.XYZ() + myPoint4.XYZ())*0.5);

  }
  
  if (myFShape.ShapeType() == TopAbs_EDGE && mySShape.ShapeType() == TopAbs_EDGE)
    AIS_EqualDistanceRelation::ComputeTwoEdgesLength(aPresentation,
						     myDrawer,
						     myArrowSize,
						     TopoDS::Edge(myFShape),
						     TopoDS::Edge(mySShape),
						     myPlane,
						     myAutomaticPosition,
						     myIsSetBndBox,
						     myBndBox,
						     Position12,
						     myAttachPoint1,
						     myAttachPoint2,
						     myPoint1,
						     myPoint2,
						     mySymbolPrs );
	 
  
  else if (myFShape.ShapeType() == TopAbs_VERTEX && mySShape.ShapeType() == TopAbs_VERTEX)
    AIS_EqualDistanceRelation::ComputeTwoVerticesLength( aPresentation,
							myDrawer,
							myArrowSize,
							TopoDS::Vertex(myFShape),
							TopoDS::Vertex(mySShape),
							myPlane,
							myAutomaticPosition,
							myIsSetBndBox,
							myBndBox,
							AIS_TOD_Unknown,
							Position12,
							myAttachPoint1,
							myAttachPoint2,
							myPoint1,
							myPoint2,
							mySymbolPrs );
  else 
    AIS_EqualDistanceRelation::ComputeOneEdgeOneVertexLength( aPresentation,
							     myDrawer,
							     myArrowSize,
							     myFShape,
							     mySShape,
							     myPlane,
							     myAutomaticPosition,
							     myIsSetBndBox,
							     myBndBox,
							     Position12,
							     myAttachPoint1,
							     myAttachPoint2,
							     myPoint1,
							     myPoint2,
							     mySymbolPrs );

  if (myShape3.ShapeType() == TopAbs_EDGE && myShape4.ShapeType() == TopAbs_EDGE)
    AIS_EqualDistanceRelation::ComputeTwoEdgesLength(aPresentation,
						     myDrawer,
						     myArrowSize,
						     TopoDS::Edge(myShape3),
						     TopoDS::Edge(myShape4),
						     myPlane,
						     myAutomaticPosition,
						     myIsSetBndBox,
						     myBndBox,
						     Position34,
						     myAttachPoint3,
						     myAttachPoint4,
						     myPoint3,
						     myPoint4,
						     mySymbolPrs );
  
  else if (myShape3.ShapeType() == TopAbs_VERTEX && myShape4.ShapeType() == TopAbs_VERTEX)
    AIS_EqualDistanceRelation::ComputeTwoVerticesLength( aPresentation,
							myDrawer,
							myArrowSize,
							TopoDS::Vertex(myShape3),
							TopoDS::Vertex(myShape4),
							myPlane,
							myAutomaticPosition,
							myIsSetBndBox,
							myBndBox,
							AIS_TOD_Unknown,
							Position34,
							myAttachPoint3,
							myAttachPoint4,
							myPoint3,
							myPoint4,
							mySymbolPrs );
   
  else
    AIS_EqualDistanceRelation::ComputeOneEdgeOneVertexLength( aPresentation,
							     myDrawer,
							     myArrowSize,
							     myShape3,
							     myShape4,
							     myPlane,
							     myAutomaticPosition,
							     myIsSetBndBox,
							     myBndBox,
							     Position34,
							     myAttachPoint3,
							     myAttachPoint4,
							     myPoint3,
							     myPoint4,
							     mySymbolPrs );
  
  DsgPrs_EqualDistancePresentation::Add( aPresentation, myDrawer, 
					myPoint1, myPoint2, myPoint3, myPoint4, myPlane );
}


//=======================================================================
//function : Compute
//purpose  : to avoid warning at compilation (SUN)
//=======================================================================

void AIS_EqualDistanceRelation::Compute( const Handle( Prs3d_Projector )& /*aProjector*/,
					 const Handle( Prs3d_Presentation )& /*aPresentation*/) 
{
// throw Standard_NotImplemented("AIS_EqualDistanceRelation::Compute( const Handle( Prs3d_Projector )&,
 // const Handle( Prs3d_Presentation )& )");
// PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ; 
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning at compilation (SUN)
//=======================================================================
void AIS_EqualDistanceRelation::Compute(const Handle(Prs3d_Projector)& aProjector, 
				  const Handle(Geom_Transformation)& aTransformation,
				  const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_EqualDistanceRelation::Compute(const Handle(Prs3d_Projector)&,
//  const Handle(Geom_Transformation)&,const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ; 
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_EqualDistanceRelation::ComputeSelection( const Handle( SelectMgr_Selection )& aSelection,
						  const Standard_Integer ) 
{
  Handle( SelectMgr_EntityOwner ) own = new SelectMgr_EntityOwner( this, 7 );
  Handle( Select3D_SensitiveSegment ) seg;

  seg = new Select3D_SensitiveSegment( own, myPoint1, myPoint2 );
  aSelection->Add( seg );

  seg = new Select3D_SensitiveSegment( own, myPoint3, myPoint4 );
  aSelection->Add( seg );

  // Line between two middles
  gp_Pnt Middle12( (myPoint1.XYZ() + myPoint2.XYZ()) * 0.5 ), 
         Middle34( (myPoint3.XYZ() + myPoint4.XYZ()) *0.5 );
  seg = new Select3D_SensitiveSegment( own, Middle12, Middle34 );
  aSelection->Add( seg );

  gp_Pnt Middle((Middle12.XYZ() +  Middle34.XYZ())*0.5);
  Standard_Real SmallDist = .001;
  Handle( Select3D_SensitiveBox ) box = new Select3D_SensitiveBox( own,
								  Middle.X() - SmallDist,
								  Middle.Y() - SmallDist,
								  Middle.Z() - SmallDist,
								  Middle.X() + SmallDist,
								  Middle.Y() + SmallDist,
								  Middle.Z() + SmallDist );
  aSelection->Add(box);

  if (myFShape.ShapeType() == TopAbs_EDGE){
    BRepAdaptor_Curve aCurve(TopoDS::Edge(myFShape));
    if (aCurve.GetType() == GeomAbs_Line){
	//add sensetive element - line
      seg = new Select3D_SensitiveSegment( own, myAttachPoint1, myPoint1);
      aSelection->Add( seg );
    }
    else if (aCurve.GetType() == GeomAbs_Circle){
      Handle(Geom_Circle) aCircle = Handle(Geom_Circle)::DownCast(aCurve.Curve().Curve());
      Standard_Real FirstPar = ElCLib::Parameter(aCircle->Circ(), myAttachPoint1),
                    LastPar  = ElCLib::Parameter(aCircle->Circ(), myPoint1);
      if (LastPar < FirstPar ) LastPar+=M_PI*2;
      //add sensetive arc
      Handle(Select3D_SensitiveCircle) circ = 
	new Select3D_SensitiveCircle( own, aCircle,  FirstPar, LastPar);
      aSelection->Add( circ );
    }
  }
  else {
      seg = new Select3D_SensitiveSegment( own, myAttachPoint1, myPoint1);
      aSelection->Add( seg );
    }
  
  if (mySShape.ShapeType() == TopAbs_EDGE){
    BRepAdaptor_Curve aCurve(TopoDS::Edge(mySShape));
    if (aCurve.GetType() == GeomAbs_Line) {
      //add sensetive element - line
      seg = new Select3D_SensitiveSegment( own, myAttachPoint2, myPoint2);
      aSelection->Add( seg );
    }
    else if (aCurve.GetType() == GeomAbs_Circle){
      Handle(Geom_Circle) aCircle = Handle(Geom_Circle)::DownCast(aCurve.Curve().Curve());
      Standard_Real FirstPar = ElCLib::Parameter(aCircle->Circ(), myAttachPoint2),
      LastPar  = ElCLib::Parameter(aCircle->Circ(), myPoint2);
      if (LastPar < FirstPar ) LastPar+=M_PI*2;
      //add sensetive arc
      Handle(Select3D_SensitiveCircle) circ = 
	new Select3D_SensitiveCircle( own,aCircle,  FirstPar, LastPar);
      aSelection->Add( circ );
    }
  }
  else {
    seg = new Select3D_SensitiveSegment( own, myAttachPoint2, myPoint2);
    aSelection->Add( seg );
  }
    
  if (myShape3.ShapeType() == TopAbs_EDGE){
    BRepAdaptor_Curve aCurve(TopoDS::Edge(myShape3));
    if (aCurve.GetType() == GeomAbs_Line) {
      //add sensetive element - line
      seg = new Select3D_SensitiveSegment( own, myAttachPoint3, myPoint3);
      aSelection->Add( seg );
    }
    else if (aCurve.GetType() == GeomAbs_Circle){
      Handle(Geom_Circle) aCircle = Handle(Geom_Circle)::DownCast(aCurve.Curve().Curve());
      Standard_Real FirstPar = ElCLib::Parameter(aCircle->Circ(), myAttachPoint3),
      LastPar  = ElCLib::Parameter(aCircle->Circ(), myPoint3);
      if (LastPar < FirstPar ) LastPar+=M_PI*2;
      Handle(Select3D_SensitiveCircle) circ = 
	new Select3D_SensitiveCircle( own, aCircle,  FirstPar, LastPar);
      aSelection->Add( circ );
    }
    else {
      seg = new Select3D_SensitiveSegment( own, myAttachPoint3, myPoint3);
      aSelection->Add( seg );
    }
  }
  else {
    seg = new Select3D_SensitiveSegment( own, myAttachPoint3, myPoint3);
    aSelection->Add( seg );
  }

  if (myShape4.ShapeType() == TopAbs_EDGE){
    BRepAdaptor_Curve aCurve(TopoDS::Edge(myShape4));
    if (aCurve.GetType() == GeomAbs_Line) {
      //add sensetive element - line
      seg = new Select3D_SensitiveSegment( own, myAttachPoint4, myPoint4);
      aSelection->Add( seg );
    }
    else if (aCurve.GetType() == GeomAbs_Circle){
      Handle(Geom_Circle) aCircle = Handle(Geom_Circle)::DownCast(aCurve.Curve().Curve());
      Standard_Real FirstPar = ElCLib::Parameter(aCircle->Circ(), myAttachPoint4),
      LastPar  = ElCLib::Parameter(aCircle->Circ(), myPoint4);
      if (LastPar < FirstPar ) LastPar+=M_PI*2;
      //add sensetive arc
      Handle(Select3D_SensitiveCircle) circ = 
	new Select3D_SensitiveCircle( own,aCircle,  FirstPar, LastPar);
      aSelection->Add( circ );
    }
  }
  else {
    seg = new Select3D_SensitiveSegment( own, myAttachPoint4, myPoint4);
    aSelection->Add( seg );
  }
}

//=======================================================================
//function : ComputeTwoEdgesLength
//purpose  : 
//=======================================================================
void AIS_EqualDistanceRelation::ComputeTwoEdgesLength( const Handle( Prs3d_Presentation )& aPresentation,
						      const Handle( Prs3d_Drawer )& aDrawer,
						      const Standard_Real ArrowSize,
						      const TopoDS_Edge & FirstEdge,
						      const TopoDS_Edge & SecondEdge,
						      const Handle( Geom_Plane )& Plane,
						      const Standard_Boolean AutomaticPos,
						      const Standard_Boolean IsSetBndBox,
						      const Bnd_Box & BndBox,
						      gp_Pnt& Position,
						      gp_Pnt& FirstAttach,
						      gp_Pnt& SecondAttach,
						      gp_Pnt& FirstExtreme,
						      gp_Pnt& SecondExtreme, 
						      DsgPrs_ArrowSide & SymbolPrs )
{ 
  gp_Dir DirAttach;
  BRepAdaptor_Curve cu1( FirstEdge );
  BRepAdaptor_Curve cu2( SecondEdge );
  
  // 3d lines
  Handle(Geom_Curve) geom1,geom2;
  gp_Pnt ptat11,ptat12,ptat21,ptat22;
  
  Standard_Boolean isInfinite1(Standard_False),isInfinite2(Standard_False);
  Handle(Geom_Curve) extCurv;
  Standard_Real arrsize = ArrowSize;// size
  Standard_Real Val=0.;
  Standard_Boolean isInPlane1, isInPlane2;

  if(!AIS::ComputeGeometry(FirstEdge,geom1, ptat11, ptat12,extCurv,isInfinite1,isInPlane1, Plane ))
    return;
  if(!AIS::ComputeGeometry(SecondEdge, geom2, ptat21, ptat22, extCurv, isInfinite2,isInPlane2, Plane))
    return;
  
  aPresentation->SetInfiniteState(isInfinite1 || isInfinite2);
  
  if (cu1.GetType() == GeomAbs_Line && cu2.GetType() == GeomAbs_Line) 
    {
      Handle(Geom_Line) geom_lin1 (Handle(Geom_Line)::DownCast (geom1));
      Handle(Geom_Line) geom_lin2 (Handle(Geom_Line)::DownCast (geom2));
      const gp_Lin& l1 = geom_lin1->Lin();
      const gp_Lin& l2 = geom_lin2->Lin();
      
      //Get Val value
      Val = l1.Distance( l2 );
      
      DirAttach = l1.Direction();
      
      if (AutomaticPos) {
	// compute position of offset point
	gp_Pnt curpos;
	Standard_Real par1=0., par2=0.;
	if(!(isInfinite1 || isInfinite2))
	  {
	    par1 = ElCLib::Parameter(l1,ptat11);
	    par2 = ElCLib::Parameter(l1,ptat21);
	    if (par1 <par2){//project ptat11 on l2
	      gp_Pnt p2 = ElCLib::Value(ElCLib::Parameter(l2,ptat11),l2);
	      curpos.SetXYZ((ptat11.XYZ()+p2.XYZ())*0.5);
	    }
	    else {//project ptat21 on l1
	      gp_Pnt p2 = ElCLib::Value(par2, l1);
	      curpos.SetXYZ((ptat21.XYZ()+p2.XYZ())*0.5);
	    }
	  }
	else if (!isInfinite1){
	  par2 = ElCLib::Parameter(l1,ptat21);
	  gp_Pnt p2 = ElCLib::Value(par2,l1);
	  curpos.SetXYZ((ptat21.XYZ()+p2.XYZ())/2.);
	}
	else if (!isInfinite2) {
	  gp_Pnt p2 = ElCLib::Value(ElCLib::Parameter(l2,ptat11),l2);
	  curpos.SetXYZ((ptat11.XYZ()+p2.XYZ())*0.5);
	}
	else   
	  curpos.SetXYZ((l1.Location().XYZ()+l2.Location().XYZ())*0.5);
 
    // compute  offset
    gp_Vec offset(DirAttach);
    offset = offset*ArrowSize*(-10.);
    curpos.Translate(offset);
    Position = curpos;
  }
  else {    // project point on the plane
    Position = AIS::ProjectPointOnPlane( Position, Plane->Pln() );
  }

  // find attach points
  if (!isInfinite1) {
    if (Position.Distance(ptat11) > Position.Distance(ptat12)) FirstAttach = ptat12;
    else FirstAttach = ptat11;
  }
  else {
    FirstAttach = ElCLib::Value(ElCLib::Parameter(l1,Position),l1);
  }
  
  if (!isInfinite2) {
    if (Position.Distance(ptat21) > Position.Distance(ptat22)) SecondAttach = ptat22;
    else SecondAttach = ptat21;
  }
  else {
    SecondAttach = ElCLib::Value(ElCLib::Parameter(l2,Position),l2);
  }

  Standard_Real confusion(Precision::Confusion());
  if (arrsize < confusion) arrsize = Val*0.1;
  if (Abs(Val) <= confusion) {arrsize = 0.;}

  Handle(Prs3d_DimensionAspect) la = aDrawer->DimensionAspect();
  Handle(Prs3d_ArrowAspect) arr = la->ArrowAspect();  
  arr->SetLength(arrsize);
  arr = la->ArrowAspect();
  arr->SetLength(arrsize);

  if (AutomaticPos && IsSetBndBox)
    Position = AIS::TranslatePointToBound( Position, DirAttach, BndBox );
 
   DsgPrs_EqualDistancePresentation::AddInterval(aPresentation,
						aDrawer,
						FirstAttach,
						SecondAttach,
						DirAttach,
						Position,
						SymbolPrs,
						FirstExtreme,
						SecondExtreme);
						
      
}
  if (cu1.GetType() == GeomAbs_Circle && cu2.GetType() == GeomAbs_Circle){
    //Get first and last points of circles
    Handle(Geom_Circle) aCir1 (Handle(Geom_Circle)::DownCast(geom1));
    Handle(Geom_Circle) aCir2 (Handle(Geom_Circle)::DownCast(geom2));
    gp_Circ aCirc1 = aCir1->Circ();
    gp_Circ aCirc2 = aCir2->Circ();

    //To avoid circles with different orientaion
    Standard_Real aTol = Precision::Confusion();
    if(aCirc2.Axis().IsOpposite(aCirc1.Axis(), aTol) ||
       aCirc2.XAxis().IsOpposite(aCirc1.XAxis(), aTol) || 
       aCirc2.YAxis().IsOpposite(aCirc1.YAxis(), aTol) )
      {
	aCirc2.SetPosition(aCirc1.Position());
	aCirc2.SetAxis(aCirc1.Axis());
      }
    
    if (AutomaticPos){ 
      Standard_Real par1 = 0, par2 = 0;
      gp_Pln aPln =  Plane->Pln();
      //Project ptat12 and ptat22 on constraint plane
      gp_Pnt PrPnt12 = AIS::ProjectPointOnPlane(ptat12, aPln);
      gp_Pnt PrPnt22 = AIS::ProjectPointOnPlane(ptat22, aPln);
      //Project circles center on constraint plane
      gp_Pnt PrCenter = AIS::ProjectPointOnPlane(aCirc1.Location(), aPln);

      gp_Dir XDir = aPln.XAxis().Direction();
      gp_Dir YDir = aPln.YAxis().Direction();
      
      
      if (PrPnt12.Distance(PrCenter) >Precision::Confusion())
	{
	  gp_Dir aDir1(PrPnt12.XYZ() - PrCenter.XYZ());
	  Standard_Real anAngle = aDir1.Angle(XDir); //Get the angle in range [0, M_PI]
	  if (aDir1.Dot(YDir) < 0)
	    anAngle = 2 * M_PI - anAngle;
	  par1 = anAngle;
	}
      
      if (PrPnt22.Distance(PrCenter) >Precision::Confusion())
	{
	  gp_Dir aDir2(PrPnt22.XYZ() - PrCenter.XYZ());
	  Standard_Real anAngle = aDir2.Angle(XDir); //Get the angle in range [0, M_PI]
	  if (aDir2.Dot(YDir) < 0)
	    anAngle = 2 * M_PI - anAngle;
	  par2 = anAngle;
	}
      
      
      if(par1 > par2 ){
	FirstExtreme = ptat12;
	Standard_Real aPar1 = ElCLib::Parameter(aCirc2, ptat12);
	SecondExtreme = ElCLib::Value(aPar1, aCirc2);
      }
      else {
	Standard_Real aPar2 =  ElCLib::Parameter(aCirc1, ptat22);
	FirstExtreme = ElCLib::Value(aPar2, aCirc1);
	SecondExtreme = ptat22;
      }
    }
    else {
      Standard_Real pospar = ElCLib::Parameter(aCirc1, Position);
      FirstExtreme  =  ElCLib::Value(pospar, aCirc1);
      pospar = ElCLib::Parameter(aCirc2, Position);
      SecondExtreme =  ElCLib::Value(pospar, aCirc2);
    }

    DsgPrs_EqualDistancePresentation::AddIntervalBetweenTwoArcs(aPresentation,
								aDrawer,
								aCirc1,
								aCirc2,
								ptat12,
								FirstExtreme, 
								ptat22,
								SecondExtreme,
								SymbolPrs);
    
    FirstAttach = ptat12; SecondAttach = ptat22; //assign attach points
    Position.SetXYZ( (FirstAttach.XYZ() + SecondAttach.XYZ())*0.5);
  }

  if (arrsize < Precision::Confusion()) arrsize = Val*0.1;
  if (Abs(Val) <=  Precision::Confusion()) {arrsize = 0.;}

//  gp_Pnt pf, pl;
  if (!isInPlane1) {
    AIS::ComputeProjEdgePresentation( aPresentation, aDrawer, FirstEdge, geom1, ptat11, ptat12);
  }
  if(!isInPlane2) {
    AIS::ComputeProjEdgePresentation( aPresentation, aDrawer, SecondEdge, geom2, ptat21, ptat22);
  }
}

//=======================================================================
//function : ComputeTwoVerticesLength
//purpose  : 
//=======================================================================

void AIS_EqualDistanceRelation::ComputeTwoVerticesLength( const Handle( Prs3d_Presentation )& aPresentation,
							 const Handle( Prs3d_Drawer )& aDrawer,
							 const Standard_Real ArrowSize,
							 const TopoDS_Vertex& FirstVertex,
							 const TopoDS_Vertex& SecondVertex,
							 const Handle( Geom_Plane )& Plane,
							 const Standard_Boolean AutomaticPos,
							 const Standard_Boolean IsSetBndBox,
							 const Bnd_Box& BndBox,
							 const AIS_TypeOfDist TypeDist,
							 gp_Pnt& Position,
							 gp_Pnt& FirstAttach,
							 gp_Pnt& SecondAttach,
							 gp_Pnt& FirstExtreme,
							 gp_Pnt& SecondExtreme, 
							 DsgPrs_ArrowSide& SymbolPrs )
{
  Standard_Boolean isOnPlane1, isOnPlane2;
  gp_Dir DirAttach;
  AIS::ComputeGeometry( FirstVertex, FirstAttach, Plane, isOnPlane1);
  AIS::ComputeGeometry( SecondVertex, SecondAttach, Plane, isOnPlane2);

  Standard_Real confusion(Precision::Confusion());
  Standard_Boolean samePoint(FirstAttach.IsEqual(SecondAttach,confusion));

  if (TypeDist == AIS_TOD_Vertical) DirAttach =  Plane->Pln().XAxis().Direction();
  else if (TypeDist == AIS_TOD_Horizontal) DirAttach =  Plane->Pln().YAxis().Direction();
  else {
    if (!samePoint) {
      DirAttach.SetXYZ(SecondAttach.XYZ() - FirstAttach.XYZ());
      DirAttach.Rotate(Plane->Pln().Axis(),M_PI/2.);
    }
  }
  
  // size
  if (AutomaticPos) {
   if (!samePoint) {
     gp_Pnt curpos((FirstAttach.XYZ()+SecondAttach.XYZ())*0.5);
     // make offset of curpos
     gp_Vec offset(DirAttach);
     offset = offset*ArrowSize*(-10.);
     curpos.Translate(offset);
     Position = curpos;
   }
   else {  
     const gp_Dir& aDir = Plane->Pln().Axis().Direction();
     gp_Vec aVec (aDir.XYZ()*10*ArrowSize);
     //Position = gp_Pnt(FirstAttach.XYZ()+gp_XYZ(1.,1.,1.)); // not correct
     Position = FirstAttach.Translated(aVec);
     Position = AIS::ProjectPointOnPlane( Position, Plane->Pln() );//not needed really
     DirAttach.SetXYZ(Position.XYZ() - FirstAttach.XYZ());
   }
  }
  else {   
    Position = AIS::ProjectPointOnPlane( Position, Plane->Pln() );
  }

 
  Handle(Prs3d_DimensionAspect) la = aDrawer->DimensionAspect();
  Handle(Prs3d_ArrowAspect) arr = la->ArrowAspect();  
  arr->SetLength(ArrowSize);
  arr = la->ArrowAspect();
  arr->SetLength(ArrowSize);

  if (AutomaticPos && IsSetBndBox)
    Position = AIS::TranslatePointToBound( Position, DirAttach, BndBox );

  DsgPrs_EqualDistancePresentation::AddInterval(aPresentation,
						aDrawer,
						FirstAttach,
						SecondAttach,
						DirAttach,
						Position,
						SymbolPrs,
						FirstExtreme, //returned
						SecondExtreme); //returned

  // Compute projection
  if ( !isOnPlane1)
    AIS::ComputeProjVertexPresentation(aPresentation, aDrawer, FirstVertex, FirstAttach);
  if ( !isOnPlane2)
    AIS::ComputeProjVertexPresentation(aPresentation, aDrawer, SecondVertex, SecondAttach);

} 


//=======================================================================
//function : ComputeOneEdgeOneVertexLength
//purpose  : 
//=======================================================================

void AIS_EqualDistanceRelation::ComputeOneEdgeOneVertexLength( const Handle( Prs3d_Presentation )& aPresentation,
							      const Handle( Prs3d_Drawer )& aDrawer,
							      const Standard_Real ArrowSize,
							      const TopoDS_Shape & FirstShape,
							      const TopoDS_Shape & SecondShape,
							      const Handle( Geom_Plane )& Plane,
							      const Standard_Boolean AutomaticPos,
							      const Standard_Boolean IsSetBndBox,
							      const Bnd_Box & BndBox,
							      gp_Pnt & Position,
							      gp_Pnt & FirstAttach,
							      gp_Pnt & SecondAttach,
							      gp_Pnt& FirstExtreme,
							      gp_Pnt& SecondExtreme, 
							      DsgPrs_ArrowSide & SymbolPrs )
{
  TopoDS_Vertex thevertex;
  TopoDS_Edge theedge;
  
  gp_Pnt ptonedge1,ptonedge2;
  Handle(Geom_Curve) aCurve;
  Handle(Geom_Curve) extCurv;
  Standard_Boolean isInfinite;
  Standard_Real Val;
  Standard_Boolean isOnPlanEdge, isOnPlanVertex;
  Standard_Integer edgenum ;

  if (FirstShape.ShapeType() == TopAbs_VERTEX) {  
    thevertex = TopoDS::Vertex(FirstShape);
    theedge   = TopoDS::Edge(SecondShape);
    edgenum   = 2; //edge is the second shape
  }
  else {
    thevertex = TopoDS::Vertex(SecondShape);
    theedge   = TopoDS::Edge(FirstShape);
    edgenum   = 1;//edge is the first shape
  }
  if (!AIS::ComputeGeometry(theedge,aCurve,ptonedge1,ptonedge2,extCurv,isInfinite,isOnPlanEdge,Plane))
    return;
  aPresentation->SetInfiniteState(isInfinite);
  AIS::ComputeGeometry(thevertex, FirstAttach, Plane, isOnPlanVertex);

  if ( aCurve->IsInstance(STANDARD_TYPE(Geom_Line)) ) 
    {
  Handle(Geom_Line) geom_lin (Handle(Geom_Line)::DownCast (aCurve));
  const gp_Lin& l = geom_lin->Lin();

  // computation of Val
  Val = l.Distance( FirstAttach );

  gp_Dir DirAttach = l.Direction();
  // size
  Standard_Real arrsize = ArrowSize;
  if (Abs(Val) <= Precision::Confusion()) {arrsize = 0.;}

  if (AutomaticPos) {
    gp_Pnt p = ElCLib::Value(ElCLib::Parameter(l,FirstAttach),l);
    gp_Pnt curpos((FirstAttach.XYZ()+p.XYZ())*0.5);
    // make offset 
    gp_Vec offset(DirAttach);
    offset = offset*ArrowSize*(-10.);
    curpos.Translate(offset);
    Position = curpos;
  }
  else { // project point on the plane
    Position = AIS::ProjectPointOnPlane( Position, Plane->Pln() );
  }
  
  if (!isInfinite) {
    if (Position.Distance(ptonedge1) > Position.Distance(ptonedge2)) SecondAttach = ptonedge2;
    else SecondAttach = ptonedge1;
  }
  else {
    SecondAttach = ElCLib::Value(ElCLib::Parameter(l,Position),l);
  }
  
  Handle(Prs3d_DimensionAspect) la = aDrawer->DimensionAspect();
  Handle(Prs3d_ArrowAspect) arr = la->ArrowAspect();  
  arr->SetLength(arrsize);
  arr = la->ArrowAspect();
  arr->SetLength(arrsize);

  if (AutomaticPos && IsSetBndBox)
    Position = AIS::TranslatePointToBound( Position, DirAttach, BndBox );
  DsgPrs_EqualDistancePresentation::AddInterval(aPresentation,
						aDrawer,
						FirstAttach,
						SecondAttach,
						DirAttach,
						Position,
						SymbolPrs,
						FirstExtreme,
						SecondExtreme);
  
}
  if (aCurve->IsInstance(STANDARD_TYPE(Geom_Circle))){
    gp_Circ aCirc1 = (Handle(Geom_Circle)::DownCast(aCurve))->Circ();
    gp_Circ aCirc2(aCirc1); aCirc2.SetRadius(0); //create the second formal circle
    if(AutomaticPos)
      {
	SecondAttach = ptonedge2; //a vertex
	Position.SetXYZ((SecondAttach.XYZ() + aCirc1.Location().XYZ())*0.5);
      }
    else {
      Standard_Real aPar =  ElCLib::Parameter(aCirc1, Position);
      SecondAttach =  ElCLib::Value(aPar, aCirc1);
    }

    Handle(Geom_Circle) aCurve2 = new Geom_Circle(aCirc2);
    DsgPrs_EqualDistancePresentation::AddIntervalBetweenTwoArcs(aPresentation,
								aDrawer,
								aCirc1, //circle or arc
								aCirc2, //really vertex
								ptonedge2, //last point of aCirc1
								SecondAttach,
								FirstAttach, //vertex really
								FirstAttach, 
								SymbolPrs);
    
    //Assign arc points 
    if (edgenum == 1){
      FirstExtreme = SecondAttach; SecondExtreme = FirstAttach;
      SecondAttach = FirstAttach; FirstAttach = ptonedge2; 
    } else { //vertex is the first shape, circle is sthe last.
      FirstExtreme = FirstAttach; SecondExtreme = SecondAttach;
      SecondAttach = ptonedge2;
    }
  }
 
  // computation of Val
  Val = FirstAttach.Distance(SecondAttach);
  
  //Display the pieces of attached to the curve if it is not 
  // in the WP
  if (!isOnPlanEdge) { // add presentation of projection of the edge in WP
      AIS::ComputeProjEdgePresentation(aPresentation,aDrawer,theedge,aCurve,ptonedge1,ptonedge2);
      }
  if (!isOnPlanVertex) { // add presentation of projection of the vertex in WP 
    AIS::ComputeProjVertexPresentation(aPresentation,aDrawer,thevertex,FirstAttach);
    }
  
}
// -- ota -- end
