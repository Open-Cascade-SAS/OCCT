// Created on: 1991-07-04
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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

#include <DBRep_DrawableShape.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <DBRep.hxx>
#include <DBRep_Edge.hxx>
#include <DBRep_Face.hxx>
#include <DBRep_HideData.hxx>
#include <DBRep_IsoBuilder.hxx>
#include <NCollection_List.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Trsf.hxx>
#include <Message_ProgressIndicator.hxx>
#include <HLRBRep.hxx>
#include <Poly_Connect.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DBRep_DrawableShape, Draw_Drawable3D)

static double IsoRatio = 1.001;

static int MaxPlotCount = 5; // To avoid huge recursive calls in
static int PlotCount    = 0; // PlotEdge and PlotIso for cases of "bad"
                             // curves and surfaces
                             // Set PlotCount = 0 before first call of
                             // PlotEdge or PlotIso
static TopoDS_Shape pickshape;
static double       upick, vpick;
#ifdef _WIN32
extern Draw_Viewer dout;
#endif

//=================================================================================================

DBRep_DrawableShape::DBRep_DrawableShape(const TopoDS_Shape& aShape,
                                         const Draw_Color&   FreeCol,
                                         const Draw_Color&   ConnCol,
                                         const Draw_Color&   EdgeCol,
                                         const Draw_Color&   IsosCol,
                                         const double        size,
                                         const int           nbisos,
                                         const int           discret)
    : mySize(size),
      myDiscret(discret),
      myFreeCol(FreeCol),
      myConnCol(ConnCol),
      myEdgeCol(EdgeCol),
      myIsosCol(IsosCol),
      myNbIsos(nbisos),
      myDispOr(false),
      mytriangulations(false),
      mypolygons(false),
      myHLR(false),
      myRg1(false),
      myRgN(false),
      myHid(false)
{
  myShape = aShape;
}

//=================================================================================================

void DBRep_DrawableShape::updateDisplayData() const
{
  myFaces.Clear();
  myEdges.Clear();

  if (myShape.IsNull())
    return;

  //==============================================================
  // Process the faces
  //==============================================================

  TopExp_Explorer ExpFace;
  TopLoc_Location l;

  for (ExpFace.Init(myShape, TopAbs_FACE); ExpFace.More(); ExpFace.Next())
  {
    TopoDS_Face TopologicalFace = TopoDS::Face(ExpFace.Current());
    if (myNbIsos != 0)
    {
      const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(TopologicalFace, l);
      if (!S.IsNull())
      {
        TopologicalFace.Orientation(TopAbs_FORWARD);
        DBRep_IsoBuilder IsoBuild(TopologicalFace, mySize, myNbIsos);
        myFaces.Append(new DBRep_Face(TopologicalFace, IsoBuild.NbDomains(), myIsosCol));
        IsoBuild.LoadIsos(myFaces.Last());
      }
      else
        myFaces.Append(new DBRep_Face(TopologicalFace, 0, myEdgeCol));
    }
    else
      myFaces.Append(new DBRep_Face(TopologicalFace, 0, myEdgeCol));
  }

  //==============================================================
  // process a 3D edge
  //==============================================================

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    edgemap;
  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, edgemap);
  int iedge;

  for (iedge = 1; iedge <= edgemap.Extent(); iedge++)
  {

    const TopoDS_Edge& theEdge = TopoDS::Edge(edgemap.FindKey(iedge));

    // skip degenerated edges
    if (BRep_Tool::Degenerated(theEdge))
      continue;

    // compute the number of faces
    int nbf = edgemap(iedge).Extent();

    Draw_Color EdgeColor;

    switch (nbf)
    {

      case 0:
        EdgeColor = myEdgeCol; // isolated edge
        break;

      case 1:
        EdgeColor = myFreeCol; // edge in only one face
        break;

      default:
        EdgeColor = myConnCol; // edge shared by at least two faces
    }

    myEdges.Append(new DBRep_Edge(theEdge, EdgeColor));
  }
}

//=======================================================================
// function : ChangeNbIsos
// purpose  : Changes the number of isoparametric curves in a shape.
//=======================================================================

void DBRep_DrawableShape::ChangeNbIsos(const int NbIsos)
{
  myFaces.Clear();
  myNbIsos = NbIsos;
  TopExp_Explorer ExpFace;
  TopLoc_Location l;

  for (ExpFace.Init(myShape, TopAbs_FACE); ExpFace.More(); ExpFace.Next())
  {
    TopoDS_Face                      TopologicalFace = TopoDS::Face(ExpFace.Current());
    const occ::handle<Geom_Surface>& S               = BRep_Tool::Surface(TopologicalFace, l);
    if (myNbIsos != 0)
    {
      if (!S.IsNull())
      {
        TopologicalFace.Orientation(TopAbs_FORWARD);
        DBRep_IsoBuilder IsoBuild(TopologicalFace, mySize, myNbIsos);
        myFaces.Append(new DBRep_Face(TopologicalFace, IsoBuild.NbDomains(), myIsosCol));
        IsoBuild.LoadIsos(myFaces.Last());
      }
      else
        myFaces.Append(new DBRep_Face(TopologicalFace, 0, myEdgeCol));
    }
    else
      myFaces.Append(new DBRep_Face(TopologicalFace, 0, myEdgeCol));
  }
}

