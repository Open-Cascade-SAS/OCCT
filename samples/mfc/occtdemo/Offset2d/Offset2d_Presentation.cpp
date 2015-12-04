// Offset2d_Presentation.cpp: implementation of the Offset2d_Presentation class.
// Presentation class: Offset of curves and wires
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Offset2d_Presentation.h"
#include <ISession_Curve.h>

#include <Geom2d_OffsetCurve.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <Quantity_Color.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <TopoDS_Wire.hxx>


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Offset2d_Presentation;

// Initialization of array of samples
const Offset2d_Presentation::SampleDescrType Offset2d_Presentation::SampleDescrs[] =
{
  {&Offset2d_Presentation::sampleCircle, 4, -20, 4, 20},
  {&Offset2d_Presentation::sampleEllipse, 4, -20, 4, 20},
  {&Offset2d_Presentation::sampleParabola, 4, -20., 4, 20},
  {&Offset2d_Presentation::sampleHyperbola, 4, -20., 4, 20},
  //{&Offset2d_Presentation::sampleBezier, 4, -10, 4, 10},
  {&Offset2d_Presentation::samplePBSpline, 4, -10, 8, 10},
  {&Offset2d_Presentation::sampleWire1, 0, 0, 4, 10},
  {&Offset2d_Presentation::sampleWire2, 4, -10, 3, 10}
};

// Colors of objects
static const Quantity_Color CurveColor       (1,1,0, Quantity_TOC_RGB);      // yellow
static const Quantity_Color OffsetCurveColor (1,0.647,0, Quantity_TOC_RGB);  // orange

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Offset2d_Presentation::Offset2d_Presentation()
{
  myNbSamples = sizeof(SampleDescrs)/sizeof(SampleDescrType);
  setName ("Offset Curves");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Offset2d_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleDescrs[myIndex].pFunc)();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

void Offset2d_Presentation::sampleCircle()
{
  Standard_CString aName = "2d Circle";
  // Create a Circle in 2d
  gp_Pnt2d aOrigin (0,0);
  gp_Dir2d aDir (1,0);
  gp_Ax2d aAxis (aOrigin, aDir);
  Standard_Real aRadius = 300;
  Handle(Geom2d_Circle) aCurve = new Geom2d_Circle (aAxis, aRadius);

  TCollection_AsciiString aText (
    "  // Create a Circle in 2d" EOL
    "  gp_Pnt2d aOrigin (0,0);" EOL
    "  gp_Dir2d aDir (1,0);" EOL
    "  gp_Ax2d aAxis (aOrigin, aDir);" EOL
    "  Standard_Real aRadius = 300;" EOL
    "  Handle(Geom2d_Circle) aCurve = new Geom2d_Circle (aAxis, aRadius);" EOL
    );
  drawAndOffsetCurve2d (aCurve, aName, aText);
}

void Offset2d_Presentation::sampleEllipse()
{
  Standard_CString aName = "2d Ellipse";
  // Create an Ellipse in 2d
  gp_Pnt2d aOrigin (0,0);
  gp_Dir2d aDir (1,0);
  gp_Ax2d aMajorAxis (aOrigin, aDir);
  Standard_Real aMajorRadius = 300;
  Standard_Real aMinorRadius = 150;
  Handle(Geom2d_Ellipse) aCurve = 
    new Geom2d_Ellipse (aMajorAxis, aMajorRadius, aMinorRadius);

  TCollection_AsciiString aText (
    "  // Create an Ellipse in 2d" EOL
    "  gp_Pnt2d aOrigin (0,0);" EOL
    "  gp_Dir2d aDir (1,0);" EOL
    "  gp_Ax2d aMajorAxis (aOrigin, aDir);" EOL
    "  Standard_Real aMajorRadius = 300;" EOL
    "  Standard_Real aMinorRadius = 150;" EOL
    "  Handle(Geom2d_Ellipse) aCurve = " EOL
    "    new Geom2d_Ellipse (aMajorAxis, aMajorRadius, aMinorRadius);" EOL
    );
  drawAndOffsetCurve2d (aCurve, aName, aText);
}

