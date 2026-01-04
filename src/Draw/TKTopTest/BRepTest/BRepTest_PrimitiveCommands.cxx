// Created on: 1993-07-22
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepTest.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepPreviewAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeWedge.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <Message.hxx>

//=======================================================================
// box
//=======================================================================

static int box(Draw_Interpretor&, int n, const char** a)
{
  gp_Pnt anOrigin;
  gp_XYZ aParams;
  gp_Dir aDir;
  gp_Dir aXDir;
  bool   isMinMax  = false;
  bool   isPreview = false;
  bool   isAxis    = false;

  for (int anArgIter = 2; anArgIter < n; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(a[anArgIter]);
    anArgCase.LowerCase();
    if (anArgCase == "-min" && anArgIter + 3 <= n)
    {
      anOrigin.SetX(Draw::Atof(a[anArgIter + 1]));
      anOrigin.SetY(Draw::Atof(a[anArgIter + 2]));
      anOrigin.SetZ(Draw::Atof(a[anArgIter + 3]));
      anArgIter += 3;
    }
    else if (anArgCase == "-max" && anArgIter + 3 <= n)
    {
      aParams.SetX(Draw::Atof(a[anArgIter + 1]));
      aParams.SetY(Draw::Atof(a[anArgIter + 2]));
      aParams.SetZ(Draw::Atof(a[anArgIter + 3]));
      isMinMax = true;
      anArgIter += 3;
    }
    else if (anArgCase == "-size" && anArgIter + 3 <= n)
    {
      aParams.SetX(Draw::Atof(a[anArgIter + 1]));
      aParams.SetY(Draw::Atof(a[anArgIter + 2]));
      aParams.SetZ(Draw::Atof(a[anArgIter + 3]));
      isMinMax = false;
      anArgIter += 3;
    }
    else if (anArgCase == "-dir" && anArgIter + 3 <= n)
    {
      double aX  = Draw::Atof(a[anArgIter + 1]);
      double anY = Draw::Atof(a[anArgIter + 2]);
      double aZ  = Draw::Atof(a[anArgIter + 3]);
      aDir.SetCoord(aX, anY, aZ);
      isAxis = true;
      anArgIter += 3;
    }
    else if (anArgCase == "-xdir" && anArgIter + 3 <= n)
    {
      double aX  = Draw::Atof(a[anArgIter + 1]);
      double anY = Draw::Atof(a[anArgIter + 2]);
      double aZ  = Draw::Atof(a[anArgIter + 3]);
      aXDir.SetCoord(aX, anY, aZ);
      isAxis = true;
      anArgIter += 3;
    }
    else if (anArgCase == "-preview")
    {
      isPreview = true;
    }
    else if (anArgIter + 5 < n || anArgIter + 2 < n)
    {
      double aValue     = 0.0;
      int    aCountReal = 0;
      int    anIter     = anArgIter;
      while (anIter < n && Draw::ParseReal(a[anIter], aValue))
      {
        anIter++;
        aCountReal++;
      }

      if (aCountReal == 6)
      {
        anOrigin.SetX(Draw::Atof(a[anArgIter]));
        anOrigin.SetY(Draw::Atof(a[anArgIter + 1]));
        anOrigin.SetZ(Draw::Atof(a[anArgIter + 2]));

        aParams.SetX(Draw::Atof(a[anArgIter + 3]));
        aParams.SetY(Draw::Atof(a[anArgIter + 4]));
        aParams.SetZ(Draw::Atof(a[anArgIter + 5]));
        anArgIter += 5;
      }

      else if (aCountReal == 3)
      {
        aParams.SetX(Draw::Atof(a[anArgIter]));
        aParams.SetY(Draw::Atof(a[anArgIter + 1]));
        aParams.SetZ(Draw::Atof(a[anArgIter + 2]));
        anArgIter += 2;
      }
      else
      {
        Message::SendFail() << "Syntax error";
        return 1;
      }
    }
  }

  if (isPreview)
  {
    TopoDS_Shape           S;
    BRepPreviewAPI_MakeBox aPreview;

    if (isMinMax)
    {
      aPreview.Init(anOrigin, aParams);
    }
    else if (!isMinMax && !isAxis)
    {
      aPreview.Init(anOrigin, aParams.X(), aParams.Y(), aParams.Z());
    }
    else if (isAxis)
    {
      gp_Ax2 anAxis(anOrigin, aDir, aXDir);
      aPreview.Init(anAxis, aParams.X(), aParams.Y(), aParams.Z());
    }
    else
    {
      aPreview.Init(aParams.X(), aParams.Y(), aParams.Z());
    }

    S = aPreview;
    DBRep::Set(a[1], S);
  }
  else
  {
    TopoDS_Solid S;
    if (isMinMax)
    {
      S = BRepPrimAPI_MakeBox(anOrigin, aParams);
    }
    else if (!isMinMax && !isAxis)
    {
      S = BRepPrimAPI_MakeBox(anOrigin, aParams.X(), aParams.Y(), aParams.Z());
    }
    else if (isAxis)
    {
      gp_Ax2 anAxis(anOrigin, aDir, aXDir);
      S = BRepPrimAPI_MakeBox(anAxis, aParams.X(), aParams.Y(), aParams.Z());
    }
    else
    {
      S = BRepPrimAPI_MakeBox(aParams.X(), aParams.Y(), aParams.Z());
    }
    DBRep::Set(a[1], S);
  }
  return 0;
}

