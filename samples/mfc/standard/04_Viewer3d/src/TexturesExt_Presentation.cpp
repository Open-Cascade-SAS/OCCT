// TexturesExt_Presentation.cpp: implementation of the TexturesExt_Presentation class.
// Creation of textural presentation of shape
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TexturesExt_Presentation.h"
#include "Viewer3dApp.h"

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <AIS_Shape.hxx>
#include <BRepTools.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <Geom_Surface.hxx>
#include <TopoDS_Face.hxx>
#include <V3d_DirectionalLight.hxx>

#define DISP(OBJ) getAISContext()->Display((OBJ), Standard_False)

// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new TexturesExt_Presentation;

// Initialization of array of samples
const TexturesExt_Presentation::PSampleFuncType TexturesExt_Presentation::SampleFuncs[] =
{
  &TexturesExt_Presentation::sampleBottle,
  &TexturesExt_Presentation::sampleTerrain,
  &TexturesExt_Presentation::sampleKitchen
};

#define EOL "\r\n"

#define ZVIEW_SIZE 100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TexturesExt_Presentation::TexturesExt_Presentation()
{
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Textured Shapes");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void TexturesExt_Presentation::DoSample()
{
	((CViewer3dApp*) AfxGetApp())->SetSampleName (L"Viewer3d");
	((CViewer3dApp*) AfxGetApp())->SetSamplePath (L"..\\..\\04_Viewer3d");
	getAISContext()->EraseAll (Standard_True);
	if (myIndex >=0 && myIndex < myNbSamples)
	{
	  // turn lights on for terrain sample
	  lightsOnOff(myIndex==1);
	  (this->*SampleFuncs[myIndex])();
	}
}

void TexturesExt_Presentation::Init()
{
  // initialize v3d_view so it displays TexturesExt well
  getViewer()->InitActiveViews();
  Handle(V3d_View) aView = getViewer()->ActiveView();
  aView->SetSize(ZVIEW_SIZE);

//  getDocument()->UpdateResultMessageDlg("Textured Shape", 
  TCollection_AsciiString Message ("Textured Shape", 
    "  TopoDS_Shape aShape;" EOL
    "" EOL
    "  // initialize aShape" EOL
    "  // aShape = ..." EOL
    "" EOL
    "  // create a textured presentation object for aShape" EOL
    "  Handle(AIS_Shape) aTShape = new AIS_Shape(aShape);" EOL
    "" EOL
    "  TCollection_AsciiString aTFileName;" EOL
    "" EOL
    "  // initialize aTFileName with an existing texture file name" EOL
    "  // (gif, bmp, xwd, rgb, and other formats are supported)" EOL
    "  // OR with an integer value string (max = Graphic3d_Texture2D::NumberOfTexturesExt())" EOL
    "  // which will indicate use of predefined texture of this number" EOL
    "  // aTFileName = ..." EOL
    "" EOL
    "  aTShape->Attributes()->SetShadingAspect (new Prs3d_ShadingAspect());" EOL
    "  Handle(Graphic3d_Texture2Dmanual) aTexture = new Graphic3d_Texture2Dmanual (aTFileName);" EOL
    "  aTShape->Attributes()->ShadingAspect()->Aspect()->SetTextureMap (aTexture);" EOL
    "  aTShape->Attributes()->ShadingAspect()->Aspect()->SetTextureMapOn();" EOL);
//	CString text(Message.ToCString());
  	getDocument()->ClearDialog();
	getDocument()->SetDialogTitle("Change face color");
  	getDocument()->AddTextInDialog(Message);
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////
//================================================================
// Function : TexturesExt_Presentation::Texturize
// display an AIS_Shape based on a given shape with texture with given filename
// filename can also be an integer value ("2", "5", etc.), in this case
// a predefined texture from Graphic3d_NameOfTexture2D with number = this value
// is loaded.
//================================================================
Handle(AIS_Shape) TexturesExt_Presentation::Texturize(const TopoDS_Shape& aShape,
                                                        TCollection_AsciiString aTFileName,
                                                        Standard_Real toScaleU,
                                                        Standard_Real toScaleV,
                                                        Standard_Real toRepeatU,
                                                        Standard_Real toRepeatV,
                                                        Standard_Real originU,
                                                        Standard_Real originV)
{
  // create a textured presentation object for aShape
  Handle(AIS_Shape) aTShape = new AIS_Shape(aShape);
  TCollection_AsciiString TFileName;
  // load texture from file if it is not an integer value
  // integer value indicates a number of texture in predefined TexturesExt enumeration
  CString anOCCTDataPathValue;
  anOCCTDataPathValue.GetEnvironmentVariable(L"CSF_OCCTDataPath");
  CString initfile = (anOCCTDataPathValue + L"\\images\\");
  if (!aTFileName.IsIntegerValue())
  {
    initfile += aTFileName.ToCString();
  }

  if (!aTShape->Attributes()->HasOwnShadingAspect())
  {
    aTShape->Attributes()->SetShadingAspect (new Prs3d_ShadingAspect());
  }
  aTShape->Attributes()->ShadingAspect()->Aspect()->SetTextureMap (new Graphic3d_Texture2Dmanual (TCollection_AsciiString ((const wchar_t* )initfile)));
  aTShape->Attributes()->ShadingAspect()->Aspect()->SetTextureMapOn();

  // do other initialization of AIS_Shape
  aTShape->SetTextureScaleUV (gp_Pnt2d ( toScaleU, toScaleV));
  aTShape->SetTextureRepeatUV(gp_Pnt2d (toRepeatU, toRepeatV));
  aTShape->SetTextureOriginUV(gp_Pnt2d (  originU, originV));

  aTShape->SetDisplayMode(AIS_Shaded);

  return aTShape;
}


//================================================================
// Function : TexturesExt_Presentation::loadShape
// loads a shape from a given brep file from data dir into a given TopoDS_Shape object
//================================================================
Standard_Boolean TexturesExt_Presentation::loadShape(TopoDS_Shape& aShape, 
                                         TCollection_AsciiString aFileName)
{
  // create a TopoDS_Shape -> read from a brep file
  CString anOCCTDataPathValue;
  anOCCTDataPathValue.GetEnvironmentVariable(L"CSF_OCCTDataPath");
  CString initfile = (anOCCTDataPathValue + L"\\occ\\" + aFileName.ToCString());

  std::filebuf aFileBuf;
  std::istream aStream (&aFileBuf);
  if (!aFileBuf.open (initfile, ios::in))
  {
    initfile += L" was not found. The sample can not be shown.";
    getDocument()->UpdateResultMessageDlg ("Textured Shape", initfile);
    return Standard_False;
  }

  BRep_Builder aBld;
  BRepTools::Read (aShape, aStream, aBld);
  if (aShape.IsNull())
  {
    initfile += " is invalid. The sample can not be shown.";
    getDocument()->UpdateResultMessageDlg ("Textured Shape", initfile);
    return Standard_False;
  }

  return Standard_True;
}

//================================================================
// Function : lightsOnOff
// Purpose  : 6 lights are used for a brighter demonstration of textured shapes
//            call lightsOnOff(false) before showing normal shape
//            call lightsOnOff(true)  before showing textured shape
//================================================================
void TexturesExt_Presentation::lightsOnOff(Standard_Boolean isOn)
{
  static Handle(V3d_Light) aLight1 = new V3d_DirectionalLight(getViewer(), V3d_XnegYposZneg);
  static Handle(V3d_Light) aLight2 = new V3d_DirectionalLight(getViewer(), V3d_XnegYnegZpos);
  static Handle(V3d_Light) aLight3 = new V3d_DirectionalLight(getViewer(), V3d_XposYnegZpos);
  static Handle(V3d_Light) aLight4 = new V3d_DirectionalLight(getViewer(), V3d_XnegYnegZneg);
  static Handle(V3d_Light) aLight5 = new V3d_DirectionalLight(getViewer(), V3d_XnegYposZpos);
  static Handle(V3d_Light) aLight6 = new V3d_DirectionalLight(getViewer(), V3d_XposYposZpos);

  if (isOn)
  {
    getViewer()->SetLightOn(aLight1);
    getViewer()->SetLightOn(aLight2);
    getViewer()->SetLightOn(aLight3);
    getViewer()->SetLightOn(aLight4);
    getViewer()->SetLightOn(aLight5);
    getViewer()->SetLightOn(aLight6);
  }
  else 
  {
    getViewer()->SetLightOff(aLight1);
    getViewer()->SetLightOff(aLight2);
    getViewer()->SetLightOff(aLight3);
    getViewer()->SetLightOff(aLight4);
    getViewer()->SetLightOff(aLight5);
    getViewer()->SetLightOff(aLight6);
  }
}

//================================================================
// Function : TexturesExt_Presentation::sampleBottle
// Purpose  : 
//================================================================
void TexturesExt_Presentation::sampleBottle()
{  
  TopoDS_Shape aShape;
  if (!loadShape(aShape, "bottle.brep")) 
    return;

  // resize and move the shape to the center of the viewer
gp_Trsf aTrsf1, aTrsf2;
aTrsf1.SetScale(gp_Pnt(0,0,0), 0.8);
aTrsf2.SetTranslation(gp_Pnt(0,0,0),gp_Pnt(0,0,-20));
aTrsf1.Multiply(aTrsf2);
BRepBuilderAPI_Transform Transformer(aTrsf1);
Transformer.Perform(aShape);
aShape = Transformer.Shape();

  TopTools_IndexedMapOfShape aFaces;
  TopExp::MapShapes(aShape, TopAbs_FACE, aFaces);

  // display original shape in shaded display mode
  Handle(AIS_Shape) aShapeIO = drawShape(aShape, Graphic3d_NOM_BRASS, Standard_False);
  getAISContext()->SetDisplayMode(aShapeIO, AIS_Shaded, Standard_False);
  // Set increased polygon offset for the main shape to avoid depth collision with textured faces
  aShapeIO->SetPolygonOffsets(Aspect_POM_Fill, 1.5, 0.5);
  DISP(aShapeIO);

  Handle(AIS_Shape) aTFace1 = Texturize(aFaces(16), "carrelage1.gif", 1, 1, 3, 2);
  DISP(aTFace1);

  Handle(AIS_Shape) aTFace2 = Texturize(aFaces(21), "carrelage1.gif", 1, 1, 3, 2);
  DISP(aTFace2);

  getViewer()->Update();
}


//================================================================
// Function : TexturesExt_Presentation::sampleLand
// Purpose  : 
//================================================================
void TexturesExt_Presentation::sampleTerrain()
{
  TopoDS_Shape aShape;
  if (!loadShape(aShape, "terrain.brep"))
    return;

  // a part of the landscape is textured
  TopTools_IndexedMapOfShape aFaces;
  TopExp::MapShapes(aShape, TopAbs_FACE, aFaces);

//  TopLoc_Location aLoc;
//  Handle(Geom_Surface) aSur = BRep_Tool::Surface(TopoDS::Face(aFaces(1)), aLoc);
//  Standard_Real u1,u2,v1,v2;
//  aSur->Bounds(u1,u2,v1,v2);
//  gp_Pnt aPnt = aSur->Value(u1, v1);
  gp_Pnt aPnt(82100,80300,10940);// point at u1,v1
  // resize and move the shape to the center of the viewer
  
  gp_Trsf aMoveTrsf;
  gp_Ax3 New(gp_Pnt(-30,-30, 0),gp_Dir(0,0,1));
  gp_Ax3 aCurrent(aPnt,gp_Dir(0,0,1));
  aMoveTrsf.SetDisplacement(aCurrent, New);

  gp_Trsf aScaleTrsf;
  aScaleTrsf.SetScale(aPnt,0.0075);

  BRepBuilderAPI_Transform aTransform(aMoveTrsf*aScaleTrsf);

  aTransform.Perform(aFaces(1));
  aShape = aTransform;

  getAISContext()->Display (Texturize (aShape, "terrain.gif"), Standard_True);
}


//================================================================
// Function : moveScale
// Purpose  : move a shape a little left and scale it to 15%.
//================================================================
static void moveScale(TopoDS_Shape& aShape)
{
  gp_Trsf aMoveTrsf;
  gp_Ax3 New(gp_Pnt(-30,-30, -10),gp_Dir(0,0,1));
  gp_Ax3 Current(gp_Pnt(0,0,0),gp_Dir(0,0,1));
  aMoveTrsf.SetDisplacement(Current, New);

  gp_Trsf aScaleTrsf;
  aScaleTrsf.SetScale(gp_Pnt(0,0,0),0.15);

  BRepBuilderAPI_Transform aTransform(aMoveTrsf*aScaleTrsf);

  aTransform.Perform(aShape);
  aShape = aTransform;
}

//================================================================
// Function : TexturesExt_Presentation::sampleKitchen
// Purpose  : kitchen with texturized items in it.
//================================================================
void TexturesExt_Presentation::sampleKitchen()
{
  TopoDS_Shape aShape;

  if (!loadShape(aShape, "Room.brep"))
    return;

  gp_Trsf aTrsf;
  gp_Ax3 NewCoordSystem (gp_Pnt(-1,-1, -1),gp_Dir(0,0,1));
  gp_Ax3 CurrentCoordSystem(gp_Pnt(0,0,0),gp_Dir(0,0,1));
  aTrsf.SetDisplacement(CurrentCoordSystem, NewCoordSystem);
  aShape.Location(TopLoc_Location(aTrsf));

  moveScale(aShape);

  // draw kitchen room whithout one wall (to better see the insides)
  TopTools_IndexedMapOfShape aFaces;
  TopExp::MapShapes(aShape, TopAbs_FACE, aFaces);  
  Standard_Integer nbFaces = aFaces.Extent();

  // create a wooden kitchen floor
  // the floor's face will be textured with texture from chataignier.gif
  DISP(Texturize(aFaces(5),"plancher.gif",1,1,2,1));

  // texturize other faces of the room with texture from wallpaper.gif (walls)
  DISP(Texturize(aFaces(1),"wallpaper.gif",1,1,8,6));
  DISP(Texturize(aFaces(3),"wallpaper.gif",1,1,8,6));
  DISP(Texturize(aFaces(4),"wallpaper.gif",1,1,8,6));

//  DISP(drawShape(aFaces(1), Quantity_NOC_LIGHTPINK, Standard_False));
//  DISP(drawShape(aFaces(3), Quantity_NOC_LIGHTPINK, Standard_False));
//  DISP(drawShape(aFaces(4), Quantity_NOC_LIGHTPINK, Standard_False));

  // texturize furniture items with "wooden" texture
  if (loadShape(aShape, "MODERN_Table_1.brep"))
  {
    moveScale(aShape);
    DISP(Texturize(aShape, "chataignier.gif"));
  }
  if (loadShape(aShape, "MODERN_Chair_1.brep"))
  {
    moveScale(aShape);
    DISP(Texturize(aShape, "chataignier.gif"));
  }
  if (loadShape(aShape, "MODERN_Cooker_1.brep"))
  {
    moveScale(aShape);

    aFaces.Clear();
    TopExp::MapShapes(aShape, TopAbs_FACE, aFaces);  
    nbFaces = aFaces.Extent();

    for (Standard_Integer i = 1; i <= nbFaces; i++)
    {
      if (i >= 59)
        DISP(drawShape(aFaces(i), Graphic3d_NOM_STEEL, Standard_False));
      else if (i >= 29)
        DISP(drawShape(aFaces(i), Graphic3d_NOM_ALUMINIUM, Standard_False));
      else if (i == 28)
        DISP(Texturize(aFaces(i), "cookerplate.gif"));
      else  
        DISP(Texturize(aFaces(i), "chataignier.gif"));
    }
  }
  if (loadShape(aShape, "MODERN_Cooker_1_opened.brep"))
  {
    moveScale(aShape);
    DISP(Texturize(aShape, "chataignier.gif"));
  }
  if (loadShape(aShape, "MODERN_Exhaust_1.brep"))
  {
    moveScale(aShape);
    DISP(drawShape(aShape, Graphic3d_NOM_STONE, Standard_False));
  }
  if (loadShape(aShape, "MODERN_MVCooker_1.brep"))
  {
    moveScale(aShape);
    DISP(drawShape(aShape, Graphic3d_NOM_SILVER, Standard_False));
  }
  if (loadShape(aShape, "MODERN_MVCooker_1_opened.brep"))
  {
    moveScale(aShape);
    DISP(drawShape(aShape, Graphic3d_NOM_SILVER, Standard_False));
  }
  if (loadShape(aShape, "MODERN_Sink_1.brep"))
  {
    moveScale(aShape);

    aFaces.Clear();
    TopExp::MapShapes(aShape, TopAbs_FACE, aFaces);  
    nbFaces = aFaces.Extent();

    for (Standard_Integer i = 1; i <= nbFaces; i++)
    {
      if (i < 145)
        DISP(drawShape(aFaces(i), Graphic3d_NOM_ALUMINIUM, Standard_False));
      else if (i == 145)
        DISP(Texturize(aFaces(i), "cookerplate.gif"));
      else  
        DISP(Texturize(aFaces(i), "chataignier.gif"));
    }
  }
  if (loadShape(aShape, "MODERN_Sink_1_opened.brep"))
  {
    moveScale(aShape);
    DISP(Texturize(aShape, "chataignier.gif"));
  }
  if (loadShape(aShape, "MODERN_Refrigerator_1.brep"))
  {
    moveScale(aShape);
    DISP(drawShape(aShape, Graphic3d_NOM_CHROME, Standard_False));
  }
  if (loadShape(aShape, "MODERN_Refrigerator_1_opened.brep"))
  {
    moveScale(aShape);
    DISP(drawShape(aShape, Graphic3d_NOM_CHROME, Standard_False));
  }

  getViewer()->Update();
}