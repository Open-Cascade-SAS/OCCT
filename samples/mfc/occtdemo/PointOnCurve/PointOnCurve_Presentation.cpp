// PointOnCurve_Presentation.cpp: implementation of the PointOnCurve_Presentation class.
// Calculation of points on Curves
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PointOnCurve_Presentation.h"

#include <AIS_Point.hxx>
#include <Precision.hxx>

#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gce_MakeLin.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TCollection_AsciiString.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GCPnts_UniformDeflection.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom_CartesianPoint.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <Geom_BSplineCurve.hxx>

#define DEGMIN 3
#define DEGMAX 8
#define SCALE  120

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new PointOnCurve_Presentation;

// Initialization of array of samples
const PointOnCurve_Presentation::PSampleFuncType PointOnCurve_Presentation::SampleFuncs[] =
{
  &PointOnCurve_Presentation::sample1,
  &PointOnCurve_Presentation::sample2,
  &PointOnCurve_Presentation::sample3,
  &PointOnCurve_Presentation::sample4,
  &PointOnCurve_Presentation::sample5
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PointOnCurve_Presentation::PointOnCurve_Presentation()
{
  FitMode=true;
  myIndex = 0;
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Points on Curves");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void PointOnCurve_Presentation::DoSample()
{
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : CreateCurve
// Purpose  : creating a BSpline Curve 
//================================================================

static Handle(Geom_BSplineCurve) CreateCurve()                     
{
  Standard_Real aCoords[][3] = 
  {
    {-5,-1,0},{-2,3,0},{3,-2.5,0},{6,2,0}
  };

  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  TColgp_Array1OfPnt aCurvePoint (1, nPoles);

  for (Standard_Integer i=0; i < nPoles; i++)
    aCurvePoint(i+1) = gp_Pnt (aCoords[i][0]*SCALE, aCoords[i][1]*SCALE, aCoords[i][2]*SCALE);

  GeomAPI_PointsToBSpline aPTB (
    aCurvePoint,DEGMIN,DEGMAX,GeomAbs_C2,Precision::Confusion());

  Handle (Geom_BSplineCurve) aCurve = aPTB.Curve();
  return aCurve;
}

//================================================================
// Function : PointOnCurve_Presentation::sample1
// Purpose  : 
//================================================================

void PointOnCurve_Presentation::sample1()
{
  Standard_CString aTitle = "Uniform deflection distribution";
  setResultTitle (aTitle);
  TCollection_AsciiString aText (
    "////////////////////////////////////////////////////////////////" EOL
    "// Computing a distribution of points on a curve."   EOL
    "////////////////////////////////////////////////////////////////" EOL EOL

    "//==============================================================" EOL
    "// Uses a criterion of maximum deflection between"   EOL 
    "//the curve and the polygon. The maximum distance between curve" EOL 
    "//and the polygon that results from the points of the " EOL
    "//distribution is not greater than Deflection." EOL 
    "//===============================================================" EOL EOL

    "// Data used by various samples:" EOL
    "Standard_Real aCoords[][3] = " EOL
    "{" EOL
    "{-5,-1,0},{-2,3,0},{3,-2.5,0},{6,2,0}" EOL
    "};" EOL EOL
    
    "// Creating and initializing array of points:" EOL
    "#define SCALE 120" EOL
    "Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);" EOL
    "TColgp_Array1OfPnt aCurvePoint (1, nPoles);" EOL
    "for (Standard_Integer i=0; i < nPoles; i++)" EOL
    "  aCurvePoint(i+1) = " EOL
    "    gp_Pnt(aCoords[i][0]*SCALE, aCoords[i][1]*SCALE, aCoords[i][2]*SCALE);" EOL EOL

    "// Creating a BSpline Curve with DegMin = 3, DegMax = 8" EOL
    "//using defined above array of poles:" EOL
    "GeomAPI_PointsToBSpline aPTB (aCurvePoint,DEGMIN,DEGMAX," EOL
    "                              GeomAbs_C2,Precision::Confusion());" EOL EOL

    "// The adapted curve is created in the following way:" EOL
    "Handle (Geom_BSplineCurve) aCurve = aPTB.Curve();" EOL
    "GeomAdaptor_Curve adapCurve (aCurve) ;" EOL EOL

    "// Computing a distribution:" EOL
    "Standard_Real Deflection = 0.6*SCALE ;" EOL
    "//After that Deflection *= 0.2" EOL
    "GCPnts_UniformDeflection aDefPoint(adapCurve,Deflection,Standard_True);" EOL EOL

    "if ( aDefPoint.IsDone() )" EOL
    "{ " EOL
    "  // Get the number of points of the distribution:" EOL
    "  Standard_Integer nbr = aDefPoint.NbPoints() ;" EOL
    "  // Getting the values of the point of index <i> in the distribution:" EOL
    "  for ( i = 1 ; i <= nbr ; i++ )" EOL
    "  {" EOL
    "    gp_Pnt aValueOfPoint = aDefPoint.Value(i);" EOL
    "  }" EOL
    "}" EOL EOL
    "//====================================================================" EOL EOL
    );

  setResultText(aText.ToCString());

  Handle (Geom_BSplineCurve) aCurve = CreateCurve();
  
  GeomAdaptor_Curve adapCurve (aCurve) ;

  Standard_Real Deflection = 0.6*SCALE ;
  Standard_Integer nbr;
  for (Standard_Integer j = 1 ; j <=3 ; j++)
  {
    if (WAIT_A_LITTLE) return;
    getAISContext()->EraseAll();
    drawCurve(aCurve);

    GCPnts_UniformDeflection aDefPoint(adapCurve,Deflection,Standard_True);
    if ( aDefPoint.IsDone() )
     { 
        if (WAIT_A_LITTLE) return;

        nbr = aDefPoint.NbPoints() ;
        for ( Standard_Integer i = 1 ; i <= nbr ; i++ )
        {   
            drawPoint(aDefPoint.Value(i));
        }
                   
        if (WAIT_A_LITTLE) return;

        for ( i = 1 ; i <= nbr-1 ; i++ )
        { 
          gp_Pnt aPoint1 = aDefPoint.Value(i);
          gp_Pnt aPoint2 = aDefPoint.Value(i+1);    
          Standard_Real aDist = aPoint1.Distance(aPoint2);

          gce_MakeLin aLin(aPoint1, aPoint2);

          Standard_Real aParam = aPoint1.Distance(aPoint2);
          Handle(Geom_TrimmedCurve) aSeg = new Geom_TrimmedCurve(new Geom_Line(aLin), 0, aParam);
          drawCurve(aSeg, Quantity_NOC_GREEN1);
        }
        if (WAIT_A_LITTLE) return;
    }

    Standard_CString aString = "Deflection = ";
    Comment(aString,aText,Deflection,nbr);
    Deflection *= 0.2; 
  }
  
}

//================================================================
// Function : PointOnCurve_Presentation::sample2
// Purpose  : 
//================================================================

void PointOnCurve_Presentation::sample2()
{
  Standard_CString aTitle = "Computing a point by length" ;
  setResultTitle (aTitle);
  TCollection_AsciiString aText (
    "////////////////////////////////////////////////////////////////" EOL
    "// Computing a point on a curve."   EOL
    "////////////////////////////////////////////////////////////////" EOL EOL

    "//==============================================================" EOL
    "// Computes the point on the curve  located at the curvilinear" EOL 
    "//distance Abscissa from the point of parameter U0. The distance " EOL
    "//being measured along the curve (curvilinear abscissa on the curve)." EOL
    "//==============================================================" EOL EOL

    "// Data used by various samples:" EOL
    "Standard_Real aCoords[][3] = " EOL
    "{" EOL
    "  {-5,-1,0},{-2,3,0},{3,-2.5,0},{6,2,0}" EOL
    "};" EOL EOL
    
    "// Creating and initializing array of points:" EOL
    "#define SCALE 120" EOL
    "Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);" EOL
    "TColgp_Array1OfPnt aCurvePoint (1, nPoles);" EOL
    "for (Standard_Integer i=0; i < nPoles; i++)" EOL
    "  aCurvePoint(i+1) = " EOL
    "    gp_Pnt(aCoords[i][0]*SCALE, aCoords[i][1]*SCALE, aCoords[i][2]*SCALE);" EOL EOL

    "// Creating a BSpline Curve with DegMin = 3, DegMax = 8" EOL
    "//using defined above array of poles:" EOL
    "GeomAPI_PointsToBSpline aPTB (aCurvePoint,DEGMIN,DEGMAX," EOL
    "                              GeomAbs_C2,Precision::Confusion());" EOL EOL

    "//The adapted curve is created in the following way:" EOL
    "Handle (Geom_BSplineCurve) aCurve = aPTB.Curve();" EOL
    "GeomAdaptor_Curve adapCurve (aCurve) ;" EOL EOL

    "//Computing a point:" EOL
    "Standard_Real Abscissa = 3*SCALE ,  U0 = 0.27 ;" EOL
    "//After that Abscissa *= 2 " EOL
    "GCPnts_AbscissaPoint absPoint( adapCurve , Abscissa , U0 ) ;" EOL EOL

    "if ( absPoint.IsDone() )" EOL
    "{" EOL
    "  //Getting the value of the point:" EOL
    "  gp_Pnt aPnt;" EOL
    "  Standard_Real aParam = absPoint.Parameter() ;" EOL
    "  aCurve->D0(aParam,aPnt);" EOL
    "}" EOL EOL

    "//====================================================================" EOL EOL
    );

  setResultText(aText.ToCString());

  Handle (Geom_BSplineCurve) aCurve = CreateCurve();

  GeomAdaptor_Curve  adapCurve (aCurve);

  gp_Pnt aPnt1,aPnt2;
  Standard_Real Abscissa = 3*SCALE ,  U0 = 0.27 ;
  for (Standard_Integer j = 1 ; j <=3 ; j++)
  {
    if (WAIT_A_LITTLE) return;
    getAISContext()->EraseAll();
    drawCurve(aCurve);

    GCPnts_AbscissaPoint absPoint( adapCurve , Abscissa , U0 ) ;
    if ( absPoint.IsDone() )
    {
     if (WAIT_A_LITTLE) return;

     Standard_Real aParam = absPoint.Parameter() ;
     aCurve->D0(U0,aPnt1);
     aCurve->D0(aParam,aPnt2);
     drawPoint(aPnt1);
     if (WAIT_A_LITTLE) return;
     drawCurve(new Geom_TrimmedCurve(aCurve,U0,aParam),Quantity_NOC_GREEN1);
     if (WAIT_A_LITTLE) return;
     drawPoint(aPnt2);  
    }

   Abscissa *= 2;
  }

}

//================================================================
// Function : PointOnCurve_Presentation::sample3
// Purpose  : 
//================================================================

void PointOnCurve_Presentation::sample3()
{
  Standard_CString aTitle = "Uniform abscissa distribution";
  setResultTitle (aTitle);
  TCollection_AsciiString aText (
    "////////////////////////////////////////////////////////////////" EOL
    "// Computing a distribution of points on a curve."   EOL
    "////////////////////////////////////////////////////////////////" EOL EOL

    "//==============================================================" EOL
    "// Computes a uniform abscissa distribution of points on a curve." EOL
    "//The distance between two consecutive points is measured along " EOL
    "//the curve." EOL
    "//==============================================================" EOL EOL

    "// Data used by various samples:" EOL
    "Standard_Real aCoords[][3] = " EOL
    "{" EOL
    "  {-5,-1,0},{-2,3,0},{3,-2.5,0},{6,2,0}" EOL
    "};" EOL EOL
    
    "// Creating and initializing array of points:" EOL
    "#define SCALE 120" EOL
    "Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);" EOL
    "TColgp_Array1OfPnt aCurvePoint (1, nPoles);" EOL
    "for (Standard_Integer i=0; i < nPoles; i++)" EOL
    "  aCurvePoint(i+1) = " EOL
    "    gp_Pnt(aCoords[i][0]*SCALE, aCoords[i][1]*SCALE, aCoords[i][2]*SCALE);" EOL EOL

    "// Creating a BSpline Curve with DegMin = 3, DegMax = 8" EOL
    "//using defined above array of poles:" EOL
    "GeomAPI_PointsToBSpline aPTB (aCurvePoint,DEGMIN,DEGMAX," EOL
    "                              GeomAbs_C2,Precision::Confusion());" EOL EOL

    "//The adapted curve is created in the following way:" EOL
    "Handle (Geom_BSplineCurve) aCurve = aPTB.Curve();" EOL
    "GeomAdaptor_Curve adapCurve (aCurve) ;" EOL EOL

    "// Computing a distribution:" EOL
    "Standard_Real Abscissa = 5*SCALE ;" EOL
    "//After that Abscissa *= 0.5 " EOL
    "GCPnts_UniformAbscissa absPoint( adapCurve , Abscissa ) ;" EOL EOL

    "if ( absPoint.IsDone() )" EOL
    "{" EOL
    "  // Get the number of points of the distribution:" EOL
    "  Standard_Integer nbr = absPoint.NbPoints() ;" EOL
    "  // Getting the values of the point of index <i> in the distribution:" EOL
    "  for ( i = 1 ; i <= nbr ; i++ )" EOL
    "  {" EOL
    "    aParam = absPoint.Parameter (i) ;" EOL
    "    gp_Pnt aPnt1;" EOL
    "    aCurve->D0(aParam,aPnt1);" EOL
    "  }" EOL
    "}" EOL EOL

     "//====================================================================" EOL EOL
  );

  setResultText(aText.ToCString());

  Handle (Geom_BSplineCurve) aCurve = CreateCurve();
  GeomAdaptor_Curve  adapCurve (aCurve) ;

  Standard_Real Abscissa = 5*SCALE ;
  gp_Pnt aPnt1,aPnt2;
  Standard_Integer nbr;

  for (Standard_Integer j = 1 ; j <=3 ; j++)
  {
    if (WAIT_A_LITTLE) return;
    getAISContext()->EraseAll();
    drawCurve(aCurve);

    GCPnts_UniformAbscissa absPoint( adapCurve , Abscissa ) ;
    if ( absPoint.IsDone() )
    { 
       Standard_Real aParam ;
       nbr = absPoint.NbPoints() ;
       if (WAIT_A_LITTLE) return;
       for ( Standard_Integer i = 1 ; i <= nbr ; i++ )
       {
         aParam = absPoint.Parameter (i) ;
         aCurve->D0(aParam,aPnt1);
         drawPoint(aPnt1);
       }
       if (WAIT_A_LITTLE) return;
    }
   Standard_CString aString = "Abscissa =  ";
   Comment(aString,aText,Abscissa,nbr);
   Abscissa *= 0.5;
  }
}

//================================================================
// Function : PointOnCurve_Presentation::sample4
// Purpose  : 
//================================================================

void PointOnCurve_Presentation::sample4()
{
  Standard_CString aTitle = "Length between two points";
  setResultTitle (aTitle);
  TCollection_AsciiString aText (
    "////////////////////////////////////////////////////////////////" EOL
    "// Computes the length of curve between two points " EOL
    "//of parameters U1 and U2."   EOL
    "////////////////////////////////////////////////////////////////" EOL EOL

    "// Data used by various samples:" EOL
    "Standard_Real aCoords[][3] = " EOL
    "{" EOL
    "  {-5,-1,0},{-2,3,0},{3,-2.5,0},{6,2,0}" EOL
    "};" EOL EOL
    
    "// Creating and initializing array of points:" EOL
    "#define SCALE 120" EOL
    "Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);" EOL
    "TColgp_Array1OfPnt aCurvePoint (1, nPoles);" EOL
    "for (Standard_Integer i=0; i < nPoles; i++)" EOL
    "  aCurvePoint(i+1) = " EOL
    "    gp_Pnt(aCoords[i][0]*SCALE, aCoords[i][1]*SCALE, aCoords[i][2]*SCALE);" EOL EOL

    "// Creating a BSpline Curve with DegMin = 3, DegMax = 8" EOL
    "//using defined above array of poles:" EOL
    "GeomAPI_PointsToBSpline aPTB (aCurvePoint,DEGMIN,DEGMAX," EOL
    "                              GeomAbs_C2,Precision::Confusion());" EOL EOL

    "//The adapted curve is created in the following way:" EOL
    "Handle (Geom_BSplineCurve) aCurve = aPTB.Curve();" EOL
    "GeomAdaptor_Curve adapCurve (aCurve) ;" EOL EOL

    "//Creating and initializing array of parameters" EOL
    "TColStd_Array2OfReal aArrayOfParameters(1,3,1,2);" EOL
    "aArrayOfParameters.SetValue(1,1,0.6);" EOL
    "aArrayOfParameters.SetValue(1,2,0.8);" EOL
    "aArrayOfParameters.SetValue(2,1,0);" EOL
    "aArrayOfParameters.SetValue(2,2,0.5);" EOL
    "aArrayOfParameters.SetValue(3,1,0.3);" EOL
    "aArrayOfParameters.SetValue(3,2,0.855);" EOL EOL

    "// Computing the length of curve between points :" EOL
    "for (Standard_Integer i = 1 ; i <=3 ; i++)" EOL
    "  Standard_Real aLength = GCPnts_AbscissaPoint::Length(adapCurve," EOL
    "    aArrayOfParameters(i,j),aArrayOfParameters(i,j+1));" EOL EOL

    "//======================================================================" EOL EOL
    );

  setResultText(aText.ToCString());

  Handle (Geom_BSplineCurve) aCurve = CreateCurve();
  GeomAdaptor_Curve  adapCurve (aCurve) ;
  
  gp_Pnt aPnt1,aPnt2;
  TColStd_Array2OfReal aArrayOfParameters(1,3,1,2);
  aArrayOfParameters.SetValue(1,1,0.6);
  aArrayOfParameters.SetValue(1,2,0.8);
  aArrayOfParameters.SetValue(2,1,0);
  aArrayOfParameters.SetValue(2,2,0.5);
  aArrayOfParameters.SetValue(3,1,0.3);
  aArrayOfParameters.SetValue(3,2,0.855);

  for (Standard_Integer i = 1 ; i <=3 ; i++)
  {
    if (WAIT_A_LITTLE) return;
    getAISContext()->EraseAll();
    drawCurve(aCurve);

    if (WAIT_A_LITTLE) return;
    Standard_Integer j = 1 ;

    aCurve->D0(aArrayOfParameters(i,j),aPnt1);
    drawPoint(aPnt1);
    if (WAIT_A_LITTLE) return;

    aCurve->D0(aArrayOfParameters(i,j+1),aPnt2);
    drawPoint(aPnt2);
    if (WAIT_A_LITTLE) return;

    drawCurve(new Geom_TrimmedCurve(aCurve,aArrayOfParameters(i,j),
                               aArrayOfParameters(i,j+1)),Quantity_NOC_GREEN1);

    Standard_Real aLength = GCPnts_AbscissaPoint::Length(adapCurve,aArrayOfParameters(i,j),
       aArrayOfParameters(i,j+1));

    Standard_CString aString = "Length = ";
    Comment(aString,aText,aLength,aArrayOfParameters(i,j),aArrayOfParameters(i,j+1));
  }
}

//================================================================
// Function : PointOnCurve_Presentation::sample5
// Purpose  : 
//================================================================

void PointOnCurve_Presentation::sample5()
{ 
  Standard_CString aTitle = "Length of a curve";
  setResultTitle (aTitle);
  TCollection_AsciiString aText (
    "////////////////////////////////////////////////////////////////" EOL
    "// Computes the length of curve ."   EOL
    "////////////////////////////////////////////////////////////////" EOL EOL

    "// Data used by various samples:" EOL
    "Standard_Real aCoords[][3] = " EOL
    "{" EOL
    "  {-5,-1,0},{-2,3,0},{3,-2.5,0},{6,2,0}" EOL
    "};" EOL EOL
    
    "// Creating and initializing array of points:" EOL
    "#define SCALE 120" EOL
    "Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);" EOL
    "TColgp_Array1OfPnt aCurvePoint (1, nPoles);" EOL
    "for (Standard_Integer i=0; i < nPoles; i++)" EOL
    "  aCurvePoint(i+1) = " EOL
    "    gp_Pnt(aCoords[i][0]*SCALE, aCoords[i][1]*SCALE, aCoords[i][2]*SCALE);" EOL EOL

    "// Creating a BSpline Curve with DegMin = 3, DegMax = 8" EOL
    "//using defined above array of poles:" EOL
    "GeomAPI_PointsToBSpline aPTB (aCurvePoint,DEGMIN,DEGMAX," EOL
    "                              GeomAbs_C2,Precision::Confusion());" EOL EOL

    "//The adapted curve is created in the following way:" EOL
    "Handle (Geom_BSplineCurve) aCurve = aPTB.Curve();" EOL
    "GeomAdaptor_Curve adapCurve (aCurve) ;" EOL EOL

    "// Computing the length of curve:" EOL
    "Standard_Real aLength = GCPnts_AbscissaPoint::Length(adapCurve);" EOL EOL 

    "//======================================================================" EOL EOL
    );

  setResultText(aText.ToCString());

  getAISContext()->EraseAll();
  Handle (Geom_BSplineCurve) aCurve = CreateCurve();
  drawCurve(aCurve);
  GeomAdaptor_Curve  adapCurve (aCurve) ;

  gp_Pnt aPnt1,aPnt2;
  if (WAIT_A_LITTLE) return;
  Standard_Real aLength = GCPnts_AbscissaPoint::Length(adapCurve);

  aCurve->D0(0,aPnt1);
  aCurve->D0(1,aPnt2);
  drawPoint(aPnt1);
  if (WAIT_A_LITTLE) return;
  drawPoint(aPnt2);       
  if (WAIT_A_LITTLE) return;

  drawCurve(aCurve,Quantity_NOC_GREEN1);
  Standard_CString aString = "Length of the curve : " ;
  Comment(aString,aText,aLength);
}


void PointOnCurve_Presentation::Comment(const Standard_CString theString,
      TCollection_AsciiString& theText, const Standard_Real theNum1,const Standard_Real theNum2,
      const Standard_Real theNum3)
{ 
      theText += EOL;
      if( theNum3 > 0 ) 
      { 
        theText += " Parameters of points :" EOL;
        theText += "   U1 = ";
        theText += TCollection_AsciiString (theNum2);
        theText += "  U2 = ";
        theText += TCollection_AsciiString (theNum3); 
        theText += EOL;  
        theText += " Result: " ;
      }
      else 
      {
        theText += " Result: " ;
        theText += EOL;  
      }
      theText += theString ;
      theText += TCollection_AsciiString (theNum1);
      theText += EOL ;    
  
  setResultText (theText.ToCString()); 

}


void PointOnCurve_Presentation::Comment(const Standard_CString theString,
      TCollection_AsciiString& theText,const Standard_Real theDistance,const Standard_Integer theNum)
{  
   theText += EOL EOL;
   theText += theString;
   theText += TCollection_AsciiString(theDistance);
   theText += EOL;
   theText += "Result: ";
   theText += EOL ;
   theText += "Number of points of the distribution:  ";
   theText += TCollection_AsciiString (theNum);
   setResultText (theText.ToCString()); 
}