//=======================================================================
// wedge
//=======================================================================

static int wedge(Draw_Interpretor&, int n, const char** a)
{
  TopoDS_Solid S;

  //  int i = 0;
  if (n == 15 || n == 18)
  {
    gp_Pnt LocalP(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4]));
    gp_Dir LocalN(Draw::Atof(a[5]), Draw::Atof(a[6]), Draw::Atof(a[7]));
    gp_Dir LocalVx(Draw::Atof(a[8]), Draw::Atof(a[9]), Draw::Atof(a[10]));
    gp_Ax2 Axis(LocalP, LocalN, LocalVx);
    //    gp_Ax2 Axis(gp_Pnt(Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4])),
    //		gp_Dir(Draw::Atof(a[5]),Draw::Atof(a[6]),Draw::Atof(a[7])),
    //		gp_Dir(Draw::Atof(a[8]),Draw::Atof(a[9]),Draw::Atof(a[10])));
    if (n == 15)
    {
      S = BRepPrimAPI_MakeWedge(Axis,
                                Draw::Atof(a[11]),
                                Draw::Atof(a[12]),
                                Draw::Atof(a[13]),
                                Draw::Atof(a[14]));
    }
    else
    {
      S = BRepPrimAPI_MakeWedge(Axis,
                                Draw::Atof(a[11]),
                                Draw::Atof(a[12]),
                                Draw::Atof(a[13]),
                                Draw::Atof(a[14]),
                                Draw::Atof(a[15]),
                                Draw::Atof(a[16]),
                                Draw::Atof(a[17]));
    }
  }
  else if (n == 6)
  {
    S =
      BRepPrimAPI_MakeWedge(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4]), Draw::Atof(a[5]));
  }
  else if (n == 9)
  {
    S = BRepPrimAPI_MakeWedge(Draw::Atof(a[2]),
                              Draw::Atof(a[3]),
                              Draw::Atof(a[4]),
                              Draw::Atof(a[5]),
                              Draw::Atof(a[6]),
                              Draw::Atof(a[7]),
                              Draw::Atof(a[8]));
  }
  else
    return 1;

  DBRep::Set(a[1], S);
  return 0;
}

//=======================================================================
// cylinder
//=======================================================================

