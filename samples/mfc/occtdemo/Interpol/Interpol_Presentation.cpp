// Interpol_Presentation.cpp: implementation of the Interpol_Presentation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Interpol_Presentation.h"

#include <gp_Pnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Precision.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Vec.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Interpol_Presentation;

// Initialization of array of samples
const Interpol_Presentation::PSampleFuncType Interpol_Presentation::SampleFuncs[] =
{
  &Interpol_Presentation::sample1,
  &Interpol_Presentation::sample2,
  &Interpol_Presentation::sample3,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Interpol_Presentation::Interpol_Presentation()
{
  myIndex = 0;
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Interpolation of a set of points to form a curve");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Interpol_Presentation::DoSample()
{
  setResultTitle("Interpolation");
  getAISContext()->EraseAll();

  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}


//================================================================
// Function : calculateNewTangent
// Purpose  : finds a tangent at the given point on the given curve,
//  rotate it by <angle> in the plane of the tangent and normal to
//  the curve in this point.
// Output   : vectors theTan, theNewTan.
// Returns  : false if failed, true if ok.
//================================================================
static Standard_Boolean calculateNewTangent(const Handle(Geom_BSplineCurve)& anInterpolationCurve, 
                                            gp_Pnt aPointOnCurve, gp_Vec& theTan, gp_Vec& theNewTan,
                                            Standard_Real angle)
{
  GeomLProp_CLProps aLProps (anInterpolationCurve, 2, Precision::Confusion());
  GeomAPI_ProjectPointOnCurve aProjector(aPointOnCurve, anInterpolationCurve);
  aLProps.SetParameter (aProjector.LowerDistanceParameter());
  gp_Dir aTangent, aNormal;
  // the tangent and must be defined to compute the normal
  if (aLProps.IsTangentDefined())
  {
    aLProps.Tangent (aTangent);
    Standard_Real aCurvature = aLProps.Curvature();
    // the curvature must be non-null and finite to compute the normal
    if (aCurvature > Precision::Confusion() && !Precision::IsInfinite(aCurvature))
      aLProps.Normal (aNormal);
    else return Standard_False;
  }
  else return Standard_False;

  gp_Dir aPlaneNormal(aTangent.XYZ() ^ aNormal.XYZ());
  gp_Ax1 anAx(aPointOnCurve, aPlaneNormal);
  theTan = aTangent; 
  theNewTan = theTan.Rotated(anAx, angle);

  return Standard_True;
}


//================================================================
// Function : interpolate
// Purpose  : builds interpolation curve using given Pnts 
//================================================================
void Interpol_Presentation::interpolate (Standard_Real aCoords[][3],
                                         Standard_Integer nPnts,
                                         TCollection_AsciiString& theText,
                                         const TColStd_Array1OfInteger& indexes,
                                         const TColStd_Array1OfReal& angles)
{
  theText += EOL
    "  Standard_Integer nPnts = sizeof(aCoords)/(sizeof(Standard_Real)*3);" EOL
    "  Handle(TColgp_HArray1OfPnt) aPnts = new TColgp_HArray1OfPnt(1, nPnts);" EOL
    "  for (Standard_Integer i=0; i < nPnts; i++)" EOL
    "  {" EOL
    "    gp_Pnt aPnt(aCoords[i][0]*75-400, aCoords[i][1]*75-400, aCoords[i][2]*75-400);" EOL
    "    aPnts->SetValue(i+1, aPnt);" EOL
    "    drawPoint(aPnt);" EOL
    "    if (WaitForInput(250)) return;" EOL
    "  }" EOL EOL

    "  //=====================================" EOL
    "  // Creating a non periodic interpolation curve without constraints " EOL
    "  // (type of resulting curve is BSpline)" EOL
    "  Standard_Boolean isPeriodic = Standard_False;" EOL
    "  GeomAPI_Interpolate aNoPeriodInterpolate(aPnts, isPeriodic, Precision::Confusion());" EOL
    "  aNoPeriodInterpolate.Perform();" EOL
    "  // check results" EOL
    "  if (!aNoPeriodInterpolate.IsDone()) return;" EOL
    "  Handle(Geom_BSplineCurve) anInterpolationCurve = aNoPeriodInterpolate.Curve();" EOL EOL

    "  //===================================" EOL
    "  // Creating a constrained interpolation curve" EOL
    "  TColgp_Array1OfVec aConstraints(1, nPnts);" EOL
    "  Handle(TColStd_HArray1OfBoolean) aFlags = new TColStd_HArray1OfBoolean(1, nPnts);" EOL
    "  Handle(TColStd_HArray1OfBoolean) aCopyFlags = new TColStd_HArray1OfBoolean(1, nPnts);" EOL
    "  Standard_Boolean toScale = Standard_False;" EOL EOL

    "  // some code initializing arrays aConstraints and aCopyFlags" EOL
    "  aCopyFlags->Init(Standard_False);" EOL
    "  aCopyFlags->SetValue(1, Standard_True);" EOL
    "  aConstraints(1) = aTanVec;" EOL
    "  // ..." EOL
    "  //" EOL EOL

    "  // Loading constraints and perform" EOL
    "  aFlags->ChangeArray1() = aCopyFlags->Array1();" EOL
    "  aNoPeriodInterpolate.Load(aConstraints, aFlags, toScale);" EOL
    "  aNoPeriodInterpolate.Perform();" EOL
    "  // check results" EOL
    "  if (!aNoPeriodInterpolate.IsDone()) return;" EOL
    "  anInterpolationCurve = aNoPeriodInterpolate.Curve();" EOL EOL

    "  //==================================================" EOL
    "  // Creating a periodic interpolation curve" EOL
    "  isPeriodic = Standard_True;" EOL
    "  GeomAPI_Interpolate aPeriodInterpolate(aPnts, isPeriodic, Precision::Confusion());" EOL
    "  // Loading constraints and perform" EOL
    "  aFlags->ChangeArray1() = aCopyFlags->Array1();" EOL
    "  aPeriodInterpolate.Load(aConstraints, aFlags, toScale);" EOL
    "  aPeriodInterpolate.Perform();" EOL
    "  // check results" EOL
    "  if (!aPeriodInterpolate.IsDone()) return;" EOL
    "  anInterpolationCurve = aPeriodInterpolate.Curve();" EOL;

  setResultText(theText.ToCString());

  Handle(TColgp_HArray1OfPnt) aPnts = new TColgp_HArray1OfPnt(1, nPnts);
  for (Standard_Integer i=0; i < nPnts; i++)
  {
    gp_Pnt aPnt(aCoords[i][0]*75-400, aCoords[i][1]*75-400, aCoords[i][2]*75-400);
    aPnts->SetValue(i+1, aPnt);
    drawPoint(aPnt);
    if (WaitForInput(250)) return;
  }
  
  //=====================================
  // Creating a non periodic interpolation curve without constraints 
  // (type of resulting curve is BSpline)
  Standard_Boolean isPeriodic = Standard_False;
  GeomAPI_Interpolate aNoPeriodInterpolate(aPnts, isPeriodic, Precision::Confusion());
  aNoPeriodInterpolate.Perform();
  // check results
  if (!aNoPeriodInterpolate.IsDone()) return;
  Handle(Geom_BSplineCurve) anInterpolationCurve = aNoPeriodInterpolate.Curve();

  Handle(AIS_InteractiveObject) aShowCurve = drawCurve(anInterpolationCurve);
  if (WAIT_A_SECOND) return;
     
  //===================================
  // Creating several constrained interpolation curves
  TColgp_Array1OfVec aConstraints(1, nPnts);
  Handle(TColStd_HArray1OfBoolean) aFlags = new TColStd_HArray1OfBoolean(1, nPnts);
  Handle(TColStd_HArray1OfBoolean) aCopyFlags = new TColStd_HArray1OfBoolean(1, nPnts);
  Standard_Boolean toScale = Standard_False;
  Standard_Real scaleVec = 1;
  aCopyFlags->Init(Standard_False);

  Handle(AIS_InteractiveObject) aShowTan;
  for (i = indexes.Lower(); i <= indexes.Upper(); i++)
  {
    gp_Pnt aPoint = aPnts->Value(indexes(i));
    Standard_Real anAngle = angles(i);

    gp_Vec aTan, aNewTan;
    if (!calculateNewTangent(anInterpolationCurve, aPoint, aTan, aNewTan, anAngle))
      return;

    if (!aShowTan.IsNull()) getAISContext()->Erase(aShowTan);
    aShowTan = drawVector(aPoint, aTan.Scaled(300));
    if (WAIT_A_LITTLE) return;

    aCopyFlags->SetValue(indexes(i), Standard_True);
    aConstraints(indexes(i)) = aNewTan * scaleVec;

    aFlags->ChangeArray1() = aCopyFlags->Array1();
    aNoPeriodInterpolate.Load(aConstraints, aFlags, toScale);
    aNoPeriodInterpolate.Perform();

    if (!aNoPeriodInterpolate.IsDone()) return;

    // replacing the curve with a new one, so calculation of the next tagent
    // on the next point from indexes will be done using the new curve
    anInterpolationCurve = aNoPeriodInterpolate.Curve();

    // display a new curve and new vector
    getAISContext()->Erase(aShowTan);
    getAISContext()->Erase(aShowCurve);
    aShowCurve = drawCurve(anInterpolationCurve);
    aShowTan = drawVector(aPoint, aNewTan.Scaled(300));

    if (WAIT_A_SECOND) return;
  }

  //==================================================
  // Creating a periodic interpolation curve
  isPeriodic = Standard_True;
  GeomAPI_Interpolate aPeriodInterpolate(aPnts, isPeriodic, Precision::Confusion());

  aFlags->ChangeArray1() = aCopyFlags->Array1();
  aPeriodInterpolate.Load(aConstraints, aFlags, toScale);
  aPeriodInterpolate.Perform();

  // the interpolation can not be computed
  if (!aPeriodInterpolate.IsDone()) return;

  anInterpolationCurve = aPeriodInterpolate.Curve();

  getAISContext()->Erase(aShowTan);
  getAISContext()->Erase(aShowCurve);
  drawCurve(anInterpolationCurve);
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////
//================================================================
// Function : Interpol_Presentation::sample1
// Purpose  : 
//================================================================
void Interpol_Presentation::sample1()
{
  ResetView();
  SetViewCenter(-43,-333);
  SetViewScale(1.4210);
  
  TCollection_AsciiString aText(
    "  // initializing array of points to interpolate" EOL
    "  Standard_Real aCoords[][3] = {{0,0,0},{3,5,0},{4,7,0},{10,10,0}};" EOL
  );

  // initializing array of interpolation curve Pnts
  Standard_Real aCoords[][3] = {{0,0,0},{3,5,0},{4,7,0},{10,10,0}};

  // initializing array of indexes of points where the constraints will be applied
  TColStd_Array1OfInteger indexes(1,2);
  indexes(1) = 1; 
  indexes(2) = 2;

  // initializing array of angles of tagent rotation in the points
  // with indexes from the array defined above.
  TColStd_Array1OfReal aAngles(1,2);
  aAngles(1) = PI/2; 
  aAngles(2) = -PI/4;
  
  interpolate(aCoords, sizeof(aCoords)/(sizeof(Standard_Real)*3), 
    aText, indexes, aAngles);
}


//================================================================
// Function : Interpol_Presentation::sample2
// Purpose  : 
//================================================================
void Interpol_Presentation::sample2()
{
  ResetView();
  SetViewCenter(-148.74,-603.06);
  SetViewScale(1.09);

  TCollection_AsciiString aText(
    "  // initializing array of points to interpolate" EOL
    "  Standard_Real aCoords[][3] = {{0,-1,0},{1,1,0},{4,5,0},{9,0,0},{13,3,0},{16,-5,0}};" EOL
  );

  // initializing array of interpolation curve Pnts
  Standard_Real aCoords[][3] = {{0,-1,0},{1,1,0},{4,5,0},{9,0,0},{13,3,0},{16,-5,0}};

  // initializing array of indexes of points where the constraints will be applied
  TColStd_Array1OfInteger indexes(1,3);
  indexes(1) = 1; 
  indexes(2) = 2; 
  indexes(3) = 6;

  // initializing array of angles of tagent rotation in the points
  // with indexes from the array defined above.
  TColStd_Array1OfReal aAngles(1,3);
  aAngles(1) = -PI/4; 
  aAngles(2) = -PI/3; 
  aAngles(3) = +PI/3;
  
  interpolate(aCoords, sizeof(aCoords)/(sizeof(Standard_Real)*3), 
    aText, indexes, aAngles);
}


//================================================================
// Function : Interpol_Presentation::sample3
// Purpose  : 
//================================================================
void Interpol_Presentation::sample3()
{
  ResetView();
  SetViewCenter(374.27539,185.40618);
  SetViewScale(0.87185);


	TCollection_AsciiString aText(
    "  // initializing array of points to interpolate" EOL
    "  Standard_Real aCoords[][3] = {{10,0,11},{13,15,10},{7,13,12},{10,0,5}};" EOL
  );

  // initializing array of interpolation curve Pnts
  Standard_Real aCoords[][3] = {{10,0,11},{13,15,10},{7,13,12},{5,5,9}};
  TColStd_Array1OfInteger indexes(1,2);
  indexes(1) = 2; indexes(2) = 4;
  TColStd_Array1OfReal aAngles(1,2);
  aAngles(1) = PI/4; aAngles(2) = -PI/3;
  
  interpolate(aCoords, sizeof(aCoords)/(sizeof(Standard_Real)*3), 
    aText, indexes, aAngles);
}
