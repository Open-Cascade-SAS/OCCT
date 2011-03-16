// File:	StdSelect_Shape.cxx
// Created:	Fri Mar 27 14:53:29 1998
// Author:	Robert COUBLANC
//		<rob@robox.paris1.matra-dtv.fr>



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
