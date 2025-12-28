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
#include <GeometryTest.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <BRepLib.hxx>
#include <BRepAlgo.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

#include <DBRep.hxx>
#include <DBRep_DrawableShape.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Point.hxx>

#include <gp.hxx>
#include <Precision.hxx>
#include <GeomAPI.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopOpeBRep_EdgesIntersector.hxx>
#include <TopOpeBRep_Point2d.hxx>
#include <TopOpeBRepDS_Transition.hxx>

#include <stdio.h>
#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

//=======================================================================
// vertex
//=======================================================================

static int vertex(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;
  if (n >= 5)
  {
    DBRep::Set(
      a[1],
      BRepBuilderAPI_MakeVertex(gp_Pnt(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4]))));
  }
  else if (n == 4)
  {
    TopoDS_Shape S = DBRep::Get(a[3]);
    if (S.IsNull())
      return 0;
    if (S.ShapeType() != TopAbs_EDGE)
      return 0;
    BRepAdaptor_Curve C(TopoDS::Edge(S));
    gp_Pnt            P;
    C.D0(Draw::Atof(a[2]), P);
    DBRep::Set(a[1], BRepBuilderAPI_MakeVertex(P));
  }
  else
  {
    occ::handle<DrawTrSurf_Point> aP = occ::down_cast<DrawTrSurf_Point>(Draw::Get(a[2]));
    DBRep::Set(a[1], BRepBuilderAPI_MakeVertex(aP->Point()));
  }
  return 0;
}

//=======================================================================
// range
//=======================================================================

static int range(Draw_Interpretor&, int n, const char** a)
{
  if (n < 4)
    return 1;
  TopoDS_Shape aLocalShape(DBRep::Get(a[1], TopAbs_EDGE));
  TopoDS_Edge  E = TopoDS::Edge(aLocalShape);
  //  TopoDS_Edge E = TopoDS::Edge(DBRep::Get(a[1],TopAbs_EDGE));
  if (E.IsNull())
    return 1;
  double       f = Draw::Atof(a[n - 2]);
  double       l = Draw::Atof(a[n - 1]);
  BRep_Builder B;
  if (n == 4)
    B.Range(E, f, l);
  else
  {
    aLocalShape   = DBRep::Get(a[2], TopAbs_FACE);
    TopoDS_Face F = TopoDS::Face(aLocalShape);
    //    TopoDS_Face F = TopoDS::Face(DBRep::Get(a[2],TopAbs_FACE));
    if (F.IsNull())
      return 1;
    B.Range(E, F, f, l);
  }
  return 0;
}

//=======================================================================
// trim
//=======================================================================

static int trim(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;
  TopoDS_Shape e = DBRep::Get(a[1]);
  if (e.IsNull())
    return 1;
  if (e.ShapeType() != TopAbs_EDGE)
    return 1;
  TopoDS_Shape v1 = DBRep::Get(a[2]);
  if (v1.IsNull())
    return 1;
  if (v1.ShapeType() != TopAbs_VERTEX)
    return 1;
  TopoDS_Shape v2;
  if (n > 3)
  {
    v2 = DBRep::Get(a[3]);
    if (v2.IsNull())
      return 1;
    if (v2.ShapeType() != TopAbs_VERTEX)
      return 1;
  }
  TopLoc_Location               L;
  double                        f, l;
  const occ::handle<Geom_Curve> C  = BRep_Tool::Curve(TopoDS::Edge(e), L, f, l);
  TopLoc_Location               LI = L.Inverted();
  e.Orientation(TopAbs_FORWARD);
  e.Move(LI);
  v1.Move(LI);
  v2.Move(LI);
  TopoDS_Edge ne;
  if (v2.IsNull())
  {
    if (v1.Orientation() == TopAbs_REVERSED)
    {
      v2 = v1;
      v1 = TopoDS_Shape();
    }
  }
  BRepBuilderAPI_MakeEdge ME(C, TopoDS::Vertex(v1), TopoDS::Vertex(v2));
  if (ME.IsDone())
  {
    ne = ME;
    ne.Move(L);
    DBRep::Set(a[1], ne);
  }
  else
  {
    // std::cout <<"Error creating edge"<<std::endl;
    di << "Error creating edge\n";
  }
  return 0;
}

//=======================================================================
// polyline
//=======================================================================

static int polyline(Draw_Interpretor&, int n, const char** a)
{
  if (n < 8)
    return 1;
  if (((n - 2) % 3) != 0)
    return 1;
  int                        i, j, np = (n - 2) / 3;
  BRepBuilderAPI_MakePolygon W;
  j = 2;
  for (i = 1; i <= np; i++)
  {
    W.Add(gp_Pnt(Draw::Atof(a[j]), Draw::Atof(a[j + 1]), Draw::Atof(a[j + 2])));
    j += 3;
  }
  DBRep::Set(a[1], W.Wire());
  return 0;
}

//=======================================================================
// polyvertex
//=======================================================================

static int polyvertex(Draw_Interpretor&, int n, const char** a)
{
  if (n < 4)
    return 1;
  int                        i;
  BRepBuilderAPI_MakePolygon W;
  for (i = 2; i < n; i++)
  {
    TopoDS_Shape S = DBRep::Get(a[i]);
    if (S.IsNull())
      return 1;
    if (S.ShapeType() != TopAbs_VERTEX)
      return 1;
    W.Add(TopoDS::Vertex(S));
  }
  DBRep::Set(a[1], W.Wire());
  return 0;
}

//=======================================================================
// wire
//=======================================================================

static int wire(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;
  int                     i;
  BRepBuilderAPI_MakeWire MW;
  bool                    IsUnsorted = !strcmp(a[2], "-unsorted");

  if (!IsUnsorted)
    for (i = 2; i < n; i++)
    {
      TopoDS_Shape S = DBRep::Get(a[i]);
      if (S.IsNull())
        continue;
      if (S.ShapeType() == TopAbs_EDGE)
        MW.Add(TopoDS::Edge(S));
      else if (S.ShapeType() == TopAbs_WIRE)
        MW.Add(TopoDS::Wire(S));
      else
        continue;
    }
  else
  {
    NCollection_List<TopoDS_Shape> aLE;
    for (i = 3; i < n; i++)
    {
      TopoDS_Shape    S = DBRep::Get(a[i]);
      TopExp_Explorer Exp(S, TopAbs_EDGE);
      for (; Exp.More(); Exp.Next())
      {
        const TopoDS_Edge& anE = TopoDS::Edge(Exp.Current());
        if (!anE.IsNull())
          aLE.Append(anE);
      }
    }
    MW.Add(aLE);
  }

  if (!MW.IsDone())
  {
    // std::cout << "Wire not done" << std::endl;
    di << "Wire not done with an error:\n";
    switch (MW.Error())
    {
      case BRepBuilderAPI_EmptyWire:
        di << "BRepBuilderAPI_EmptyWire\n";
        break;
      case BRepBuilderAPI_DisconnectedWire:
        di << "BRepBuilderAPI_DisconnectedWire\n";
        break;
      case BRepBuilderAPI_NonManifoldWire:
        di << "BRepBuilderAPI_NonManifoldWire\n";
        break;
      default:
        break;
    }
  }
  else
    DBRep::Set(a[1], MW);
  return 0;
}

//=======================================================================
// mkedge
//=======================================================================

