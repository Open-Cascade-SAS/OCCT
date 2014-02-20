// Created on: 1996-12-05
// Created by: Odile Olivier
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

//GER61351	//GG_171199     Enable to set an object RGB color instead a restricted object NameOfColor.

#define BUC60915 	//GG 05/06/01 Enable to compute the requested arrow size 
//			if any in all dimensions.

#include <AIS.hxx>
#include <Graphic3d_Group.hxx>

#include <AIS_Relation.ixx>

#include <AIS_Drawer.hxx>
#include <AIS_GraphicTool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_CartesianPoint.hxx>

#include <Prs3d_LineAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_TextAspect.hxx>

#include <StdPrs_WFDeflectionShape.hxx>
#include <StdPrs_Point.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <ElCLib.hxx>
#include <TopExp.hxx>


#include <Precision.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

//=======================================================================
//function : AIS_Relation
//purpose  : 
//=======================================================================
AIS_Relation::AIS_Relation(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d)
:AIS_InteractiveObject(aTypeOfPresentation3d),
 myVal(1.),
 myPosition(0.,0.,0.),
#ifdef BUC60915
 myArrowSize( myVal / 10. ),
#endif
 myAutomaticPosition(Standard_True),
 myExtShape(0),
 myFirstOffset(0.),mySecondOffset(0.),
 myIsSetBndBox( Standard_False )
#ifdef BUC60915
 ,myArrowSizeIsDefined( Standard_False)
#endif
{
}



//=======================================================================
//function : ComputeProjEdgePresentation
//purpose  : 
//=======================================================================

void AIS_Relation::ComputeProjEdgePresentation(const Handle(Prs3d_Presentation)& aPrs, 
					       const TopoDS_Edge& anEdge,
					       const Handle(Geom_Curve)& ProjCurv, 
					       const gp_Pnt& FirstP, 
					       const gp_Pnt& LastP, 
					       const Quantity_NameOfColor aColor,
					       const Standard_Real width,
					       const Aspect_TypeOfLine aProjTOL,
					       const Aspect_TypeOfLine aCallTOL) const 
{
  if (!myDrawer->HasWireAspect()){
    myDrawer->SetWireAspect(new Prs3d_LineAspect(aColor,aProjTOL,2.));}
  else {
    const Handle(Prs3d_LineAspect)& li = myDrawer->WireAspect();
    li->SetColor(aColor);
    li->SetTypeOfLine(aProjTOL);
    li->SetWidth(width);
  }

  Standard_Real pf, pl;
  TopLoc_Location loc;
  Handle(Geom_Curve) curve;
  Standard_Boolean isInfinite;
  curve = BRep_Tool::Curve(anEdge,loc,pf,pl);
  isInfinite = (Precision::IsInfinite(pf) || Precision::IsInfinite(pl));

  TopoDS_Edge E;

  // Calcul de la presentation de l'edge
  if (ProjCurv->IsInstance(STANDARD_TYPE(Geom_Line)) ) {
    const Handle(Geom_Line) & gl = (Handle(Geom_Line)&) ProjCurv;
    if ( !isInfinite) {
      pf = ElCLib::Parameter(gl->Lin(),FirstP);
      pl = ElCLib::Parameter(gl->Lin(),LastP);
      BRepBuilderAPI_MakeEdge MakEd(gl->Lin(), pf, pl);
      E = MakEd.Edge();
    }
    else {
      BRepBuilderAPI_MakeEdge MakEd(gl->Lin());
      E = MakEd.Edge();
    }
  }
  else if (ProjCurv->IsInstance(STANDARD_TYPE(Geom_Circle)) ) {
    const Handle(Geom_Circle) & gc = (Handle(Geom_Circle)&) ProjCurv;
    pf = ElCLib::Parameter(gc->Circ(),FirstP);
    pl = ElCLib::Parameter(gc->Circ(),LastP);
    BRepBuilderAPI_MakeEdge MakEd(gc->Circ(),pf, pl);
    E = MakEd.Edge();
  }
  StdPrs_WFDeflectionShape::Add(aPrs, E, myDrawer);

  //Calcul de la presentation des lignes de raccord
  myDrawer->WireAspect()->SetTypeOfLine(aCallTOL);
  if (!isInfinite) {
    gp_Pnt ppf, ppl;
    ppf = BRep_Tool::Pnt( TopExp::FirstVertex(TopoDS::Edge(anEdge)));
    ppl = BRep_Tool::Pnt( TopExp::LastVertex(TopoDS::Edge(anEdge)));
    if (FirstP.Distance( ppf ) > gp::Resolution())
      {
	BRepBuilderAPI_MakeEdge MakEd1(FirstP, ppf);
	StdPrs_WFDeflectionShape::Add(aPrs, MakEd1.Edge(), myDrawer);
      }
    else
      {
	BRepBuilderAPI_MakeVertex MakVert1( FirstP );
	StdPrs_WFDeflectionShape::Add(aPrs, MakVert1.Vertex(), myDrawer);
      }
    if (LastP.Distance( ppl ) > gp::Resolution())
      {
	BRepBuilderAPI_MakeEdge MakEd2(LastP, ppl);
	StdPrs_WFDeflectionShape::Add(aPrs, MakEd2.Edge(), myDrawer);
      }
    else
      {
	BRepBuilderAPI_MakeVertex MakVert2( LastP );
	StdPrs_WFDeflectionShape::Add(aPrs, MakVert2.Vertex(), myDrawer);
      }
/*
    BRepBuilderAPI_MakeEdge MakEd1(FirstP, ppf);
    StdPrs_WFDeflectionShape::Add(aPrs, MakEd1.Edge(), myDrawer);
    BRepBuilderAPI_MakeEdge MakEd2(LastP, ppl);
    StdPrs_WFDeflectionShape::Add(aPrs, MakEd2.Edge(), myDrawer);
*/
  }
}