//=======================================================================
// Function : NbIsos
// Purpose  : Returns the number of isoparametric curves in a shape.
//=======================================================================

int DBRep_DrawableShape::NbIsos() const
{
  return myNbIsos;
}

//=================================================================================================

int DBRep_DrawableShape::Discret() const
{
  return myDiscret;
}

Standard_EXPORT Draw_Color DBRep_ColorOrientation(const TopAbs_Orientation Or);

static void PlotIso(Draw_Display&            dis,
                    occ::handle<DBRep_Face>& F,
                    BRepAdaptor_Surface&     S,
                    GeomAbs_IsoType          T,
                    double&                  U,
                    double&                  V,
                    double                   Step,
                    bool&                    halt)
{

  ++PlotCount;

  gp_Pnt Pl, Pr, Pm;

  if (T == GeomAbs_IsoU)
  {
    S.D0(U, V, Pl);
    S.D0(U, V + Step / 2., Pm);
    S.D0(U, V + Step, Pr);
  }
  else
  {
    S.D0(U, V, Pl);
    S.D0(U + Step / 2., V, Pm);
    S.D0(U + Step, V, Pr);
  }

  if (PlotCount > MaxPlotCount)
  {
    dis.DrawTo(Pr);
    if (dis.HasPicked())
    {
      pickshape = F->Face();
      upick     = (T == GeomAbs_IsoU) ? U : U + Step;
      vpick     = (T == GeomAbs_IsoU) ? V + Step : V;
      halt      = true;
    };
    return;
  }

  if (Pm.Distance(Pl) + Pm.Distance(Pr) <= IsoRatio * Pl.Distance(Pr))
  {
    dis.DrawTo(Pr);
    if (dis.HasPicked())
    {
      pickshape = F->Face();
      upick     = (T == GeomAbs_IsoU) ? U : U + Step;
      vpick     = (T == GeomAbs_IsoU) ? V + Step : V;
      halt      = true;
    };
  }
  else if (T == GeomAbs_IsoU)
  {
    PlotIso(dis, F, S, T, U, V, Step / 2, halt);
    double aLocalV = V + Step / 2;
    PlotIso(dis, F, S, T, U, aLocalV, Step / 2, halt);
  }
  else
  {
    PlotIso(dis, F, S, T, U, V, Step / 2, halt);
    double aLocalU = U + Step / 2;
    PlotIso(dis, F, S, T, aLocalU, V, Step / 2, halt);
  }
}

static void PlotEdge(Draw_Display&            dis,
                     occ::handle<DBRep_Edge>& E,
                     const Adaptor3d_Curve&   C,
                     double&                  f,
                     double                   step,
                     bool&                    halt)
{

  ++PlotCount;

  gp_Pnt Pl, Pr, Pm;

  C.D0(f, Pl);
  C.D0(f + step / 2., Pm);
  C.D0(f + step, Pr);

  if (PlotCount > MaxPlotCount)
  {
    dis.DrawTo(Pr);
    if (dis.HasPicked())
    {
      pickshape = E->Edge();
      upick     = f + step;
      vpick     = 0;
      halt      = true;
    }
    return;
  }

  if (Pm.Distance(Pl) + Pm.Distance(Pr) <= IsoRatio * Pl.Distance(Pr))
  {
    dis.DrawTo(Pr);
    if (dis.HasPicked())
    {
      pickshape = E->Edge();
      upick     = f + step;
      vpick     = 0;
      halt      = true;
    };
  }
  else
  {
    PlotEdge(dis, E, C, f, step / 2, halt);
    double aLocalF = f + step / 2;
    PlotEdge(dis, E, C, aLocalF, step / 2, halt);
  }
}

//=================================================================================================

