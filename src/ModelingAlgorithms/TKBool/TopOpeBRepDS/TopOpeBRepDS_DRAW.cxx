// Created on: 1997-11-26
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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

#ifdef DRAW
  #include <TopOpeBRepTool_DRAW.hxx>
  #include <TopOpeBRepDS_DRAW.hxx>

  #include <DrawTrSurf.hxx>
  #include <DBRep.hxx>
  #include <Geom2d_TrimmedCurve.hxx>
  #include <Geom2d_Curve.hxx>
  #include <Geom2d_Line.hxx>
  #include <Geom_Line.hxx>
  #include <Geom_TrimmedCurve.hxx>
  #include <BRep_TEdge.hxx>
  #include <BRep_CurveRepresentation.hxx>
  #include <NCollection_List.hxx>
  #include <BRep_Tool.hxx>
  #include <GeomAPI_ProjectPointOnSurf.hxx>
  #include <TopoDS.hxx>
  #include <TopExp_Explorer.hxx>
  #include <Precision.hxx>

Standard_EXPORT void FUN_draw(const TopoDS_Shape& s)
{
  char* nnn = TCollection_AsciiString("name").ToCString();
  if (s.IsNull())
    std::cout << "nullshape" << std::endl;
  DBRep::Set(nnn, s);
}

Standard_EXPORT void FUN_draw(const gp_Pnt& p)
{
  char* nnn = TCollection_AsciiString("name").ToCString();
  DrawTrSurf::Set(nnn, p);
}

Standard_EXPORT void FUN_draw(const gp_Pnt2d& p)
{
  char* nnn = TCollection_AsciiString("name").ToCString();
  DrawTrSurf::Set(nnn, p);
}

Standard_EXPORT void FUN_draw(const occ::handle<Geom2d_Curve> c, const double dpar)
{
  char* nnn = TCollection_AsciiString("c2d").ToCString();
  if (dpar <= Precision::Confusion())
  {
    DrawTrSurf::Set(nnn, c);
    return;
  }
  occ::handle<Geom2d_TrimmedCurve> tC = new Geom2d_TrimmedCurve(c, 0., dpar);
  DrawTrSurf::Set(nnn, tC);
}

Standard_EXPORT void FUN_draw(const gp_Pnt& p, const gp_Dir& d)
{
  TCollection_AsciiString aa("dir");
  FUN_tool_draw(aa, p, d);
}

Standard_EXPORT void FUN_brep_draw(const TCollection_AsciiString& aa, const gp_Pnt& p)
{
  FUN_tool_draw(aa, p);
}

Standard_EXPORT void FUN_brep_draw(const TCollection_AsciiString& aa,
                                   const gp_Pnt&                  p,
                                   const gp_Dir&                  d)
{
  FUN_tool_draw(aa, p, d);
}

Standard_EXPORT void FUN_brep_draw(const TCollection_AsciiString& aa, const TopoDS_Shape& s)
{
  FUN_tool_draw(aa, s);
}

Standard_EXPORT void FUN_brep_draw(const TCollection_AsciiString& aa,
                                   const occ::handle<Geom_Curve>& C,
                                   const double&                  f,
                                   const double&                  l)
{
  FUN_tool_draw(aa, C, f, l);
}

Standard_EXPORT void FUN_brep_draw(const TCollection_AsciiString& aa,
                                   const occ::handle<Geom_Curve>& C)
{
  FUN_tool_draw(aa, C);
}

Standard_EXPORT void FUN_DrawMap(
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    DataforDegenEd)
{
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itemap(DataforDegenEd);
  for (; itemap.More(); itemap.Next())
  {
    TopoDS_Shape                             v = itemap.Key();
    NCollection_List<TopoDS_Shape>::Iterator itoflos(itemap.Value());
    if (!itoflos.More())
      continue;
    TopoDS_Shape Ec = itoflos.Value();
    if (!itoflos.More())
      continue;
    itoflos.Next();
    TopoDS_Shape Ed = itoflos.Value();

    bool tr = false;
    if (tr)
    {
      FUN_draw(v);
      FUN_draw(Ec);
      FUN_draw(Ed);
    }
  }
}

