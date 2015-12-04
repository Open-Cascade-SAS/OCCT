// Projection_Presentation.cpp: implementation of the Projection_Presentation class.
// Projection of points on curves and curves on surfaces
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Projection_Presentation.h"

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>

#include <Geom_Line.hxx>
#include <GeomProjLib.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <Precision.hxx>
#include <OCCDemo_Presentation.h>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>

#include <gce_MakeLin.hxx>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <Approx_CurveOnSurface.hxx>
#include <ProjLib_CompProjectedCurve.hxx>
#include <TColGeom_SequenceOfCurve.hxx>

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Projection_Presentation;

// Initialization of array of samples
const Projection_Presentation::PSampleFuncType Projection_Presentation::SampleFuncs[] =
{
  &Projection_Presentation::sample1,
  &Projection_Presentation::sample2,
  &Projection_Presentation::sample3,
  &Projection_Presentation::sample4,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Projection_Presentation::Projection_Presentation()
{
  FitMode=false;
  myIndex = 0;
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Projection of points onto curve and surface, and curve onto surface");
}

//////////////////////////////////////////////////////////////////////
// Create bspline curve
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : createCurve
// Purpose  : returns a BSpline curve with 10 poles
//================================================================
static Handle(Geom_BSplineCurve) createCurve()
{
  Standard_Real aCoords[][3] = {{-6,3,0},{-5,1,0},{-4,0,0},{-2,1,0},{-1,-1,0},
                                {0,-1,0},{1,0,0},{2,2,0},{3,1,0},{4,0,0}};

  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  TColgp_Array1OfPnt aCurvePoint (1, nPoles);

  for (Standard_Integer i=0; i < nPoles; i++)
    aCurvePoint(i+1) = gp_Pnt (aCoords[i][0]*100, aCoords[i][1]*100, aCoords[i][2]*100);

  Standard_Integer MinDegree = 3;
  Standard_Integer MaxDegree = 8;
  
  return GeomAPI_PointsToBSpline(aCurvePoint, MinDegree,
    MaxDegree, GeomAbs_C2, Precision::Confusion());
}

//================================================================
// Function : createSurface
// Purpose  : returns a BSpline Surface with 8 poles
//================================================================
static Handle(Geom_BSplineSurface) createSurface()
{
  // define points array
  TColgp_Array2OfPnt aPoints (1,2,1,4);

  // initializing array of points
  aPoints(1,1) = gp_Pnt(-4,4.5,-3); aPoints(1,2) = gp_Pnt(-2.2,5,-2);
  aPoints(1,3) = gp_Pnt(1,2,-2);    aPoints(1,4) = gp_Pnt(3.5,4.5,-2);
  aPoints(2,1) = gp_Pnt(-4,-3,-3);  aPoints(2,2) = gp_Pnt(-1.5,-0.5,-4);
  aPoints(2,3) = gp_Pnt(2,-1,-3);   aPoints(2,4) = gp_Pnt(5,-3,-2);

  for (Standard_Integer i=1; i <= aPoints.ColLength(); i++)
    for (Standard_Integer j=1; j <= aPoints.RowLength(); j++)
        aPoints(i,j).ChangeCoord() = aPoints(i,j).Coord() * 100 + gp_XYZ(0,0,300);

  Standard_Integer MinDegree = 3;
  Standard_Integer MaxDegree = 8;
  
  return GeomAPI_PointsToBSplineSurface (
    aPoints, MinDegree, MaxDegree, GeomAbs_C2, Precision::Confusion());
}


//=========================================================================================
// Sample execution
//=========================================================================================

void Projection_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////


//==========================================================================================
// Function : Projection_Presentation::sample1
// Purpose  : 
//==========================================================================================

void Projection_Presentation::sample1()
{
  setResultTitle("Projection of a Point onto a Curve");
  TCollection_AsciiString aText(
    "  // define a BSpline curve" EOL
    "  Handle(Geom_BSplineCurve) aCurve = createCurve();" EOL
    "" EOL
    "  // define initial point for projection" EOL
    "  gp_Pnt aPoint(0,-70,150);" EOL
    "" EOL
    "  // projection of a point onto a curve" EOL
    "  GeomAPI_ProjectPointOnCurve aPPC(aPoint,aCurve);" EOL
    "" EOL
    "  // evaluate solutions" EOL
    "  Standard_Integer aNumSolutions = aPPC.NbPoints();" EOL
    "  TColgp_Array1OfPnt aPoints(1, Max(aNumSolutions, 1));" EOL
    "  Standard_Real aDistance;" EOL
    "  gp_Pnt aNearestPoint;" EOL
    "  Standard_Real aNearestParam;" EOL
    "" EOL
    "  if (aNumSolutions > 0)" EOL
    "  {" EOL
    "    for(Standard_Integer i = 1; i <= aNumSolutions; i++)" EOL
    "      aPoints(i) = aPPC.Point(i);" EOL
    "" EOL
    "    // The nearest solution" EOL
    "    aNearestParam = aPPC.LowerDistanceParameter();" EOL
    "    aNearestPoint = aPPC.NearestPoint();" EOL
    "    aDistance = aPPC.LowerDistance();" EOL
    "  }" EOL EOL);
  setResultText(aText.ToCString());

  // define a BSpline curve
  Handle(Geom_BSplineCurve) aCurve = createCurve();

  // define initial point for projection
  gp_Pnt aPoint(0,-70,150);

  // projection of a point onto a curve
  GeomAPI_ProjectPointOnCurve aPPC(aPoint,aCurve);

  // evaluate solutions
  Standard_Integer aNumSolutions = aPPC.NbPoints();
  TColgp_Array1OfPnt aPoints(1, Max(aNumSolutions, 1));
  Standard_Real aDistance;
  gp_Pnt aNearestPoint;
  Standard_Real aNearestParam;

  if (aNumSolutions > 0)
  {
    for(Standard_Integer i = 1; i <= aNumSolutions; i++)
      aPoints(i) = aPPC.Point(i);

    // The nearest solution
    aNearestParam = aPPC.LowerDistanceParameter();
    aNearestPoint = aPPC.NearestPoint();
    aDistance = aPPC.LowerDistance();

    aText = aText +
      "  //=================================================" EOL EOL

      "  Lower distance parameter = " + TCollection_AsciiString(aNearestParam) + EOL
      "  Nearest distance = " + TCollection_AsciiString(aDistance) + EOL;
    setResultText(aText.ToCString());
  }
  
  // output initial point
  drawPoint(aPoint,Quantity_Color(Quantity_NOC_YELLOW));
  if(WAIT_A_LITTLE) return;

  // output Bspline curve
  drawCurve(aCurve);

  // output solutions
  if (aNumSolutions > 0)
  {
    if(WAIT_A_SECOND) return;

    for(int i = 1; i <= aNumSolutions; i++)
      drawPoint(aPoints(i));

    // output line between nearest points
    Handle(Geom_Line) aLine = new Geom_Line(gce_MakeLin(aPoint, aNearestPoint));
    Handle(Geom_TrimmedCurve) aTrimmedCurve = new Geom_TrimmedCurve(aLine,0,aDistance);
    drawCurve(aTrimmedCurve,Quantity_Color(Quantity_NOC_WHITE));
  }
}

//==========================================================================================
// Function : Projection_Presentation::sample2
// Purpose  : 
//==========================================================================================

void Projection_Presentation::sample2()
{
  setResultTitle("Projection of a Point onto a Surface");
  TCollection_AsciiString aText(
    "  // define initial point" EOL
    "  gp_Pnt aPoint(-380,-370,150);" EOL
    "  " EOL
    "  // define a cylindrical surface" EOL
    "  gp_Pnt aCenterPoint(50,-20,-30);" EOL
    "  gp_Dir aDir(0,0,1);" EOL
    "  gp_Ax3 anAx3(aCenterPoint, aDir);" EOL
    "  Standard_Real Radius = 300;" EOL
    "  Handle(Geom_CylindricalSurface) aCylSurface =" EOL
    "    new Geom_CylindricalSurface(anAx3, Radius);" EOL
    "" EOL
    "  // projection of a point onto a surface" EOL
    "  GeomAPI_ProjectPointOnSurf aPPS(aPoint,aCylSurface);" EOL
    "" EOL
    "  // evaluate solutions" EOL
    "  Standard_Integer aNumSolutions = aPPS.NbPoints();" EOL
    "  TColgp_Array1OfPnt aPoints(1, Max(aNumSolutions, 1));" EOL
    "  Standard_Real aDistance;" EOL
    "  gp_Pnt aNearestPoint;" EOL
    "  Standard_Real aNearestU, aNearestV;" EOL
    "" EOL
    "  if (aNumSolutions > 0)" EOL
    "  {" EOL
    "    for(Standard_Integer i = 1; i <= aNumSolutions; i++)" EOL
    "      aPoints(i) = aPPS.Point(i);" EOL
    "" EOL
    "    // The nearest solution" EOL
    "    aPPS.LowerDistanceParameters(aNearestU, aNearestV);" EOL
    "    aNearestPoint = aPPS.NearestPoint();" EOL
    "    aDistance = aPPS.LowerDistance();" EOL
    "  }" EOL EOL);
  setResultText(aText.ToCString());

  // define initial point
  gp_Pnt aPoint(-380,-370,150);
  
  // define a cylindrical surface
  gp_Pnt aCenterPoint(50,-20,-30);
  gp_Dir aDir(0,0,1);
  gp_Ax3 anAx3(aCenterPoint, aDir);
  Standard_Real Radius = 300;
  Handle(Geom_CylindricalSurface) aCylSurface =
    new Geom_CylindricalSurface(anAx3, Radius);

  // projection of a point onto a surface
  GeomAPI_ProjectPointOnSurf aPPS(aPoint,aCylSurface);

  // evaluate solutions
  Standard_Integer aNumSolutions = aPPS.NbPoints();
  TColgp_Array1OfPnt aPoints(1, Max(aNumSolutions, 1));
  Standard_Real aDistance;
  gp_Pnt aNearestPoint;
  Standard_Real aNearestU, aNearestV;

  if (aNumSolutions > 0)
  {
    for(Standard_Integer i = 1; i <= aNumSolutions; i++)
      aPoints(i) = aPPS.Point(i);

    // The nearest solution
    aPPS.LowerDistanceParameters(aNearestU, aNearestV);
    aNearestPoint = aPPS.NearestPoint();
    aDistance = aPPS.LowerDistance();

    aText = aText +
      "  //=================================================" EOL EOL

      "  Lower distance parameters:  U = " + TCollection_AsciiString(aNearestU) +
      ", V = "+ TCollection_AsciiString(aNearestV) + EOL
      "  Nearest distance = " + TCollection_AsciiString(aDistance) + EOL;
    setResultText(aText.ToCString());
  }

  // output initial point
  drawPoint(aPoint);
  if(WAIT_A_LITTLE) return;

  // output cylindrical surface
  Handle(Geom_RectangularTrimmedSurface) aCylTrimmed = 
    new Geom_RectangularTrimmedSurface(aCylSurface,0,2*PI,-200,300,Standard_True,Standard_True);
  
  drawSurface(aCylTrimmed);

  // output solutions
  if (aNumSolutions > 0)
  {
    if(WAIT_A_SECOND) return;

    for(int i = 1; i <= aNumSolutions; i++)
      drawPoint(aPoints(i));

    // output line between nearest points
    Handle(Geom_Line) aLine = new Geom_Line(gce_MakeLin(aPoint, aNearestPoint));
    Handle(Geom_TrimmedCurve) aTrimmedCurve = new Geom_TrimmedCurve(aLine,0,aDistance);
    drawCurve(aTrimmedCurve,Quantity_Color(Quantity_NOC_WHITE));
  }
}

//==========================================================================================
// Function : Projection_Presentation::sample3
// Purpose  : 
//==========================================================================================

void Projection_Presentation::sample3()
{
  setResultTitle("Projection of a Curve onto a Surface");
  setResultText(
    "  // create BSpline curve" EOL
    "  Handle(Geom_BSplineCurve) aCurve = createCurve();" EOL
    "" EOL
    "  // define initial point" EOL
    "  gp_Pnt aPoint(0,0,0);" EOL
    "  " EOL
    "  // define a planar surface" EOL
    "  gp_Pnt aPlanePnt(0,0,-100);" EOL
    "  gp_Dir aPlaneDir(0,0,1);" EOL
    "  Handle(Geom_Plane) aPlane = new Geom_Plane(aPlanePnt, aPlaneDir);" EOL
    "" EOL
    "  // create a cylindrical surface" EOL
    "  gp_Pnt aCylPnt(-100,0,550);" EOL
    "  gp_Dir aCylDir(0,1,0);" EOL
    "  gp_Ax3 anAx3(aCylPnt, aCylDir);" EOL
    "  Standard_Real aRadius = 800;" EOL
    "  Handle(Geom_CylindricalSurface) aCylSurface = " EOL
    "    new Geom_CylindricalSurface(anAx3, aRadius);" EOL
    "" EOL
    "  // Projection of a curve onto a planar surface" EOL
    "  Handle(Geom_Curve) aPlaneProjCurve = GeomProjLib::Project(aCurve, aPlane);" EOL
    "" EOL
    "  // Projection of a curve onto a cylindrical surface" EOL
    "  Handle(Geom_Curve) aCylProjCurve = GeomProjLib::Project(aCurve, aCylSurface);" EOL
  );
  
  // create BSpline curve
  Handle(Geom_BSplineCurve) aCurve = createCurve();

  // define initial point
  gp_Pnt aPoint(0,0,0);
  
  // define a planar surface
  gp_Pnt aPlanePnt(0,0,-100);
  gp_Dir aPlaneDir(0,0,1);
  Handle(Geom_Plane) aPlane = new Geom_Plane(aPlanePnt, aPlaneDir);

  // create a cylindrical surface
  gp_Pnt aCylPnt(-100,0,550);
  gp_Dir aCylDir(0,1,0);
  gp_Ax3 anAx3(aCylPnt, aCylDir);
  Standard_Real aRadius = 800;
  Handle(Geom_CylindricalSurface) aCylSurface = 
    new Geom_CylindricalSurface(anAx3, aRadius);

  // Projection of a curve onto a planar surface
  Handle(Geom_Curve) aPlaneProjCurve = GeomProjLib::Project(aCurve, aPlane);

  // Projection of a curve onto a cylindrical surface
  Handle(Geom_Curve) aCylProjCurve = GeomProjLib::Project(aCurve, aCylSurface);

  // ===== output ===========
  // output original curve
  drawCurve(aCurve);
  if(WAIT_A_LITTLE) return;

  // output the first surface - plane
  Handle(AIS_InteractiveObject) aPlaneIO = drawSurface(
    aPlane, Quantity_Color(Quantity_NOC_YELLOW), Standard_False);
  getAISContext()->SetTransparency(aPlaneIO, 0.4, Standard_False);
  getAISContext()->Display(aPlaneIO);

  if(WAIT_A_SECOND) return; // wait longer.  "Projecting... Please wait..."

  // output projected curve  
  Handle(AIS_InteractiveObject) aProjCurveIO = drawCurve(aPlaneProjCurve, Quantity_Color(Quantity_NOC_GREEN));
  if(WAIT_A_SECOND) return;

  // Erasing the first surface and projected curve and 
  // showing the second case of projection
  getAISContext()->Erase(aPlaneIO, Standard_False);
  getAISContext()->Erase(aProjCurveIO, Standard_False);

  // output the second surface - cylindrical surface
  Handle(Geom_RectangularTrimmedSurface) aCylTrimmed = 
   new Geom_RectangularTrimmedSurface(aCylSurface,PI/2,-PI/2,-450,550,Standard_True,Standard_True);
  Handle(AIS_InteractiveObject) aCylIO = drawSurface(
    aCylTrimmed, Quantity_Color(Quantity_NOC_YELLOW), Standard_False);
  getAISContext()->SetTransparency(aCylIO, 0.4, Standard_False);
  getAISContext()->Display(aCylIO);

  if(WAIT_A_SECOND) return; // wait longer.  "Projecting... Please wait..."

  // output projection curve
  drawCurve(aCylProjCurve,Quantity_Color(Quantity_NOC_GREEN));
}

//==========================================================================================
// Function : Projection_Presentation::sample4
// Purpose  : 
//==========================================================================================
void Projection_Presentation::sample4()
{
  setResultTitle("Projection of Curve onto a BSpline Surface");
  setResultText(
    "  // create a BSpline curve" EOL
    "  Handle(Geom_BSplineCurve) aCurve = createCurve();" EOL
    "" EOL
    "  // create a BSpline surface" EOL
    "  Handle(Geom_BSplineSurface) aSurface = createSurface();" EOL
    "" EOL
    "  // projecting a curve onto a bspline surface" EOL
    "  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface(aSurface);" EOL
    "  Handle(GeomAdaptor_HCurve)   HC = new GeomAdaptor_HCurve(aCurve);" EOL
    "  Standard_Real Tol  = 0.0001;" EOL
    "  Standard_Real TolU = 0.00001;" EOL
    "  Standard_Real TolV = 0.00001;" EOL
    "  ProjLib_CompProjectedCurve Proj(HS,HC,TolU,TolV);" EOL
    "  " EOL
    "  // sequence of projected curves" EOL
    "  TColGeom_SequenceOfCurve aSolutionCurves;" EOL
    "  " EOL
    "  Standard_Integer ProjMaxDegree = 8;" EOL
    "  Standard_Integer ProjMaxSegments = 30;" EOL
    "  Standard_Real f,l;" EOL
    "  Standard_Integer aNumSolutions = Proj.NbCurves();" EOL
    "  for (Standard_Integer i = 1; i <= aNumSolutions; i++)" EOL
    "  {" EOL
    "    Proj.Bounds(i,f,l);" EOL
    "    Handle(Adaptor2d_HCurve2d) HC2d = Proj.Trim(f,l,TolU);" EOL
    "    Standard_Boolean aOnly3d = Standard_True;" EOL
    "    Approx_CurveOnSurface Approx(" EOL
    "      HC2d, HS, f, l, Tol,GeomAbs_C2,ProjMaxDegree,ProjMaxSegments,aOnly3d);" EOL
    "    " EOL
    "    if (Approx.IsDone() && Approx.HasResult())" EOL
    "      aSolutionCurves.Append(Approx.Curve3d());" EOL
    "  }" EOL
  );
  
  // create a BSpline curve
  Handle(Geom_BSplineCurve) aCurve = createCurve();

  // create a BSpline surface
  Handle(Geom_BSplineSurface) aSurface = createSurface();

  // projecting a curve onto a bspline surface
  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface(aSurface);
  Handle(GeomAdaptor_HCurve)   HC = new GeomAdaptor_HCurve(aCurve);
  Standard_Real Tol  = 0.0001;
  Standard_Real TolU = 0.00001;
  Standard_Real TolV = 0.00001;
  ProjLib_CompProjectedCurve Proj(HS,HC,TolU,TolV);

  // sequence of projected curves
  TColGeom_SequenceOfCurve aSolutionCurves;

  Standard_Integer ProjMaxDegree = 8;
  Standard_Integer ProjMaxSegments = 30;
  Standard_Real f,l;
  Standard_Integer aNumSolutions = Proj.NbCurves();
  for (Standard_Integer i = 1; i <= aNumSolutions; i++)
  {
    Proj.Bounds(i,f,l);
    Handle(Adaptor2d_HCurve2d) HC2d = Proj.Trim(f,l,TolU);
    Standard_Boolean aOnly3d = Standard_True;
    Approx_CurveOnSurface Approx(
      HC2d, HS, f, l, Tol,GeomAbs_C2,ProjMaxDegree,ProjMaxSegments,aOnly3d);
    
    if (Approx.IsDone() && Approx.HasResult())
      aSolutionCurves.Append(Approx.Curve3d());
  }

  // output bspline curve
  drawCurve(aCurve);
  if(WAIT_A_LITTLE) return;

  // output bspline surface
  Handle(AIS_InteractiveObject) aCylIO = drawSurface(
    aSurface, Quantity_Color(Quantity_NOC_YELLOW), Standard_False);
  getAISContext()->SetTransparency(aCylIO, 0.4, Standard_False);
  getAISContext()->Display(aCylIO);

  if(WAIT_A_SECOND) return; // wait longer.  "Projecting... Please wait..."

  // output projected curves
  for (i = 1; i <= aNumSolutions; i++)
    drawCurve(aSolutionCurves(i), Quantity_Color(Quantity_NOC_GREEN));
}

