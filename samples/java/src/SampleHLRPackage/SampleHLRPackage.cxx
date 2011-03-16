// File:	SampleHLRPackage.cxx
// Created:	Mon Mar  6 14:52:10 2000
// Author:	UI team
//		<ui@flox.nnov.matra-dtv.fr>


#include <SampleHLRPackage.hxx>

#include <Aspect_Window.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <V2d_Viewer.hxx>
#include <V2d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ISession2D_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>
#include <ISession2D_Shape.hxx>
#include <Quantity_Color.hxx>
#include <Aspect_ColorMap.hxx>
#include <Aspect_GenericColorMap.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_WidthMap.hxx>
#include <Aspect_WidthMapEntry.hxx>
#include <Aspect_TypeMap.hxx>
#include <Aspect_TypeMapEntry.hxx>
#include <Aspect_LineStyle.hxx>
#include <TColQuantity_Array1OfLength.hxx>
#include <Geom_Axis2Placement.hxx>
#include <gp.hxx>
#include <V3d_Coordinate.hxx>
#include <Prs3d_Projector.hxx>
#include <HLRAlgo_Projector.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

#ifdef WNT
#include <WNT_Window.hxx>
#include <WNT_GraphicDevice.hxx>
#include <WNT_WDriver.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#else
#include <Xw_Window.hxx>
#include <Xw_GraphicDevice.hxx>
#include <Xw_Driver.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#endif



/*----------------------------------------------------------------------*/
#ifdef WNT
static Handle(Graphic3d_WNTGraphicDevice) defaultDevice;
static Handle(WNT_GraphicDevice) default2dDevice;
#else
static Handle(Graphic3d_GraphicDevice) defaultDevice;
static Handle(Xw_GraphicDevice) default2dDevice;
#endif 


Handle(AIS_Trihedron) myTrihedron;
Handle(ISession2D_Shape) myDisplayableShape;


//===============================================================
// Function name: CreateViewer3d
//===============================================================
 Handle(V3d_Viewer) SampleHLRPackage::CreateViewer3d(const Standard_ExtString aName) 
{
#ifdef WNT
  if (defaultDevice.IsNull()) 
    defaultDevice = new Graphic3d_WNTGraphicDevice();
  return new V3d_Viewer(defaultDevice, aName);
#else
  if (defaultDevice.IsNull()) 
    defaultDevice = new Graphic3d_GraphicDevice("");
  return new V3d_Viewer(defaultDevice, aName);
#endif //WNT
}

