// Created on: 1996-10-09
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#include <Draw.hxx>
#include <DrawFairCurve_MinimalVariation.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Point.hxx>
#include <FairCurve_MinimalVariation.hxx>
#include <GeometryTest.hxx>
#include <gp_Pnt2d.hxx>

//=======================================================================
bool IsGoodNumber(int argc, int waiting, Draw_Interpretor& di)
//=======================================================================
{
  // argc vaut 1 de plus, puisque argv[0] contient le nom de la commande
  if (argc != (waiting + 1))
  {
    di << "Waiting " << waiting << " arguments\n";
    return false;
  }
  else
    return true;
}

//=======================================================================
static int BattenCurve(Draw_Interpretor& di, int argc, const char** argv)
//=======================================================================
{
  if (!IsGoodNumber(argc, 6, di))
    return 1;

  const char* cp1        = argv[1];
  const char* cp2        = argv[2];
  const char* cangle1    = argv[3];
  const char* cangle2    = argv[4];
  const char* cheigth    = argv[5];
  const char* BattenName = argv[6];

  FairCurve_AnalysisCode Iana;
  double   a1 = Draw::Atof(cangle1), a2 = Draw::Atof(cangle2), h = Draw::Atof(cheigth);
  gp_Pnt2d P1, P2;

  if (!DrawTrSurf::GetPoint2d(cp1, P1))
    return 1;
  if (!DrawTrSurf::GetPoint2d(cp2, P2))
    return 1;

  FairCurve_Batten* Bat = new FairCurve_Batten(P1, P2, h);
  Bat->SetAngle1(a1 * M_PI / 180);
  Bat->SetAngle2(a2 * M_PI / 180);

  Bat->Compute(Iana);

  occ::handle<DrawFairCurve_Batten> aBatten = new DrawFairCurve_Batten(Bat);

  if (aBatten.IsNull())
  {
    di << " Batten null \n";
    return 1;
  }
  Draw::Set(BattenName, aBatten);

  return 0;
  // !!! Delete of Bat have to be make in DrawFairCurve_Batten destructor !!!!!
}

//=======================================================================
static int MVCurve(Draw_Interpretor& di, int argc, const char** argv)
//=======================================================================
{
  if (!IsGoodNumber(argc, 6, di))
    return 1;

  const char* cp1     = argv[1];
  const char* cp2     = argv[2];
  const char* cangle1 = argv[3];
  const char* cangle2 = argv[4];
  const char* cheigth = argv[5];
  const char* MVCName = argv[6];

  FairCurve_AnalysisCode Iana;
  double   a1 = Draw::Atof(cangle1), a2 = Draw::Atof(cangle2), h = Draw::Atof(cheigth);
  gp_Pnt2d P1, P2;

  if (!DrawTrSurf::GetPoint2d(cp1, P1))
    return 1;
  if (!DrawTrSurf::GetPoint2d(cp2, P2))
    return 1;

  FairCurve_MinimalVariation* MVC = new FairCurve_MinimalVariation(P1, P2, h);
  MVC->SetAngle1(a1 * M_PI / 180);
  MVC->SetAngle2(a2 * M_PI / 180);

  MVC->Compute(Iana);

  occ::handle<DrawFairCurve_MinimalVariation> aMVC = new DrawFairCurve_MinimalVariation(MVC);

  if (aMVC.IsNull())
  {
    di << " MVC null \n";
    return 1;
  }
  Draw::Set(MVCName, aMVC);

  return 0;
  // !!! Delete of Bat have to be make in DrawFairCurve_MinimalVariation destructor !!!!!
}

