// Created on: 1994-10-04
// Created by: Yves FRICAUD
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

#include <BRepTest.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf_Curve2d.hxx>

#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>

#include <MAT_Bisector.hxx>
#include <MAT_Zone.hxx>
#include <MAT_Graph.hxx>
#include <MAT_BasicElt.hxx>
#include <MAT_Side.hxx>

#include <Bisector_BisecAna.hxx>
#include <Precision.hxx>

#include <BRepOffsetAPI_MakeOffset.hxx>
#include <BRepMAT2d_Explorer.hxx>
#include <BRepMAT2d_BisectingLocus.hxx>
#include <BRepMAT2d_LinkTopoBilo.hxx>

#include <gp_Parab2d.hxx>
#include <gp_Hypr2d.hxx>

#include <DBRep.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

static BRepMAT2d_BisectingLocus MapBiLo;
static BRepMAT2d_Explorer       anExplo;
static BRepMAT2d_LinkTopoBilo   TopoBilo;
static MAT_Side                 SideOfMat = MAT_Left;
static bool                     LinkComputed;

static void DrawCurve(const occ::handle<Geom2d_Curve>& aCurve, const int Indice);

//==========================================================================
// function : topoLoad
//           loading of a face in the explorer.
//==========================================================================
static int topoload(Draw_Interpretor&, int argc, const char** argv)
{
  if (argc < 2)
    return 1;

  TopoDS_Shape C1 = DBRep::Get(argv[1], TopAbs_FACE);

  if (C1.IsNull())
    return 1;

  TopoDS_Face aFace = TopoDS::Face(C1);

  if (argc >= 3 && (strcmp(argv[2], "-approx") == 0))
  {
    double aTol = 0.1;
    aFace       = BRepOffsetAPI_MakeOffset::ConvertFace(aFace, aTol);
  }

  anExplo.Perform(aFace);
  return 0;
}

//==========================================================================
// function : drawcont
//           visualization of the contour defined by the explorer.
//==========================================================================
static int drawcont(Draw_Interpretor&, int, const char**)
{
  occ::handle<Geom2d_TrimmedCurve> C;

  for (int j = 1; j <= anExplo.NumberOfContours(); j++)
  {
    for (anExplo.Init(j); anExplo.More(); anExplo.Next())
    {
      DrawCurve(anExplo.Value(), 1);
    }
  }
  return 0;
}

//==========================================================================
// function : mat
//           calculate the map of locations bisector on the contour defined by
//           the explorer.
//==========================================================================
static int mat(Draw_Interpretor&, int n, const char** a)
{
  GeomAbs_JoinType theJoinType = GeomAbs_Arc;
  if (n >= 2 && strcmp(a[1], "i") == 0)
    theJoinType = GeomAbs_Intersection;

  bool IsOpenResult = false;
  if (n == 3 && strcmp(a[2], "o") == 0)
    IsOpenResult = true;

  MapBiLo.Compute(anExplo, 1, SideOfMat, theJoinType, IsOpenResult);
  LinkComputed = false;

  return 0;
}

//============================================================================
// function : zone
//           construction and display of the proximity zone associated to the
//           base elements defined by the edge or the vertex.
//============================================================================
static int zone(Draw_Interpretor&, int argc, const char** argv)
{
  if (argc < 2)
    return 1;

  TopoDS_Shape S = DBRep::Get(argv[1], TopAbs_EDGE);
  if (S.IsNull())
  {
    S = DBRep::Get(argv[1], TopAbs_VERTEX);
  }

  if (!LinkComputed)
  {
    TopoBilo.Perform(anExplo, MapBiLo);
    LinkComputed = true;
  }

  bool                  Reverse;
  occ::handle<MAT_Zone> TheZone = new MAT_Zone();

  for (TopoBilo.Init(S); TopoBilo.More(); TopoBilo.Next())
  {
    const occ::handle<MAT_BasicElt>& BE = TopoBilo.Value();
    TheZone->Perform(BE);
    for (int i = 1; i <= TheZone->NumberOfArcs(); i++)
    {
      DrawCurve(MapBiLo.GeomBis(TheZone->ArcOnFrontier(i), Reverse).Value(), 2);
    }
  }
  return 0;
}

//==========================================================================
// function : side
//           side = left  => calculation to the left of the contour.
//           side = right => calculation to the right of the contour.
//==========================================================================

static int side(Draw_Interpretor&, int, const char** argv)
{
  if (!strcmp(argv[1], "right"))
    SideOfMat = MAT_Right;
  else
    SideOfMat = MAT_Left;

  return 0;
}