void DBRep_DrawableShape::DrawOn(Draw_Display& dis) const
{
  bool halt = false;

  if (myShape.IsNull())
  {
    dis.SetColor(myConnCol);
    dis.DrawString(gp_Pnt(0, 0, 0), "Null Shape");
    return;
  }

  if (myFaces.IsEmpty() || myEdges.IsEmpty())
    updateDisplayData();

  // hidden lines
  if (myHLR)
  {
    DBRep_DrawableShape* p = (DBRep_DrawableShape*)this;
    p->DisplayHiddenLines(dis);
    return;
  }

  GeomAbs_IsoType T;
  double          Par, T1, T2;
  double          U1, U2, V1, V2, stepU = 0., stepV = 0.;
  //  gp_Pnt P, P1;
  gp_Pnt P;
  int    i, j;

  // Faces
  occ::handle<Poly_Triangulation> Tr;
  TopLoc_Location                 aTempLoc;
  TopLoc_Location                 loc;

  NCollection_List<occ::handle<DBRep_Face>>::Iterator itf(myFaces);

  while (itf.More() && !halt)
  {

    const occ::handle<DBRep_Face>& F = itf.Value();
    dis.SetColor(F->Color());

    occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(F->Face(), aTempLoc);

    if (!aSurf.IsNull())
    {

      bool restriction = false;
      if (aSurf->IsUPeriodic() || aSurf->IsVPeriodic())
      {
        double SU1 = 0., SU2 = 0., SV1 = 0., SV2 = 0.;
        double FU1 = 0., FU2 = 0., FV1 = 0., FV2 = 0.;
        aSurf->Bounds(SU1, SU2, SV1, SV2);
        BRepTools::UVBounds(F->Face(), FU1, FU2, FV1, FV2);
        if (aSurf->IsUPeriodic())
        {
          if (FU1 < SU1 || FU1 > SU2)
            restriction = true;
          if (!restriction && (FU2 < SU1 || FU2 > SU2))
            restriction = true;
        }
        if (!restriction && aSurf->IsVPeriodic())
        {
          if (FV1 < SV1 || FV1 > SV2)
            restriction = true;
          if (!restriction && (FV2 < SV1 || FV2 > SV2))
            restriction = true;
        }
        bool zeroS = (fabs(SU2 - SU1) <= 1.e-9 || fabs(SV2 - SV1) <= 1.e-9);
        bool zeroF = (fabs(FU2 - FU1) <= 1.e-9 || fabs(FV2 - FV1) <= 1.e-9);
        if (restriction && (zeroS || zeroF))
          restriction = false;
        if (restriction && (FU1 >= FU2 || FV1 >= FV2))
          restriction = false;
        if (restriction && (fabs(FU2 - FU1) > 4.1e+100 || fabs(FV2 - FV1) > 4.1e+100))
          restriction = false;
      }

      BRepAdaptor_Surface S(F->Face(), restriction);

      // BRepAdaptor_Surface S(F->Face(),false);

      GeomAbs_SurfaceType SurfType = S.GetType();

// If the type of the surface is GeomAbs_SurfaceOfExtrusion or GeomAbs_SurfaceOfRevolution
#ifdef OCCT_DEBUG
      GeomAbs_CurveType CurvType;
#else
      GeomAbs_CurveType CurvType = GeomAbs_OtherCurve;
#endif

      int N = F->NbIsos();

      int                        Intrv, nbIntv;
      int                        nbUIntv = S.NbUIntervals(GeomAbs_CN);
      int                        nbVIntv = S.NbVIntervals(GeomAbs_CN);
      NCollection_Array1<double> TI(1, std::max(nbUIntv, nbVIntv) + 1);

      for (i = 1; i <= N; i++)
      {

        F->GetIso(i, T, Par, T1, T2);
        if (T == GeomAbs_IsoU)
        {
          S.VIntervals(TI, GeomAbs_CN);
          V1     = std::max(T1, TI(1));
          V2     = std::min(T2, TI(2));
          U1     = Par;
          U2     = Par;
          stepU  = 0;
          nbIntv = nbVIntv;
        }
        else
        {
          S.UIntervals(TI, GeomAbs_CN);
          U1     = std::max(T1, TI(1));
          U2     = std::min(T2, TI(2));
          V1     = Par;
          V2     = Par;
          stepV  = 0;
          nbIntv = nbUIntv;
        }

        S.D0(U1, V1, P);
        dis.MoveTo(P);

        for (Intrv = 1; Intrv <= nbIntv; Intrv++)
        {

          if (TI(Intrv) <= T1 && TI(Intrv + 1) <= T1)
            continue;
          if (TI(Intrv) >= T2 && TI(Intrv + 1) >= T2)
            continue;
          if (T == GeomAbs_IsoU)
          {
            V1    = std::max(T1, TI(Intrv));
            V2    = std::min(T2, TI(Intrv + 1));
            stepV = (V2 - V1) / myDiscret;
          }
          else
          {
            U1    = std::max(T1, TI(Intrv));
            U2    = std::min(T2, TI(Intrv + 1));
            stepU = (U2 - U1) / myDiscret;
          }

          switch (SurfType)
          {
              //-------------GeomAbs_Plane---------------
            case GeomAbs_Plane:
              break;
              //----GeomAbs_Cylinder   GeomAbs_Cone------
            case GeomAbs_Cylinder:
            case GeomAbs_Cone:
              if (T == GeomAbs_IsoV)
              {
                for (j = 1; j < myDiscret; j++)
                {
                  U1 += stepU;
                  V1 += stepV;
                  S.D0(U1, V1, P);
                  dis.DrawTo(P);
                  if (dis.HasPicked())
                  {
                    pickshape = F->Face();
                    upick     = U1;
                    vpick     = V1;
                    halt      = true;
                  }
                }
              }
              break;
              //---GeomAbs_Sphere   GeomAbs_Torus--------
              // GeomAbs_BezierSurface GeomAbs_BezierSurface
            case GeomAbs_Sphere:
            case GeomAbs_Torus:
            case GeomAbs_OffsetSurface:
            case GeomAbs_OtherSurface:
              for (j = 1; j < myDiscret; j++)
              {
                U1 += stepU;
                V1 += stepV;
                S.D0(U1, V1, P);
                dis.DrawTo(P);
                if (dis.HasPicked())
                {
                  pickshape = F->Face();
                  upick     = U1;
                  vpick     = V1;
                  halt      = true;
                }
              }
              break;
              //-------------GeomAbs_BSplineSurface------
            case GeomAbs_BezierSurface:
            case GeomAbs_BSplineSurface:
              for (j = 1; j <= myDiscret / 2; j++)
              {
                occ::handle<DBRep_Face> aLocalFace = F;

                PlotCount = 0;

                PlotIso(dis,
                        aLocalFace,
                        S,
                        T,
                        U1,
                        V1,
                        (T == GeomAbs_IsoV) ? stepU * 2. : stepV * 2.,
                        halt);
                U1 += stepU * 2.;
                V1 += stepV * 2.;
              }
              break;
              //-------------GeomAbs_SurfaceOfExtrusion--
              //-------------GeomAbs_SurfaceOfRevolution-
            case GeomAbs_SurfaceOfExtrusion:
            case GeomAbs_SurfaceOfRevolution:
              if ((T == GeomAbs_IsoV && SurfType == GeomAbs_SurfaceOfRevolution)
                  || (T == GeomAbs_IsoU && SurfType == GeomAbs_SurfaceOfExtrusion))
              {
                if (SurfType == GeomAbs_SurfaceOfExtrusion)
                  break;
                for (j = 1; j < myDiscret; j++)
                {
                  U1 += stepU;
                  V1 += stepV;
                  S.D0(U1, V1, P);
                  dis.DrawTo(P);
                  if (dis.HasPicked())
                  {
                    pickshape = F->Face();
                    upick     = U1;
                    vpick     = V1;
                    halt      = true;
                  }
                }
              }
              else
              {
                CurvType = (S.BasisCurve())->GetType();
                switch (CurvType)
                {
                  case GeomAbs_Line:
                    break;
                  case GeomAbs_Circle:
                  case GeomAbs_Ellipse:
                    for (j = 1; j < myDiscret; j++)
                    {
                      U1 += stepU;
                      V1 += stepV;
                      S.D0(U1, V1, P);
                      dis.DrawTo(P);
                      if (dis.HasPicked())
                      {
                        pickshape = F->Face();
                        upick     = U1;
                        vpick     = V1;
                        halt      = true;
                      }
                    }
                    break;
                  case GeomAbs_Parabola:
                  case GeomAbs_Hyperbola:
                  case GeomAbs_BezierCurve:
                  case GeomAbs_BSplineCurve:
                  case GeomAbs_OffsetCurve:
                  case GeomAbs_OtherCurve:
                    for (j = 1; j <= myDiscret / 2; j++)
                    {
                      occ::handle<DBRep_Face> aLocalFace = F;

                      PlotCount = 0;

                      PlotIso(dis,
                              aLocalFace,
                              S,
                              T,
                              U1,
                              V1,
                              (T == GeomAbs_IsoV) ? stepU * 2. : stepV * 2.,
                              halt);
                      U1 += stepU * 2.;
                      V1 += stepV * 2.;
                    }
                    break;
                }
              }
          }
        }
        S.D0(U2, V2, P);
        dis.DrawTo(P);
        if (dis.HasPicked())
        {
          pickshape = F->Face();
          upick     = U2;
          vpick     = V2;
          halt      = true;
        }
      }
    }

    //=====================================
    // trace des eventuelles triangulations.
    //=====================================

    if (aSurf.IsNull() || mytriangulations)
    {
      Tr = BRep_Tool::Triangulation(F->Face(), loc);
      if (!Tr.IsNull())
      {
        display(Tr, loc.Transformation(), dis);
      }
    }
    itf.Next();
  }

  // Edges
  NCollection_List<occ::handle<DBRep_Edge>>::Iterator ite(myEdges);
  while (ite.More() && !halt)
  {

    const occ::handle<DBRep_Edge>& E = ite.Value();

    if (myDispOr)
      dis.SetColor(DBRep_ColorOrientation(E->Edge().Orientation()));
    else
      dis.SetColor(E->Color());

    // display geometrical curve if exists.
    bool   isgeom = BRep_Tool::IsGeometric(E->Edge());
    double aCheckU1, aCheckU2;

    if (isgeom)
    {
      // check the range (to report bad edges)
      BRep_Tool::Range(E->Edge(), aCheckU1, aCheckU2);
      if (aCheckU2 < aCheckU1)
      {
        // bad orientation
        std::cout << "DBRep_DrawableShape : Bad parameters on edge." << std::endl;
        BRepTools::Dump(E->Edge(), std::cout);
        ite.Next();
        continue;
      }

      if (BRep_Tool::Degenerated(E->Edge()))
      {
        ite.Next();
        continue;
      }

      BRepAdaptor_Curve C(E->Edge());

      double f = C.FirstParameter();
      double l = C.LastParameter();
      if (Precision::IsNegativeInfinite(f))
      {
        if (Precision::IsPositiveInfinite(l))
        {
          f = -mySize;
          l = mySize;
        }
        else
        {
          f = l - mySize;
        }
      }
      else if (Precision::IsPositiveInfinite(l))
      {
        l = f + mySize;
      }

      occ::handle<Adaptor3d_Curve> HC       = C.Trim(f, l, Precision::Confusion());
      GeomAbs_CurveType            CurvType = HC->GetType();

      int                        intrv, nbintv = HC->NbIntervals(GeomAbs_CN);
      NCollection_Array1<double> TI(1, nbintv + 1);
      HC->Intervals(TI, GeomAbs_CN);

      HC->D0(HC->FirstParameter(), P);
      dis.MoveTo(P);

      for (intrv = 1; intrv <= nbintv; intrv++)
      {
        double t    = TI(intrv);
        double step = (TI(intrv + 1) - t) / myDiscret;

        switch (CurvType)
        {
          case GeomAbs_Line:
            break;
          case GeomAbs_Circle:
          case GeomAbs_Ellipse:
            for (j = 1; j < myDiscret; j++)
            {
              t += step;
              C.D0(t, P);
              dis.DrawTo(P);
              if (dis.HasPicked())
              {
                pickshape = E->Edge();
                upick     = t;
                vpick     = 0;
                halt      = true;
              }
            }
            break;
          case GeomAbs_Parabola:
          case GeomAbs_Hyperbola:
          case GeomAbs_BezierCurve:
          case GeomAbs_BSplineCurve:
          case GeomAbs_OffsetCurve:
          case GeomAbs_OtherCurve:
            for (j = 1; j <= myDiscret / 2; j++)
            {
              occ::handle<DBRep_Edge> aLocaLEdge(E);
              PlotCount = 0;
              PlotEdge(dis, aLocaLEdge, *HC, t, step * 2., halt);
              t += step * 2.;
            }
            break;
        }
      }

      C.D0(HC->LastParameter(), P);
      dis.DrawTo(P);
      if (dis.HasPicked())
      {
        pickshape = E->Edge();
        upick     = l;
        vpick     = 0;
        halt      = true;
      }

      if (myDispOr)
      {
        // display an arrow at the end
        gp_Pnt aPnt;
        gp_Vec V;
        C.D1(l, aPnt, V);
        gp_Pnt2d p1, p2;
        dis.Project(aPnt, p1);
        aPnt.Translate(V);
        dis.Project(aPnt, p2);
        gp_Vec2d v(p1, p2);
        if (v.Magnitude() > gp::Resolution())
        {
          double   L = 20 / dis.Zoom();
          double   H = 10 / dis.Zoom();
          gp_Dir2d d(v);
          p2.SetCoord(p1.X() - L * d.X() - H * d.Y(), p1.Y() - L * d.Y() + H * d.X());
          dis.MoveTo(p2);
          p2.SetCoord(p1.X() - L * d.X() + H * d.Y(), p1.Y() - L * d.Y() - H * d.X());
          dis.DrawTo(p1);
          dis.DrawTo(p2);
        }

        //	gp_Vec tang;
        //	C.D1(l,P,tang);
      }
    }

    //======================================
    // trace des representations polygonales:
    //======================================

    if (!isgeom || mypolygons)
    {

      // Polygones 3d:
      occ::handle<Poly_Polygon3D> Polyg = BRep_Tool::Polygon3D(E->Edge(), loc);
      if (!Polyg.IsNull())
      {
        const NCollection_Array1<gp_Pnt>& Points = Polyg->Nodes();
        int                               po;
        for (po = Points.Lower() + 1; po <= Points.Upper(); po++)
        {
          dis.Draw((Points.Value(po - 1)).Transformed(loc), (Points.Value(po)).Transformed(loc));
          if (dis.HasPicked())
          {
            pickshape = E->Edge();
            upick     = 0;
            vpick     = 0;
            halt      = true;
          }
        }
      }
      else
      {

        // Polygone sur triangulation:
        occ::handle<Poly_Triangulation>          PolyTr;
        occ::handle<Poly_PolygonOnTriangulation> Poly;
        BRep_Tool::PolygonOnTriangulation(E->Edge(), Poly, PolyTr, loc);
        if (!Poly.IsNull())
        {
          const NCollection_Array1<int>& Indices = Poly->Nodes();
          for (i = Indices.Lower() + 1; i <= Indices.Upper(); i++)
          {
            dis.Draw(PolyTr->Node(Indices(i - 1)).Transformed(loc),
                     PolyTr->Node(Indices(i)).Transformed(loc));
            if (dis.HasPicked())
            {
              pickshape = E->Edge();
              upick     = 0;
              vpick     = 0;
              halt      = true;
            }
          }
        }
      }
    }

    ite.Next();
  }

  // Vertices

  TopExp_Explorer exv;
  dis.SetColor(myConnCol);

  for (exv.Init(myShape, TopAbs_VERTEX, TopAbs_EDGE); exv.More() && !halt; exv.Next())
  {

    if (myDispOr)
      dis.SetColor(DBRep_ColorOrientation(exv.Current().Orientation()));
    dis.DrawMarker(BRep_Tool::Pnt(TopoDS::Vertex(exv.Current())), Draw_Losange);
    if (dis.HasPicked())
    {
      pickshape = exv.Current();
      upick     = 0;
      vpick     = 0;
      halt      = true;
    }
  }
}

