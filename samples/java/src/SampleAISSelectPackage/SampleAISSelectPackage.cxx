#include <SampleAISSelectPackage.hxx>
#include <TCollection_AsciiString.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <User_Cylinder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <BRep_Tool.hxx>
#include <Quantity_Color.hxx>

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
 Handle(V3d_Viewer) SampleAISSelectPackage::CreateViewer (const Standard_ExtString aName) 
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
void SampleAISSelectPackage::SetWindow (const Handle(V3d_View)& aView,
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
// Function name: DisplayBox
//===============================================================
 void SampleAISSelectPackage::DisplayBox(const Handle(AIS_InteractiveContext)& aContext,
					 TCollection_AsciiString& Message) 
{
  aContext->CloseAllContexts();
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
// Function name: DisplayCylinder
//===============================================================
 void SampleAISSelectPackage::DisplayCylinder(const Handle(AIS_InteractiveContext)& aContext,
					      TCollection_AsciiString& Message) 
{
  aContext->CloseAllContexts();
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
// Function name: SelectVertices
//===============================================================
 void SampleAISSelectPackage::SelectVertices(const Handle(AIS_InteractiveContext)& aContext,
					     TCollection_AsciiString& Message) 
{
  aContext->CloseAllContexts();
  aContext->OpenLocalContext();
  aContext->ActivateStandardMode(TopAbs_VERTEX);

  Message = "\
		\n\
myAISContext->OpenLocalContext(); \n\
myAISContext->ActivateStandardMode(TopAbs_VERTEX); \n\
		\n";
}

//===============================================================
// Function name: SelectEdges
//===============================================================
 void SampleAISSelectPackage::SelectEdges(const Handle(AIS_InteractiveContext)& aContext,
					  TCollection_AsciiString& Message) 
{
  aContext->CloseAllContexts();
  aContext->OpenLocalContext();
  aContext->ActivateStandardMode(TopAbs_EDGE);

  Message = "\
		\n\
myAISContext->OpenLocalContext(); \n\
myAISContext->ActivateStandardMode(TopAbs_EDGE); \n\
		\n";
}

//===============================================================
// Function name: SelectFaces
//===============================================================
 void SampleAISSelectPackage::SelectFaces(const Handle(AIS_InteractiveContext)& aContext,
					  TCollection_AsciiString& Message) 
{
  aContext->CloseAllContexts();
  aContext->OpenLocalContext();
  aContext->ActivateStandardMode(TopAbs_FACE);

  Message = "\
		\n\
myAISContext->OpenLocalContext(); \n\
myAISContext->ActivateStandardMode(TopAbs_FACE); \n\
		\n";
}

//===============================================================
// Function name: SelectNeutral
//===============================================================
 void SampleAISSelectPackage::SelectNeutral(const Handle(AIS_InteractiveContext)& aContext,
					    TCollection_AsciiString& Message) 
{
  aContext->CloseAllContexts();

  Message = "\
		\n\
myAISContext->CloseAllContexts(); \n\
		\n";
}

//===============================================================
// Function name: MakeFillet
//===============================================================
 Standard_Integer SampleAISSelectPackage::MakeFillet(const Handle(AIS_InteractiveContext)& aContext,
						     const Standard_Real aValue,
						     TCollection_AsciiString& Message) 
{
  aContext->InitSelected();
  if(aContext->MoreSelected()) {
    Handle(AIS_Shape) S = Handle(AIS_Shape)::DownCast(aContext->SelectedInteractive());
	if(S.IsNull()) {
		return 2;
	}
    BRepFilletAPI_MakeFillet aFillet(S->Shape());

    for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected())
    {
		TopoDS_Shape aLocalShape = aContext->SelectedShape();
		TopoDS_Edge anEdge;
		if(aLocalShape.ShapeType() == TopAbs_EDGE) {
			anEdge = TopoDS::Edge(aLocalShape);
		}
	
		if (anEdge.IsNull())
			return 2;
		aFillet.Add(aValue, anEdge);
	}

    TopoDS_Shape aNewShape;
    try {
      aNewShape = aFillet.Shape();
    }
    catch(Standard_Failure) {
      return 1;
    }

    S->Set(aNewShape);
    aContext->Redisplay(S);

  Message = "\
		\n\
Handle(AIS_Shape) S = Handle(AIS_Shape)::DownCast(myAISContext->SelectedInteractive()); \n\
BRepFilletAPI_MakeFillet aFillet(S->Shape()); \n\
 \n\
for (myAISContext->InitSelected(); myAISContext->MoreSelected(); myAISContext->NextSelected()) \n\
{ \n\
  TopoDS_Edge anEdge = TopoDS::Edge(myAISContext->SelectedShape()); \n\
  aFillet.Add(aValue, anEdge); \n\
} \n\
 \n\
TopoDS_Shape aNewShape; \n\
 \n\
aNewShape = aFillet.Shape(); \n\
 \n\
S->Set(aNewShape); \n\
myAISContext->Redisplay(S); \n\
		\n";
    return 0;
  }
  return 2;
}

//===============================================================
// Function name: IsCylinderSelected
//===============================================================
 Standard_Boolean SampleAISSelectPackage::IsCylinderSelected(const Handle(AIS_InteractiveContext)& aContext) 
{
  Standard_Boolean result = Standard_False;

  aContext->InitCurrent();
  if (aContext->MoreCurrent()) {
    if (aContext->Current()->IsKind(STANDARD_TYPE(User_Cylinder)))
      result = Standard_True;
  }

  return result;
}

//===============================================================
// Function name: StartSelectFace
//===============================================================
 void SampleAISSelectPackage::StartSelectFace(const Handle(AIS_InteractiveContext)& aContext) 
{
  aContext->OpenLocalContext();
  aContext->Activate(aContext->Current(),4);
}

//===============================================================
// Function name: GetFaceColor
//===============================================================
 Quantity_Color SampleAISSelectPackage::GetFaceColor(const Handle(AIS_InteractiveContext)& aContext) 
{
  Quantity_Color aColor(Quantity_NOC_WHITE);

  aContext->InitSelected();
  if (aContext->MoreSelected()) {
    Handle(AIS_InteractiveObject) Current = aContext->SelectedInteractive();
    if (Current->HasColor())
      aColor = aContext->Color(Current);
  }

  return aColor;
}

//===============================================================
// Function name: SetFaceColor
//===============================================================
 void SampleAISSelectPackage::SetFaceColor(const Handle(AIS_InteractiveContext)& aContext,
					   const Quantity_Color& aColor,
					   TCollection_AsciiString& Message) 
{
  TopoDS_Shape S = aContext->SelectedShape();
  Handle(Geom_Surface) Surface = BRep_Tool::Surface(TopoDS::Face(S));
  if (Surface->IsKind(STANDARD_TYPE(Geom_Plane)))
    Handle(User_Cylinder)::DownCast(aContext->Current())->SetPlanarFaceColor(aColor.Name());
  else
    Handle(User_Cylinder)::DownCast(aContext->Current())->SetCylindricalFaceColor(aColor.Name());
  aContext->Redisplay(aContext->Current());

  Message = "\
		\n\
TopoDS_Shape S = myAISContext->SelectedShape(); \n\
Handle(Geom_Surface) Surface = BRep_Tool::Surface(TopoDS::Face(S)); \n\
 \n\
if (Surface->IsKind(STANDARD_TYPE(Geom_Plane))) \n\
  Handle(User_Cylinder)::DownCast(myAISContext->Current())->SetPlanarFaceColor(aColor.Name()); \n\
else \n\
  Handle(User_Cylinder)::DownCast(myAISContext->Current())->SetCylindricalFaceColor(aColor.Name()); \n\
 \n\
myAISContext->Redisplay(myAISContext->Current()); \n\
 \n\
 \n\
NOTE: a User_Cylinder is an object defined by the user. \n\
The User_Cylinder class inherits the AIS_InteractiveObject \n\
CASCADE class, its usage is the same as an AIS_InteractiveObject. \n\
Methods SetPlanarFaceColor and SetCylindricalFaceColor are also \n\
defined in the User_Cylinder class. \n\
		\n";
}


//===============================================================
// Function name: EndSelectFace
//===============================================================
 void SampleAISSelectPackage::EndSelectFace(const Handle(AIS_InteractiveContext)& aContext) 
{
  aContext->CloseLocalContext();
}
