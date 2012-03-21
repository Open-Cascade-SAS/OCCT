// Created on: 1998-03-27
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
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




#include <StdSelect_Shape.ixx>
#include <StdPrs_WFShape.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <TopAbs_ShapeEnum.hxx>

StdSelect_Shape::StdSelect_Shape(const TopoDS_Shape& sh):
       mysh(sh)
{}
     
void StdSelect_Shape::Compute(const Handle(PrsMgr_PresentationManager3d)& PM,
			      const Handle(Prs3d_Presentation)& P,
			      const Standard_Integer aMode)
{
  if(mysh.IsNull()) return;
  
  static Handle(Prs3d_Drawer) DRWR;
  if(DRWR.IsNull()){
    DRWR = new Prs3d_Drawer();
    DRWR->WireAspect()->SetWidth(2);
    DRWR->LineAspect()->SetWidth(2.);
    DRWR->PlaneAspect()->EdgesAspect()->SetWidth(2.);
    DRWR->FreeBoundaryAspect()->SetWidth(2.);
    DRWR->UnFreeBoundaryAspect()->SetWidth(2.);
    Standard_Integer I = 5;//pour tests...
    DRWR->PointAspect()->SetTypeOfMarker((Aspect_TypeOfMarker)I);
    DRWR->PointAspect()->SetScale(2.);
  }
  
  Standard_Boolean CanShade = (mysh.ShapeType()<5 || mysh.ShapeType()==8);
  if(aMode==1){
    if(CanShade)
      StdPrs_ShadedShape::Add(P,mysh,DRWR);
    else
      StdPrs_WFShape::Add(P,mysh,DRWR);
  }
  else if (aMode==0)
    StdPrs_WFShape::Add(P,mysh,DRWR);
}

void StdSelect_Shape::Compute(const Handle_Prs3d_Projector& aProjector ,
			      const Handle_Geom_Transformation& aGeomTrans, 
			      const Handle_Prs3d_Presentation& aPresentation )
{
  PrsMgr_PresentableObject::Compute(aProjector,aGeomTrans,aPresentation);
}

void StdSelect_Shape::Compute(const Handle_PrsMgr_PresentationManager2d& aPrsMgr,
			      const Handle_Graphic2d_GraphicObject& aGO, 
			      const int anInt)
{
  PrsMgr_PresentableObject::Compute(aPrsMgr,aGO,anInt);
}

void StdSelect_Shape::Compute(const Handle_Prs3d_Projector& aProjector,
			      const Handle_Prs3d_Presentation& aPresentation)
{
  PrsMgr_PresentableObject::Compute(aProjector,aPresentation);
}
