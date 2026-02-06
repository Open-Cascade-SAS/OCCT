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

#include <Convert_TorusToBSplineSurface.hxx>
#include <gp.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <Standard_DomainError.hxx>

#include <array>

namespace
{
constexpr int TheUDegree  = 2;
constexpr int TheVDegree  = 2;
constexpr int MaxNbUKnots = 4;
constexpr int MaxNbVKnots = 4;
constexpr int MaxNbUPoles = 7;
constexpr int MaxNbVPoles = 7;
} // namespace

static void ComputePoles(const double                R,
                         const double                r,
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

  std::array<double, MaxNbVPoles> x;
  std::array<double, MaxNbVPoles> z;

  x[0] = R + r * std::cos(V1);
  z[0] = r * std::sin(V1);

  double VStart = V1;
  for (i = 1; i <= nbVSpans; i++)
  {
    x[2 * i - 1] = R + r * std::cos(VStart + AlfaV) / std::cos(AlfaV);
    z[2 * i - 1] = r * std::sin(VStart + AlfaV) / std::cos(AlfaV);
    x[2 * i]     = R + r * std::cos(VStart + 2 * AlfaV);
    z[2 * i]     = r * std::sin(VStart + 2 * AlfaV);
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

Convert_TorusToBSplineSurface::Convert_TorusToBSplineSurface(const gp_Torus& T,
                                                             const double    U1,
                                                             const double    U2,
                                                             const double    V1,
                                                             const double    V2)
    : Convert_ElementarySurfaceToBSplineSurface(MaxNbUPoles,
                                                MaxNbVPoles,
                                                MaxNbUKnots,
                                                MaxNbVKnots,
                                                TheUDegree,
                                                TheVDegree)
{
  double deltaU = U2 - U1;
  double deltaV = V2 - V1;
  Standard_DomainError_Raise_if((deltaU > 2 * M_PI) || (deltaU < 0.) || (deltaV > 2 * M_PI)
                                  || (deltaV < 0.),
                                "Convert_TorusToBSplineSurface");

  myData.IsUPeriodic = false;
  myData.IsVPeriodic = false;

  int i, j;
  // construction of the torus in the reference mark xOy.

  // Number of spans : maximum opening = 150 degrees ( = PI / 1.2 rds)
  int    nbUSpans = (int)std::trunc(1.2 * deltaU / M_PI) + 1;
  int    nbVSpans = (int)std::trunc(1.2 * deltaV / M_PI) + 1;
  double AlfaU    = deltaU / (nbUSpans * 2);
  double AlfaV    = deltaV / (nbVSpans * 2);

  myNbUPoles = 2 * nbUSpans + 1;
  myNbVPoles = 2 * nbVSpans + 1;
  myNbUKnots = nbUSpans + 1;
  myNbVKnots = nbVSpans + 1;

  double R = T.MajorRadius();
  double r = T.MinorRadius();

  ComputePoles(R, r, U1, U2, V1, V2, myData.Poles);

  for (i = 1; i <= myNbUKnots; i++)
  {
    myData.UKnots(i) = U1 + (i - 1) * 2 * AlfaU;
    myData.UMults(i) = 2;
  }
  myData.UMults(1)++;
  myData.UMults(myNbUKnots)++;
  for (i = 1; i <= myNbVKnots; i++)
  {
    myData.VKnots(i) = V1 + (i - 1) * 2 * AlfaV;
    myData.VMults(i) = 2;
  }
  myData.VMults(1)++;
  myData.VMults(myNbVKnots)++;

  // Replace the bspline in the reference of the torus.
  // and calculate the weight of the bspline.
  double  W1, W2;
  gp_Trsf Trsf;
  Trsf.SetTransformation(T.Position(), gp::XOY());

  for (i = 1; i <= myNbUPoles; i++)
  {
    if (i % 2 == 0)
      W1 = std::cos(AlfaU);
    else
      W1 = 1.;

    for (j = 1; j <= myNbVPoles; j++)
    {
      if (j % 2 == 0)
        W2 = std::cos(AlfaV);
      else
        W2 = 1.;

      myData.Weights(i, j) = W1 * W2;
      myData.Poles(i, j).Transform(Trsf);
    }
  }
  Finalize();
}

//=================================================================================================

Convert_TorusToBSplineSurface::Convert_TorusToBSplineSurface(const gp_Torus& T,
                                                             const double    Param1,
                                                             const double    Param2,
                                                             const bool      UTrim)
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
                                "Convert_TorusToBSplineSurface");

  int    i, j;
  double deltaU, deltaV;

  myData.IsUPeriodic = !UTrim;
  myData.IsVPeriodic = UTrim;

  double R = T.MajorRadius();
  double r = T.MinorRadius();

  double W1, W2, CosU, CosV;

  if (myData.IsUPeriodic)
  {
    ComputePoles(R, r, 0, 2. * M_PI, Param1, Param2, myData.Poles);

    myNbUPoles = 6;
    myNbUKnots = 4;

    deltaV          = Param2 - Param1;
    int    nbVSpans = (int)std::trunc(1.2 * deltaV / M_PI) + 1;
    double AlfaV    = deltaV / (nbVSpans * 2);
    myNbVPoles      = 2 * nbVSpans + 1;
    myNbVKnots      = nbVSpans + 1;

    for (i = 1; i <= myNbUKnots; i++)
    {
      myData.UKnots(i) = (i - 1) * 2. * M_PI / 3.;
      myData.UMults(i) = 2;
    }
    for (i = 1; i <= myNbVKnots; i++)
    {
      myData.VKnots(i) = Param1 + (i - 1) * 2 * AlfaV;
      myData.VMults(i) = 2;
    }
    myData.VMults(1)++;
    myData.VMults(myNbVKnots)++;

    CosU = 0.5; // = std::cos(pi /3)
    CosV = std::cos(AlfaV);
  }
  else
  {
    ComputePoles(R, r, Param1, Param2, 0., 2. * M_PI, myData.Poles);

    myNbVPoles = 6;
    myNbVKnots = 4;

    deltaU          = Param2 - Param1;
    int    nbUSpans = (int)std::trunc(1.2 * deltaU / M_PI) + 1;
    double AlfaU    = deltaU / (nbUSpans * 2);
    myNbUPoles      = 2 * nbUSpans + 1;
    myNbUKnots      = nbUSpans + 1;

    for (i = 1; i <= myNbVKnots; i++)
    {
      myData.VKnots(i) = (i - 1) * 2. * M_PI / 3.;
      myData.VMults(i) = 2;
    }
    for (i = 1; i <= myNbUKnots; i++)
    {
      myData.UKnots(i) = Param1 + (i - 1) * 2 * AlfaU;
      myData.UMults(i) = 2;
    }
    myData.UMults(1)++;
    myData.UMults(myNbUKnots)++;

    CosV = 0.5; // = std::cos(pi /3)
    CosU = std::cos(AlfaU);
  }

  // Replace the bspline in the reference of the torus.
  // and calculate the weight of the bspline.
  gp_Trsf Trsf;
  Trsf.SetTransformation(T.Position(), gp::XOY());

  for (i = 1; i <= myNbUPoles; i++)
  {
    if (i % 2 == 0)
      W1 = CosU;
    else
      W1 = 1.;

    for (j = 1; j <= myNbVPoles; j++)
    {
      if (j % 2 == 0)
        W2 = CosV;
      else
        W2 = 1.;

      myData.Weights(i, j) = W1 * W2;
      myData.Poles(i, j).Transform(Trsf);
    }
  }
  Finalize();
}