static int mkedge(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;

  occ::handle<Geom_Curve>     C   = DrawTrSurf::GetCurve(a[2]);
  occ::handle<Geom2d_Curve>   C2d = DrawTrSurf::GetCurve2d(a[2]);
  occ::handle<Poly_Polygon3D> P3d = DrawTrSurf::GetPolygon3D(a[2]);

  if (C.IsNull() && C2d.IsNull() && P3d.IsNull())
  {
    // std::cout << a[2] << " is not a curve" << std::endl;
    di << a[2] << " is not a curve or polygon 3d\n";
    return 1;
  }

  TopoDS_Edge edge;

  if (n == 3)
  {
    if (!C.IsNull())
      edge = BRepBuilderAPI_MakeEdge(C);
    else if (!C2d.IsNull())
      edge = BRepBuilderAPI_MakeEdge2d(C2d);
    else
    {
      BRep_Builder aBB;
      aBB.MakeEdge(edge, P3d);
    }
  }
  else
  {
    occ::handle<Geom_Surface> S;
    int                       i = 0;
    if (!C2d.IsNull())
    {
      S = DrawTrSurf::GetSurface(a[3]);
      if (!S.IsNull())
        i = 1;
    }
    TopoDS_Shape  aLocalShape(DBRep::Get(a[3 + i], TopAbs_VERTEX));
    TopoDS_Vertex V1 = TopoDS::Vertex(aLocalShape);
    //    TopoDS_Vertex V1 = TopoDS::Vertex(DBRep::Get(a[3+i],TopAbs_VERTEX));
    if (n == 5 + i)
    {
      if (V1.IsNull())
      {
        if (!C.IsNull())
          edge = BRepBuilderAPI_MakeEdge(C, Draw::Atof(a[3]), Draw::Atof(a[4]));
        else if (S.IsNull())
          edge = BRepBuilderAPI_MakeEdge2d(C2d, Draw::Atof(a[3]), Draw::Atof(a[4]));
        else
          edge = BRepBuilderAPI_MakeEdge(C2d, S, Draw::Atof(a[4]), Draw::Atof(a[5]));
      }
      else
      {
        aLocalShape      = DBRep::Get(a[4 + i], TopAbs_VERTEX);
        TopoDS_Vertex V2 = TopoDS::Vertex(aLocalShape);
        //  TopoDS_Vertex V2 = TopoDS::Vertex(DBRep::Get(a[4+i],TopAbs_VERTEX));
        if (!C.IsNull())
          edge = BRepBuilderAPI_MakeEdge(C, V1, V2);
        else if (S.IsNull())
          edge = BRepBuilderAPI_MakeEdge2d(C2d, V1, V2);
        else
          edge = BRepBuilderAPI_MakeEdge(C2d, S, V1, V2);
      }
    }
    else if (n == 7 + i)
    {
      aLocalShape      = DBRep::Get(a[5 + i], TopAbs_VERTEX);
      TopoDS_Vertex V2 = TopoDS::Vertex(aLocalShape);
      //      TopoDS_Vertex V2 = TopoDS::Vertex(DBRep::Get(a[5+i],TopAbs_VERTEX));
      if (!C.IsNull())
        edge = BRepBuilderAPI_MakeEdge(C, V1, V2, Draw::Atof(a[4]), Draw::Atof(a[6]));
      else if (S.IsNull())
        edge = BRepBuilderAPI_MakeEdge2d(C2d, V1, V2, Draw::Atof(a[4]), Draw::Atof(a[6]));
      else
        edge = BRepBuilderAPI_MakeEdge(C2d, S, V1, V2, Draw::Atof(a[5]), Draw::Atof(a[7]));
    }
    else
      return 1;
  }

  DBRep::Set(a[1], edge);
  return 0;
}

//=======================================================================
// mkcurve
//=======================================================================
Standard_IMPORT Draw_Color DrawTrSurf_CurveColor(const Draw_Color col);
Standard_IMPORT void       DBRep_WriteColorOrientation();
Standard_IMPORT Draw_Color DBRep_ColorOrientation(const TopAbs_Orientation Or);

static int mkcurve(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;

  bool DispOrientation = !strcmp(a[0], "mkoricurve");

  TopoDS_Shape S = DBRep::Get(a[2], TopAbs_EDGE);
  if (S.IsNull())
    return 1;
  TopLoc_Location L;
  double          f, l;

  occ::handle<Geom_Curve> C = BRep_Tool::Curve(TopoDS::Edge(S), L, f, l);
  if (C.IsNull())
  {
    // std::cout << a[2] << " has no 3d curve" << std::endl;
    di << a[2] << " has no 3d curve\n";
    return 1;
  }
  C = new Geom_TrimmedCurve(C, f, l);

  Draw_Color col, savecol;

  if (DispOrientation)
  {
    DBRep_WriteColorOrientation();
    col     = DBRep_ColorOrientation(TopoDS::Edge(S).Orientation());
    savecol = DrawTrSurf_CurveColor(col);
  }
  DrawTrSurf::Set(a[1], C->Transformed(L.Transformation()));
  if (DispOrientation)
  {
    DrawTrSurf_CurveColor(savecol);
  }

  return 0;
}

//=================================================================================================

static int mkpoint(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;

  TopoDS_Shape S = DBRep::Get(a[2], TopAbs_VERTEX);
  if (S.IsNull())
    return 1;

  TopoDS_Vertex V = TopoDS::Vertex(S);

  gp_Pnt P = BRep_Tool::Pnt(V);
  DrawTrSurf::Set(a[1], P);

  return 0;
}

//=======================================================================
// mk2dcurve
//=======================================================================

static int mk2dcurve(Draw_Interpretor& di, int na, const char** a)
{
  if (na < 3)
    return 1;

  TopoDS_Shape S;
  S = DBRep::Get(a[2], TopAbs_EDGE);
  if (S.IsNull())
    return 1;

  TopoDS_Edge E = TopoDS::Edge(S);

  TopLoc_Location           L;
  double                    f = 0., l = 0.;
  occ::handle<Geom2d_Curve> C;
  occ::handle<Geom_Surface> Surf;

  bool hasFace = false;

  if (na == 3)
  {
    // get the first PCurve connected to edge E
    BRep_Tool::CurveOnSurface(E, C, Surf, L, f, l);
  }
  else if (na == 4)
  {
    S = DBRep::Get(a[3], TopAbs_FACE);
    if (S.IsNull())
    {
      int ind = Draw::Atoi(a[3]);
      BRep_Tool::CurveOnSurface(E, C, Surf, L, f, l, ind);
    }
    else
    {
      hasFace       = true;
      TopoDS_Face F = TopoDS::Face(S);
      C             = BRep_Tool::CurveOnSurface(E, F, f, l);
    }
  }

  if (C.IsNull())
  {
    // std::cout << a[2] << " has no 2d curve"; if (na == 4) std::cout << " on " << a[3];
    // std::cout << std::endl;
    di << a[2] << " has no 2d curve";

    if (hasFace)
    {
      di << " on " << a[3];
    }

    di << "\n";
    return 1;
  }
  C = new Geom2d_TrimmedCurve(C, f, l);
  DrawTrSurf::Set(a[1], C);

  return 0;
}

//=======================================================================
// edge
//=======================================================================

static int edge(Draw_Interpretor&, int n, const char** a)
{
  if (n < 4)
    return 1;
  TopoDS_Shape V1 = DBRep::Get(a[2], TopAbs_VERTEX);
  TopoDS_Shape V2 = DBRep::Get(a[3], TopAbs_VERTEX);
  if (V1.IsNull() || V2.IsNull())
    return 1;
  TopoDS_Edge E = BRepBuilderAPI_MakeEdge(TopoDS::Vertex(V1), TopoDS::Vertex(V2));
  DBRep::Set(a[1], E);
  return 0;
}

//=======================================================================
// isoedge
//=======================================================================

static int isoedge(Draw_Interpretor&, int n, const char** a)
{
  if (n < 6)
    return 1;

  bool         uiso = *a[0] == 'u';
  double       p    = Draw::Atof(a[3]);
  double       p1   = Draw::Atof(a[4]);
  double       p2   = Draw::Atof(a[5]);
  TopoDS_Shape Sh   = DBRep::Get(a[2], TopAbs_FACE);
  if (Sh.IsNull())
    return 1;
  TopLoc_Location                  Loc;
  const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(TopoDS::Face(Sh), Loc);
  double                           UMin, UMax, VMin, VMax;
  BRepTools::UVBounds(TopoDS::Face(Sh), UMin, UMax, VMin, VMax);
  if (uiso)
  {
    VMin = std::min(VMin, std::min(p1, p2));
    VMax = std::max(VMax, std::max(p1, p2));
  }
  else
  {
    UMin = std::min(UMin, std::min(p1, p2));
    UMax = std::max(VMax, std::max(p1, p2));
  }

  occ::handle<Geom_RectangularTrimmedSurface> TS =
    new Geom_RectangularTrimmedSurface(S, UMin, UMax, VMin, VMax);
  occ::handle<Geom_Curve>  C;
  occ::handle<Geom2d_Line> L;
  if (uiso)
  {
    C = TS->UIso(p);
    L = new Geom2d_Line(gp_Pnt2d(p, 0), gp_Dir2d(gp_Dir2d::D::Y));
  }
  else
  {
    C = TS->VIso(p);
    L = new Geom2d_Line(gp_Pnt2d(0, p), gp_Dir2d(gp_Dir2d::D::X));
  }

  TopoDS_Edge E = BRepBuilderAPI_MakeEdge(C, p1, p2);
  E.Location(Loc);
  BRep_Builder B;
  B.UpdateEdge(E, L, TopoDS::Face(Sh), 0.);

  DBRep::Set(a[1], E);
  return 0;
}