//===============================================================
// Function name: SetWindow3d
//===============================================================
void SampleHLRPackage::SetWindow3d (const Handle(V3d_View)& aView,
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
// Function name: CreateViewer2d
//===============================================================
Handle(V2d_Viewer) SampleHLRPackage::CreateViewer2d (const Standard_ExtString aName)
{
#ifdef WNT
  if(default2dDevice.IsNull()) 
    default2dDevice = new WNT_GraphicDevice();
#else
  if(default2dDevice.IsNull()) 
    default2dDevice = new Xw_GraphicDevice("",Xw_TOM_READONLY);
#endif
  return new V2d_Viewer(default2dDevice,aName,"");
}


//===============================================================
// Function name: CreateView2d
//===============================================================
Handle(V2d_View) SampleHLRPackage::CreateView2d (const Handle(V2d_Viewer)& aViewer,
						      const Standard_Integer hiwin,
						      const Standard_Integer lowin)
{
#ifdef WNT
  Handle(WNT_GraphicDevice) GD = Handle(WNT_GraphicDevice)::DownCast(aViewer->Device());
  Handle(WNT_Window) W = new WNT_Window(GD,hiwin,lowin,Quantity_NOC_MATRAGRAY);
  Handle(WNT_WDriver) D = new WNT_WDriver(W);
#else
  Handle(Xw_GraphicDevice) GD = Handle(Xw_GraphicDevice)::DownCast(aViewer->Device());
  Handle(Xw_Window) W = new Xw_Window(GD,hiwin,lowin,Xw_WQ_DRAWINGQUALITY,Quantity_NOC_MATRAGRAY);
  Handle(Xw_Driver) D = new Xw_Driver(W);
#endif
  
  Handle(V2d_View) V = new V2d_View(D,aViewer);
  V->Update();
  return V;
}


//===============================================================
// Function name: InitMaps
//===============================================================
 void SampleHLRPackage::InitMaps(const Handle(V2d_Viewer)& aViewer) 
{
  // update the Maps :
  //  entries are reserve for utilisation :
  //   - 1 for Visible edges HighLighted
  //   - 2 for Visible edges 
  //   - 3 for Hidden  edges HighLighted
  //   - 4 for Hidden  edges 
  
  Handle(Aspect_GenericColorMap) aColorMap = Handle(Aspect_GenericColorMap)::DownCast(aViewer->ColorMap());
  if (!aColorMap.IsNull()) {
    aColorMap->AddEntry(Aspect_ColorMapEntry (1,Quantity_Color(Quantity_NOC_RED  ))); // in fact just update
    aColorMap->AddEntry(Aspect_ColorMapEntry (2,Quantity_Color(Quantity_NOC_WHITE))); // in fact just update
    aColorMap->AddEntry(Aspect_ColorMapEntry (3,Quantity_Color(Quantity_NOC_RED  ))); // in fact just update
    aColorMap->AddEntry(Aspect_ColorMapEntry (4,Quantity_Color(Quantity_NOC_BLUE1))); // in fact just update
    aViewer->SetColorMap(aColorMap);
  }

  Handle(Aspect_WidthMap) aWidthMap = aViewer->WidthMap();
  aWidthMap->AddEntry(Aspect_WidthMapEntry(1,0.8));  // in fact just update
  aWidthMap->AddEntry(Aspect_WidthMapEntry(2,0.4));  // in fact just update
  aWidthMap->AddEntry(Aspect_WidthMapEntry(3,0.6));  // in fact just update
  aWidthMap->AddEntry(Aspect_WidthMapEntry(4,0.2));  // in fact just update
  aViewer->SetWidthMap(aWidthMap);

  Handle(Aspect_TypeMap) aTypeMap = aViewer->TypeMap();
  aTypeMap->AddEntry(Aspect_TypeMapEntry(1,Aspect_LineStyle(Aspect_TOL_SOLID)));
  aTypeMap->AddEntry(Aspect_TypeMapEntry(2,Aspect_LineStyle(Aspect_TOL_SOLID)));
  TColQuantity_Array1OfLength anArray(1,2);
  anArray(1) = 0.5;    anArray(2) = 0.5;
  aTypeMap->AddEntry(Aspect_TypeMapEntry(3,Aspect_LineStyle(anArray)));
  aTypeMap->AddEntry(Aspect_TypeMapEntry(4,Aspect_LineStyle(anArray)));
  aViewer->SetTypeMap(aTypeMap);
}

//===============================================================
// Function name: DisplayTrihedron
//===============================================================
 void SampleHLRPackage::DisplayTrihedron(const Handle(AIS_InteractiveContext)& aContext) 
{
  Handle(Geom_Axis2Placement) aTrihedronAxis=new Geom_Axis2Placement(gp::XOY());
  myTrihedron=new AIS_Trihedron(aTrihedronAxis);

  aContext->Display(myTrihedron);
}

//===============================================================
// Function name: GetShapes
//===============================================================
 Standard_Boolean SampleHLRPackage::GetShapes(const Handle(AIS_InteractiveContext)& aSrcContext,
					      const Handle(AIS_InteractiveContext)& aDestContext)
{
  myDisplayableShape = new ISession2D_Shape();

  aDestContext->EraseAll(Standard_False);
  aDestContext->Display(myTrihedron);

  Standard_Boolean OneOrMoreFound = Standard_False;
  for (aSrcContext->InitCurrent();aSrcContext->MoreCurrent();aSrcContext->NextCurrent())
  {
    Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(aSrcContext->Current());

    if (!anAISShape.IsNull())
      {
        OneOrMoreFound = Standard_True;
        TopoDS_Shape aShape = anAISShape->Shape();
        myDisplayableShape->Add(aShape);
	aDestContext->Display(anAISShape);
      }
  }
  return OneOrMoreFound;
}


//===============================================================
// Function name: Apply
//===============================================================
 void SampleHLRPackage::Apply(const Handle(ISession2D_InteractiveContext)& aContext2d,
			      const Standard_Integer aDisplayMode) 
{
  aContext2d->EraseAll(Standard_False);
  aContext2d->Display(myDisplayableShape,  // object
		      aDisplayMode,        // display mode
		      aDisplayMode,        // selection mode 
		      Standard_True);      // Redraw
}

//===============================================================
// Function name: UpdateProjector
//===============================================================
 void SampleHLRPackage::UpdateProjector(const Handle(V3d_View)& aView) 
{
  V3d_Coordinate DX,DY,DZ,XAt,YAt,ZAt, Vx,Vy,Vz ; 
  aView->Proj(DX,DY,DZ); 
  aView->At(XAt,YAt,ZAt); 
  aView->Up( Vx,Vy,Vz );
  Standard_Boolean IsPerspective = (aView->Type() == V3d_PERSPECTIVE);
  Quantity_Length aFocus = 1;
  Prs3d_Projector aPrs3dProjector(IsPerspective,aFocus,DX,DY,DZ,XAt,YAt,ZAt,Vx,Vy,Vz);
  HLRAlgo_Projector aProjector = aPrs3dProjector.Projector();
  
  if (myDisplayableShape.IsNull()) return;
  myDisplayableShape->SetProjector(aProjector);
}

//===============================================================
// Function name: SetNbIsos
//===============================================================
 void SampleHLRPackage::SetNbIsos(const Standard_Integer aNbIsos) 
{
  myDisplayableShape->SetNbIsos(aNbIsos);
}

//===============================================================
// Function name: ReadBRep
//===============================================================
 Standard_Boolean SampleHLRPackage::ReadBRep(const Standard_CString aFileName,
					     const Handle(AIS_InteractiveContext)& aContext) 
{
  TopoDS_Shape aShape;
  BRep_Builder aBuilder;
  Standard_Boolean result = BRepTools::Read(aShape,aFileName,aBuilder);
  if (result) 
    aContext->Display(new AIS_Shape(aShape));
  return result;
}

//===============================================================
// Function name: SaveBRep
//===============================================================
 Standard_Boolean SampleHLRPackage::SaveBRep(const Standard_CString aFileName,
					     const Handle(AIS_InteractiveContext)& aContext) 
{
  TopoDS_Shape aShape;
  Standard_Boolean isFound = Standard_False;

  Handle(AIS_InteractiveObject) picked;
  for(aContext->InitCurrent();aContext->MoreCurrent();aContext->NextCurrent()) {
    picked = aContext->Current();
    if (aContext->Current()->IsKind(STANDARD_TYPE(AIS_Shape))) {
      aShape = Handle(AIS_Shape)::DownCast(picked)->Shape();
      isFound = Standard_True;
      break;
    }
  }

  if (isFound) {
    Standard_Boolean result = BRepTools::Write(aShape,aFileName); 
    return result;
  }

  return Standard_False;
}

//===============================================================
// Function name: SaveImage
//===============================================================
#ifndef WNT
 Standard_Boolean SampleHLRPackage::SaveImage(const Standard_CString ,
					      const Standard_CString ,
					      const Handle(V3d_View)& ) 
{
#else
 Standard_Boolean SampleHLRPackage::SaveImage(const Standard_CString  aFileName,
					      const Standard_CString  aFormat,
					      const Handle(V3d_View)& aView) 
{
  Handle(Aspect_Window) anAspectWindow = aView->Window();
  Handle(WNT_Window) aWNTWindow = Handle(WNT_Window)::DownCast(anAspectWindow);

  if (aFormat == "bmp") aWNTWindow->SetOutputFormat(WNT_TOI_BMP);
  if (aFormat == "gif") aWNTWindow->SetOutputFormat(WNT_TOI_GIF);
  if (aFormat == "xwd") aWNTWindow->SetOutputFormat(WNT_TOI_XWD);

  aWNTWindow->Dump(aFileName);
#endif
  return Standard_True;
}

//===============================================================
// Function name: SaveImage
//===============================================================
#ifndef WNT
 Standard_Boolean SampleHLRPackage::SaveImage(const Standard_CString ,
					      const Standard_CString ,
					      const Handle(V2d_View)& ) 
{
#else
 Standard_Boolean SampleHLRPackage::SaveImage(const Standard_CString  aFileName,
					      const Standard_CString  aFormat,
					      const Handle(V2d_View)& aView) 
{
  Handle(Aspect_Window) anAspectWindow = aView->Driver()->Window();
  Handle(WNT_Window) aWNTWindow = Handle(WNT_Window)::DownCast(anAspectWindow);

  if (aFormat == "bmp") aWNTWindow->SetOutputFormat(WNT_TOI_BMP);
  if (aFormat == "gif") aWNTWindow->SetOutputFormat(WNT_TOI_GIF);
  if (aFormat == "xwd") aWNTWindow->SetOutputFormat(WNT_TOI_XWD);

  aWNTWindow->Dump(aFileName);
#endif
  return Standard_True;
}