//=================================================================================================

Convert_TorusToBSplineSurface::Convert_TorusToBSplineSurface(const gp_Torus& T)
    : Convert_ElementarySurfaceToBSplineSurface(MaxNbUPoles,
                                                MaxNbVPoles,
                                                MaxNbUKnots,
                                                MaxNbVKnots,
                                                TheUDegree,
                                                TheVDegree)
{
  myData.IsUPeriodic = true;
  myData.IsVPeriodic = true;

  double W1, W2;
  int    i, j;

  myNbUPoles = 6;
  myNbVPoles = 6;
  myNbUKnots = 4;
  myNbVKnots = 4;

  // Construction of the Torus in the reference mark xOy.

  double R = T.MajorRadius();
  double r = T.MinorRadius();

  ComputePoles(R, r, 0., 2. * M_PI, 0., 2. * M_PI, myData.Poles);

  myData.UKnots(1) = myData.VKnots(1) = 0.;
  myData.UKnots(2) = myData.VKnots(2) = 2. * M_PI / 3.;
  myData.UKnots(3) = myData.VKnots(3) = 4. * M_PI / 3.;
  myData.UKnots(4) = myData.VKnots(4) = 2. * M_PI;
  for (i = 1; i <= 4; i++)
  {
    myData.UMults(i) = myData.VMults(i) = 2;
  }

  // Replace the bspline in the mark of the torus.
  // and calculate the weight of the bspline.
  gp_Trsf Trsf;
  Trsf.SetTransformation(T.Position(), gp::XOY());

  for (i = 1; i <= myNbUPoles; i++)
  {
    if (i % 2 == 0)
      W1 = 0.5;
    else
      W1 = 1.;

    for (j = 1; j <= myNbVPoles; j++)
    {
      if (j % 2 == 0)
        W2 = 0.5;
      else
        W2 = 1.;

      myData.Weights(i, j) = W1 * W2;
      myData.Poles(i, j).Transform(Trsf);
    }
  }
  Finalize();
}
