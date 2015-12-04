// Extrema_Presentation.cpp: implementation of the Extrema_Presentation class.
// Calculation of extrema between geometries or shapes
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Extrema_Presentation.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepBuilderAPI_TransitionMode.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>

#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_ExtremaCurveSurface.hxx>
#include <GeomAPI_ExtremaSurfaceSurface.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomFill_Pipe.hxx>
#include <GeomFill_Trihedron.hxx>

#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_BezierSurface.hxx>

#include <gp_Pnt.hxx>
#include <gp_Ax3.hxx>
#include <Precision.hxx>
#include <AIS_Shape.hxx>

#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Edge.hxx>

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Extrema_Presentation;

// Initialization of array of samples
const Extrema_Presentation::PSampleFuncType Extrema_Presentation::SampleFuncs[] =
{
  &Extrema_Presentation::sampleCurveCurve,
  &Extrema_Presentation::sampleCurveSur,
  &Extrema_Presentation::sampleSurSur,
  &Extrema_Presentation::sampleShapeShape,
  &Extrema_Presentation::sampleShellShell
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Extrema_Presentation::Extrema_Presentation()
{
  myIndex = 0;
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Calculation of Extrema");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Extrema_Presentation::DoSample()
{
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//================================================================
// Function : displayExtemaEdge
// Purpose  : displays an edge between the two given points with white color
//================================================================
void Extrema_Presentation::displayExtemaEdge(const gp_Pnt& p1, const gp_Pnt& p2)
{
  Handle(AIS_Shape) anExtremaIO = new AIS_Shape(BRepBuilderAPI_MakeEdge(p1,p2));
  anExtremaIO->SetColor(Quantity_NOC_WHITE);
  anExtremaIO->SetWidth(3);
  drawPoint(p1, Quantity_Color(Quantity_NOC_WHITE));
  drawPoint(p2, Quantity_Color(Quantity_NOC_WHITE));
  getAISContext()->Display(anExtremaIO);
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////
//================================================================
// Function : Extrema_Presentation::samplePoints
// Purpose  : curve <-> curve extrema
//================================================================
void Extrema_Presentation::sampleCurveCurve()
{
  getAISContext()->EraseAll();

  ResetView();
  SetViewCenter(282.89618368456, 81.618799162700);
  SetViewScale(1.0106109766380);

  setResultTitle("Extrema between curve and curve");

  TCollection_AsciiString aText(
    "  // initializing array of bezier curve poles" EOL
    "  Standard_Real aCoords[][3] = {" EOL
    "    {0,0,0},{0,1,0},{1,1,0.2},{1,2,0.2},{2,2,0.4},{2,1,0.4},{3,1,0.6},{3,0,0.6}," EOL
    "    {2,0,0.8},{2,-1,0},{3,-1,0},{3,-2,-0.5},{4,-2,1},{4,-1,1.2},{5,-1,1.2}," EOL
    "    {5,0,1.4},{6,0,1.4},{6,-1,1.6},{7,-1,1.6},{7,0,1.8},{8,0,1.8},{8,1,2}" EOL
    "  };" EOL
    "  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);" EOL
    "  TColgp_Array1OfPnt aPoles (1, nPoles);" EOL
    "  for (Standard_Integer i=0; i < nPoles; i++)" EOL
    "    aPoles(i+1) = gp_Pnt (aCoords[i][0]*100, aCoords[i][1]*100, aCoords[i][2]*100);" EOL EOL

    "  // Two curves for finding extrema between them" EOL
    "  Handle(Geom_Curve) aCurve1 = new Geom_BezierCurve(aPoles);" EOL
    "  Handle(Geom_Curve) aCurve2 = new Geom_Circle(gp_Ax2(gp_Pnt(100,100,400), gp_Dir(0,0,1)), 300);" EOL EOL

    "  // Finding extrema between the curves" EOL
    "  GeomAPI_ExtremaCurveCurve anExtrema (aCurve1, aCurve2);" EOL EOL

    "  Standard_Real aShortestDistance = anExtrema.LowerDistance();" EOL
    "  Standard_Real u1, u2;" EOL
    "  gp_Pnt P1, P2;" EOL
    "  anExtrema.LowerDistanceParameters(u1, u2);" EOL
    "  anExtrema.NearestPoints(P1,P2);" EOL EOL

    "  // iterating through all solutions" EOL
    "  for (i = 0; i <= anExtrema.NbExtrema(); i++)" EOL
    "  {" EOL
    "    Standard_Real aDistance = anExtrema.Distance(i);" EOL
    "    anExtrema.Parameters(i, u1, u2);" EOL
    "    anExtrema.Points(i, P1, P2);" EOL
    "  }" EOL EOL
  );
  setResultText(aText.ToCString());

  // points to build the curves
  Standard_Real aCoords[][3] = {
    {0,0,0},{0,1,0},{1,1,0.2},{1,2,0.2},{2,2,0.4},{2,1,0.4},{3,1,0.6},{3,0,0.6},
    {2,0,0.8},{2,-1,0},{3,-1,0},{3,-2,-0.5},{4,-2,1},{4,-1,1.2},{5,-1,1.2},
    {5,0,1.4},{6,0,1.4},{6,-1,1.6},{7,-1,1.6},{7,0,1.8},{8,0,1.8},{8,1,2}
  };
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  TColgp_Array1OfPnt aPoles (1, nPoles);
  for (Standard_Integer i=0; i < nPoles; i++)
    aPoles(i+1) = gp_Pnt (aCoords[i][0]*100, aCoords[i][1]*100, aCoords[i][2]*100);
  
  Handle(Geom_Curve) aCurve1 = new Geom_BezierCurve(aPoles);
  Handle(Geom_Curve) aCurve2 = new Geom_Circle(gp_Ax2(gp_Pnt(100,100,400), gp_Dir(0,0,1)), 300);

  // displaying the curves in the viewer
  drawCurve(aCurve1);
  if (WAIT_A_LITTLE) return;
  drawCurve(aCurve2);
  if (WAIT_A_LITTLE) return;

  // Finding shortest extrema between the curves
  gp_Pnt P1, P2;
  GeomAPI_ExtremaCurveCurve anExtrema(aCurve1, aCurve2);
  anExtrema.NearestPoints(P1,P2);
  displayExtemaEdge(P1,P2);

  Standard_Real aShortestDistance = anExtrema.LowerDistance();
  Standard_Real u1, u2;
  anExtrema.LowerDistanceParameters(u1, u2);

  char buffer[256];
  sprintf(buffer, 
    "  // DISTANCE BETWEEN THE CURVES = %.2f" EOL
    "  // PARAMETER ON CURVE1 = %.2f" EOL
    "  // PARAMETER ON CURVE2 = %.2f" EOL, aShortestDistance, u1, u2);
  aText += buffer;
  setResultText(aText.ToCString());
}

//================================================================
// Function : Extrema_Presentation::sampleCurveSur
// Purpose  : Curve <-> Surface extrema
//================================================================
void Extrema_Presentation::sampleCurveSur()
{
  getAISContext()->EraseAll();

  ResetView();
  SetViewCenter(-107.67723432505, 109.55602570748);
  SetViewScale(0.63361834647187);

  setResultTitle("Extrema between curve and surface");
  TCollection_AsciiString aText(
    "  // the curve - first object for finding the extrema." EOL
    "  Handle(Geom_Curve) aCurve;" EOL
    "  Handle(TColgp_HArray1OfPnt) aPoles = new TColgp_HArray1OfPnt(1, 4);" EOL EOL

    "  // points to build the curves" EOL
    "  Standard_Real a[][3] = {{2,1,10},{0,12,0},{5,6,0},{0,10,11}};  " EOL EOL
 
    "  // filling the arrays of poles, scaling them, displaying the points" EOL
    "  for (Standard_Integer i=0; i < 4; i++)" EOL
    "    aPoles->SetValue(i+1, gp_Pnt(a[i][0]*75-300, a[i][1]*75-300, a[i][2]*75-400));" EOL EOL

    "  //Creating an interpolation curve" EOL
    "  GeomAPI_Interpolate aInterpolate1(aPoles, false, Precision::Confusion());" EOL
    "  aInterpolate1.Perform();" EOL
    "  if (aInterpolate1.IsDone())    aCurve = aInterpolate1.Curve();" EOL EOL
  
    "  // creating an axis parallel to Z axis " EOL
    "  gp_Ax3 anAx(gp_Pnt(-400, -400, -200), gp_Dir(0,0,1)); " EOL
    "  // creating a spherical surface" EOL
    "  Handle(Geom_SphericalSurface) aSurface = new Geom_SphericalSurface(anAx, 1);" EOL EOL

    "  // Finding extrema between the curve and the surface" EOL
    "  GeomAPI_ExtremaCurveSurface anExtrema(aCurve, aSurface);" EOL EOL

    "  Standard_Real aShortestDistance = anExtrema.LowerDistance();" EOL
    "  Standard_Real w, u, v;" EOL
    "  gp_Pnt P1, P2;" EOL
    "  anExtrema.LowerDistanceParameters(w, u, v);" EOL
    "  anExtrema.NearestPoints(P1,P2);" EOL EOL

    "  // iterating through all solutions" EOL
    "  for (i = 0; i <= anExtrema.NbExtrema(); i++)" EOL
    "  {" EOL
    "    Standard_Real aDistance = anExtrema.Distance(i);" EOL
    "    anExtrema.Parameters(i, w, u, v);" EOL
    "    anExtrema.Points(i, P1, P2);" EOL
    "  }" EOL EOL
  );
  setResultText(aText.ToCString());
  
  // the curve - first object for finding the extrema.
  Handle(Geom_Curve) aCurve;
  Handle(TColgp_HArray1OfPnt) aPoles = new TColgp_HArray1OfPnt(1, 4); 

  // points to build the curves
  Standard_Real a[][3] = {{2,1,10},{0,12,0},{5,6,0},{0,10,11}};  
 
  // filling the arrays of poles, scaling them, displaying the points
  for (Standard_Integer i=0; i < 4; i++)
    aPoles->SetValue(i+1, gp_Pnt(a[i][0]*75-300, a[i][1]*75-300, a[i][2]*75-400));

  //Creating an interpolation curve
  GeomAPI_Interpolate aInterpolate1(aPoles, false, Precision::Confusion());
  aInterpolate1.Perform();
  if (aInterpolate1.IsDone())    aCurve = aInterpolate1.Curve();
  
  // creating an axis parallel to Z axis 
  gp_Ax3 anAx(gp_Pnt(-400, -400, -200), gp_Dir(0,0,1)); 
  // creating a spherical surface - second object to find the extrema between
  Handle(Geom_SphericalSurface) aSurface = new Geom_SphericalSurface(anAx, 200);

  drawCurve(aCurve);
  if (WAIT_A_LITTLE) return;
  drawSurface(aSurface);  
  if (WAIT_A_LITTLE) return;

  // Finding extrema between the curve and the surface
  gp_Pnt P1, P2;
  GeomAPI_ExtremaCurveSurface anExtrema(aCurve, aSurface);
  anExtrema.NearestPoints(P1,P2);
  displayExtemaEdge(P1,P2);

  Standard_Real aShortestDistance = anExtrema.LowerDistance();
  Standard_Real w, u, v;
  anExtrema.LowerDistanceParameters(w, u, v);

  char buffer[256];
  sprintf(buffer, 
    "  // DISTANCE BETWEEN THE CURVE AND THE SURFACE = %.2f" EOL
    "  // PARAMETER ON THE CURVE = %.2f" EOL
    "  // PARAMETERS ON THE SURFACE: U = %.2f, V = %.2f" EOL, aShortestDistance, w, u, v);
  aText += buffer;
  setResultText(aText.ToCString());  
}


//================================================================
// Function : Extrema_Presentation::sampleSurSur
// Purpose  : Surface <-> Surface extrema
//================================================================
void Extrema_Presentation::sampleSurSur()
{
  getAISContext()->EraseAll();
  setResultTitle("Extrema between surface and surface");
  TCollection_AsciiString aText(
    "  // creating an axis parallel to Z axis" EOL
    "  gp_Ax3 anAx1(gp_Pnt(-400,-400,-200), gp_Dir(0,0,1)); " EOL
    "  // creating a spherical surface" EOL
    "  Handle(Geom_SphericalSurface) aSphericalSur = new Geom_SphericalSurface(anAx1,200);" EOL EOL 

    "  // creating an axis parallel to Y axis " EOL
    "  gp_Ax3 anAx2(gp_Pnt(300,300,300), gp_Dir(0,0,1)); " EOL
    "  // creating a toroidal surface" EOL
    "  Handle(Geom_ToroidalSurface) aToroidalSur = new Geom_ToroidalSurface(anAx2, 300, 100);" EOL EOL 
  
    "  // Finding extrema between the surfaces" EOL
    "  GeomAPI_ExtremaSurfaceSurface anExtrema(aSphericalSur,aToroidalSur);" EOL EOL

    "  Standard_Real aShortestDistance = anExtrema.LowerDistance();" EOL
    "  Standard_Real u1, v1, u2, v2;" EOL
    "  gp_Pnt P1, P2;" EOL
    "  anExtrema.LowerDistanceParameters(u1, v1, u2, v2);" EOL
    "  anExtrema.NearestPoints(P1,P2);" EOL EOL

    "  // iterating through all solutions" EOL
    "  for (i = 0; i <= anExtrema.NbExtrema(); i++)" EOL
    "  {" EOL
    "    Standard_Real aDistance = anExtrema.Distance(i);" EOL
    "    anExtrema.Parameters(i, u1, v1, u2, v2);" EOL
    "    anExtrema.Points(i, P1, P2);" EOL
    "  }" EOL EOL
  );
  setResultText(aText.ToCString());
  
  // creating an axis parallel to Z axis 
  gp_Ax3 anAx1(gp_Pnt(-400,-400,-200), gp_Dir(0,0,1)); 
  // creating a spherical surface
  Handle(Geom_SphericalSurface) aSphericalSur = new Geom_SphericalSurface(anAx1,200);  

  // creating an axis parallel to Y axis 
  gp_Ax3 anAx2(gp_Pnt(300,300,300), gp_Dir(0,0,1)); 
  // creating a toroidal surface
  Handle(Geom_ToroidalSurface) aToroidalSur = new Geom_ToroidalSurface(anAx2, 300, 100);
  
  drawSurface(aSphericalSur);
  if (WAIT_A_LITTLE) return;
  drawSurface(aToroidalSur);
  if (WAIT_A_LITTLE) return;

  // Finding extrema between the surfaces
  gp_Pnt P1, P2;
  GeomAPI_ExtremaSurfaceSurface anExtrema(aSphericalSur,aToroidalSur);
  anExtrema.NearestPoints(P1,P2);
  displayExtemaEdge(P1, P2);

  Standard_Real aShortestDistance = anExtrema.LowerDistance();
  Standard_Real u1, v1, u2, v2;
  anExtrema.LowerDistanceParameters(u1, v1, u2, v2);

  char buffer[256];
  sprintf(buffer,
    "  // DISTANCE BETWEEN THE SURFACES = %.2f" EOL
    "  // PARAMETERS ON SURFACE1: U = %.2f, V = %.2f" EOL
    "  // PARAMETERS ON SURFACE2: U = %.2f, V = %.2f" EOL, aShortestDistance, u1, v1, u2, v2);
  aText += buffer;
  setResultText(aText.ToCString());
}

//================================================================
// Function : Extrema_Presentation::sampleShapeShape
// Purpose  : TopoDS_Shape <-> TopoDS_Shape extrema
//================================================================
void Extrema_Presentation::sampleShapeShape()
{
  getAISContext()->EraseAll();
  setResultTitle("Extrema between shapes");
  TCollection_AsciiString aText(
    "  Standard_Real a = 500;" EOL
    "  Standard_Real b = a/2;" EOL
    "  BRepPrimAPI_MakeBox aBoxMaker (gp_Pnt(0, -b, -b), a, a, a);" EOL
    "  BRepPrimAPI_MakeCylinder aCylMaker(gp_Ax2(gp_Pnt(-a,0,0), gp_Dir(.2,-.3,.5)), a/4, a);" EOL EOL
  );
  setResultText(aText.ToCString());
  
  Standard_Real a = 500;
  Standard_Real b = a/2;
  BRepPrimAPI_MakeBox aBoxMaker (gp_Pnt(0, -b, -b), a, a, a);
  BRepPrimAPI_MakeCylinder aCylMaker(gp_Ax2(gp_Pnt(-a,0,0), gp_Dir(.2,-.3,.5)), a/4, a);

  TopoDS_Solid aShape1 = aBoxMaker.Solid();
  TopoDS_Solid aShape2 = aCylMaker.Solid();

  doShapeShapeExtrema(aShape1, aShape2, aText);
}


//================================================================
// Function : Extrema_Presentation::sampleShellShell
// Purpose  : 
//================================================================
void Extrema_Presentation::sampleShellShell()
{
  getAISContext()->EraseAll();
  setResultTitle("Extrema between two shells");
  TCollection_AsciiString aText(
    "  // building a first shell: the underlaying surface is created" EOL
    "  // using sweep algorythm.  One bezier curve is swept along another bezier curve." EOL
    "  // creating the curves" EOL
    "  Standard_Real aCoords1[][3] = {{0,0,0},{10,0,0},{20,10,12},{25,30,20},{50,40,50}};" EOL
    "  Standard_Real aCoords2[][3] = {{0,0,0},{0,0,10},{0,10,10},{0,10,20}};" EOL
    "  Standard_Integer nPoles1 = sizeof(aCoords1)/(sizeof(Standard_Real)*3);" EOL
    "  Standard_Integer nPoles2 = sizeof(aCoords2)/(sizeof(Standard_Real)*3);" EOL
    "  TColgp_Array1OfPnt aPoles1 (1, nPoles1);" EOL
    "  TColgp_Array1OfPnt aPoles2 (1, nPoles2);" EOL
    "  for (Standard_Integer i=0; i < nPoles1; i++)" EOL
    "    aPoles1(i+1) = gp_Pnt (aCoords1[i][0]*10-250, aCoords1[i][1]*10-250, aCoords1[i][2]*10-50);" EOL
    "  for (i=0; i < nPoles2; i++)" EOL
    "    aPoles2(i+1) = gp_Pnt (aCoords2[i][0]*10-250, aCoords2[i][1]*10-250, aCoords2[i][2]*10-50);" EOL EOL
  
    "  Handle(Geom_Curve) aPathCurve = new Geom_BezierCurve(aPoles1);" EOL
    "  Handle(Geom_Curve) aSweepCurve = new Geom_BezierCurve(aPoles2);" EOL EOL

    "  // creating the surface: sweeping aSweepCurve along aPathCurve" EOL
    "  GeomFill_Trihedron Option = GeomFill_IsFrenet;" EOL
    "  GeomFill_Pipe aSweepMaker;" EOL
    "  aSweepMaker.GenerateParticularCase(Standard_True);" EOL
    "  aSweepMaker.Init(aPathCurve, aSweepCurve, Option);" EOL
    "  Standard_Integer NbSeg = 30, MaxDegree = 10;" EOL
    "  Standard_Real Tol = 1.e-4;" EOL
    "  aSweepMaker.Perform(Precision::Confusion(), Standard_False,  GeomAbs_C2, MaxDegree, NbSeg);" EOL EOL

    "  // creating a TopoDS_Shell from Geom_Surface" EOL
    "  TopoDS_Shell aShape1 = BRepBuilderAPI_MakeShell(aSweepMaker.Surface());" EOL EOL

    "  // creating a second shell based on a bezier surface" EOL
    "  Standard_Real aSurCoords[][3] = {{-10,-10,-30},{-10,50,40},{-10,70,-20},{50,-10,40}," EOL
    "                              {50,50,0},{50,70,30},{70,-10,20},{70,50,30},{70,70,-20}};" EOL
    "  TColgp_Array2OfPnt aSurPoles(1,3,1,3);" EOL
    "  for (i=0; i < 3; i++)" EOL
    "    for (Standard_Integer j=0; j < 3; j++)" EOL
    "      aSurPoles(i+1,j+1) = gp_Pnt (aSurCoords[i*3+j][0]*10-250, aSurCoords[i*3+j][1]*10-250, aSurCoords[i*3+j][2]*10-350);" EOL EOL

    "  Handle(Geom_BezierSurface) aSurface = new Geom_BezierSurface(aSurPoles);" EOL EOL

    "  // creating a TopoDS_Shell from Geom_Surface" EOL 
    "  TopoDS_Shell aShape2 = BRepBuilderAPI_MakeShell(aSurface);" EOL EOL
  );
  setResultText(aText.ToCString());

  // building a first shell: the underlaying surface is created
  // using sweep algorythm.  One bezier curve is swept along another bezier curve.
  // creating the curves
  Standard_Real aCoords1[][3] = {{0,0,0},{10,0,0},{20,10,12},{25,30,20},{50,40,50}};
  Standard_Real aCoords2[][3] = {{0,0,0},{0,0,10},{0,10,10},{0,10,20}};
  Standard_Integer nPoles1 = sizeof(aCoords1)/(sizeof(Standard_Real)*3);
  Standard_Integer nPoles2 = sizeof(aCoords2)/(sizeof(Standard_Real)*3);
  TColgp_Array1OfPnt aPoles1 (1, nPoles1);
  TColgp_Array1OfPnt aPoles2 (1, nPoles2);
  for (Standard_Integer i=0; i < nPoles1; i++)
    aPoles1(i+1) = gp_Pnt (aCoords1[i][0]*10-250, aCoords1[i][1]*10-250, aCoords1[i][2]*10-50);
  for (i=0; i < nPoles2; i++)
    aPoles2(i+1) = gp_Pnt (aCoords2[i][0]*10-250, aCoords2[i][1]*10-250, aCoords2[i][2]*10-50);
  
  Handle(Geom_Curve) aPathCurve = new Geom_BezierCurve(aPoles1);
  Handle(Geom_Curve) aSweepCurve = new Geom_BezierCurve(aPoles2);

  // creating the surface: sweeping aSweepCurve along aPathCurve
  GeomFill_Trihedron Option = GeomFill_IsFrenet;
  GeomFill_Pipe aSweepMaker;
  aSweepMaker.GenerateParticularCase(Standard_True);
  aSweepMaker.Init(aPathCurve, aSweepCurve, Option);
  Standard_Integer NbSeg = 30, MaxDegree = 10;
  Standard_Real Tol = 1.e-4;
  aSweepMaker.Perform(Precision::Confusion(), Standard_False,  GeomAbs_C2, MaxDegree, NbSeg);

  // creating a TopoDS_Shell from Geom_Surface
  TopoDS_Shell aShape1 = BRepBuilderAPI_MakeShell(aSweepMaker.Surface());

  // creating a second shell based on a bezier surface
  Standard_Real aSurCoords[][3] = {{-10,-10,-30},{-10,50,40},{-10,70,-20},{50,-10,40},
                                   {50,50,0},{50,70,30},{70,-10,20},{70,50,30},{70,70,-20}};
  TColgp_Array2OfPnt aSurPoles(1,3,1,3);
  for (i=0; i < 3; i++)
    for (Standard_Integer j=0; j < 3; j++)
      aSurPoles(i+1,j+1) = gp_Pnt (aSurCoords[i*3+j][0]*10-250, aSurCoords[i*3+j][1]*10-250, aSurCoords[i*3+j][2]*10-350);

  Handle(Geom_BezierSurface) aSurface = new Geom_BezierSurface(aSurPoles);

  // creating a TopoDS_Shell from Geom_Surface
  TopoDS_Shell aShape2 = BRepBuilderAPI_MakeShell(aSurface);

  // displaying extrema between shape and shape
  doShapeShapeExtrema(aShape1, aShape2, aText);
}


//================================================================
// Function : Extrema_Presentation::doShapeShapeExtrema
// Purpose  : 
//================================================================
void Extrema_Presentation::doShapeShapeExtrema(const TopoDS_Shape& theShape1, 
                                               const TopoDS_Shape& theShape2,
                                               TCollection_AsciiString& theText)
{
  theText += 
    "  BRepExtrema_DistShapeShape anExtrema(aShape1, aShape2);" EOL
    "  if (anExtrema.IsDone())" EOL
    "  {" EOL
    "    Standard_Real aShortestDistance = anExtrema.Value();" EOL
    "    Standard_Real w1, w2, u1, v1, u2, v2;" EOL EOL

    "    // iterating through ALL solutions, 1 solution in our case" EOL
    "    for (Standard_Integer i = 1; i <= anExtrema.NbSolution(); i++)" EOL
    "    {" EOL
    "      gp_Pnt p1 = anExtrema.PointOnShape1(i);" EOL
    "      gp_Pnt p2 = anExtrema.PointOnShape2(i);" EOL EOL

    "      TopoDS_Shape aSupportShape1 = anExtrema.SupportOnShape1(i);" EOL
    "      TopoDS_Shape aSupportShape2 = anExtrema.SupportOnShape2(i);" EOL EOL

    "      if (anExtrema.SupportTypeOnShape1() == BRepExtrema::IsOnEdge)" EOL
    "        anExtrema.ParOnEdgeS1(i, w1);" EOL
    "      if (anExtrema.SupportTypeOnShape2() == BRepExtrema::IsOnEdge)" EOL
    "        anExtrema.ParOnEdgeS2(i, w2);" EOL
    "      if (anExtrema.SupportTypeOnShape1() == BRepExtrema::IsOnFace)" EOL
    "        anExtrema.ParOnEdgeS1(i, u1, v1);" EOL
    "      if (anExtrema.SupportTypeOnShape2() == BRepExtrema::IsOnFace)" EOL
    "        anExtrema.ParOnEdgeS2(i, u2, v2);" EOL
    "    }" EOL
    "  }" EOL EOL;

  getAISContext()->Display(new AIS_Shape(theShape1));
  if (WAIT_A_LITTLE) return;
  getAISContext()->Display(new AIS_Shape(theShape2));
  if (WAIT_A_LITTLE) return;

  // calculating shell-shell extrema
  BRepExtrema_DistShapeShape anExtrema(theShape1, theShape2);
  if (anExtrema.IsDone())
  {
    Standard_Real aShortestDistance = anExtrema.Value();
    Standard_Real w1, w2, u1, v1, u2, v2;

    // displaying ALL solutions
    for (Standard_Integer i = 1; i <= anExtrema.NbSolution(); i++)
    {
      displayExtemaEdge(anExtrema.PointOnShape1(i), anExtrema.PointOnShape2(i));

      Handle(AIS_Shape) aSupportShape1 = new AIS_Shape(anExtrema.SupportOnShape1(i));
      aSupportShape1->SetMaterial(Graphic3d_NOM_PLASTIC);
      aSupportShape1->SetColor(Quantity_NOC_GRAY97);
      Handle(AIS_Shape) aSupportShape2 = new AIS_Shape(anExtrema.SupportOnShape2(i));
      aSupportShape2->SetMaterial(Graphic3d_NOM_PLASTIC);
      aSupportShape2->SetColor(Quantity_NOC_GRAY97);
      getAISContext()->Display(aSupportShape1, Standard_False);
      getAISContext()->Display(aSupportShape2);

      char buffer[512];
      sprintf(buffer,
        "  // DISTANCE BETWEEN THE SHAPES = %.2f" EOL, aShortestDistance);      
      theText += buffer;

      if (anExtrema.SupportTypeShape1(i) == BRepExtrema_IsOnEdge)
      {
        anExtrema.ParOnEdgeS1(i, w1);
        sprintf(buffer,
          "  // TYPE OF SUPPORTING SHAPE ON SHAPE1 IS \"EDGE\","EOL
          "  //     VALUES OF PARAMETER = %.2f" EOL, w1);
        theText += buffer;
      }
      if (anExtrema.SupportTypeShape2(i) == BRepExtrema_IsOnEdge)
      {
        anExtrema.ParOnEdgeS2(i, w2);
        sprintf(buffer,
          "  // TYPE OF SUPPORTING SHAPE ON SHAPE2 IS \"EDGE\","EOL
          "  //     VALUES OF PARAMETER = %.2f" EOL, w2);
        theText += buffer;
      }
      if (anExtrema.SupportTypeShape1(i) == BRepExtrema_IsInFace)
      {
        anExtrema.ParOnFaceS1(i, u1, v1);
        sprintf(buffer,
          "  // TYPE OF SUPPORTING SHAPE ON SHAPE1 IS \"FACE\","EOL
          "  //     VALUES OF PARAMETERS: U = %.2f, V = %.2f" EOL, u1, v1);
        theText += buffer;

      }
      if (anExtrema.SupportTypeShape2(i) == BRepExtrema_IsInFace)
      {
        anExtrema.ParOnFaceS2(i, u2, v2);
        sprintf(buffer,
          "  // TYPE OF SUPPORTING SHAPE ON SHAPE2 IS \"FACE\","EOL
          "  //     VALUES OF PARAMETERS: U = %.2f, V = %.2f" EOL, u2, v2);
        theText += buffer;
      }

      setResultText(theText.ToCString());
    }
  }
}


