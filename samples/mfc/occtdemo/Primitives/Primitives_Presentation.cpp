// Primitives_Presentation.cpp: implementation of the Primitives_Presentation class.
// Construction of primitives.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Primitives_Presentation.h"

#include <Standard.hxx>

#include <TColgp_Array1OfPnt.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax2.hxx>

#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakeWedge.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>

#define SCALE 50

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Primitives_Presentation;

// Initialization of array of samples
const Primitives_Presentation::PSampleFuncType Primitives_Presentation::SampleFuncs[] =
{
  &Primitives_Presentation::sample1,
  &Primitives_Presentation::sample2,
  &Primitives_Presentation::sample3,
  &Primitives_Presentation::sample4,
  &Primitives_Presentation::sample5
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Primitives_Presentation::Primitives_Presentation()
{
  FitMode=false;
  myIndex = 0;
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Primitives");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Primitives_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////
//================================================================

//================================================================
// Function : Primitives_Presentation::sample1
// Purpose  : Building prisms
//================================================================

void Primitives_Presentation::sample1()
{

  setResultTitle("Building prisms");
  TCollection_AsciiString aText (
    "///////////////////////////////////////////////////////" EOL
    "// Building linear swept topologies, called prisms" EOL
    "///////////////////////////////////////////////////////" EOL EOL
    
    "//=====================================================" EOL
    "// A prism is defined by a basis shape (wire or face)," EOL
    "//which is swept. " EOL
    "//=====================================================" EOL EOL

    "//array coordinates of points:" EOL
    "Standard_Real aCoords[][3] = {" EOL
    "  //for base:" EOL
    "  {1,1,0},{4,8,0},{10,10,0},{12,6,0},{8,0,0}," EOL
    "  //for vectors:" EOL
    "  {1,1,7.5},{4,4,6}" EOL
    "};" EOL EOL

    "//array of points uses for building base of prisms and" EOL
    "//a sweeping direction" EOL
    "Standard_Integer nPoint = sizeof(aCoords)/(sizeof(Standard_Real)*3);" EOL
    "TColgp_Array1OfPnt aCurvePoint (1, nPoint);" EOL EOL

    "//scale:" EOL
    "for (Standard_Integer i=0; i < nPoint; i++)" EOL
    "  aCurvePoint(i+1) = gp_Pnt (aCoords[i][0]*SCALE-300," EOL
    "                             aCoords[i][1]*SCALE-300," EOL
    "                             aCoords[i][2]*SCALE-200);" EOL
   
    "//creating wire:" EOL
    "BRepBuilderAPI_MakePolygon aPol;" EOL
    "for (i=0; i < nPoint-2; i++)" EOL
    "  aPol.Add (aCurvePoint(i+1));" EOL
    "TopoDS_Wire aWire = aPol.Wire();" EOL EOL

    "//creating face:" EOL
    "TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aWire);" EOL EOL
  
    "//creating vectors:" EOL
    "gp_Vec aVec1(aCurvePoint(1),aCurvePoint(nPoint-1));" EOL
    "gp_Vec aVec2(aCurvePoint(1),aCurvePoint(nPoint));" EOL EOL
  
    "//creating prisms:" EOL EOL
    
    "  //the base is wire" EOL
    "  TopoDS_Shape aPrism1 = BRepPrimAPI_MakePrism(aWire,aVec1);" EOL
    "  TopoDS_Shape aPrism2 = BRepPrimAPI_MakePrism(aWire,aVec2);" EOL EOL

    "  //the base is face" EOL
    "  TopoDS_Shape aPrism3 = BRepPrimAPI_MakePrism(aFace,aVec1);" EOL
    "  TopoDS_Shape aPrism4 = BRepPrimAPI_MakePrism(aFace,aVec2);" EOL EOL

    );

  setResultText(aText.ToCString());

  //array coordinates of points:
  Standard_Real aCoords[][3] = {
    //for base:
    {1,1,0},{4,8,0},{10,10,0},{12,6,0},{8,0,0},{1,1,0},
    //for vectors:
    {1,1,7.5},{-1,-1,6}
  };

  //array of points uses for building base of prisms and
  //a sweeping direction.
  Standard_Integer nPoint = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  TColgp_Array1OfPnt aCurvePoint (1, nPoint);

  //scale:
  for (Standard_Integer i=0; i < nPoint; i++)
    aCurvePoint(i+1) = gp_Pnt (aCoords[i][0]*SCALE-300,
                               aCoords[i][1]*SCALE-300,
                               aCoords[i][2]*SCALE-200);
   
  //creating wire:
  BRepBuilderAPI_MakePolygon aPol;
  for (i=0; i < nPoint-2; i++)
    aPol.Add (aCurvePoint(i+1));
  TopoDS_Wire aWire = aPol.Wire();

  //creating face:
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aWire);
  
  //creating vectors:
  gp_Vec aVec1(aCurvePoint(1),aCurvePoint(nPoint-1));
  gp_Vec aVec2(aCurvePoint(1),aCurvePoint(nPoint));
  
  //creating prisms:
  TopoDS_Shape aPrism1 = BRepPrimAPI_MakePrism(aWire,aVec1);
  TopoDS_Shape aPrism2 = BRepPrimAPI_MakePrism(aWire,aVec2);
  TopoDS_Shape aPrism3 = BRepPrimAPI_MakePrism(aFace,aVec1);
  TopoDS_Shape aPrism4 = BRepPrimAPI_MakePrism(aFace,aVec2);

  //====================================================

  drawShape(aWire,Quantity_NOC_RED);
  if(WAIT_A_SECOND) return;
  Handle(AIS_InteractiveObject) aShowVec = drawVector(aCurvePoint(1),aVec1,Quantity_NOC_CYAN1);
  if(WAIT_A_LITTLE) return;
  Handle(AIS_Shape) aShowPrism = drawShape(aPrism1);
  if(WAIT_A_SECOND) return;

  getAISContext()->Erase(aShowVec);
  aShowVec = drawVector(aCurvePoint(1),aVec2,Quantity_NOC_CYAN1);
  if(WAIT_A_LITTLE) return;
  aShowPrism->Set(aPrism2);
  getAISContext()->Redisplay(aShowPrism);
  if(WAIT_A_SECOND) return;

  getAISContext()->Erase(aShowVec);
  getAISContext()->Erase(aShowPrism);
  drawShape(aFace);
  if(WAIT_A_SECOND) return;
  aShowVec = drawVector(aCurvePoint(1),aVec1,Quantity_NOC_CYAN1);
  if(WAIT_A_LITTLE) return;
  aShowPrism = drawShape(aPrism3);
  if(WAIT_A_SECOND) return;

  getAISContext()->Erase(aShowVec);
  aShowVec = drawVector(aCurvePoint(1),aVec2,Quantity_NOC_CYAN1);
  if(WAIT_A_LITTLE) return;
  aShowPrism->Set(aPrism4);
  getAISContext()->Redisplay(aShowPrism);
}

//================================================================
// Function : Primitives_Presentation::sample2
// Purpose  : Building cylinders and portions of cylinders
//================================================================

void Primitives_Presentation::sample2()
{
  setResultTitle("Building cylinders");
  TCollection_AsciiString aText (
    "///////////////////////////////////////////////////////" EOL
    "// Building cylinders and portions of cylinders" EOL
    "///////////////////////////////////////////////////////" EOL EOL

    "  //the local coordinate system Axes:" EOL
    "  gp_Ax2 Axes = gp::XOY();" EOL
    "  Axes.SetLocation(gp_Pnt(0,0,-300));" EOL EOL

    "  //the cylinder of radius aRadius and height aHeight" EOL
    "  Standard_Real aRadius = 200 , aHeight = 600 ;" EOL
    "  TopoDS_Shape aCylinder1 = " EOL
    "    BRepPrimAPI_MakeCylinder(Axes,aRadius,aHeight);  " EOL EOL

    "  //restrict cylinder by angle" EOL
    "  Standard_Real Angle1 = 3*PI/2, Angle2 = PI, Angle3 = 2*PI/3;" EOL
    "  TopoDS_Shape aCylinder2 = " EOL
    "    BRepPrimAPI_MakeCylinder(Axes,aRadius,aHeight,Angle1);" EOL
    "  TopoDS_Shape aCylinder3 = " EOL
    "    BRepPrimAPI_MakeCylinder(Axes,aRadius,aHeight,Angle2);" EOL
    "  TopoDS_Shape aCylinder4 = " EOL
    "    BRepPrimAPI_MakeCylinder(Axes,aRadius,aHeight,Angle3);" EOL
  );

  setResultText(aText.ToCString());
  
  //the local coordinate system Axes:
  gp_Ax2 Axes = gp::XOY();
  Axes.SetLocation(gp_Pnt(0,0,-300));

  //the cylinder of radius aRadius and height aHeight
  Standard_Real aRadius = 200 , aHeight = 600 ;
  TopoDS_Shape aCylinder1 = 
    BRepPrimAPI_MakeCylinder(Axes,aRadius,aHeight);  

  //restrict cylinder by angle
  Standard_Real Angle1 = 3*PI/2, Angle2 = PI, Angle3 = 2*PI/3;
  TopoDS_Shape aCylinder2 = 
    BRepPrimAPI_MakeCylinder(Axes,aRadius,aHeight,Angle1);
  TopoDS_Shape aCylinder3 = 
    BRepPrimAPI_MakeCylinder(Axes,aRadius,aHeight,Angle2);
  TopoDS_Shape aCylinder4 = 
    BRepPrimAPI_MakeCylinder(Axes,aRadius,aHeight,Angle3);

  //=====================================================

  getAISContext()->EraseAll();
  Handle(AIS_Shape) aShowShape = drawShape(aCylinder1);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aCylinder2);
  getAISContext()->Redisplay(aShowShape);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aCylinder3);
  getAISContext()->Redisplay(aShowShape);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aCylinder4);
  getAISContext()->Redisplay(aShowShape);
}

