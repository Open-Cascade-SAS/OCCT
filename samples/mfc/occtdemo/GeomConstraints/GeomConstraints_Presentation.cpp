// GeomConstraints_Presentation.cpp: implementation of the GeomConstraints_Presentation class.
// Construct curves and surfaces from constraints
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeomConstraints_Presentation.h"
#include <OCCDemo_Presentation.h>

#include <Standard_Stream.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>

#include <GeomAdaptor_HCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_TrimmedCurve.hxx>

#include <GeomPlate_Surface.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <BRepFill_CurveConstraint.hxx>
#include <GeomFill_SimpleBound.hxx>
#include <GeomFill_ConstrainedFilling.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomFill_BezierCurves.hxx>
#include <FairCurve_AnalysisCode.hxx>
#include <FairCurve_Batten.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Precision.hxx>
#include <GeomAPI_PointsToBSpline.hxx>

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new GeomConstraints_Presentation;

// Initialization of array of samples
const GeomConstraints_Presentation::PSampleFuncType GeomConstraints_Presentation::SampleFuncs[] =
{ 
  &GeomConstraints_Presentation::sample1,
  &GeomConstraints_Presentation::sample2,
  &GeomConstraints_Presentation::sample3,
  &GeomConstraints_Presentation::sample4,
  &GeomConstraints_Presentation::sample5,
  &GeomConstraints_Presentation::sample6
};



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GeomConstraints_Presentation::GeomConstraints_Presentation()
{
  myIndex = 0;
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("GeomConstraints");
}

//=========================================================================================
// Sample execution
//=========================================================================================

void GeomConstraints_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}


//=========================================================================================
// Function creating Surface,BSPline,Bezier and 2dBSpline Curves
//=========================================================================================

static Handle(Geom_BSplineCurve) createBSplineCurve(const Standard_Integer nPoles,
                                 const Standard_Real theCoords[][3])
{
  TColgp_Array1OfPnt aCurvePoint (1, nPoles);

  for (Standard_Integer i=0; i < nPoles; i++)
    aCurvePoint(i+1) = gp_Pnt (theCoords[i][0]*100, theCoords[i][1]*100, theCoords[i][2]*100);

  Standard_Integer MinDegree = 3;
  Standard_Integer MaxDegree = 8;

  return GeomAPI_PointsToBSpline (
    aCurvePoint, MinDegree, MaxDegree, GeomAbs_C2, Precision::Confusion());
}


static Handle(Geom_BezierCurve) createBezierCurve(const Standard_Integer nPoles,
                                 const Standard_Real theCoords[][3])
{
  TColgp_Array1OfPnt aCurvePoint (1, nPoles);

  for (Standard_Integer i=0; i < nPoles; i++)
    aCurvePoint(i+1) = gp_Pnt (theCoords[i][0]*100, theCoords[i][1]*100, theCoords[i][2]*100);
 
  Handle(Geom_BezierCurve) aBCurve = new Geom_BezierCurve(aCurvePoint);
  return aBCurve;
}

static Handle(Geom2d_BSplineCurve) battenCurve(const Standard_Real& theAngle1,
                                                const Standard_Real& theAngle2)
{
   //define points
  gp_Pnt2d aPoint1(-500,0);
  gp_Pnt2d aPoint2(400,0);
  
  //define height
  Standard_Real Height = 5;

  // method batten
  FairCurve_Batten aBatten(aPoint1,aPoint2,Height);

  //change parameters
  aBatten.SetAngle1(theAngle1);
  aBatten.SetAngle2(theAngle2);
  FairCurve_AnalysisCode aCode;
  Standard_Boolean aCompute = aBatten.Compute(aCode);
    
  Handle(Geom2d_BSplineCurve) theCurve;
  if(aCompute)
    theCurve = aBatten.Curve();
  return theCurve;
}


