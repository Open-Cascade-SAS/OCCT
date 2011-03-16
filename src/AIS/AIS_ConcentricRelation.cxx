// File:	AIS_ConcentricRelation.cdl
// Created:	Tue Dec  5 15:09:04 1996
// Author:	Flore Lantheaume/Odile Olivier
//              <ODL>

#include <Standard_NotImplemented.hxx>

#include <AIS_ConcentricRelation.ixx>

#include <SelectMgr_EntityOwner.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitiveSegment.hxx>

#include <DsgPrs_ConcentricPresentation.hxx>

#include <TopoDS.hxx>

#include <BRepAdaptor_Curve.hxx>

#include <GeomAbs_CurveType.hxx>
#include <Geom_Circle.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>

#include <TopoDS_Vertex.hxx>
#include <AIS.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_ConcentricRelation::AIS_ConcentricRelation(
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
void AIS_ConcentricRelation::Compute(const Handle(PrsMgr_PresentationManager3d)&, 
				     const Handle(Prs3d_Presentation)& aPresentation, 
				     const Standard_Integer)
{
  aPresentation->Clear();

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

void AIS_ConcentricRelation::Compute(const Handle_Prs3d_Projector& aProjector, const Handle_Geom_Transformation& aTransformation, const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_ConcentricRelation::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
  PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeTwoEdgesConcentric
//purpose  : 
//=======================================================================
void AIS_ConcentricRelation::ComputeEdgeVertexConcentric(const Handle(Prs3d_Presentation)& aPresentation)
{
  TopoDS_Edge E;
  TopoDS_Vertex V;
  if (myFShape.ShapeType() == TopAbs_EDGE) {
    E = TopoDS::Edge(myFShape);
    V = TopoDS::Vertex(mySShape);
  }
  else {
    E = TopoDS::Edge(myFShape);
    V = TopoDS::Vertex(mySShape);
  }
  gp_Pnt p1,p2;
  Handle(Geom_Curve) C;
  Handle(Geom_Curve) extCurv;
  Standard_Boolean isInfinite;
  Standard_Boolean isOnPlanEdge, isOnPlanVertex;
  if (!AIS::ComputeGeometry(E,C,p1,p2,extCurv,isInfinite,isOnPlanEdge,myPlane)) return;
  gp_Pnt P;
  AIS::ComputeGeometry(V,P, myPlane, isOnPlanVertex);

  const Handle(Geom_Circle)& CIRCLE = (Handle(Geom_Circle)&) C;
  myCenter = CIRCLE->Location();
  myRad = Min(CIRCLE->Radius()/5.,15.);
  gp_Dir vec(p1.XYZ() - myCenter.XYZ() );
  gp_Vec vectrans(vec);
  myPnt = myCenter.Translated(vectrans.Multiplied(myRad));
  DsgPrs_ConcentricPresentation::Add(aPresentation,myDrawer,myCenter,myRad,myDir,myPnt);
  if (!isOnPlanEdge) AIS::ComputeProjEdgePresentation(aPresentation,myDrawer,E,CIRCLE,p1,p2);
  if (!isOnPlanVertex) AIS::ComputeProjVertexPresentation(aPresentation,myDrawer,V,P);
}

//=======================================================================
//function : ComputeTwoEdgesConcentric
//purpose  : 
//=======================================================================
void AIS_ConcentricRelation::ComputeTwoVerticesConcentric(const Handle(Prs3d_Presentation)& aPresentation)
{
  TopoDS_Vertex V1,V2;
  V1 = TopoDS::Vertex(myFShape);
  V2 = TopoDS::Vertex(myFShape);  
  Standard_Boolean isOnPlanVertex1(Standard_True),isOnPlanVertex2(Standard_True);
  gp_Pnt P1,P2;
  AIS::ComputeGeometry(V1,P1, myPlane,isOnPlanVertex1);
  AIS::ComputeGeometry(V2,P2, myPlane,isOnPlanVertex2);
  myCenter = P1;
  myRad    = 15.;
  gp_Dir vec(myPlane->Pln().Position().XDirection());
  gp_Vec vectrans(vec);
  myPnt = myCenter.Translated(vectrans.Multiplied(myRad));
  DsgPrs_ConcentricPresentation::Add(aPresentation,myDrawer,myCenter,myRad,myDir,myPnt);
  if (!isOnPlanVertex1) AIS::ComputeProjVertexPresentation(aPresentation,myDrawer,V1,P1);
  if (!isOnPlanVertex1) AIS::ComputeProjVertexPresentation(aPresentation,myDrawer,V2,P2);
}

//=======================================================================
//function : ComputeTwoEdgesConcentric
//purpose  : 
//=======================================================================
void AIS_ConcentricRelation::ComputeTwoEdgesConcentric(const Handle(Prs3d_Presentation)& aPresentation)
{
  BRepAdaptor_Curve curv1(TopoDS::Edge(myFShape));
  BRepAdaptor_Curve curv2(TopoDS::Edge(mySShape));
  
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
  
  const Handle(Geom_Circle)& gcirc1 = (Handle(Geom_Circle)&) geom1;
  const Handle(Geom_Circle)& gcirc2 = (Handle(Geom_Circle)&) geom2;
  
  myCenter = gcirc1->Location();
  
  // on choisit le rayon egal a 1/5 ieme du rayon du plus petit des
  // 2 cercles. On impose une borne sup au rayon( 0.02 au hasard)
  Standard_Real rad1 = gcirc1->Radius();
  Standard_Real rad2 = gcirc2->Radius();
  myRad = (rad1 > rad2 ) ? rad2 : rad1;
  myRad /= 5;
  if (myRad > 15.) myRad =15.;
  
  
  //Calcul d'un point du cercle de rayon myRad
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
//function : Compute
//purpose  : to avoid warning
//=======================================================================

void AIS_ConcentricRelation::Compute(const Handle(Prs3d_Projector)&, 
				     const Handle(Prs3d_Presentation)&)
{
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================

void AIS_ConcentricRelation::Compute(const Handle(PrsMgr_PresentationManager2d)&, 
				     const Handle(Graphic2d_GraphicObject)&,
				     const Standard_Integer)
{
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_ConcentricRelation::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection, 
					      const Standard_Integer)
{
  Handle(SelectMgr_EntityOwner) own = new SelectMgr_EntityOwner(this,7);
  
  //Creation de 2 sensitives cercles
     // le plus grand
  gp_Ax2 ax(myCenter, myDir);
  Handle(Geom_Circle) Circ = new Geom_Circle(ax, myRad) ;
  Handle(Select3D_SensitiveCircle) 
    sensit = new Select3D_SensitiveCircle (own,
					   Circ);
  aSelection->Add(sensit);
     // le plus petit
  Circ->SetRadius(myRad/2);
  sensit = new Select3D_SensitiveCircle (own,
					 Circ);
  aSelection->Add(sensit);

  //Creation de 2 segments sensitifs pour la croix
  Handle(Select3D_SensitiveSegment) seg;
  gp_Pnt otherPnt = myPnt.Mirrored(myCenter);
  seg = new Select3D_SensitiveSegment(own,
				      otherPnt,
				      myPnt);
  aSelection->Add(seg);

  gp_Ax1 RotateAxis(myCenter, myDir);
  gp_Pnt FPnt = myCenter.Rotated(RotateAxis, PI/2);
  gp_Pnt SPnt = myCenter.Rotated(RotateAxis, -PI/2);
  seg = new Select3D_SensitiveSegment(own,
				      FPnt,
				      SPnt);
  aSelection->Add(seg);

}

