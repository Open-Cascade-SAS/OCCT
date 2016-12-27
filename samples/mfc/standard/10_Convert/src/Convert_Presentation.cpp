// Convert_Presentation.cpp: implementation of the Convert_Presentation class.
// Conversion of elementary geometry to BSpline curves and surfaces
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Convert_Presentation.h"

#include <Quantity_Color.hxx>

#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>

#include <GeomConvert.hxx>

#include <Geom_BezierSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_BSplineSurface.hxx>


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Convert_Presentation;

// Initialization of array of samples
const Convert_Presentation::PSampleFuncType Convert_Presentation::SampleFuncs[] =
{
  &Convert_Presentation::sampleCircle,
  &Convert_Presentation::sampleEllipse,
  &Convert_Presentation::sampleBezier,
  &Convert_Presentation::sampleBezierSurface,
  &Convert_Presentation::sampleCylindricalSurface,
  &Convert_Presentation::sampleRevolSurface,
  &Convert_Presentation::sampleToroidalSurface,
  &Convert_Presentation::sampleConicalSurface,
  &Convert_Presentation::sampleSphericalSurface
};

// Colors of objects
static const Quantity_Color CurveColor       (1,1,0, Quantity_TOC_RGB);      // yellow
static const Quantity_Color SurfaceColor     (1,1,0, Quantity_TOC_RGB);      // yellow
static const Quantity_Color BSplineColor     (1,0.647,0, Quantity_TOC_RGB);  // orange
static const Quantity_Color BSplineSurfaceColor (0,0,1, Quantity_TOC_RGB);   // blue

