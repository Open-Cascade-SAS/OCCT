// ISession_Point.cpp: implementation of the ISession_Point class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\GeometryApp.h"
#include "ISession_Point.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif
IMPLEMENT_STANDARD_HANDLE(ISession_Point,AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(ISession_Point,AIS_InteractiveObject)

#include "Graphic2d_CircleMarker.hxx"
#include "StdPrs_Point.hxx"
#include "Geom_CartesianPoint.hxx"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ISession_Point::ISession_Point(Standard_Real X,Standard_Real Y ,Standard_Real Z)
:myPoint(gp_Pnt(X,Y,Z))
{

}

ISession_Point::ISession_Point(gp_Pnt2d& aPoint,Standard_Real Elevation)
:myPoint(gp_Pnt(aPoint.X(),aPoint.Y(),Elevation))
{

}

ISession_Point::ISession_Point(gp_Pnt& aPoint)
:myPoint(aPoint)
{

}

ISession_Point::~ISession_Point()
{

}

void ISession_Point::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                             const Handle(Prs3d_Presentation)& aPresentation,
                             const Standard_Integer aMode)
{
    Handle(Geom_CartesianPoint) aGeomPoint = new Geom_CartesianPoint(myPoint);

    StdPrs_Point::Add(aPresentation,aGeomPoint,myDrawer);
 }


void ISession_Point::Compute(const Handle(Prs3d_Projector)& aProjector,
                             const Handle(Prs3d_Presentation)& aPresentation) 
 {
 }

void ISession_Point::Compute(const Handle(PrsMgr_PresentationManager2d)& aPresentationManager, 
                         const Handle(Graphic2d_GraphicObject)& aGrObj, 
                         const Standard_Integer unMode)
{
  Handle(Graphic2d_CircleMarker) aCircleMarker;
  aCircleMarker = new Graphic2d_CircleMarker(aGrObj,myPoint.X(),myPoint.Y(),0,0,1);
}

void ISession_Point::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection, 
				      const Standard_Integer unMode)
{
}