void Offset2d_Presentation::sampleParabola()
{
  Standard_CString aName = "2d Parabola";
  // Create a Parabola in 2d
  gp_Pnt2d aOrigin (0,0);
  gp_Dir2d aDir (0,1);
  gp_Ax2d aMirrorAxis (aOrigin, aDir);
  Standard_Real aFocal = 100;
  Handle(Geom2d_Parabola) aCurve = 
    new Geom2d_Parabola (aMirrorAxis, aFocal);

  TCollection_AsciiString aText (
    "  // Create a Parabola in 2d" EOL
    "  gp_Pnt2d aOrigin (0,0);" EOL
    "  gp_Dir2d aDir (0,1);" EOL
    "  gp_Ax2d aMirrorAxis (aOrigin, aDir);" EOL
    "  Standard_Real aFocal = 100;" EOL
    "  Handle(Geom2d_Parabola) aCurve = " EOL
    "    new Geom2d_Parabola (aMirrorAxis, aFocal);" EOL
    );
  drawAndOffsetCurve2d (aCurve, aName, aText);
}

void Offset2d_Presentation::sampleHyperbola()
{
  Standard_CString aName = "2d Hyperbola";
  // Create a Hyperbola in 2d
  gp_Pnt2d aOrigin (0,0);
  gp_Dir2d aDir (1,1);
  gp_Ax2d aMajorAxis (aOrigin, aDir);
  Standard_Real aMajorRadius = 100;
  Standard_Real aMinorRadius = 100;
  Handle(Geom2d_Hyperbola) aCurve = 
    new Geom2d_Hyperbola (aMajorAxis, aMajorRadius, aMinorRadius);

  TCollection_AsciiString aText (
    "  // Create a Hyperbola in 2d" EOL
    "  gp_Pnt2d aOrigin (0,0);" EOL
    "  gp_Dir2d aDir (1,1);" EOL
    "  gp_Ax2d aMajorAxis (aOrigin, aDir);" EOL
    "  Standard_Real aMajorRadius = 100;" EOL
    "  Standard_Real aMinorRadius = 100;" EOL
    "  Handle(Geom2d_Hyperbola) aCurve = " EOL
    "    new Geom2d_Hyperbola (aMajorAxis, aMajorRadius, aMinorRadius);" EOL
    );
  drawAndOffsetCurve2d (aCurve, aName, aText);
}

void Offset2d_Presentation::sampleBezier()
{
  Standard_CString aName = "2d BezierCurve";
  // Create a BezierCurve in 2d
  Standard_Real aPolesCoords[][2] = {
    {0,0},{0,1},{1,1},{1,2},{2,2},{2,1},{3,1},{3,0},{2,0},{2,-1},
    {3,-1},{3,-2},{4,-2},{4,-1},{5,-1},{5,0},{6,0},{6,-1},{7,-1},
    {7,0},{8,0},{8,1},{7,1},{7,2},{6,2},{6,1},{5,1}
  };
  TColgp_Array1OfPnt2d aPoles (1, sizeof(aPolesCoords)/(sizeof(Standard_Real)*2));
 
  for (Standard_Integer i=1; i <= aPoles.Upper(); i++)
    aPoles(i) = gp_Pnt2d (aPolesCoords[i-1][0]*100, aPolesCoords[i-1][1]*100);
  
  Handle(Geom2d_BezierCurve) aCurve = 
    new Geom2d_BezierCurve (aPoles);

  TCollection_AsciiString aText (
    "  // Create a BezierCurve in 2d" EOL
    "  Standard_Real aPolesCoords[][2] = {" EOL
    "    {0,0},{0,1},{1,1},{1,2},{2,2},{2,1},{3,1},{3,0},{2,0},{2,-1}," EOL
    "    {3,-1},{3,-2},{4,-2},{4,-1},{5,-1},{5,0},{6,0},{6,-1},{7,-1}," EOL
    "    {7,0},{8,0},{8,1},{7,1},{7,2},{6,2},{6,1},{5,1}" EOL
    "  };" EOL
    "  TColgp_Array1OfPnt2d aPoles (1, sizeof(aPolesCoords)/(sizeof(Standard_Real)*2));" EOL
    " " EOL
    "  for (Standard_Integer i=1; i <= aPoles.Upper(); i++)" EOL
    "    aPoles(i) = gp_Pnt2d (aPolesCoords[i-1][0]*100, aPolesCoords[i-1][1]*100);" EOL
    "  " EOL
    "  Handle(Geom2d_BezierCurve) aCurve = " EOL
    "    new Geom2d_BezierCurve (aPoles);" EOL
    );
  drawAndOffsetCurve2d (aCurve, aName, aText);
}