//================================================================
// Function : Primitives_Presentation::sample3
// Purpose  : Building cones and portions of cones.
//================================================================

void Primitives_Presentation::sample3()
{
  setResultTitle("Building cones");
  TCollection_AsciiString aText (
    "///////////////////////////////////////////////////////" EOL
    "//Building cones and portions of cones" EOL
    "///////////////////////////////////////////////////////" EOL EOL

    "  //the local coordinate system Axes:" EOL
    "  gp_Ax2 Axes = gp::XOY();" EOL
    "  Axes.SetLocation(gp_Pnt(0,0,-300));" EOL EOL

    "  //the cone of height aHeight, radius aRad1 in the plane z = 0" EOL
    "  //and aRad2 in the plane z = aHeight" EOL
    "  Standard_Real aRad1 = 150, aRad2 = 300, aHeight = 600;" EOL
    "  TopoDS_Shape aCone1 = BRepPrimAPI_MakeCone(Axes,aRad1,aRad2,aHeight);" EOL
    "  aRad1 = 400;" EOL
    "  TopoDS_Shape aCone2 = BRepPrimAPI_MakeCone(Axes,aRad1,aRad2,aHeight);" EOL EOL

    "  //restrict cone by angle" EOL
    "  Standard_Real Angle1 = 3*PI/2, Angle2 = 5*PI/4;" EOL
    "  TopoDS_Shape aCone3 = BRepPrimAPI_MakeCone(Axes,aRad1,aRad2,aHeight,Angle1);" EOL
    "  TopoDS_Shape aCone4 = BRepPrimAPI_MakeCone(Axes,aRad1,aRad2,aHeight,Angle2);" EOL
  );

  setResultText(aText.ToCString());

  //the local coordinate system Axes:
  gp_Ax2 Axes = gp::XOY();
  Axes.SetLocation(gp_Pnt(0,0,-300));

  //the cone of height aHeight, radius aRad1 in the plane z = 0
  //and aRad2 in the plane z = aHeight
  Standard_Real aRad1 = 150, aRad2 = 300, aHeight = 600;
  TopoDS_Shape aCone1 = BRepPrimAPI_MakeCone(Axes,aRad1,aRad2,aHeight);
  aRad1 = 400;
  TopoDS_Shape aCone2 = BRepPrimAPI_MakeCone(Axes,aRad1,aRad2,aHeight);

  //restrict cone by angle
  Standard_Real Angle1 = 3*PI/2, Angle2 = 5*PI/4;
  TopoDS_Shape aCone3 = BRepPrimAPI_MakeCone(Axes,aRad1,aRad2,aHeight,Angle1);
  TopoDS_Shape aCone4 = BRepPrimAPI_MakeCone(Axes,aRad1,aRad2,aHeight,Angle2);
  
  //=====================================================

  getAISContext()->EraseAll();
  Handle(AIS_Shape) aShowShape = drawShape(aCone1);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aCone2);
  getAISContext()->Redisplay(aShowShape);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aCone3);
  getAISContext()->Redisplay(aShowShape);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aCone4);
  getAISContext()->Redisplay(aShowShape);
}

