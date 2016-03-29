// Textures_Presentation.cpp: implementation of the Textures_Presentation class.
// Creation of textural presentation of shape
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Textures_Presentation.h"


#include <TopoDS_Solid.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepFeat_Gluer.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <AIS_TexturedShape.hxx>
#include <BRepTools.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <gce_MakeCirc.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_SpotLight.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Texture2D.hxx>


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Textures_Presentation;

// Initialization of array of samples
const Textures_Presentation::PSampleFuncType Textures_Presentation::SampleFuncs[] =
{
  &Textures_Presentation::sampleCustomTexture2,
  &Textures_Presentation::sampleCustomTexture3,
//  &Textures_Presentation::samplePredefTexture1,
//  &Textures_Presentation::samplePredefTexture2,
  &Textures_Presentation::sampleCustomTexture1
};

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

#define ZVIEW_SIZE 100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Textures_Presentation::Textures_Presentation()
{
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Textured Shapes");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Textures_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

void Textures_Presentation::Init()
{
  // initialize v3d_view so it displays textures well
  getViewer()->InitActiveViews();
  Handle(V3d_View) aView = getViewer()->ActiveView();
  aView->SetSize(ZVIEW_SIZE);

  setResultTitle("Textured Shape");
  setResultText(
    "  TopoDS_Shape aShape;" EOL
    "" EOL
    "  // initialize aShape" EOL
    "  // aShape = ..." EOL
    "" EOL
    "  // create a textured presentation object for aShape" EOL
    "  Handle(AIS_TexturedShape) aTShape = new AIS_TexturedShape(aShape);" EOL
    "" EOL
    "  TCollection_AsciiString aTFileName;" EOL
    "" EOL
    "  // initialize aTFileName with an existing texture file name" EOL
    "  // (gif, bmp, xwd, rgb, and other formats are supported)" EOL
    "  // OR with an integer value string (max = Graphic3d_Texture2D::NumberOfTextures())" EOL
    "  // which will indicate use of predefined texture of this number" EOL
    "  // aTFileName = ..." EOL
    "" EOL
    "  aTShape->SetTextureFileName(aTFileName);" EOL
    "" EOL
    "  // do other initialization of AIS_TexturedShape" EOL
    "  Standard_Real nRepeat;" EOL
    "  Standard_Boolean toRepeat;" EOL
    "  Standard_Boolean toScale;" EOL
    "  // initialize aRepeat, toRepeat, toScale ..." EOL
    "" EOL
    "  aTShape->SetTextureMapOn();" EOL
    "  aTShape->SetTextureRepeat(toRepeat, nRepeat, nRepeat);" EOL
    "  aTShape->SetTextureScale(toScale);" EOL
    "  " EOL
    "  // mode 3 is \"textured\" mode of AIS_TexturedShape, " EOL
    "  // other modes will display the \"normal\", non-textured shape," EOL
    "  // in wireframe(1) or shaded(2) modes correspondingly" EOL
    "  aTShape->SetDisplayMode(3); " EOL);
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////




//================================================================
// Function : lightsOnOff
// Purpose  : 6 lights are used for a brighter demonstration of textured shapes
//            call lightsOnOff(false) before showing normal shape
//            call lightsOnOff(true)  before showing textured shape
//================================================================
void Textures_Presentation::lightsOnOff(Standard_Boolean isOn)
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
// Function : Textures_Presentation::displayTextured
// display an AIS_TexturedShape based on a given shape with texture with given filename
// filename can also be an integer value ("2", "5", etc.), in this case
// a predefined texture from Graphic3d_NameOfTexture2D with number = this value
// is loaded.
// the function returns !WAIT_A_SECOND
//================================================================
Standard_Boolean Textures_Presentation::displayTextured(const TopoDS_Shape& aShape,
                                                        TCollection_AsciiString aTFileName,
                                                        Standard_Boolean toDisplayOriginal,
                                                        Standard_Boolean toScale,
                                                        Standard_Integer nRepeat)
{
  // create a textured presentation object for aShape
  Handle(AIS_TexturedShape) aTShape = new AIS_TexturedShape(aShape);

  // load texture from file if it is not an integer value
  // integer value indicates a number of texture in predefined textures enumeration
  if (!aTFileName.IsIntegerValue())
  {
    TCollection_AsciiString aTmp(aTFileName);
    aTFileName = GetDataDir();
    aTFileName = aTFileName + "\\" + aTmp;
  }

  aTShape->SetTextureFileName(aTFileName);

  // do other initialization of AIS_TexturedShape
  aTShape->SetTextureMapOn();
  aTShape->SetTextureRepeat(Standard_True, nRepeat, nRepeat);
  aTShape->SetTextureScale(toScale);
  
  aTShape->SetDisplayMode(3); // mode 3 is "textured" mode

  // output to the screen, first original shape, then textured
  if (toDisplayOriginal)
  {
    lightsOnOff(Standard_False);

    Handle(AIS_Shape) aAisShape = drawShape(aShape/*, Quantity_NOC_WHITE*/);
    if (WAIT_A_SECOND) return Standard_False;
    getAISContext()->Erase(aAisShape, Standard_False);
  }

  // show textured shape more brightly - turn on more lights
  lightsOnOff(Standard_True);

  getAISContext()->Display(aTShape, Standard_True);

  return Standard_True;
}


//================================================================
// Function : Textures_Presentation::loadShape
// loads a shape from a given brep file from data dir into a given TopoDS_Shape object
//================================================================
Standard_Boolean Textures_Presentation::loadShape(TopoDS_Shape& aShape, 
                                                  TCollection_AsciiString aFileName)
{
  // create a TopoDS_Shape -> read from a brep file
  TCollection_AsciiString aPath(GetDataDir());
  aPath = aPath + "\\" + aFileName;

  BRep_Builder aBld;
  Standard_Boolean isRead = BRepTools::Read (aShape, aPath.ToCString(), aBld);
  if (!isRead)
  {
    aPath += " was not found.  The sample can not be shown.";
    setResultText(aPath.ToCString());
    return Standard_False;
  }

  return Standard_True;
}


//================================================================
// Function : ScaleShape
// Purpose  : scales a given TopoDS_Shape
//================================================================
static void ScaleShape(TopoDS_Shape& aShape, Standard_Real ScaleFactor)
{
  gp_Trsf aTrsf;
  aTrsf.SetScaleFactor(ScaleFactor);
  aShape.Location(TopLoc_Location(aTrsf));
}

//================================================================
// Function : Textures_Presentation::samplePoints
// Purpose  : 
//================================================================
void Textures_Presentation::sampleCustomTexture1()
{
  Standard_Real a = 30.0;
  TopoDS_Shape aShape = BRepPrimAPI_MakeBox(gp_Pnt(-a/2,-a/2,-a/2),a,a,a);
  TopTools_IndexedMapOfShape aFaces;
  TopExp::MapShapes(aShape, TopAbs_FACE, aFaces);

  // faces on faces
  Standard_CString OurFaces[] = 
  {
    "aev.gif","mla.gif","msv.gif","asv.gif","agn.gif","emo.gif",0
  };

  lightsOnOff(Standard_False);
  for (Standard_Integer i = 1; i <= aFaces.Extent(); i++)
  {
    drawShape(aFaces(i));
    if (WAIT_A_LITTLE) return;
  }

  getViewer()->Erase();

  for (i = 1; i <= aFaces.Extent(); i++)
  {
    displayTextured(aFaces(i), OurFaces[i-1], Standard_False, Standard_True, 1);
    if (WAIT_A_LITTLE) return;
  }
}


//================================================================
// Function : Textures_Presentation::sampleCustomTexture2
// Purpose  : 
//================================================================
void Textures_Presentation::sampleCustomTexture2()
{  
  TopoDS_Shape aShape;
  if (loadShape(aShape, "Pump_Nut.brep"))
  {
    // move the shape to [0,0,0]
    gp_Trsf aTrsf;
    gp_Ax3 oldAx3(gp_Pnt(81,280,0), gp::DZ());
    gp_Ax3 newAx3(gp_Pnt(0,0,0), gp::DZ());
    aTrsf.SetDisplacement(oldAx3, newAx3);
    aShape.Move(TopLoc_Location(aTrsf));

    displayTextured(aShape, "texture1.gif");
  }
}


//================================================================
// Function : Textures_Presentation::sampleCustomTexture3
// Purpose  : 
//================================================================
void Textures_Presentation::sampleCustomTexture3()
{
  TopoDS_Shape aShape;
  if (loadShape(aShape, "Pump_TopCover.brep"))
  {
    ScaleShape(aShape, 0.2);    
    displayTextured(aShape, "texture2.gif");
  }
}

/*
//================================================================
// Function : Textures_Presentation::samplePredefTexture1
// Purpose  : 
//================================================================
void Textures_Presentation::samplePredefTexture1()
{
  // the number of points of wires:
  const Standard_Integer aNbOfPnt = 4;

  // creates arrays of coordinates of wires:
  Standard_Real aCoords [][aNbOfPnt][3] =
  {{{-4,0,-2},{0,4,-2},{4,0,-2},{0,-4,-2}},{{-2,-2,2},{-2,2,2},{2,2,2},{2,-2,2}}};

  // the number of wires:
  Standard_Integer aNbOfWire = (sizeof(aCoords)/(sizeof(Standard_Real)*3))/aNbOfPnt;

  BRepOffsetAPI_ThruSections aTSec(Standard_True,Standard_True,0.001);
  for( Standard_Integer i = 0 ; i < aNbOfWire ; i++)
  {
    BRepBuilderAPI_MakePolygon aPol;
    for( Standard_Integer j = 0 ; j < aNbOfPnt ; j++)
      aPol.Add(gp_Pnt(aCoords[i][j][0],aCoords[i][j][1],aCoords[i][j][2]));

    aPol.Close();
    aTSec.AddWire(aPol.Wire());
  }

  aTSec.AddWire(BRepBuilderAPI_MakeWire(
    BRepBuilderAPI_MakeEdge(gce_MakeCirc(gp_Pnt(0,0,4),gp::DZ(),5))));

  aTSec.Build();

  if (!aTSec.IsDone()) return;

  TopoDS_Shape aShape = aTSec.Shape();

  ScaleShape(aShape, 5);

  // texture 1 - "alien skin"
  displayTextured(aShape, "1");
}


//================================================================
// Function : Textures_Presentation::samplePredefTexture2
// Purpose  : 
//================================================================
void Textures_Presentation::samplePredefTexture2()
{
  Standard_Integer aColLength = 4;
  Standard_Integer aRowLength = 4;
  TColStd_Array2OfReal aZPoints(1,aColLength,1,aRowLength);

  Standard_Real theZCoords [] = {{-2},{1},{1},{-1},{1},{-2},
   {-2},{1},{-2},{-3},{-2},{-2},{-4},{-2},{-2},{-2}};
  
  for(Standard_Integer i = 0; i < aRowLength ; i++)
    for(Standard_Integer j = 0; j < aColLength ; j++)
      aZPoints(i+1,j+1) = (theZCoords[i*aRowLength+j] * 50);

  Standard_Real aXStep = 250, aYStep = 250;
  Standard_Real aXBound = -450, aYBound = -300;

  Handle(Geom_BSplineSurface) aSurface = 
    GeomAPI_PointsToBSplineSurface(aZPoints,aXBound,aXStep,aYBound,aYStep,3,8);

  TopoDS_Face aShape = BRepBuilderAPI_MakeFace(aSurface);

  ScaleShape(aShape, 0.09);

  // texture 9 - "clouds"
  displayTextured(aShape, "9", Standard_True, Standard_True, 1);
}
*/