// Created on: 1996-12-05
// Created by: Jean-Pierre COMBE/Odile Olivier
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


#include <AIS.hxx>
#include <AIS_PerpendicularRelation.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <DsgPrs_PerpenPresentation.hxx>
#include <ElCLib.hxx>
#include <gce_MakeDir.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Transformation.hxx>
#include <GeomAPI.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Precision.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_PerpendicularRelation,AIS_Relation)

//=======================================================================
//function : Constructor
//purpose  : TwoEdgesPerpendicular
//=======================================================================
AIS_PerpendicularRelation::AIS_PerpendicularRelation(const TopoDS_Shape& aFShape, 
						     const TopoDS_Shape& aSShape, 
						     const Handle(Geom_Plane)& aPlane)
:AIS_Relation()
{
  myFShape = aFShape;
  mySShape = aSShape;
  myPlane = aPlane;
}

//=======================================================================
//function : Constructor
//purpose  : TwoFacesPerpendicular
//=======================================================================
AIS_PerpendicularRelation::AIS_PerpendicularRelation(const TopoDS_Shape& aFShape, 
						     const TopoDS_Shape& aSShape)
:AIS_Relation()
{
  myFShape = aFShape;
  mySShape = aSShape;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_PerpendicularRelation::Compute(const Handle(PrsMgr_PresentationManager3d)&, 
					const Handle(Prs3d_Presentation)& aPresentation, 
					const Standard_Integer)
{
  if (myFShape.ShapeType() == mySShape.ShapeType()) {
    switch (myFShape.ShapeType()) {
    case TopAbs_FACE :
      {
	// cas perpendiculaire entre deux faces
	ComputeTwoFacesPerpendicular(aPresentation);
      }
      break;
    case TopAbs_EDGE :
      {
	// cas perpendiculaire entre deux edges
	ComputeTwoEdgesPerpendicular(aPresentation);
      }
      break;
    default:
      break;
    }
  }
  // Cas pas traite - Edge/Face
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================
void AIS_PerpendicularRelation::Compute(const Handle(Prs3d_Projector)& aProjector,
					const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_PerpendicularRelation::Compute(const Handle(Prs3d_Projector)&,const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

void AIS_PerpendicularRelation::Compute(const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTransformation, const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_PerpendicularRelation::Compute(const Handle(Prs3d_Projector)&, const Handle(Geom_Transformation)&, const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_PerpendicularRelation::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection, 
						 const Standard_Integer)
{
  Handle(SelectMgr_EntityOwner) own = new SelectMgr_EntityOwner(this,7);
  const gp_Pnt& pos = myPosition;
  Handle(Select3D_SensitiveSegment) seg;
  Standard_Boolean ok1(Standard_False),ok2(Standard_False);

  if (!myFAttach.IsEqual(pos,Precision::Confusion())) {
    seg = new Select3D_SensitiveSegment(own,
					myFAttach,
					pos);
    aSelection->Add(seg);
    ok1 = Standard_True;
 }
  if (!mySAttach.IsEqual(myPosition,Precision::Confusion())) {
    seg = new Select3D_SensitiveSegment(own,
					mySAttach,
					pos);
    aSelection->Add(seg);
    ok2 = Standard_True;
  }

  if (ok1 && ok2) {
    gp_Vec vec1(gce_MakeDir(pos,myFAttach));
    gp_Vec vec2(gce_MakeDir(pos,mySAttach));
    Standard_Real dist1(pos.Distance(myFAttach));
    Standard_Real dist2(pos.Distance(mySAttach));
    vec1 *= dist1;
    vec1 *= .2;
    vec2 *= dist2;
    vec2 *= .2;
    
    gp_Pnt pAx11 = pos.Translated(vec1);
    gp_Pnt pAx22 = pos.Translated(vec2);
    gp_Pnt p_symb = pAx22.Translated(vec1);
    seg = new Select3D_SensitiveSegment(own,pAx11,p_symb);
    aSelection->Add(seg);
    seg = new Select3D_SensitiveSegment(own,p_symb,pAx22);
    aSelection->Add(seg);
  }
}

//=======================================================================
//function : ComputeTwoFacesPerpendicular
//purpose  : 
//=======================================================================
void AIS_PerpendicularRelation::ComputeTwoFacesPerpendicular
  (const Handle(Prs3d_Presentation)& /*aPresentation*/)
{
}

//=======================================================================
//function : ComputeTwoEdgesPerpendicular
//purpose  : 
//=======================================================================
void AIS_PerpendicularRelation::ComputeTwoEdgesPerpendicular(const Handle(Prs3d_Presentation)& aPresentation)
{
  // 3d lines
  Handle(Geom_Curve) geom1,geom2;
  gp_Pnt pint3d,p1,p2,pAx1,pAx2,ptat11,ptat12,ptat21,ptat22;
  Standard_Boolean isInfinite1,isInfinite2;
  Handle(Geom_Curve) extCurv;
  if ( !AIS::ComputeGeometry(TopoDS::Edge(myFShape),TopoDS::Edge(mySShape),
			    myExtShape,
			    geom1,geom2,
			    ptat11,ptat12,ptat21,ptat22,
			    extCurv,
			    isInfinite1,isInfinite2,
			    myPlane) ) return;

  Standard_Boolean interOut1(Standard_False),interOut2(Standard_False);
  
  Handle(Geom_Line) geom_lin1;
  Handle(Geom_Line) geom_lin2;
  if ( geom1->IsInstance(STANDARD_TYPE(Geom_Ellipse)) )
    {
      Handle(Geom_Ellipse) geom_el (Handle(Geom_Ellipse)::DownCast (geom1));
      // construct lines through focuses
      gp_Ax1 elAx = geom_el->XAxis();
      gp_Lin ll (elAx);
      geom_lin1 = new Geom_Line(ll);
      Standard_Real focex = geom_el->MajorRadius() - geom_el->Focal()/2.0;
      gp_Vec transvec = gp_Vec(elAx.Direction())*focex;
      ptat11 = geom_el->Focus1().Translated(transvec);
      ptat12 = geom_el->Focus2().Translated(-transvec);
      interOut1 = Standard_True;
    }
  else if ( geom1->IsInstance(STANDARD_TYPE(Geom_Line)) )
    {
      geom_lin1 = Handle(Geom_Line)::DownCast (geom1);
    }
  else return;

  if (geom2->IsInstance(STANDARD_TYPE(Geom_Ellipse)))
    {
      Handle(Geom_Ellipse) geom_el (Handle(Geom_Ellipse)::DownCast (geom2));
      // construct lines through focuses
      gp_Ax1 elAx = geom_el->XAxis();
      gp_Lin ll (elAx);
      geom_lin2 = new Geom_Line(ll);
      Standard_Real focex = geom_el->MajorRadius() - geom_el->Focal()/2.0;
      gp_Vec transvec = gp_Vec(elAx.Direction())*focex;
      ptat21 = geom_el->Focus1().Translated(transvec);
      ptat22 = geom_el->Focus2().Translated(-transvec);
      interOut2 = Standard_True;
    }
  else if ( geom2->IsInstance(STANDARD_TYPE(Geom_Line)) )
    {
      geom_lin2 = Handle(Geom_Line)::DownCast (geom2);
    }
  else return;

  // current face
  BRepBuilderAPI_MakeFace makeface (myPlane->Pln());
  TopoDS_Face face (makeface.Face());  
  BRepAdaptor_Surface adp (makeface.Face());
  
  // 2d lines => projection of 3d on current plane
  Handle(Geom2d_Curve) aGeom2dCurve = GeomAPI::To2d(geom_lin1,myPlane->Pln());
  Handle(Geom2d_Line) lin1_2d = Handle(Geom2d_Line)::DownCast (aGeom2dCurve) ;
  aGeom2dCurve = GeomAPI::To2d(geom_lin2,myPlane->Pln());
  Handle(Geom2d_Line) lin2_2d = Handle(Geom2d_Line)::DownCast (aGeom2dCurve) ;
  IntAna2d_AnaIntersection inter(lin1_2d->Lin2d(),lin2_2d->Lin2d());
  if (!inter.IsDone()) return;
  if (!inter.NbPoints()) return;
  
  gp_Pnt2d pint(inter.Point(1).Value());
  pint3d = adp.Value(pint.X(),pint.Y());

  myPosition = pint3d;
  // recherche points attache
  Standard_Real par1,par2,curpar,pmin,pmax;//,dist,sign;
  Standard_Real length(0.);
  
  if ( isInfinite1 && isInfinite2 )
    {
      Standard_Real curpar1 = ElCLib::Parameter(geom_lin1->Lin(),pint3d);
      Standard_Real curpar2 = ElCLib::Parameter(geom_lin2->Lin(),pint3d);
      par1 = par2 = 50.;    
      p1 = p2 = pint3d;
      myFAttach = ElCLib::Value(curpar1+par1,geom_lin1->Lin());
      mySAttach = ElCLib::Value(curpar2+par2,geom_lin2->Lin());    
    }
  else
    {
      Standard_Boolean lengthComputed (Standard_False);
      if ( !isInfinite1 )
	{
	  curpar = ElCLib::Parameter(geom_lin1->Lin(),pint3d);
	  par1 = ElCLib::Parameter(geom_lin1->Lin(),ptat11);
	  par2 = ElCLib::Parameter(geom_lin1->Lin(),ptat12);
	  pmin = Min(par1,par2);
	  pmax = Max(par1,par2);
      
	  if ( myPosition.SquareDistance(ptat11) > myPosition.SquareDistance(ptat12) )
	    p1 = ptat11;
	  else
	    p1 = ptat12;
	  if ( (curpar < pmin) || (curpar > pmax) )
	    {
	      interOut1 = Standard_True;
	    }
	  if ( !isInfinite2 ) length = 2.*Min(ptat11.Distance(ptat12),ptat21.Distance(ptat22))/5.;
	  else length = 2.*ptat11.Distance(ptat12)/5.;
	  lengthComputed = Standard_True;
	  gp_Vec vec1 (gce_MakeDir(myPosition,p1));
	  vec1.Multiply(length);
	  pAx1 = myPosition.Translated(vec1);
	  myFAttach = pAx1;
	}
      if ( !isInfinite2 )
	{
	  curpar = ElCLib::Parameter(geom_lin2->Lin(),pint3d);
	  par1 = ElCLib::Parameter(geom_lin2->Lin(),ptat21);
	  par2 = ElCLib::Parameter(geom_lin2->Lin(),ptat22);
	  pmin = Min(par1,par2);
	  pmax = Max(par1,par2);
	  
	  if ( myPosition.SquareDistance(ptat21) > myPosition.SquareDistance(ptat22) ) p2 = ptat21;
	  else p2 = ptat22;
	  if ( (curpar < pmin) || (curpar > pmax) )
	    {
	      interOut2 = Standard_True;
	    }
	  gp_Vec vec2 (gce_MakeDir(myPosition,p2));
	  if ( !lengthComputed )
	    {
	      if ( !isInfinite1 ) length = 2.*Min(ptat11.Distance(ptat12),ptat21.Distance(ptat22))/5.;
	      else length = 2.*ptat21.Distance(ptat22)/5.;
	    }
	  vec2.Multiply(length);
	  pAx2 = myPosition.Translated(vec2);
	  mySAttach = pAx2;
	}
      if ( isInfinite1 )
	{
	  p1 = myPosition;
	  gp_Vec vec1(geom_lin1->Lin().Direction());
	  vec1.Multiply(length);
	  myFAttach = myPosition.Translated(vec1);
	}
      if ( isInfinite2 )
	{
	  p2 = myPosition;
	  gp_Vec vec2(geom_lin2->Lin().Direction());
	  vec2.Multiply(length);
	  mySAttach = myPosition.Translated(vec2);      
	}
    }
  DsgPrs_PerpenPresentation::Add(aPresentation,myDrawer,
				 myFAttach,mySAttach,
				 p1,p2,
				 myPosition,
				 interOut1,interOut2);

  if ( (myExtShape != 0) && !extCurv.IsNull()) {
    gp_Pnt pf,pl;
    if ( myExtShape == 1 ) {
      if (!isInfinite1) {
	pf = ptat11; 
	pl = ptat12;
      }
      aPresentation->SetInfiniteState(isInfinite1);
      ComputeProjEdgePresentation(aPresentation,TopoDS::Edge(myFShape),geom_lin1,pf,pl);
    }
    else {
      if (!isInfinite2) {
	pf = ptat21; 
	pl = ptat22;
      }
      aPresentation->SetInfiniteState(isInfinite2);
      ComputeProjEdgePresentation(aPresentation,TopoDS::Edge(mySShape),geom_lin2,pf,pl);
    }
  }
}