//================================================================
// Function : Primitives_Presentation::sample4
// Purpose  : Building tori or portions of tori.
//================================================================

void Primitives_Presentation::sample4()
{
  setResultTitle("Building tori");
  TCollection_AsciiString aText (
    "///////////////////////////////////////////////////////" EOL
    "//Building tori or portions of tori" EOL
    "///////////////////////////////////////////////////////" EOL EOL

    "  //the torus of major radius aRad1 and minor radius aRad2" EOL
    "  Standard_Real aRad1 = 400, aRad2 = 100;" EOL
    "  TopoDS_Shape aTorus1 = BRepPrimAPI_MakeTorus(aRad1,aRad2); " EOL EOL

    "  // restrict major circle by angle MajorAngle" EOL
    "  Standard_Real MajorAngle1 = PI, MajorAngle2 = 3*PI/2;" EOL
    "  TopoDS_Shape aTorus2 = BRepPrimAPI_MakeTorus(aRad1,aRad2,MajorAngle1);" EOL
    "  TopoDS_Shape aTorus3 = BRepPrimAPI_MakeTorus(aRad1,aRad2,MajorAngle2);" EOL EOL

    "  // restrict minor circle by angles MinorAngleMin and MinorAngleMax" EOL
    "  Standard_Real MinorAngleMin1 = -5/8.*PI, MinorAngleMax1 = 7/8.*PI;" EOL
    "  Standard_Real MinorAngleMin2 = -PI/2, MinorAngleMax2 = PI/2;" EOL
    "  TopoDS_Shape aTorus4 = " EOL
    "    BRepPrimAPI_MakeTorus(aRad1,aRad2,MinorAngleMin1,MinorAngleMax1,MajorAngle2);" EOL
    "  TopoDS_Shape aTorus5 = " EOL
    "    BRepPrimAPI_MakeTorus(aRad1,aRad2,MinorAngleMin2,MinorAngleMax2,MajorAngle2);" EOL
    );
  setResultText(aText.ToCString());

  //the torus of major radius aRad1 and minor radius aRad2
  Standard_Real aRad1 = 400, aRad2 = 100;
  TopoDS_Shape aTorus1 = BRepPrimAPI_MakeTorus(aRad1,aRad2); 

  // restrict major circle by angle MajorAngle
  Standard_Real MajorAngle1 = PI, MajorAngle2 = 3/2.*PI;
  TopoDS_Shape aTorus2 = BRepPrimAPI_MakeTorus(aRad1,aRad2,MajorAngle1);
  TopoDS_Shape aTorus3 = BRepPrimAPI_MakeTorus(aRad1,aRad2,MajorAngle2);

  // restrict minor circle by angles MinorAngleMin and MinorAngleMax
  Standard_Real MinorAngleMin1 = -5/8.*PI, MinorAngleMax1 = 7/8.*PI;
  Standard_Real MinorAngleMin2 = -PI/2, MinorAngleMax2 = PI/2;
  TopoDS_Shape aTorus4 = 
    BRepPrimAPI_MakeTorus(aRad1,aRad2,MinorAngleMin1,MinorAngleMax1,MajorAngle2);
  TopoDS_Shape aTorus5 = 
    BRepPrimAPI_MakeTorus(aRad1,aRad2,MinorAngleMin2,MinorAngleMax2,MajorAngle2);

  //=====================================================
  
  getAISContext()->EraseAll();
  Handle(AIS_Shape) aShowShape = drawShape(aTorus1);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aTorus2);
  getAISContext()->Redisplay(aShowShape);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aTorus3);
  getAISContext()->Redisplay(aShowShape);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aTorus4);
  getAISContext()->Redisplay(aShowShape);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aTorus5);
  getAISContext()->Redisplay(aShowShape);
}