//=======================================================================
// transfert
//=======================================================================

static int transfert(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;

  TopoDS_Shape E1 = DBRep::Get(a[1], TopAbs_EDGE);
  TopoDS_Shape E2 = DBRep::Get(a[2], TopAbs_EDGE);
  if (E1.IsNull() || E2.IsNull())
    return 1;

  BRep_Builder B;
  B.Transfert(TopoDS::Edge(E1), TopoDS::Edge(E2));
  return 0;
}

//=======================================================================
// profile
// command to build a profile
//=======================================================================

static int profile(Draw_Interpretor& di, int n, const char** a)
{
  // this command build a profile
  // from a moving point

  if (n == 1)
  {
    // print help

    // std::cout << "profile name [code values] [code values] ...\n";
    // std::cout << "  Build a profile in the XY plane from a moving point and direction\n";
    // std::cout << "  The original point and direction are 0 0 and 1 0\n";
    // std::cout << "  Codes and values describe the point or direction change\n";
    // std::cout << "  When the point change the direction becomes the tangent\n";
    // std::cout << "  All angles are in degree (may be negative)\n";
    // std::cout << "  By default the profile is closed and a face is built\n";
    // std::cout << "\n";
    // std::cout << "  Instruction Parameters         Action\n";
    // std::cout << "  O           X Y Z              Set the origin of the plane\n";
    // std::cout << "  P           DX DY DZ UX UY UZ  Set the normal and X of the plane\n";
    // std::cout << "  S           Face               Set the Plane (Face must be a Face)\n";
    // std::cout << "  F           X Y                Set the first point\n";
    // std::cout << "  X           DX                 Translate point along X\n";
    // std::cout << "  Y           DY                 Translate point along Y\n";
    // std::cout << "  L           DL                 Translate point along direction\n";
    // std::cout << "  XX          X                  Set point X coordinate\n";
    // std::cout << "  YY          Y                  Set point Y coordinate\n";
    // std::cout << "  T           DX DY              Translate point\n";
    // std::cout << "  TT          X Y                Set point\n";
    // std::cout << "  R           Angle              Rotate direction\n";
    // std::cout << "  RR          Angle              Set direction\n";
    // std::cout << "  D           DX DY              Set direction\n";
    // std::cout << "  IX          X                  Intersect with vertical\n";
    // std::cout << "  IY          Y                  Intersect with horizontal\n";
    // std::cout << "  C           Radius Angle       Arc of circle tangent to direction\n";
    // std::cout << "  W                              Make a closed wire\n";
    // std::cout << "  WW                             Make an open wire\n";
    // std::cout << std::endl;
    di << "profile name [code values] [code values] ...\n";
    di << "  Build a profile in the XY plane from a moving point and direction\n";
    di << "  The original point and direction are 0 0 and 1 0\n";
    di << "  Codes and values describe the point or direction change\n";
    di << "  When the point change the direction becomes the tangent\n";
    di << "  All angles are in degree (may be negative)\n";
    di << "  By default the profile is closed and a face is built\n";
    di << "\n";
    di << "  Instruction Parameters         Action\n";
    di << "  O           X Y Z              Set the origin of the plane\n";
    di << "  P           DX DY DZ UX UY UZ  Set the normal and X of the plane\n";
    di << "  S           Face               Set the Plane (Face must be a Face)\n";
    di << "  F           X Y                Set the first point\n";
    di << "  X           DX                 Translate point along X\n";
    di << "  Y           DY                 Translate point along Y\n";
    di << "  L           DL                 Translate point along direction\n";
    di << "  XX          X                  Set point X coordinate\n";
    di << "  YY          Y                  Set point Y coordinate\n";
    di << "  T           DX DY              Translate point\n";
    di << "  TT          X Y                Set point\n";
    di << "  R           Angle              Rotate direction\n";
    di << "  RR          Angle              Set direction\n";
    di << "  D           DX DY              Set direction\n";
    di << "  IX          X                  Intersect with vertical\n";
    di << "  IY          Y                  Intersect with horizontal\n";
    di << "  C           Radius Angle       Arc of circle tangent to direction\n";
    di << "  W                              Make a closed wire\n";
    di << "  WW                             Make an open wire\n";
    di << "\n";
    return 0;
  }

  int                     i  = 2;
  double                  x0 = 0, y0 = 0, x = 0, y = 0, dx = 1, dy = 0;
  BRepBuilderAPI_MakeWire MW;
  gp_Ax3                  DummyHP(gp::XOY());
  gp_Pln                  P(DummyHP);

  enum
  {
    line,
    circle,
    none
  } move;

  bool                      face      = true;
  bool                      close     = true;
  bool                      first     = true;
  bool                      stayfirst = false;
  bool                      isplanar  = true;
  TopoDS_Shape              S;
  TopLoc_Location           TheLocation;
  occ::handle<Geom_Surface> Surface;
  while (i < n)
  {

    double length = 0, radius = 0, angle = 0;
    move = none;

    switch (a[i][0])
    {

      case 'F':
      case 'f':
        i += 2;
        if (i >= n)
          goto badargs;
        if (!first)
        {
          di << "profile: The F instruction must precede all moves";
          return 1;
        }
        x0 = x = Draw::Atof(a[i - 1]);
        y0 = y    = Draw::Atof(a[i]);
        stayfirst = true;
        break;

      case 'O':
      case 'o':
        i += 3;
        if (i >= n)
          goto badargs;
        P.SetLocation(gp_Pnt(Draw::Atof(a[i - 2]), Draw::Atof(a[i - 1]), Draw::Atof(a[i])));
        stayfirst = true;
        break;

      case 'P':
      case 'p':
        i += 6;
        if (i >= n)
          goto badargs;
        {
          gp_Vec vn(Draw::Atof(a[i - 5]), Draw::Atof(a[i - 4]), Draw::Atof(a[i - 3]));
          gp_Vec vx(Draw::Atof(a[i - 2]), Draw::Atof(a[i - 1]), Draw::Atof(a[i]));
          if (vn.Magnitude() <= Precision::Confusion())
          {
            di << "profile : null direction";
            return 1;
          }
          if (vx.Magnitude() <= Precision::Confusion())
          {
            di << "profile : null direction";
            return 1;
          }
          gp_Ax2 ax(P.Location(), vn, vx);
          P.SetPosition(ax);
        }
        stayfirst = true;
        break;

      case 'S':
      case 's':
        i += 1;
        if (i >= n)
          goto badargs;
        {
          TopoDS_Shape aLocalShape(DBRep::Get(a[i], TopAbs_FACE));
          TopoDS_Face  Face = TopoDS::Face(aLocalShape);
          //  TopoDS_Face Face = TopoDS::Face(DBRep::Get(a[i],TopAbs_FACE));
          if (Face.IsNull())
          {
            di << "profile : no face found";
            return 1;
          }
          Surface                       = BRep_Tool::Surface(Face, TheLocation);
          occ::handle<Geom_Plane> Plane = occ::down_cast<Geom_Plane>(Surface);
          if (Plane.IsNull())
          {
            isplanar = false;
          }
          else
            P = Plane->Pln();
        }
        stayfirst = true;
        break;

      case 'X':
      case 'x':
        i++;
        if (i >= n)
          goto badargs;
        length = Draw::Atof(a[i]);
        if ((a[i - 1][1] == 'X') || (a[i - 1][1] == 'x'))
        {
          length -= x;
        }
        dx   = 1;
        dy   = 0;
        move = line;
        break;

      case 'Y':
      case 'y':
        i++;
        if (i >= n)
          goto badargs;
        length = Draw::Atof(a[i]);
        if ((a[i - 1][1] == 'Y') || (a[i - 1][1] == 'y'))
        {
          length -= y;
        }
        dx   = 0;
        dy   = 1;
        move = line;
        break;

      case 'L':
      case 'l':
        i++;
        if (i >= n)
          goto badargs;
        length = Draw::Atof(a[i]);
        move   = line;
        break;

      case 'T':
      case 't':
        i += 2;
        if (i >= n)
          goto badargs;
        {
          double vx = Draw::Atof(a[i - 1]);
          double vy = Draw::Atof(a[i]);
          if ((a[i - 2][1] == 'T') || (a[i - 2][1] == 't'))
          {
            vx -= x;
            vy -= y;
          }
          length = std::sqrt(vx * vx + vy * vy);
          if (length > Precision::Confusion())
          {
            move = line;
            dx   = vx / length;
            dy   = vy / length;
          }
        }
        break;

      case 'R':
      case 'r':
        i++;
        if (i >= n)
          goto badargs;
        angle = Draw::Atof(a[i]) * (M_PI / 180.0);
        if ((a[i - 1][1] == 'R') || (a[i - 1][1] == 'r'))
        {
          dx = std::cos(angle);
          dy = std::sin(angle);
        }
        else
        {
          double c = std::cos(angle);
          double s = std::sin(angle);
          double t = c * dx - s * dy;
          dy       = s * dx + c * dy;
          dx       = t;
        }
        break;

      case 'D':
      case 'd':
        i += 2;
        if (i >= n)
          goto badargs;
        {
          double vx = Draw::Atof(a[i - 1]);
          double vy = Draw::Atof(a[i]);
          length    = std::sqrt(vx * vx + vy * vy);
          if (length > Precision::Confusion())
          {
            // move = line; DUB
            dx = vx / length;
            dy = vy / length;
          }
        }
        break;

      case 'C':
      case 'c':
        i += 2;
        if (i >= n)
          goto badargs;
        radius = Draw::Atof(a[i - 1]);
        if (std::abs(radius) > Precision::Confusion())
        {
          angle = Draw::Atof(a[i]) * (M_PI / 180.0);
          move  = circle;
        }
        break;

      case 'I':
      case 'i':
        i++;
        if (i >= n)
          goto badargs;
        length = Draw::Atof(a[i]);
        if ((a[i - 1][1] == 'X') || (a[i - 1][1] == 'x'))
        {
          if (std::abs(dx) < Precision::Confusion())
          {
            di << "Profile : cannot intersect, arg " << i - 1;
            return 1;
          }
          length = (length - x) / dx;
          move   = line;
        }
        else if ((a[i - 1][1] == 'Y') || (a[i - 1][1] == 'y'))
        {
          if (std::abs(dy) < Precision::Confusion())
          {
            di << "Profile : cannot intersect, arg " << i - 1;
            return 1;
          }
          length = (length - y) / dy;
          move   = line;
        }

        break;

      case 'W':
      case 'w':
        face = false;
        if ((a[i][1] == 'W') || (a[i][1] == 'w'))
        {
          close = false;
        }
        i = n - 1;
        break;

      default:
        di << "profile : unknown code " << a[i];
        return 1;
    }

  again:

    switch (move)
    {

      case line: {
        if (length < 0)
        {
          length = -length;
          dx     = -dx;
          dy     = -dy;
        }
        occ::handle<Geom2d_Line> l = new Geom2d_Line(gp_Pnt2d(x, y), gp_Dir2d(dx, dy));
        if (isplanar)
          MW.Add(BRepBuilderAPI_MakeEdge(GeomAPI::To3d(l, P), 0, length));
        else
          MW.Add(BRepBuilderAPI_MakeEdge(l, Surface, 0, length));
        x += length * dx;
        y += length * dy;
      }
      break;

      case circle: {
        bool sense = true;
        if (radius < 0)
        {
          radius = -radius;
          sense  = !sense;
          dx     = -dx;
          dy     = -dy;
        }
        gp_Ax2d ax(gp_Pnt2d(x - radius * dy, y + radius * dx), gp_Dir2d(dy, -dx));
        if (angle < 0)
        {
          angle = -angle;
          sense = !sense;
        }
        occ::handle<Geom2d_Circle> c = new Geom2d_Circle(ax, radius, sense);
        if (isplanar)
          MW.Add(BRepBuilderAPI_MakeEdge(GeomAPI::To3d(c, P), 0, angle));
        else
          MW.Add(BRepBuilderAPI_MakeEdge(c, Surface, 0, angle));
        gp_Pnt2d p;
        gp_Vec2d v;
        c->D1(angle, p, v);
        x  = p.X();
        y  = p.Y();
        dx = v.X() / radius;
        dy = v.Y() / radius;
      }
      break;

      case none:
        break;
    }

    // update first
    first     = stayfirst;
    stayfirst = false;

    // next segment....
    i++;
    if ((i == n) && close)
    {
      // the closing segment
      dx     = x0 - x;
      dy     = y0 - y;
      length = std::sqrt(dx * dx + dy * dy);
      if (length > Precision::Confusion())
      {
        move = line;
        dx   = dx / length;
        dy   = dy / length;
        goto again;
      }
    }
  }

  // get the result, face or wire

  if (face)
  {
    if (isplanar)
      S = BRepBuilderAPI_MakeFace(P, MW.Wire());
    else
    {
      BRepBuilderAPI_MakeFace MFace;
      MFace.Init(Surface, false, Precision::Confusion());
      MFace.Add(MW.Wire());
      S = MFace.Face();
    }
  }
  else
  {
    S = MW;
  }

  if (!TheLocation.IsIdentity())
    S.Move(TheLocation);

  if (!isplanar)
  {
    double Tol = 1.e-5;
    BRepLib::BuildCurves3d(S, Tol);
  }

  DBRep::Set(a[1], S);

  return 0;

badargs:
  di << "profile : bad number of arguments";
  return 1;
}