void Offset2d_Presentation::samplePBSpline()
{
  Standard_CString aName = "2d BSplineCurve";
  // Create a Periodic BSplineCurve in 2d
  Standard_Real aPolesCoords[][2] = {
    {0,0},{0,1},{1,1},{1,0},{2,0},{2,-1},{1,-1},
    {1,-2},{0,-2},{0,-1},{-1,-1},{-1,0}
  };
  Standard_Integer nPoles = sizeof(aPolesCoords)/(sizeof(Standard_Real)*2);
  TColgp_Array1OfPnt2d aPoles (1, nPoles);
  TColStd_Array1OfReal aKnots (1, nPoles+1);
  TColStd_Array1OfInteger aMultiplicities(1, nPoles+1);
  const Standard_Integer aDegree = 3;
  const Standard_Boolean isPeriodic = Standard_True;

  for (Standard_Integer i=1; i <= aPoles.Upper(); i++)
    aPoles(i) = gp_Pnt2d (aPolesCoords[i-1][0]*100, aPolesCoords[i-1][1]*100);
  for (i=1; i <= aKnots.Upper(); i++)
    aKnots(i) = i-1;
  aMultiplicities.Init(1);

  Handle(Geom2d_BSplineCurve) aCurve = 
    new Geom2d_BSplineCurve (aPoles, aKnots, aMultiplicities, aDegree, isPeriodic);

  TCollection_AsciiString aText (
    "  // Create a Periodic BSplineCurve in 2d" EOL
    "  Standard_Real aPolesCoords[][2] = {" EOL
    "    {0,0},{0,1},{1,1},{1,0},{2,0},{2,-1},{1,-1}," EOL
    "    {1,-2},{0,-2},{0,-1},{-1,-1},{-1,0}" EOL
    "  };" EOL
    "  Standard_Integer nPoles = sizeof(aPolesCoords)/(sizeof(Standard_Real)*2);" EOL
    "  TColgp_Array1OfPnt2d aPoles (1, nPoles);" EOL
    "  TColStd_Array1OfReal aKnots (1, nPoles+1);" EOL
    "  TColStd_Array1OfInteger aMultiplicities(1, nPoles+1);" EOL
    "  const Standard_Integer aDegree = 3;" EOL
    "  const Standard_Boolean isPeriodic = Standard_True;" EOL
    "" EOL
    "  for (Standard_Integer i=1; i <= aPoles.Upper(); i++)" EOL
    "    aPoles(i) = gp_Pnt2d (aPolesCoords[i-1][0]*100, aPolesCoords[i-1][1]*100);" EOL
    "  for (i=1; i <= aKnots.Upper(); i++)" EOL
    "    aKnots(i) = i-1;" EOL
    "  aMultiplicities.Init(1);" EOL
    "" EOL
    "  Handle(Geom2d_BSplineCurve) aCurve = " EOL
    "    new Geom2d_BSplineCurve (aPoles, aKnots, aMultiplicities, aDegree, isPeriodic);" EOL
    );
  drawAndOffsetCurve2d (aCurve, aName, aText);
}

