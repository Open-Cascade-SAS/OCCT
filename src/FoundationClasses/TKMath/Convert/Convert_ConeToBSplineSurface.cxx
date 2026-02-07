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

#include <Convert_ConeToBSplineSurface.hxx>
#include <gp.hxx>
#include <gp_Cone.hxx>
#include <gp_Trsf.hxx>
#include <Standard_DomainError.hxx>

namespace
{
constexpr int TheUDegree  = 2;
constexpr int TheVDegree  = 1;
constexpr int TheNbUKnots = 5;
constexpr int TheNbVKnots = 2;
constexpr int TheNbUPoles = 9;
constexpr int TheNbVPoles = 2;
} // namespace

static void ComputePoles(const double                R,
                         const double                A,
                         const double                U1,
                         const double                U2,
                         const double                V1,
                         const double                V2,
                         NCollection_Array2<gp_Pnt>& Poles)
{
  double deltaU = U2 - U1;

  int i;

  // Number of spans : maximum opening = 150 degrees ( = PI / 1.2 rds)
  int    nbUSpans = (int)std::trunc(1.2 * deltaU / M_PI) + 1;
  double AlfaU    = deltaU / (nbUSpans * 2);

  double x[TheNbVPoles];
  double z[TheNbVPoles];

  x[0] = R + V1 * std::sin(A);
  z[0] = V1 * std::cos(A);
  x[1] = R + V2 * std::sin(A);
  z[1] = V2 * std::cos(A);

  double UStart = U1;
  Poles(1, 1)   = gp_Pnt(x[0] * std::cos(UStart), x[0] * std::sin(UStart), z[0]);
  Poles(1, 2)   = gp_Pnt(x[1] * std::cos(UStart), x[1] * std::sin(UStart), z[1]);

  for (i = 1; i <= nbUSpans; i++)
  {
    Poles(2 * i, 1) = gp_Pnt(x[0] * std::cos(UStart + AlfaU) / std::cos(AlfaU),
                             x[0] * std::sin(UStart + AlfaU) / std::cos(AlfaU),
                             z[0]);
    Poles(2 * i, 2) = gp_Pnt(x[1] * std::cos(UStart + AlfaU) / std::cos(AlfaU),
                             x[1] * std::sin(UStart + AlfaU) / std::cos(AlfaU),
                             z[1]);
    Poles(2 * i + 1, 1) =
      gp_Pnt(x[0] * std::cos(UStart + 2 * AlfaU), x[0] * std::sin(UStart + 2 * AlfaU), z[0]);
    Poles(2 * i + 1, 2) =
      gp_Pnt(x[1] * std::cos(UStart + 2 * AlfaU), x[1] * std::sin(UStart + 2 * AlfaU), z[1]);
    UStart += 2 * AlfaU;
  }
}

//=================================================================================================

Convert_ConeToBSplineSurface::Convert_ConeToBSplineSurface(const gp_Cone& C,
                                                           const double   U1,
                                                           const double   U2,
                                                           const double   V1,
                                                           const double   V2)
    : Convert_ElementarySurfaceToBSplineSurface(TheNbUPoles,
                                                TheNbVPoles,
                                                TheNbUKnots,
                                                TheNbVKnots,
                                                TheUDegree,
                                                TheVDegree)
{
  double deltaU = U2 - U1;
  Standard_DomainError_Raise_if((std::abs(V2 - V1) <= std::abs(Epsilon(V1))) || (deltaU > 2 * M_PI)
                                  || (deltaU < 0.),
                                "Convert_ConeToBSplineSurface");

  isuperiodic = false;
  isvperiodic = false;

  int i, j;
  // construction of cone in the reference mark xOy.

  // Number of spans : maximum opening = 150 degrees ( = PI / 1.2 rds)
  int    nbUSpans = (int)std::trunc(1.2 * deltaU / M_PI) + 1;
  double AlfaU    = deltaU / (nbUSpans * 2);

  nbUPoles = 2 * nbUSpans + 1;
  nbUKnots = nbUSpans + 1;

  nbVPoles = 2;
  nbVKnots = 2;

  double R = C.RefRadius();
  double A = C.SemiAngle();

  ComputePoles(R, A, U1, U2, V1, V2, poles);

  for (i = 1; i <= nbUKnots; i++)
  {
    uknots(i) = U1 + (i - 1) * 2 * AlfaU;
    umults(i) = 2;
  }
  umults(1)++;
  umults(nbUKnots)++;
  vknots(1) = V1;
  vmults(1) = 2;
  vknots(2) = V2;
  vmults(2) = 2;

  // Replace the bspline in the mark of the sphere.
  // and calculate the weight of the bspline.
  double  W1;
  gp_Trsf Trsf;
  Trsf.SetTransformation(C.Position(), gp::XOY());

  for (i = 1; i <= nbUPoles; i++)
  {
    if (i % 2 == 0)
      W1 = std::cos(AlfaU);
    else
      W1 = 1.;

    for (j = 1; j <= nbVPoles; j++)
    {
      weights(i, j) = W1;
      poles(i, j).Transform(Trsf);
    }
  }
}

//=================================================================================================

Convert_ConeToBSplineSurface::Convert_ConeToBSplineSurface(const gp_Cone& C,
                                                           const double   V1,
                                                           const double   V2)
    : Convert_ElementarySurfaceToBSplineSurface(TheNbUPoles,
                                                TheNbVPoles,
                                                TheNbUKnots,
                                                TheNbVKnots,
                                                TheUDegree,
                                                TheVDegree)
{
  Standard_DomainError_Raise_if(std::abs(V2 - V1) <= std::abs(Epsilon(V1)),
                                "Convert_ConeToBSplineSurface");

  int i, j;

  isuperiodic = true;
  isvperiodic = false;

  // construction of the cone in the reference mark xOy.

  double R = C.RefRadius();
  double A = C.SemiAngle();

  ComputePoles(R, A, 0., 2. * M_PI, V1, V2, poles);

  nbUPoles = 6;
  nbUKnots = 4;
  nbVPoles = 2;
  nbVKnots = 2;

  for (i = 1; i <= nbUKnots; i++)
  {
    uknots(i) = (i - 1) * 2. * M_PI / 3.;
    umults(i) = 2;
  }
  vknots(1) = V1;
  vmults(1) = 2;
  vknots(2) = V2;
  vmults(2) = 2;

  // replace bspline in the mark of the cone.
  // and calculate the weight of bspline.
  double  W;
  gp_Trsf Trsf;
  Trsf.SetTransformation(C.Position(), gp::XOY());

  for (i = 1; i <= nbUPoles; i++)
  {
    if (i % 2 == 0)
      W = 0.5; // = std::cos(pi /3)
    else
      W = 1.;

    for (j = 1; j <= nbVPoles; j++)
    {
      weights(i, j) = W;
      poles(i, j).Transform(Trsf);
    }
  }
}