static bool FUN_hascurveonsurf(const TopoDS_Edge& edge, const TopoDS_Face& face)
{
  TopLoc_Location           L;
  occ::handle<Geom_Surface> S = BRep_Tool::Surface(face, L);

  occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&edge.TShape());
  const NCollection_List<occ::handle<BRep_CurveRepresentation>>&    lcr = TE->Curves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);
  bool                                                              iscurveonS = false;

  for (; itcr.More(); itcr.Next())
  {
    const occ::handle<BRep_CurveRepresentation>& cr               = itcr.Value();
    bool                                         iscurveonsurface = cr->IsCurveOnSurface();
    if (!iscurveonsurface)
      continue;
    iscurveonS = cr->IsCurveOnSurface(S, L);
    if (iscurveonS)
      break;
  }
  return iscurveonS;
}

Standard_EXPORT void FUN_draw2de(const TopoDS_Shape& ed, const TopoDS_Shape& fa)
{
  char*  nnn = TCollection_AsciiString("name").ToCString();
  double f, l;
  if (ed.IsNull())
    return;
  if (fa.IsNull())
    return;
  TopoDS_Edge edge    = TopoDS::Edge(ed);
  TopoDS_Face face    = TopoDS::Face(fa);
  bool        hascons = FUN_hascurveonsurf(edge, face);
  if (!hascons)
    return;

  TopAbs_Orientation        ori   = edge.Orientation();
  bool                      sense = (ori == TopAbs_FORWARD) ? true : false;
  occ::handle<Geom2d_Curve> C2d   = BRep_Tool::CurveOnSurface(edge, face, f, l);

  occ::handle<Geom2d_TrimmedCurve> tC2d = new Geom2d_TrimmedCurve(C2d, f, l, sense);
  DrawTrSurf::Set(nnn, tC2d);
} // FUN_draw2de

Standard_EXPORT void FUN_draw2d(const double&      par,
                                const TopoDS_Edge& E,
                                const TopoDS_Edge& Eref,
                                const TopoDS_Face& Fref)
{
  TopAbs_Orientation oriE       = E.Orientation();
  TopAbs_Orientation oriEref    = Eref.Orientation();
  bool               ErefonFref = false;
  bool               EonFref    = false;
  TopExp_Explorer    ex;
  int                ne = 0;
  for (ex.Init(Fref, TopAbs_EDGE); ex.More(); ex.Next())
    ne++;
  if (ne < 1)
    return;
  for (ex.Init(Fref, TopAbs_EDGE); ex.More(); ex.Next())
  {
    const TopoDS_Edge ed = TopoDS::Edge(ex.Current());
    if (ed.IsSame(Eref))
    {
      ErefonFref = true;
      break;
    }
    if (ed.IsSame(E))
    {
      EonFref = true;
      break;
    }
  }
  gp_Pnt2d p2d;
  if (ErefonFref || EonFref)
  {
    double                    f, l;
    occ::handle<Geom2d_Curve> C2d;
    if (ErefonFref)
    {
      C2d = BRep_Tool::CurveOnSurface(Eref, Fref, f, l);
      FUN_draw2de(Eref, Fref);
    }
    if (EonFref)
    {
      C2d = BRep_Tool::CurveOnSurface(E, Fref, f, l);
      FUN_draw2de(E, Fref);
    }
    C2d->D0(par, p2d);
  }
  else
  {
    double                  f, l;
    occ::handle<Geom_Curve> C3d = BRep_Tool::Curve(Eref, f, l);
    gp_Pnt                  P;
    C3d->D0(par, P);
    occ::handle<Geom_Surface>  S = BRep_Tool::Surface(Fref);
    GeomAPI_ProjectPointOnSurf PonS(P, S);
    if (!PonS.Extrema().IsDone())
      return;
    if (PonS.NbPoints() == 0)
      return;
    double u, v;
    PonS.Parameters(1, u, v);
    p2d = gp_Pnt2d(u, v);
  }

  FUN_draw(p2d);
} // FUN_draw2d

#endif