void Offset2d_Presentation::sampleWire1()
{
  Standard_CString aName = "Wire 1";
  // Create an open polyline wire
  Standard_Real aCoords[][3] = {
    {0,0,0},{0,1,0},{1,1,0},{1,2,0},{2,2,0},{2,1,0},{3,1,0},{3,0,0},
    {2,0,0},{2,-1,0},{3,-1,0},{3,-2,0},{4,-2,0},{4,-1,0},{5,-1,0},
    {5,0,0},{6,0,0},{6,-1,0},{7,-1,0},{7,0,0},{8,0,0},{8,1,0},
    {7,1,0},{7,2,0},{6,2,0},{6,1,0},{5,1,0}
  };
  Standard_Integer nPoints = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  BRepBuilderAPI_MakePolygon aPol;
  for (Standard_Integer i=0; i < nPoints; i++)
    aPol.Add (gp_Pnt (aCoords[i][0]*100, aCoords[i][1]*100, aCoords[i][2]*100));
  TopoDS_Wire aWire = aPol.Wire();

  TCollection_AsciiString aText (
    "  // Create an open polyline wire" EOL
    "  Standard_Real aCoords[][3] = {" EOL
    "    {0,0,0},{0,1,0},{1,1,0},{1,2,0},{2,2,0},{2,1,0},{3,1,0},{3,0,0}," EOL
    "    {2,0,0},{2,-1,0},{3,-1,0},{3,-2,0},{4,-2,0},{4,-1,0},{5,-1,0}," EOL
    "    {5,0,0},{6,0,0},{6,-1,0},{7,-1,0},{7,0,0},{8,0,0},{8,1,0}," EOL
    "    {7,1,0},{7,2,0},{6,2,0},{6,1,0},{5,1,0}" EOL
    "  };" EOL
    "  Standard_Integer nPoints = sizeof(aCoords)/(sizeof(Standard_Real)*3);" EOL
    "  BRepBuilderAPI_MakePolygon aPol;" EOL
    "  for (Standard_Integer i=0; i < nPoints; i++)" EOL
    "    aPol.Add (gp_Pnt (aCoords[i][0]*100, aCoords[i][1]*100, aCoords[i][2]*100));" EOL
    "  TopoDS_Wire aWire = aPol.Wire();" EOL
    );
  drawAndOffsetWire (aWire, aName, aText);
}

void Offset2d_Presentation::sampleWire2()
{
  Standard_CString aName = "Wire 2";
  // Create a closed polyline wire
  Standard_Real aCoords[][3] = {
    {0,0,0},{0,1,0},{1,1,0},{1,0,0},{2,0,0},{2,-1,0},{1,-1,0},
    {1,-2,0},{0,-2,0},{0,-1,0},{-1,-1,0},{-1,0,0},{0,0,0}
  };
  Standard_Integer nPoints = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  BRepBuilderAPI_MakePolygon aPol;
  for (Standard_Integer i=0; i < nPoints; i++)
    aPol.Add (gp_Pnt (aCoords[i][0]*100, aCoords[i][1]*100, aCoords[i][2]*100));
  TopoDS_Wire aWire = aPol.Wire();

  TCollection_AsciiString aText (
    "  // Create a closed polyline wire" EOL
    "  Standard_Real aCoords[][3] = {" EOL
    "    {0,0,0},{0,1,0},{1,1,0},{1,0,0},{2,0,0},{2,-1,0},{1,-1,0}," EOL
    "    {1,-2,0},{0,-2,0},{0,-1,0},{-1,-1,0},{-1,0,0},{0,0,0}" EOL
    "  };" EOL
    "  Standard_Integer nPoints = sizeof(aCoords)/(sizeof(Standard_Real)*3);" EOL
    "  BRepBuilderAPI_MakePolygon aPol;" EOL
    "  for (Standard_Integer i=0; i < nPoints; i++)" EOL
    "    aPol.Add (gp_Pnt (aCoords[i][0]*100, aCoords[i][1]*100, aCoords[i][2]*100));" EOL
    "  TopoDS_Wire aWire = aPol.Wire();" EOL
    );
  drawAndOffsetWire (aWire, aName, aText);
}

