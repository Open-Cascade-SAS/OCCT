#include <SampleViewer3DPackage.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <TCollection_AsciiString.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <V3d_Coordinate.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_SpotLight.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_Plane.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt2d.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>
#include <Precision.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Edge.hxx>


#ifdef WNT
#include <WNT_Window.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#else
#include <Xw_Window.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#endif  // WNT



/*----------------------------------------------------------------------*/
#ifdef WNT
static Handle(Graphic3d_WNTGraphicDevice) defaultdevice;
#else
static Handle(Graphic3d_GraphicDevice) defaultdevice;
#endif  // WNT


Handle(V3d_AmbientLight) myCurrent_AmbientLight;
Handle(V3d_SpotLight) myCurrent_SpotLight;
Handle(V3d_PositionalLight) myCurrent_PositionalLight;
Handle(V3d_DirectionalLight) myCurrent_DirectionalLight;
Handle(V3d_Plane) myPlane;
Handle(AIS_Shape) myShape;


gp_Pnt p1, p2, p3;
Handle(AIS_Shape) spotConeShape=new AIS_Shape(TopoDS_Solid());
Handle(AIS_Shape) directionalEdgeShape=new AIS_Shape(TopoDS_Edge());

Standard_Boolean isConeDisplayed = Standard_False;
Standard_Boolean isEdgeDisplayed = Standard_False;

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


//===============================================================
// Function name: CreateViewer
//===============================================================
 Handle(V3d_Viewer) SampleViewer3DPackage::CreateViewer(const Standard_ExtString aName) 
{
#ifndef WNT
  if (defaultdevice.IsNull()) defaultdevice = new Graphic3d_GraphicDevice("");
  return new V3d_Viewer(defaultdevice, aName);
#else
  if (defaultdevice.IsNull()) defaultdevice = new Graphic3d_WNTGraphicDevice();
  return new V3d_Viewer(defaultdevice, aName);
#endif //WNT
}