//==========================================================================
// function : result
//           Complete display of the calculated map.
//==========================================================================
static int result(Draw_Interpretor&, int, const char**)
{
  int  i, NbArcs = 0;
  bool Rev;

  NbArcs = MapBiLo.Graph()->NumberOfArcs();

  for (i = 1; i <= NbArcs; i++)
  {
    DrawCurve(MapBiLo.GeomBis(MapBiLo.Graph()->Arc(i), Rev).Value(), 3);
  }
  return 0;
}

//==========================================================================
// function : DrawCurve
//           Display of curve <aCurve> of Geom2d in a color defined by <Indice>.
//  Indice = 1 yellow,
//  Indice = 2 blue,
//  Indice = 3 red,
//  Indice = 4 green.
//==========================================================================
void DrawCurve(const occ::handle<Geom2d_Curve>& aCurve, const int Indice)
{
  occ::handle<Standard_Type>      type = aCurve->DynamicType();
  occ::handle<Geom2d_Curve>       curve, CurveDraw;
  occ::handle<DrawTrSurf_Curve2d> dr;
  Draw_Color                      Couleur;

  if (type == STANDARD_TYPE(Geom2d_TrimmedCurve))
  {
    curve = occ::down_cast<Geom2d_TrimmedCurve>(aCurve)->BasisCurve();
    type  = curve->DynamicType();
    if (type == STANDARD_TYPE(Bisector_BisecAna))
    {
      curve = occ::down_cast<Bisector_BisecAna>(curve)->Geom2dCurve();
      type  = curve->DynamicType();
    }
    // PB of representation of semi_infinite curves.
    gp_Parab2d gpParabola;
    gp_Hypr2d  gpHyperbola;
    double     Focus;
    double     Limit = 50000.;
    double     delta = 400;

    // PB of representation of semi_infinite curves.
    if (aCurve->LastParameter() == Precision::Infinite())
    {

      if (type == STANDARD_TYPE(Geom2d_Parabola))
      {
        gpParabola  = occ::down_cast<Geom2d_Parabola>(curve)->Parab2d();
        Focus       = gpParabola.Focal();
        double Val1 = std::sqrt(Limit * Focus);
        double Val2 = std::sqrt(Limit * Limit);
        delta       = (Val1 <= Val2 ? Val1 : Val2);
      }
      else if (type == STANDARD_TYPE(Geom2d_Hyperbola))
      {
        gpHyperbola  = occ::down_cast<Geom2d_Hyperbola>(curve)->Hypr2d();
        double Majr  = gpHyperbola.MajorRadius();
        double Minr  = gpHyperbola.MinorRadius();
        double Valu1 = Limit / Majr;
        double Valu2 = Limit / Minr;
        double Val1  = std::log(Valu1 + std::sqrt(Valu1 * Valu1 - 1));
        double Val2  = std::log(Valu2 + std::sqrt(Valu2 * Valu2 + 1));
        delta        = (Val1 <= Val2 ? Val1 : Val2);
      }
      if (aCurve->FirstParameter() == -Precision::Infinite())
        CurveDraw = new Geom2d_TrimmedCurve(aCurve, -delta, delta);
      else
        CurveDraw = new Geom2d_TrimmedCurve(aCurve,
                                            aCurve->FirstParameter(),
                                            aCurve->FirstParameter() + delta);
    }
    else
    {
      CurveDraw = aCurve;
    }
    // end PB.
  }
  else
  {
    CurveDraw = aCurve;
  }

  if (Indice == 1)
    Couleur = Draw_jaune;
  else if (Indice == 2)
    Couleur = Draw_bleu;
  else if (Indice == 3)
    Couleur = Draw_rouge;
  else if (Indice == 4)
    Couleur = Draw_vert;

  int Discret = 50;

  if (type == STANDARD_TYPE(Geom2d_Circle))
    dr = new DrawTrSurf_Curve2d(CurveDraw, Couleur, 30, false);
  else if (type == STANDARD_TYPE(Geom2d_Line))
    dr = new DrawTrSurf_Curve2d(CurveDraw, Couleur, 2, false);
  else
    dr = new DrawTrSurf_Curve2d(CurveDraw, Couleur, Discret, false);

  dout << dr;
  dout.Flush();
}

//==========================================================================
// function BRepTest:: MatCommands
//==========================================================================

void BRepTest::MatCommands(Draw_Interpretor& theCommands)
{
  theCommands.Add("topoload", "load face: topoload face [-approx]", __FILE__, topoload);
  theCommands.Add("drawcont", "display current contour", __FILE__, drawcont);
  theCommands.Add("mat", "computes the mat: mat [a/i [o]]", __FILE__, mat);
  theCommands.Add("side", "side left/right", __FILE__, side);
  theCommands.Add("result", "result", __FILE__, result);
  theCommands.Add("zone", "zone edge or vertex", __FILE__, zone);
}
