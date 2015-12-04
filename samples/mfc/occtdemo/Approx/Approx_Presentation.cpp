// Approx_Presentation.cpp: implementation of the Approx_Presentation class.
// Approximation of curves and surfaces from points.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Approx_Presentation.h"

#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <gp_Pnt.hxx>

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

#define  SCALE  100


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Approx_Presentation;

// Initialization of array of samples
const Approx_Presentation::PSampleFuncType Approx_Presentation::SampleFuncs[] =
{
  &Approx_Presentation::sample1,
  &Approx_Presentation::sample2,
  &Approx_Presentation::sample3,
  &Approx_Presentation::sample4
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Approx_Presentation::Approx_Presentation()
{
  myIndex = 0;
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Approximation of curves and surfaces.");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Approx_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////
//================================================================

//defining the data of BSpline curves and surfaces:

static Standard_Real Tol [] = 
{
  0.5*SCALE,
  0.7*SCALE,
  0.4*SCALE,
    1*SCALE
};

static Standard_Integer DegMin [] = 
{
  2,3,5,7
};

static Standard_Integer DegMax [] = 
{
  7,9,10,12
};

static GeomAbs_Shape Continuity [] = 
{
  GeomAbs_C2,  GeomAbs_C1,  GeomAbs_C2,  GeomAbs_C3
}; 


//================================================================
// Function : Comment
// Purpose  : 
//================================================================
 
static TCollection_AsciiString  Comment(Standard_Real Step,
                                        Standard_Integer Upper,
                                        Standard_Integer DegMin,
                                        Standard_Integer DegMax,
                                        Standard_Integer Indicator,
                                        Standard_Real Tol)
{
  TCollection_AsciiString aText;
    aText = ( 
    "/////////////////////////////////////////////////////////////////" EOL
    "// Approximation of surface." EOL
    "// Building a BSpline surface which approximates a set of points." EOL
    "/////////////////////////////////////////////////////////////////" EOL EOL

    "// creating a set of points:" EOL
    );

    aText += "Standard_Real Step = ";
    aText += TCollection_AsciiString(Step);
    aText += ";" EOL;
    aText += "Standard_Integer Upper = ";
    aText += TCollection_AsciiString(Upper);
    aText += ";" EOL EOL;
    aText += (
    "  //a set of X and Y coordinates:" EOL
    "    Standard_Real aXStep = Step , aYStep = Step ;" EOL
    "    Standard_Real aX0 = -300, aY0 = -200;" EOL
    "  //Z coordinates:" EOL
    "    TColStd_Array2OfReal aZPoints( 1, Upper , 1, Upper );" EOL EOL

    "// initializing array of Z coordinates:" EOL
    "// aZPoints(1,1) = -2;" EOL
    "// aZPoints(1,2) = 3;" EOL
    "// ..." EOL EOL

    "//creating a approximate BSpline surface:" EOL
    );

    aText += "Parameters of surface:" EOL ;
    aText += "DegMin = ";
    aText += TCollection_AsciiString(DegMin);
    aText += ";" EOL;
    aText += "DegMax = ";
    aText += TCollection_AsciiString(DegMax);
    aText += ";" EOL;
    aText += "Continuity = " ;
  
    if( Indicator == 2 )
      aText += "GeomAbs_C1";
    if( Indicator == 3 )
      aText += "GeomAbs_C2";
    if( Indicator == 4 )
      aText += "GeomAbs_C3";
    aText += ";" EOL;
    aText += "Tolerance = ";
    aText += TCollection_AsciiString(Tol/SCALE);
    aText += ";" EOL EOL ;

    aText += (
      "GeomAPI_PointsToBSplineSurface aPTBS;" EOL
      "aPTBS.Init(aZPoints,aX0,aXStep,aY0,aYStep," EOL
      "           DegMin,DegMax,Continuity,Tolerance);" EOL
      "Handle(Geom_BSplineSurface) aSurface = aPTBS.Surface();" EOL EOL EOL
    );

  return aText;
  
}

//================================================================
// Function : Approx_Presentation::CreateBSplineSurface
// Purpose  : 
//================================================================

Handle(Geom_BSplineSurface) Approx_Presentation::CreateBSplineSurface(TColStd_Array2OfReal& aZPoints,
                                                               Standard_Real theXStep,
                                                               Standard_Real theYStep,
                                                               Standard_Integer Count)
{  
  Standard_Real aX0 = -300, aY0 = -200;
  
  GeomAPI_PointsToBSplineSurface aPTBS;
  aPTBS.Init(aZPoints,aX0,theXStep,aY0,theYStep,
    DegMin[Count],DegMax[Count],Continuity[Count],Tol[Count]);
  Handle(Geom_BSplineSurface) aSurface = aPTBS.Surface();

  return aSurface;
}


//================================================================
// Function : Approx_Presentation::DrawModifyBSplineSurface
// Purpose  : 
//================================================================

Standard_Boolean Approx_Presentation::DrawModifyBSplineSurface(TColStd_Array2OfReal& aZPoints,
                                                               Standard_Real theXStep,
                                                               Standard_Real theYStep,
                                                               Standard_Integer theIndexX,
                                                               Standard_Integer theIndexY,
                                                               Standard_Real theDeflection,
                                                               Handle(AIS_InteractiveObject)& aMovePnt,
                                                               Handle(AIS_InteractiveObject)& aObj,
                                                               Standard_Integer Count)
                           
{ 
  Handle(AIS_InteractiveObject) auxObj;
  Standard_Real aX0 = -300, aY0 = -200;
  Standard_Real aLastZ = aZPoints(theIndexX,theIndexY);
  aZPoints(theIndexX,theIndexY) += 100*theDeflection; 
  Standard_Real aCurrentX = aX0 + theXStep*(theIndexX-1),
                aCurrentY = aY0 + theYStep*(theIndexY-1);

  getAISContext()->Erase(aMovePnt);
  aMovePnt = drawPoint(gp_Pnt(aCurrentX,aCurrentY,aZPoints(theIndexX,theIndexY)));

  if(WAIT_A_LITTLE) return Standard_False;
  
    
  GeomAPI_PointsToBSplineSurface aPTBS;
  aPTBS.Init(aZPoints,aX0,theXStep,aY0,theYStep,
    DegMin[Count],DegMax[Count],Continuity[Count],Tol[Count]);

  aZPoints(theIndexX,theIndexY) = aLastZ;

  Handle(Geom_BSplineSurface) aSurface = aPTBS.Surface();

  auxObj = drawSurface(aSurface);
  getAISContext()->Erase(aObj);
  aObj = auxObj;
  if(WAIT_A_SECOND) return Standard_False;

  return Standard_True;
}




//================================================================
// Function : Approx_Presentation::sample1
// Purpose  : 
//================================================================

void Approx_Presentation::sample1()
{  
  Standard_Integer Count = 0;

  TCollection_AsciiString aText = ( 
    "//////////////////////////////////////////////////////////////" EOL
    "// Approximation of curve." EOL
    "// Building a BSpline curve which approximates a set of points." EOL
    "//////////////////////////////////////////////////////////////" EOL EOL

    "// creating a set of points to approximate," EOL
    "// nPoint is the number of points:" EOL
    "Standard_Integer nPoint = 20;" EOL
    "TColgp_Array1OfPnt aCurvePoint (1, nPoint);" EOL EOL

    "// initializing this array of points:" EOL
    "// aCurvePoint(1,1) = gp_Pnt(-6,1,0);" EOL
    "// aCurvePoint(1,2) = gp_Pnt(-5,1.5,0);" EOL
    "// ..." EOL EOL

    "//creating an empty approximation algorithm:" EOL
    "GeomAPI_PointsToBSpline aPTB;" EOL EOL
    
    "//creating a approximate BSpline curve:" EOL
  );

  aText += "//parameters of curve:" EOL ;
  aText += "DegMin = ";
  aText += TCollection_AsciiString(DegMin[Count]);
  aText += ";" EOL;
  aText += "DegMax = ";
  aText += TCollection_AsciiString(DegMax[Count]);
  aText += ";" EOL;
  aText += "Continuity = GeomAbs_C2" ;
  aText += ";" EOL;
  aText += "Tolerance = ";
  aText += TCollection_AsciiString(Tol[Count]/SCALE);
  aText += ";" EOL EOL ;

  aText += (
    "aPTB.Init(aCurvePoint,DegMin,DegMax,Continuity,Tolerance);" EOL
    "Handle(Geom_BSplineCurve) aCurve = aPTB.Curve();" EOL
  );

  setResultTitle("Creating approximations of curves");
  setResultText(aText.ToCString());

  getAISContext()->EraseAll();  

  Standard_Real aCoords[][3] = {
    {-6,1,0},{-5,1.5,0},{-4,2,0},{-3.5,3,0},{-3,2.7,0},{-2,2.5,0},{-1.5,1,0},{-1,0.5,0},
    {0,0,0},{1,0.3,0},{2,1,0},{3,1.5,0},{4,2.3,0},{5,2.7,0},{5.5,3.2,0},{6,2.5,0},
    {7,2,0},{7.5,1,0},{8,0,0},{8.5,-1,0}
  };

  Standard_Integer nPoint = sizeof(aCoords)/(sizeof(Standard_Real)*3);

  TColgp_Array1OfPnt aCurvePoint (1, nPoint);

  GeomAPI_PointsToBSpline aPTB;

  Handle(AIS_InteractiveObject) aIndexPnt [2],aObj;
  Standard_Integer aIndex[2] = {9,13};
  Standard_Real aDeflection[2] = {-1.5,2};
  
  for (Standard_Integer i=0; i < nPoint; i++)
  {
    aCurvePoint(i+1) = gp_Pnt (aCoords[i][0]*SCALE-100, aCoords[i][1]*SCALE-100, aCoords[i][2]*SCALE); 
    if( i+1 == aIndex[0])
      aIndexPnt[0] = drawPoint(aCurvePoint(aIndex[0]));
    if( i+1 == aIndex[1])
      aIndexPnt[1] = drawPoint(aCurvePoint(aIndex[1]));
    if( i+1 != aIndex[0] && i+1 != aIndex[1])
      drawPoint(aCurvePoint(i+1));
  }

  if(WAIT_A_LITTLE) return;

  aPTB.Init(aCurvePoint,DegMin[Count],DegMax[Count],Continuity[Count],Tol[Count]);
  Handle(Geom_BSplineCurve) aCurve = aPTB.Curve();
  aObj = drawCurve(aCurve);

  for( i = 0 ; i < 2 ; i++)
  {
    if (WAIT_A_SECOND) return;
    getAISContext()->Erase(aIndexPnt[i]);
    
    aCurvePoint(aIndex[i]) = gp_Pnt(aCurvePoint(aIndex[i]).X()-SCALE*aDeflection[i],
                                    aCurvePoint(aIndex[i]).Y()+SCALE*aDeflection[i],
                                    aCurvePoint(aIndex[i]).Z());
  

    aIndexPnt[i] = drawPoint(aCurvePoint(aIndex[i]));
    aPTB.Init(aCurvePoint,DegMin[Count],DegMax[Count],Continuity[Count],Tol[Count]);
    aCurve = aPTB.Curve();

    if (WAIT_A_LITTLE) return;
    getAISContext()->Erase(aObj);
    aObj = drawCurve(aCurve);    
  }

}


//================================================================
// Function : Approx_Presentation::sample2
// Purpose  : 
//================================================================

void Approx_Presentation::sample2()
{
  setResultTitle("Creating approximations of surfaces");

  Standard_Integer Count = 1;
  Standard_Real aZCoords [] = 
  {
    {-1},{0},{0},{1},{0},{-1},{-1},{0},{0},{-1.5},{-2.5},{0},{1},{-2},{-3},{0}
  }; 

  Standard_Real aXStep = 175, aYStep = 175;
  Standard_Real aX0 = -300, aY0 = -200;

  Standard_Integer anUpper = 4;
  TColStd_Array2OfReal aZPoints(1,anUpper,1,anUpper);

  Standard_Integer aIndexX[] =  {  4,  3,   2,   2,    1 };
  Standard_Integer aIndexY[] =  {  4,  3,   2,   3,    4 };
  Standard_Real aDeflection[] = { 1.5, 2.5, 1.5, 1.5, -1 };

  Standard_Integer aNumOfIndexPnt = sizeof(aIndexX)/sizeof(Standard_Integer);

  
  TColgp_Array2OfPnt aPnt(1,4,1,4);
  Handle(AIS_InteractiveObject) aShowPnt[4][4],aObj,aMovePnt;
   
  Standard_Integer aColLength = aZPoints.ColLength();
  Standard_Integer aRowLength = aZPoints.RowLength();
  Standard_Integer aIndex = -1;
  
  for(Standard_Integer i = 0 ; i < aRowLength ; i++)
  {
    for(Standard_Integer j = 0; j < aColLength ; j++)
    {
      aIndex++;
      aZPoints(i+1,j+1) = aZCoords[aIndex];     
    }
  }

  Standard_Real auxY0,auxX0 = aX0 - aXStep;

  for( i = 0 ; i < aColLength ; i++)
  {
    auxX0 += aXStep;
    auxY0 = aY0 - aYStep;

    for(Standard_Integer j = 0 ; j < aRowLength ; j++)
    {
      aZPoints(i+1,j+1) *=SCALE;      
      auxY0 += aYStep;
      aPnt(i+1,j+1) = gp_Pnt (auxX0,auxY0,aZPoints(i+1,j+1));
      aShowPnt[i][j] = drawPoint(aPnt(i+1,j+1));
    }
  }

  if(WAIT_A_LITTLE) return ;

  TCollection_AsciiString aText;
  aText = Comment(aXStep,anUpper,DegMin[Count],DegMax[Count],Count+1,Tol[Count]);
  setResultText(aText.ToCString());

  Handle(Geom_BSplineSurface) aSurface = CreateBSplineSurface(aZPoints,aXStep,aYStep,Count);
  aObj = drawSurface(aSurface);
  Standard_Boolean aBool;
  if(WAIT_A_LITTLE) return ;
   
  for( i = 0 ; i < aNumOfIndexPnt ; i++)
  {
    aMovePnt = aShowPnt[aIndexX[i]-1][aIndexY[i]-1];
    aBool = DrawModifyBSplineSurface(aZPoints,aXStep,aYStep,aIndexX[i],aIndexY[i],
                                     aDeflection[i],aMovePnt,aObj,Count);
    if(!aBool) return;

    if( i < aNumOfIndexPnt - 1)
    {
    drawPoint(aPnt(aIndexX[i],aIndexY[i]));
    getAISContext()->Erase(aMovePnt);
    }
  }

}


//================================================================
// Function : Approx_Presentation::sample3
// Purpose  : 
//================================================================

void Approx_Presentation::sample3()
{
  setResultTitle("Creating approximations of surfaces");

  Standard_Integer Count = 2;
  Standard_Real aZCoords [] = 
  {
    {-3},{-2.3},{-3},{-0.5},{-1},{-1},{-1},{-1},{0},{0},{0},{0},{1},{-1},{-1},{0}
  };

  Standard_Integer anUpper = 4;
  TColStd_Array2OfReal aZPoints(1,anUpper,1,anUpper);
  
  Standard_Integer aIndexX[] =  {1,   2,   3,  4};
  Standard_Integer aIndexY[] =  {1,   2,   3,  4};
  Standard_Real aDeflection[] = {1.5, 2,   1,  1.5};

  Standard_Integer aNumOfIndexPnt = sizeof(aIndexX)/sizeof(Standard_Integer);

  
  TColgp_Array2OfPnt aPnt(1,4,1,4);
  Handle(AIS_InteractiveObject) aShowPnt[4][4],aObj,aMovePnt;
   
  Standard_Integer aColLength = aZPoints.ColLength();
  Standard_Integer aRowLength = aZPoints.RowLength();
  Standard_Integer aIndex = -1;
  
  for(Standard_Integer i = 0 ; i < aRowLength ; i++)
  {
    for(Standard_Integer j = 0; j < aColLength ; j++)
    {
      aIndex++;
      aZPoints(i+1,j+1) = aZCoords[aIndex];     
    }
  }

  Standard_Real aXStep = 175, aYStep = 175;
  Standard_Real aX0 = -300, aY0 = -200;
  Standard_Real auxY0,auxX0 = aX0 - aXStep;

  for( i = 0 ; i < aColLength ; i++)
  {
    auxX0 += aXStep;
    auxY0 = aY0 - aYStep;

    for(Standard_Integer j = 0 ; j < aRowLength ; j++)
    {
      aZPoints(i+1,j+1) *=SCALE;      
      auxY0 += aYStep;
      aPnt(i+1,j+1) = gp_Pnt (auxX0,auxY0,aZPoints(i+1,j+1));
      aShowPnt[i][j] = drawPoint(aPnt(i+1,j+1));
    }
  }

  if(WAIT_A_LITTLE) return ;

  TCollection_AsciiString aText;
  aText += Comment(aXStep,anUpper,DegMin[Count],DegMax[Count],Count+1,Tol[Count]);
  setResultText(aText.ToCString());

  Handle(Geom_BSplineSurface) aSurface = CreateBSplineSurface(aZPoints,aXStep,aYStep,Count);
  aObj = drawSurface(aSurface);
  Standard_Boolean aBool;
  if(WAIT_A_LITTLE) return ;
   
  for( i = 0 ; i < aNumOfIndexPnt ; i++)
  {
    aMovePnt = aShowPnt[aIndexX[i]-1][aIndexY[i]-1];
    aBool = DrawModifyBSplineSurface(aZPoints,aXStep,aYStep,aIndexX[i],aIndexY[i],
                                     aDeflection[i],aMovePnt,aObj,Count);
    if(!aBool) return;

    if( i < aNumOfIndexPnt - 1)
    {
    drawPoint(aPnt(aIndexX[i],aIndexY[i]));
    getAISContext()->Erase(aMovePnt);
    }
  }

}


//================================================================
// Function : Approx_Presentation::sample4
// Purpose  : 
//================================================================

void Approx_Presentation::sample4()
{
  setResultTitle("Creating approximations of surfaces");

  Standard_Integer Count = 3;
  Standard_Real aZCoords [] = 
  {
    {-1.5},{0.5},{1},{0.5},{-1.5},{0},{-0.5},{0},{-0.5},{0},{1},{-0.5},{0},{-0.5},{1},{0},{-0.5},
    {0},{-0.5},{0},{-1.5},{0.5},{1},{0.5},{-1.5}
  }; 

  Standard_Integer anUpper = 5;
  TColStd_Array2OfReal aZPoints(1,anUpper,1,anUpper);
  
  Standard_Integer aIndexX[] =  { 1, 3,  5 };
  Standard_Integer aIndexY[] =  { 3, 3,  3 };
  Standard_Real aDeflection[] = {-2, 1, -2 };

  Standard_Integer aNumOfIndexPnt = sizeof(aIndexX)/sizeof(Standard_Integer);

  
  TColgp_Array2OfPnt aPnt(1,5,1,5);
  Handle(AIS_InteractiveObject) aShowPnt[5][5],aObj,aMovePnt;
   
  Standard_Integer aColLength = aZPoints.ColLength();
  Standard_Integer aRowLength = aZPoints.RowLength();
  Standard_Integer aIndex = -1;
  
  for(Standard_Integer i = 0 ; i < aRowLength ; i++)
  {
    for(Standard_Integer j = 0; j < aColLength ; j++)
    {
      aIndex++;
      aZPoints(i+1,j+1) = aZCoords[aIndex];     
    }
  }

  Standard_Real aXStep = 140, aYStep = 140;
  Standard_Real aX0 = -300, aY0 = -200;
  Standard_Real auxY0,auxX0 = aX0 - aXStep;

  for( i = 0 ; i < aColLength ; i++)
  {
    auxX0 += aXStep;
    auxY0 = aY0 - aYStep;

    for(Standard_Integer j = 0 ; j < aRowLength ; j++)
    {
      aZPoints(i+1,j+1) *=SCALE;      
      auxY0 += aYStep;
      aPnt(i+1,j+1) = gp_Pnt (auxX0,auxY0,aZPoints(i+1,j+1));
      aShowPnt[i][j] = drawPoint(aPnt(i+1,j+1));
    }
  }

  if(WAIT_A_LITTLE) return ;

  TCollection_AsciiString aText;
  aText += Comment(aXStep,anUpper,DegMin[Count],DegMax[Count],Count+1,Tol[Count]);
  setResultText(aText.ToCString());

  Handle(Geom_BSplineSurface) aSurface = CreateBSplineSurface(aZPoints,aXStep,aYStep,Count);
  aObj = drawSurface(aSurface);
  Standard_Boolean aBool;
  if(WAIT_A_LITTLE) return ;
   
  for( i = 0 ; i < aNumOfIndexPnt ; i++)
  {
    aMovePnt = aShowPnt[aIndexX[i]-1][aIndexY[i]-1];
    aBool = DrawModifyBSplineSurface(aZPoints,aXStep,aYStep,aIndexX[i],aIndexY[i],
                                     aDeflection[i],aMovePnt,aObj,Count);
    if(!aBool) return;

    if( i < aNumOfIndexPnt - 1)
    {
    drawPoint(aPnt(aIndexX[i],aIndexY[i]));
    getAISContext()->Erase(aMovePnt);
    }
  }

}


