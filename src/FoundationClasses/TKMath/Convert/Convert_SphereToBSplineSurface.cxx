// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// JCV 16/10/91

#include <Convert_SphereToBSplineSurface.hxx>
#include <gp.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>
#include <Standard_DomainError.hxx>

namespace
{
constexpr int TheUDegree  = 2;
constexpr int TheVDegree  = 2;
constexpr int MaxNbUKnots = 4;
constexpr int MaxNbVKnots = 3;
constexpr int MaxNbUPoles = 7;
constexpr int MaxNbVPoles = 5;
} // namespace

static void ComputePoles(const double                R,
                         const double                U1,
                         const double                U2,
                         const double                V1,
                         const double                V2,
                         NCollection_Array2<gp_Pnt>& Poles)
{
  double deltaU = U2 - U1;
  double deltaV = V2 - V1;

  int i, j;

  // Number of spans : maximum opening = 150 degrees ( = PI / 1.2 rds)
  int    nbUSpans = (int)std::trunc(1.2 * deltaU / M_PI) + 1;
  int    nbVSpans = (int)std::trunc(1.2 * deltaV / M_PI) + 1;
  double AlfaU    = deltaU / (nbUSpans * 2);
  double AlfaV    = deltaV / (nbVSpans * 2);

  int nbVP = 2 * nbVSpans + 1;

  double x[MaxNbVPoles];
  double z[MaxNbVPoles];

  x[0] = R * std::cos(V1);
  z[0] = R * std::sin(V1);

  double VStart = V1;
  for (i = 1; i <= nbVSpans; i++)
  {
    x[2 * i - 1] = R * std::cos(VStart + AlfaV) / std::cos(AlfaV);
    z[2 * i - 1] = R * std::sin(VStart + AlfaV) / std::cos(AlfaV);
    x[2 * i]     = R * std::cos(VStart + 2 * AlfaV);
    z[2 * i]     = R * std::sin(VStart + 2 * AlfaV);
    VStart += 2 * AlfaV;
  }

  double UStart = U1;
  for (j = 0; j <= nbVP - 1; j++)
  {
    Poles(1, j + 1) = gp_Pnt(x[j] * std::cos(UStart), x[j] * std::sin(UStart), z[j]);
  }

  for (i = 1; i <= nbUSpans; i++)
  {
    for (j = 0; j <= nbVP - 1; j++)
    {
      Poles(2 * i, j + 1) = gp_Pnt(x[j] * std::cos(UStart + AlfaU) / std::cos(AlfaU),
                                   x[j] * std::sin(UStart + AlfaU) / std::cos(AlfaU),
                                   z[j]);
      Poles(2 * i + 1, j + 1) =
        gp_Pnt(x[j] * std::cos(UStart + 2 * AlfaU), x[j] * std::sin(UStart + 2 * AlfaU), z[j]);
    }
    UStart += 2 * AlfaU;
  }
}

//=================================================================================================

Convert_SphereToBSplineSurface::Convert_SphereToBSplineSurface(const gp_Sphere& Sph,
                                                               const double     U1,
                                                               const double     U2,
                                                               const double     V1,
                                                               const double     V2)
    : Convert_ElementarySurfaceToBSplineSurface(MaxNbUPoles,
                                                MaxNbVPoles,
                                                MaxNbUKnots,
                                                MaxNbVKnots,
                                                TheUDegree,
                                                TheVDegree)
{
  double deltaU = U2 - U1;
  double deltaV = V2 - V1;
  Standard_DomainError_Raise_if((deltaU > 2 * M_PI) || (deltaU < 0.) || (V1 < -M_PI / 2.0)
                                  || (V2 > M_PI / 2),
                                "Convert_SphereToBSplineSurface");

  isuperiodic = false;
  isvperiodic = false;

  int i, j;
  // construction of the sphere in the reference mark xOy.

  // Number of spans : maximum opening = 150 degrees ( = PI / 1.2 rds)
  int    nbUSpans = (int)std::trunc(1.2 * deltaU / M_PI) + 1;
  int    nbVSpans = (int)std::trunc(1.2 * deltaV / M_PI) + 1;
  double AlfaU    = deltaU / (nbUSpans * 2);
  double AlfaV    = deltaV / (nbVSpans * 2);

  nbUPoles = 2 * nbUSpans + 1;
  nbVPoles = 2 * nbVSpans + 1;
  nbUKnots = nbUSpans + 1;
  nbVKnots = nbVSpans + 1;

  double R = Sph.Radius();

  ComputePoles(R, U1, U2, V1, V2, poles);

  for (i = 1; i <= nbUKnots; i++)
  {
    uknots(i) = U1 + (i - 1) * 2 * AlfaU;
    umults(i) = 2;
  }
  umults(1)++;
  umults(nbUKnots)++;
  for (i = 1; i <= nbVKnots; i++)
  {
    vknots(i) = V1 + (i - 1) * 2 * AlfaV;
    vmults(i) = 2;
  }
  vmults(1)++;
  vmults(nbVKnots)++;

  // Replace the bspline in the reference of the sphere.
  // and calculate the weight of the bspline.
  double  W1, W2;
  gp_Trsf Trsf;
  Trsf.SetTransformation(Sph.Position(), gp::XOY());

  for (i = 1; i <= nbUPoles; i++)
  {
    if (i % 2 == 0)
      W1 = std::cos(AlfaU);
    else
      W1 = 1.;

    for (j = 1; j <= nbVPoles; j++)
    {
      if (j % 2 == 0)
        W2 = std::cos(AlfaV);
      else
        W2 = 1.;

      weights(i, j) = W1 * W2;
      poles(i, j).Transform(Trsf);
    }
  }
}