//=======================================================================
//function : ComputeProjVertexPresentation
//purpose  : 
//=======================================================================

void AIS_Relation::ComputeProjVertexPresentation(const Handle(Prs3d_Presentation)& aPrs, 
						 const TopoDS_Vertex& aVertex,
						 const gp_Pnt& ProjPoint, 
						 const Quantity_NameOfColor aColor,
						 const Standard_Real width,
						 const Aspect_TypeOfMarker aProjTOM,
						 const Aspect_TypeOfLine aCallTOL) const 
{
  if (!myDrawer->HasPointAspect()){
    myDrawer->SetPointAspect(new Prs3d_PointAspect(aProjTOM, aColor,1));}
  else {
    const Handle(Prs3d_PointAspect)& pa = myDrawer->PointAspect();
    pa->SetColor(aColor);
    pa->SetTypeOfMarker(aProjTOM);
  }
  
  // calcul du projete
  StdPrs_Point::Add(aPrs, new Geom_CartesianPoint(ProjPoint), myDrawer);

  if (!myDrawer->HasWireAspect()){
    myDrawer->SetWireAspect(new Prs3d_LineAspect(aColor,aCallTOL,2.));}
  else {
    const Handle(Prs3d_LineAspect)& li = myDrawer->WireAspect();
    li->SetColor(aColor);
    li->SetTypeOfLine(aCallTOL);
    li->SetWidth(width);
  }
  
  // Si les points ne sont pas confondus...
  if (!ProjPoint.IsEqual (BRep_Tool::Pnt(aVertex),Precision::Confusion())) {
    // calcul des lignes de rappel
    BRepBuilderAPI_MakeEdge MakEd(ProjPoint,BRep_Tool::Pnt(aVertex));
    StdPrs_WFDeflectionShape::Add(aPrs, MakEd.Edge(), myDrawer);
  }
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void AIS_Relation::SetColor(const Quantity_NameOfColor aCol)
{
  SetColor(Quantity_Color(aCol));
}

void AIS_Relation::SetColor(const Quantity_Color &aCol)
{
  if(hasOwnColor && myOwnColor==aCol) return;

  if (!myDrawer->HasTextAspect()) myDrawer->SetTextAspect(new Prs3d_TextAspect());
  hasOwnColor=Standard_True;
  myOwnColor=aCol;
  myDrawer->TextAspect()->SetColor(aCol);

  Standard_Real WW = HasWidth()? Width():AIS_GraphicTool::GetLineWidth(myDrawer->Link(),AIS_TOA_Line);
  if (!myDrawer->HasLineAspect()) {
    myDrawer->SetLineAspect(new Prs3d_LineAspect(aCol,Aspect_TOL_SOLID,WW));
  }
  if (!myDrawer->HasDimensionAspect()) {
     myDrawer->SetDimensionAspect(new Prs3d_DimensionAspect);
  }

  myDrawer->LineAspect()->SetColor(aCol);  
  const Handle(Prs3d_DimensionAspect)& DIMENSION = myDrawer->DimensionAspect();
  const Handle(Prs3d_LineAspect)&   LINE   = myDrawer->LineAspect();
  const Handle(Prs3d_TextAspect)&   TEXT   = myDrawer->TextAspect();

  DIMENSION->SetLineAspect(LINE);
  DIMENSION->SetTextAspect(TEXT); 
}

//=======================================================================
//function : UnsetColor
//purpose  : 
//=======================================================================
void AIS_Relation::UnsetColor()
{
  if (!hasOwnColor) return;
  hasOwnColor = Standard_False;
  const Handle(Prs3d_LineAspect)& LA = myDrawer->LineAspect();
  Quantity_Color CC;
  AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Line,CC);
  LA->SetColor(CC);
  myDrawer->DimensionAspect()->SetLineAspect(LA);
  myDrawer->SetTextAspect(myDrawer->Link()->TextAspect());
}

//=======================================================================
//function : AcceptDisplayMode
//purpose  : 
//=======================================================================

 Standard_Boolean  AIS_Relation::
AcceptDisplayMode(const Standard_Integer aMode) const
{return aMode == 0;}


//=======================================================================
//function : SetFirstShape
//purpose  : 
//=======================================================================

void AIS_Relation::SetFirstShape(const TopoDS_Shape& aFShape)
{
  myFShape = aFShape;
}


//=======================================================================
//function : SetSecondShape
//purpose  : 
//=======================================================================

void AIS_Relation::SetSecondShape(const TopoDS_Shape& aSShape)
{
  mySShape = aSShape;
}

//=======================================================================
//function : KindOfDimension
//purpose  : 
//=======================================================================
AIS_KindOfDimension AIS_Relation::KindOfDimension() const 
{return AIS_KOD_NONE;}

//=======================================================================
//function : IsMovable
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Relation::IsMovable() const 
{return Standard_False;}


