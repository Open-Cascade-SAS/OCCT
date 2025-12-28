// Created on: 1994-02-18
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Standard_Macro.hxx>

#include <iostream>

#include <iomanip>

#include <fstream>
#include <BRepTest.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <DBRep.hxx>
#include <BRepGProp.hxx>
#include <TopoDS_Shape.hxx>
#include <GProp_PrincipalProps.hxx>

#include <Draw_Axis3D.hxx>
#include <Precision.hxx>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

int props(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
  {
    di << "Use: " << a[0] << " shape [epsilon] [c[losed]] [x y z] [-skip] [-full] [-tri]\n";
    di << "Compute properties of the shape, exact geometry (curves, surfaces) or\n";
    di << "some discrete data (polygons, triangulations) can be used for calculations\n";
    di << "The epsilon, if given, defines relative precision of computation\n";
    di << "The \"closed\" flag, if present, do computation only closed shells of the shape\n";
    di << "The centroid coordinates will be put to DRAW variables x y z (if given)\n";
    di << "Shared entities will be take in account only one time in the skip mode\n";
    di << "All values are outputted with the full precision in the full mode.\n";
    di << "Preferable source of geometry data are triangulations in case if it exists, if the -tri "
          "key is used.\n";
    di << "If epsilon is given, exact geometry (curves, surfaces) are used for calculations "
          "independently of using key -tri\n\n";
    return 1;
  }

  bool UseTriangulation = false;
  if (n >= 2 && strcmp(a[n - 1], "-tri") == 0)
  {
    UseTriangulation = true;
    --n;
  }
  bool isFullMode = false;
  if (n >= 2 && strcmp(a[n - 1], "-full") == 0)
  {
    isFullMode = true;
    --n;
  }
  bool SkipShared = false;
  if (n >= 2 && strcmp(a[n - 1], "-skip") == 0)
  {
    SkipShared = true;
    --n;
  }

  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull())
    return 0;

  GProp_GProps G;

  bool   onlyClosed = false;
  double eps        = 1.0;
  bool   witheps    = false;
  if ((n > 2 && *a[2] == 'c') || (n > 3 && *a[3] == 'c'))
    onlyClosed = true;
  if (n > 2 && *a[2] != 'c' && n != 5)
  {
    eps     = Draw::Atof(a[2]);
    witheps = true;
  }

  if (witheps)
  {
    if (std::abs(eps) < Precision::Angular())
      return 2;
    if (*a[0] == 'l')
      BRepGProp::LinearProperties(S, G, SkipShared);
    else if (*a[0] == 's')
      eps = BRepGProp::SurfaceProperties(S, G, eps, SkipShared);
    else
      eps = BRepGProp::VolumeProperties(S, G, eps, onlyClosed, SkipShared);
  }
  else
  {
    if (*a[0] == 'l')
      BRepGProp::LinearProperties(S, G, SkipShared, UseTriangulation);
    else if (*a[0] == 's')
      BRepGProp::SurfaceProperties(S, G, SkipShared, UseTriangulation);
    else
      BRepGProp::VolumeProperties(S, G, onlyClosed, SkipShared, UseTriangulation);
  }

  gp_Pnt P = G.CentreOfMass();
  gp_Mat I = G.MatrixOfInertia();

  if (n >= 5)
  {
    int shift = n - 5;
    Draw::Set(a[shift + 2], P.X());
    Draw::Set(a[shift + 3], P.Y());
    Draw::Set(a[shift + 4], P.Z());
  }

  GProp_PrincipalProps Pr = G.PrincipalProperties();
  double               Ix, Iy, Iz;
  Pr.Moments(Ix, Iy, Iz);

  if (!isFullMode)
  {
    Standard_SStream aSStream1;
    aSStream1 << "\n\n";
    aSStream1 << "Mass : " << std::setw(15) << G.Mass() << "\n\n";
    if (witheps && *a[0] != 'l')
      aSStream1 << "Relative error of mass computation : " << std::setw(15) << eps << "\n\n";

    aSStream1 << "Center of gravity : \n";
    aSStream1 << "X = " << std::setw(15) << P.X() << "\n";
    aSStream1 << "Y = " << std::setw(15) << P.Y() << "\n";
    aSStream1 << "Z = " << std::setw(15) << P.Z() << "\n";
    aSStream1 << "\n";

    aSStream1 << "Matrix of Inertia : \n";
    aSStream1 << std::setw(15) << I(1, 1);
    aSStream1 << " " << std::setw(15) << I(1, 2);
    aSStream1 << " " << std::setw(15) << I(1, 3) << "\n";
    aSStream1 << std::setw(15) << I(2, 1);
    aSStream1 << " " << std::setw(15) << I(2, 2);
    aSStream1 << " " << std::setw(15) << I(2, 3) << "\n";
    aSStream1 << std::setw(15) << I(3, 1);
    aSStream1 << " " << std::setw(15) << I(3, 2);
    aSStream1 << " " << std::setw(15) << I(3, 3) << "\n";
    aSStream1 << "\n";
    aSStream1 << std::ends;
    di << aSStream1;

    Standard_SStream aSStream2;
    aSStream2 << "Moments : \n";
    aSStream2 << "IX = " << std::setw(15) << Ix << "\n";
    aSStream2 << "IY = " << std::setw(15) << Iy << "\n";
    aSStream2 << "IZ = " << std::setw(15) << Iz << "\n";
    aSStream2 << "\n";
    aSStream2 << std::ends;
    di << aSStream2;
  }
  else
  {
    di << "\n\nMass : " << G.Mass() << "\n\n";
    if (witheps && *a[0] != 'l')
    {
      di << "Relative error of mass computation : " << eps << "\n\n";
    }

    di << "Center of gravity : \n";
    di << "X = " << P.X() << "\n";
    di << "Y = " << P.Y() << "\n";
    di << "Z = " << P.Z() << "\n\n";

    di << "Matrix of Inertia :\n";
    di << I(1, 1) << "    " << I(1, 2) << "    " << I(1, 3) << "\n";
    di << I(2, 1) << "    " << I(2, 2) << "    " << I(2, 3) << "\n";
    di << I(3, 1) << "    " << I(3, 2) << "    " << I(3, 3) << "\n\n";

    di << "Moments :\n";
    di << "IX = " << Ix << "\n";
    di << "IY = " << Iy << "\n";
    di << "IZ = " << Iz << "\n\n";
  }

  // if (n == 2) {
  gp_Ax2 axes(P, Pr.ThirdAxisOfInertia(), Pr.FirstAxisOfInertia());

  occ::handle<Draw_Axis3D> Dax = new Draw_Axis3D(axes, Draw_orange, 30);
  dout << Dax;
  //}

  return 0;
}