//=======================================================================
// profile
// command to build a profile
//=======================================================================

static int bsplineprof(Draw_Interpretor& di, int n, const char** a)
{
  // this command build a profile
  // from a moving point

  if (n == 1)
  {
    // print help

    // std::cout << " bsplineprof name [S face] [W WW]  "              << std::endl;
    // std::cout << " for an edge : <digitizes> ... <mouse button 2> " << std::endl ;
    // std::cout << " to end profile : <mouse button 3> "              << std::endl ;
    // std::cout << "  Build a profile in the XY plane from digitizes" << std::endl ;
    // std::cout << "  By default the profile is closed and a face is built\n";
    // std::cout << "\n";
    // std::cout << "  W                              Make a closed wire\n";
    // std::cout << "  WW                             Make an open wire\n";
    // std::cout << std::endl;
    di << " bsplineprof name [S face] [W WW]  " << "\n";
    di << " for an edge : <digitizes> ... <mouse button 2> " << "\n";
    di << " to end profile : <mouse button 3> " << "\n";
    di << "  Build a profile in the XY plane from digitizes" << "\n";
    di << "  By default the profile is closed and a face is built\n";
    di << "\n";
    di << "  W                              Make a closed wire\n";
    di << "  WW                             Make an open wire\n";
    di << "\n";
    return 0;
  }

  gp_Pnt2d last_point(0.0e0, 0.0e0);
  gp_Pnt2d first_point(0.0e0, 0.0e0);
  int      i    = 2;
  bool     wait = true;
  //  double x0 = 0, y0 = 0, x = 0, y = 0, dx = 1, dy = 0;
  double                  x = 0, y = 0, dx = 1, dy = 0;
  BRepBuilderAPI_MakeWire MW;
  gp_Ax3                  DummyHP(gp::XOY());
  gp_Pln                  P(DummyHP);
  bool                    face  = true;
  bool                    close = true;
  //  bool first = true;
  bool                      isplanar = true;
  double                    length;
  TopoDS_Shape              S;
  TopLoc_Location           TheLocation;
  occ::handle<Geom_Surface> Surface;
  if (n > 2)
  {
    while (i < n)
    {

      switch (a[i][0])
      {

        case 'S':
        case 's':
          i += 1;
          {
            TopoDS_Shape aLocalShape(DBRep::Get(a[i], TopAbs_FACE));
            TopoDS_Face  Face = TopoDS::Face(aLocalShape);
            //    TopoDS_Face Face = TopoDS::Face(DBRep::Get(a[i],TopAbs_FACE));
            if (Face.IsNull())
            {
              di << "profile : no face found";
              return 1;
            }
            Surface                       = BRep_Tool::Surface(Face, TheLocation);
            occ::handle<Geom_Plane> Plane = occ::down_cast<Geom_Plane>(Surface);
            if (Plane.IsNull())
            {
              isplanar = false;
            }
            else
              P = Plane->Pln();
          }
          i += 1;
          break;

        case 'W':
        case 'w':
          face = false;
          if ((a[i][1] == 'W') || (a[i][1] == 'w'))
          {
            close = false;
          }
          i = n - 1;
          break;

        default:
          di << "profile : unknown code " << a[i];
          return 1;
      }
    }
  }
  //
  //  to be done : close the profile using the first point of the contour
  //               and the point taken with mouse button 3
  //
  occ::handle<Geom2d_BSplineCurve>           C;
  occ::handle<Geom_Curve>                    curve3d_ptr;
  int                                        id, ii;
  int                                        X, Y, b, not_done;
  int                                        num_points = 0;
  gp_Pnt2d                                   a_point(0.0e0, 0.0e0);
  occ::handle<NCollection_HArray1<gp_Pnt2d>> points_array_ptr =
    new NCollection_HArray1<gp_Pnt2d>(1, 1);
  occ::handle<NCollection_HArray1<gp_Pnt2d>> new_points_array_ptr;

  not_done = 1;
  while (not_done)
  {

    dout.Select(id, X, Y, b, wait);
    double z = dout.Zoom(id);
    a_point.SetCoord(1, (double)X / z);
    a_point.SetCoord(2, (double)Y / z);
    if (num_points == 0)
    {
      points_array_ptr                    = new NCollection_HArray1<gp_Pnt2d>(1, 1);
      points_array_ptr->ChangeArray1()(1) = a_point;
      first_point                         = a_point;
    }
    num_points += 1;
    if (num_points >= 2)
    {
      new_points_array_ptr = new NCollection_HArray1<gp_Pnt2d>(1, num_points);
      for (ii = 1; ii <= num_points - 1; ii++)
      {
        new_points_array_ptr->ChangeArray1()(ii) = points_array_ptr->Array1()(ii);
      }
      new_points_array_ptr->ChangeArray1()(num_points) = a_point;
      Geom2dAPI_Interpolate a2dInterpolator(new_points_array_ptr, false, 1.0e-5);
      a2dInterpolator.Perform();
      if (a2dInterpolator.IsDone())
      {
        C           = a2dInterpolator.Curve();
        curve3d_ptr = GeomAPI::To3d(C, P);
        DrawTrSurf::Set(a[1], curve3d_ptr);
        dout.RepaintView(id);
      }
      points_array_ptr = new_points_array_ptr;
    }

    if (b == 2 || b == 3)
    {
      if (num_points == 2)
      {
        x  = last_point.Coord(1);
        y  = last_point.Coord(2);
        dx = a_point.Coord(1) - x;
        dy = a_point.Coord(2) - y;
        gp_Vec2d a_vector(dx, dy);
        length                     = a_vector.Magnitude();
        occ::handle<Geom2d_Line> l = new Geom2d_Line(gp_Pnt2d(x, y), gp_Dir2d(dx, dy));
        if (isplanar)
        {
          MW.Add(BRepBuilderAPI_MakeEdge(GeomAPI::To3d(l, P), 0, length));
        }
        else
        {
          MW.Add(BRepBuilderAPI_MakeEdge(l, Surface, 0, length));
        }
      }
      else if (num_points > 2)
      {
        if (isplanar)
        {
          MW.Add(BRepBuilderAPI_MakeEdge(curve3d_ptr,
                                         curve3d_ptr->FirstParameter(),
                                         curve3d_ptr->LastParameter()));
        }
        else
        {
          MW.Add(BRepBuilderAPI_MakeEdge(C, Surface, C->FirstParameter(), C->LastParameter()));
        }
      }
      if (num_points >= 2)
      {
        last_point                          = a_point;
        points_array_ptr->ChangeArray1()(1) = a_point;
        num_points                          = 1;
        DBRep::Set(a[1], MW.Wire());
      }
    }
    if (b == 3)
    {
      not_done = 0;
    }
  }
  a_point = first_point;
  if (close)
  {

    x  = last_point.Coord(1);
    y  = last_point.Coord(2);
    dx = a_point.Coord(1) - x;
    dy = a_point.Coord(2) - y;
    gp_Vec2d a_vector(dx, dy);
    length                     = a_vector.Magnitude();
    occ::handle<Geom2d_Line> l = new Geom2d_Line(gp_Pnt2d(x, y), gp_Dir2d(dx, dy));
    if (isplanar)
      MW.Add(BRepBuilderAPI_MakeEdge(GeomAPI::To3d(l, P), 0, length));
    else
      MW.Add(BRepBuilderAPI_MakeEdge(l, Surface, 0, length));
  }
  if (face)
  {
    if (isplanar)
      S = BRepBuilderAPI_MakeFace(P, MW.Wire());
    else
    {
      BRepBuilderAPI_MakeFace MFace;
      MFace.Init(Surface, false, Precision::Confusion());
      MFace.Add(MW.Wire());
      S = MFace.Face();
    }
  }
  else
  {
    S = MW;
  }

  if (!TheLocation.IsIdentity())
    S.Move(TheLocation);

  if (!isplanar)
  {
    double Tol = 1.e-5;
    BRepLib::BuildCurves3d(S, Tol);
  }

  DBRep::Set(a[1], S);

  return 0;
}

