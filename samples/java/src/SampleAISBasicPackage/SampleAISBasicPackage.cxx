#include <SampleAISBasicPackage.hxx>
#include <TCollection_AsciiString.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_Circle.hxx>
#include <AIS_Line.hxx>
#include <User_Cylinder.hxx>
#include <Geom_Line.hxx>
#include <Geom_Axis2Placement.hxx>
#include <GC_MakeCircle.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

#ifdef WNT
#include <WNT_Window.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#else
#include <Xw_Window.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#endif




//===============================================================
// Function name: CreateViewer
//===============================================================
 Handle(V3d_Viewer) SampleAISBasicPackage::CreateViewer (const Standard_ExtString aName) 
{
#ifdef WNT
static Handle(Graphic3d_WNTGraphicDevice) defaultDevice;
    
  if (defaultDevice.IsNull()) 
    defaultDevice = new Graphic3d_WNTGraphicDevice();
  return new V3d_Viewer(defaultDevice, aName);
#else
static Handle(Graphic3d_GraphicDevice) defaultDevice;
    
  if (defaultDevice.IsNull())
    defaultDevice = new Graphic3d_GraphicDevice("");
  return new V3d_Viewer(defaultDevice, aName);
#endif //WNT
}

//===============================================================
// Function name: SetWindow
//===============================================================
void SampleAISBasicPackage::SetWindow (const Handle(V3d_View)& aView,
				       const Standard_Integer hiwin,
				       const Standard_Integer lowin)
{
#ifdef WNT
  Handle(Graphic3d_WNTGraphicDevice) d = 
    Handle(Graphic3d_WNTGraphicDevice)::DownCast(aView->Viewer()->Device());
  Handle(WNT_Window) w = new WNT_Window(d,hiwin,lowin);
#else
  Handle(Graphic3d_GraphicDevice) d = 
    Handle(Graphic3d_GraphicDevice)::DownCast(aView->Viewer()->Device());
  Handle(Xw_Window) w = new Xw_Window(d,hiwin,lowin,Xw_WQ_3DQUALITY);
#endif
  aView->SetWindow(w);
}



 void SampleAISBasicPackage::DisplayTrihedron(const Handle(AIS_InteractiveContext)& aContext) 
{
  Handle(AIS_Trihedron) aTrihedron;
  Handle(Geom_Axis2Placement) anAxis=new Geom_Axis2Placement(gp::XOY());
  aTrihedron=new AIS_Trihedron(anAxis);
  aContext->Display(aTrihedron);
}

 void SampleAISBasicPackage::DisplayCircle(const Handle(AIS_InteractiveContext)& aContext,
					   TCollection_AsciiString& Message) 
{
  GC_MakeCircle C(gp_Pnt(-100.,-300.,0.),gp_Pnt(-50.,-200.,0.),gp_Pnt(-10.,-250.,0.));
  Handle(AIS_Circle) anAISCirc = new AIS_Circle(C.Value());
  aContext->Display(anAISCirc);

  Message = "\
		\n\
GC_MakeCircle C(gp_Pnt(-100.,-300.,0.),gp_Pnt(-50.,-200.,0.),gp_Pnt(-10.,-250.,0.)); \n\
Handle(AIS_Circle) anAISCirc = new AIS_Circle(C.Value()); \n\
myAISContext->Display(anAISCirc); \n\
		\n";
}

 void SampleAISBasicPackage::DisplayLine(const Handle(AIS_InteractiveContext)& aContext,
					 TCollection_AsciiString& Message) 
{
  gp_Lin L(gp_Pnt(0.,0.,0.),gp_Dir(1.,0.,0.));
  Handle(Geom_Line) aLine = new Geom_Line(L);
  Handle(AIS_Line) anAISLine = new AIS_Line(aLine);
  aContext->Display(anAISLine);	

  Message = "\
		\n\
gp_Lin L(gp_Pnt(0.,0.,0.),gp_Dir(1.,0.,0.)); \n\
Handle(Geom_Line) aLine = new Geom_Line(L); \n\
Handle(AIS_Line) anAISLine = new AIS_Line(aLine); \n\
myAISContext->Display(anAISLine); \n\
		\n";
}

 void SampleAISBasicPackage::DisplaySphere(const Handle(AIS_InteractiveContext)& aContext,
					   TCollection_AsciiString& Message) 
{
  BRepPrimAPI_MakeSphere S(gp_Pnt(200.,300.,200.), 100.);
  Handle(AIS_Shape) anAISShape = new AIS_Shape(S.Shape());
  aContext->SetColor(anAISShape,Quantity_NOC_AZURE); 
  aContext->SetMaterial(anAISShape,Graphic3d_NOM_PLASTIC);    
  aContext->SetDisplayMode(anAISShape,1);
  aContext->Display(anAISShape);	

  Message = "\
		\n\
BRepPrimAPI_MakeSphere S(gp_Pnt(200.,300.,200.), 100.); \n\
Handle(AIS_Shape) anAISShape = new AIS_Shape(S.Shape()); \n\
myAISContext->SetColor(anAISShape,Quantity_NOC_AZURE); \n\
myAISContext->SetMaterial(anAISShape,Graphic3d_NOM_PLASTIC); \n\
myAISContext->SetDisplayMode(anAISShape,1); \n\
myAISContext->Display(anAISShape); \n\
		\n";
}

 void SampleAISBasicPackage::DisplayCylinder(const Handle(AIS_InteractiveContext)& aContext,
					     TCollection_AsciiString& Message) 
{
  Handle(User_Cylinder) aCyl = new User_Cylinder(100.,200.);
  aContext->SetDisplayMode(aCyl,1);
  aContext->Display(aCyl);	

  Message = "\
		\n\
Handle(User_Cylinder) aCyl = new User_Cylinder(100.,200.); \n\
myAISContext->SetDisplayMode(aCyl,1); \n\
myAISContext->Display(aCyl); \n\
 \n\
NOTE: a User_Cylinder is an object defined by the user. \n\
The User_Cylinder class inherits the AIS_InteractiveObject \n\
CASCADE class, its usage is the same as an AIS_InteractiveObject. \n\
		\n";
}

