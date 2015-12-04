// LProps_Presentation.cpp: implementation of the LProps_Presentation class.
// Presentation class: Local properties of curves and surfaces
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LProps_Presentation.h"

#include <Quantity_Color.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <GeomLProp_CLProps.hxx>
#include <Precision.hxx>
#include <Geom_Circle.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomLProp_SLProps.hxx>


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new LProps_Presentation;

// Initialization of array of samples
const LProps_Presentation::PSampleFuncType LProps_Presentation::SampleFuncs[] =
{
  &LProps_Presentation::sampleBezier,
  &LProps_Presentation::samplePBSpline,
  &LProps_Presentation::sampleBezierSurface
};

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

#define ASTR(_val)  TCollection_AsciiString(_val)
#define PRINT_COORDS(_p) ASTR((_p).X()) + " " + ASTR((_p).Y()) + " " + ASTR((_p).Z())

#define D1Color   Quantity_Color(Quantity_NOC_DARKOLIVEGREEN4)
#define D2Color   Quantity_Color(Quantity_NOC_DARKOLIVEGREEN)
#define D3Color   Quantity_Color(Quantity_NOC_DARKGREEN)
#define TanColor  Quantity_Color(Quantity_NOC_GREEN)
#define NormColor Quantity_Color(Quantity_NOC_CYAN4)
#define CrvtColor Quantity_Color(Quantity_NOC_BLUE1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LProps_Presentation::LProps_Presentation()
{
  FitMode=false;
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Local Properties of Curves and Surfaces");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void LProps_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

void LProps_Presentation::sampleBezier()
{
  Standard_CString aName = "BezierCurve";
  // Create a BezierCurve
  Standard_Real aCoords[][3] = {
    {0,0,0},{0,1,0},{1,1,0.2},{1,2,0.2},{2,2,0.4},{2,1,0.4},{3,1,0.6},{3,0,0.6},
    {2,0,0.8},{2,-1,0},{3,-1,0},{3,-2,-0.5},{4,-2,1},{4,-1,1.2},{5,-1,1.2},
    {5,0,1.4},{6,0,1.4},{6,-1,1.6},{7,-1,1.6},{7,0,1.8},{8,0,1.8},{8,1,2}
  };
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  TColgp_Array1OfPnt aPoles (1, nPoles);
  for (Standard_Integer i=0; i < nPoles; i++)
    aPoles(i+1) = gp_Pnt (aCoords[i][0]*100-500, aCoords[i][1]*100, aCoords[i][2]*100);
  Handle(Geom_BezierCurve) aCurve = new Geom_BezierCurve (aPoles);

  Standard_Real aPoints[] = {0, 0.1, 0.4, 0.6, 0.9};
  showCurveLProps (aCurve, aName, sizeof(aPoints)/sizeof(Standard_Real), aPoints);
}

void LProps_Presentation::samplePBSpline()
{
  Standard_CString aName = "BSplineCurve";
  // Create a Periodic BSplineCurve
  Standard_Real aCoords[][3] = {
    {0,0,0},{0,1,0},{1,1,0},{1,0,0},{2,0,-0.3},{2,-1,-0.3},{1,-1,0},
    {1,-2,0},{0,-2,0},{0,-1,0},{-1,-1,0.3},{-1,0,0.3}
  };
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  TColgp_Array1OfPnt aPoles (1, nPoles);
  TColStd_Array1OfReal aKnots (1, nPoles+1);
  TColStd_Array1OfInteger aMults(1, nPoles+1);
  for (Standard_Integer i=0; i < nPoles; i++)
    aPoles(i+1) = gp_Pnt (aCoords[i][0]*200, aCoords[i][1]*200, aCoords[i][2]*200);
  for (i=0; i < aKnots.Length(); i++)
    aKnots(i+1) = Standard_Real(i)/aKnots.Length();
  aMults.Init(1);
  Standard_Integer aDegree = 3;
  Standard_Boolean isPeriodic = Standard_True;
  Handle(Geom_BSplineCurve) aCurve = 
    new Geom_BSplineCurve (aPoles, aKnots, aMults, aDegree, isPeriodic);

  Standard_Real aPoints[] = {0, 0.1, 0.4, 0.6, 0.8};
  showCurveLProps (aCurve, aName, sizeof(aPoints)/sizeof(Standard_Real), aPoints);
}

void LProps_Presentation::sampleBezierSurface()
{
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

  Standard_Real aPoints[][2] = {{0.1,0}, {0.8,0.1}, {0.4,0.3}, {0.6,0.9}, {0.9,0.98}};
  showSurfaceLProps (aSurface, aName, 
    sizeof(aPoints)/(sizeof(Standard_Real)*2), aPoints);
}

void LProps_Presentation::showCurveLProps (Handle(Geom_Curve) theCurve,
                                           const Standard_CString theName,
                                           const Standard_Integer theNbPoints,
                                           const Standard_Real thePoints[])
{
  TCollection_AsciiString aTitle ("Local properties of a ");
  aTitle += theName;
  TCollection_AsciiString aText;
  aText = aText +
    "  // Create a " + theName + EOL
    "  Handle(Geom_" + theName + ") aCurve;" EOL
    "  // initialize aCurve" EOL
    "  // aCurve = ..." EOL EOL

    "  // define parameter at which properties should be computed" EOL
    "  Standard_Real aParam;" EOL
    "  // aParam = ..." EOL EOL

    "  // compute local properties" EOL
    "  Standard_Integer maxOrder = 3;" EOL
    "  Standard_Real aResol = gp::Resolution();" EOL
    "  GeomLProp_CLProps aLProps (theCurve, maxOrder, aResol);" EOL
    "  aLProps.SetParameter (aParam);" EOL
    "  gp_Pnt aPnt = aLProps.Value();" EOL
    "  gp_Vec aVecD1 = aLProps.D1();" EOL
    "  gp_Vec aVecD2 = aLProps.D2();" EOL
    "  gp_Vec aVecD3 = aLProps.D3();" EOL
    "  gp_Dir aTangent, aNormal;" EOL
    "  Standard_Real aCurvature;" EOL
    "  gp_Pnt aCentreOfCurvature;" EOL
    "  Standard_Boolean isCurvatureValid = Standard_False;" EOL
    "  Standard_Boolean isTan = aLProps.IsTangentDefined();" EOL
    "  // the tangent must be defined" EOL
    "  // to compute the curvature and the normal" EOL
    "  if (isTan)" EOL
    "  {" EOL
    "    aLProps.Tangent (aTangent);" EOL
    "    aCurvature = aLProps.Curvature();" EOL
    "    // the curvature must be non-null and finite" EOL
    "    // to compute the centre of curvature and the normal" EOL
    "    if (aCurvature > aResol && !Precision::IsInfinite(aCurvature))" EOL
    "    {" EOL
    "      isCurvatureValid = Standard_True;" EOL
    "      aLProps.CentreOfCurvature (aCentreOfCurvature);" EOL
    "      aLProps.Normal (aNormal);" EOL
    "    }" EOL
    "  }" EOL EOL
    "//======================================" EOL;
  setResultTitle (aTitle.ToCString());
  setResultText (aText.ToCString());

  // Display theCurve
  drawCurve (theCurve);

  Standard_Integer maxOrder = 3;
  Standard_Real aResol = gp::Resolution();
  GeomLProp_CLProps aLProps (theCurve, maxOrder, aResol);
  Handle(AIS_InteractiveObject) aObjs[7];
  for (int i=0; i < theNbPoints; i++)
  {
    if (WAIT_A_LITTLE)
      return;
    for (int j=0; j < sizeof(aObjs)/sizeof(Handle(AIS_InteractiveObject)); j++)
      if (!aObjs[j].IsNull())
        getAISContext()->Erase(aObjs[j]);

    // define parameter at which properties should be computed
    Standard_Real aParam;
    aParam = thePoints[i];

    // compute local properties
    aLProps.SetParameter (aParam);
    gp_Pnt aPnt = aLProps.Value();
    gp_Vec aVecD1 = aLProps.D1();
    gp_Vec aVecD2 = aLProps.D2();
    gp_Vec aVecD3 = aLProps.D3();
    gp_Dir aTangent, aNormal;
    Standard_Real aCurvature;
    gp_Pnt aCentreOfCurvature;
    Standard_Boolean isCurvatureValid = Standard_False;
    Standard_Boolean isTan = aLProps.IsTangentDefined();
    // the tangent must be defined
    // to compute the curvature and the normal
    if (isTan)
    {
      aLProps.Tangent (aTangent);
      aCurvature = aLProps.Curvature();
      // the curvature must be non-null and finite
      // to compute the centre of curvature and the normal
      if (aCurvature > aResol && !Precision::IsInfinite(aCurvature))
      {
        isCurvatureValid = Standard_True;
        aLProps.CentreOfCurvature (aCentreOfCurvature);
        aLProps.Normal (aNormal);
      }
    }

    // show results
    aText += EOL " Results with parameter ";
    aText += ASTR(thePoints[i]) + " :" EOL
      "aPnt = (" + PRINT_COORDS(aPnt) + ")" EOL
      "aVecD1 = (" + PRINT_COORDS(aVecD1) + ")" EOL
      "aVecD2 = (" + PRINT_COORDS(aVecD2) + ")" EOL
      "aVecD3 = (" + PRINT_COORDS(aVecD3) + ")" EOL;
    if (isTan)
    {
      aText = aText +
        "aTangent = (" + PRINT_COORDS(aTangent) + ")" EOL
        "aCurvature = " + ASTR(aCurvature) + EOL;
      if (isCurvatureValid)
      {
        aText = aText +
          "aCentreOfCurvature = (" + PRINT_COORDS(aCentreOfCurvature) + ")" EOL
          "aNormal = (" + PRINT_COORDS(aNormal) + ")" EOL;
      }
    }
    else
      aText += "Tangent is not defined" EOL;
    setResultText (aText.ToCString());

    // draw objects
    aObjs[0] = drawPoint (aPnt);
    aObjs[3] = drawVector (aPnt, aVecD3, D3Color);
    aObjs[2] = drawVector (aPnt, aVecD2, D2Color);
    aObjs[1] = drawVector (aPnt, aVecD1, D1Color);
    if (isTan)
      aObjs[4] = drawVector (aPnt, gp_Vec(aTangent)*50, TanColor);
    if (isCurvatureValid)
    {
      aObjs[5] = drawVector (aPnt, gp_Vec(aNormal)*50, NormColor);
      Handle(Geom_Circle) aCircle =
        new Geom_Circle (gp_Ax2 (aCentreOfCurvature, aNormal^aTangent), 
                         aCentreOfCurvature.Distance(aPnt));
      aObjs[6] = drawCurve (aCircle, CrvtColor);
    }
  }
}

void LProps_Presentation::showSurfaceLProps (Handle(Geom_Surface) theSurface,
                                             const Standard_CString theName,
                                             const Standard_Integer theNbPoints,
                                             const Standard_Real thePoints[][2])
{
  TCollection_AsciiString aTitle ("Local properties of a ");
  aTitle += theName;
  TCollection_AsciiString aText;
  aText = aText +
    "  // Create a " + theName + EOL
    "  Handle(Geom_" + theName + ") aSurface;" EOL
    "  // initialize aSurface" EOL
    "  // aSurface = ..." EOL EOL

    "  // define U and V parameters at which properties should be computed" EOL
    "  Standard_Real aUParam, aVParam;" EOL
    "  // aUParam = ..." EOL
    "  // aVParam = ..." EOL EOL

    "  // compute local properties" EOL
    "  Standard_Integer maxOrder = 2;" EOL
    "  Standard_Real aResol = gp::Resolution();" EOL
    "  GeomLProp_SLProps aLProps (theSurface, maxOrder, aResol);" EOL
    "  aLProps.SetParameters (aUParam, aVParam);" EOL
    "  gp_Pnt aPnt = aLProps.Value();" EOL
    "  gp_Vec aVecD1U = aLProps.D1U();" EOL
    "  gp_Vec aVecD1V = aLProps.D1V();" EOL
    "  gp_Vec aVecD2U = aLProps.D2U();" EOL
    "  gp_Vec aVecD2V = aLProps.D2V();" EOL
    "  gp_Vec aVecDUV = aLProps.DUV();" EOL
    "  gp_Dir aTangentU, aTangentV, aNormal, aMaxCurvD, aMinCurvD;" EOL
    "  Standard_Real aMaxCurvature, aMinCurvature, aMeanCurvature, aGausCurvature;" EOL
    "  // determine availability of properties" EOL
    "  Standard_Boolean isTanU = aLProps.IsTangentUDefined();" EOL
    "  Standard_Boolean isTanV = aLProps.IsTangentVDefined();" EOL
    "  Standard_Boolean isNormal = aLProps.IsNormalDefined();" EOL
    "  Standard_Boolean isCurvature = aLProps.IsCurvatureDefined();" EOL
    "  if (isTanU)" EOL
    "    aLProps.TangentU (aTangentU);" EOL
    "  if (isTanV)" EOL
    "    aLProps.TangentV (aTangentV);" EOL
    "  if (isNormal)" EOL
    "    aNormal = aLProps.Normal();" EOL
    "  if (isCurvature)" EOL
    "  {" EOL
    "    aMaxCurvature = aLProps.MaxCurvature();" EOL
    "    aMinCurvature = aLProps.MinCurvature();" EOL
    "    aGausCurvature = aLProps.GaussianCurvature();" EOL
    "    aMeanCurvature = aLProps.MeanCurvature();" EOL
    "    aLProps.CurvatureDirections (aMaxCurvD, aMinCurvD);" EOL
    "  }" EOL EOL
    "//======================================" EOL;
  setResultTitle (aTitle.ToCString());
  setResultText (aText.ToCString());

  // Display theSurface
  drawSurface (theSurface);

  Standard_Integer maxOrder = 2;
  Standard_Real aResol = gp::Resolution();
  GeomLProp_SLProps aLProps (theSurface, maxOrder, aResol);
  Handle(AIS_InteractiveObject) aObjs[11];
  for (int i=0; i < theNbPoints; i++)
  {
    if (WAIT_A_LITTLE)
      return;
    for (int j=0; j < sizeof(aObjs)/sizeof(Handle(AIS_InteractiveObject)); j++)
      if (!aObjs[j].IsNull())
        getAISContext()->Erase(aObjs[j]);

    // define U and V parameters at which properties should be computed
    Standard_Real aUParam, aVParam;
    aUParam = thePoints[i][0];
    aVParam = thePoints[i][1];

    // compute local properties
    aLProps.SetParameters (aUParam, aVParam);
    gp_Pnt aPnt = aLProps.Value();
    gp_Vec aVecD1U = aLProps.D1U();
    gp_Vec aVecD1V = aLProps.D1V();
    gp_Vec aVecD2U = aLProps.D2U();
    gp_Vec aVecD2V = aLProps.D2V();
    gp_Vec aVecDUV = aLProps.DUV();
    gp_Dir aTangentU, aTangentV, aNormal, aMaxCurvD, aMinCurvD;
    Standard_Real aMaxCurvature, aMinCurvature, aMeanCurvature, aGausCurvature;
    // determine availability of properties
    Standard_Boolean isTanU = aLProps.IsTangentUDefined();
    Standard_Boolean isTanV = aLProps.IsTangentVDefined();
    Standard_Boolean isNormal = aLProps.IsNormalDefined();
    Standard_Boolean isCurvature = aLProps.IsCurvatureDefined();
    if (isTanU)
      aLProps.TangentU (aTangentU);
    if (isTanV)
      aLProps.TangentV (aTangentV);
    if (isNormal)
      aNormal = aLProps.Normal();
    if (isCurvature)
    {
      aMaxCurvature = aLProps.MaxCurvature();
      aMinCurvature = aLProps.MinCurvature();
      aGausCurvature = aLProps.GaussianCurvature();
      aMeanCurvature = aLProps.MeanCurvature();
      aLProps.CurvatureDirections (aMaxCurvD, aMinCurvD);
    }

    // show results
    aText = aText + EOL " Results with parameters "
      "U=" + ASTR(thePoints[i][0]) + " V=" + ASTR(thePoints[i][1]) + " :" EOL
      "aPnt = (" + PRINT_COORDS(aPnt) + ")" EOL
      "aVecD1U = (" + PRINT_COORDS(aVecD1U) + ")" EOL
      "aVecD1V = (" + PRINT_COORDS(aVecD1V) + ")" EOL
      "aVecD2U = (" + PRINT_COORDS(aVecD2U) + ")" EOL
      "aVecD2V = (" + PRINT_COORDS(aVecD2V) + ")" EOL
      "aVecDUV = (" + PRINT_COORDS(aVecDUV) + ")" EOL;
    if (isTanU)
      aText = aText +
        "aTangentU = (" + PRINT_COORDS(aTangentU) + ")" EOL;
    if (isTanV)
      aText = aText +
        "aTangentV = (" + PRINT_COORDS(aTangentV) + ")" EOL;
    if (isNormal)
      aText = aText +
        "aNormal = (" + PRINT_COORDS(aNormal) + ")" EOL;
    if (isCurvature)
      aText = aText +
        "aMaxCurvature = " + ASTR(aMaxCurvature) + EOL
        "aMinCurvature = " + ASTR(aMinCurvature) + EOL
        "aMeanCurvature = " + ASTR(aMeanCurvature) + EOL
        "aGausCurvature = " + ASTR(aGausCurvature) + EOL
        "aMaxCurvD = (" + PRINT_COORDS(aMaxCurvD) + ")" EOL
        "aMinCurvD = (" + PRINT_COORDS(aMinCurvD) + ")" EOL;
    setResultText (aText.ToCString());

    // draw objects
    aObjs[0] = drawPoint (aPnt);
    aObjs[3] = drawVector (aPnt, aVecD2U, D2Color);
    aObjs[4] = drawVector (aPnt, aVecD2V, D2Color);
    aObjs[1] = drawVector (aPnt, aVecD1U, D1Color);
    aObjs[2] = drawVector (aPnt, aVecD1V, D1Color);
    aObjs[5] = drawVector (aPnt, aVecDUV, D3Color);
    if (isTanU)
      aObjs[6] = drawVector (aPnt, gp_Vec(aTangentU)*50, TanColor);
    if (isTanV)
      aObjs[7] = drawVector (aPnt, gp_Vec(aTangentV)*50, TanColor);
    if (isNormal)
      aObjs[8] = drawVector (aPnt, gp_Vec(aNormal)*50, NormColor);
    if (isCurvature)
    {
      aObjs[9] = drawVector (aPnt, gp_Vec(aMaxCurvD)*50, CrvtColor);
      aObjs[10] = drawVector (aPnt, gp_Vec(aMinCurvD)*50, CrvtColor);
    }
  }
}