//=======================================================================
// 2dprofile
// command to build a profile
//=======================================================================

static int profile2d(Draw_Interpretor& di, int n, const char** a)
{
  // this command build a profile with 2d curves.
  // from a moving point

  if (n == 1)
  {
    // print help

    // std::cout << "profile name [code values] [code values] ...\n";
    // std::cout << "  Build a profile in the UV plane from a moving point and direction\n";
    // std::cout << "  The original point and direction are 0 0 and 1 0\n";
    // std::cout << "  Codes and values describe the point or direction change\n";
    // std::cout << "  When the point change the direction becomes the tangent\n";
    // std::cout << "  All angles are in degree (may be negative)\n";
    // std::cout << "  By default the profile is closed \n";
    // std::cout << "\n";
    // std::cout << "  Instruction Parameters         Action\n";
    // std::cout << "  F           X Y                Set the first point\n";
    // std::cout << "  X           DX                 Translate point along X\n";
    // std::cout << "  Y           DY                 Translate point along Y\n";
    // std::cout << "  L           DL                 Translate point along direction\n";
    // std::cout << "  XX          X                  Set point X coordinate\n";
    // std::cout << "  YY          Y                  Set point Y coordinate\n";
    // std::cout << "  T           DX DY              Translate point\n";
    // std::cout << "  TT          X Y                Set point\n";
    // std::cout << "  R           Angle              Rotate direction\n";
    // std::cout << "  RR          Angle              Set direction\n";
    // std::cout << "  D           DX DY              Set direction\n";
    // std::cout << "  IX          X                  Intersect with vertical\n";
    // std::cout << "  IY          Y                  Intersect with horizontal\n";
    // std::cout << "  C           Radius Angle       Arc of circle tangent to direction\n";
    // std::cout << "  W                              Make a closed wire\n";
    // std::cout << std::endl;
    di << "profile name [code values] [code values] ...\n";
    di << "  Build a profile in the UV plane from a moving point and direction\n";
    di << "  The original point and direction are 0 0 and 1 0\n";
    di << "  Codes and values describe the point or direction change\n";
    di << "  When the point change the direction becomes the tangent\n";
    di << "  All angles are in degree (may be negative)\n";
    di << "  By default the profile is closed \n";
    di << "\n";
    di << "  Instruction Parameters         Action\n";
    di << "  F           X Y                Set the first point\n";
    di << "  X           DX                 Translate point along X\n";
    di << "  Y           DY                 Translate point along Y\n";
    di << "  L           DL                 Translate point along direction\n";
    di << "  XX          X                  Set point X coordinate\n";
    di << "  YY          Y                  Set point Y coordinate\n";
    di << "  T           DX DY              Translate point\n";
    di << "  TT          X Y                Set point\n";
    di << "  R           Angle              Rotate direction\n";
    di << "  RR          Angle              Set direction\n";
    di << "  D           DX DY              Set direction\n";
    di << "  IX          X                  Intersect with vertical\n";
    di << "  IY          Y                  Intersect with horizontal\n";
    di << "  C           Radius Angle       Arc of circle tangent to direction\n";
    di << "  W                              Make a closed wire\n";
    di << "\n";
    return 0;
  }

  int    i = 2, NbCurves = 0;
  double x0 = 0, y0 = 0, x = 0, y = 0, dx = 1, dy = 0;

  enum
  {
    line,
    circle,
    none
  } move;

  bool  close     = true;
  bool  first     = true;
  bool  stayfirst = false;
  char* name      = new char[100];

  while (i < n)
  {

    double length = 0, radius = 0, angle = 0;
    move = none;

    switch (a[i][0])
    {

      case 'F':
      case 'f':
        i += 2;
        if (i >= n)
          goto badargs;
        if (!first)
        {
          di << "profile: The F instruction must precede all moves";
          return 1;
        }
        x0 = x = Draw::Atof(a[i - 1]);
        y0 = y    = Draw::Atof(a[i]);
        stayfirst = true;
        break;

      case 'X':
      case 'x':
        i++;
        if (i >= n)
          goto badargs;
        length = Draw::Atof(a[i]);
        if ((a[i - 1][1] == 'X') || (a[i - 1][1] == 'x'))
        {
          length -= x;
        }
        dx   = 1;
        dy   = 0;
        move = line;
        break;

      case 'Y':
      case 'y':
        i++;
        if (i >= n)
          goto badargs;
        length = Draw::Atof(a[i]);
        if ((a[i - 1][1] == 'Y') || (a[i - 1][1] == 'y'))
        {
          length -= y;
        }
        dx   = 0;
        dy   = 1;
        move = line;
        break;

      case 'L':
      case 'l':
        i++;
        if (i >= n)
          goto badargs;
        length = Draw::Atof(a[i]);
        move   = line;
        break;

      case 'T':
      case 't':
        i += 2;
        if (i >= n)
          goto badargs;
        {
          double vx = Draw::Atof(a[i - 1]);
          double vy = Draw::Atof(a[i]);
          if ((a[i - 2][1] == 'T') || (a[i - 2][1] == 't'))
          {
            vx -= x;
            vy -= y;
          }
          length = std::sqrt(vx * vx + vy * vy);
          if (length > Precision::Confusion())
          {
            move = line;
            dx   = vx / length;
            dy   = vy / length;
          }
        }
        break;

      case 'R':
      case 'r':
        i++;
        if (i >= n)
          goto badargs;
        angle = Draw::Atof(a[i]) * (M_PI / 180.0);
        if ((a[i - 1][1] == 'R') || (a[i - 1][1] == 'r'))
        {
          dx = std::cos(angle);
          dy = std::sin(angle);
        }
        else
        {
          double c = std::cos(angle);
          double s = std::sin(angle);
          double t = c * dx - s * dy;
          dy       = s * dx + c * dy;
          dx       = t;
        }
        break;

      case 'D':
      case 'd':
        i += 2;
        if (i >= n)
          goto badargs;
        {
          double vx = Draw::Atof(a[i - 1]);
          double vy = Draw::Atof(a[i]);
          length    = std::sqrt(vx * vx + vy * vy);
          if (length > Precision::Confusion())
          {
            // move = line; DUB
            dx = vx / length;
            dy = vy / length;
          }
        }
        break;

      case 'C':
      case 'c':
        i += 2;
        if (i >= n)
          goto badargs;
        radius = Draw::Atof(a[i - 1]);
        if (std::abs(radius) > Precision::Confusion())
        {
          angle = Draw::Atof(a[i]) * (M_PI / 180.0);
          move  = circle;
        }
        break;

      case 'I':
      case 'i':
        i++;
        if (i >= n)
          goto badargs;
        length = Draw::Atof(a[i]);
        if ((a[i - 1][1] == 'X') || (a[i - 1][1] == 'x'))
        {
          if (std::abs(dx) < Precision::Confusion())
          {
            di << "Profile : cannot intersect, arg " << i - 1;
            return 1;
          }
          length = (length - x) / dx;
          move   = line;
        }
        else if ((a[i - 1][1] == 'Y') || (a[i - 1][1] == 'y'))
        {
          if (std::abs(dy) < Precision::Confusion())
          {
            di << "Profile : cannot intersect, arg " << i - 1;
            return 1;
          }
          length = (length - y) / dy;
          move   = line;
        }

        break;

      case 'W':
      case 'w':
        close = false;
        i     = n - 1;
        break;

      default:
        di << "profile : unknown code " << a[i];
        return 1;
    }

  again:

    switch (move)
    {

      case line: {
        if (length < 0)
        {
          length = -length;
          dx     = -dx;
          dy     = -dy;
        }
        occ::handle<Geom2d_Line>         l  = new Geom2d_Line(gp_Pnt2d(x, y), gp_Dir2d(dx, dy));
        occ::handle<Geom2d_TrimmedCurve> ct = new Geom2d_TrimmedCurve(l, 0, length);
        NbCurves++;
        Sprintf(name, "%s_%d", a[1], NbCurves);
        DrawTrSurf::Set(name, ct);
        di.AppendElement(name);
        x += length * dx;
        y += length * dy;
      }
      break;

      case circle: {
        bool sense = true;
        if (radius < 0)
        {
          radius = -radius;
          sense  = !sense;
          dx     = -dx;
          dy     = -dy;
        }
        gp_Ax2d ax(gp_Pnt2d(x - radius * dy, y + radius * dx), gp_Dir2d(dy, -dx));
        if (angle < 0)
        {
          angle = -angle;
          sense = !sense;
        }
        occ::handle<Geom2d_Circle>       c  = new Geom2d_Circle(ax, radius, sense);
        occ::handle<Geom2d_TrimmedCurve> ct = new Geom2d_TrimmedCurve(c, 0, angle);
        NbCurves++;
        Sprintf(name, "%s_%d", a[1], NbCurves);
        DrawTrSurf::Set(name, ct);
        di.AppendElement(name);
        gp_Pnt2d p;
        gp_Vec2d v;
        c->D1(angle, p, v);
        x  = p.X();
        y  = p.Y();
        dx = v.X() / radius;
        dy = v.Y() / radius;
      }
      break;

      case none:
        break;
    }

    // update first
    first     = stayfirst;
    stayfirst = false;

    // next segment....
    i++;
    if ((i == n) && close)
    {
      // the closing segment
      dx     = x0 - x;
      dy     = y0 - y;
      length = std::sqrt(dx * dx + dy * dy);
      if (length > Precision::Confusion())
      {
        move = line;
        dx   = dx / length;
        dy   = dy / length;
        goto again;
      }
    }
  }
  const char* aName;
  aName = "CurX";
  Draw::Set(aName, x);
  aName = "CurY";
  Draw::Set(aName, y);
  aName = "CurDX";
  Draw::Set(aName, dx);
  aName = "CurDY";
  Draw::Set(aName, dy);

  return 0;

badargs:
  di << "profile : bad number of arguments";
  return 1;
}

