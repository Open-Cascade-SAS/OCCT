#include <ISession_Point.ixx>
#include <Geom_CartesianPoint.hxx>
#include <StdPrs_Point.hxx>
#include <Graphic2d_CircleMarker.hxx>


ISession_Point::ISession_Point()
{
}

ISession_Point::ISession_Point(const Standard_Real X,const Standard_Real Y,const Standard_Real Z)
     :myPoint(gp_Pnt(X,Y,Z))
{
}

ISession_Point::ISession_Point(const gp_Pnt2d& aPoint,const Standard_Real Elevation)
     :myPoint(gp_Pnt(aPoint.X(),aPoint.Y(),Elevation))
{
}

ISession_Point::ISession_Point(const gp_Pnt& aPoint)
     :myPoint(aPoint)
{
}

 void ISession_Point::Compute(const Handle(PrsMgr_PresentationManager3d)& ,const Handle(Prs3d_Presentation)& aPresentation,const Standard_Integer ) 
{
  Handle(Geom_CartesianPoint) aGeomPoint = new Geom_CartesianPoint(myPoint);
  StdPrs_Point::Add(aPresentation,aGeomPoint,myDrawer);
}

 void ISession_Point::Compute(const Handle(Prs3d_Projector)& ,const Handle(Prs3d_Presentation)& ) 
{
}

 void ISession_Point::Compute(const Handle(PrsMgr_PresentationManager2d)& ,const Handle(Graphic2d_GraphicObject)& aGrObj,const Standard_Integer ) 
{
  Handle(Graphic2d_CircleMarker) aCircleMarker;
  aCircleMarker = new Graphic2d_CircleMarker(aGrObj,myPoint.X(),myPoint.Y(),0,0,1);
}

 void ISession_Point::ComputeSelection(const Handle(SelectMgr_Selection)& ,const Standard_Integer ) 
{
}

