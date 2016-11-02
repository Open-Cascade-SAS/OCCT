// Created on: 2015-02-18
// Created by: Nikolai BUKHALOV
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2015 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <IntPatch_PointLine.hxx>

#include <Adaptor3d_HSurface.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntPatch_PointLine,IntPatch_Line)

IntPatch_PointLine::IntPatch_PointLine (const Standard_Boolean Tang,
                                        const IntSurf_TypeTrans Trans1,
                                        const IntSurf_TypeTrans Trans2) : 
  IntPatch_Line(Tang, Trans1, Trans2)
{}

IntPatch_PointLine::IntPatch_PointLine (const Standard_Boolean Tang,
                                        const IntSurf_Situation Situ1,
                                        const IntSurf_Situation Situ2) : 
  IntPatch_Line(Tang, Situ1, Situ2)
{}

IntPatch_PointLine::IntPatch_PointLine (const Standard_Boolean Tang) : 
  IntPatch_Line(Tang)
{}

//=======================================================================
//function : CurvatureRadiusOfIntersLine
//purpose  :
//        ATTENTION!!!
//            Returns negative value if computation is not possible
//=======================================================================
Standard_Real IntPatch_PointLine::
                CurvatureRadiusOfIntersLine(const Handle(Adaptor3d_HSurface)& theS1,
                                            const Handle(Adaptor3d_HSurface)& theS2,
                                            const IntSurf_PntOn2S& theUVPoint)
{
  const Standard_Real aSmallValue = 1.0/Precision::Infinite();
  const Standard_Real aSqSmallValue = aSmallValue*aSmallValue;

  Standard_Real aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  theUVPoint.Parameters(aU1, aV1, aU2, aV2);
  gp_Pnt aPt;
  gp_Vec aDU1, aDV1, aDUU1, aDUV1, aDVV1;
  gp_Vec aDU2, aDV2, aDUU2, aDUV2, aDVV2;
  
  theS1->D2(aU1, aV1, aPt, aDU1, aDV1, aDUU1, aDVV1, aDUV1);
  theS2->D2(aU2, aV2, aPt, aDU2, aDV2, aDUU2, aDVV2, aDUV2);

#if 0
  //The code in this block contains TEST CASES for
  //this algorithm only. It is stupedly to create OCCT-test for
  //the method, which will be changed possibly never.
  //However, if we do something in this method we can check its
  //functionality easily. For that:
  //  1. Initialyze aTestID variable by the correct value;
  //  2. Compile this test code fragment.

  int aTestID = 0;
  Standard_Real anExpectedSqRad = -1.0;
  switch(aTestID)
  {
  case 1:
    //Intersection between two spherical surfaces: O1(0.0, 0.0, 0.0), R1 = 3
    //and O2(5.0, 0.0, 0.0), R2 = 5.0.
    //Considered point has coordinates: (0.9, 0.0, 0.3*sqrt(91.0)).

    aDU1.SetCoord(0.00000000000000000, 0.90000000000000002, 0.00000000000000000);
    aDV1.SetCoord(-2.8618176042508372, 0.00000000000000000, 0.90000000000000002);
    aDUU1.SetCoord(-0.90000000000000002, 0.00000000000000000, 0.00000000000000000);
    aDUV1.SetCoord(0.00000000000000000, -2.8618176042508372, 0.00000000000000000);
    aDVV1.SetCoord(-0.90000000000000002, 0.00000000000000000, -2.8618176042508372);
    aDU2.SetCoord(0.00000000000000000, -4.0999999999999996, 0.00000000000000000);
    aDV2.SetCoord(-2.8618176042508372, 0.00000000000000000, -4.0999999999999996);
    aDUU2.SetCoord(4.0999999999999996, 0.00000000000000000, 0.00000000000000000);
    aDUV2.SetCoord(0.00000000000000000, -2.8618176042508372, 0.00000000000000000);
    aDVV2.SetCoord(4.0999999999999996, 0.00000000000000000, -2.8618176042508372);
    anExpectedSqRad = 819.0/100.0;
    break;
  case 2:
    //Intersection between spherical surfaces: O1(0.0, 0.0, 0.0), R1 = 10
    //and the plane 3*x+4*y+z=26.
    //Considered point has coordinates: (-1.68, 5.76, 8.0).

    aDU1.SetCoord(-5.76, -1.68, 0.0);
    aDV1.SetCoord(2.24, -7.68, 6.0);
    aDUU1.SetCoord(1.68, -5.76, 0.0);
    aDUV1.SetCoord(7.68, 2.24, 0.0);
    aDVV1.SetCoord(1.68, -5.76, -8.0);
    aDU2.SetCoord(1.0, 0.0, -3.0);
    aDV2.SetCoord(0.0, 1.0, -4.0);
    aDUU2.SetCoord(0.0, 0.0, 0.0);
    aDUV2.SetCoord(0.0, 0.0, 0.0);
    aDVV2.SetCoord(0.0, 0.0, 0.0);
    anExpectedSqRad = 74.0;
    break;
  default:
    aTestID = 0;
    break;
  }
#endif

  const gp_Vec aN1(aDU1.Crossed(aDV1)), aN2(aDU2.Crossed(aDV2));
  //Tangent vactor to the intersection curve
  const gp_Vec aCTan(aN1.Crossed(aN2));
  const Standard_Real aSqMagnFDer = aCTan.SquareMagnitude();
  
  if(aSqMagnFDer < aSqSmallValue)
    return -1.0;

  Standard_Real aDuS1 = 0.0, aDvS1 = 0.0, aDuS2 = 0.0, aDvS2 = 1.0;

  {
    //This algorithm is described in NonSingularProcessing() function
    //in ApproxInt_ImpPrmSvSurfaces.gxx file
    Standard_Real aSqNMagn = aN1.SquareMagnitude();
    gp_Vec aTgU(aCTan.Crossed(aDU1)), aTgV(aCTan.Crossed(aDV1));
    Standard_Real aDeltaU = aTgV.SquareMagnitude()/aSqNMagn;
    Standard_Real aDeltaV = aTgU.SquareMagnitude()/aSqNMagn;

    aDuS1 = Sign(sqrt(aDeltaU), aTgV.Dot(aN1));
    aDvS1 = -Sign(sqrt(aDeltaV), aTgU.Dot(aN1));

    aSqNMagn = aN2.SquareMagnitude();
    aTgU.SetXYZ(aCTan.Crossed(aDU2).XYZ());
    aTgV.SetXYZ(aCTan.Crossed(aDV2).XYZ());
    aDeltaU = aTgV.SquareMagnitude()/aSqNMagn;
    aDeltaV = aTgU.SquareMagnitude()/aSqNMagn;

    aDuS2 = Sign(sqrt(aDeltaU), aTgV.Dot(aN2));
    aDvS2 = -Sign(sqrt(aDeltaV), aTgU.Dot(aN2));
  }

  //According to "Marching along surface/surface intersection curves
  //with an adaptive step length"
  //by Tz.E.Stoyagov
  //(http://www.sciencedirect.com/science/article/pii/016783969290046R)
  //we obtain the system:
  //            {A*a+B*b=F1
  //            {B*a+C*b=F2
  //where a and b should be found.
  //After that, 2nd derivative of the intersection curve can be computed as
  //            r''(t)=a*aN1+b*aN2.

  const Standard_Real aA = aN1.Dot(aN1), aB = aN1.Dot(aN2), aC = aN2.Dot(aN2);
  const Standard_Real aDetSyst = aB*aB - aA*aC;

  if(Abs(aDetSyst) < aSmallValue)
  {//Indetermined system solution
    return -1.0;
  }

  const Standard_Real aF1 = aDuS1*aDuS1*aDUU1.Dot(aN1) + 
                            2.0*aDuS1*aDvS1*aDUV1.Dot(aN1) +
                            aDvS1*aDvS1*aDVV1.Dot(aN1);
  const Standard_Real aF2 = aDuS2*aDuS2*aDUU2.Dot(aN2) +
                            2.0*aDuS2*aDvS2*aDUV2.Dot(aN2) +
                            aDvS2*aDvS2*aDVV2.Dot(aN2);

  //Principal normal to the intersection curve
  const gp_Vec aCNorm((aF1*aC-aF2*aB)/aDetSyst*aN1 + (aA*aF2-aF1*aB)/aDetSyst*aN2);
  const Standard_Real aSqMagnSDer = aCNorm.CrossSquareMagnitude(aCTan);

  if(aSqMagnSDer < aSqSmallValue)
  {//Intersection curve has null curvature in observed point
    return Precision::Infinite();
  }

  //square of curvature radius
  const Standard_Real aFactSqRad = aSqMagnFDer*aSqMagnFDer*aSqMagnFDer/aSqMagnSDer;

#if 0
  if(aTestID)
  {
    if(Abs(aFactSqRad - anExpectedSqRad) < Precision::Confusion())
    {
      printf("OK: Curvature radius is equal to expected (%5.10g)", anExpectedSqRad);
    }
    else
    {
      printf("Error: Curvature radius is not equal to expected: %5.10g != %5.10g",
              aFactSqRad, anExpectedSqRad);
    }
  }
#endif

  return sqrt(aFactSqRad);
}
