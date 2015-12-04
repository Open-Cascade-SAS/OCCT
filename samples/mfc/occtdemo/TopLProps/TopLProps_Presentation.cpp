// TopLProps_Presentation.cpp: implementation of the TopLProps_Presentation class.
// Determine the local properties of shapes.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TopLProps_Presentation.h" 

#include <Precision.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_Circle.hxx>
#include <GC_MakeSegment.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLProp_CLProps.hxx>
#include <BRepLProp_SLProps.hxx>


#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

#define SCALE 70
#define ASTR(_val)  TCollection_AsciiString(_val)
#define PRINT_COORDS(_p) ASTR((_p).X()) + " " + ASTR((_p).Y()) + " " + ASTR((_p).Z())

#define D1Color   Quantity_Color(Quantity_NOC_DARKOLIVEGREEN4)
#define D2Color   Quantity_Color(Quantity_NOC_DARKOLIVEGREEN)
#define D3Color   Quantity_Color(Quantity_NOC_DARKGREEN)
#define TanColor  Quantity_Color(Quantity_NOC_GREEN)
#define NormColor Quantity_Color(Quantity_NOC_CYAN4)
#define CrvtColor Quantity_Color(Quantity_NOC_BLUE1)


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new TopLProps_Presentation;

// Initialization of array of samples
const TopLProps_Presentation::PSampleFuncType TopLProps_Presentation::SampleFuncs[] =
{
  &TopLProps_Presentation::sample1,
  &TopLProps_Presentation::sample2,
  &TopLProps_Presentation::sample3,
  &TopLProps_Presentation::sample4
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TopLProps_Presentation::TopLProps_Presentation()
{
  myIndex = 0;
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Local Properties of Shapes");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void TopLProps_Presentation::DoSample()
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
// Function : CreateRevolShape
// Purpose  : 
//================================================================

static TopoDS_Shape CreateRevolShape()
{
  Standard_Real aCoords1[][3] = 
  {
    {0,-8,2},{0,-7.5,1},{0,-7,0.5},{0,-6.5,1.5},{0,-6,2},{0,-5,2.5},{0,-4,2.8},
    {0,-3,2.6},{0,-2,2.4},{0,-1,2},{0,-0.5,1.5},{0,0,1.2}
  };
  Standard_Real aCoords2[][3] =
  {
    {0,0,1.2},{0,0.5,1},{0,1,0.8},{0,2,1.2},{0,3,1.5},{0,4,2},{0,4.5,2.7},{0,5,3}
  };

  Standard_Integer nPoles1 = sizeof(aCoords1)/(sizeof(Standard_Real)*3);
  Standard_Integer nPoles2 = sizeof(aCoords2)/(sizeof(Standard_Real)*3);

  TColgp_Array1OfPnt aCurvePoint1 (1, nPoles1);
  TColgp_Array1OfPnt aCurvePoint2 (1, nPoles2);

  for (Standard_Integer i=0; i < nPoles1; i++)
    aCurvePoint1(i+1) = gp_Pnt (aCoords1[i][0]*SCALE, aCoords1[i][1]*SCALE, aCoords1[i][2]*SCALE);
  for ( i=0; i < nPoles2; i++)
    aCurvePoint2(i+1) = gp_Pnt (aCoords2[i][0]*SCALE, aCoords2[i][1]*SCALE, aCoords2[i][2]*SCALE);

  GeomAPI_PointsToBSpline aPTB1 (aCurvePoint1,3,10,GeomAbs_C3,0.3*SCALE);
  Handle (Geom_BSplineCurve) aCurve1 = aPTB1.Curve();
  GeomAPI_PointsToBSpline aPTB2 (aCurvePoint2,3,10,GeomAbs_C3,0.3*SCALE);
  Handle (Geom_BSplineCurve) aCurve2 = aPTB2.Curve();

  gp_Ax1 anAxis = gp_Ax1(gp_Pnt(0,0,0),gp::DY());
  GC_MakeSegment aSegment(gp_Pnt(0,-8*SCALE,0),gp_Pnt(0,-8*SCALE,2*SCALE));
  Handle(Geom_TrimmedCurve) aLine = aSegment.Value();
  Handle(Geom_SurfaceOfRevolution) aSurface1 = new Geom_SurfaceOfRevolution(aLine, anAxis);
  Handle(Geom_SurfaceOfRevolution) aSurface2 = new Geom_SurfaceOfRevolution(aCurve1, anAxis);
  Handle(Geom_SurfaceOfRevolution) aSurface3 = new Geom_SurfaceOfRevolution(aCurve2, anAxis);
  
  BRep_Builder aBuilder;
  TopoDS_Compound aShape;
  aBuilder.MakeCompound(aShape);
  TopoDS_Face aFace1,aFace2,aFace3;
  aFace1 = BRepBuilderAPI_MakeFace(aSurface1);
  aFace2 = BRepBuilderAPI_MakeFace(aSurface2);
  aFace3 = BRepBuilderAPI_MakeFace(aSurface3);
  
  aBuilder.Add(aShape,aFace1);
  aBuilder.Add(aShape,aFace2);
  aBuilder.Add(aShape,aFace3);
  
  return aShape;
}

//================================================================
// Function : CreateBSplShape
// Purpose  : 
//================================================================

static TopoDS_Shape CreateBSplnShape()
{
  Standard_Real aZCoords1 [] = 
  {
    -0.3,1.2,0,-0.5,
    1.5,0.5,1.5,-1.5,
    1.5,0.5,1.5,-1.5,
    0.5,-0.5,0.5,-0.5
  };
  Standard_Real aZCoords2 [] = 
  {
    -0.3,1.2,0,-0.5,
    1.5,-1.5,-2,-1.5,
    1.5,-1.5,-2,-1.5,
    0.5,-0.5,0.5,-0.5
  };
  TColStd_Array2OfReal aZPoints1(1,4,1,4);
  TColStd_Array2OfReal aZPoints2(1,4,1,4);

  Standard_Integer aColLength1 = aZPoints1.ColLength();
  Standard_Integer aRowLength1 = aZPoints1.RowLength();
  Standard_Integer aColLength2 = aZPoints2.ColLength();
  Standard_Integer aRowLength2 = aZPoints2.RowLength();
  Standard_Integer aIndex = -1;
  
  for(Standard_Integer i = 0 ; i < aRowLength1 ; i++)
  {
    for(Standard_Integer j = 0; j < aColLength1 ; j++)
    {
      aIndex++;
      aZPoints1(i+1,j+1) = aZCoords1[aIndex];     
    }
  }

  aIndex = -1;
  for( i = 0 ; i < aRowLength2 ; i++)
  {
    for(Standard_Integer j = 0; j < aColLength2 ; j++)
    {
      aIndex++;
      aZPoints2(i+1,j+1) = aZCoords2[aIndex];     
    }
  }

  Standard_Real aXStep = 170, aYStep = 170;
  Standard_Real aX0 = -350, aY0 = -250;
  Standard_Real auxY0,auxX0 = aX0 - aXStep;

  for( i = 0 ; i < aColLength1 ; i++)
  {
    auxX0 += aXStep;
    auxY0 = aY0 - aYStep;

    for(Standard_Integer j = 0 ; j < aRowLength1 ; j++)
    {
      aZPoints1(i+1,j+1) *=2*SCALE; 
      aZPoints2(i+1,j+1) *=2*SCALE;
      auxY0 += aYStep;
    }
  }

  GeomAPI_PointsToBSplineSurface aPTBS;
  aPTBS.Init(aZPoints1,aX0,aXStep,aY0,aYStep,3,10,GeomAbs_C3,0.3*SCALE);
  Handle(Geom_BSplineSurface) aSurface1 = aPTBS.Surface();
  
  aPTBS.Init(aZPoints2,aX0,aXStep,aY0,aYStep,3,10,GeomAbs_C3,0.3*SCALE);
  Handle(Geom_BSplineSurface) aSurface2 = aPTBS.Surface();

  TopoDS_Face aFace1,aFace2;
  aFace1 = BRepBuilderAPI_MakeFace (aSurface1);
  aFace2 = BRepBuilderAPI_MakeFace (aSurface2);

  BRep_Builder aBuilder;
  TopoDS_Compound aShape;
  aBuilder.MakeCompound(aShape);
  aBuilder.Add(aShape,aFace1);
  aBuilder.Add(aShape,aFace2);

  return aShape;

}

//================================================================
// Function : TopLProps_Presentation::sample1
// Purpose  : 
//================================================================

void TopLProps_Presentation::sample1()
{
  TopoDS_Shape aShape = CreateRevolShape();

  // get aNumEdge-th edge
  int aNumEdge = 5;
  TopExp_Explorer anExp(aShape, TopAbs_EDGE);
  TopoDS_Edge aEdge;
  for (int i=1; anExp.More() && i <= aNumEdge; anExp.Next(), i++)
    aEdge = TopoDS::Edge(anExp.Current());
  if (aEdge.IsNull()) return;
  
  //show:
  Handle(AIS_InteractiveObject) aShowShape = drawShape(aShape);
  if(WAIT_A_SECOND) return;
  drawShape(aEdge,Quantity_NOC_RED);
  if(WAIT_A_SECOND) return;
  getAISContext()->Erase(aShowShape);
  if(WAIT_A_LITTLE) return;

  Standard_Real aPoints [] = { 0.1,0.5,0.7};
  Standard_Integer aNbPoints = sizeof(aPoints)/sizeof(Standard_Real);
  showEdgeLProps(aEdge,aNbPoints,aPoints);
}

//================================================================
// Function : TopLProps_Presentation::sample2
// Purpose  : 
//================================================================

void TopLProps_Presentation::sample2()
{
  TopoDS_Shape aShape = CreateRevolShape();

  // get aNumFace-th face
  int aNumFace = 3;
  TopExp_Explorer anExp(aShape, TopAbs_FACE);
  TopoDS_Face aFace;
  for (int i=1; anExp.More() && i <= aNumFace; anExp.Next(), i++)
    aFace = TopoDS::Face(anExp.Current());
  if (aFace.IsNull()) return;

  //show:
  Handle(AIS_InteractiveObject) aShowShape = drawShape(aShape);
  if(WAIT_A_SECOND) return;
  Handle(AIS_InteractiveObject) aShowFace = drawShape(aFace,Graphic3d_NOM_BRASS,Standard_False);
  getAISContext()->SetDisplayMode(aShowFace,AIS_WireFrame);
  getAISContext()->Display(aShowFace);
  if(WAIT_A_SECOND) return;
  getAISContext()->UnsetDisplayMode(aShowFace);
  getAISContext()->Erase(aShowShape);
  if(WAIT_A_LITTLE) return;

  Standard_Real aPoints [][2] = { {0.1,0.1},{0.7,0.3},{0.5,0.6} };
  Standard_Integer aNbPoints = sizeof(aPoints)/(sizeof(Standard_Real)*2);
  showFaceLProps(aFace,aNbPoints,aPoints);
}

//================================================================
// Function : TopLProps_Presentation::sample3
// Purpose  : 
//================================================================

void TopLProps_Presentation::sample3()
{
  TopoDS_Shape aShape = CreateBSplnShape();
  
  // get aNumEdge-th edge
  int aNumEdge = 1;
  TopExp_Explorer anExp(aShape, TopAbs_EDGE);
  TopoDS_Edge aEdge;
  for (int i=1; anExp.More() && i <= aNumEdge; anExp.Next(), i++)
    aEdge = TopoDS::Edge(anExp.Current());
  if (aEdge.IsNull()) return;
  
  Standard_Real aFirst,aLast;
  BRep_Tool::Range(aEdge,aFirst,aLast) ;

  Standard_Real P1,P2,P3;
  P1 = aFirst + (aLast-aFirst)*0.25;
  P2 = aFirst + (aLast-aFirst)*0.5;
  P3 = aFirst + (aLast-aFirst)*0.833;
  Standard_Real aPoints [] = {P1,P2,P3 };
  Standard_Integer aNbPoints = sizeof(aPoints)/sizeof(Standard_Real);

  //show:
  Handle(AIS_InteractiveObject) aShowShape = drawShape(aShape);
  if(WAIT_A_SECOND) return;

  drawShape(aEdge,Quantity_NOC_RED);
  if(WAIT_A_SECOND) return;
  getAISContext()->Erase(aShowShape);
  if(WAIT_A_LITTLE) return;

  showEdgeLProps(aEdge,aNbPoints,aPoints);
}

//================================================================
// Function : TopLProps_Presentation::sample4
// Purpose  : 
//================================================================

void TopLProps_Presentation::sample4()
{  
  TopoDS_Shape aShape = CreateBSplnShape();

  // get aNumFace-th face
  int aNumFace = 1;
  TopExp_Explorer anExp(aShape, TopAbs_FACE);
  TopoDS_Face aFace;
  for (int i=1; anExp.More() && i <= aNumFace; anExp.Next(), i++)
    aFace = TopoDS::Face(anExp.Current());
  if (aFace.IsNull()) return;

  Standard_Real UMin,UMax,VMin,VMax;
  BRepTools::UVBounds(aFace,UMin,UMax,VMin,VMax);
  Standard_Real U1 = UMin + (UMax-UMin)*0.769,
                U2 = UMin + (UMax-UMin)*0.833,
                U3 = UMin + (UMax-UMin)*0.333,
                V1 = VMin + (VMax-VMin)*0.333,
                V2 = VMin + (VMax-VMin)*0.5,
                V3 = VMin + (VMax-VMin)*0.667;
  Standard_Real aPoints [][2] =
  { 
    {U1,V1},{U2,V2},{U3,V3}
  };
  Standard_Integer aNbPoints = sizeof(aPoints)/(sizeof(Standard_Real)*2);

  //show:
  Handle(AIS_InteractiveObject) aShowShape = drawShape(aShape);
  if(WAIT_A_SECOND) return;
  Handle(AIS_InteractiveObject) aShowFace = drawShape(aFace,Graphic3d_NOM_BRASS,Standard_False);
  getAISContext()->SetDisplayMode(aShowFace,AIS_WireFrame);
  getAISContext()->Display(aShowFace);
  if(WAIT_A_SECOND) return;
  getAISContext()->UnsetDisplayMode(aShowFace);
  getAISContext()->Erase(aShowShape);
  if(WAIT_A_LITTLE) return;

  showFaceLProps(aFace,aNbPoints,aPoints);
}

//================================================================
// Function : TopLProps_Presentation::showEdgeLProps
// Purpose  : 
//================================================================

void TopLProps_Presentation::showEdgeLProps(TopoDS_Edge& theEdge,
                                            const Standard_Integer theNbPoints,
                                            const Standard_Real thePoints[])
{
  TCollection_AsciiString aTitle ("Local properties on edge");
  TCollection_AsciiString aText = 
    "  // Create an Edge" EOL
    "  TopoDS_Edge theEdge;" EOL
    "  // initialize theEdge" EOL
    "  // theEdge = ... ;" EOL EOL

    "  // define parameter at which properties should be computed" EOL
    "  Standard_Real aParam;" EOL
    "  // aParam = ..." EOL EOL

    "  // create an algorithm for computing the local properties" EOL
    "  // at a point on an edge" EOL
    "  BRepAdaptor_Curve anAdapCurve (theEdge) ;" EOL
    "  Standard_Integer maxOrder = 3;" EOL
    "  Standard_Real aResol = gp::Resolution();" EOL
    "  BRepLProp_CLProps aLProps (anAdapCurve,maxOrder,aResol);" EOL EOL

    "  // compute local properties" EOL
    "  aLProps.SetParameter (aParam);" EOL
    "  gp_Pnt aPnt = aLProps.Value();" EOL
    "  gp_Vec aVecD1 = aLProps.D1();" EOL
    "  gp_Vec aVecD2 = aLProps.D2();" EOL
    "  gp_Vec aVecD3 = aLProps.D3();" EOL
    "  gp_Dir aTangent, aNormal;" EOL
    "  Standard_Real aCurvature;" EOL
    "  gp_Pnt aCentreOfCurvature;" EOL
    "  Standard_Boolean isCurvatureValid = Standard_False;" EOL
    "  Standard_Boolean isTan = aLProps.IsTangentDefined();" EOL EOL

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

  BRepAdaptor_Curve anAdapCurve (theEdge) ;
  Handle(AIS_InteractiveObject) aObjs[7];
  Standard_Integer maxOrder = 3;
  Standard_Real aResol = gp::Resolution();
  BRepLProp_CLProps aLProps (anAdapCurve,maxOrder,aResol);
  for(Standard_Integer i = 0; i < theNbPoints ; i++)
  {
    if (WAIT_A_LITTLE) return;

    for (int j=0; j < sizeof(aObjs)/sizeof(Handle(AIS_InteractiveObject)); j++)
      if (!aObjs[j].IsNull())
        getAISContext()->Erase(aObjs[j]);

    // compute local properties
    aLProps.SetParameter (thePoints[i]);
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

    //show:
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

//================================================================
// Function : TopLProps_Presentation::showFaceLProps
// Purpose  : 
//================================================================

void TopLProps_Presentation::showFaceLProps(TopoDS_Face& theFace,
                                            const Standard_Integer theNbPoints,
                                            const Standard_Real thePoints[][2])
{
  TCollection_AsciiString aTitle ("Local properties on face");
  TCollection_AsciiString aText;
  aText = 
    "  // Create a Face" EOL
    "  TopoDS_Face theFace;" EOL
    "  // initialize aFace" EOL
    "  // aFace = ..." EOL EOL

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
    "//======================================" EOL EOL;
  setResultTitle (aTitle.ToCString());
  setResultText (aText.ToCString());

  BRepAdaptor_Surface anAdapSurface (theFace);

  Standard_Integer maxOrder = 2;
  Standard_Real aResol = gp::Resolution();
  BRepLProp_SLProps aLProps ( anAdapSurface, maxOrder , aResol);
  Handle(AIS_InteractiveObject) aObjs[11];
  for (int i=0; i < theNbPoints; i++)
  {
    if (WAIT_A_LITTLE)
      return;
    for (int j=0; j < sizeof(aObjs)/sizeof(Handle(AIS_InteractiveObject)); j++)
      if (!aObjs[j].IsNull())
        getAISContext()->Erase(aObjs[j]);

    // compute local properties
    aLProps.SetParameters (thePoints[i][0], thePoints[i][1]);
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

    //show:
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