//================================================================
// Function : Primitives_Presentation::sample5
// Purpose  : Building wedges.
//================================================================

void Primitives_Presentation::sample5()
{
  setResultTitle("Building wedges");
  TCollection_AsciiString aText (
    "///////////////////////////////////////////////////////" EOL
    "// Building wedges, i.e. boxes with inclined faces" EOL
    "///////////////////////////////////////////////////////" EOL EOL

    "//the local coordinate system Axes:" EOL
    "gp_Ax2 Axes = gp::XOY();" EOL
    "Axes.SetLocation(gp_Pnt(-200,-100,-200));" EOL EOL

    "//creating wedges:" EOL EOL

    "  //wedges where the base, located in the plane z = 0," EOL
    "  //is defined by dx, dy, ltx and where the top is parallel " EOL
    "  //to the base and located in the plane defined by the equation: z = dz" EOL
    "  Standard_Real dx = 500, dy = 300, dz = 400, ltx = 650;" EOL
    "  TopoDS_Shape aWedge1 = BRepPrimAPI_MakeWedge(Axes,dx,dy,dz,ltx);  " EOL EOL

    "  dx = 650; dy = 300; dz = 400; ltx = 300;" EOL
    "  TopoDS_Shape aWedge2= BRepPrimAPI_MakeWedge(Axes,dx,dy,dz,ltx);" EOL EOL

    "  //wedges where the base is defined by dx, dy, xmin, xmax, zmin, zmax" EOL
    "  //and where the top is defined by dx, dy, dz, xmin, xmax, zmin, zmax" EOL
    "  dx = 600, dy = 600, dz = 300;" EOL
    "  Standard_Real xmin = 250, zmin = 0, xmax = 350, zmax = 300;" EOL
    "  TopoDS_Shape aWedge3= BRepPrimAPI_MakeWedge(Axes,dx,dy,dz,xmin,zmin,xmax,zmax);" EOL EOL

    "  dy = 300; dz = 500;" EOL
    "  xmin = 200; zmin = 200; xmax = 400;" EOL
    "  TopoDS_Shape aWedge4= BRepPrimAPI_MakeWedge(Axes,dx,dy,dz,xmin,zmin,xmax,zmax);" EOL EOL
  );

  setResultText(aText.ToCString());

  //the local coordinate system Axes:
  gp_Ax2 Axes = gp::XOY();
  Axes.SetLocation(gp_Pnt(-200,-100,-200));

  //creating wedges:

  //wedges where the base, located in the plane z = 0,
  //is defined by dx, dy, ltx and where the top is parallel
  //to the base and located in the plane defined by the equation: z = dz
  Standard_Real dx = 500, dy = 300, dz = 400, ltx = dx+100;
  TopoDS_Shape aWedge1 = BRepPrimAPI_MakeWedge(Axes,dx,dy,dz,ltx);

  ltx = dx-100;
  TopoDS_Shape aWedge2= BRepPrimAPI_MakeWedge(Axes,dx,dy,dz,ltx);

  //wedges where the base is defined by dx, dy, xmin, xmax, zmin, zmax
  //and where the top is defined by dx, dy, dz, xmin, xmax, zmin, zmax
  Standard_Real xmin = 0, zmin = 0, xmax = dx-200, zmax = dz+100;
  TopoDS_Shape aWedge3= BRepPrimAPI_MakeWedge(Axes,dx,dy,dz,xmin,zmin,xmax,zmax);
  
  xmin = 200;
  TopoDS_Shape aWedge4= BRepPrimAPI_MakeWedge(Axes,dx,dy,dz,xmin,zmin,xmax,zmax);
  
  zmin = -100;
  TopoDS_Shape aWedge5= BRepPrimAPI_MakeWedge(Axes,dx,dy,dz,xmin,zmin,xmax,zmax);

  //=====================================================

  getAISContext()->EraseAll();
  Handle(AIS_Shape) aShowShape = drawShape(aWedge1);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aWedge2);
  getAISContext()->Redisplay(aShowShape);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aWedge3);
  getAISContext()->Redisplay(aShowShape);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aWedge4);
  getAISContext()->Redisplay(aShowShape);
  if(WAIT_A_SECOND) return;

  aShowShape->Set (aWedge5);
  getAISContext()->Redisplay(aShowShape);
}
