#include <SampleAISDisplayModePackage.hxx>
#include <TCollection_AsciiString.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <User_Cylinder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Quantity_Color.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <V3d_DirectionalLight.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt2d.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>
#include <Precision.hxx>


#ifdef WNT
#include <WNT_Window.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#else
#include <Xw_Window.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#endif


/*----------------------------------------------------------------------*/

Handle(V3d_DirectionalLight) aLight;
Handle(AIS_Shape) aShape;
gp_Pnt p1, p2;



//===============================================================
// Function name: CreateViewer
//===============================================================
 Handle(V3d_Viewer) SampleAISDisplayModePackage::CreateViewer (const Standard_ExtString aName) 
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
void SampleAISDisplayModePackage::SetWindow (const Handle(V3d_View)& aView,
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


/*----------------------------------------------------------------------*/

gp_Pnt ConvertClickToPoint(Standard_Real x, Standard_Real y, Handle(V3d_View) aView)
{
  V3d_Coordinate XEye,YEye,ZEye,XAt,YAt,ZAt;
  aView->Eye(XEye,YEye,ZEye);
  aView->At(XAt,YAt,ZAt);
  gp_Pnt EyePoint(XEye,YEye,ZEye);
  gp_Pnt AtPoint(XAt,YAt,ZAt);

  gp_Vec EyeVector(EyePoint,AtPoint);
  gp_Dir EyeDir(EyeVector);

  gp_Pln PlaneOfTheView = gp_Pln(AtPoint,EyeDir);
  V3d_Coordinate X,Y,Z;
  aView->Convert(Standard_Integer(x),Standard_Integer(y),X,Y,Z);
  gp_Pnt ConvertedPoint(X,Y,Z);
  gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView,ConvertedPoint);
  
  gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(),
				     ConvertedPointOnPlane.Y(),
				     PlaneOfTheView);
  return ResultPoint;
}


//======================================================================
//=                                                                    =
//=                            Display objects                         =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: DisplayBox
//===============================================================
 void SampleAISDisplayModePackage::DisplayBox(const Handle(AIS_InteractiveContext)& aContext,
					      TCollection_AsciiString& Message) 
{
  BRepPrimAPI_MakeBox B(gp_Pnt(-400.,-400.,-100.),200.,150.,100.);
  Handle(AIS_Shape) aBox = new AIS_Shape(B.Shape());
  aContext->Display(aBox);

  Message = "\
		\n\
BRepPrimAPI_MakeBox B(gp_Pnt(-400.,-400.,-100.),200.,150.,100.); \n\
Handle(AIS_Shape) aBox = new AIS_Shape(B.Shape()); \n\
myAISContext->Display(aBox); \n\
		\n";
}

//===============================================================
// Function name: DisplaySphere
//===============================================================
 void SampleAISDisplayModePackage::DisplaySphere(const Handle(AIS_InteractiveContext)& aContext,
						 TCollection_AsciiString& Message) 
{
  BRepPrimAPI_MakeSphere S(gp_Pnt(200.,300.,200.), 100.);
  Handle(AIS_Shape) anAISShape = new AIS_Shape(S.Shape());
  aContext->Display(anAISShape);

  Message = "\
		\n\
BRepPrimAPI_MakeSphere S(gp_Pnt(200.,300.,200.), 100.); \n\
Handle(AIS_Shape) anAISShape = new AIS_Shape(S.Shape()); \n\
myAISContext->Display(anAISShape); \n\
		\n";
}