//=================================================================================================

void DBRep_DrawableShape::DisplayHiddenLines(Draw_Display& dis)
{
  int id = dis.ViewId();

  // get the projection
  gp_Trsf T;
  dout.GetTrsf(id, T);
  double focal = -1;
  if (!strcmp(dout.GetType(id), "PERS"))
    focal = dout.Focal(id);
  double Ang, Def;
  HLRBRep::PolyHLRAngleAndDeflection(myAng, Ang, Def);
  IMeshTools_Parameters aMeshParams;
  aMeshParams.Relative   = true;
  aMeshParams.Deflection = Def;
  aMeshParams.Angle      = Ang;

  BRepMesh_IncrementalMesh MESH(myShape, aMeshParams);
  bool                     recompute = true;
  // find if the view must be recomputed
  NCollection_List<DBRep_HideData>::Iterator it(myHidData);

  while (it.More())
  {
    if (it.Value().ViewId() == id)
    {
      // we have the view
      // but did we rotate it
      double ang = it.Value().Angle();
      recompute  = !it.Value().IsSame(T, focal) || myAng != ang;
      if (recompute)
        myHidData.Remove(it);
      else
      {
        it.ChangeValue().DrawOn(dis, myRg1, myRgN, myHid, myConnCol, myIsosCol);
        if (dis.HasPicked())
        {
          pickshape = it.Value().LastPick();
          upick     = 0;
          vpick     = 0;
        }
      }
      break;
    }
    it.Next();
  }
  // recompute the hidden lines and display them
  if (recompute)
  {
    DBRep_HideData theData;
    myHidData.Append(theData);
    myHidData.Last().Set(id, T, focal, myShape, myAng);
    myHidData.Last().DrawOn(dis, myRg1, myRgN, myHid, myConnCol, myIsosCol);
    if (dis.HasPicked())
    {
      pickshape = myHidData.Last().LastPick();
      upick     = 0;
      vpick     = 0;
    }
  }
}