//===============================================================
// Function name: SetWindow
//===============================================================
void SampleViewer3DPackage::SetWindow (const Handle(V3d_View)& aView,
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

//===============================================================
// Function name: CreateGraphicDriver
//===============================================================
 Handle(Graphic3d_GraphicDriver) SampleViewer3DPackage::CreateGraphicDriver() 
{
#ifndef WNT
  if (defaultdevice.IsNull()) 
    defaultdevice = new Graphic3d_GraphicDevice("");
  return Handle(Graphic3d_GraphicDriver)::DownCast(defaultdevice->GraphicDriver());
#else
  if (defaultdevice.IsNull()) 
    defaultdevice = new Graphic3d_WNTGraphicDevice();
  return  Handle(Graphic3d_GraphicDriver)::DownCast(defaultdevice->GraphicDriver());
#endif //WNT
}



//======================================================================
//=                                                                    =
//=                          Display figures                           =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: DisplayBox
//===============================================================
 void SampleViewer3DPackage::DisplayBox(const Handle(AIS_InteractiveContext)& aContext) 
{
  BRepPrimAPI_MakeBox B(gp_Pnt(0,-400,-100), 200.,150.,100.);
  Handle(AIS_Shape) aBox = new AIS_Shape(B.Shape());
  aContext->Display(aBox);	
}

//===============================================================
// Function name: DisplayCylinder
//===============================================================
 void SampleViewer3DPackage::DisplayCylinder(const Handle(AIS_InteractiveContext)& aContext) 
{
  gp_Ax2 CylAx2(gp_Pnt(0,0,-100), gp_Dir(gp_Vec(gp_Pnt(0,0,-100),gp_Pnt(0,0,100))));
  BRepPrimAPI_MakeCylinder C(CylAx2, 80.,200.);
  Handle(AIS_Shape) aCyl = new AIS_Shape(C.Shape());
  aContext->SetColor(aCyl,Quantity_NOC_WHITE); 
  aContext->SetMaterial(aCyl,Graphic3d_NOM_SHINY_PLASTIC);    
  aContext->SetDisplayMode(aCyl,1);
  aContext->Display(aCyl);
}

//===============================================================
// Function name: DisplaySphere
//===============================================================
 void SampleViewer3DPackage::DisplaySphere(const Handle(AIS_InteractiveContext)& aContext) 
{
  BRepPrimAPI_MakeSphere S(gp_Pnt(0,300,0), 100.);
  Handle(AIS_Shape) aSphere = new AIS_Shape(S.Shape());
  aContext->SetMaterial(aSphere,Graphic3d_NOM_SILVER);    
  aContext->SetDisplayMode(aSphere,1);
  aContext->Display(aSphere);
}

//===============================================================
// Function name: EraseAll
//===============================================================
 void SampleViewer3DPackage::EraseAll(const Handle(AIS_InteractiveContext)& aContext) 
{
  aContext->EraseAll(Standard_False);
}


//======================================================================
//=                                                                    =
//=                               Spot Light                           =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: CreateSpotLight
//===============================================================
 void SampleViewer3DPackage::CreateSpotLight(const Handle(AIS_InteractiveContext)& aContext,
					     TCollection_AsciiString& Message) 
{
  // Spot light source creation
  aContext->OpenLocalContext();

  Message = "\
myCurrent_SpotLight = new V3d_SpotLight(myView->Viewer(), Xt, Yt, Zt, Xp, Yp, Zp,Quantity_NOC_RED);\n\
\n\
myView->SetLightOn(myCurrent_SpotLight);\n\
\n\
  ";
}

//===============================================================
// Function name: SetSpotLight
//===============================================================
 void SampleViewer3DPackage::SetSpotLight(const Handle(AIS_InteractiveContext)& aContext,
					  const Handle(V3d_View)& aView,
					  const Standard_Integer X,const Standard_Integer Y,
					  TCollection_AsciiString& Message) 
{
  p1 = ConvertClickToPoint(X,Y,aView);
  myCurrent_SpotLight = new V3d_SpotLight(aView->Viewer(),0.,0.,1., p1.X(),p1.Y(),p1.Z(),
					  Quantity_NOC_RED);
  aView->SetLightOn(myCurrent_SpotLight);

  p2 = gp_Pnt(p1.X(),p1.Y(),p1.Z()+1.);
  Standard_Real coneHeigth=p1.Distance(p2);
  BRepPrimAPI_MakeCone MakeCone(gp_Ax2(p1, gp_Dir(gp_Vec(p1, p2))), 
				0, (p1.Distance(p2))/tan(1.04), coneHeigth);
  spotConeShape->Set(MakeCone.Solid());

  if (isConeDisplayed)
    aContext->Redisplay(spotConeShape,0,-1);
  else
    isConeDisplayed = Standard_True;

  aContext->Display(spotConeShape,0,-1);

  Message = "\
myCurrent_SpotLight->SetDirection(Xv, Yv, Zv);\n\
  ";
}

//===============================================================
// Function name: DirectingSpotLight
//===============================================================
 void SampleViewer3DPackage::DirectingSpotLight(const Handle(AIS_InteractiveContext)& aContext,
						const Handle(V3d_View)& aView,
						const Standard_Integer X,const Standard_Integer Y) 
{
  p2 = ConvertClickToPoint(X,Y,aView);
  //Update the light dynamically
  Standard_Real coneHeigth=p1.Distance(p2);
  if( coneHeigth>Precision::Confusion())
    {
      BRepPrimAPI_MakeCone MakeCone(gp_Ax2(p1, gp_Dir(gp_Vec(p1, p2))), 
				    0, (p1.Distance(p2))/tan(1.04), coneHeigth);
      spotConeShape->Set(MakeCone.Solid());
      aContext->Redisplay(spotConeShape,0,-1);
      myCurrent_SpotLight->SetDirection(p2.X()-p1.X(),p2.Y()-p1.Y(),p2.Z()-p1.Z());
      aView->UpdateLights();
    }
}

//===============================================================
// Function name: DirectSpotLight
//===============================================================
 void SampleViewer3DPackage::DirectSpotLight(const Handle(V3d_View)& aView,
					     const Standard_Integer X,const Standard_Integer Y,
					     TCollection_AsciiString& Message) 
{
  p2 = ConvertClickToPoint(X,Y,aView);

  Message = "\
myCurrent_SpotLight->SetAngle(Angle) ;\n\
  ";
}

//===============================================================
// Function name: ExpandingSpotLight
//===============================================================
 void SampleViewer3DPackage::ExpandingSpotLight(const Handle(AIS_InteractiveContext)& aContext,
						const Handle(V3d_View)& aView,
						const Standard_Integer X,const Standard_Integer Y) 
{
  p3 = ConvertClickToPoint(X,Y,aView);
  //Update the light dynamically
  Standard_Real coneHeigth=p1.Distance(p2);
  if( (p2.Distance(p3))>Precision::Confusion())
    {
      BRepPrimAPI_MakeCone MakeCone(gp_Ax2(p1, gp_Dir(gp_Vec(p1, p2))), 
				    0, p2.Distance(p3), coneHeigth);
      spotConeShape->Set(MakeCone.Solid());
      aContext->Redisplay(spotConeShape,0,-1);
      myCurrent_SpotLight->SetAngle(atan(p2.Distance(p3)/p1.Distance(p2))) ;
      aView->UpdateLights();
    }
}

//===============================================================
// Function name: ExpandSpotLight
//===============================================================
 void SampleViewer3DPackage::ExpandSpotLight(const Handle(AIS_InteractiveContext)& aContext) 
{
  aContext->Erase(spotConeShape);
  aContext->CloseLocalContext();
}


//======================================================================
//=                                                                    =
//=                         Positional Light                           =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: CreatePositionalLight
//===============================================================
 void SampleViewer3DPackage::CreatePositionalLight(const Handle(AIS_InteractiveContext)& aContext,
						   const Handle(V3d_View)& aView,
						   TCollection_AsciiString& Message) 
{
  // Positional light source creation
  aContext->OpenLocalContext();
  myCurrent_PositionalLight=new V3d_PositionalLight(aView->Viewer(),0,0,0,Quantity_NOC_GREEN,1,0);
  aView->SetLightOn(myCurrent_PositionalLight);

  Message = "\
myCurrent_PositionalLight=new V3d_PositionalLight(myView->Viewer(),Xp, Yp, Zp,Quantity_NOC_GREEN,1,0);\n\
\n\
myView->SetLightOn(myCurrent_PositionalLight) ;\n\
  ";
}

//===============================================================
// Function name: DirectingPositionalLight
//===============================================================
 void SampleViewer3DPackage::DirectingPositionalLight(const Handle(V3d_View)& aView,
						      const Standard_Integer X,const Standard_Integer Y) 
{
  p2 = ConvertClickToPoint(X,Y,aView);
  //Update the light dynamically
  myCurrent_PositionalLight->SetPosition(p2.X(),p2.Y(),p2.Z());
  aView->UpdateLights();
}

//===============================================================
// Function name: DirectPositionalLight
//===============================================================
 void SampleViewer3DPackage::DirectPositionalLight(const Handle(AIS_InteractiveContext)& aContext,
						   const Handle(V3d_View)& aView,
						   const Standard_Integer X,const Standard_Integer Y,
						   TCollection_AsciiString& Message) 
{
  p1 = ConvertClickToPoint(X,Y,aView);
  myCurrent_PositionalLight->SetPosition(p1.X(),p1.Y(),p1.Z()) ;
  aContext->CloseLocalContext();

  Message = "\
myCurrent_PositionalLight->SetPosition(Xp, Yp, Zp) ;\n\
  ";
}


//======================================================================
//=                                                                    =
//=                        Directional Light                           =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: CreateDirectionalLight
//===============================================================
 void SampleViewer3DPackage::CreateDirectionalLight(const Handle(AIS_InteractiveContext)& aContext,
						    TCollection_AsciiString& Message) 
{
  // Directional light source creation
  aContext->OpenLocalContext();

  Message = "\
myCurrent_DirectionalLight = new V3d_DirectionalLight(myView->Viewer(), Xt, Yt, Zt, Xp, Yp, Zp);\n\
\n\
myView->SetLightOn(myCurrent_DirectionalLight);\n\
\n\
  ";
}

//===============================================================
// Function name: SetDirectionalLight
//===============================================================
 void SampleViewer3DPackage::SetDirectionalLight(const Handle(AIS_InteractiveContext)& aContext,
						 const Handle(V3d_View)& aView,
						 const Standard_Integer X,const Standard_Integer Y,
						 TCollection_AsciiString& Message) 
{
  p1 = ConvertClickToPoint(X,Y,aView);
  p2 = gp_Pnt(p1.X(),p1.Y(),p1.Z()+1.);
  BRepBuilderAPI_MakeEdge MakeEdge(p1, p2);
  directionalEdgeShape->Set(MakeEdge.Edge());

  if (isEdgeDisplayed)
    aContext->Redisplay(directionalEdgeShape,0,-1);
  else
    isEdgeDisplayed = Standard_True;

  aContext->Display(directionalEdgeShape,0,-1);

  // Create a directional light
  myCurrent_DirectionalLight = new V3d_DirectionalLight(aView->Viewer(), p1.X(),p1.Y(),p1.Z(),0.,0.,1.);
  aView->SetLightOn(myCurrent_DirectionalLight);
  
  Message = "\
myCurrent_DirectionalLight->SetDirection(Xv, Yv, Zv);\n\
  ";
}

//===============================================================
// Function name: DirectingDirectionalLight
//===============================================================
 void SampleViewer3DPackage::DirectingDirectionalLight(const Handle(AIS_InteractiveContext)& aContext,
						       const Handle(V3d_View)& aView,
						       const Standard_Integer X,const Standard_Integer Y) 
{
  p2 = ConvertClickToPoint(X,Y,aView);
  //Update the light dynamically
  if( p1.Distance(p2)>Precision::Confusion())
    {
      BRepBuilderAPI_MakeEdge MakeEdge(p1, p2);
      directionalEdgeShape->Set(MakeEdge.Edge());
      aContext->Redisplay(directionalEdgeShape,0,-1);
      myCurrent_DirectionalLight->SetDirection(p2.X()-p1.X(),p2.Y()-p1.Y(),p2.Z()-p1.Z());
      aView->UpdateLights();
    }
}

//===============================================================
// Function name: DirectDirectionalLight
//===============================================================
 void SampleViewer3DPackage::DirectDirectionalLight(const Handle(AIS_InteractiveContext)& aContext) 
{
  aContext->Erase(directionalEdgeShape);
  aContext->CloseLocalContext();
}


//======================================================================
//=                                                                    =
//=                          Ambient Light                             =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: CreateAmbientLight
//===============================================================
 void SampleViewer3DPackage::CreateAmbientLight(const Handle(AIS_InteractiveContext)& aContext,
						const Handle(V3d_View)& aView,
						TCollection_AsciiString& Message) 
{
  // Ambient light source creation
  aContext->OpenLocalContext();
  myCurrent_AmbientLight=new V3d_AmbientLight(aView->Viewer(), Quantity_NOC_GRAY);
  aView->SetLightOn(myCurrent_AmbientLight) ;	
  aContext->CloseLocalContext();

  aView->UpdateLights();

  Message = "\
myCurrent_AmbientLight=new V3d_AmbientLight(myView->Viewer(), Quantity_NOC_GRAY);\n\
\n\
myView->SetLightOn(myCurrent_AmbientLight) ;\n\
  ";
}

//===============================================================
// Function name: ClearLights
//===============================================================
 void SampleViewer3DPackage::ClearLights(const Handle(V3d_View)& aView,
					 TCollection_AsciiString& Message) 
{
  // Setting Off all non global active lights
  for(aView->InitActiveLights(); aView->MoreActiveLights(); aView->NextActiveLights())
    {
      if (!aView->Viewer()->IsGlobalLight(aView->ActiveLight())) 
	aView->SetLightOff(aView->ActiveLight());
    }

  aView->Update();

  Message = "\
for(myView->InitActiveLights(); myView->MoreActiveLights(); myView->NextActiveLights())\n\
   {\n\
      if (!aView->Viewer()->IsGlobalLight(aView->ActiveLight()))\n\
	myView->SetLightOff(myView->ActiveLight()); //Setting Off all non global active lights\n\
   }\n\
  ";
}


//======================================================================
//=                                                                    =
//=                             ZClipping                              =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: ChangeZClippingDepth
//===============================================================
 void SampleViewer3DPackage::ChangeZClippingDepth(const Handle(V3d_View)& aView,
						  const Quantity_Length Depth,
						  TCollection_AsciiString& Message) 
{
  // Setting the ZClipping depth at Depth value
  aView->SetZClippingDepth(Depth);

  aView->Update();

  Message = "\
myView->SetZClippingDepth(Depth);\n\
\n\
myView->Update();\n\
  ";
}

//===============================================================
// Function name: ChangeZClippingWidth
//===============================================================
 void SampleViewer3DPackage::ChangeZClippingWidth(const Handle(V3d_View)& aView,
						  const Quantity_Length Width,
						  TCollection_AsciiString& Message) 
{
  // Setting the ZClipping width at Width value
  aView->SetZClippingWidth(Width);

  aView->Update();

  Message = "\
myView->SetZClippingWidth(Width);\n\
\n\
myView->Update();\n\
  ";
}

//===============================================================
// Function name: ChangeZClippingType
//===============================================================
 void SampleViewer3DPackage::ChangeZClippingType(const Handle(V3d_View)& aView,
						 const V3d_TypeOfZclipping Type,
						 TCollection_AsciiString& Message) 
{
  // Setting the ZClipping type at Type value
  aView->SetZClippingType(Type);
  aView->Update();

  Message = "\
aView->SetZClippingType(Type);\n\
\n\
aView->Update();\n\
  ";
}


//======================================================================
//=                                                                    =
//=                               ZCueing                              =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: ChangeZCueingDepth
//===============================================================
 void SampleViewer3DPackage::ChangeZCueingDepth(const Handle(V3d_View)& aView,
						const Quantity_Length Depth,
						TCollection_AsciiString& Message) 
{
  // Setting the ZCueing depth at Depth value
  aView->SetZCueingDepth(Depth);

  aView->Update();

  Message = "\
myView->SetZCueingDepth(Depth);\n\
\n\
myView->Update();\n\
  ";
}

//===============================================================
// Function name: ChangeZCueingWidth
//===============================================================
 void SampleViewer3DPackage::ChangeZCueingWidth(const Handle(V3d_View)& aView,
						const Quantity_Length Width,
						TCollection_AsciiString& Message) 
{
  // Setting the ZCueing width at Width value
  aView->SetZCueingWidth(Width);

  aView->Update();

  Message = "\
myView->SetZCueingWidth(Width);\n\
\n\
myView->Update();\n\
  ";
}

//===============================================================
// Function name: ChangeZCueingOnOff
//===============================================================
 void SampleViewer3DPackage::ChangeZCueingOnOff(const Handle(V3d_View)& aView,
						const Standard_Boolean IsOn,
						TCollection_AsciiString& Message) 
{
  Quantity_Length Depth, Width;

  if(IsOn)
    {
      if(!aView->ZCueing(Depth, Width))
	// Setting the ZCueing on if it's not yet
	aView->SetZCueingOn();
    }
  else
    {
      if(aView->ZCueing(Depth, Width))
	// Setting the ZCueing off if it's not yet
	aView->SetZCueingOff();
    }

  aView->Update();


  Message = "\
if(!myView->ZCueing(Depth, Width));\n\
	myView->SetZCueingOn();\n\
else\n\
	myView->SetZCueingOff();\n\
\n\
myView->Update();\n\
  ";
}


//======================================================================
//=                                                                    =
//=                            Other modes                             =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: ChangeShadingModel
//===============================================================
 void SampleViewer3DPackage::ChangeShadingModel(const Handle(V3d_View)& aView,
						const V3d_TypeOfShadingModel Model) 
{
  aView->SetShadingModel(Model);
  aView->Update();	
}

//===============================================================
// Function name: ChangeAntialiasing
//===============================================================
 void SampleViewer3DPackage::ChangeAntialiasing(const Handle(V3d_View)& aView,
						TCollection_AsciiString& Message) 
{
  if(!aView->Antialiasing())
    aView->SetAntialiasingOn();
  else
    aView->SetAntialiasingOff();
  
  aView->Update();

  Message = "\
if(!myView->Antialiasing())\n\
	myView->SetAntialiasingOn();\n\
else\n\
	myView->SetAntialiasingOff();\n\
  ";
}


//======================================================================
//=                                                                    =
//=                          Model Clipping                            =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: CreateClippingPlane
//===============================================================
 void SampleViewer3DPackage::CreateClippingPlane(const Handle(V3d_Viewer)& aViewer) 
{
  if( myPlane.IsNull() ) 
    {
      //creates a plane defined : center of the box ( 50,50,50) and 1 direction
      gp_Pln tmpPln(gp_Pnt(0,0,0),gp_Dir(1,0,0));
      //getting the coefficients of the gp_Pln ( ax+by+cz+d = 0 )
      Standard_Real A,B,C,D;
      tmpPln.Coefficients(A,B,C,D);
      //with these coefficients, creating a V3d_Plane
      myPlane = new V3d_Plane(aViewer,A,B,C,D);
      //creates the Face
      //NOTE : the face must be behind the clipping plane !!	  
      tmpPln = gp_Pln(gp_Pnt(0.1,0,0),gp_Dir(1,0,0));
      BRepBuilderAPI_MakeFace MakeFace(tmpPln, 200, -200, 410, -410);
      TopoDS_Face S = MakeFace.Face();
      //display the face
      myShape = new AIS_Shape(S);
    }
}

//===============================================================
// Function name: DisplayClippingPlane
//===============================================================
 void SampleViewer3DPackage::DisplayClippingPlane(const Handle(AIS_InteractiveContext)& aContext,
						  const Handle(V3d_View)& aView,
						  Standard_Real& Z,Standard_Boolean& IsOn) 
{
  Standard_Real A,B,C,D;
  myPlane->Plane(A,B,C,D);
  Z = D;

  Handle(V3d_Plane) thePlane;
  for (aView->InitActivePlanes(); 
       aView->MoreActivePlanes(); 
       aView->NextActivePlanes()) {
    thePlane = aView->ActivePlane() ;
    if (thePlane == myPlane) IsOn = Standard_True;
  }

  if (IsOn)
    if (!myShape.IsNull())
      aContext->Display(myShape);
}

//===============================================================
// Function name: ChangeModelClippingZ
//===============================================================
 void SampleViewer3DPackage::ChangeModelClippingZ(const Handle(AIS_InteractiveContext)& aContext,
						  const Handle(V3d_View)& aView,const Standard_Real Z,
						  const Standard_Boolean IsOn,
						  TCollection_AsciiString& Message) 
{
  // Setting the ModelClippingZ at Z value
  gp_Pln  clipPln(gp_Pnt(-Z,0,0),gp_Dir(1,0,0));

  Standard_Real A,B,C,D;
  clipPln.Coefficients(A,B,C,D);
  myPlane->SetPlane(A,B,C,D);

  if(IsOn)
    aView->SetPlaneOn(myPlane); 

  gp_Trsf myTrsf;
  myTrsf.SetTranslation(gp_Pnt(Z,0,0), gp_Pnt(0,0,0));
  aContext->SetLocation(myShape,TopLoc_Location(myTrsf)) ;
  aContext->Redisplay(myShape);
  aView->Update();

  Message = "\
gp_Pln  clipPln(gp_Pnt(-Z,0,0),gp_Dir(1,0,0));\n\
Standard_Real A,B,C,D;\n\
clipPln.Coefficients(A,B,C,D);\n\
myPlane->SetPlane(A,B,C,D);\n\
myView->SetPlaneOn(myPlane); \n\
  ";
}

//===============================================================
// Function name: ChangeModelClippingOnOff
//===============================================================
 void SampleViewer3DPackage::ChangeModelClippingOnOff(const Handle(AIS_InteractiveContext)& aContext,
						      const Handle(V3d_View)& aView,
						      const Standard_Boolean IsOn,
						      TCollection_AsciiString& Message) 
{
  if (IsOn)
    {
      //activate the plane
      aView->SetPlaneOn(myPlane);
      aContext->Display(myShape);
    }
  else
    //deactivate the plane
    {
      aView->SetPlaneOff(myPlane);
      aContext->Erase(myShape);
    }

  aView->Update();

  Message = "\
if(IsOn) \n\
	myView->SetPlaneOn(myPlane); \n\
else \n\
	myView->SetPlaneOff(myPlane); \n\
  ";
}

//===============================================================
// Function name: ClearClippingPlane
//===============================================================
 void SampleViewer3DPackage::ClearClippingPlane(const Handle(AIS_InteractiveContext)& aContext,
						const Handle(V3d_View)& aView,
						const Standard_Boolean IsOn) 
{
  if (IsOn)
    //deactivate the plane
    aView->SetPlaneOff(myPlane);

  if(!myShape.IsNull())
    aContext->Erase(myShape);
  
  if (IsOn)
    aView->Update();
}

