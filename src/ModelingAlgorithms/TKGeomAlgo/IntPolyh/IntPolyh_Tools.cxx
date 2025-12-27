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

#include <Adaptor3d_Surface.hxx>
#include <Precision.hxx>
#include <IntCurveSurface_ThePolyhedronOfHInter.hxx>

//=================================================================================================

void IntPolyh_Tools::IsEnlargePossible(const occ::handle<Adaptor3d_Surface>& theSurf,
                                       bool&                theUEnlarge,
                                       bool&                theVEnlarge)
{
  theUEnlarge = false;
  theVEnlarge = false;

  // In the context of IntPolyh_Intersection only BSpline and Bezier surfaces
  // should be enlarged
  if (theSurf->GetType() == GeomAbs_BSplineSurface || theSurf->GetType() == GeomAbs_BezierSurface)
  {
    // Check U periodicity and closeness
    if (!theSurf->IsUClosed() && !theSurf->IsUPeriodic())
    {
      // Check that surface is not infinite in U direction
      if (!Precision::IsInfinite(theSurf->FirstUParameter())
          && !Precision::IsInfinite(theSurf->LastUParameter()))
      {
        theUEnlarge = true;
      }
    }

    // Check V periodicity and closeness
    if (!theSurf->IsVClosed() && !theSurf->IsVPeriodic())
    {
      // Check that surface is not infinite in V direction
      if (!Precision::IsInfinite(theSurf->FirstVParameter())
          && !Precision::IsInfinite(theSurf->LastVParameter()))
      {
        theVEnlarge = true;
      }
    }
  }
}

//=======================================================================
// function : EnlargeZone
// purpose  : Enlarges the sampling zone of the surface
//=======================================================================
static void EnlargeZone(const occ::handle<Adaptor3d_Surface>& theSurf,
                        double&                   u0,
                        double&                   u1,
                        double&                   v0,
                        double&                   v1)
{
  bool isToEnlargeU, isToEnlargeV;
  IntPolyh_Tools::IsEnlargePossible(theSurf, isToEnlargeU, isToEnlargeV);
  // Enlarge U
  if (isToEnlargeU)
  {
    double delta_u = 0.01 * std::abs(u1 - u0);
    u0 -= delta_u;
    u1 += delta_u;
  }

  if (isToEnlargeV)
  {
    double delta_v = 0.01 * std::abs(v1 - v0);
    v0 -= delta_v;
    v1 += delta_v;
  }
}

//=================================================================================================

void IntPolyh_Tools::MakeSampling(const occ::handle<Adaptor3d_Surface>& theSurf,
                                  const int           theNbSU,
                                  const int           theNbSV,
                                  const bool           theEnlargeZone,
                                  NCollection_Array1<double>&            theUPars,
                                  NCollection_Array1<double>&            theVPars)
{
  // Resize arrays
  theUPars.Resize(1, theNbSU, false);
  theVPars.Resize(1, theNbSV, false);
  //
  double u0, u1, v0, v1;
  u0 = theSurf->FirstUParameter();
  u1 = theSurf->LastUParameter();
  v0 = theSurf->FirstVParameter();
  v1 = theSurf->LastVParameter();

  // Enlarge surface intersection zone if necessary
  if (theEnlargeZone)
    EnlargeZone(theSurf, u0, u1, v0, v1);

  int aNbSamplesU1 = theNbSU - 1;
  int aNbSamplesV1 = theNbSV - 1;

  // U step
  double dU = (u1 - u0) / double(aNbSamplesU1);
  // V step
  double dV = (v1 - v0) / double(aNbSamplesV1);

  // Fill arrays
  for (int i = 0; i < theNbSU; ++i)
  {
    double aU = u0 + i * dU;
    if (i == aNbSamplesU1)
    {
      aU = u1;
    }
    theUPars.SetValue(i + 1, aU);
  }
  //
  for (int i = 0; i < theNbSV; ++i)
  {
    double aV = v0 + i * dV;
    if (i == aNbSamplesV1)
    {
      aV = v1;
    }
    theVPars.SetValue(i + 1, aV);
  }
}

//=================================================================================================

double IntPolyh_Tools::ComputeDeflection(const occ::handle<Adaptor3d_Surface>& theSurf,
                                                const NCollection_Array1<double>&      theUPars,
                                                const NCollection_Array1<double>&      theVPars)
{
  IntCurveSurface_ThePolyhedronOfHInter polyhedron(theSurf, theUPars, theVPars);
  double                         aDeflTol = polyhedron.DeflectionOverEstimation();
  return aDeflTol;
}

//=================================================================================================

void IntPolyh_Tools::FillArrayOfPointNormal(const occ::handle<Adaptor3d_Surface>& theSurf,
                                            const NCollection_Array1<double>&      theUPars,
                                            const NCollection_Array1<double>&      theVPars,
                                            IntPolyh_ArrayOfPointNormal&     thePoints)
{
  int aNbU = theUPars.Length();
  int aNbV = theVPars.Length();
  int iCnt = 0;
  thePoints.Init(aNbU * aNbV);
  for (int i = 1; i <= aNbU; ++i)
  {
    double aU = theUPars(i);

    for (int j = 1; j <= aNbV; ++j)
    {
      double aV = theVPars(j);
      // Compute the point
      gp_Pnt aP;
      gp_Vec aDU, aDV;
      theSurf->D1(aU, aV, aP, aDU, aDV);
      // Compute normal
      gp_Vec        aVNorm  = aDU.Crossed(aDV);
      double aLength = aVNorm.Magnitude();
      if (aLength > gp::Resolution())
      {
        aVNorm /= aLength;
      }
      else
      {
        aVNorm.SetCoord(0.0, 0.0, 0.0);
      }

      // Save the pair
      IntPolyh_PointNormal& aPN = thePoints[iCnt];
      aPN.Point                 = aP;
      aPN.Normal                = aVNorm;

      ++iCnt;
    }
  }
  thePoints.SetNbItems(iCnt);
}
