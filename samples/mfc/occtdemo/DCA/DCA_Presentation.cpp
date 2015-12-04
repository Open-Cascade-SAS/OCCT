// DCA_Presentation.cpp: implementation of the DCA_Presentation class.
// Geometry Direct Construction Algorithms
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DCA_Presentation.h"

#include <gce_MakeCirc.hxx>
#include <gce_MakeElips.hxx>
#include <gce_MakeParab.hxx>
#include <gce_MakePln.hxx>
#include <gce_MakeHypr.hxx>
#include <gce_MakeLin.hxx>
#include <gce_MakeCylinder.hxx>
#include <gce_MakeCone.hxx>

#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Torus.hxx>
#include <gp_Sphere.hxx>

#include <GC_MakeTrimmedCylinder.hxx>
#include <GC_MakeCylindricalSurface.hxx>
#include <GC_MakeTrimmedCone.hxx>
#include <OCCDemo_Presentation.h>
#include <AIS_Point.hxx>
#include <AIS_Shape.hxx>
#include <Precision.hxx>

#include <Geom_Line.hxx>
#include <Geom_Vector.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <TCollection_AsciiString.hxx>

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new DCA_Presentation;

// Initialization of array of samples
const DCA_Presentation::PSampleFuncType DCA_Presentation::SampleFuncs[] =
{
  &DCA_Presentation::sampleLine,
  &DCA_Presentation::sampleParabola,
  &DCA_Presentation::sampleHyperbola,
  &DCA_Presentation::sampleCircle,
  &DCA_Presentation::sampleEllipse,
  &DCA_Presentation::samplePlane,
  &DCA_Presentation::sampleCylindricalSurface,
  &DCA_Presentation::sampleSphericalSurface,
  &DCA_Presentation::sampleToroidalSurface,
  &DCA_Presentation::sampleConicalSurface,
  };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DCA_Presentation::DCA_Presentation()
{
  myIndex = 0;
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Direct Construction Algorithms");
  FitMode=false;
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void DCA_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

// global data used by various samples
static gp_Pnt aCenterPoint(120, 0, 0);

//================================================================
// Function : DCA_Presentation::sampleLine
// Purpose  : 
//================================================================
void DCA_Presentation::sampleLine()
{
  Standard_CString aTitle = "Construction of lines";

  TCollection_AsciiString aText(
    "  // Define two points" EOL
    "  gp_Pnt aPoint(-350, 0, 0);" EOL
    "  gp_Pnt aPoint1(600, 0, 0);" EOL EOL

    "  // Create a line between two points" EOL
    "  gp_Lin aLin = gce_MakeLin(aPoint, aPoint1);" EOL
    );
  Comment(aTitle,aText);
  
  // Define two points
  gp_Pnt aPoint(-350, 0, 0);
  gp_Pnt aPoint1(600, 0, 0);

  // Create a line between two points  
  gp_Lin aLin = gce_MakeLin(aPoint,aPoint1);

  drawPoint(aPoint1);
  drawPoint(aPoint);

  if(WAIT_A_LITTLE) return;

  Handle(Geom_Line) aLine = new Geom_Line(aLin);
  Handle(Geom_TrimmedCurve) aCurve = 
    new Geom_TrimmedCurve(aLine, -50, aPoint.Distance(aPoint1)+50);
  drawCurve(aCurve);
}

//================================================================
// Function : DCA_Presentation::sampleParabola
// Purpose  : 
//================================================================
void DCA_Presentation::sampleParabola()
{ 
  Standard_CString aTitle = " Construction of parabols";
  getAISContext()->EraseAll();
  TCollection_AsciiString aText(
    "  //define direction vectors" EOL
    "  gp_Dir aDir(1,0,0);" EOL
    "  gp_Dir aDir1(0,1,0);" EOL
    "" EOL
    "  //define two points" EOL
    "  gp_Pnt aPointParab(80 ,0 ,-200);" EOL
    "  gp_Pnt aFocalPointParab(80 ,0 ,-100);" EOL
    "" EOL
    "  //define coordinate system and an axis" EOL
    "  gp_Ax2 anAx2(aPointParab, aDir);" EOL
    "  gp_Ax1 anAx1(aPointParab, aDir1); " EOL
    "" EOL
    "  //METHOD1.(coordinate system, focal_length)" EOL
    "  Standard_Real aFocalLength = 50;" EOL
    "  gp_Parab aParab = gce_MakeParab(anAx2,aFocalLength);" EOL
    "  " EOL
    "  //METHOD2.(axis,point)" EOL
    "  gp_Parab aParab1 = gce_MakeParab(anAx1,aFocalPointParab);" EOL
    );
  
  Comment(aTitle,aText);
  
  //define direction vectors
  gp_Dir aDir(1,0,0);
  gp_Dir aDir1(0,1,0);

  //define two points
  gp_Pnt aPointParab(80 ,0 ,-200);
  gp_Pnt aFocalPointParab(80 ,0 ,-100);

  //define coordinate system and an axis
  gp_Ax2 anAx2(aPointParab, aDir);
  gp_Ax1 anAx1(aPointParab, aDir1); 

  //METHOD1.(coordinate system, focal_length)
  Standard_Real aFocalLength = 50;
  gp_Parab aParab = gce_MakeParab(anAx2,aFocalLength);
  
  //METHOD2.(axis,point)
  gp_Parab aParab1 = gce_MakeParab(anAx1,aFocalPointParab);

  //output first parabola
  drawPoint(aPointParab);
  drawVector(aPointParab,gp_Vec(aDir)*150,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  Handle(Geom_Line) aFocalLenLin = new Geom_Line(gce_MakeLin(aPointParab, gp_Dir(0,0,1)));
  Handle(Geom_TrimmedCurve) aTrim = new Geom_TrimmedCurve(aFocalLenLin, -aFocalLength/2, aFocalLength/2);
  drawCurve(aTrim);

  if(WAIT_A_LITTLE) return;
  
  Handle(Geom_Parabola) aParabola = new Geom_Parabola(aParab);
  Handle(Geom_TrimmedCurve) aParabTrimmed = 
    new Geom_TrimmedCurve(aParabola,-300,300,Standard_True);
  drawCurve(aParabTrimmed);

  if(WAIT_A_LITTLE) return;

  getAISContext()->EraseAll();

  //output second parabola
  drawPoint(aPointParab);
  drawVector(aPointParab,gp_Vec(aDir1)*150,Quantity_Color(Quantity_NOC_WHITE));
  drawPoint(aFocalPointParab);

  if(WAIT_A_LITTLE) return;

  Handle(Geom_Parabola) aParabola1 = new Geom_Parabola(aParab1);
  Handle(Geom_TrimmedCurve) aParabTrimmed1 = new Geom_TrimmedCurve(aParabola1,-300,300,Standard_True);
  drawCurve(aParabTrimmed1);
}
//================================================================
// Function : DCA_Presentation::sampleHyperbola
// Purpose  : 
//================================================================
void DCA_Presentation::sampleHyperbola()
{
  Standard_CString aTitle = " Construction of hyperbols ";
  TCollection_AsciiString aText(
    "  //define direction vectors" EOL
    "  gp_Dir aDir(1,0,0);" EOL
    "" EOL
    "  //define four points" EOL
    "  gp_Pnt aPointHypr(100 ,0 ,-200);" EOL
    "  gp_Pnt aPointHypr1(100 ,0 ,-300);" EOL
    "  gp_Pnt aPointHypr2(100 ,0 ,-100);" EOL
    "  gp_Pnt aPointHypr3(100 ,100 ,-300);" EOL
    "" EOL
    "  //define axis" EOL
    "  gp_Ax2 anAxHypr(aPointHypr, aDir);" EOL
    "" EOL
    "  //define radii" EOL
    "  Standard_Real MajorRadius=100;" EOL
    "  Standard_Real MinorRadius=80;" EOL
    "  " EOL
    "  //METHOD1.(axis,major and minor radiuses)" EOL
    "  gp_Hypr aHypr1 = gce_MakeHypr(anAxHypr, MajorRadius, MinorRadius);" EOL
    "" EOL
    "  //METHOD2.(three points)" EOL
    "  gp_Hypr aHypr2 = gce_MakeHypr(aPointHypr2, aPointHypr3, aPointHypr1);" EOL
    );
  Comment(aTitle,aText);
  
  //define direction vectors
  gp_Dir aDir(1,0,0);

  //define four points
  gp_Pnt aPointHypr(100 ,0 ,-200);
  gp_Pnt aPointHypr1(100 ,0 ,-300);
  gp_Pnt aPointHypr2(100 ,0 ,-100);
  gp_Pnt aPointHypr3(100 ,100 ,-300);

  //define axis
  gp_Ax2 anAxHypr(aPointHypr, aDir);

  //define radii
  Standard_Real MajorRadius=100;
  Standard_Real MinorRadius=80;
  
  //METHOD1.(axis,major and minor radiuses)
  gp_Hypr aHypr1 = gce_MakeHypr(anAxHypr, MajorRadius, MinorRadius);

  //METHOD2.(three points)
  gp_Hypr aHypr2 = gce_MakeHypr(aPointHypr2, aPointHypr3, aPointHypr1);


  //output first hyperbola
  drawPoint(aPointHypr);
  drawVector(aPointHypr,gp_Vec(aDir)*200,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  Handle(Geom_Hyperbola) aHyperbola1 = new Geom_Hyperbola(aHypr1);
  Handle(Geom_TrimmedCurve) aHyprTrimmed =
    new Geom_TrimmedCurve(aHyperbola1,-3,4,Standard_True);
  drawCurve(aHyprTrimmed);

  if(WAIT_A_LITTLE) return;

  getAISContext()->EraseAll();

  //output seconf hyperbola
  //output points and vector
  drawPoint(aPointHypr1);
  drawPoint(aPointHypr2);
  drawPoint(aPointHypr3);

  if(WAIT_A_LITTLE) return;

  drawVector(aPointHypr1,gp_Vec(aPointHypr1,aPointHypr2)*3,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output of display MajorRadius (yellow color)
  Handle(Geom_Line) aLine = new Geom_Line(gce_MakeLin(aPointHypr1,aPointHypr2));
  Handle(Geom_TrimmedCurve) aTrimmed1 = 
    new Geom_TrimmedCurve(aLine, 0, aPointHypr1.Distance(aPointHypr2));
  drawCurve(aTrimmed1,Quantity_Color(Quantity_NOC_YELLOW));

  if(WAIT_A_LITTLE) return;

  //output of display MinorRadius (yellow color)
  Handle(Geom_Line) aLine1 = new Geom_Line(gce_MakeLin(aPointHypr3, gp_Dir(0,-1,0)));
  Handle(Geom_TrimmedCurve) aTrimmed2 = new Geom_TrimmedCurve(aLine1, 0,100);
  drawCurve(aTrimmed2,Quantity_Color(Quantity_NOC_YELLOW)); 

  if(WAIT_A_LITTLE) return;

  //output hyperbola
  Handle(Geom_Hyperbola) aHyperbola2 = new Geom_Hyperbola(aHypr2);
  Handle(Geom_TrimmedCurve) aHyprTrimmed1 = 
    new Geom_TrimmedCurve(aHyperbola2,-2,2,Standard_True);
  drawCurve(aHyprTrimmed1);
}
//================================================================
// Function : DCA_Presentation::sampleCircle
// Purpose  : 
//================================================================
void DCA_Presentation::sampleCircle()
{
  Standard_CString aTitle = " Construction of Circle";
  TCollection_AsciiString aText(
    "  //define direction vectors" EOL
    "  gp_Dir aDir(1,0,0);" EOL
    "" EOL
    "  //define axis" EOL
    "  gp_Ax2 anAx2(aCenterPoint, aDir);" EOL
    "" EOL
    "  //define points" EOL
    "  gp_Pnt aCirclePoint1(0,420,0);" EOL
    "  gp_Pnt aCirclePoint2(0,120,300);" EOL
    "  gp_Pnt aCirclePoint3(0,-200,0);" EOL
    "  gp_Pnt aCirclePoint4(120,0,100);" EOL
    "" EOL
    "  //define base circle" EOL
    "  //define radius" EOL
    "  Standard_Real radius = 300;" EOL
    "  gp_Circ aCircle(anAx2, radius);" EOL
    " " EOL
    "  // METHOD 1.(point,vector,radius)" EOL
    "  gp_Circ aCirc = gce_MakeCirc (aCenterPoint, aDir, radius);" EOL
    "" EOL
    "  // METHOD 2.(three points)" EOL
    "  gp_Circ aCirc1 = gce_MakeCirc(aCirclePoint1, aCirclePoint2, aCirclePoint3);" EOL
    "" EOL
    "  // METHOD 3.(circle,point)" EOL
    "  gp_Circ aCirc2 = gce_MakeCirc(aCircle, aCirclePoint4);" EOL
    );  
  Comment(aTitle,aText);
 
  //define direction vectors
  gp_Dir aDir(1,0,0);

  //define axis
  gp_Ax2 anAx2(aCenterPoint, aDir);

  //define points
  gp_Pnt aCirclePoint1(0,420,0);
  gp_Pnt aCirclePoint2(0,120,300);
  gp_Pnt aCirclePoint3(0,-200,0);
  gp_Pnt aCirclePoint4(120,0,100);

  //define base circle
  //define radius
  Standard_Real radius = 300;
  gp_Circ aCircle(anAx2, radius);
 
  // METHOD 1.(point,vector,radius)
  gp_Circ aCirc = gce_MakeCirc (aCenterPoint, aDir, radius);

  // METHOD 2.(three points)
  gp_Circ aCirc1 = gce_MakeCirc(aCirclePoint1, aCirclePoint2, aCirclePoint3);

  // METHOD 3.(circle,point)
  gp_Circ aCirc2 = gce_MakeCirc(aCircle, aCirclePoint4);


  // METHOD 1.(output of display)

  //output point and vector
  drawPoint(aCenterPoint);
  drawVector(aCenterPoint,gp_Vec(aDir)*400,Quantity_Color(Quantity_NOC_WHITE));
  if(WAIT_A_LITTLE) return;

  //output radius
  Handle(Geom_Line) aLine = new Geom_Line(gce_MakeLin (aCenterPoint,gp_Dir(0,0,1)));
  Handle(Geom_TrimmedCurve) aTrimmed = new Geom_TrimmedCurve(aLine,0,300);
  drawCurve(aTrimmed,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output circle
  drawCurve(new Geom_Circle(aCirc));

  if(WAIT_A_LITTLE)  return;

  getAISContext()->EraseAll();

  // METHOD 2.(output of display)

  //output points
  drawPoint(aCirclePoint1);
  drawPoint(aCirclePoint2);
  drawPoint(aCirclePoint3);

  if(WAIT_A_LITTLE)  return;

  //output circle  
  drawCurve(new Geom_Circle(aCirc1),Quantity_Color(Quantity_NOC_RED));

  if(WAIT_A_LITTLE)  return;

  getAISContext()->EraseAll();

  // METHOD 3.(output of display)

  //output point and base circle
  drawPoint(aCirclePoint4);
  drawCurve(new Geom_Circle(aCircle),Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE)  return;

  //output circle
  drawCurve(new Geom_Circle(aCirc2),Quantity_Color(Quantity_NOC_RED));
}
//================================================================
// Function : DCA_Presentation::sampleEllipse
// Purpose  : 
//================================================================
void DCA_Presentation::sampleEllipse()
{
  Standard_CString aTitle = " Construction of Ellipse";
  TCollection_AsciiString aText(
    "  //define direction vectors" EOL
    "  gp_Dir aDir(1,0,0);" EOL
    "" EOL
    "  //define axis" EOL
    "  gp_Ax2 anAx2(aCenterPoint, aDir);" EOL
    "" EOL
    "  //define points" EOL
    "  gp_Pnt aEllipsPoint1(120,0,300);" EOL
    "  gp_Pnt aEllipsPoint2(120,120,0);" EOL
    "" EOL
    "  //define radiuses" EOL
    "  Standard_Real MajorRadius = 300;" EOL
    "  Standard_Real MinorRadius = 120;" EOL
    "" EOL
    "  //METHOD 1.(axis,two radiuses)" EOL
    "  gp_Elips aElips = gce_MakeElips(anAx2, MajorRadius, MinorRadius);" EOL
    "" EOL
    "  //METHOD 2 (three points)  " EOL
    "  gp_Elips aElips1 = gce_MakeElips(aEllipsPoint1, aEllipsPoint2, aCenterPoint);" EOL
    );
  Comment(aTitle,aText);
  
  //define direction vectors
  gp_Dir aDir(1,0,0);

  //define axis
  gp_Ax2 anAx2(aCenterPoint, aDir);

  //define points
  gp_Pnt aEllipsPoint1(120,0,300);
  gp_Pnt aEllipsPoint2(120,120,0);

  //define radiuses
  Standard_Real MajorRadius = 300;
  Standard_Real MinorRadius = 120;

  //METHOD 1.(axis,two radiuses)
  gp_Elips aElips = gce_MakeElips(anAx2, MajorRadius, MinorRadius);

  //METHOD 2 (three points)  
  gp_Elips aElips1 = gce_MakeElips(aEllipsPoint1, aEllipsPoint2, aCenterPoint);


  //METHOD 1 .(output of display)
  //output point and vector 
  drawPoint(aCenterPoint);
  drawVector(aCenterPoint,gp_Vec(aDir)*400,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output first ellipse
  drawCurve(new Geom_Ellipse(aElips));

  if(WAIT_A_LITTLE) return;

  getAISContext()->EraseAll();

  //METHOD 2 .(output of display)
  drawPoint(aCenterPoint);
  drawPoint(aEllipsPoint1);
  drawPoint(aEllipsPoint2);

  if(WAIT_A_LITTLE)  return;
  
  //define MajorRadius 
  gp_Dir aDir1(0,1,0);
  gp_Dir aDir2(0,0,1);
  gce_MakeLin aMakeLin(aCenterPoint,aDir1);
  gp_Lin aLine = aMakeLin.Value();

  //define MinorRadius
  gce_MakeLin aMakeLin1(aCenterPoint,aDir2);
  gp_Lin aLine1 = aMakeLin1.Value();

  //output MajorRadius
  Handle(Geom_Line) aLineMajorRadius = new Geom_Line(aLine);
  Handle(Geom_TrimmedCurve) aTrimmed = 
    new Geom_TrimmedCurve(aLineMajorRadius,0,120);
  drawCurve(aTrimmed,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output MinorRadius
  Handle(Geom_Line) aLineMinorrRadius = new Geom_Line(aLine1);
  Handle(Geom_TrimmedCurve) aTrimmed1 = 
    new Geom_TrimmedCurve(aLineMinorrRadius,0,300);
  drawCurve(aTrimmed1,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output second ellipse
  drawCurve(new Geom_Ellipse(aElips1),Quantity_Color(Quantity_NOC_RED));
}
//================================================================
// Function : DCA_Presentation::samplePlane
// Purpose  : 
//================================================================
void DCA_Presentation::samplePlane()
{
  Standard_CString aTitle = "Construction of Plane  ";
  TCollection_AsciiString aText(
    "  //define direction vector" EOL
    "  gp_Dir aDir(1,0,0);" EOL
    "" EOL
    "  //define axis" EOL
    "  gp_Ax2 anAx2(aCenterPoint,aDir);" EOL
    "" EOL
    "  //define three points" EOL
    "  gp_Pnt aPointPlane1(0,0,0);" EOL
    "  gp_Pnt aPointPlane2(0,-120,0);" EOL
    "  gp_Pnt aPointPlane3(0,50 , -100);" EOL
    "" EOL
    "  //METHOD 1.(axis)" EOL
    "  gp_Pln aPlane = gce_MakePln(anAx2);" EOL
    "  " EOL
    "  //METHOD 2 (three points)  " EOL
    "  gp_Pln aPlane1 = gce_MakePln(aPointPlane1,aPointPlane2,aPointPlane3);" EOL
    );
  Comment(aTitle,aText);
  
  //define direction vector
  gp_Dir aDir(1,0,0);

  //define axis
  gp_Ax2 anAx2(aCenterPoint,aDir);

  //define three points
  gp_Pnt aPointPlane1(0,0,0);
  gp_Pnt aPointPlane2(0,-120,0);
  gp_Pnt aPointPlane3(0,50 , -100);

  //METHOD 1.(axis)
  gp_Pln aPlane = gce_MakePln(anAx2);
  
  //METHOD 2 (three points)  
  gp_Pln aPlane1 = gce_MakePln(aPointPlane1,aPointPlane2,aPointPlane3);
  
  
  //METHOD 1. (output of display)
  drawPoint(aCenterPoint);
  drawVector(aCenterPoint,gp_Vec(aDir)*400,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;
    
  Handle(Geom_Plane) aPln = new Geom_Plane(aPlane);
  Handle(Geom_RectangularTrimmedSurface) aPlnTrimmed = 
    new Geom_RectangularTrimmedSurface(aPln,-300,300,-300,300,Standard_True,Standard_True);
  drawSurface(aPlnTrimmed);

  if(WAIT_A_LITTLE) return;

  getAISContext()->EraseAll();

  //METHOD 2.(output of display)
  drawPoint(aPointPlane1);
  drawPoint(aPointPlane2);
  drawPoint(aPointPlane3);

  if(WAIT_A_LITTLE) return;

  Handle(Geom_Plane) aPln1 = new Geom_Plane(aPlane1);
  Handle(Geom_RectangularTrimmedSurface) aPlnTrimmed1 = 
    new Geom_RectangularTrimmedSurface(aPln1,-200,300,-200,300,Standard_True,Standard_True);
  drawSurface(aPlnTrimmed1);

}
//================================================================
// Function : OCCDemo_Presentation::sampleCylindricalSurface
// Purpose  : 
//================================================================
void DCA_Presentation::sampleCylindricalSurface()
{
  Standard_CString aTitle = " Construction of CylindricalSurface ";
  TCollection_AsciiString aText(
    "  //define direction vectors" EOL
    "  gp_Dir aDir(1,0,0);" EOL
    "  " EOL
    "  //define points " EOL
    "  gp_Pnt aCylinderPoint1(0,20,-20);" EOL
    "  gp_Pnt aCylinderPoint2(0,20,-100);" EOL
    "  gp_Pnt aCylinderPoint3(120,20,-100);" EOL
    "  gp_Pnt aCylinderPoint4(120,20,100);" EOL
    "" EOL
    "  //define axises" EOL
    "  gp_Ax3 anAx3(aCenterPoint,aDir);" EOL
    "" EOL
    "  //define base cylinder" EOL
    "  Standard_Real rad = 10;" EOL
    "  gp_Cylinder aCylinder(anAx3,rad);" EOL
    "" EOL
    "  //define base circle" EOL
    "  Standard_Real Radius = 100;" EOL
    "  gp_Circ aCirc(anAx3.Ax2(),Radius);" EOL
    "" EOL
    "  //METHOD 1.(axis,radius) " EOL
    "  gp_Cylinder aCyl = gce_MakeCylinder (anAx3.Ax2(),Radius);" EOL
    "" EOL
    "  //METHOD 2.(three points) " EOL
    "  gp_Cylinder aCyl1 = gce_MakeCylinder(aCylinderPoint2,aCylinderPoint3,aCylinderPoint1);" EOL
    "  " EOL
    "  //METHOD 3.(point,base cylinder)" EOL
    "  gp_Cylinder aCyl2 = gce_MakeCylinder(aCylinder,aCylinderPoint4);" EOL
    "" EOL
    "  //METHOD 4.(base circle)" EOL
    "  gp_Cylinder aCyl3 = gce_MakeCylinder(aCirc);" EOL
    );
  Comment(aTitle,aText);
  
  //define direction vectors
  gp_Dir aDir(1,0,0);
  
  //define points 
  gp_Pnt aCylinderPoint1(0,20,-20);
  gp_Pnt aCylinderPoint2(0,20,-100);
  gp_Pnt aCylinderPoint3(120,20,-100);
  gp_Pnt aCylinderPoint4(120,20,100);

  //define axises
  gp_Ax3 anAx3(aCenterPoint,aDir);

  //define base cylinder
  Standard_Real rad = 10;
  gp_Cylinder aCylinder(anAx3,rad);

  //define base circle
  Standard_Real Radius = 100;
  gp_Circ aCirc(anAx3.Ax2(),Radius);

  //METHOD 1.(axis,radius) 
  gp_Cylinder aCyl = gce_MakeCylinder (anAx3.Ax2(),Radius);

  //METHOD 2.(three points) 
  gp_Cylinder aCyl1 = gce_MakeCylinder(aCylinderPoint2,aCylinderPoint3,aCylinderPoint1);
  
  //METHOD 3.(point,base cylinder)
  gp_Cylinder aCyl2 = gce_MakeCylinder(aCylinder,aCylinderPoint4);

  //METHOD 4.(base circle)
  gp_Cylinder aCyl3 = gce_MakeCylinder(aCirc);


  //METHOD 1.(output of dispay)
  drawPoint(aCenterPoint);
  drawVector(aCenterPoint,gp_Vec(aDir)*400,Quantity_Color(Quantity_NOC_WHITE));
  if(WAIT_A_LITTLE) return;
  //output radius
  gp_Dir aDir1(0,0,1);
  gce_MakeLin aMakeLin(aCenterPoint,aDir1);
  gp_Lin aLin = aMakeLin.Value();
  Handle(Geom_Line) aLine = new Geom_Line(aLin);
  Handle(Geom_TrimmedCurve) aTrimmed = new Geom_TrimmedCurve(aLine,-100,0);
  drawCurve(aTrimmed,Quantity_Color(Quantity_NOC_WHITE));
  if(WAIT_A_LITTLE) return;
  // output cylinder
  Handle(Geom_CylindricalSurface) aCylSurface =
    new Geom_CylindricalSurface(aCyl);
  Handle(Geom_RectangularTrimmedSurface) aCylTrimmed = 
    new Geom_RectangularTrimmedSurface(aCylSurface,0,2*PI,-200,300,Standard_True,Standard_True);
  drawSurface(aCylTrimmed);
  if(WAIT_A_LITTLE) return;
  getAISContext()->EraseAll();

  //METHOD 2.(output of dispay)
  //output points
  drawPoint(aCylinderPoint1);
  drawPoint(aCylinderPoint2);
  drawPoint(aCylinderPoint3);
  if(WAIT_A_LITTLE) return;
  //output vector
  drawVector(aCylinderPoint2,gp_Vec(aCylinderPoint2,aCylinderPoint3)*3,Quantity_Color(Quantity_NOC_WHITE));
   if(WAIT_A_LITTLE) return;
  //output radius
  Handle(Geom_Line) aLine1 = new Geom_Line(gce_MakeLin(aCylinderPoint1,aDir1));
  Handle(Geom_TrimmedCurve) aTrimmed1 = new Geom_TrimmedCurve(aLine1,-80,0);
  drawCurve(aTrimmed1,Quantity_Color(Quantity_NOC_WHITE));
  if(WAIT_A_LITTLE) return;
  //output cylinder
  Handle(Geom_CylindricalSurface) aCylSurface1 = 
    new Geom_CylindricalSurface(aCyl1);
  Handle(Geom_RectangularTrimmedSurface) aCylTrimmed1 = 
    new Geom_RectangularTrimmedSurface(aCylSurface1,0,2*PI,-300,100,Standard_True,Standard_True);
  drawSurface(aCylTrimmed1);
  if(WAIT_A_LITTLE) return;
  getAISContext()->EraseAll();

  //METHOD 3.(output of dispay)
  //output base cylinder  
  Handle(Geom_CylindricalSurface) aCylSurf = new Geom_CylindricalSurface(aCylinder);
  Handle(Geom_RectangularTrimmedSurface) aCylTrimmed2 = 
    new Geom_RectangularTrimmedSurface(aCylSurf,0,2*PI,-200,200,Standard_True,Standard_True);
  drawSurface(aCylTrimmed2 ,Quantity_Color(Quantity_NOC_WHITE));
  //output point
  drawPoint(aCylinderPoint4);
  if(WAIT_A_LITTLE) return;
  //output cylinder
  Handle(Geom_CylindricalSurface) aCylSurface2 = 
    new Geom_CylindricalSurface(aCyl2);
  Handle(Geom_RectangularTrimmedSurface) aCylTrimmed3 =
    new Geom_RectangularTrimmedSurface(aCylSurface2,0,2*PI,-200,200,Standard_True,Standard_True);
  drawSurface(aCylTrimmed3);
  if(WAIT_A_LITTLE) return;
  getAISContext()->EraseAll();
  //METHOD 4.(output of dispay)
  drawPoint(aCenterPoint);
  drawVector(aCenterPoint,gp_Vec(aDir)*400,Quantity_Color(Quantity_NOC_WHITE));
  //output base circle
  drawCurve(new Geom_Circle(aCirc),Quantity_Color(Quantity_NOC_WHITE));
  if(WAIT_A_LITTLE)  return;
  //output cylinder
  Handle(Geom_CylindricalSurface) aCylSurface3 = 
    new Geom_CylindricalSurface(aCyl3);
  Handle(Geom_RectangularTrimmedSurface) aCylTrimmed4 =
    new Geom_RectangularTrimmedSurface(aCylSurface3,0,2*PI,-200,200,Standard_True,Standard_True);
  drawSurface(aCylTrimmed4);
  }
//================================================================
// Function : DCA_Presentation::sampleToroidalSurface
// Purpose  : 
//================================================================
void DCA_Presentation::sampleToroidalSurface()
{
  Standard_CString aTitle = " Construction of ToroidalSurface ";
  TCollection_AsciiString aText(
    "  //define direction vectors" EOL
    "  gp_Dir aDir(1,0,0);" EOL
    "  //define toroidal surface's axis" EOL
    "  gp_Ax3 anAx3(aCenterPoint, aDir);" EOL
    "" EOL
    "  //define two radiuses" EOL
    "  Standard_Real MajorRadius = 200;" EOL
    "  Standard_Real MinorRadius = 100;" EOL
    "  " EOL
    "  //make torus" EOL
    "  gp_Torus aTorus(anAx3, MajorRadius, MinorRadius);" EOL
  );
  Comment(aTitle,aText);
  
  //define direction vectors
  gp_Dir aDir(1,0,0);
  //define toroidal surface's axis
  gp_Ax3 anAx3(aCenterPoint, aDir);

  //define two radiuses
  Standard_Real MajorRadius = 200;
  Standard_Real MinorRadius = 100;
  
  //make torus
  gp_Torus aTorus(anAx3, MajorRadius, MinorRadius);

  //display shapes in the viewer
  drawPoint(aCenterPoint);
  drawVector(aCenterPoint,gp_Vec(aDir)*400,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output MajorRadius
  Handle(Geom_Line) R1 = new Geom_Line(gce_MakeLin(aCenterPoint, gp_Dir(0,1,0)));
  Handle(Geom_TrimmedCurve) aTrimmedR1 = new Geom_TrimmedCurve(R1,0,MajorRadius);
  drawCurve(aTrimmedR1, Quantity_Color(Quantity_NOC_WHITE));
  drawCurve(new Geom_Circle(anAx3.Ax2(), MajorRadius));

  if(WAIT_A_LITTLE) return;
  
  // output MinorRadius as a circle
  gp_Ax2 aMinorRadAx(aTrimmedR1->EndPoint(), gp_Dir(0,0,1));
  Handle(Geom_Circle) aMinorRadCirc = new Geom_Circle(aMinorRadAx, MinorRadius);
  drawCurve(aMinorRadCirc);

  if(WAIT_A_LITTLE) return;

  //output torus  
  drawSurface(new Geom_ToroidalSurface(aTorus));
  
}
//================================================================
// Function : DCA_Presentation::sampleConicalSurface
// Purpose  : 
//================================================================
void DCA_Presentation::sampleConicalSurface()
{
  Standard_CString aTitle = " Construction of ConicalSurface";
  TCollection_AsciiString aText(
    "  //define direction vectors" EOL
    "  gp_Dir aDir(1,0,0);" EOL
    "  " EOL
    "  //define points" EOL
    "  gp_Pnt aPoint_1(0,0,0);" EOL
    "  gp_Pnt aPoint_2(240,0,0);" EOL
    "  gp_Pnt aPoint_3(0,-50,0);" EOL
    "  gp_Pnt aPoint_4(100,-60,0);" EOL
    "  gp_Pnt aConePnt(120,-100,0);" EOL
    "  gp_Pnt aConePnt1(0,-50,0);" EOL
    "  gp_Pnt aConePnt2(0,-100,0);" EOL
    "" EOL
    "  //define axises" EOL
    "  gp_Ax2 anAx2(aCenterPoint,aDir);" EOL
    "  gp_Ax1 anAx1(aCenterPoint,aDir);" EOL
    "" EOL
    "  //define radiuses" EOL
    "  Standard_Real Radius1 = 100;" EOL
    "  Standard_Real Radius2 = 50;" EOL
    "  Standard_Real Radius3 = 70;" EOL
    "" EOL
    "  //define angle" EOL
    "  Standard_Real Angle = PI/6;" EOL
    "" EOL
    "  //METHOD 1.(axis,two points)" EOL
    "  gp_Cone aCone1 = gce_MakeCone(anAx1, aConePnt1, aConePnt);" EOL
    "" EOL
    "  //METHOD 2.(two points,two radiuses)" EOL
    "  gp_Cone aCone2 = gce_MakeCone(aConePnt2,aConePnt,Radius2,Radius1);" EOL
    "" EOL
    "  //METHOD 3.(axis,angle,radius)" EOL
    "  gp_Cone aCone3 = gce_MakeCone(anAx2,Angle,Radius3);" EOL
    "" EOL
    "  //METHOD 4.(four points)" EOL
    "  gp_Cone aCone4 = gce_MakeCone (aPoint_1,aPoint_2,aPoint_3,aPoint_4);" EOL
  );
  
  Comment(aTitle,aText);

  //define direction vectors
  gp_Dir aDir(1,0,0);
  
  //define points
  gp_Pnt aPoint_1(0,0,0);
  gp_Pnt aPoint_2(240,0,0);
  gp_Pnt aPoint_3(0,-50,0);
  gp_Pnt aPoint_4(100,-60,0);
  gp_Pnt aConePnt(120,-100,0);
  gp_Pnt aConePnt1(0,-50,0);
  gp_Pnt aConePnt2(0,-100,0);

  //define axises
  gp_Ax2 anAx2(aCenterPoint,aDir);
  gp_Ax1 anAx1(aCenterPoint,aDir);

  //define radiuses
  Standard_Real Radius1 = 100;
  Standard_Real Radius2 = 50;
  Standard_Real Radius3 = 70;

  //define angle
  Standard_Real Angle = PI/6;

  //METHOD 1.(axis,two points)
  gp_Cone aCone1 = gce_MakeCone(anAx1, aConePnt1, aConePnt);

  //METHOD 2.(two points,two radiuses)
  gp_Cone aCone2 = gce_MakeCone(aConePnt2,aConePnt,Radius2,Radius1);

  //METHOD 3.(axis,angle,radius)
  gp_Cone aCone3 = gce_MakeCone(anAx2,Angle,Radius3);

  //METHOD 4.(four points)
  gp_Cone aCone4 = gce_MakeCone (aPoint_1,aPoint_2,aPoint_3,aPoint_4);

  
  //METHOD1.(output of display)
  //output axis
  drawPoint(aCenterPoint);
  drawVector(aCenterPoint,gp_Vec(aDir)*400,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output points
  drawPoint(aConePnt);
  drawPoint(aConePnt1);

  gp_Dir aDir1(0,1,0);
  //output first radius 
  Handle(Geom_Line) aLine = new Geom_Line(gce_MakeLin(aConePnt,aDir1));
  Handle(Geom_TrimmedCurve) aTrimmed = new Geom_TrimmedCurve(aLine,0,100);
  drawCurve(aTrimmed,Quantity_Color(Quantity_NOC_WHITE));

  //output second radius
  Handle(Geom_Line) aLine1 = new Geom_Line(gce_MakeLin(aConePnt1,aDir1));
  Handle(Geom_TrimmedCurve) aTrimmed1 = new Geom_TrimmedCurve(aLine1,0,50);
  drawCurve(aTrimmed1,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output ruling of cone
  Handle(Geom_Line) aLine2 = new Geom_Line(gce_MakeLin(aConePnt,aConePnt1));
  Handle(Geom_TrimmedCurve) aTrimmed2 = 
    new Geom_TrimmedCurve(aLine2,0,aConePnt.Distance(aConePnt1));
  drawCurve(aTrimmed2,Quantity_Color(Quantity_NOC_RED));

  if(WAIT_A_LITTLE) return;

  //output cone
  Handle(Geom_ConicalSurface) aConSurface1 = new Geom_ConicalSurface(aCone1);
  Handle(Geom_RectangularTrimmedSurface) aConTrimmed1 = 
    new Geom_RectangularTrimmedSurface(aConSurface1,0,2*PI,-50,450,Standard_True,Standard_True);
  drawSurface(aConTrimmed1);

  if(WAIT_A_LITTLE) return;

  getAISContext()->EraseAll();
  
  //METHOD2.(output of display)
  drawPoint(aConePnt2);
  drawPoint(aConePnt);

  if(WAIT_A_LITTLE) return;

  drawVector(aConePnt2,gp_Vec(aConePnt2,aConePnt)*2,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output of first radius 
  Handle(Geom_Line) aLineR1 = new Geom_Line(gce_MakeLin (aConePnt,aDir1));
  Handle(Geom_TrimmedCurve) aTrimmedR1 = new Geom_TrimmedCurve(aLineR1,0,Radius1);
  drawCurve(aTrimmedR1,Quantity_Color(Quantity_NOC_WHITE));

  //output of second radius 
  Handle(Geom_Line) aLineR2 = new Geom_Line(gce_MakeLin (aConePnt2,aDir1));
  Handle(Geom_TrimmedCurve) aTrimmedR2 = new Geom_TrimmedCurve(aLineR2,0,Radius2);
  drawCurve(aTrimmedR2,Quantity_Color(Quantity_NOC_WHITE));

  //output cone
  Handle(Geom_ConicalSurface) aConSurface2 = new Geom_ConicalSurface(aCone2);
  Handle(Geom_RectangularTrimmedSurface) aConTrimmed2 =
    new Geom_RectangularTrimmedSurface(aConSurface2,0,2*PI,-120,400,Standard_True,Standard_True);
  drawSurface(aConTrimmed2);

  if(WAIT_A_LITTLE) return;

  getAISContext()->EraseAll();
  
  //METHOD3.(output of display)
  drawPoint(aCenterPoint);
  drawVector(aCenterPoint,gp_Vec(aDir)*400,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output radius
  Handle(Geom_Line) aLine3 = new Geom_Line(gce_MakeLin (aCenterPoint,aDir1));
  Handle(Geom_TrimmedCurve) aTrimmed3 = new Geom_TrimmedCurve(aLine3,0,70);
  drawCurve(aTrimmed3 ,Quantity_Color(Quantity_NOC_WHITE));

  //output cone
  Handle(Geom_ConicalSurface) aConSurface3 = new Geom_ConicalSurface(aCone3);
  Handle(Geom_RectangularTrimmedSurface) aConTrimmed = 
    new Geom_RectangularTrimmedSurface(aConSurface3,0,2*PI,-300,300,Standard_True,Standard_True);
  drawSurface(aConTrimmed);

  if(WAIT_A_LITTLE) return;

  getAISContext()->EraseAll();

  //METHOD4.(output of display)
  
  //output points
  drawPoint(aPoint_1);
  drawPoint(aPoint_2);
  drawPoint(aPoint_3);
  drawPoint(aPoint_4);  

  if(WAIT_A_LITTLE) return;

  //output vector
  drawVector(aPoint_1,gp_Vec(aPoint_1,aPoint_2)*2,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output ruling of cone
  Handle(Geom_Line) aLine4 = new Geom_Line(gce_MakeLin (aPoint_3,aPoint_4));
  Handle(Geom_TrimmedCurve) aTrimmed4 = 
    new Geom_TrimmedCurve(aLine4,0,aPoint_3.Distance(aPoint_4));
  drawCurve(aTrimmed4 ,Quantity_Color(Quantity_NOC_RED));

  if(WAIT_A_LITTLE) return;

  //output first radius
  Handle(Geom_Line) aLine5 = new Geom_Line(gce_MakeLin (aPoint_3,aDir1));
  Handle(Geom_TrimmedCurve) aTrimmed5 = new Geom_TrimmedCurve(aLine5,0,50);
  drawCurve(aTrimmed5 ,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output second radius
  Handle(Geom_Line) aLine6 = new Geom_Line(gce_MakeLin (aPoint_4,aDir1));
  Handle(Geom_TrimmedCurve) aTrimmed6 = new Geom_TrimmedCurve(aLine6,0,60);
  drawCurve(aTrimmed6 ,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  //output cone
  Handle(Geom_ConicalSurface) aConSurface4 = new Geom_ConicalSurface(aCone4);
  Handle(Geom_RectangularTrimmedSurface) aConTrimmed4 = 
    new Geom_RectangularTrimmedSurface(aConSurface4,0,2*PI,-300,300,Standard_True,Standard_True);
  drawSurface(aConTrimmed4);

}
//================================================================
// Function : DCA_Presentation::sampleSphericalSurface
// Purpose  : 
//================================================================
void DCA_Presentation::sampleSphericalSurface()
{
  Standard_CString aTitle = " Construction of SphericalSurface";
  TCollection_AsciiString aText(
    "  //define direction vector" EOL
    "  gp_Dir aDir(1,0,0);" EOL
    "" EOL
    "  //define axises" EOL
    "  gp_Ax3 anAx3(aCenterPoint,aDir);" EOL
    "" EOL
    "  //define radius" EOL
    "  Standard_Real Radius = 300;" EOL
    "" EOL
    "  //make sphere" EOL
    "  gp_Sphere aSphere(anAx3,Radius);" EOL
  );
  Comment(aTitle,aText);

  //define direction vector
  gp_Dir aDir(1,0,0);

  //define axises
  gp_Ax3 anAx3(aCenterPoint,aDir);

  //define radius
  Standard_Real Radius = 300;

  //make sphere
  gp_Sphere aSphere(anAx3,Radius);
  
  //output of display
  drawPoint(aCenterPoint);
  drawVector(aCenterPoint,gp_Vec(aDir)*400,Quantity_Color(Quantity_NOC_WHITE));

  if(WAIT_A_LITTLE) return;

  drawSurface(new Geom_SphericalSurface(aSphere));
}



//================================================================
// Function : DCA_Presentation::Comment
// Purpose  : 
//================================================================
void DCA_Presentation::Comment(const Standard_CString theTitle,
                               TCollection_AsciiString &theText)
{
  setResultTitle(theTitle);
  setResultText(theText.ToCString());
}