//=======================================================================
static int SetPoint(Draw_Interpretor& di, int argc, const char** argv)
//=======================================================================
{
  if (!IsGoodNumber(argc, 3, di))
    return 1;

  const char* side       = argv[1];
  const char* PointName  = argv[2];
  const char* BattenName = argv[3];

  int cote = Draw::Atoi(side);

  occ::handle<DrawTrSurf_Point> Pnt = occ::down_cast<DrawTrSurf_Point>(Draw::Get(PointName));
  if (Pnt.IsNull())
    return 1;

  occ::handle<DrawFairCurve_Batten> Bat =
    occ::down_cast<DrawFairCurve_Batten>(Draw::Get(BattenName));
  if (Bat.IsNull())
    return 1;

  Bat->SetPoint(cote, Pnt->Point2d());
  Draw::Repaint();
  return 0;
}

//=======================================================================
static int SetAngle(Draw_Interpretor& di, int argc, const char** argv)
//=======================================================================
{
  if (!IsGoodNumber(argc, 3, di))
    return 1;

  const char* side       = argv[1];
  const char* val        = argv[2];
  const char* BattenName = argv[3];

  double angle = Draw::Atof(val);
  int    cote  = Draw::Atoi(side);

  occ::handle<DrawFairCurve_Batten> Bat =
    occ::down_cast<DrawFairCurve_Batten>(Draw::Get(BattenName));
  if (Bat.IsNull())
    return 1;

  Bat->SetAngle(cote, angle);
  Draw::Repaint();
  return 0;
}

//=======================================================================
static int SetCurvature(Draw_Interpretor& di, int argc, const char** argv)
//=======================================================================
{
  if (!IsGoodNumber(argc, 3, di))
    return 1;

  const char* side    = argv[1];
  const char* val     = argv[2];
  const char* MVCName = argv[3];

  double rho  = Draw::Atof(val);
  int    cote = Draw::Atoi(side);

  occ::handle<DrawFairCurve_MinimalVariation> MVC =
    occ::down_cast<DrawFairCurve_MinimalVariation>(Draw::Get(MVCName));
  if (MVC.IsNull())
    return 1;

  MVC->SetCurvature(cote, rho);
  Draw::Repaint();
  return 0;
}

//=======================================================================
static int SetSlide(Draw_Interpretor& di, int argc, const char** argv)
//=======================================================================
{
  if (!IsGoodNumber(argc, 2, di))
    return 1;

  const char* val        = argv[1];
  const char* BattenName = argv[2];

  double slide = Draw::Atof(val);

  occ::handle<DrawFairCurve_Batten> Bat =
    occ::down_cast<DrawFairCurve_Batten>(Draw::Get(BattenName));
  if (Bat.IsNull())
    return 1;

  Bat->SetSliding(slide);
  Draw::Repaint();
  return 0;
}

//=======================================================================
static int FreeAngle(Draw_Interpretor& di, int argc, const char** argv)
//=======================================================================
{
  if (!IsGoodNumber(argc, 2, di))
    return 1;

  const char* side       = argv[1];
  const char* BattenName = argv[2];

  int cote = Draw::Atoi(side);

  occ::handle<DrawFairCurve_Batten> Bat =
    occ::down_cast<DrawFairCurve_Batten>(Draw::Get(BattenName));
  if (Bat.IsNull())
    return 1;

  Bat->FreeAngle(cote);
  Draw::Repaint();
  return 0;
}

//=======================================================================
static int FreeCurvature(Draw_Interpretor& di, int argc, const char** argv)
//=======================================================================
{
  if (!IsGoodNumber(argc, 2, di))
    return 1;

  const char* side    = argv[1];
  const char* MVCName = argv[2];

  int cote = Draw::Atoi(side);

  occ::handle<DrawFairCurve_MinimalVariation> MVC =
    occ::down_cast<DrawFairCurve_MinimalVariation>(Draw::Get(MVCName));
  if (MVC.IsNull())
    return 1;

  MVC->FreeCurvature(cote);
  Draw::Repaint();
  return 0;
}

