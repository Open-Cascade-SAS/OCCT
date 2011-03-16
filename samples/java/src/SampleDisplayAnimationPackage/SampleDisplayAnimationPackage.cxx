#include <SampleDisplayAnimationPackage.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>


#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

#ifdef WNT
#include <WNT_Window.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#else
#include <Xw_Window.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#endif


/*----------------------------------------------------------------------*/


Handle(AIS_Shape) myAISCrankArm;
Handle(AIS_Shape) myAISCylinderHead;
Handle(AIS_Shape) myAISPropeller;
Handle(AIS_Shape) myAISPiston;
Handle(AIS_Shape) myAISEngineBlock;

Standard_Real myDeviation;
Standard_Integer myAngle;
Standard_Integer thread;



//===============================================================
// Function name: CreateViewer
//===============================================================
 Handle(V3d_Viewer) SampleDisplayAnimationPackage::CreateViewer(const Standard_ExtString aName) 
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
 void SampleDisplayAnimationPackage::SetWindow(const Handle(V3d_View)& aView,
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
// Function name: LoadData
//===============================================================
 Standard_Boolean SampleDisplayAnimationPackage::LoadData(const Handle(AIS_InteractiveContext)& aContext,
							  const Standard_CString aPath) 
{
  myDeviation = 0.0008;
  thread = 4;
  myAngle = 0;


  BRep_Builder B;
  TopoDS_Shape CrankArm;
  TopoDS_Shape CylinderHead;
  TopoDS_Shape Propeller;
  TopoDS_Shape Piston;
  TopoDS_Shape EngineBlock;
  TCollection_AsciiString aName;

  aName = aPath;  aName += "CrankArm.rle";
  BRepTools::Read(CrankArm, aName.ToCString(), B);
  aName = aPath;  aName += "CylinderHead.rle";
  BRepTools::Read(CylinderHead, aName.ToCString(), B);
  aName = aPath;  aName += "Propeller.rle";
  BRepTools::Read(Propeller, aName.ToCString(), B);
  aName = aPath;  aName += "Piston.rle";
  BRepTools::Read(Piston, aName.ToCString(), B);
  aName = aPath;  aName += "EngineBlock.rle";
  BRepTools::Read(EngineBlock, aName.ToCString(), B);

  if (CrankArm.IsNull() || CylinderHead.IsNull() || Propeller.IsNull() ||
      Piston.IsNull() || EngineBlock.IsNull())
    return Standard_False;


  aContext->SetDeviationCoefficient(myDeviation);

  myAISCylinderHead = new AIS_Shape(CylinderHead);
  aContext->SetColor(myAISCylinderHead, Quantity_NOC_WHITE);
  aContext->SetMaterial(myAISCylinderHead, Graphic3d_NOM_PLASTIC);

  myAISEngineBlock = new AIS_Shape(EngineBlock);
  aContext->SetColor(myAISEngineBlock, Quantity_NOC_WHITE);
  aContext->SetMaterial(myAISEngineBlock, Graphic3d_NOM_PLASTIC);

  aContext->Display(myAISCylinderHead, 1, -1, Standard_False, Standard_False);
  aContext->Display(myAISEngineBlock, 1, -1, Standard_False, Standard_False);

  myAISCrankArm = new AIS_Shape(CrankArm);
  aContext->SetColor(myAISCrankArm, Quantity_NOC_HOTPINK);
  aContext->SetMaterial(myAISCrankArm, Graphic3d_NOM_PLASTIC);

  myAISPiston = new AIS_Shape(Piston);
  aContext->SetColor(myAISPiston, Quantity_NOC_WHITE);
  aContext->SetMaterial(myAISPiston, Graphic3d_NOM_PLASTIC);

  myAISPropeller = new AIS_Shape(Propeller);
  aContext->SetColor(myAISPropeller, Quantity_NOC_RED);
  aContext->SetMaterial(myAISPropeller, Graphic3d_NOM_PLASTIC);

  aContext->Display(myAISCrankArm, 1, -1, Standard_False, Standard_False);
  aContext->Display(myAISPropeller, 1, -1, Standard_False, Standard_False);
  aContext->Display(myAISPiston, 1, -1, Standard_True, Standard_False);

  return Standard_True;
}

//===============================================================
// Function name: ChangePosition
//===============================================================
 void SampleDisplayAnimationPackage::ChangePosition(const Handle(AIS_InteractiveContext)& aContext) 
{
  Standard_Real angleA;
  Standard_Real angleB;
  Standard_Real X;
  gp_Ax1 ax1(gp_Pnt(0,0,0), gp_Vec(0,0,1));

  myAngle++;

  angleA = thread*myAngle*PI/180;
  X = Sin(angleA)*3/8;
  angleB = atan(X/Sqrt(-X*X+1));
  Standard_Real decal(25*0.6);


  // Build a transformation on the display
  gp_Trsf aPropellerTrsf;
  aPropellerTrsf.SetRotation(ax1, angleA);
  aContext->SetLocation(myAISPropeller, aPropellerTrsf);

  gp_Ax3 base(gp_Pnt(3*decal*(1-Cos(angleA)), -3*decal*Sin(angleA),0),
	      gp_Vec(0,0,1), gp_Vec(1,0,0));
  gp_Trsf aCrankArmTrsf;
  aCrankArmTrsf.SetTransformation(base.Rotated(gp_Ax1(gp_Pnt(3*decal,0,0), gp_Dir(0,0,1)), angleB));
  aContext->SetLocation(myAISCrankArm, aCrankArmTrsf);

  gp_Trsf aPistonTrsf;
  aPistonTrsf.SetTranslation(gp_Vec(-3*decal*(1-Cos(angleA))-8*decal*(1-Cos(angleB)),0,0));
  aContext->SetLocation(myAISPiston, aPistonTrsf);

  aContext->UpdateCurrentViewer();
}

//===============================================================
// Function name: GetDeviationCoefficient
//===============================================================
 Standard_Real SampleDisplayAnimationPackage::GetDeviationCoefficient() 
{
  return myDeviation;
}

//===============================================================
// Function name: SetDeviationCoefficient
//===============================================================
 void SampleDisplayAnimationPackage::SetDeviationCoefficient(const Handle(AIS_InteractiveContext)& aContext,
							     const Standard_Real aValue) 
{
  myDeviation = aValue;

  aContext->SetDeviationCoefficient(myDeviation);
  TopoDS_Shape Propeller = myAISPropeller->Shape();
  BRepTools::Clean(Propeller);

  myAISPropeller->Set(Propeller);
  aContext->Deactivate(myAISPropeller);
  aContext->Redisplay(myAISPropeller);
}

//===============================================================
// Function name: GetAngleIncrement
//===============================================================
 Standard_Integer SampleDisplayAnimationPackage::GetAngleIncrement() 
{
  return thread;
}

//===============================================================
// Function name: SetAngleIncrement
//===============================================================
 void SampleDisplayAnimationPackage::SetAngleIncrement(const Standard_Integer aValue) 
{
  thread = aValue;
}

//===============================================================
// Function name: SaveImage
//===============================================================
#ifndef WNT
 Standard_Boolean SampleDisplayAnimationPackage::SaveImage(const Standard_CString ,
							   const Standard_CString ,
							   const Handle(V3d_View)& ) 
{
#else
 Standard_Boolean SampleDisplayAnimationPackage::SaveImage(const Standard_CString aFileName,
							   const Standard_CString aFormat,
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

