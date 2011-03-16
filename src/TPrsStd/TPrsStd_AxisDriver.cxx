#include <TPrsStd_AxisDriver.ixx>

#include <TDF_Label.hxx>
#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Geometry.hxx>
#include <AIS_Axis.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Geom_Line.hxx>
#include <gp_Lin.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>



//=======================================================================
//function :
//purpose  : 
//=======================================================================
TPrsStd_AxisDriver::TPrsStd_AxisDriver()
{
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_AxisDriver::Update (const TDF_Label& aLabel,
					    Handle(AIS_InteractiveObject)& anAISObject) 
{

  Handle(TDataXtd_Axis) apAxis;
  if ( !aLabel.FindAttribute(TDataXtd_Axis::GetID(), apAxis) ) {
    return Standard_False;
  }

  gp_Lin lin;  
  Handle(TNaming_NamedShape) NS;
  if(aLabel.FindAttribute(TNaming_NamedShape::GetID(),NS)){
    if(TNaming_Tool::GetShape(NS).IsNull()){
      return Standard_False;
    }
  }
  
  Handle(AIS_Axis) aistrihed;
  if (TDataXtd_Geometry::Line(aLabel,lin)) {
    Handle(Geom_Line) apt = new Geom_Line (lin);
    
    //  Update de l'AIS
    if (anAISObject.IsNull())
      aistrihed = new AIS_Axis(apt);
    else {
      aistrihed = Handle(AIS_Axis)::DownCast(anAISObject);
      if (aistrihed.IsNull()) 
	aistrihed = new AIS_Axis(apt);
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
  return Standard_False;
}