//=================================================================================================

int mkoffset(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 5)
    return 1;
  char name[100];

  BRepOffsetAPI_MakeOffset Paral;

  bool             ToApprox    = false;
  GeomAbs_JoinType theJoinType = GeomAbs_Arc;

  int anIndArg = 6;
  if (n >= 6)
  {
    if (strcmp(a[5], "-approx") == 0)
    {
      ToApprox = true;
      anIndArg++;
    }

    if (n >= anIndArg && strcmp(a[anIndArg - 1], "i") == 0)
      theJoinType = GeomAbs_Intersection;
  }

  TopoDS_Shape Base = DBRep::Get(a[2], TopAbs_FACE);

  if (Base.IsNull())
  {
    Base = DBRep::Get(a[2]);
    if (Base.IsNull())
      return 1;
    Paral.Init(theJoinType);
    TopExp_Explorer exp;
    for (exp.Init(Base, TopAbs_WIRE); exp.More(); exp.Next())
    {
      TopoDS_Wire aLocalShape = TopoDS::Wire(exp.Current());
      Paral.AddWire(aLocalShape);
    }
  }
  else
  {
    Base.Orientation(TopAbs_FORWARD);
    Paral.Init(TopoDS::Face(Base), theJoinType);
  }
  Paral.SetApprox(ToApprox);

  double U, dU;
  int    Nb;
  dU = Draw::Atof(a[4]);
  Nb = Draw::Atoi(a[3]);

  double Alt = 0.;
  if (n > anIndArg)
    Alt = Draw::Atof(a[anIndArg]);

  int Compt = 1;

  for (int i = 1; i <= Nb; i++)
  {
    U = i * dU;
    Paral.Perform(U, Alt);

    if (!Paral.IsDone())
    {
      di << " Error: Offset is not done.\n";
      return 1;
    }
    else
    {
      Sprintf(name, "%s_%d", a[1], Compt++);
      char* temp = name; // portage WNT
      DBRep::Set(temp, Paral.Shape());
    }
  }

  return 0;
}

