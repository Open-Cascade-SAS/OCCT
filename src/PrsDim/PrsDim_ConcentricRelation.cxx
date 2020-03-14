// Created on: 1996-12-05
// Created by: Flore Lantheaume/Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
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

#include <PrsDim_ConcentricRelation.hxx>

#include <PrsDim.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <DsgPrs_ConcentricPresentation.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Plane.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Prs3d_Presentation.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_NotImplemented.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsDim_ConcentricRelation, PrsDim_Relation)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
PrsDim_ConcentricRelation::PrsDim_ConcentricRelation(
	const TopoDS_Shape& aFShape, 
	const TopoDS_Shape& aSShape, 
	const Handle(Geom_Plane)& aPlane)
{
  myFShape = aFShape;
  mySShape = aSShape;
  myPlane = aPlane;
  myDir = aPlane->Pln().Axis().Direction();
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void PrsDim_ConcentricRelation::Compute(const Handle(PrsMgr_PresentationManager3d)&, 
				     const Handle(Prs3d_Presentation)& aPresentation, 
				     const Standard_Integer)
{
  TopAbs_ShapeEnum type2(mySShape.ShapeType());
  aPresentation->SetInfiniteState(Standard_True);
  switch (myFShape.ShapeType()) {
  case TopAbs_EDGE: 
    {
      if (type2 == TopAbs_EDGE) ComputeTwoEdgesConcentric(aPresentation);
      else if (type2 == TopAbs_VERTEX) ComputeEdgeVertexConcentric(aPresentation);
    }
  break;
  
  case TopAbs_VERTEX: 
    {
      if (type2 == TopAbs_VERTEX) ComputeTwoVerticesConcentric(aPresentation);
      else if (type2 == TopAbs_EDGE) ComputeEdgeVertexConcentric(aPresentation);      
    }
  break;
  default: {return;}
  }  
}

//=======================================================================
//function : ComputeTwoEdgesConcentric
//purpose  : 
//=======================================================================
void PrsDim_ConcentricRelation::ComputeEdgeVertexConcentric(const Handle(Prs3d_Presentation)& aPresentation)
{
  TopoDS_Edge E;
  TopoDS_Vertex V;
  if (myFShape.ShapeType() == TopAbs_EDGE) {
    E = TopoDS::Edge(myFShape);
    V = TopoDS::Vertex(mySShape);
  }
  else {
    E = TopoDS::Edge(mySShape);
    V = TopoDS::Vertex(myFShape);
  }
  gp_Pnt p1,p2;
  Handle(Geom_Curve) C;
  Handle(Geom_Curve) extCurv;
  Standard_Boolean isInfinite;
  Standard_Boolean isOnPlanEdge, isOnPlanVertex;
  if (!PrsDim::ComputeGeometry(E,C,p1,p2,extCurv,isInfinite,isOnPlanEdge,myPlane)) return;
  gp_Pnt P;
  PrsDim::ComputeGeometry(V,P, myPlane, isOnPlanVertex);

  Handle(Geom_Circle) CIRCLE (Handle(Geom_Circle)::DownCast (C));
  myCenter = CIRCLE->Location();
  myRad = Min(CIRCLE->Radius()/5.,15.);
  gp_Dir vec(p1.XYZ() - myCenter.XYZ() );
  gp_Vec vectrans(vec);
  myPnt = myCenter.Translated(vectrans.Multiplied(myRad));
  DsgPrs_ConcentricPresentation::Add(aPresentation,myDrawer,myCenter,myRad,myDir,myPnt);
  if (!isOnPlanEdge)   PrsDim::ComputeProjEdgePresentation(aPresentation,myDrawer,E,CIRCLE,p1,p2);
  if (!isOnPlanVertex) PrsDim::ComputeProjVertexPresentation(aPresentation,myDrawer,V,P);
}

//=======================================================================
//function : ComputeTwoEdgesConcentric
//purpose  : 
//=======================================================================
void PrsDim_ConcentricRelation::ComputeTwoVerticesConcentric(const Handle(Prs3d_Presentation)& aPresentation)
{
  TopoDS_Vertex V1,V2;
  V1 = TopoDS::Vertex(myFShape);
  V2 = TopoDS::Vertex(myFShape);  
  Standard_Boolean isOnPlanVertex1(Standard_True),isOnPlanVertex2(Standard_True);
  gp_Pnt P1,P2;
  PrsDim::ComputeGeometry(V1,P1, myPlane,isOnPlanVertex1);
  PrsDim::ComputeGeometry(V2,P2, myPlane,isOnPlanVertex2);
  myCenter = P1;
  myRad    = 15.;
  gp_Dir vec(myPlane->Pln().Position().XDirection());
  gp_Vec vectrans(vec);
  myPnt = myCenter.Translated(vectrans.Multiplied(myRad));
  DsgPrs_ConcentricPresentation::Add(aPresentation,myDrawer,myCenter,myRad,myDir,myPnt);
  if (!isOnPlanVertex1) PrsDim::ComputeProjVertexPresentation(aPresentation,myDrawer,V1,P1);
  if (!isOnPlanVertex2) PrsDim::ComputeProjVertexPresentation(aPresentation,myDrawer,V2,P2);
}

//=======================================================================
//function : ComputeTwoEdgesConcentric
//purpose  : 
//=======================================================================
void PrsDim_ConcentricRelation::ComputeTwoEdgesConcentric(const Handle(Prs3d_Presentation)& aPresentation)
{
  BRepAdaptor_Curve curv1(TopoDS::Edge(myFShape));
  BRepAdaptor_Curve curv2(TopoDS::Edge(mySShape));
  
  gp_Pnt ptat11,ptat12,ptat21,ptat22;
  Handle(Geom_Curve) geom1,geom2;
  Standard_Boolean isInfinite1,isInfinite2;
  Handle(Geom_Curve) extCurv;
  if (!PrsDim::ComputeGeometry(TopoDS::Edge(myFShape), TopoDS::Edge(mySShape),
			    myExtShape,
			    geom1, geom2,
			    ptat11, ptat12,
			    ptat21, ptat22,
			    extCurv,
			    isInfinite1,isInfinite2,
			    myPlane)) {
    return;
  }
  
  Handle(Geom_Circle) gcirc1 (Handle(Geom_Circle)::DownCast (geom1));
  Handle(Geom_Circle) gcirc2 (Handle(Geom_Circle)::DownCast (geom2));
  
  myCenter = gcirc1->Location();
  
  // choose the radius equal to 1/5 of the smallest radius of 
  // 2 circles. Limit is imposed ( 0.02 by chance)
  Standard_Real aRad1 = gcirc1->Radius();
  Standard_Real aRad2 = gcirc2->Radius();
  myRad = (aRad1 > aRad2 ) ? aRad2 : aRad1;
  myRad /= 5;
  if (myRad > 15.) myRad =15.;
  
  
  //Calculate a point of circle of radius myRad
  gp_Dir vec(ptat11.XYZ() - myCenter.XYZ() );
  gp_Vec vectrans(vec);
  myPnt = myCenter.Translated(vectrans.Multiplied(myRad));
  
  DsgPrs_ConcentricPresentation::Add(aPresentation,
				     myDrawer,
				     myCenter,
				     myRad,
				     myDir,
				     myPnt);
  if ( (myExtShape != 0) &&  !extCurv.IsNull()) {
    gp_Pnt pf, pl;
    if ( myExtShape == 1 ) {
      if (!isInfinite1) {
	pf = ptat11; 
	pl = ptat12;
      }
      ComputeProjEdgePresentation(aPresentation,TopoDS::Edge(myFShape),gcirc1,pf,pl);
    }
    else {
      if (!isInfinite2) {
	pf = ptat21; 
	pl = ptat22;
      }
      ComputeProjEdgePresentation(aPresentation,TopoDS::Edge(mySShape),gcirc2,pf,pl);
    }
  }
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void PrsDim_ConcentricRelation::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection, 
					      const Standard_Integer)
{
  Handle(SelectMgr_EntityOwner) own = new SelectMgr_EntityOwner(this,7);
  
  //Creation of 2 sensitive circles
     // the greater
  gp_Ax2 ax(myCenter, myDir);
  gp_Circ aCirc (ax, myRad);
  Handle(Select3D_SensitiveCircle) sensit = new Select3D_SensitiveCircle (own, aCirc);
  aSelection->Add(sensit);
     // the smaller
  aCirc.SetRadius(myRad/2);
  sensit = new Select3D_SensitiveCircle (own, aCirc);
  aSelection->Add(sensit);

  //Creation of 2 segments sensitive for the cross
  Handle(Select3D_SensitiveSegment) seg;
  gp_Pnt otherPnt = myPnt.Mirrored(myCenter);
  seg = new Select3D_SensitiveSegment(own,
				      otherPnt,
				      myPnt);
  aSelection->Add(seg);

  gp_Ax1 RotateAxis(myCenter, myDir);
  gp_Pnt FPnt = myCenter.Rotated(RotateAxis, M_PI/2);
  gp_Pnt SPnt = myCenter.Rotated(RotateAxis, -M_PI/2);
  seg = new Select3D_SensitiveSegment(own,
				      FPnt,
				      SPnt);
  aSelection->Add(seg);

}