//=================================================================================================

Convert_SphereToBSplineSurface::Convert_SphereToBSplineSurface(const gp_Sphere& Sph,
                                                               const double     Param1,
                                                               const double     Param2,
                                                               const bool       UTrim)
    : Convert_ElementarySurfaceToBSplineSurface(MaxNbUPoles,
                                                MaxNbVPoles,
                                                MaxNbUKnots,
                                                MaxNbVKnots,
                                                TheUDegree,
                                                TheVDegree)
{
#ifndef No_Exception
  double delta = Param2 - Param1;
#endif
  Standard_DomainError_Raise_if((delta > 2 * M_PI) || (delta < 0.),
                                "Convert_SphereToBSplineSurface");

  int    i, j;
  double deltaU, deltaV;

  isuperiodic = !UTrim;
  isvperiodic = false;

  double R = Sph.Radius();

  double W1, W2, CosU, CosV;

  if (isuperiodic)
  {
    ComputePoles(R, 0., 2. * M_PI, Param1, Param2, poles);

    nbUPoles = 6;
    nbUKnots = 4;

    deltaV          = Param2 - Param1;
    int    nbVSpans = (int)std::trunc(1.2 * deltaV / M_PI) + 1;
    double AlfaV    = deltaV / (nbVSpans * 2);
    nbVPoles        = 2 * nbVSpans + 1;
    nbVKnots        = nbVSpans + 1;

    for (i = 1; i <= nbUKnots; i++)
    {
      uknots(i) = (i - 1) * 2. * M_PI / 3.;
      umults(i) = 2;
    }
    for (i = 1; i <= nbVKnots; i++)
    {
      vknots(i) = Param1 + (i - 1) * 2 * AlfaV;
      vmults(i) = 2;
    }
    vmults(1)++;
    vmults(nbVKnots)++;

    CosU = 0.5; // = std::cos(pi /3)
    CosV = std::cos(AlfaV);
  }
  else
  {
    ComputePoles(R, Param1, Param2, -M_PI / 2., M_PI / 2., poles);

    nbVPoles = 5;
    nbVKnots = 3;

    deltaU          = Param2 - Param1;
    int    nbUSpans = (int)std::trunc(1.2 * deltaU / M_PI) + 1;
    double AlfaU    = deltaU / (nbUSpans * 2);
    nbUPoles        = 2 * nbUSpans + 1;
    nbUKnots        = nbUSpans + 1;

    vknots(1) = -M_PI / 2.;
    vmults(1) = 3;
    vknots(2) = 0.;
    vmults(2) = 2;
    vknots(3) = M_PI / 2.;
    vmults(3) = 3;
    for (i = 1; i <= nbUKnots; i++)
    {
      uknots(i) = Param1 + (i - 1) * 2 * AlfaU;
      umults(i) = 2;
    }
    umults(1)++;
    umults(nbUKnots)++;

    CosV = 0.5; // = std::cos(pi /3)
    CosU = std::cos(AlfaU);
  }

  // Replace the bspline in the mark of the sphere.
  // and calculate the weight of bspline.
  gp_Trsf Trsf;
  Trsf.SetTransformation(Sph.Position(), gp::XOY());

  for (i = 1; i <= nbUPoles; i++)
  {
    if (i % 2 == 0)
      W1 = CosU;
    else
      W1 = 1.;

    for (j = 1; j <= nbVPoles; j++)
    {
      if (j % 2 == 0)
        W2 = CosV;
      else
        W2 = 1.;

      weights(i, j) = W1 * W2;
      poles(i, j).Transform(Trsf);
    }
  }
}

//=================================================================================================

Convert_SphereToBSplineSurface::Convert_SphereToBSplineSurface(const gp_Sphere& Sph)
    : Convert_ElementarySurfaceToBSplineSurface(MaxNbUPoles,
                                                MaxNbVPoles,
                                                MaxNbUKnots,
                                                MaxNbVKnots,
                                                TheUDegree,
                                                TheVDegree)
{
  isuperiodic = true;
  isvperiodic = false;

  double W1, W2;
  int    i, j;

  nbUPoles = 6;
  nbVPoles = 5;
  nbUKnots = 4;
  nbVKnots = 3;

  // Construction of the sphere in the reference mark xOy.

  double R = Sph.Radius();

  ComputePoles(R, 0., 2. * M_PI, -M_PI / 2., M_PI / 2., poles);

  uknots(1) = 0.;
  uknots(2) = 2. * M_PI / 3.;
  uknots(3) = 4. * M_PI / 3.;
  uknots(4) = 2. * M_PI;
  vknots(1) = -M_PI / 2.;
  vknots(2) = 0.;
  vknots(3) = M_PI / 2.;
  for (i = 1; i <= 4; i++)
  {
    umults(i) = 2;
  }
  vmults(1) = vmults(3) = 3;
  vmults(2)             = 2;

  // Replace the bspline in the mark of the sphere.
  // and calculate the weight of the bspline.
  gp_Trsf Trsf;
  Trsf.SetTransformation(Sph.Position(), gp::XOY());

  for (i = 1; i <= nbUPoles; i++)
  {
    if (i % 2 == 0)
      W1 = 0.5;
    else
      W1 = 1.;

    for (j = 1; j <= nbVPoles; j++)
    {
      if (j % 2 == 0)
        W2 = std::sqrt(2.) / 2.;
      else
        W2 = 1.;

      weights(i, j) = W1 * W2;
      poles(i, j).Transform(Trsf);
    }
  }
}