//=================================================================================================

int openoffset(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 5)
    return 1;
  char name[100];

  BRepOffsetAPI_MakeOffset Paral;

  bool             ToApprox    = false;
  GeomAbs_JoinType theJoinType = GeomAbs_Arc;

  int anIndArg = 6;
  if (n >= 6)
  {
    if (strcmp(a[5], "-approx") == 0)
    {
      ToApprox = true;
      anIndArg++;
    }

    if (n >= anIndArg && strcmp(a[anIndArg - 1], "i") == 0)
      theJoinType = GeomAbs_Intersection;
  }

  TopoDS_Shape Base = DBRep::Get(a[2], TopAbs_FACE);

  if (Base.IsNull())
  {
    Base = DBRep::Get(a[2], TopAbs_WIRE);
    if (Base.IsNull())
      return 1;
    Paral.Init(theJoinType, true);
    Paral.AddWire(TopoDS::Wire(Base));
  }
  else
  {
    Base.Orientation(TopAbs_FORWARD);
    Paral.Init(TopoDS::Face(Base), theJoinType, true);
  }
  Paral.SetApprox(ToApprox);

  double U, dU;
  int    Nb;
  dU = Draw::Atof(a[4]);
  Nb = Draw::Atoi(a[3]);

  double Alt = 0.;

  int Compt = 1;

  for (int i = 1; i <= Nb; i++)
  {
    U = i * dU;
    Paral.Perform(U, Alt);

    if (!Paral.IsDone())
    {
      di << " Error: Offset is not done.\n";
      return 1;
    }
    else
    {
      Sprintf(name, "%s_%d", a[1], Compt++);
      char* temp = name; // portage WNT
      DBRep::Set(temp, Paral.Shape());
    }
  }

  return 0;
}

//=================================================================================================

int pickface(Draw_Interpretor& di, int, const char**)
{
  const char* pick_name = ".";

  TopoDS_Shape S = DBRep::Get(pick_name, TopAbs_FACE);
  if (S.IsNull())
    return 1;

  char* name = new char[100];
  Sprintf(name, "PickedFace %s", pick_name);
  DBRep::Set(name, S);
  di.AppendElement(name);
  return 0;
}

int edgeintersector(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 5)
    return 1;

  TopoDS_Edge  E[2];
  TopoDS_Shape aLocalShape(DBRep::Get(a[2], TopAbs_EDGE));
  E[0] = TopoDS::Edge(aLocalShape);
  //  E[0] = TopoDS::Edge(DBRep::Get(a[2],TopAbs_EDGE));
  if (E[0].IsNull())
    return 1;
  aLocalShape = DBRep::Get(a[3], TopAbs_EDGE);
  E[1]        = TopoDS::Edge(aLocalShape);
  //  E[1] = TopoDS::Edge(DBRep::Get(a[3],TopAbs_EDGE));
  if (E[1].IsNull())
    return 1;
  aLocalShape   = DBRep::Get(a[4], TopAbs_FACE);
  TopoDS_Face F = TopoDS::Face(aLocalShape);
  //  TopoDS_Face F  = TopoDS::Face(DBRep::Get(a[4],TopAbs_FACE));
  if (F.IsNull())
    return 1;

  TopOpeBRep_EdgesIntersector EInter;
  char                        name[100];
  //------------------------------------------------------
  // Calculate point of intersection 2D
  //-----------------------------------------------------
  EInter.SetFaces(F, F);
  double TolInter = 1.e-7;
  if (n == 6)
    TolInter = Draw::Atof(a[5]);
  EInter.ForceTolerances(TolInter, TolInter);
  bool reducesegments = true;
  EInter.Perform(E[0], E[1], reducesegments);

  if (EInter.IsEmpty())
  {
    // std::cout << " No intersection found" << std::endl;
    di << " No intersection found\n";
    return 0;
  }

  BRep_Builder B;

  int              NbV = 0;
  constexpr double Tol = Precision::PConfusion();

  bool rejectreducedsegmentpoints = true;
  EInter.InitPoint(rejectreducedsegmentpoints);
  for (; EInter.MorePoint(); EInter.NextPoint())
  {
    const TopOpeBRep_Point2d& P2D = EInter.Point();
    gp_Pnt                    P   = P2D.Value();
    TopoDS_Vertex             V   = BRepLib_MakeVertex(P);
    NbV++;
    Sprintf(name, "%s_%d", a[1], NbV);
    DBRep::Set(name, V);
    for (int i = 1; i <= 2; i++)
    {
      //---------------------------------------------------------------
      // to be able to rank parameter on edge
      // it is necessary to code it internally
      //---------------------------------------------------------------
      double U = P2D.Parameter(i);

      V.Orientation(TopAbs_INTERNAL);
      B.UpdateVertex(V, U, E[i - 1], Tol);
      //      B.UpdateVertex(TopoDS::Vertex(V.Oriented(TopAbs_INTERNAL)),
      //		     U,E[i-1],Tol);
      //---------------------------------------------------------------
      // Orientation of vertex in the transition.
      //---------------------------------------------------------------
      TopAbs_Orientation OO = TopAbs_REVERSED;
      if (P2D.IsVertex(i))
      {
        OO = P2D.Vertex(i).Orientation();
      }
      else if (P2D.Transition(i).Before() == TopAbs_OUT)
      {
        OO = TopAbs_FORWARD;
      }
      // std::cout << " Orientation of vertex " << NbV << " on " << a[i+1] << ": ";
      di << " Orientation of vertex " << NbV << " on " << a[i + 1] << ": ";
      if (OO == TopAbs_FORWARD)
      {
        // std::cout << "FORWARD" << std::endl;
        di << "FORWARD\n";
      }
      else
      {
        // std::cout << "REVERSED" << std::endl;
        di << "REVERSED\n";
      }
    }
  }
  // POP pour NT
  return 0;
}

//=================================================================================
// function : arclinconvert
// purpose  : Convert a single face to a face with contour made of arcs and segments
//=================================================================================