//===============================================================
// Function name: DisplayCylinder
//===============================================================
 void SampleAISDisplayModePackage::DisplayCylinder(const Handle(AIS_InteractiveContext)& aContext,
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

//===============================================================
// Function name: EraseAll
//===============================================================
 void SampleAISDisplayModePackage::EraseAll(const Handle(AIS_InteractiveContext)& aContext,
					    TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  Message = "\
		\n\
myAISContext->EraseAll(Standard_False); \n\
		\n";
}

//======================================================================
//=                                                                    =
//=                          Context properties                        =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: InitContext
//===============================================================
 void SampleAISDisplayModePackage::InitContext(const Handle(AIS_InteractiveContext)& aContext) 
{
  aContext->DefaultDrawer()->ShadingAspect()->SetColor(Quantity_NOC_CHARTREUSE1);
  aContext->DefaultDrawer()->ShadingAspect()->SetMaterial(Graphic3d_NOM_SILVER);
}

//===============================================================
// Function name: GetIsosNumber
//===============================================================
 void SampleAISDisplayModePackage::GetIsosNumber(const Handle(AIS_InteractiveContext)& aContext,
						 Standard_Integer& u,Standard_Integer& v) 
{
  u = aContext->DefaultDrawer()->UIsoAspect()->Number();
  v = aContext->DefaultDrawer()->VIsoAspect()->Number();
}

//===============================================================
// Function name: SetIsosNumber
//===============================================================
 void SampleAISDisplayModePackage::SetIsosNumber(const Handle(AIS_InteractiveContext)& aContext,
						 const Standard_Integer u,
						 const Standard_Integer v,
						 TCollection_AsciiString& Message) 
{
  aContext->DefaultDrawer()->UIsoAspect()->SetNumber(u);
  aContext->DefaultDrawer()->VIsoAspect()->SetNumber(v);

  Message = "\
		\n\
myAISContext->DefaultDrawer()->UIsoAspect()->SetNumber(u); \n\
myAISContext->DefaultDrawer()->VIsoAspect()->SetNumber(v); \n\
		\n";
}

//===============================================================
// Function name: SetDisplayMode
//===============================================================
 void SampleAISDisplayModePackage::SetDisplayMode(const Handle(AIS_InteractiveContext)& aContext,
						  const AIS_DisplayMode aMode,
						  TCollection_AsciiString& Message) 
{
  aContext->SetDisplayMode(aMode);

  Message = "\
		\n\
myAISContext->SetDisplayMode(aMode); \n\
		\n";
}

//======================================================================
//=                                                                    =
//=                           Object properties                        =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: SetObjectDisplayMode
//===============================================================
 void SampleAISDisplayModePackage::SetObjectDisplayMode(const Handle(AIS_InteractiveContext)& aContext,
							const AIS_DisplayMode aMode,
							TCollection_AsciiString& Message) 
{
  for (aContext->InitCurrent(); aContext->MoreCurrent(); aContext->NextCurrent())
    aContext->SetDisplayMode(aContext->Current(), aMode);

  Message = "\
		\n\
for (myAISContext->InitCurrent(); myAISContext->MoreCurrent(); \n\
     myAISContext->NextCurrent()) \n\
  myAISContext->SetDisplayMode(myAISContext->Current(), aMode); \n\
		\n";
}

//===============================================================
// Function name: SetObjectMaterial
//===============================================================
 void SampleAISDisplayModePackage::SetObjectMaterial(const Handle(AIS_InteractiveContext)& aContext,
						     const Graphic3d_NameOfMaterial aName,
						     TCollection_AsciiString& Message) 
{
  for (aContext->InitCurrent(); aContext->MoreCurrent(); aContext->NextCurrent())
    aContext->SetMaterial(aContext->Current(), aName);

  Message = "\
		\n\
for (myAISContext->InitCurrent(); myAISContext->MoreCurrent(); \n\
     myAISContext->NextCurrent()) \n\
  myAISContext->SetMaterial(myAISContext->Current(), aName); \n\
		\n";
}

//===============================================================
// Function name: GetObjectColor
//===============================================================
 Quantity_Color SampleAISDisplayModePackage::GetObjectColor(const Handle(AIS_InteractiveContext)& aContext) 
{
  Handle(AIS_InteractiveObject) Current;
  Quantity_Color aColor;

  aContext->InitCurrent();
  if (aContext->MoreCurrent()) {
    Current = aContext->Current();
    if (Current->HasColor())
      aColor = Current->Color();
  }

  return aColor;
}

//===============================================================
// Function name: SetObjectColor
//===============================================================
 void SampleAISDisplayModePackage::SetObjectColor(const Handle(AIS_InteractiveContext)& aContext,
						  const Quantity_Color& aColor,
						  TCollection_AsciiString& Message) 
{
  for (aContext->InitCurrent(); aContext->MoreCurrent(); aContext->NextCurrent())
    aContext->SetColor(aContext->Current(), aColor.Name());

  Message = "\
		\n\
for (myAISContext->InitCurrent(); myAISContext->MoreCurrent(); \n\
     myAISContext->NextCurrent()) \n\
  myAISContext->SetColor(myAISContext->Current(), aColor.Name()); \n\
		\n";
}

//===============================================================
// Function name: GetObjectTransparency
//===============================================================
 Standard_Real SampleAISDisplayModePackage::GetObjectTransparency(const Handle(AIS_InteractiveContext)& aContext) 
{
  Standard_Real aValue = 0.0;

  aContext->InitCurrent();
  if (aContext->MoreCurrent())
    aValue = aContext->Current()->Transparency();

  return aValue;
}

//===============================================================
// Function name: SetObjectTransparency
//===============================================================
 void SampleAISDisplayModePackage::SetObjectTransparency(const Handle(AIS_InteractiveContext)& aContext,
							 const Standard_Real aValue,
							 TCollection_AsciiString& Message) 
{
  for (aContext->InitCurrent(); aContext->MoreCurrent(); aContext->NextCurrent())
    aContext->SetTransparency(aContext->Current(), aValue);

  Message = "\
		\n\
for (myAISContext->InitCurrent(); myAISContext->MoreCurrent(); \n\
     myAISContext->NextCurrent()) \n\
  myAISContext->SetTransparency(myAISContext->Current(), aValue); \n\
		\n";
}



//======================================================================
//=                                                                    =
//=                               Light                                =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: CreateLight
//===============================================================
 void SampleAISDisplayModePackage::CreateLight(const Handle(AIS_InteractiveContext)& aContext) 
{
  aContext->OpenLocalContext();
}

//===============================================================
// Function name: SetFirstPointOfLight
//===============================================================
 void SampleAISDisplayModePackage::SetFirstPointOfLight(const Handle(AIS_InteractiveContext)& aContext,
							const Handle(V3d_View)& aView,
							const Standard_Integer X,
							const Standard_Integer Y) 
{
  p1 = ConvertClickToPoint(X,Y,aView);

  // Create a directional light
  aLight = new V3d_DirectionalLight(aView->Viewer(), p1.X(),p1.Y(),p1.Z(),0.,0.,1.);

  p2 = gp_Pnt(p1.X(),p1.Y(),p1.Z()+1.);

  BRepBuilderAPI_MakeEdge E(p1, p2);
  aShape = new AIS_Shape(E.Edge());

  aShape->SetColor(Quantity_NOC_YELLOW);
  aContext->Display(aShape);

  // Activate the light in the view
  aView->SetLightOn(aLight);
}

//===============================================================
// Function name: MoveSecondPointOfLight
//===============================================================
 void SampleAISDisplayModePackage::MoveSecondPointOfLight(const Handle(AIS_InteractiveContext)& aContext,
							  const Handle(V3d_View)& aView,
							  const Standard_Integer X,
							  const Standard_Integer Y) 
{
  p2 = ConvertClickToPoint(X,Y,aView);

  if (p1.Distance(p2)>Precision::Confusion()) {
    BRepBuilderAPI_MakeEdge E(p1, p2);
    aShape->Set(E.Edge());
    aContext->Redisplay(aShape);

    //Update the light dynamically
    aLight->SetDirection(p2.X()-p1.X(),p2.Y()-p1.Y(),p2.Z()-p1.Z());
    aView->UpdateLights();
  }
}

//===============================================================
// Function name: SetSecondPointOfLight
//===============================================================
 void SampleAISDisplayModePackage::SetSecondPointOfLight(const Handle(AIS_InteractiveContext)& aContext) 
{
  aContext->CloseLocalContext();
}