void Offset2d_Presentation::drawAndOffsetCurve2d (Handle(Geom2d_Curve) theCurve,
                                                const Standard_CString theName,
                                                TCollection_AsciiString& theText)
{
  TCollection_AsciiString aTitle ("Offset to ");
  aTitle += theName;
  theText += EOL;
  theText += "  // Make an offset curve" EOL;
  theText += "  Standard_Real aDist = ";
  theText += TCollection_AsciiString (SampleDescrs[myIndex].dPos);
  theText += ";" EOL;
  theText += "  Handle(Geom2d_OffsetCurve) aOffCurve =" EOL;
  theText += "    new Geom2d_OffsetCurve (aCurve, aDist);" EOL;
  setResultTitle (aTitle.ToCString());
  setResultText (theText.ToCString());

  // Display theCurve
  drawCurve (theCurve, CurveColor);
  getViewer()->InitActiveViews();
  getViewer()->ActiveView()->FitAll();
  getViewer()->ActiveView()->Update();

  // Make offset curves
  Handle(AIS_InteractiveObject) aObj;
  for (int i=1; i <= SampleDescrs[myIndex].nNeg; i++)
  {
    if (WAIT_A_LITTLE)
      return;
    if (!aObj.IsNull())
      getAISContext()->Erase(aObj);
    Standard_Real aDist = SampleDescrs[myIndex].dNeg * i;
    Handle(Geom2d_OffsetCurve) aOffCurve = new Geom2d_OffsetCurve (theCurve, aDist);
    aObj = drawCurve (aOffCurve, OffsetCurveColor);
  }
  for (i=1; i <= SampleDescrs[myIndex].nPos; i++)
  {
    if (WAIT_A_LITTLE)
      return;
    if (!aObj.IsNull())
      getAISContext()->Erase(aObj);
    Standard_Real aDist = SampleDescrs[myIndex].dPos * i;
    Handle(Geom2d_OffsetCurve) aOffCurve = new Geom2d_OffsetCurve (theCurve, aDist);
    aObj = drawCurve (aOffCurve, OffsetCurveColor);
  }
}

void Offset2d_Presentation::drawAndOffsetWire(const TopoDS_Wire& theWire,
                                            const Standard_CString theName,
                                            TCollection_AsciiString& theText)
{
  TCollection_AsciiString aTitle ("Offset to ");
  aTitle += theName;
  theText += EOL;
  theText += "  // Make an offset shape" EOL;
  theText += "  Standard_Real aDist = ";
  theText += TCollection_AsciiString (SampleDescrs[myIndex].dPos);
  theText += ";" EOL;
  theText += "  BRepOffsetAPI_MakeOffset aOffAlgo (aWire, GeomAbs_Arc);" EOL;
  theText += "  aOffAlgo.Perform (aDist);" EOL;
  theText += "  if (aOffAlgo.IsDone())" EOL;
  theText += "    TopoDS_Shape aOffShape = aOffAlgo.Shape();" EOL;
  setResultTitle (aTitle.ToCString());
  setResultText (theText.ToCString());

  // Display theWire
  Handle(AIS_InteractiveObject) aObj = new AIS_Shape (theWire);
  getAISContext()->SetColor (aObj, CurveColor);
  getAISContext()->Display (aObj);
  getViewer()->InitActiveViews();
  getViewer()->ActiveView()->FitAll();
  getViewer()->ActiveView()->Update();

  // Make offset shapes
  aObj.Nullify();
  for (int i=1; i <= SampleDescrs[myIndex].nNeg; i++)
  {
    if (WAIT_A_LITTLE)
      return;
    if (!aObj.IsNull())
      getAISContext()->Erase(aObj);
    Standard_Real aDist = SampleDescrs[myIndex].dNeg * i;
    BRepOffsetAPI_MakeOffset aOffAlgo (theWire, GeomAbs_Arc);
    aOffAlgo.Perform (aDist);
    if (aOffAlgo.IsDone())
    {
      TopoDS_Shape aOffShape = aOffAlgo.Shape();
      aObj = new AIS_Shape (aOffShape);
      getAISContext()->SetColor (aObj, OffsetCurveColor);
      getAISContext()->Display (aObj);
    }
  }
  for (i=1; i <= SampleDescrs[myIndex].nPos; i++)
  {
    if (WAIT_A_LITTLE)
      return;
    if (!aObj.IsNull())
      getAISContext()->Erase(aObj);
    Standard_Real aDist = SampleDescrs[myIndex].dPos * i;
    BRepOffsetAPI_MakeOffset aOffAlgo (theWire, GeomAbs_Arc);
    aOffAlgo.Perform (aDist);
    if (aOffAlgo.IsDone())
    {
      TopoDS_Shape aOffShape = aOffAlgo.Shape();
      aObj = new AIS_Shape (aOffShape);
      getAISContext()->SetColor (aObj, OffsetCurveColor);
      getAISContext()->Display (aObj);
    }
  }
}
