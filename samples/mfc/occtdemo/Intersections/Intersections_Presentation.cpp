// Intersections_Presentation.cpp: implementation of the Intersections_Presentation class.
// Intersections of curves and surfaces
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Intersections_Presentation.h"

#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>

#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomAPI_IntCS.hxx>
#include <GeomAPI_IntSS.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Intersections_Presentation;

// Initialization of array of samples
const Intersections_Presentation::PSampleFuncType Intersections_Presentation::SampleFuncs[] =
{
  &Intersections_Presentation::InterCurveCurve,
  &Intersections_Presentation::SelfInterCurveCurve,
  &Intersections_Presentation::InterCurveSurface,
  &Intersections_Presentation::InterSurfaceSurface,
};

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Intersections_Presentation::Intersections_Presentation()
{
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Intersections of curves and surfaces");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Intersections_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////


static Handle(Geom2d_BSplineCurve) create2dBSplineCurve(const Standard_Integer thePoles,
                                 const Standard_Real theCoords[][2])
{
  TColgp_Array1OfPnt2d thePoints(1, thePoles);

  for (Standard_Integer i=0; i < thePoles; i++)
   thePoints(i+1) = gp_Pnt2d (theCoords[i][0]*100, theCoords[i][1]*100);
  
  Standard_Integer MinDegree = 3;
  Standard_Integer MaxDegree = 8;

  return Geom2dAPI_PointsToBSpline (
    thePoints, MinDegree, MaxDegree);
}


static Handle(Geom_BSplineSurface) createBSplineSurface(const Standard_Real theZCoords[],
                                                       const Standard_Real theXStep,
                                                       const Standard_Real theYStep,
                                                       const Standard_Real theXBound,
                                                       const Standard_Real theYBound)
{
  TColStd_Array2OfReal aZPoints(1,4,1,4);
    
  Standard_Integer aColLength = aZPoints.ColLength();
  Standard_Integer aRowLength = aZPoints.RowLength();
  Standard_Integer aIndex = 0;
  Standard_Integer k = 100;
  
  for(Standard_Integer i = 0 ; i < aRowLength ; i++)
    for(Standard_Integer j = 0; j < aColLength ; j++)
      aZPoints(i+1,j+1) = (theZCoords[aIndex++] * k);

  return GeomAPI_PointsToBSplineSurface (
    aZPoints,theXBound,theXStep,theYBound,theYStep,3,8);
}

//==========================================================================================
// Function : Intersections_Presentation::InterCurveCurve
// Purpose  : 
//==========================================================================================
  
void Intersections_Presentation::InterCurveCurve()
{
  setResultTitle("Intersection of 2d curves");
  TCollection_AsciiString aText(
    " // define two 2d curves" EOL
    " Handle(Geom2d_Curve) aCurve1,aCurve2;" EOL
    " // initializing curves ..." EOL EOL

    " // construct algo" EOL
    " Standard_Real Tol = 2;" EOL
    " Geom2dAPI_InterCurveCurve aInterCC(aCurve1,aCurve2,Tol);" EOL EOL

    " // number of intersection points" EOL
    " Standard_Integer aNumberPoints = aInterCC.NbPoints();" EOL EOL

    " // evaluate solutions" EOL
    " for(Standard_Integer i = 1; i <= aNumberPoints; i++)" EOL
    " {" EOL
    "   gp_Pnt2d aPnt = aInterCC.Point(i);" EOL
    "   // use point as you need ..." EOL
    " }" EOL EOL

    " // number of tangential intersections " EOL
    " Standard_Integer aNumberSegments = aInterCC.NbSegments();" EOL EOL

    " // evaluate solutions" EOL
    " for(Standard_Integer j = 1; j <= aNumberSegments; j++)" EOL
    " {" EOL
    "   Handle(Geom2d_Curve) a2dCurve1,a2dCurve2;" EOL
    "   aInterCC.Segment(j,a2dCurve1,a2dCurve2);" EOL
    "   // use curves as you need ..." EOL
    " }" EOL EOL
    );
  setResultText(aText.ToCString());


  // define arrays of points
  Standard_Real aPolesCoords1[][2] = {
    {-6,0.5},{-5,1},{-2,0},{-1,-1.01},{0,-3},{2,-4},{4,-1},{5,0}
  };
   Standard_Real aPolesCoords2[][2] = {{-6,1},{-5,0},{-4,-1},
    {-3,-1},{-2,0},{-1,-1},{0,-2.5},{1,-3.4},{2,-2},{3,-2},{4,-2},{5,-3}
  };

  // define arrays lengths
  Standard_Integer nPoles1 = sizeof(aPolesCoords1)/(sizeof(Standard_Real)*2);
  Standard_Integer nPoles2 = sizeof(aPolesCoords2)/(sizeof(Standard_Real)*2);

  // make two bspline curves
  Handle(Geom2d_BSplineCurve) aCurve1 = create2dBSplineCurve(nPoles1,aPolesCoords1);
  Handle(Geom2d_BSplineCurve) aCurve2 = create2dBSplineCurve(nPoles2,aPolesCoords2);

  // construct algo
  Standard_Real Tol = 2;
  Geom2dAPI_InterCurveCurve aInterCC(aCurve1,aCurve2,Tol);

  // number of intersection points
  Standard_Integer aNumberPoints = aInterCC.NbPoints();
  // number of tangential intersections 
  Standard_Integer aNumberSegments = aInterCC.NbSegments();
  
  // output
  aText = aText +
    " //=================================================" EOL EOL

    " Number of intersection points = " + TCollection_AsciiString(aNumberPoints) + EOL
    " Number of tangential intersections = " + TCollection_AsciiString(aNumberSegments) + EOL;
  setResultText(aText.ToCString());

  gp_Pnt aPoint(0,0,0);
  gp_Dir aDir(0,0,1);
  gp_Ax2 anAx2(aPoint,aDir);
  
  drawCurve(aCurve1,Quantity_Color(Quantity_NOC_RED),Standard_True,anAx2);
  drawCurve(aCurve2,Quantity_Color(Quantity_NOC_RED),Standard_True,anAx2);
  
  if(WAIT_A_SECOND) return;
  // output intersection points
  if (aNumberPoints > 0)
    for(Standard_Integer i = 1; i <= aNumberPoints; i++)
    {
      drawPoint(gp_Pnt(aInterCC.Point(i).X(),aInterCC.Point(i).Y(),0));
    }

  if(WAIT_A_SECOND) return;
  
  if (aNumberSegments > 0)
    for(Standard_Integer j = 1; j <= aNumberSegments; j++)
    {
      Handle(Geom2d_Curve) a2dCurve1;
      Handle(Geom2d_Curve) a2dCurve2;
      aInterCC.Segment(j,a2dCurve1,a2dCurve2);
      drawCurve(a2dCurve1,Quantity_Color(Quantity_NOC_YELLOW),Standard_True,anAx2);
      drawCurve(a2dCurve2,Quantity_Color(Quantity_NOC_YELLOW),Standard_True,anAx2);
    }
}

//==========================================================================================
// Function : Intersections_Presentation::SelfInterCurveCurve
// Purpose  : 
//==========================================================================================
  
void Intersections_Presentation::SelfInterCurveCurve()
{
   setResultTitle("Self-intersection of 2d curve");
  TCollection_AsciiString aText(
    " // define 2d curve" EOL
    " Handle(Geom2d_Curve) aCurve1;" EOL
    " // initializing curve ..." EOL EOL

    " // construct algo" EOL
    " Standard_Real Tol = 2;" EOL
    " Geom2dAPI_InterCurveCurve aInterCC(aCurve1,Tol);" EOL EOL

    " // number of intersection points" EOL
    " Standard_Integer aNumberPoints = aInterCC.NbPoints();" EOL EOL

    " // evaluate solutions" EOL
    " for(Standard_Integer i = 1; i <= aNumberPoints; i++)" EOL
    " {" EOL
    "   gp_Pnt2d aPnt = aInterCC.Point(i);" EOL
    "   // use point as you need ..." EOL
    " }" EOL EOL

    " // number of tangential intersections " EOL
    " Standard_Integer aNumberSegments = aInterCC.NbSegments();" EOL EOL

    " // evaluate solutions" EOL
    " for(Standard_Integer j = 1; j <= aNumberSegments; j++)" EOL
    " {" EOL
    "   Handle(Geom2d_Curve) a2dCurve1;" EOL
    "   aInterCC.Segment(j,a2dCurve1);" EOL
    "   // use curve as you need ..." EOL
    " }" EOL EOL 
    );
  setResultText(aText.ToCString());

  // define array of points
  Standard_Real aPolesCoords1[][2] = {
    {-5,-2},{-4,-1},{0,2.4},{6,4},{6,-3},
    {1,-1},{-3,2},{-1,2.2},{3,3.5},{4,4.9}
    };

  // define array length
  Standard_Integer nPoles1 = sizeof(aPolesCoords1)/(sizeof(Standard_Real)*2);

  // make bspline curve
  Handle(Geom2d_BSplineCurve) aCurve1 = create2dBSplineCurve(nPoles1,aPolesCoords1);

  // construct algo
  Standard_Real Tol = 2;
  Geom2dAPI_InterCurveCurve aInterCC(aCurve1,Tol);
  
  aInterCC.Init(aCurve1,Tol);

  // number of intersection points
  Standard_Integer aNumberPoints = aInterCC.NbPoints();
  // number of tangential intersections 
  Standard_Integer aNumberSegments = aInterCC.NbSegments();
  
  // output
  aText = aText +
    " //=================================================" EOL EOL

    " Number of intersection points = " + TCollection_AsciiString(aNumberPoints) + EOL
    " Number of tangential intersections = " + TCollection_AsciiString(aNumberSegments) + EOL;
  setResultText(aText.ToCString());

  gp_Pnt aPoint(0,0,0);
  gp_Dir aDir(0,0,1);
  gp_Ax2 anAx2(aPoint,aDir);
  
  drawCurve(aCurve1,Quantity_Color(Quantity_NOC_RED),Standard_True,anAx2);
  
  if(WAIT_A_SECOND) return;
  
  // output intersection points
  for(Standard_Integer i = 1; i <= aNumberPoints; i++)
   drawPoint(gp_Pnt(aInterCC.Point(i).X(),aInterCC.Point(i).Y(),0));
  

  if(WAIT_A_SECOND) return;
  
  // output tangential intersections 
  for(Standard_Integer j = 1; j <= aNumberSegments; j++)
  {
    Handle(Geom2d_Curve) a2dCurve1;
    aInterCC.Segment(j,a2dCurve1);
    drawCurve(a2dCurve1,Quantity_Color(Quantity_NOC_YELLOW),Standard_True,anAx2);
  }
}

//==========================================================================================
// Function : Intersections_Presentation::InterCurveSurface
// Purpose  : 
//==========================================================================================


void Intersections_Presentation::InterCurveSurface()
{
 
  setResultTitle("Intersection of curve and surface");
  TCollection_AsciiString aText(
    " // define curve" EOL
    " Handle(Geom_Curve) aCurve;" EOL
    " // initializing curve ..." EOL EOL

    " // define surface" EOL
    " Handle(Geom_Surface) aSurface;" EOL
    " // initializing surface ..." EOL EOL

    " // construct algo" EOL
    " GeomAPI_IntCS aInterCS(aCurve,aSurface);" EOL
    " if(!aInterCS.IsDone()) return;" EOL EOL

    " // number of intersection points" EOL
    " Standard_Integer aNumberPoints = aInterCS.NbPoints();" EOL EOL

    " // evaluate solutions" EOL
    " for(Standard_Integer i = 1; i <= aNumberPoints; i++)" EOL
    " {" EOL
    "   Standard_Real aParamU,aParamV,aParamOnCurve;" EOL
    "   aInterCS.Parameters(i,aParamU,aParamV,aParamOnCurve);" EOL
    "   gp_Pnt aPnt = aInterCS.Point(i);" EOL
    "   // use solution as you need ..." EOL
    " }" EOL EOL

    " // number of tangential intersections " EOL
    " Standard_Integer aNumberSegments = aInterCS.NbSegments();" EOL EOL

    " // evaluate solutions" EOL
    " for(Standard_Integer j = 1; j <= aNumberSegments; j++)" EOL
    "   Handle(Geom_Curve) aCurve1 = aInterCS.Segment(j);" EOL
    "   // use solution as you need ..." EOL EOL
    );
  setResultText(aText.ToCString());
  
  // define bounds surface
  Standard_Real aXStep = 262.5, aYStep = 262.5;
  Standard_Real aXBound = -450, aYBound = -300;

  // define array of points
  Standard_Real aZCoords [] = {{-1},{0},{0},{0.5},{0},{-1},
   {-1},{0},{-1},{-1.5},{-1},{-1},{1},{-1},{-1},{-1}}; 

  // creating bspline surface
  Handle(Geom_BSplineSurface) aSurface = createBSplineSurface(aZCoords,aXStep,aYStep,aXBound,aYBound);
  
  // make bspline curve
  // define array of points
  Standard_Real aCoords[][3] = {{-3,-3,-2},{-2,-2,-1},{-1,-1,-1},
   {0,0,1},{1,2,3},{2,1,0},{3,1,-3},{4,2,-4},{5,2,-3}};
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  
  TColgp_Array1OfPnt aCurvePoint (1, nPoles);
  
  for (Standard_Integer n=0; n < nPoles; n++)
    aCurvePoint(n+1) = gp_Pnt (aCoords[n][0]*100, aCoords[n][1]*100, aCoords[n][2]*100);
  
  // define parameters
  Standard_Integer MinDegree = 3;
  Standard_Integer MaxDegree = 8;

  // make bspline curve
  GeomAPI_PointsToBSpline aCurve(aCurvePoint, MinDegree, MaxDegree);


  // construct algo
  GeomAPI_IntCS aInterCS(aCurve,aSurface);

  if(!aInterCS.IsDone()) return;
    
  // number of intersection points
  Standard_Integer aNumberPoints = aInterCS.NbPoints();
  
  // number of tangential intersections 
  Standard_Integer aNumberSegments = aInterCS.NbSegments(); 
  
  // define and output parameters points
  aText = aText +
    " //=================================================" EOL EOL
    " Number of intersection points = " + TCollection_AsciiString(aNumberPoints) + EOL 
    " Number of tangential intersections = " + TCollection_AsciiString(aNumberSegments) + EOL EOL;

  if(aNumberPoints > 0)
  {
    aText = aText +
    " Parameters (U,V) on the surface of the intersection point." EOL 
    " Parameter (ParamOnCurve) on the curve of the intersection point." EOL EOL;
  
  for(Standard_Integer j = 1; j <= aNumberPoints; j++)
  {
    Standard_Real aParamU,aParamV,aParamOnCurve;
    aInterCS.Parameters(j,aParamU,aParamV,aParamOnCurve);

    aText = aText +
      " Intersection point " + j + " : U = " + TCollection_AsciiString(aParamU) + EOL
      "                      : V = " + TCollection_AsciiString(aParamV) + EOL
      "             ParamOnCurve = " + TCollection_AsciiString(aParamOnCurve) + EOL EOL;
  }
  setResultText(aText.ToCString());
  }

  drawSurface(aSurface);
  if(WAIT_A_SECOND) return;
  drawCurve(aCurve);
  if(WAIT_A_SECOND) return;

  for(Standard_Integer i = 1; i <= aNumberPoints; i++)
  {
    drawPoint(gp_Pnt(aInterCS.Point(i)));
  }


  for(Standard_Integer k = 1; k <= aNumberSegments; k++)
  {
    Handle(Geom_Curve) aCurve1 = aInterCS.Segment(k);
    drawCurve(aCurve1,Quantity_NOC_YELLOW);
  }
}

//==========================================================================================
// Function : Intersections_Presentation::InterSurfaceSurface
// Purpose  : 
//==========================================================================================

void Intersections_Presentation::InterSurfaceSurface()
{
  setResultTitle("Intersection of surfaces");
  TCollection_AsciiString aText(
    " // define two surfaces" EOL
    " Handle(Geom_Surface) aSurface1,aSurface2;" EOL
    " // initializing surfaces ..." EOL EOL

    " // construct algo" EOL
    " Standard_Real Tol = 0.1e-7;" EOL
    " GeomAPI_IntSS aInterSS(aSurface1,aSurface2,Tol);" EOL
    " if(!aInterSS.IsDone()) return;" EOL EOL

    " // number of intersection lines" EOL
    " Standard_Integer aNumberLines = aInterSS.NbLines();" EOL EOL

    " // evaluate solutions" EOL
    " for(Standard_Integer i = 1; i <= aNumberLines; i++)" EOL
    " {" EOL
    "   Handle(Geom_Curve) aCurve = aInterSS.Line(i);" EOL
    "   // use aCurve as you need ..." EOL
    " }" EOL EOL

   );
  setResultText(aText.ToCString());
  
  // define bounds of surfaces
  Standard_Real aXStep1 = 250, aYStep1 = 250;
  Standard_Real aXBound1 = -450, aYBound1 = -300;
  Standard_Real aXStep2 = 250, aYStep2 = 150;
  Standard_Real aXBound2 = -650, aYBound2 = -200;

  // define arrays of points
  Standard_Real aZCoords1 [] = {{-1},{0},{0},{-0.5},{0},{-1},
   {-1},{0},{-1},{-1.5},{-1},{-1},{-2},{-1},{-1},{-1}}; 

  Standard_Real aZCoords2 [] = {{-2},{-1},{-1},{-1},{-1},{-1.5},
   {-1.5},{-1},{-1.5},{-1},{0},{0},{-1},{0},{0},{0}}; 

  // make bspline surface
  Handle(Geom_BSplineSurface) aSurface1 = createBSplineSurface(aZCoords1,aXStep1,aYStep1,aXBound1,aYBound1);
  Handle(Geom_BSplineSurface) aSurface2 = createBSplineSurface(aZCoords2,aXStep2,aYStep2,aXBound2,aYBound2);

  // construct algo
  Standard_Real Tol = 0.1e-7;
  GeomAPI_IntSS aInterSS(aSurface1,aSurface2,Tol);

  if(!aInterSS.IsDone()) return;

  // number of intersection lines
  Standard_Integer aNumberLines = aInterSS.NbLines();

  // output
  aText = aText +
    " //=================================================" EOL EOL
    " Number of intersection lines = " + TCollection_AsciiString(aNumberLines) + EOL;
  setResultText(aText.ToCString());

  drawSurface(aSurface1);
  if(WAIT_A_SECOND) return;
  drawSurface(aSurface2,Quantity_NOC_YELLOW);
  if(WAIT_A_SECOND) return;
  
  Standard_Real aLineWidth = 3.0;

  for(Standard_Integer i = 1; i <= aNumberLines; i++)
  {
    // show intersection line
    Handle(Geom_Curve) aCurve = aInterSS.Line(i);
    Handle(AIS_InteractiveObject) anIO = drawCurve(aCurve, Quantity_NOC_RED, Standard_False);
    Handle(Prs3d_LineAspect) aLA = 
      new Prs3d_LineAspect(Quantity_NOC_RED,Aspect_TOL_SOLID,aLineWidth);
    anIO->Attributes()->SetLineAspect(aLA);
    getAISContext()->Display(anIO); 
  }
}