//=================================================================================================

TopoDS_Shape DBRep_DrawableShape::Shape() const
{
  return myShape;
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DBRep_DrawableShape::Copy() const
{
  occ::handle<DBRep_DrawableShape> D = new DBRep_DrawableShape(myShape,
                                                               myFreeCol,
                                                               myConnCol,
                                                               myEdgeCol,
                                                               myIsosCol,
                                                               mySize,
                                                               myNbIsos,
                                                               myDiscret);
  return D;
}

//=================================================================================================

void DBRep_DrawableShape::DisplayOrientation(const bool D)
{
  myDispOr = D;
}

//=================================================================================================

void DBRep_DrawableShape::DisplayTriangulation(const bool D)
{
  mytriangulations = D;
}

//=================================================================================================

void DBRep_DrawableShape::DisplayPolygons(const bool D)
{
  mypolygons = D;
}

//=================================================================================================

void DBRep_DrawableShape::DisplayHLR(const bool   withHLR,
                                     const bool   withRg1,
                                     const bool   withRgN,
                                     const bool   withHid,
                                     const double ang)
{
  myHLR = withHLR;
  myRg1 = withRg1;
  myRgN = withRgN;
  myHid = withHid;
  myAng = ang;
}

//=================================================================================================

bool DBRep_DrawableShape::DisplayTriangulation() const
{
  return mytriangulations;
}

//=================================================================================================

bool DBRep_DrawableShape::DisplayPolygons() const
{
  return mypolygons;
}

//=================================================================================================

void DBRep_DrawableShape::GetDisplayHLR(bool&   withHLR,
                                        bool&   withRg1,
                                        bool&   withRgN,
                                        bool&   withHid,
                                        double& ang) const
{
  withHLR = myHLR;
  withRg1 = myRg1;
  withRgN = myRgN;
  withHid = myHid;
  ang     = myAng;
}

//=================================================================================================

void DBRep_DrawableShape::Dump(Standard_OStream& S) const
{
  BRepTools::Dump(myShape, S);
}

//=================================================================================================

void DBRep_DrawableShape::Save(Standard_OStream& theStream) const
{
  BRep_Builder       aBuilder;
  BRepTools_ShapeSet aShapeSet(aBuilder);
  aShapeSet.Add(myShape);
  occ::handle<Draw_ProgressIndicator> aProgress = Draw::GetProgressBar();
  aShapeSet.Write(theStream, Message_ProgressIndicator::Start(aProgress));
  if (aProgress.IsNull() || !aProgress->UserBreak())
  {
    aShapeSet.Write(myShape, theStream);
  }
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DBRep_DrawableShape::Restore(Standard_IStream& theStream)
{
  const DBRep_Params&                 aParams = DBRep::Parameters();
  BRep_Builder                        aBuilder;
  BRepTools_ShapeSet                  aShapeSet(aBuilder);
  occ::handle<Draw_ProgressIndicator> aProgress = Draw::GetProgressBar();
  aShapeSet.Read(theStream, Message_ProgressIndicator::Start(aProgress));
  if (!aProgress.IsNull() && aProgress->UserBreak())
  {
    return occ::handle<Draw_Drawable3D>();
  }

  TopoDS_Shape aTopoShape;
  aShapeSet.Read(aTopoShape, theStream);
  occ::handle<DBRep_DrawableShape> aDrawShape = new DBRep_DrawableShape(aTopoShape,
                                                                        Draw_vert,
                                                                        Draw_jaune,
                                                                        Draw_rouge,
                                                                        Draw_bleu,
                                                                        aParams.Size,
                                                                        aParams.NbIsos,
                                                                        aParams.Discretization);
  aDrawShape->DisplayTriangulation(aParams.DispTriangles);
  aDrawShape->DisplayPolygons(aParams.DisplayPolygons);
  aDrawShape->DisplayHLR(aParams.WithHLR,
                         aParams.WithRg1,
                         aParams.WithRgN,
                         aParams.WithHid,
                         aParams.HLRAngle);
  return aDrawShape;
}

//=================================================================================================

void DBRep_DrawableShape::Whatis(Draw_Interpretor& s) const
{
  if (myShape.IsNull())
  {
    return;
  }

  s << "shape " << TopAbs::ShapeTypeToString(myShape.ShapeType()) << " "
    << TopAbs::ShapeOrientationToString(myShape.Orientation());

  if (myShape.Free())
    s << " Free";
  if (myShape.Modified())
    s << " Modified";
  if (myShape.Orientable())
    s << " Orientable";
  if (myShape.Closed())
    s << " Closed";
  if (myShape.Infinite())
    s << " Infinite";
  if (myShape.Convex())
    s << " Convex";
}

//=================================================================================================

void DBRep_DrawableShape::LastPick(TopoDS_Shape& s, double& u, double& v)
{
  s = pickshape;
  u = upick;
  v = vpick;
}

//=================================================================================================

void DBRep_DrawableShape::display(const occ::handle<Poly_Triangulation>& T,
                                  const gp_Trsf&                         tr,
                                  Draw_Display&                          dis) const
{
  // Build the connect tool
  Poly_Connect pc(T);

  int i, j, nFree, nbTriangles = T->NbTriangles();
  int t[3];

  // count the free edges
  nFree = 0;
  for (i = 1; i <= nbTriangles; i++)
  {
    pc.Triangles(i, t[0], t[1], t[2]);
    for (j = 0; j < 3; j++)
      if (t[j] == 0)
        nFree++;
  }

  // allocate the arrays
  NCollection_Array1<int>                   Free(1, std::max(1, 2 * nFree));
  NCollection_Vector<NCollection_Vec2<int>> anInternal;

  int fr = 1;

  int n[3];
  for (i = 1; i <= nbTriangles; i++)
  {
    pc.Triangles(i, t[0], t[1], t[2]);
    T->Triangle(i).Get(n[0], n[1], n[2]);
    for (j = 0; j < 3; j++)
    {
      int k = (j + 1) % 3;
      if (t[j] == 0)
      {
        Free(fr)     = n[j];
        Free(fr + 1) = n[k];
        fr += 2;
      }
      // internal edge if this triangle has a lower index than the adjacent
      else if (i < t[j])
      {
        anInternal.Append(NCollection_Vec2<int>(n[j], n[k]));
      }
    }
  }

  // Display the edges

  // free edges
  int nn;
  dis.SetColor(Draw_rouge);
  nn = Free.Length() / 2;
  for (i = 1; i <= nn; i++)
  {
    dis.Draw(T->Node(Free[2 * i - 1]).Transformed(tr), T->Node(Free[2 * i]).Transformed(tr));
  }

  // internal edges

  dis.SetColor(Draw_bleu);
  for (NCollection_Vector<NCollection_Vec2<int>>::Iterator anInterIter(anInternal);
       anInterIter.More();
       anInterIter.Next())
  {
    const int n1 = anInterIter.Value()[0];
    const int n2 = anInterIter.Value()[1];
    dis.Draw(T->Node(n1).Transformed(tr), T->Node(n2).Transformed(tr));
  }
}

//=================================================================================================

bool DBRep_DrawableShape::addMeshNormals(NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>& theNormals,
                                         const TopoDS_Face&                             theFace,
                                         const double                                   theLength)
{
  TopLoc_Location                        aLoc;
  const occ::handle<Poly_Triangulation>& aTriangulation = BRep_Tool::Triangulation(theFace, aLoc);
  const bool                             hasNormals     = aTriangulation->HasNormals();
  if (aTriangulation.IsNull() || (!hasNormals && !aTriangulation->HasUVNodes()))
  {
    return false;
  }

  BRepAdaptor_Surface aSurface(theFace);
  for (int aNodeIter = 1; aNodeIter <= aTriangulation->NbNodes(); ++aNodeIter)
  {
    gp_Pnt aP1 = aTriangulation->Node(aNodeIter);
    if (!aLoc.IsIdentity())
    {
      aP1.Transform(aLoc.Transformation());
    }

    gp_Vec aNormal;
    if (hasNormals)
    {
      aNormal = aTriangulation->Normal(aNodeIter);
    }
    else
    {
      const gp_Pnt2d& aUVNode = aTriangulation->UVNode(aNodeIter);
      gp_Pnt          aDummyPnt;
      gp_Vec          aV1, aV2;
      aSurface.D1(aUVNode.X(), aUVNode.Y(), aDummyPnt, aV1, aV2);
      aNormal = aV1.Crossed(aV2);
    }

    const double aNormalLen = aNormal.Magnitude();
    if (aNormalLen > 1.e-10)
    {
      aNormal.Multiply(theLength / aNormalLen);
    }
    else
    {
      aNormal.SetCoord(aNormalLen / 2.0, 0.0, 0.0);
      std::cout << "Null normal at node X = " << aP1.X() << ", Y = " << aP1.Y()
                << ", Z = " << aP1.Z() << "\n";
    }

    const gp_Pnt aP2 = aP1.Translated(aNormal);
    theNormals.Append(std::pair<gp_Pnt, gp_Pnt>(aP1, aP2));
  }
  return true;
}

//=================================================================================================

void DBRep_DrawableShape::addMeshNormals(
  NCollection_DataMap<TopoDS_Face, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>>& theNormals,
  const TopoDS_Shape&                                                              theShape,
  const double                                                                     theLength)
{
  TopLoc_Location aLoc;
  for (TopExp_Explorer aFaceIt(theShape, TopAbs_FACE); aFaceIt.More(); aFaceIt.Next())
  {
    const TopoDS_Face&                             aFace        = TopoDS::Face(aFaceIt.Current());
    NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>* aFaceNormals = theNormals.ChangeSeek(aFace);
    if (aFaceNormals == NULL)
    {
      aFaceNormals = theNormals.Bound(aFace, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>());
    }

    addMeshNormals(*aFaceNormals, aFace, theLength);
  }
}

//=================================================================================================

bool DBRep_DrawableShape::addSurfaceNormals(
  NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>& theNormals,
  const TopoDS_Face&                             theFace,
  const double                                   theLength,
  const int                                      theNbAlongU,
  const int                                      theNbAlongV)
{
  {
    TopLoc_Location                  aLoc;
    const occ::handle<Geom_Surface>& aSurface = BRep_Tool::Surface(theFace, aLoc);
    if (aSurface.IsNull())
    {
      return false;
    }
  }

  double aUmin = 0.0, aVmin = 0.0, aUmax = 0.0, aVmax = 0.0;
  BRepTools::UVBounds(theFace, aUmin, aUmax, aVmin, aVmax);
  const bool   isUseMidU = (theNbAlongU == 1);
  const bool   isUseMidV = (theNbAlongV == 1);
  const double aDU       = (aUmax - aUmin) / (isUseMidU ? 2 : (theNbAlongU - 1));
  const double aDV       = (aVmax - aVmin) / (isUseMidV ? 2 : (theNbAlongV - 1));

  BRepAdaptor_Surface aSurface(theFace);
  for (int aUIter = 0; aUIter < theNbAlongU; ++aUIter)
  {
    const double aU = aUmin + (isUseMidU ? 1 : aUIter) * aDU;
    for (int aVIter = 0; aVIter < theNbAlongV; ++aVIter)
    {
      const double aV = aVmin + (isUseMidV ? 1 : aVIter) * aDV;

      gp_Pnt aP1;
      gp_Vec aV1, aV2;
      aSurface.D1(aU, aV, aP1, aV1, aV2);

      gp_Vec aVec       = aV1.Crossed(aV2);
      double aNormalLen = aVec.Magnitude();
      if (aNormalLen > 1.e-10)
      {
        aVec.Multiply(theLength / aNormalLen);
      }
      else
      {
        aVec.SetCoord(aNormalLen / 2.0, 0.0, 0.0);
        std::cout << "Null normal at U = " << aU << ", V = " << aV << "\n";
      }

      const gp_Pnt aP2 = aP1.Translated(aVec);
      theNormals.Append(std::pair<gp_Pnt, gp_Pnt>(aP1, aP2));
    }
  }
  return true;
}

//=================================================================================================

void DBRep_DrawableShape::addSurfaceNormals(
  NCollection_DataMap<TopoDS_Face, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>>& theNormals,
  const TopoDS_Shape&                                                              theShape,
  const double                                                                     theLength,
  const int                                                                        theNbAlongU,
  const int                                                                        theNbAlongV)
{
  for (TopExp_Explorer aFaceIt(theShape, TopAbs_FACE); aFaceIt.More(); aFaceIt.Next())
  {
    const TopoDS_Face&                             aFace        = TopoDS::Face(aFaceIt.Current());
    NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>* aFaceNormals = theNormals.ChangeSeek(aFace);
    if (aFaceNormals == NULL)
    {
      aFaceNormals = theNormals.Bound(aFace, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>());
    }
    addSurfaceNormals(*aFaceNormals, aFace, theLength, theNbAlongU, theNbAlongV);
  }
}