//=======================================================================
static int FreeSlide(Draw_Interpretor& di, int argc, const char** argv)
//=======================================================================
{
  if (!IsGoodNumber(argc, 1, di))
    return 1;

  const char* BattenName = argv[1];

  occ::handle<DrawFairCurve_Batten> Bat =
    occ::down_cast<DrawFairCurve_Batten>(Draw::Get(BattenName));
  if (Bat.IsNull())
    return 1;

  Bat->FreeSliding();
  Draw::Repaint();
  return 0;
}

//=======================================================================
static int SetHeight(Draw_Interpretor& di, int argc, const char** argv)
//=======================================================================
{
  if (!IsGoodNumber(argc, 2, di))
    return 1;

  const char* val        = argv[1];
  const char* BattenName = argv[2];

  double Height = Draw::Atof(val);

  occ::handle<DrawFairCurve_Batten> Bat =
    occ::down_cast<DrawFairCurve_Batten>(Draw::Get(BattenName));
  if (Bat.IsNull())
    return 1;

  Bat->SetHeight(Height);
  Draw::Repaint();
  return 0;
}

//=======================================================================
static int SetSlope(Draw_Interpretor& di, int argc, const char** argv)
//=======================================================================
{
  if (!IsGoodNumber(argc, 2, di))
    return 1;

  const char* val        = argv[1];
  const char* BattenName = argv[2];

  double Slope = Draw::Atof(val);

  occ::handle<DrawFairCurve_Batten> Bat =
    occ::down_cast<DrawFairCurve_Batten>(Draw::Get(BattenName));
  if (Bat.IsNull())
    return 1;

  Bat->SetSlope(Slope);
  Draw::Repaint();
  return 0;
}
//=======================================================================
static int SetPhysicalRatio(Draw_Interpretor& di, int argc, const char** argv)
//=======================================================================
{
  if (!IsGoodNumber(argc, 2, di))
    return 1;

  const char* val     = argv[1];
  const char* MVCName = argv[2];

  double ratio = Draw::Atof(val);

  occ::handle<DrawFairCurve_MinimalVariation> MVC =
    occ::down_cast<DrawFairCurve_MinimalVariation>(Draw::Get(MVCName));
  if (MVC.IsNull())
    return 1;

  MVC->SetPhysicalRatio(ratio);
  Draw::Repaint();
  return 0;
}

//=======================================================================
void GeometryTest::FairCurveCommands(Draw_Interpretor& TheCommande)
//=======================================================================

{
  const char* g;

  g = "FairCurve command";

  TheCommande.Add("battencurve",
                  "battencurve P1 P2 Angle1 Angle2 Heigth BattenName",
                  __FILE__,
                  BattenCurve,
                  g);
  TheCommande.Add("minvarcurve",
                  "MVCurve P1 P2 Angle1 Angle2 Heigth MVCName",
                  __FILE__,
                  MVCurve,
                  g);
  TheCommande.Add("setpoint", "setpoint side point BattenName ", __FILE__, SetPoint, g);
  TheCommande.Add("setangle", "setangle side angle BattenName ", __FILE__, SetAngle, g);
  TheCommande.Add("setslide", "setangle slidingfactor BattenName ", __FILE__, SetSlide, g);
  TheCommande.Add("freeangle", "freeangle side BattenName", __FILE__, FreeAngle, g);
  TheCommande.Add("freeslide", "freeslide BattenName", __FILE__, FreeSlide, g);
  TheCommande.Add("setheight", "setheight height BattenName ", __FILE__, SetHeight, g);
  TheCommande.Add("setslope", "setslope slope BattenName ", __FILE__, SetSlope, g);
  TheCommande.Add("setcurvature", "setcurvature side rho MVCName ", __FILE__, SetCurvature, g);
  TheCommande.Add("freecurvature", "freecurvature side  MVCName ", __FILE__, FreeCurvature, g);
  TheCommande.Add("setphysicalratio",
                  "physicalratio ratio MVCName ",
                  __FILE__,
                  SetPhysicalRatio,
                  g);
}