static int arclinconvert(Draw_Interpretor& /*dout*/, int n, const char** a)
{
  // Check the command arguments
  if (n < 3)
  {
    std::cout << "Error: " << a[0] << " - invalid number of arguments" << std::endl;
    std::cout << "Usage: type help " << a[0] << std::endl;
    return 1; // TCL_ERROR
  }

  // read shape
  const TopoDS_Shape aShape = DBRep::Get(a[2]);
  if (aShape.IsNull())
  {
    std::cout << "Error: " << a[2] << " is null" << std::endl;
    return 1; // TCL_ERROR
  }

  TopAbs_ShapeEnum aType = aShape.ShapeType();
  if (aType != TopAbs_WIRE && aType != TopAbs_FACE)
  {
    std::cout << "Error: " << a[2] << " is neither wire no face" << std::endl;
    return 1; // TCL_ERROR
  }

  // read tolerance
  double aTol = 0.01;
  if (n > 3)
    aTol = Draw::Atof(a[3]);
  std::cout << "Info: tolerance is set to " << aTol << std::endl;

  TopoDS_Shape aResult;

  if (aType == TopAbs_WIRE)
  {
    bool                    OnlyPlane = false;
    BRepBuilderAPI_MakeFace aFaceMaker(TopoDS::Wire(aShape), OnlyPlane);
    if (aFaceMaker.Error() != BRepBuilderAPI_FaceDone)
    {
      std::cout << "Error: failed to find a face for the wire " << a[2] << std::endl;
      return 1; // TCL_ERROR
    }
    const TopoDS_Face& aFace = aFaceMaker.Face();
    TopoDS_Iterator    anIter(aFace);
    TopoDS_Wire        aWire = TopoDS::Wire(anIter.Value());
    aResult                  = BRepAlgo::ConvertWire(aWire, aTol, aFace);
  }
  else if (aType == TopAbs_FACE)
  {
    TopoDS_Face aFace = TopoDS::Face(aShape);
    aResult           = BRepAlgo::ConvertFace(aFace, aTol);
  }

  if (aResult.IsNull())
  {
    std::cout << "Error: could not convert " << a[2] << std::endl;
    return 1; // TCL_ERROR
  }

  DBRep::Set(a[1], aResult);
  return 0; // TCL_OK
}

//=================================================================================================

int concatC0wire(Draw_Interpretor&, int n, const char** c)
{
  if (n < 3)
    return 1;

  TopoDS_Shape S = DBRep::Get(c[2], TopAbs_WIRE);

  if (S.IsNull())
    return 1; // test if the shape is empty

  TopoDS_Wire  W = TopoDS::Wire(S);
  TopoDS_Shape res;

  res = BRepAlgo::ConcatenateWireC0(W); // treatment
  DBRep::Set(c[1], res);
  return 0;
}

//=======================================================================
// function : concatwire
// purpose  : reduce the multiply degree of the knots to the minimum without
//           changing the geometry
//=======================================================================

static int concatwire(Draw_Interpretor&, int n, const char** c)
{
  GeomAbs_Shape Option = GeomAbs_C1;
  if (n < 3)
    return 1;

  if (n == 4) // check if it's C1 or G1
    if (!strcmp(c[3], "G1"))
      Option = GeomAbs_G1;

  TopoDS_Shape S = DBRep::Get(c[2], TopAbs_WIRE);

  if (S.IsNull())
    return 1; // test if the shape is empty

  TopoDS_Wire W = TopoDS::Wire(S);
  TopoDS_Wire res;
  res = BRepAlgo::ConcatenateWire(W, Option); // processing
  DBRep::Set(c[1], res);
  return 0;
}

//=================================================================================================

int build3d(Draw_Interpretor& di, int n, const char** a)
{

  if ((n < 2) || (n > 3))
  {
    // std::cout << " 1 or 2 arguments expected" << std::endl;
    di << " 1 or 2 arguments expected\n";
    return 1;
  }

  bool         Ok;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull())
    return 1;

  if (n == 2)
  {
    Ok = BRepLib::BuildCurves3d(S);
  }
  else
  {
    Ok = BRepLib::BuildCurves3d(S, Draw::Atof(a[2]));
  }
  // if (!Ok) {std::cout << " one of the computation failed" << std::endl;}
  if (!Ok)
  {
    di << " one of the computation failed\n";
  }

  return 0;
}

//=======================================================================
// function : reducepcurves
// purpose  : remove pcurves that are unused in this shape
//=======================================================================

int reducepcurves(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
    return 1;

  int i;
  for (i = 1; i < n; i++)
  {
    TopoDS_Shape aShape = DBRep::Get(a[i]);
    if (aShape.IsNull())
      // std::cout << a[i] << " is not a valid shape" << std::endl;
      di << a[i] << " is not a valid shape\n";
    else
      BRepTools::RemoveUnusedPCurves(aShape);
  }

  return 0;
}

//=================================================================================================

void BRepTest::CurveCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;
  done = true;

  DBRep::BasicCommands(theCommands);
  GeometryTest::CurveCommands(theCommands);

  const char* g = "TOPOLOGY Curve topology commands";

  theCommands.Add("vertex", "vertex name [x y z | p edge | poin]", __FILE__, vertex, g);

  theCommands.Add("etrim", "etrim edge v1 [v2]", __FILE__, trim, g);

  theCommands.Add("range", "range edge [face] first last", __FILE__, range, g);

  theCommands.Add("polyline", "polyline name x1 y1 z1 x2 y2 z2 ...", __FILE__, polyline, g);

  theCommands.Add("polyvertex", "polyvertex name v1 v2 ...", __FILE__, polyvertex, g);

  theCommands.Add("wire", "wire wirename [-unsorted] e1/w1 [e2/w2 ...]", __FILE__, wire, g);

  theCommands.Add("profile", "profile, no args to get help", __FILE__, profile, g);

  theCommands.Add("bsplineprof", "bsplineprof, no args to get help", __FILE__, bsplineprof, g);

  theCommands.Add("2dprofile", "2dprofile, no args to get help", __FILE__, profile2d, g);

  theCommands.Add(
    "mkoffset",
    "mkoffset result face/compound of wires  nboffset stepoffset [-approx] [jointype(a/i) [alt]]",
    __FILE__,
    mkoffset,
    g);

  theCommands.Add("openoffset",
                  "openoffset result face/wire nboffset stepoffset [-approx] [jointype(a/i)]",
                  __FILE__,
                  openoffset,
                  g);

  theCommands.Add("mkedge",
                  "mkedge edge curve [surface] [pfirst plast] [vfirst [pfirst] vlast [plast]] ",
                  __FILE__,
                  mkedge,
                  g);

  theCommands.Add("mkcurve", "mkcurve curve edge", __FILE__, mkcurve, g);

  theCommands.Add(
    "mkoricurve",
    "mkoricurve curve edge: \n  the curve is colored according to the orientation of the edge",
    __FILE__,
    mkcurve,
    g);

  theCommands.Add("mk2dcurve", "mk2dcurve curve edge [face OR index]", __FILE__, mk2dcurve, g);

  theCommands.Add("mkpoint", "mkpoint point vertex", __FILE__, mkpoint, g);

  theCommands.Add("uisoedge", "uisoedge edge face u v1 v2", __FILE__, isoedge, g);

  theCommands.Add("edge", "edge edgename v1 v2", __FILE__, edge, g);

  theCommands.Add("visoedge", "visoedge edge face v u1 u2", __FILE__, isoedge, g);

  theCommands.Add("transfert", "transfert edge1 edge2", __FILE__, transfert, g);

  theCommands.Add("pickface", "pickface", __FILE__, pickface, g);

  theCommands.Add("edgeintersector",
                  "edgeintersector r E1 E2 F [Tol]",
                  __FILE__,
                  edgeintersector,
                  g);

  theCommands.Add("build3d", "build3d S [tol]", __FILE__, build3d, g);

  theCommands.Add("reducepcurves", "reducepcurves shape1 shape2 ...", __FILE__, reducepcurves, g);

  theCommands.Add("arclinconvert",
                  "arclinconvert result wire/face [tol]",
                  __FILE__,
                  arclinconvert,
                  g);

  theCommands.Add("concatC0wire", "concatC0wire result wire", __FILE__, concatC0wire, g);

  theCommands.Add("concatwire", "concatwire result wire [option](G1/C1)", __FILE__, concatwire, g);
}
