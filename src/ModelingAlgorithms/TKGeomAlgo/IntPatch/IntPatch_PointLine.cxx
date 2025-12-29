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

#include <Adaptor3d_Surface.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntPatch_PointLine, IntPatch_Line)

IntPatch_PointLine::IntPatch_PointLine(const bool              Tang,
                                       const IntSurf_TypeTrans Trans1,
                                       const IntSurf_TypeTrans Trans2)
    : IntPatch_Line(Tang, Trans1, Trans2)
{
}

IntPatch_PointLine::IntPatch_PointLine(const bool              Tang,
                                       const IntSurf_Situation Situ1,
                                       const IntSurf_Situation Situ2)
    : IntPatch_Line(Tang, Situ1, Situ2)
{
}

IntPatch_PointLine::IntPatch_PointLine(const bool Tang)
    : IntPatch_Line(Tang)
{
}

//=======================================================================
// function : CurvatureRadiusOfIntersLine
// purpose  :
//        ATTENTION!!!
//            Returns negative value if computation is not possible
//=======================================================================
double IntPatch_PointLine::CurvatureRadiusOfIntersLine(const occ::handle<Adaptor3d_Surface>& theS1,
                                                       const occ::handle<Adaptor3d_Surface>& theS2,
                                                       const IntSurf_PntOn2S& theUVPoint)
{
  constexpr double aSmallValue   = 1.0 / Precision::Infinite();
  constexpr double aSqSmallValue = aSmallValue * aSmallValue;

  double aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  theUVPoint.Parameters(aU1, aV1, aU2, aV2);
  gp_Pnt aPt;
  gp_Vec aDU1, aDV1, aDUU1, aDUV1, aDVV1;
  gp_Vec aDU2, aDV2, aDUU2, aDUV2, aDVV2;

  theS1->D2(aU1, aV1, aPt, aDU1, aDV1, aDUU1, aDVV1, aDUV1);
  theS2->D2(aU2, aV2, aPt, aDU2, aDV2, aDUU2, aDVV2, aDUV2);


  const gp_Vec aN1(aDU1.Crossed(aDV1)), aN2(aDU2.Crossed(aDV2));
  // Tangent vector to the intersection curve
  const gp_Vec aCTan(aN1.Crossed(aN2));
  const double aSqMagnFDer = aCTan.SquareMagnitude();

  if (aSqMagnFDer < 1.0e-8)
  {
    // Use 1.0e-4 (instead of aSmallValue) to provide
    // stable computation between different platforms.
    // See test bugs modalg_7 bug29807_sc01
    return -1.0;
  }

  double aDuS1 = 0.0, aDvS1 = 0.0, aDuS2 = 0.0, aDvS2 = 1.0;

  {
    // This algorithm is described in NonSingularProcessing() function
    // in ApproxInt_ImpPrmSvSurfaces.gxx file
    double aSqNMagn = aN1.SquareMagnitude();
    gp_Vec aTgU(aCTan.Crossed(aDU1)), aTgV(aCTan.Crossed(aDV1));
    double aDeltaU = aTgV.SquareMagnitude() / aSqNMagn;
    double aDeltaV = aTgU.SquareMagnitude() / aSqNMagn;

    aDuS1 = std::copysign(sqrt(aDeltaU), aTgV.Dot(aN1));
    aDvS1 = -std::copysign(sqrt(aDeltaV), aTgU.Dot(aN1));

    aSqNMagn = aN2.SquareMagnitude();
    aTgU.SetXYZ(aCTan.Crossed(aDU2).XYZ());
    aTgV.SetXYZ(aCTan.Crossed(aDV2).XYZ());
    aDeltaU = aTgV.SquareMagnitude() / aSqNMagn;
    aDeltaV = aTgU.SquareMagnitude() / aSqNMagn;

    aDuS2 = std::copysign(sqrt(aDeltaU), aTgV.Dot(aN2));
    aDvS2 = -std::copysign(sqrt(aDeltaV), aTgU.Dot(aN2));
  }

  // According to "Marching along surface/surface intersection curves
  // with an adaptive step length"
  // by Tz.E.Stoyagov
  //(http://www.sciencedirect.com/science/article/pii/016783969290046R)
  // we obtain the system:
  //             {A*a+B*b=F1
  //             {B*a+C*b=F2
  // where a and b should be found.
  // After that, 2nd derivative of the intersection curve can be computed as
  //             r''(t)=a*aN1+b*aN2.

  const double aA = aN1.Dot(aN1), aB = aN1.Dot(aN2), aC = aN2.Dot(aN2);
  const double aDetSyst = aB * aB - aA * aC;

  if (std::abs(aDetSyst) < aSmallValue)
  {
    // Undetermined system solution
    return -1.0;
  }

  const double aF1 = aDuS1 * aDuS1 * aDUU1.Dot(aN1) + 2.0 * aDuS1 * aDvS1 * aDUV1.Dot(aN1)
                     + aDvS1 * aDvS1 * aDVV1.Dot(aN1);
  const double aF2 = aDuS2 * aDuS2 * aDUU2.Dot(aN2) + 2.0 * aDuS2 * aDvS2 * aDUV2.Dot(aN2)
                     + aDvS2 * aDvS2 * aDVV2.Dot(aN2);

  // Principal normal to the intersection curve
  const gp_Vec aCNorm((aF1 * aC - aF2 * aB) / aDetSyst * aN1
                      + (aA * aF2 - aF1 * aB) / aDetSyst * aN2);
  const double aSqMagnSDer = aCNorm.CrossSquareMagnitude(aCTan);

  if (aSqMagnSDer < aSqSmallValue)
  { // Intersection curve has null curvature in observed point
    return Precision::Infinite();
  }

  // square of curvature radius
  const double aFactSqRad = aSqMagnFDer * aSqMagnFDer * aSqMagnFDer / aSqMagnSDer;


  return sqrt(aFactSqRad);
}
