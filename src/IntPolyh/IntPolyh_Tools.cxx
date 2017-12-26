// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <IntPolyh_Tools.hxx>

#include <Adaptor3d_HSurface.hxx>
#include <Precision.hxx>
#include <IntCurveSurface_ThePolyhedronOfHInter.hxx>

//=======================================================================
//function : IsEnlargePossible
//purpose  :
//=======================================================================
void IntPolyh_Tools::IsEnlargePossible(const Handle(Adaptor3d_HSurface)& theSurf,
                                       Standard_Boolean& theUEnlarge,
                                       Standard_Boolean& theVEnlarge)
{
  theUEnlarge = Standard_False;
  theVEnlarge = Standard_False;

  // In the context of IntPolyh_Intersection only BSpline and Bezier surfaces
  // should be enlarged
  if (theSurf->GetType() == GeomAbs_BSplineSurface ||
      theSurf->GetType() == GeomAbs_BezierSurface)
  {
    // Check U periodicity and closeness
    if (!theSurf->IsUClosed() && !theSurf->IsUPeriodic())
    {
      // Check that surface is not infinite in U direction
      if (!Precision::IsInfinite(theSurf->FirstUParameter()) &&
          !Precision::IsInfinite(theSurf->LastUParameter()))
      {
        theUEnlarge = Standard_True;
      }
    }

    // Check V periodicity and closeness
    if (!theSurf->IsVClosed() && !theSurf->IsVPeriodic())
    {
      // Check that surface is not infinite in V direction
      if (!Precision::IsInfinite(theSurf->FirstVParameter()) &&
          !Precision::IsInfinite(theSurf->LastVParameter()))
      {
        theVEnlarge = Standard_True;
      }
    }
  }
}

//=======================================================================
//function : EnlargeZone
//purpose  : Enlarges the sampling zone of the surface
//=======================================================================
static void EnlargeZone(const Handle(Adaptor3d_HSurface)& theSurf,
                        Standard_Real &u0,
                        Standard_Real &u1,
                        Standard_Real &v0,
                        Standard_Real &v1)
{
  Standard_Boolean isToEnlargeU, isToEnlargeV;
  IntPolyh_Tools::IsEnlargePossible(theSurf, isToEnlargeU, isToEnlargeV);
  // Enlarge U
  if (isToEnlargeU)
  {
    Standard_Real delta_u = 0.01*Abs(u1 - u0);
    u0 -= delta_u;
    u1 += delta_u;
  }

  if (isToEnlargeV)
  {
    Standard_Real delta_v = 0.01*Abs(v1 - v0);
    v0 -= delta_v;
    v1 += delta_v;
  }
}

//=======================================================================
//function : MakeSampling
//purpose  :
//=======================================================================
void IntPolyh_Tools::MakeSampling(const Handle(Adaptor3d_HSurface)& theSurf,
                                  const Standard_Integer theNbSU,
                                  const Standard_Integer theNbSV,
                                  const Standard_Boolean theEnlargeZone,
                                  TColStd_Array1OfReal& theUPars,
                                  TColStd_Array1OfReal& theVPars)
{
  // Resize arrays
  theUPars.Resize(1, theNbSU, Standard_False);
  theVPars.Resize(1, theNbSV, Standard_False);
  //
  Standard_Real u0, u1, v0, v1;
  u0 = theSurf->FirstUParameter();
  u1 = theSurf->LastUParameter();
  v0 = theSurf->FirstVParameter();
  v1 = theSurf->LastVParameter();

  // Enlarge surface intersection zone if necessary
  if (theEnlargeZone)
    EnlargeZone(theSurf, u0, u1, v0, v1);

  Standard_Integer aNbSamplesU1 = theNbSU - 1;
  Standard_Integer aNbSamplesV1 = theNbSV - 1;

  // U step
  Standard_Real dU = (u1 - u0) / Standard_Real(aNbSamplesU1);
  // V step
  Standard_Real dV = (v1 - v0) / Standard_Real(aNbSamplesV1);

  // Fill arrays
  for (Standard_Integer i = 0; i < theNbSU; ++i) {
    Standard_Real aU = u0 + i*dU;
    if (i == aNbSamplesU1) {
      aU = u1;
    }
    theUPars.SetValue(i + 1, aU);
  }
  //
  for (Standard_Integer i = 0; i < theNbSV; ++i) {
    Standard_Real aV = v0 + i*dV;
    if (i == aNbSamplesV1) {
      aV = v1;
    }
    theVPars.SetValue(i + 1, aV);
  }
}

//=======================================================================
//function : ComputeDeflection
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_Tools::ComputeDeflection(const Handle(Adaptor3d_HSurface)& theSurf,
                                                const TColStd_Array1OfReal& theUPars,
                                                const TColStd_Array1OfReal& theVPars)
{
  IntCurveSurface_ThePolyhedronOfHInter polyhedron(theSurf, theUPars, theVPars);
  Standard_Real aDeflTol = polyhedron.DeflectionOverEstimation();
  return aDeflTol;
}

//=======================================================================
//function : FillArrayOfPointNormal
//purpose  : 
//=======================================================================
void IntPolyh_Tools::FillArrayOfPointNormal(const Handle(Adaptor3d_HSurface)& theSurf,
                                            const TColStd_Array1OfReal& theUPars,
                                            const TColStd_Array1OfReal& theVPars,
                                            IntPolyh_ArrayOfPointNormal& thePoints)
{
  Standard_Integer aNbU = theUPars.Length();
  Standard_Integer aNbV = theVPars.Length();
  Standard_Integer iCnt = 0;
  thePoints.Init(aNbU * aNbV);
  for (Standard_Integer i = 1; i <= aNbU; ++i)
  {
    Standard_Real aU = theUPars(i);

    for (Standard_Integer j = 1; j <= aNbV; ++j)
    {
      Standard_Real aV = theVPars(j);
      // Compute the point
      gp_Pnt aP;
      gp_Vec aDU, aDV;
      theSurf->D1(aU, aV, aP, aDU, aDV);
      // Compute normal
      gp_Vec aVNorm = aDU.Crossed(aDV);
      Standard_Real aLength = aVNorm.Magnitude();
      if (aLength > gp::Resolution())
      {
        aVNorm /= aLength;
      }
      else
      {
        aVNorm.SetCoord(0.0, 0.0, 0.0);
      }

      // Save the pair
      IntPolyh_PointNormal& aPN  = thePoints[iCnt];
      aPN.Point = aP;
      aPN.Normal = aVNorm;

      ++iCnt;
    }
  }
  thePoints.SetNbItems(iCnt);
}