static int cylinder(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;
  TopoDS_Solid            S;
  occ::handle<Geom_Plane> P = occ::down_cast<Geom_Plane>(DrawTrSurf::Get(a[2]));

  if (n == 4)
  {
    S = BRepPrimAPI_MakeCylinder(Draw::Atof(a[2]), Draw::Atof(a[3]));
  }
  else if (n == 5)
  {
    if (P.IsNull())
      S = BRepPrimAPI_MakeCylinder(Draw::Atof(a[2]),
                                   Draw::Atof(a[3]),
                                   Draw::Atof(a[4]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeCylinder(P->Pln().Position().Ax2(), Draw::Atof(a[3]), Draw::Atof(a[4]));
  }
  else if (n == 6)
  {
    if (P.IsNull())
      return 1;
    else
      S = BRepPrimAPI_MakeCylinder(P->Pln().Position().Ax2(),
                                   Draw::Atof(a[3]),
                                   Draw::Atof(a[4]),
                                   Draw::Atof(a[5]) * (M_PI / 180.0));
  }
  else
    return 1;

  DBRep::Set(a[1], S);
  return 0;
}

//=======================================================================
// cone
//=======================================================================

static int cone(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;
  TopoDS_Solid S;

  occ::handle<Geom_Plane> P = occ::down_cast<Geom_Plane>(DrawTrSurf::Get(a[2]));

  if (n == 5)
  {
    S = BRepPrimAPI_MakeCone(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4]));
  }
  else if (n == 6)
  {
    if (P.IsNull())
      S = BRepPrimAPI_MakeCone(Draw::Atof(a[2]),
                               Draw::Atof(a[3]),
                               Draw::Atof(a[4]),
                               Draw::Atof(a[5]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeCone(P->Pln().Position().Ax2(),
                               Draw::Atof(a[3]),
                               Draw::Atof(a[4]),
                               Draw::Atof(a[5]));
  }
  else if (n == 7)
  {
    S = BRepPrimAPI_MakeCone(P->Pln().Position().Ax2(),
                             Draw::Atof(a[3]),
                             Draw::Atof(a[4]),
                             Draw::Atof(a[5]),
                             Draw::Atof(a[6]) * (M_PI / 180.0));
  }
  else
    return 1;

  DBRep::Set(a[1], S);
  return 0;
}

//=======================================================================
// sphere
//=======================================================================

static int sphere(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;
  TopoDS_Solid S;

  occ::handle<Geom_Plane> P = occ::down_cast<Geom_Plane>(DrawTrSurf::Get(a[2]));

  if (n == 3)
  {
    S = BRepPrimAPI_MakeSphere(Draw::Atof(a[2]));
  }
  else if (n == 4)
  {
    if (P.IsNull())
      S = BRepPrimAPI_MakeSphere(Draw::Atof(a[2]), Draw::Atof(a[3]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeSphere(P->Pln().Position().Ax2(), Draw::Atof(a[3]));
  }
  else if (n == 5)
  {
    if (P.IsNull())
      S = BRepPrimAPI_MakeSphere(Draw::Atof(a[2]),
                                 Draw::Atof(a[3]) * (M_PI / 180.0),
                                 Draw::Atof(a[4]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeSphere(P->Pln().Position().Ax2(),
                                 Draw::Atof(a[3]),
                                 Draw::Atof(a[4]) * (M_PI / 180.0));
  }
  else if (n == 6)
  {
    if (P.IsNull())
      S = BRepPrimAPI_MakeSphere(Draw::Atof(a[2]),
                                 Draw::Atof(a[3]) * (M_PI / 180.0),
                                 Draw::Atof(a[4]) * (M_PI / 180.0),
                                 Draw::Atof(a[5]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeSphere(P->Pln().Position().Ax2(),
                                 Draw::Atof(a[3]),
                                 Draw::Atof(a[4]) * (M_PI / 180.0),
                                 Draw::Atof(a[5]) * (M_PI / 180.0));
  }
  else if (n == 7)
  {
    S = BRepPrimAPI_MakeSphere(P->Pln().Position().Ax2(),
                               Draw::Atof(a[3]),
                               Draw::Atof(a[4]) * (M_PI / 180.0),
                               Draw::Atof(a[5]) * (M_PI / 180.0),
                               Draw::Atof(a[6]) * (M_PI / 180.0));
  }
  else
    return 1;

  DBRep::Set(a[1], S);
  return 0;
}

//=======================================================================
// torus
//=======================================================================

static int torus(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;
  TopoDS_Solid S;

  occ::handle<Geom_Plane> P = occ::down_cast<Geom_Plane>(DrawTrSurf::Get(a[2]));

  if (n == 4)
  {
    S = BRepPrimAPI_MakeTorus(Draw::Atof(a[2]), Draw::Atof(a[3]));
  }
  else if (n == 5)
  {
    if (P.IsNull())
      S = BRepPrimAPI_MakeTorus(Draw::Atof(a[2]),
                                Draw::Atof(a[3]),
                                Draw::Atof(a[4]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeTorus(P->Pln().Position().Ax2(), Draw::Atof(a[3]), Draw::Atof(a[4]));
  }
  else if (n == 6)
  {
    if (P.IsNull())
      S = BRepPrimAPI_MakeTorus(Draw::Atof(a[2]),
                                Draw::Atof(a[3]),
                                Draw::Atof(a[4]) * (M_PI / 180.0),
                                Draw::Atof(a[5]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeTorus(P->Pln().Position().Ax2(),
                                Draw::Atof(a[3]),
                                Draw::Atof(a[4]),
                                Draw::Atof(a[5]) * (M_PI / 180.0));
  }
  else if (n == 7)
  {
    if (P.IsNull())
      S = BRepPrimAPI_MakeTorus(Draw::Atof(a[2]),
                                Draw::Atof(a[3]),
                                Draw::Atof(a[4]) * (M_PI / 180.0),
                                Draw::Atof(a[5]) * (M_PI / 180.0),
                                Draw::Atof(a[6]) * (M_PI / 180.0));
    else
      S = BRepPrimAPI_MakeTorus(P->Pln().Position().Ax2(),
                                Draw::Atof(a[3]),
                                Draw::Atof(a[4]),
                                Draw::Atof(a[5]) * (M_PI / 180.0),
                                Draw::Atof(a[6]) * (M_PI / 180.0));
  }
  else if (n == 8)
  {
    S = BRepPrimAPI_MakeTorus(P->Pln().Position().Ax2(),
                              Draw::Atof(a[3]),
                              Draw::Atof(a[4]),
                              Draw::Atof(a[5]) * (M_PI / 180.0),
                              Draw::Atof(a[6]) * (M_PI / 180.0),
                              Draw::Atof(a[7]) * (M_PI / 180.0));
  }
  else
    return 1;

  DBRep::Set(a[1], S);
  return 0;
}

//=================================================================================================

void BRepTest::PrimitiveCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;
  done = true;

  DBRep::BasicCommands(theCommands);

  const char* g = "Primitive building commands";

  theCommands.Add(
    "box",
    "box name [dx dy dz] [x y z dx dy dz]"
    "\n\t\t:            [-min x y z] [-size dx dy dz] [-max x y z]"
    "\n\t\t:            [-dir x y z -xdir x y z] [-preview]"
    "\n\t\t: Construct axes-aligned box and put result into 'name' variable"
    "\n\t\t:  -min   box lower corner, origin; (0,0,0) by default"
    "\n\t\t:  -size  box dimensions   (alternative to -max)"
    "\n\t\t:  -max   box upper corner (alternative to -size)"
    "\n\t\t:  -dir   main direction of coordinate system (DZ by default)"
    "\n\t\t:  -xdir  x direction of coordinate system (DX by default)"
    "\n\t\t:  -preview non-solid shape will be created (vertex, edge, rectangle or box);"
    "\n\t\t:           otherwise, return NULL shape in case of zero box dimension.",
    __FILE__,
    box,
    g);

  theCommands.Add("wedge",
                  "wedge name [Ox Oy Oz Zx Zy Zz Xx Xy Xz] dx dy dz ltx / xmin zmin xmax zmax",
                  __FILE__,
                  wedge,
                  g);

  theCommands.Add("pcylinder",
                  "pcylinder name [plane(ax2)] R H [angle]"
                  "\n\t\t: Construct a cylinder and put result into 'name' variable."
                  "\n\t\t: Parameters of the cylinder :"
                  "\n\t\t: - plane coordinate system for the construction of the cylinder"
                  "\n\t\t: - R     cylinder radius"
                  "\n\t\t: - H     cylinder height"
                  "\n\t\t: - angle cylinder top radius",
                  __FILE__,
                  cylinder,
                  g);

  theCommands.Add(
    "pcone",
    "pcone name [plane(ax2)] R1 R2 H [angle]"
    "\n\t\t: Construct a cone, part cone or conical frustum and put result into 'name' variable."
    "\n\t\t: Parameters of the cone :"
    "\n\t\t: - plane  coordinate system for the construction of the cone"
    "\n\t\t: - R1     cone bottom radius"
    "\n\t\t: - R2     cone top radius"
    "\n\t\t: - H      cone height"
    "\n\t\t: - angle  angle to create a part cone",
    __FILE__,
    cone,
    g);

  theCommands.Add("psphere",
                  "psphere name [plane(ax2)] R [angle1 angle2] [angle]"
                  "\n\t\t: Construct a sphere, spherical segment or spherical wedge and put result "
                  "into 'name' variable."
                  "\n\t\t: Parameters of the sphere :"
                  "\n\t\t: - plane  coordinate system for the construction of the sphere"
                  "\n\t\t: - R      sphere radius"
                  "\n\t\t: - angle1 first angle to create a spherical segment  [-90; 90]"
                  "\n\t\t: - angle2 second angle to create a spherical segment [-90; 90]"
                  "\n\t\t: - angle  angle to create a spherical wedge",
                  __FILE__,
                  sphere,
                  g);

  theCommands.Add("ptorus",
                  "ptorus name [plane(ax2)] R1 R2 [angle1 angle2] [angle]"
                  "\n\t\t: Construct a torus or torus segment and put result into 'name' variable."
                  "\n\t\t: Parameters of the torus :"
                  "\n\t\t: - plane  coordinate system for the construction of the torus"
                  "\n\t\t: - R1     distance from the center of the pipe to the center of the torus"
                  "\n\t\t: - R2     radius of the pipe"
                  "\n\t\t: - angle1 first angle to create a torus ring segment"
                  "\n\t\t: - angle2 second angle to create a torus ring segment"
                  "\n\t\t: - angle  angle to create a torus pipe segment",
                  __FILE__,
                  torus,
                  g);
}
