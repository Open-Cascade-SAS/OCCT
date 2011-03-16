#include <TPrsStd_PointDriver.ixx>

#include <TDF_Label.hxx>
#include <TDataXtd_Point.hxx>
#include <TDataXtd_Geometry.hxx>
//#include <TDataStd_Datum.hxx>
#include <AIS_Point.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Geom_Point.hxx>
#include <Geom_CartesianPoint.hxx>
#include <gp_Pnt.hxx>



//=======================================================================
//function :
//purpose  : 
//=======================================================================
TPrsStd_PointDriver::TPrsStd_PointDriver()
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_PointDriver::Update (const TDF_Label& aLabel,
					     Handle(AIS_InteractiveObject)& anAISObject) 
{
  Handle(TDataXtd_Point) appoint;

  if ( !aLabel.FindAttribute(TDataXtd_Point::GetID(), appoint) ) {
    return Standard_False;
  }
 
  gp_Pnt pnt;
  if (!TDataXtd_Geometry::Point(aLabel,pnt)) {
    return Standard_False;
  }
  Handle(Geom_CartesianPoint) apt = new Geom_CartesianPoint(pnt);
  
  //  Update de l'AIS
  Handle(AIS_Point) aistrihed;
  if (anAISObject.IsNull())
    aistrihed = new AIS_Point(apt);
  else {
    aistrihed = Handle(AIS_Point)::DownCast(anAISObject);
    if (aistrihed.IsNull()) 
      aistrihed = new AIS_Point(apt);
    else {
      aistrihed->SetComponent(apt);
      aistrihed->ResetLocation();
      aistrihed->SetToUpdate();
      aistrihed->UpdateSelection();
    }
  }
  anAISObject = aistrihed;
  return Standard_True;
}