int vpropsgk(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
  {
    di << "Use: " << a[0] << " shape epsilon closed span mode [x y z] [-skip]\n";
    di << "Compute properties of the shape\n";
    di << "The epsilon defines relative precision of computation\n";
    di << "The \"closed\" flag, if equal 1, causes computation only closed shells of the shape\n";
    di << "The \"span\" flag, if equal 1, says that computation is performed on spans\n";
    di << "      This option makes effect only for BSpline surfaces.\n";
    di << "mode can be 0 - only volume calculations\n";
    di << "            1 - volume and gravity center\n";
    di << "            2 - volume, gravity center and matrix of inertia\n";
    di << "The centroid coordinates will be put to DRAW variables x y z (if given)\n\n";
    return 1;
  }

  if (n > 2 && n < 6)
  {
    di << "Wrong arguments\n";
    return 1;
  }

  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull())
    return 0;

  GProp_GProps G;
  bool         SkipShared = false;
  if (n >= 2 && strcmp(a[n - 1], "-skip") == 0)
  {
    SkipShared = true;
    --n;
  }

  bool   onlyClosed = false;
  bool   isUseSpan  = false;
  bool   CGFlag     = false;
  bool   IFlag      = false;
  double eps        = 1.e-3;
  // double    aDefaultTol = 1.e-3;
  int mode = 0;

  eps  = Draw::Atof(a[2]);
  mode = Draw::Atoi(a[3]);
  if (mode > 0)
    onlyClosed = true;
  mode = Draw::Atoi(a[4]);
  if (mode > 0)
    isUseSpan = true;

  mode = Draw::Atoi(a[5]);
  if (mode == 1 || mode == 3)
    CGFlag = true;
  if (mode == 2 || mode == 3)
    IFlag = true;

  // OSD_Chronometer aChrono;

  // aChrono.Reset();
  // aChrono.Start();
  eps = BRepGProp::VolumePropertiesGK(S, G, eps, onlyClosed, isUseSpan, CGFlag, IFlag, SkipShared);
  // aChrono.Stop();

  Standard_SStream aSStream0;
  int              anOutWidth = 24;

  aSStream0.precision(15);
  aSStream0 << "\n\n";
  aSStream0 << "Mass : " << std::setw(anOutWidth) << G.Mass() << "\n\n";
  aSStream0 << "Relative error of mass computation : " << std::setw(anOutWidth) << eps << "\n\n";
  aSStream0 << std::ends;
  di << aSStream0;

  if (CGFlag || IFlag)
  {
    Standard_SStream aSStream1;
    gp_Pnt           P = G.CentreOfMass();
    if (n > 6)
    {
      Draw::Set(a[6], P.X());
    }
    if (n > 7)
    {
      Draw::Set(a[7], P.Y());
    }
    if (n > 8)
    {
      Draw::Set(a[8], P.Z());
    }

    aSStream1.precision(15);
    aSStream1 << "Center of gravity : \n";
    aSStream1 << "X = " << std::setw(anOutWidth) << P.X() << "\n";
    aSStream1 << "Y = " << std::setw(anOutWidth) << P.Y() << "\n";
    aSStream1 << "Z = " << std::setw(anOutWidth) << P.Z() << "\n";
    aSStream1 << "\n";

    if (IFlag)
    {
      gp_Mat I = G.MatrixOfInertia();

      aSStream1 << "Matrix of Inertia : \n";
      aSStream1 << std::setw(anOutWidth) << I(1, 1);
      aSStream1 << " " << std::setw(anOutWidth) << I(1, 2);
      aSStream1 << " " << std::setw(anOutWidth) << I(1, 3) << "\n";
      aSStream1 << std::setw(anOutWidth) << I(2, 1);
      aSStream1 << " " << std::setw(anOutWidth) << I(2, 2);
      aSStream1 << " " << std::setw(anOutWidth) << I(2, 3) << "\n";
      aSStream1 << std::setw(anOutWidth) << I(3, 1);
      aSStream1 << " " << std::setw(anOutWidth) << I(3, 2);
      aSStream1 << " " << std::setw(anOutWidth) << I(3, 3) << "\n";
      aSStream1 << "\n";
    }
    aSStream1 << std::ends;
    di << aSStream1;
  }

  if (IFlag)
  {

    GProp_PrincipalProps Pr = G.PrincipalProperties();

    double Ix, Iy, Iz;
    Pr.Moments(Ix, Iy, Iz);
    gp_Pnt P = G.CentreOfMass();

    Standard_SStream aSStream2;

    aSStream2.precision(15);
    aSStream2 << "Moments : \n";
    aSStream2 << "IX = " << std::setw(anOutWidth) << Ix << "\n";
    aSStream2 << "IY = " << std::setw(anOutWidth) << Iy << "\n";
    aSStream2 << "IZ = " << std::setw(anOutWidth) << Iz << "\n";
    aSStream2 << "\n";
    aSStream2 << "\n";
    aSStream2 << std::ends;
    di << aSStream2;

    gp_Ax2 axes(P, Pr.ThirdAxisOfInertia(), Pr.FirstAxisOfInertia());

    occ::handle<Draw_Axis3D> Dax = new Draw_Axis3D(axes, Draw_orange, 30);
    dout << Dax;
  }
  return 0;
}

//=================================================================================================

void BRepTest::GPropCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;
  done = true;

  DBRep::BasicCommands(theCommands);

  const char* g = "Global properties";
  theCommands.Add("lprops",
                  "lprops name [x y z] [-skip] [-full] [-tri]: compute linear properties",
                  __FILE__,
                  props,
                  g);
  theCommands.Add("sprops",
                  "sprops name [epsilon] [x y z] [-skip] [-full] [-tri]:\n"
                  "  compute surfacic properties",
                  __FILE__,
                  props,
                  g);
  theCommands.Add("vprops",
                  "vprops name [epsilon] [c[losed]] [x y z] [-skip] [-full] [-tri]:\n"
                  "  compute volumic properties",
                  __FILE__,
                  props,
                  g);

  theCommands.Add(
    "vpropsgk",
    "vpropsgk name epsilon closed span mode [x y z] [-skip] : compute volumic properties",
    __FILE__,
    vpropsgk,
    g);
}