#define EOL "\r\n"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Convert_Presentation::Convert_Presentation()
{
  setName ("Conversion to BSpline curves and surfaces");
  myIndex = 0;
  myNbFuncs = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  myNbSamples = myNbFuncs;
  FitMode = true;
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Convert_Presentation::DoSample()
{
	((COCCDemoApp*) AfxGetApp())->SetSampleName (L"Convert");
  ((COCCDemoApp*) AfxGetApp())->SetSamplePath (L"..\\..\\10_Convert");
	getAISContext()->EraseAll (Standard_True);
	if (myIndex >=0 && myIndex < myNbFuncs)
    (this->*SampleFuncs[myIndex])();
}

//================================================================
// Function : Convert_Presentation::drawSurfaceAndItsBSpline
// Purpose  : 
//================================================================
void Convert_Presentation::drawSurfaceAndItsBSpline(const Handle(Geom_Surface) & theSurface, 
                                                    const Standard_CString theName, 
                                                    TCollection_AsciiString& theText)
{
  TCollection_AsciiString aTitle ("Converting ");
  aTitle += theName;
  aTitle += " to BSpline surface";

  theText += EOL
    "  Handle(Geom_BSplineSurface) aBSplineSurface = " EOL
    "    GeomConvert::SurfaceToBSplineSurface(aSurface);" EOL;

  setResultTitle (aTitle.ToCString());
  setResultText (theText.ToCString());

  drawSurface (theSurface, SurfaceColor);

  if (WAIT_A_LITTLE) return;

  Handle(Geom_BSplineSurface) aBSplineSurface = GeomConvert::SurfaceToBSplineSurface(theSurface);

  _ASSERTE(!aBSplineSurface.IsNull());

  drawSurface (aBSplineSurface, BSplineSurfaceColor);
}

//================================================================
// Function : Convert_Presentation::drawCurveAndItsBSpline
// Purpose  : 
//================================================================
void Convert_Presentation::drawCurveAndItsBSpline(Handle(Geom_Curve) theCurve, 
                                                  const Standard_CString theName, 
                                                  TCollection_AsciiString& theText)
{
  TCollection_AsciiString aTitle ("Converting ");
  aTitle += theName;
  aTitle += " to BSpline curve";

  theText += EOL
    "  Handle(Geom_BSplineCurve) aBSpline = " EOL
    "    GeomConvert::CurveToBSplineCurve(aCurve);" EOL;

  setResultTitle (aTitle.ToCString());
  setResultText (theText.ToCString());
  
  drawCurve (theCurve, CurveColor);
  
  if (WAIT_A_LITTLE) return;

  Handle(Geom_Curve) aBSpline = GeomConvert::CurveToBSplineCurve(theCurve);

  drawCurve (aBSpline, BSplineColor);
}


//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : Convert_Presentation::sampleCircle
// Purpose  : 
//================================================================
void Convert_Presentation::sampleCircle()
{
  gp_Pnt aOrigin (0,0,0);
  gp_Dir aDir (1,0,0);
  gp_Ax2 aAxis (aOrigin, aDir);
  Standard_Real aRadius = 300;
  Handle(Geom_Circle) aCurve = new Geom_Circle (aAxis, aRadius);

  TCollection_AsciiString aText (
    "  gp_Pnt aOrigin (0,0,0);" EOL
    "  gp_Dir aDir (1,0,0);" EOL
    "  gp_Ax2 aAxis (aOrigin, aDir);" EOL
    "  Standard_Real aRadius = 300;" EOL
    "  Handle(Geom_Circle) aCurve = new Geom_Circle (aAxis, aRadius);" EOL
    );
  drawCurveAndItsBSpline (aCurve, "Circle", aText);
}

//================================================================
// Function : Convert_Presentation::sampleEllipse
// Purpose  : 
//================================================================
void Convert_Presentation::sampleEllipse()
{
  gp_Pnt aOrigin (0,0,0);
  gp_Dir aDir (1,0,0);
  gp_Ax2 aMajorAxis (aOrigin, aDir);
  Standard_Real aMajorRadius = 300;
  Standard_Real aMinorRadius = 150;
  Handle(Geom_Ellipse) aCurve = 
    new Geom_Ellipse (aMajorAxis, aMajorRadius, aMinorRadius);

  TCollection_AsciiString aText (
    "  gp_Pnt aOrigin (0,0,0);" EOL
    "  gp_Dir aDir (1,0,0);" EOL
    "  gp_Ax2 aAxis (aOrigin, aDir);" EOL
    "  Standard_Real aMajorRadius = 300;" EOL
    "  Standard_Real aMinorRadius = 150;" EOL
    "  Handle(Geom_Ellipse) aCurve = " EOL
    "    new Geom_Ellipse (aAxis, aMajorRadius, aMinorRadius);" EOL
    );
  drawCurveAndItsBSpline (aCurve, "Ellipse", aText);
}

//================================================================
// Function : Convert_Presentation::sampleBezier
// Purpose  : 
//================================================================
void Convert_Presentation::sampleBezier()
{
  TCollection_AsciiString aText (
    "  Standard_Real aPolesCoords[][3] = {" EOL
    "    {0,0,0},{0,1,0},{1,1,0},{1,2,0},{2,2,0},{2,1,0},{3,1,0},{3,0,0},{2,0,0},{2,-1,0}," EOL
    "    {3,-1,0},{3,-2,0},{4,-2,0},{4,-1,0},{5,-1,0},{5,0,0},{6,0,0},{6,-1,0},{7,-1,0}," EOL
    "    {7,0,0},{8,0,0},{8,1,0}" EOL
    "  };" EOL
    "  TColgp_Array1OfPnt aPoles (1, sizeof(aPolesCoords)/(sizeof(Standard_Real)*2));" EOL
    " " EOL
    "  for (Standard_Integer i=1; i <= aPoles.Upper(); i++)" EOL
    "    aPoles(i) = gp_Pnt (aPolesCoords[i-1][0]*100, " EOL
    "                        aPolesCoords[i-1][1]*100, " EOL
    "                        aPolesCoords[i-1][2]*100);" EOL
    "  " EOL
    "  Handle(Geom_BezierCurve) aCurve = new Geom_BezierCurve (aPoles);" EOL
    );

  Standard_Real aPolesCoords[][3] = {
    {0,0,0},{0,1,0},{1,1,0},{1,2,0},{2,2,0},{2,1,0},{3,1,0},{3,0,0},{2,0,0},{2,-1,0},
    {3,-1,0},{3,-2,0},{4,-2,0},{4,-1,0},{5,-1,0},{5,0,0},{6,0,0},{6,-1,0},{7,-1,0},
    {7,0,0},{8,0,0},{8,1,0}
  };
  TColgp_Array1OfPnt aPoles (1, sizeof(aPolesCoords)/(sizeof(Standard_Real)*3));
 
  for (Standard_Integer i=1; i <= aPoles.Upper(); i++)
    aPoles(i) = gp_Pnt (aPolesCoords[i-1][0]*150-500, 
                        aPolesCoords[i-1][1]*150, 
                        aPolesCoords[i-1][2]*150);
  
  Handle(Geom_BezierCurve) aCurve = new Geom_BezierCurve (aPoles);

  drawCurveAndItsBSpline (aCurve, "BezierCurve", aText);
}

//================================================================
// Function : Convert_Presentation::sampleBezierSurface
// Purpose  : 
//================================================================
void Convert_Presentation::sampleBezierSurface()
{
  getAISContext()->EraseAll (Standard_True);

  Standard_CString aName = "BezierSurface";
  // Create a BezierSurface
  TColgp_Array2OfPnt aPoles(1,2,1,4); // 8 points
  TColStd_Array2OfReal aWeights(1,2,1,4);
  // initializing array of points
  aPoles.SetValue(1,1,gp_Pnt(0,10,0));     aPoles.SetValue(1,2,gp_Pnt(3.3,6.6,3));
  aPoles.SetValue(1,3,gp_Pnt(6.6,6.6,-3)); aPoles.SetValue(1,4,gp_Pnt(10,10,0));
  aPoles.SetValue(2,1,gp_Pnt(0,0,0));      aPoles.SetValue(2,2,gp_Pnt(3.3,3.3,-3));
  aPoles.SetValue(2,3,gp_Pnt(6.6,3.3,3));  aPoles.SetValue(2,4,gp_Pnt(10,0,0));  
  // scaling poles
  for (Standard_Integer i=1; i <= aPoles.ColLength(); i++)
    for (Standard_Integer j=1; j <= aPoles.RowLength(); j++)
      aPoles(i,j).ChangeCoord() = aPoles(i,j).Coord() * 100 + gp_XYZ(-500,-500,0);
  //initializing array of weights
  aWeights.SetValue(1,1,1); aWeights.SetValue(1,2,3);
  aWeights.SetValue(1,3,9); aWeights.SetValue(1,4,1);
  aWeights.SetValue(2,1,1); aWeights.SetValue(2,2,2);
  aWeights.SetValue(2,3,5); aWeights.SetValue(2,4,1);
  Handle(Geom_BezierSurface) aSurface =
    new Geom_BezierSurface(aPoles, aWeights);

  TCollection_AsciiString aText (
    "  // Create a BezierSurface" EOL
    "  TColgp_Array2OfPnt aPoles(1,2,1,4); // 8 points" EOL
    "  TColStd_Array2OfReal aWeights(1,2,1,4);" EOL
    "  // initializing array of points" EOL
    "  aPoles.SetValue(1,1,gp_Pnt(0,10,0));     aPoles.SetValue(1,2,gp_Pnt(3.3,6.6,3));" EOL
    "  aPoles.SetValue(1,3,gp_Pnt(6.6,6.6,-3)); aPoles.SetValue(1,4,gp_Pnt(10,10,0));" EOL
    "  aPoles.SetValue(2,1,gp_Pnt(0,0,0));      aPoles.SetValue(2,2,gp_Pnt(3.3,3.3,-3));" EOL
    "  aPoles.SetValue(2,3,gp_Pnt(6.6,3.3,3));  aPoles.SetValue(2,4,gp_Pnt(10,0,0));  " EOL
    "  // scaling poles" EOL
    "  for (Standard_Integer i=1; i <= aPoles.ColLength(); i++)" EOL
    "    for (Standard_Integer j=1; j <= aPoles.RowLength(); j++)" EOL
    "      aPoles(i,j).ChangeCoord() = aPoles(i,j).Coord() * 100 + gp_XYZ(-500,-500,0);" EOL
    "  //initializing array of weights" EOL
    "  aWeights.SetValue(1,1,1); aWeights.SetValue(1,2,3);" EOL
    "  aWeights.SetValue(1,3,9); aWeights.SetValue(1,4,1);" EOL
    "  aWeights.SetValue(2,1,1); aWeights.SetValue(2,2,2);" EOL
    "  aWeights.SetValue(2,3,5); aWeights.SetValue(2,4,1);" EOL
    "  Handle(Geom_BezierSurface) aSurface =" EOL
    "    new Geom_BezierSurface(aPoles, aWeights);" EOL
    );

  drawSurfaceAndItsBSpline (aSurface, aName, aText);
}

//================================================================
// Function : OCCDemo_Presentation::sampleCylindricalSurface
// Purpose  : 
//================================================================
void Convert_Presentation::sampleCylindricalSurface()
{
  getAISContext()->EraseAll (Standard_True);

  Standard_CString aName = "Cylindrical Surface";
  TCollection_AsciiString aText (
    "  // creating an axis parallel to Y axis" EOL
    "  gp_Ax3 anAx(gp_Pnt(0,0,0), gp_Dir(0,1,0));" EOL EOL

    "  // creating a cylindrical surface along anAx with radius = 100" EOL
    "  Handle(Geom_CylindricalSurface) aCylSurface = new Geom_CylindricalSurface(anAx, 100);" EOL EOL

    "  // only finit surfaces can be converted to BSpline surfaces, " EOL
    "  // cylindrical surface is infinite, it must be trimmed" EOL
    "  Handle(Geom_RectangularTrimmedSurface) aSurface = " EOL
    "    new Geom_RectangularTrimmedSurface(aCylSurface, 0, 2*PI, -1000, 1000, Standard_True, Standard_True);" EOL);

  // creating an axis parallel to Y axis
  gp_Ax3 anAx(gp_Pnt(0,0,0), gp_Dir(0,1,0));

  // creating a cylindrical surface along anAx with radius = 4
  Handle(Geom_CylindricalSurface) aCylSurface = new Geom_CylindricalSurface(anAx, 100);

  // only finit surfaces can be converted to BSpline surfaces, 
  // cylindrical surface is infinite, it must be trimmed
  Handle(Geom_RectangularTrimmedSurface) aSurface = 
    new Geom_RectangularTrimmedSurface(aCylSurface, 0, 2*M_PI, -1000, 1000, Standard_True, Standard_True);
  
  drawSurfaceAndItsBSpline(aSurface, aName, aText);
}

//================================================================
// Function : OCCDemo_Presentation::sampleRevolSurface
// Purpose  : 
//================================================================
void Convert_Presentation::sampleRevolSurface()
{
  FitMode=false;
  ResetView();
  TranslateView(-176.84682, -102.12892);
  SetViewScale(0.69326);

  getAISContext()->EraseAll (Standard_True);

  Standard_CString aName = "Surface of revolution";
  TCollection_AsciiString aText (
    "  // creating a curve for revolution.  Let it be a Bezier curve." EOL
    "  Handle(Geom_BezierCurve) aBezierCurve;" EOL EOL

    "  // array of the bezier curve poles" EOL
    "  TColgp_Array1OfPnt aPoles(1,4);" EOL
    "  // array of the poles' weights" EOL
    "  TColStd_Array1OfReal aWeights(1,4);" EOL EOL

    "  aPoles(1) = gp_Pnt(0, 0, 0);      aWeights(1) = 1;" EOL
    "  aPoles(2) = gp_Pnt(150, 250, 0);  aWeights(2) =75;" EOL
    "  aPoles(3) = gp_Pnt(350, 150, 0);  aWeights(3) =120;" EOL
    "  aPoles(4) = gp_Pnt(500, 500, 0);  aWeights(4) = 1;" EOL EOL

    "  // creating a bezier curve" EOL
    "  aBezierCurve = new Geom_BezierCurve(aPoles, aWeights);" EOL EOL

    "  // creating a surface of revolution of the bezier curve around Y axis" EOL
    "  gp_Ax1 anAx(gp_Pnt(0, 0, 0), gp_Dir(0,1,0));" EOL
    "  Handle(Geom_SurfaceOfRevolution) aSurface = new Geom_SurfaceOfRevolution(aBezierCurve, anAx);" EOL
    );
  
  // array of the bezier curve poles
  TColgp_Array1OfPnt aPoles(1,4);
  // array of the poles' weights
  TColStd_Array1OfReal aWeights(1,4);

  aPoles(1) = gp_Pnt(0, 0, 0);      aWeights(1) = 1;
  aPoles(2) = gp_Pnt(150, 250, 0);  aWeights(2) =75;
  aPoles(3) = gp_Pnt(350, 150, 0);  aWeights(3) =120;
  aPoles(4) = gp_Pnt(500, 500, 0);  aWeights(4) = 1;

  Handle(Geom_Curve) aBezierCurve = new Geom_BezierCurve(aPoles, aWeights);
  drawCurve(aBezierCurve);

  // creating a surface of revolution of the bezier curve around Y axis
  gp_Ax1 anAx(gp_Pnt(0,0,0), gp_Dir(0,1,0));
  Handle(Geom_SurfaceOfRevolution) aSurface = new Geom_SurfaceOfRevolution(aBezierCurve, anAx);

  drawSurfaceAndItsBSpline (aSurface, aName, aText);
  FitMode=true;
}

//================================================================
// Function : Convert_Presentation::sampleToroidalSurface
// Purpose  : 
//================================================================
void Convert_Presentation::sampleToroidalSurface()
{
  getAISContext()->EraseAll (Standard_True);

  Standard_CString aName = "Toroidal surface";
  TCollection_AsciiString aText (
    "  // creating an axis parallel to Y axis" EOL
    "  gp_Ax3 anAx(gp_Pnt(0,0,0), gp_Dir(0,1,0));" EOL
    "  // creating a toroidal surface with major radius = 240 and minor radius = 120" EOL
    "  Handle(Geom_ToroidalSurface) aSurface = new Geom_ToroidalSurface(anAx, 240, 120);" EOL);

  // creating an axis parallel to Y axis 
  gp_Ax3 anAx(gp_Pnt(0,0,0), gp_Dir(0,1,0)); 
  // creating a toroidal surface with major radius = 240 and minor radius = 120
  Handle(Geom_ToroidalSurface) aSurface = new Geom_ToroidalSurface(anAx, 240, 120);

  drawSurfaceAndItsBSpline(aSurface, aName, aText);
}

//================================================================
// Function : Convert_Presentation::sampleConicalSurface
// Purpose  : 
//================================================================
void Convert_Presentation::sampleConicalSurface()
{
  getAISContext()->EraseAll (Standard_True);

  Standard_CString aName = "Conical surface";
  TCollection_AsciiString aText (
    "  // creating an axis parallel to Z axis" EOL
    "  gp_Ax3 anAx(gp_Pnt(0,0,0), gp_Dir(0,0,1)); " EOL
    "  // creating a conical surface with base radius = 10 and angle = 20 deg" EOL
    "  Handle(Geom_ConicalSurface) aConicalSurface = new Geom_ConicalSurface(anAx,PI/9., 10);" EOL EOL

    "  // only finit surfaces can be converted to BSpline surfaces, " EOL
    "  // conical surface is infinite, it must be trimmed" EOL
    "  Handle(Geom_RectangularTrimmedSurface) aSurface = " EOL
    "    new Geom_RectangularTrimmedSurface(aConicalSurface, 0, 2*PI, -1000, 1000, Standard_True, Standard_True);" EOL);

  // creating an axis parallel to Z axis 
  gp_Ax3 anAx(gp_Pnt(0,0,0), gp_Dir(0,0,1)); 
  // creating a conical surface with base radius = 10 and angle = 20 deg
  Handle(Geom_ConicalSurface) aConicalSurface = new Geom_ConicalSurface(anAx,M_PI/9., 10);

  // only finit surfaces can be converted to BSpline surfaces, 
  // conical surface is infinite, it must be trimmed
  Handle(Geom_RectangularTrimmedSurface) aSurface = 
    new Geom_RectangularTrimmedSurface(aConicalSurface, 0, 2*M_PI, -1000, 1000, Standard_True, Standard_True);

  drawSurfaceAndItsBSpline(aSurface, aName, aText);
}

//================================================================
// Function : Convert_Presentation::sampleSphericalSurface
// Purpose  : 
//================================================================
void Convert_Presentation::sampleSphericalSurface()
{
  getAISContext()->EraseAll (Standard_True);

  Standard_CString aName = "Spherical surface";
  TCollection_AsciiString aText (
    "// creating an axis parallel to Z axis" EOL
    "gp_Ax3 anAx(gp_Pnt(0,0,0), gp_Dir(0,0,1));" EOL
    "// creating a spherical surface with radius = 300" EOL
    "Handle(Geom_SphericalSurface) aSurface = new Geom_SphericalSurface(anAx,300);" EOL);

  // creating an axis parallel to Z axis 
  gp_Ax3 anAx(gp_Pnt(0,0,0), gp_Dir(0,0,1)); 
  // creating a spherical surface with radius = 300
  Handle(Geom_SphericalSurface) aSurface = new Geom_SphericalSurface(anAx,300);

  drawSurfaceAndItsBSpline(aSurface, aName, aText);
}