static Handle(Geom_Surface) createSurface(
                                 const Standard_Integer endBound,
                                 TColgp_Array1OfPnt& thePoint,
                                 const Handle(Geom_Plane)& thePlane)
{
  Standard_Integer Degree = 3;
  Standard_Integer NbPtsOnCur = 10;
  Standard_Integer NbIter = 3;
  Standard_Integer Order = 0;
  Standard_Integer MaxSeg=9;
  Standard_Integer MaxDegree=5;
  Standard_Real dmax,anApproxTol = 0.001, aConstrTol = Precision::Confusion();
  
  // define object BuildPlateSurface
  GeomPlate_BuildPlateSurface BPSurf(Degree,NbPtsOnCur,NbIter);
  BPSurf.LoadInitSurface(thePlane);
  
  for (Standard_Integer i = 1; i <= endBound ; i++)
  {
  // points constraint
    Handle(GeomPlate_PointConstraint) PCont = 
      new GeomPlate_PointConstraint(thePoint.Value(i),Order, aConstrTol);
    BPSurf.Add(PCont);
  }
  BPSurf.Perform();

  // make PlateSurface
  Handle(GeomPlate_Surface) PSurf;
  Handle(Geom_Surface) aSurf;
  
  if (BPSurf.IsDone())
  {
    PSurf = BPSurf.Surface();

    // define parameter approximation
    dmax = Max(0.01,10*BPSurf.G0Error());

    // make approximation
    GeomPlate_MakeApprox Mapp(PSurf,anApproxTol, MaxSeg,MaxDegree,dmax);
    aSurf = Mapp.Surface();
  }

  return aSurf;
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////


//==========================================================================================
// Function : GeomConstraints_Presentation::sample1
// Purpose  : 
//==========================================================================================

void GeomConstraints_Presentation::sample1()
{
  setResultTitle("Method Batten (Create 2dBSpline Curve)");
  TCollection_AsciiString aText(
    " // define points" EOL
    " gp_Pnt2d aPoint1(-500,0);" EOL
    " gp_Pnt2d aPoint2(400,0);" EOL EOL

    " // define height" EOL
    " Standard_Real Height = 5;" EOL EOL
    
    " // construct algo" EOL
    " FairCurve_Batten aBatten(aPoint1,aPoint2,Height);" EOL EOL
    
    " // set constraint angles " EOL
    " Standard_Real Angle1 = PI/4; " EOL
    " Standard_Real Angle2 = PI/3; " EOL
    " aBatten.SetAngle1(Angle1);" EOL
    " aBatten.SetAngle2(Angle2);" EOL EOL

    " FairCurve_AnalysisCode aCode;" EOL 
    " Standard_Boolean aCompute = aBatten.Compute(aCode);" EOL EOL

    " // create BSpline Curve " EOL
    " Handle(Geom2d_BSplineCurve) aCurve; " EOL
    " if(aCompute) " EOL
    "   aCurve = aBatten.Curve();" EOL EOL
  );
  setResultText(aText.ToCString());
  
  // define points
  gp_Pnt2d aPoint1(-500,0);
  gp_Pnt2d aPoint2(400,0);

  Standard_Real Angle1=0,Angle2=0;
  Standard_Real Angle3=PI/9,Angle4=PI/4,Angle5=PI/3;
 
  Handle(Geom2d_BSplineCurve) aCurve = battenCurve(Angle1,Angle2);
  Handle(Geom2d_BSplineCurve) aCurve1 = battenCurve(Angle3,Angle3);
  Handle(Geom2d_BSplineCurve) aCurve2 = battenCurve(Angle4,Angle5);
  
  // output 
  gp_Pnt aPoint(0,0,0);
  gp_Pnt aPnt1(-500,0,0);
  gp_Pnt aPnt2(400,0,0);
  gp_Dir aDir(0,0,1);
  gp_Ax2 anAx2(aPoint,aDir);
  
  Handle(Geom2d_Line) aLine = new Geom2d_Line(aPoint1,gp_Dir2d(1,0));
  Handle(Geom2d_TrimmedCurve) aTrimmedLine =
    new Geom2d_TrimmedCurve(aLine,-250,aPoint1.Distance(aPoint2) + 250);
  
  gp_Lin2d aLin(aPoint1,gp_Dir2d(1,0)) ;
  Handle(Geom2d_Line) aLine10 = new Geom2d_Line(aLin.Rotated(aPoint1,PI/9));
  Handle(Geom2d_Line) aLine11 = new Geom2d_Line(aLin.Rotated(aPoint1,PI/4));
  Handle(Geom2d_TrimmedCurve) aTrimmedLine10 = 
    new Geom2d_TrimmedCurve(aLine10,0,300);
  Handle(Geom2d_TrimmedCurve) aTrimmedLine11 = 
    new Geom2d_TrimmedCurve(aLine11,0,300);
  
  gp_Lin2d aLin1(aPoint2,gp_Dir2d(-1,0)) ;
  Handle(Geom2d_Line) aLine20 = new Geom2d_Line(aLin1.Rotated(aPoint2,-PI/9));
  Handle(Geom2d_Line) aLine21 = new Geom2d_Line(aLin1.Rotated(aPoint2,-PI/3));
  Handle(Geom2d_TrimmedCurve) aTrimmedLine20 = 
    new Geom2d_TrimmedCurve(aLine20,0,300);
  Handle(Geom2d_TrimmedCurve) aTrimmedLine21 = 
    new Geom2d_TrimmedCurve(aLine21,0,300);
  
  gp_Ax2d anAx2d_1(aPoint1,gp_Dir2d(1,0));
  gp_Ax2d anAx2d_2(aPoint2,gp_Dir2d(-1,0));
  Standard_Real Radius = 200;
  Handle(Geom2d_Circle) aCircle1_1 = new 
    Geom2d_Circle(anAx2d_1,Radius);
  Handle(Geom2d_Circle) aCircle2_1 = new 
    Geom2d_Circle(anAx2d_2,Radius);

  Handle(Geom2d_TrimmedCurve) aTrimmedCircle1_1 = 
    new Geom2d_TrimmedCurve(aCircle1_1,0,PI/9);
  Handle(Geom2d_TrimmedCurve) aTrimmedCircle1_2 =
    new Geom2d_TrimmedCurve(aCircle2_1,-PI/9,0);
  Handle(Geom2d_TrimmedCurve) aTrimmedCircle2_1 = 
    new Geom2d_TrimmedCurve(aCircle1_1,0,PI/4);
  Handle(Geom2d_TrimmedCurve) aTrimmedCircle2_2 = 
    new Geom2d_TrimmedCurve(aCircle2_1,-PI/3,0);
  
  Handle(AIS_InteractiveObject) aObjs1[10];
  
  // output point
  drawPoint(aPnt1);
  drawPoint(aPnt2);
  
  drawCurve(aTrimmedLine,Quantity_Color(Quantity_NOC_GRAY),Standard_True,anAx2);
  // output first curve
  aObjs1[0]=drawCurve(aCurve,Quantity_Color(Quantity_NOC_RED),Standard_True,anAx2);
  if(WAIT_A_SECOND) return;
  getAISContext()->Erase(aObjs1[0]);
  
  // output second curve
  aObjs1[1]=drawCurve(aTrimmedLine20,Quantity_Color(Quantity_NOC_GRAY),Standard_True,anAx2);
  aObjs1[2]=drawCurve(aTrimmedLine10,Quantity_Color(Quantity_NOC_GRAY),Standard_True,anAx2);
  aObjs1[3]=drawCurve(aTrimmedCircle1_1,Quantity_Color(Quantity_NOC_GRAY),Standard_True,anAx2);
  aObjs1[4]=drawCurve(aTrimmedCircle1_2,Quantity_Color(Quantity_NOC_GRAY),Standard_True,anAx2);
  aObjs1[5]=drawCurve(aCurve1,Quantity_Color(Quantity_NOC_RED),Standard_True,anAx2);
  if(WAIT_A_SECOND) return;
  
  getAISContext()->Erase(aObjs1[1],Standard_False);
  getAISContext()->Erase(aObjs1[2],Standard_False);
  getAISContext()->Erase(aObjs1[3],Standard_False);
  getAISContext()->Erase(aObjs1[4],Standard_False);
  getAISContext()->Erase(aObjs1[5],Standard_False);
  // output third curve
  drawCurve(aTrimmedLine21,Quantity_Color(Quantity_NOC_GRAY),Standard_True,anAx2);
  drawCurve(aTrimmedLine11,Quantity_Color(Quantity_NOC_GRAY),Standard_True,anAx2);
 
  aObjs1[6]=drawCurve(aTrimmedCircle2_1,Quantity_Color(Quantity_NOC_GRAY),Standard_True,anAx2);
  aObjs1[7]=drawCurve(aTrimmedCircle2_2,Quantity_Color(Quantity_NOC_GRAY),Standard_True,anAx2);
  aObjs1[8]=drawCurve(aCurve2,Quantity_Color(Quantity_NOC_RED),Standard_True,anAx2);
  
} 


//==========================================================================================
// Function : GeomConstraints_Presentation::sample2
// Purpose  : 
//==========================================================================================


void GeomConstraints_Presentation::sample2()
{
  setResultTitle("Create PlateSurface");
  TCollection_AsciiString aText(
    " // define points" EOL
    " gp_Pnt P1(-300.,-300.,0.); " EOL
    " gp_Pnt P2(300.,-300.,0.);" EOL
    " gp_Pnt P3(300.,300.,0.);" EOL
    " gp_Pnt P4(-300.,300.,0.); " EOL
    " gp_Pnt P5(0.,0.,150.); " EOL
    " gp_Pnt P6(-150.,-100.,30.);" EOL EOL
    
    " // define parameters" EOL
    " Standard_Integer Order = 0;" EOL
    " Standard_Integer Tang = 0;" EOL
    " Standard_Integer Degree = 3;" EOL
    " Standard_Integer NbPtsOnCur = 10;" EOL
    " Standard_Integer NbIter = 3;" EOL
    " Standard_Integer MaxSeg=9;" EOL
    " Standard_Integer MaxDegree=5;" EOL
    " Standard_Real dmax,Tol = 0.001;" EOL EOL
    
    " // create lines between points" EOL
    " Handle(Geom_Line) aLine1,aLine2,Line3;" EOL
    "  aLine1 = new Geom_Line(P1,gp_Dir(1,0,0));" EOL
    "  aLine2 = new Geom_Line(P2,gp_Dir(0,1,0));" EOL
    "  aLine3 = new Geom_Line(P3,gp_Dir(-1,0,0));" EOL EOL
    
    " // adapted curves" EOL
    " Handle(GeomAdaptor_HCurve) aCurve1,aCurve2,aCurve3; " EOL
    "  aCurve1 = new GeomAdaptor_HCurve(aLine1); " EOL
    "  aCurve2 = new GeomAdaptor_HCurve(aLine2); " EOL
    "  aCurve3 = new GeomAdaptor_HCurve(aLine3); " EOL EOL
    
    " // curves constraint" EOL
    " Handle(BRepFill_CurveConstraint) aBFCC1,aBFCC2,aBFCC3; " EOL
    "  aBFCC1 = new BRepFill_CurveConstraint(aCurve1,Tang);" EOL
    "  aBFCC2 = new BRepFill_CurveConstraint(aCurve2,Tang);" EOL
    "  aBFCC3 = new BRepFill_CurveConstraint(aCurve3,Tang);" EOL EOL
    
    " // point constraint" EOL
    " Handle(GeomPlate_PointConstraint) PCont,PCont1; " EOL
    "  PCont = new GeomPlate_PointConstraint(P5,Order);" EOL 
    "  PCont1 = new GeomPlate_PointConstraint(P6,Order);" EOL EOL
    
    " // define object BuildPlateSurface" EOL
    " GeomPlate_BuildPlateSurface BPSurf(Degree,NbPtsOnCur,NbIter);" EOL EOL
    
    " // add all used constraints " EOL
    " BPSurf.Add(PCont);" EOL
    " BPSurf.Add(PCont1);" EOL
    " BPSurf.Add(aBFCC1);" EOL
    " BPSurf.Add(aBFCC2);" EOL
    " BPSurf.Add(aBFCC3);" EOL EOL
 
    " // build BuildPlateSurface" EOL
    " BPSurf.Perform();" EOL EOL
    
    " // make PlateSurface" EOL
    " Handle(GeomPlate_Surface) PSurf = BPSurf.Surface();" EOL EOL
    
    " // define parameter approximation" EOL
    " dmax = Max(0.01,10*BPSurf.G0Error());" EOL EOL
    
    " // make approximation" EOL
    " GeomPlate_MakeApprox Mapp(PSurf,Tol,MaxSeg,MaxDegree,dmax);" EOL
    " Handle(Geom_Surface) aSurf;" EOL
    " aSurf = Mapp.Surface();" EOL EOL
    );
  setResultText(aText.ToCString());
 
  // define points
  gp_Pnt P1(-300.,-300.,0.); 
  gp_Pnt P2(300.,-300.,0.);
  gp_Pnt P3(300.,300.,0.);
  gp_Pnt P4(-300.,300.,0.); 
  gp_Pnt P5(0.,0.,150.); 
  gp_Pnt P6(-150.,-100.,30.);
  
  // define parameters
  Standard_Integer Order = 0;
  Standard_Integer Tang = 0;
  Standard_Integer Degree = 3;
  Standard_Integer NbPtsOnCur = 10;
  Standard_Integer NbIter = 3;
  Standard_Integer MaxSeg=9;
  Standard_Integer MaxDegree=5;
  Standard_Real dmax,Tol = 0.001;

  // make lines
  Handle(Geom_Line) aLine1 = new Geom_Line(P1,gp_Dir(1,0,0));
  Handle(Geom_Line) aLine2 = new Geom_Line(P2,gp_Dir(0,1,0));
  Handle(Geom_Line) aLine3 = new Geom_Line(P3,gp_Dir(-1,0,0));
  
  // trimmed lines
  Handle(Geom_TrimmedCurve) aLine11 = 
    new Geom_TrimmedCurve(aLine1,0,P1.Distance(P2));
  Handle(Geom_TrimmedCurve) aLine21 = 
    new Geom_TrimmedCurve(aLine2,0,P2.Distance(P3));
  Handle(Geom_TrimmedCurve) aLine31 = 
    new Geom_TrimmedCurve(aLine3,0,P3.Distance(P4));
  
  // adapted curves
  Handle(GeomAdaptor_HCurve) aCurve1 = new GeomAdaptor_HCurve(aLine11);
  Handle(GeomAdaptor_HCurve) aCurve2 = new GeomAdaptor_HCurve(aLine21);
  Handle(GeomAdaptor_HCurve) aCurve3 = new GeomAdaptor_HCurve(aLine31);

  // curves constraint
  Handle(BRepFill_CurveConstraint) aBFCC1= 
    new BRepFill_CurveConstraint(aCurve1,Tang);
  Handle(BRepFill_CurveConstraint) aBFCC2= 
    new BRepFill_CurveConstraint(aCurve2,Tang);
  Handle(BRepFill_CurveConstraint) aBFCC3= 
    new BRepFill_CurveConstraint(aCurve3,Tang);

  // points constraint
  Handle(GeomPlate_PointConstraint) PCont = 
    new GeomPlate_PointConstraint(P5,Order);
  Handle(GeomPlate_PointConstraint) PCont1 = 
    new GeomPlate_PointConstraint(P6,Order);
  
  // define object BuildPlateSurface
  GeomPlate_BuildPlateSurface BPSurf(Degree,NbPtsOnCur,NbIter);
 
  // ======= 1 =========
  BPSurf.Add(PCont);
  BPSurf.Add(PCont1);
  BPSurf.Add(aBFCC1);
  BPSurf.Add(aBFCC2);
  BPSurf.Add(aBFCC3);
  BPSurf.Perform();
   // make PlateSurface
  Handle(GeomPlate_Surface) PSurf3 = BPSurf.Surface();
  // define parameter approximation
   dmax = Max(0.01,10*BPSurf.G0Error());
  // make approximation
  GeomPlate_MakeApprox Mapp3(PSurf3,Tol,MaxSeg,MaxDegree,dmax);
  Handle(Geom_Surface) aSurf3 = Mapp3.Surface();
  
  // ======= 2 =========
  BPSurf.Init();
  BPSurf.Add(PCont);
  BPSurf.Add(aBFCC1);
  BPSurf.Add(aBFCC2);
  BPSurf.Add(aBFCC3);
  BPSurf.Perform();
   // make PlateSurface
  Handle(GeomPlate_Surface) PSurf2 = BPSurf.Surface();
   // make approximation
  GeomPlate_MakeApprox Mapp2(PSurf2,Tol,MaxSeg,MaxDegree,dmax);
  Handle(Geom_Surface) aSurf2 = Mapp2.Surface();

  // ======= 3 =========
  BPSurf.Init();
  BPSurf.Add(aBFCC1);
  BPSurf.Add(PCont);
  BPSurf.Add(aBFCC3);
  BPSurf.Perform();
   // make PlateSurface
  Handle(GeomPlate_Surface) PSurf1 = BPSurf.Surface();
  // make approximation
  GeomPlate_MakeApprox Mapp1(PSurf1,Tol,MaxSeg,MaxDegree,dmax);
  Handle(Geom_Surface) aSurf1 = Mapp1.Surface();
  
  // ===== output =====

  // make and trimmed plane
  Handle(Geom_Plane) aPlane = new Geom_Plane(P1,gp_Dir(0,0,1));
  Handle(Geom_RectangularTrimmedSurface) aTrimmedPlane = 
    new Geom_RectangularTrimmedSurface(aPlane,-200,700,-200,700,Standard_True,Standard_True);

  Handle(AIS_InteractiveObject) aObj;
  
  drawCurve(aLine11);
  drawCurve(aLine31);
  if(WAIT_A_SECOND) return;
  aObj=drawSurface(aTrimmedPlane);
  if(WAIT_A_SECOND) return;
  
  drawPoint(P5);
  if(WAIT_A_SECOND) return;
  getAISContext()->Erase(aObj,Standard_False);
  aObj=drawSurface(aSurf1);
  if(WAIT_A_SECOND) return;
  
  drawCurve(aLine21);
  if(WAIT_A_SECOND) return;
  getAISContext()->Erase(aObj,Standard_False);
  aObj=drawSurface(aSurf2);
  if(WAIT_A_SECOND) return;

  drawPoint(P6);
  if(WAIT_A_SECOND) return;
  getAISContext()->Erase(aObj,Standard_False);
  drawSurface(aSurf3);
}


//==========================================================================================
// Function : GeomConstraints_Presentation::sample3
// Purpose  : 
//==========================================================================================

void GeomConstraints_Presentation::sample3()
{
  
  setResultTitle("Surface from cloud of points");
  TCollection_AsciiString aText(
    " // define parameters for GeomPlate_BuildPlateSurface" EOL
    " Standard_Integer Degree = 3;" EOL
    " Standard_Integer NbPtsOnCur = 10;" EOL
    " Standard_Integer NbIter = 3;" EOL
    " Standard_Integer Order = 0;" EOL
    " Standard_Integer MaxSeg=9;" EOL
    " Standard_Integer MaxDegree=5;" EOL
    " Standard_Real dmax,anApproxTol = 0.001;" EOL
    " Standard_Real aConstrTol = Precision::Confusion();" EOL
    "" EOL
    " // number of points for GeomPlate_BuildPlateSurface" EOL
    " Standard_Integer aSize;" EOL
    "" EOL
    " // define aSize ..." EOL
    "" EOL
    " // define array of points" EOL 
    " TColgp_Array1OfPnt aTColPnt(1,aSize);" EOL
    "" EOL
    " // initialize array " EOL
    " // ..." EOL
    "" EOL
    " // define object BuildPlateSurface" EOL
    " GeomPlate_BuildPlateSurface BPSurf(Degree,NbPtsOnCur,NbIter);" EOL
    "" EOL
    " for (Standard_Integer i = 1; i <= aSize ; i++)" EOL
    " {" EOL
    "   // points constraint" EOL
    "   Handle(GeomPlate_PointConstraint) PCont = " EOL
    "     new GeomPlate_PointConstraint(aTColPnt.Value(i),Order,aConstrTol);" EOL
    "   BPSurf.Add(PCont);" EOL
    " }" EOL
    "    " EOL
    " // build BuildPlateSurface" EOL
    " BPSurf.Perform();" EOL
    "" EOL
    " // make PlateSurface" EOL
    " Handle(GeomPlate_Surface) PSurf;" EOL
    " Handle(Geom_Surface) aSurf;" EOL
    " " EOL
    " if (BPSurf.IsDone())" EOL
    " {" EOL
    "   PSurf = BPSurf.Surface();" EOL
    ""EOL
    "   // define parameter approximation" EOL
    "   dmax = Max(0.01,10*BPSurf.G0Error());" EOL
    ""EOL
    "   // make approximation" EOL
    "   GeomPlate_MakeApprox Mapp(PSurf,anApproxTol,MaxSeg,MaxDegree,dmax);" EOL
    "   aSurf = Mapp.Surface();" EOL
    " }" EOL
  );
  setResultText(aText.ToCString()); 

  TCollection_AsciiString aFileName(GetDataDir());
  aFileName += "\\points.dat";

  // open file
  ifstream aFile;
  aFile.open(aFileName.ToCString(), ios::failbit);
  if(!aFile)
  {
    aFileName += " was not found.  The sample can not be shown.";
    setResultText(aFileName.ToCString());
    return;
  }
    
  // define parameters
  Standard_Integer aSize;
  Standard_Integer aVal1 = 0;
  Standard_Integer aVal2 = 0;
  Standard_Integer aVal3 = 0;

  aFile>>aSize;
  
  // define array
  TColgp_Array1OfPnt aTColPnt(1,aSize);
  gp_Pnt aPoint(0,0,-2);
  gp_Dir aDir(0,0,1);
  Handle(Geom_Plane) aPlane = 
    new Geom_Plane(aPoint,aDir);
 
  // read of file
  for(Standard_Integer i =0; i < aSize; i++)
  {
    aFile>>aVal1;
    aFile>>aVal2;
    aFile>>aVal3;
    aTColPnt(i+1) = gp_Pnt(aVal1*40, aVal2*40, aVal3*40);
  }  
  aFile.close();

  Handle(AIS_InteractiveObject) anIO,aNewIO;
 
  for(Standard_Integer j = 1; j <= int(aSize / 20); j++)
  {
    // output points
    for(Standard_Integer n = 20*(j-1) + 1; n <= 20*j; n++)
    {
      drawPoint(aTColPnt.Value(n));
    }

    if(WAIT_A_SECOND) return;

    // create surface
    Handle(Geom_Surface) aSurf = createSurface(n-1, aTColPnt,aPlane);
   
    if (!aSurf.IsNull())
    {
      // output surface
      aNewIO = drawSurface(aSurf, Quantity_NOC_LEMONCHIFFON3, Standard_False);
      getAISContext()->Erase(anIO, Standard_False);
      getAISContext()->Display(aNewIO);
      anIO = aNewIO;
      if(WAIT_A_SECOND) return;
     }
  }
}

//==========================================================================================
// Function : GeomConstraints_Presentation::sample4
// Purpose  : Make Bezier Surface by Bezier Curves
//==========================================================================================

void GeomConstraints_Presentation::sample4()
{
  setResultTitle("Fill surface between bezier curves");
  TCollection_AsciiString aText(
    
    " // define Bezier curves " EOL
    " Handle(Geom_BezierCurve) aBezierCurve,aBezierCurve1,aBezierCurve2; " EOL
    " // initializing bezier curves  ..." EOL EOL

    " // initializing object that creates a surface based on the given bezier curves" EOL
    " GeomFill_BezierCurves aFBC1(aBezierCurve,aBezierCurve1,GeomFill_CurvedStyle);" EOL
    " GeomFill_BezierCurves aFBC2(aBezierCurve,aBezierCurve1,aBezierCurve2,GeomFill_CoonsStyle);" EOL EOL
    " // make Bezier Surface" EOL
    " Handle(Geom_BezierSurface) aBezierSurf1 = aFBC1.Surface();" EOL
    " Handle(Geom_BezierSurface) aBezierSurf2 = aFBC2.Surface();" EOL


    );
  setResultText(aText.ToCString());

  Handle(AIS_InteractiveObject) aObjs[2];

  // define arrays of points
  Standard_Real aCoords[][3] = {{-6,0,-3},{-4,1,-3},{-2,-3,-3},{0,3,-1}};
  Standard_Real aCoords1[][3] = {{-6,0,-3},{-4,2,-2},{-4,0,-1},{-6,0,1}};
  Standard_Real aCoords2[][3] = {{-6,0,1},{-3,0,0},{2,3,1}};
  
  // define lengths 
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  Standard_Integer nPoles1 = sizeof(aCoords1)/(sizeof(Standard_Real)*3);
  Standard_Integer nPoles2 = sizeof(aCoords2)/(sizeof(Standard_Real)*3);
  
  // make Bezier curves
  Handle(Geom_BezierCurve) aBezierCurve = createBezierCurve(nPoles,aCoords);
  Handle(Geom_BezierCurve) aBezierCurve1 = createBezierCurve(nPoles1,aCoords1);
  Handle(Geom_BezierCurve) aBezierCurve2 = createBezierCurve(nPoles2,aCoords2);


   // initializing object that creates a surface based on the given bezier curves
  GeomFill_BezierCurves aFBC1(aBezierCurve,aBezierCurve1,GeomFill_CurvedStyle);
  GeomFill_BezierCurves aFBC2(aBezierCurve,aBezierCurve1,aBezierCurve2,GeomFill_CurvedStyle);
  
  Handle(Geom_BezierSurface) aBezierSurf1 = aFBC1.Surface();
  Handle(Geom_BezierSurface) aBezierSurf2 = aFBC2.Surface();
 
  // output bezier curves and surface
  drawCurve(aBezierCurve);  
  if(WAIT_A_LITTLE) return;
  drawCurve(aBezierCurve1);
  if(WAIT_A_SECOND) return;
  // output surface 1
  aObjs[0]=drawSurface(aBezierSurf1);
  if(WAIT_A_SECOND) return;
  getAISContext()->Erase(aObjs[0],Standard_False);
  if(WAIT_A_LITTLE) return;
  drawCurve(aBezierCurve2);  
  if(WAIT_A_SECOND) return;
  // output surface 2
  aObjs[1]=drawSurface(aBezierSurf2);
}

//==========================================================================================
// Function : GeomConstraints_Presentation::sample5
// Purpose  : Make BSpline Surface by BSpline Curves
//==========================================================================================

void GeomConstraints_Presentation::sample5()
{ 
  setResultTitle("Fill surface between bspline curves");
  TCollection_AsciiString aText(
    " // define bspline curves " EOL
    " Handle (Geom_BSplineCurve) aBSCurve1,aBSCurve2,aBSCurve3,aBSCurve4; "EOL
    " //initializing bspline curves ..." EOL EOL

    " // initializing object that creates a surface based on the given bspline curves" EOL
    " GeomFill_BSplineCurves aFBSC1(aBSCurve1,aBSCurve2,aBSCurve3,GeomFill_CurvedStyle);" EOL
    " GeomFill_BSplineCurves aFBSC2(aBSCurve1,aBSCurve2,aBSCurve3,aBSCurve4,GeomFill_CurvedStyle);" EOL EOL

    " // make BSpline Surface" EOL
    " Handle(Geom_BSplineSurface) aBSplineSurf1 = aFBSC1.Surface();" EOL
    " Handle(Geom_BSplineSurface) aBSplineSurf2 = aFBSC2.Surface();" EOL
    );
  setResultText(aText.ToCString());


  Handle(AIS_InteractiveObject) aObjs[2];
  // define arrays of points 
  Standard_Real aCoords[][3] = {{-4,0,-2},{-2,0,-4},{0,0,-1},{3,0,2}};
  Standard_Real aCoords1[][3] = {{-4,0,-2},{-3,2,-1},{-2,2,1}};
  Standard_Real aCoords2[][3] = {{-2,2,1},{-1,0,0},{0,1,2}};
  Standard_Real aCoords3[][3] = {{0,1,2},{2,2,4},{3,0,2}};
  
  // define lengths 
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  Standard_Integer nPoles1 = sizeof(aCoords1)/(sizeof(Standard_Real)*3);
  Standard_Integer nPoles2 = sizeof(aCoords2)/(sizeof(Standard_Real)*3);
  Standard_Integer nPoles3 = sizeof(aCoords3)/(sizeof(Standard_Real)*3);
  
  // make bspline curves  
  Handle(Geom_BSplineCurve) aBSCurve1 = createBSplineCurve(nPoles,aCoords); 
  Handle(Geom_BSplineCurve) aBSCurve2 = createBSplineCurve(nPoles1,aCoords1);
  Handle(Geom_BSplineCurve) aBSCurve3 = createBSplineCurve(nPoles2,aCoords2);
  Handle(Geom_BSplineCurve) aBSCurve4 = createBSplineCurve(nPoles3,aCoords3);

  // initializing object that creates a surface based on the given bspline curves
  GeomFill_BSplineCurves aFBSC1(aBSCurve1,aBSCurve2,aBSCurve3,GeomFill_CurvedStyle);
  GeomFill_BSplineCurves aFBSC2(aBSCurve1,aBSCurve2,aBSCurve3,aBSCurve4,GeomFill_CurvedStyle);

  Handle(Geom_BSplineSurface) aBSplineSurf1 = aFBSC1.Surface();
  Handle(Geom_BSplineSurface) aBSplineSurf2 = aFBSC2.Surface();
 
  // output
  drawCurve(aBSCurve1);
  if(WAIT_A_LITTLE) return;
  drawCurve(aBSCurve2);
  if(WAIT_A_LITTLE) return;
  drawCurve(aBSCurve3);
  if(WAIT_A_SECOND) return;
  
  aObjs[0]=drawSurface(aBSplineSurf1);  
  if(WAIT_A_SECOND) return;
  getAISContext()->Erase(aObjs[0],Standard_False);
  if(WAIT_A_LITTLE) return;
  drawCurve(aBSCurve4);
  if(WAIT_A_SECOND) return;
  aObjs[1]=drawSurface(aBSplineSurf2);
  
}

//==========================================================================================
// Function : GeomConstraints_Presentation::sample6
// Purpose  : Create a surface based on the given bspline and bezier curves
//==========================================================================================

void GeomConstraints_Presentation::sample6()
{
  setResultTitle("Constraint Filling");
  TCollection_AsciiString aText(
    " // define parameters" EOL
    " Standard_Real Tol = 0.00001;" EOL
    " Standard_Real dummy = 0.;" EOL
    " Standard_Integer MaxDeg = 3;" EOL
    " Standard_Integer MaxSeg = 8;" EOL EOL

    " // make bspline curves " EOL
    " Handle(Geom_BSplineCurve) aBSCurve,aBSCurve1; "EOL
    " //initializing bspline curves ..." EOL EOL

    " // make bezier curves " EOL
    " Handle(Geom_BezierCurve) aBezierCurve; "EOL
    " // initializing bezier curves ..." EOL EOL
    
    " // adapted bspline and bezier curves" EOL
    " Handle(GeomAdaptor_HCurve) adapHCurve = " EOL
    "  new GeomAdaptor_HCurve(aBSCurve);" EOL
    " Handle(GeomAdaptor_HCurve) adapHCurve1 = " EOL
    "  new GeomAdaptor_HCurve(aBSCurve1);" EOL
    " Handle(GeomAdaptor_HCurve) adapHCurve2 = " EOL
    "  new GeomAdaptor_HCurve(aBezierCurve);" EOL EOL

    " // simple bound" EOL
    " Handle(GeomFill_SimpleBound) myBoundary = " EOL 
    "  new GeomFill_SimpleBound(adapHCurve,Tol,dummy); " EOL
    " Handle(GeomFill_SimpleBound) myBoundary1 = " EOL
    "  new GeomFill_SimpleBound(adapHCurve1,Tol,dummy); " EOL
    " Handle(GeomFill_SimpleBound) myBoundary2 = " EOL
    "  new GeomFill_SimpleBound(adapHCurve2,Tol,dummy); " EOL EOL

    " // define and initilization ConstrainedFilling" EOL
    " GeomFill_ConstrainedFilling aFCF(MaxDeg,MaxSeg);" EOL EOL
    " aFCF.Init(myBoundary,myBoundary1,myBoundary2); " EOL EOL
    " // make BSpline Surface" EOL
    " Handle(Geom_BSplineSurface) aBSplineSurf; "EOL
    "  aBSplineSurface = aFCF.Surface();" EOL EOL
    );
  setResultText(aText.ToCString());

  // define arrays of points 
  Standard_Real aCoords[][3] = {{-5,2,-2},{-3,1,-4},{0,0,-1},{3,0,2}};
  Standard_Real aCoords1[][3] = {{-5,2,-2},{-3,4,-1},{-2,3,1}};
  Standard_Real aCoords2[][3] = {{-2,3,1},{-1,3,0},{2,1,0},{3,0,2}};
  
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  Standard_Integer nPoles1 = sizeof(aCoords1)/(sizeof(Standard_Real)*3);
  Standard_Integer nPoles2 = sizeof(aCoords2)/(sizeof(Standard_Real)*3);
  
  // define parameters
  Standard_Real Tol = 0.00001;
  Standard_Real dummy = 0.;
  Standard_Integer MaxDeg = 3;
  Standard_Integer MaxSeg = 8;

  // The adapted curves is created in the following way:
  Handle(Geom_Curve) aBSCurve = createBSplineCurve(nPoles,aCoords); 
  Handle(Geom_Curve) aBSCurve1 = createBSplineCurve(nPoles1,aCoords1);
  Handle(Geom_BezierCurve) aBezierCurve = createBezierCurve(nPoles2,aCoords2);

  // adapted BSpline and Bezier curves
  Handle(GeomAdaptor_HCurve) adapHCurve = new GeomAdaptor_HCurve(aBSCurve);
  Handle(GeomAdaptor_HCurve) adapHCurve1 = new GeomAdaptor_HCurve(aBSCurve1);
  Handle(GeomAdaptor_HCurve) adapHCurve2 = new GeomAdaptor_HCurve(aBezierCurve);

  // simple bound
  Handle(GeomFill_SimpleBound) myBoundary = 
    new GeomFill_SimpleBound(adapHCurve,Tol,dummy);
  Handle(GeomFill_SimpleBound) myBoundary1 = 
    new GeomFill_SimpleBound(adapHCurve1,Tol,dummy);
  Handle(GeomFill_SimpleBound) myBoundary2 = 
    new GeomFill_SimpleBound(adapHCurve2,Tol,dummy);

  // define and initilization ConstrainedFilling
  GeomFill_ConstrainedFilling aFCF(MaxDeg,MaxSeg);
  aFCF.Init(myBoundary,myBoundary1,myBoundary2); 
  // make BSpline Surface
  Handle(Geom_BSplineSurface) aBSplineSurf = aFCF.Surface();

  // output curves and surface
  drawCurve(aBSCurve);
  if(WAIT_A_LITTLE) return;
  drawCurve(aBSCurve1);
  if(WAIT_A_LITTLE) return;
  drawCurve(aBezierCurve);
  if(WAIT_A_SECOND) return;
  drawSurface(aBSplineSurf);
}