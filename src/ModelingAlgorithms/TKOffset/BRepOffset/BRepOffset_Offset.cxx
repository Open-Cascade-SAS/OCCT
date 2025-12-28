// Created on: 1995-10-19
// Created by: Bruno DUMORTIER
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

#include <BRepOffset_Offset.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepOffset.hxx>
#include <BRepOffset_Tool.hxx>
#include <BRepTools.hxx>
#include <ElSLib.hxx>
#include <gce_MakePln.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <GeomFill_Pipe.hxx>
#include <GeomLib.hxx>
#include <GeomProjLib.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Torus.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <ShapeFix_Shape.hxx>
#include <Standard_ConstructionError.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Sequence.hxx>

#ifdef OCCT_DEBUG
static bool Affich   = false;
static int  NbOFFSET = 0;
#endif
#ifdef DRAW
  #include <DrawTrSurf.hxx>
  #include <DBRep.hxx>
#endif
#include <cstdio>
#include <Geom_BSplineSurface.hxx>

static gp_Pnt GetFarestCorner(const TopoDS_Wire& aWire)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Vertices;
  TopExp::MapShapes(aWire, TopAbs_VERTEX, Vertices);

  double MaxDist = 0.;
  gp_Pnt thePoint;
  for (int i = 1; i <= Vertices.Extent(); i++)
    for (int j = 1; j <= Vertices.Extent(); j++)
    {
      const TopoDS_Vertex& V1    = TopoDS::Vertex(Vertices(i));
      const TopoDS_Vertex& V2    = TopoDS::Vertex(Vertices(j));
      gp_Pnt               P1    = BRep_Tool::Pnt(V1);
      gp_Pnt               P2    = BRep_Tool::Pnt(V2);
      double               aDist = P1.SquareDistance(P2);
      if (aDist > MaxDist)
      {
        MaxDist  = aDist;
        thePoint = P1;
      }
    }

  return thePoint;
}

//=================================================================================================

static void UpdateEdge(const TopoDS_Edge&             E,
                       const occ::handle<Geom_Curve>& C,
                       const TopLoc_Location&         L,
                       const double                   Tol)
{
  // Cut curves to avoid copies in the extensions.
  BRep_Builder                   B;
  occ::handle<Geom_TrimmedCurve> BC = occ::down_cast<Geom_TrimmedCurve>(C);
  if (!BC.IsNull())
  {
    B.UpdateEdge(E, BC->BasisCurve(), L, Tol);
  }
  else
  {
    B.UpdateEdge(E, C, L, Tol);
  }
}

//=================================================================================================

static void UpdateEdge(const TopoDS_Edge&               E,
                       const occ::handle<Geom2d_Curve>& C,
                       const TopoDS_Face&               F,
                       const double                     Tol)
{
  // Cut curves to avoid copies in the extensions.
  BRep_Builder                     B;
  occ::handle<Geom2d_TrimmedCurve> BC = occ::down_cast<Geom2d_TrimmedCurve>(C);
  if (!BC.IsNull())
  {
    B.UpdateEdge(E, BC->BasisCurve(), F, Tol);
  }
  else
  {
    B.UpdateEdge(E, C, F, Tol);
  }
}

//=================================================================================================

static void UpdateEdge(const TopoDS_Edge&               E,
                       const occ::handle<Geom2d_Curve>& C1,
                       const occ::handle<Geom2d_Curve>& C2,
                       const TopoDS_Face&               F,
                       const double                     Tol)
{
  // Cut curves to avoid copies in the extensions.
  BRep_Builder                     B;
  occ::handle<Geom2d_Curve>        NC1, NC2;
  occ::handle<Geom2d_TrimmedCurve> BC1 = occ::down_cast<Geom2d_TrimmedCurve>(C1);
  occ::handle<Geom2d_TrimmedCurve> BC2 = occ::down_cast<Geom2d_TrimmedCurve>(C2);
  if (!BC1.IsNull())
    NC1 = BC1->BasisCurve();
  else
    NC1 = C1;
  if (!BC2.IsNull())
    NC2 = BC2->BasisCurve();
  else
    NC2 = C2;
  B.UpdateEdge(E, NC1, NC2, F, Tol);
}

//=======================================================================
// function : ComputeCurve3d
// purpose  : Particular case of Curve On Surface.
//=======================================================================

static void ComputeCurve3d(const TopoDS_Edge&               Edge,
                           const occ::handle<Geom2d_Curve>& Curve,
                           const occ::handle<Geom_Surface>& Surf,
                           const TopLoc_Location&           Loc,
                           double                           Tol)
{
  // try to find the particular case
  // if not found call BRepLib::BuildCurve3d

  bool IsComputed = false;

  // Search only isos on analytic surfaces.
  Geom2dAdaptor_Curve C(Curve);
  GeomAdaptor_Surface S(Surf);
  GeomAbs_CurveType   CTy = C.GetType();
  GeomAbs_SurfaceType STy = S.GetType();
  BRep_Builder        TheBuilder;

  if (STy != GeomAbs_Plane)
  { // if plane buildcurve3d manage KPart
    if (CTy == GeomAbs_Line)
    {
      gp_Dir2d D = C.Line().Direction();
      if (D.IsParallel(gp::DX2d(), Precision::Angular()))
      { // Iso V.
        if (STy == GeomAbs_Sphere)
        {
          gp_Pnt2d P = C.Line().Location();
          if (std::abs(std::abs(P.Y()) - M_PI / 2.) < Precision::PConfusion())
          {
            TheBuilder.Degenerated(Edge, true);
          }
          else
          {
            gp_Sphere Sph  = S.Sphere();
            gp_Ax3    Axis = Sph.Position();
            gp_Circ   Ci   = ElSLib::SphereVIso(Axis, Sph.Radius(), P.Y());
            gp_Dir    DRev = Axis.XDirection().Crossed(Axis.YDirection());
            gp_Ax1    AxeRev(Axis.Location(), DRev);
            Ci.Rotate(AxeRev, P.X());
            occ::handle<Geom_Circle> Circle = new Geom_Circle(Ci);
            if (D.IsOpposite(gp::DX2d(), Precision::Angular()))
              Circle->Reverse();
            UpdateEdge(Edge, Circle, Loc, Tol);
          }
          IsComputed = true;
        }
        else if (STy == GeomAbs_Cylinder)
        {
          gp_Cylinder Cyl  = S.Cylinder();
          gp_Pnt2d    P    = C.Line().Location();
          gp_Ax3      Axis = Cyl.Position();
          gp_Circ     Ci   = ElSLib::CylinderVIso(Axis, Cyl.Radius(), P.Y());
          gp_Dir      DRev = Axis.XDirection().Crossed(Axis.YDirection());
          gp_Ax1      AxeRev(Axis.Location(), DRev);
          Ci.Rotate(AxeRev, P.X());
          occ::handle<Geom_Circle> Circle = new Geom_Circle(Ci);
          if (D.IsOpposite(gp::DX2d(), Precision::Angular()))
            Circle->Reverse();
          UpdateEdge(Edge, Circle, Loc, Tol);
          IsComputed = true;
        }
        else if (STy == GeomAbs_Cone)
        {
          gp_Cone  Cone = S.Cone();
          gp_Pnt2d P    = C.Line().Location();
          gp_Ax3   Axis = Cone.Position();
          gp_Circ  Ci   = ElSLib::ConeVIso(Axis, Cone.RefRadius(), Cone.SemiAngle(), P.Y());
          gp_Dir   DRev = Axis.XDirection().Crossed(Axis.YDirection());
          gp_Ax1   AxeRev(Axis.Location(), DRev);
          Ci.Rotate(AxeRev, P.X());
          occ::handle<Geom_Circle> Circle = new Geom_Circle(Ci);
          if (D.IsOpposite(gp::DX2d(), Precision::Angular()))
            Circle->Reverse();
          UpdateEdge(Edge, Circle, Loc, Tol);
          IsComputed = true;
        }
        else if (STy == GeomAbs_Torus)
        {
          gp_Torus Tore = S.Torus();
          gp_Pnt2d P    = C.Line().Location();
          gp_Ax3   Axis = Tore.Position();
          gp_Circ  Ci   = ElSLib::TorusVIso(Axis, Tore.MajorRadius(), Tore.MinorRadius(), P.Y());
          gp_Dir   DRev = Axis.XDirection().Crossed(Axis.YDirection());
          gp_Ax1   AxeRev(Axis.Location(), DRev);
          Ci.Rotate(AxeRev, P.X());
          occ::handle<Geom_Circle> Circle = new Geom_Circle(Ci);
          if (D.IsOpposite(gp::DX2d(), Precision::Angular()))
            Circle->Reverse();
          UpdateEdge(Edge, Circle, Loc, Tol);
          IsComputed = true;
        }
      }
      else if (D.IsParallel(gp::DY2d(), Precision::Angular()))
      { // Iso U.
        if (STy == GeomAbs_Sphere)
        {
          gp_Sphere Sph  = S.Sphere();
          gp_Pnt2d  P    = C.Line().Location();
          gp_Ax3    Axis = Sph.Position();
          // calculate iso 0.
          gp_Circ Ci = ElSLib::SphereUIso(Axis, Sph.Radius(), 0.);

          // set to sameparameter (rotation of circle - offset of Y)
          gp_Dir DRev = Axis.XDirection().Crossed(Axis.Direction());
          gp_Ax1 AxeRev(Axis.Location(), DRev);
          Ci.Rotate(AxeRev, P.Y());

          // transformation en iso U ( = P.X())
          DRev   = Axis.XDirection().Crossed(Axis.YDirection());
          AxeRev = gp_Ax1(Axis.Location(), DRev);
          Ci.Rotate(AxeRev, P.X());
          occ::handle<Geom_Circle> Circle = new Geom_Circle(Ci);

          if (D.IsOpposite(gp::DY2d(), Precision::Angular()))
            Circle->Reverse();
          UpdateEdge(Edge, Circle, Loc, Tol);
          IsComputed = true;
        }
        else if (STy == GeomAbs_Cylinder)
        {
          gp_Cylinder Cyl = S.Cylinder();
          gp_Pnt2d    P   = C.Line().Location();
          gp_Lin      L   = ElSLib::CylinderUIso(Cyl.Position(), Cyl.Radius(), P.X());
          gp_Vec      Tr(L.Direction());
          Tr.Multiply(P.Y());
          L.Translate(Tr);
          occ::handle<Geom_Line> Line = new Geom_Line(L);
          if (D.IsOpposite(gp::DY2d(), Precision::Angular()))
            Line->Reverse();
          UpdateEdge(Edge, Line, Loc, Tol);
          IsComputed = true;
        }
        else if (STy == GeomAbs_Cone)
        {
          gp_Cone  Cone = S.Cone();
          gp_Pnt2d P    = C.Line().Location();
          gp_Lin   L = ElSLib::ConeUIso(Cone.Position(), Cone.RefRadius(), Cone.SemiAngle(), P.X());
          gp_Vec   Tr(L.Direction());
          Tr.Multiply(P.Y());
          L.Translate(Tr);
          occ::handle<Geom_Line> Line = new Geom_Line(L);
          if (D.IsOpposite(gp::DY2d(), Precision::Angular()))
            Line->Reverse();
          UpdateEdge(Edge, Line, Loc, Tol);
          IsComputed = true;
        }
        else if (STy == GeomAbs_Torus)
        {
          gp_Torus Tore = S.Torus();
          gp_Pnt2d P    = C.Line().Location();
          gp_Ax3   Axis = Tore.Position();
          gp_Circ  Ci   = ElSLib::TorusUIso(Axis, Tore.MajorRadius(), Tore.MinorRadius(), P.X());
          Ci.Rotate(Ci.Axis(), P.Y());
          occ::handle<Geom_Circle> Circle = new Geom_Circle(Ci);

          if (D.IsOpposite(gp::DY2d(), Precision::Angular()))
            Circle->Reverse();
          UpdateEdge(Edge, Circle, Loc, Tol);
          IsComputed = true;
        }
      }
    }
  }
  else
  { // Cas Plan
    occ::handle<Geom_Curve> C3d = GeomAPI::To3d(Curve, S.Plane());
    UpdateEdge(Edge, C3d, Loc, Tol);
    IsComputed = true;
  }
  if (!IsComputed)
  {
    // BRepLib::BuildCurves3d(Edge,Tol);
    // Les Courbes 3d des edges dans le cas general ne sont calcules que si
    //  necessaire
    // ie dans les tuyaux et les bouchons ..
    //  dans la derniere etapes de MakeShells on reconstruira les courbes3d
    //  des edges du resultat qui n en ont pas.
  }
}

//=================================================================================================

BRepOffset_Offset::BRepOffset_Offset() = default;

//=================================================================================================

BRepOffset_Offset::BRepOffset_Offset(const TopoDS_Face&     Face,
                                     const double           Offset,
                                     const bool             OffsetOutside,
                                     const GeomAbs_JoinType JoinType)
{
  Init(Face, Offset, OffsetOutside, JoinType);
}

//=================================================================================================

BRepOffset_Offset::BRepOffset_Offset(
  const TopoDS_Face&                                                              Face,
  const double                                                                    Offset,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& Created,
  const bool                                                                      OffsetOutside,
  const GeomAbs_JoinType                                                          JoinType)
{
  Init(Face, Offset, Created, OffsetOutside, JoinType);
}

//=================================================================================================

BRepOffset_Offset::BRepOffset_Offset(const TopoDS_Edge&  Path,
                                     const TopoDS_Edge&  Edge1,
                                     const TopoDS_Edge&  Edge2,
                                     const double        Offset,
                                     const bool          Polynomial,
                                     const double        Tol,
                                     const GeomAbs_Shape Conti)
{
  Init(Path, Edge1, Edge2, Offset, Polynomial, Tol, Conti);
}

//=================================================================================================

BRepOffset_Offset::BRepOffset_Offset(const TopoDS_Edge&  Path,
                                     const TopoDS_Edge&  Edge1,
                                     const TopoDS_Edge&  Edge2,
                                     const double        Offset,
                                     const TopoDS_Edge&  FirstEdge,
                                     const TopoDS_Edge&  LastEdge,
                                     const bool          Polynomial,
                                     const double        Tol,
                                     const GeomAbs_Shape Conti)
{
  Init(Path, Edge1, Edge2, Offset, FirstEdge, LastEdge, Polynomial, Tol, Conti);
}

//=================================================================================================

BRepOffset_Offset::BRepOffset_Offset(const TopoDS_Vertex&                  Vertex,
                                     const NCollection_List<TopoDS_Shape>& LEdge,
                                     const double                          Offset,
                                     const bool                            Polynomial,
                                     const double                          Tol,
                                     const GeomAbs_Shape                   Conti)
{
  Init(Vertex, LEdge, Offset, Polynomial, Tol, Conti);
}

//=================================================================================================

void BRepOffset_Offset::Init(const TopoDS_Face&     Face,
                             const double           Offset,
                             const bool             OffsetOutside,
                             const GeomAbs_JoinType JoinType)
{
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> Empty;
  Init(Face, Offset, Empty, OffsetOutside, JoinType);
}

//=================================================================================================

void BRepOffset_Offset::Init(
  const TopoDS_Face&                                                              Face,
  const double                                                                    Offset,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& Created,
  const bool                                                                      OffsetOutside,
  const GeomAbs_JoinType                                                          JoinType)
{
  myShape         = Face;
  double myOffset = Offset;
  if (Face.Orientation() == TopAbs_REVERSED)
    myOffset *= -1.;

  TopLoc_Location           L;
  occ::handle<Geom_Surface> S = BRep_Tool::Surface(Face, L);
  // On detrime les surfaces, evite des recopies dans les extensions.
  occ::handle<Geom_RectangularTrimmedSurface> RT =
    occ::down_cast<Geom_RectangularTrimmedSurface>(S);
  if (!RT.IsNull())
    S = RT->BasisSurface();
  bool IsTransformed = false;
  if ((S->IsKind(STANDARD_TYPE(Geom_BSplineSurface))
       || S->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
       || S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))
       || S->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
      && !L.IsIdentity())
  {
    S = occ::down_cast<Geom_Surface>(S->Copy());
    S->Transform(L.Transformation());
    IsTransformed = true;
  }
  // particular case of cone
  occ::handle<Geom_ConicalSurface> Co;
  Co = occ::down_cast<Geom_ConicalSurface>(S);
  if (!Co.IsNull())
  {
    double Uc, Vc;
    gp_Pnt Apex = Co->Apex();
    ElSLib::Parameters(Co->Cone(), Apex, Uc, Vc);
    double UU1, UU2, VV1, VV2;
    BRepTools::UVBounds(Face, UU1, UU2, VV1, VV2);
    if (VV2 < Vc && Co->SemiAngle() > 0)
      myOffset *= -1;
    else if (VV1 > Vc && Co->SemiAngle() < 0)
      myOffset *= -1;
    if (!Co->Position().Direct())
      myOffset *= -1;
  }

  occ::handle<Geom_Surface> TheSurf = BRepOffset::Surface(S, myOffset, myStatus);

  // processing offsets of faces with possible degenerated edges
  bool   UminDegen = false;
  bool   UmaxDegen = false;
  bool   VminDegen = false;
  bool   VmaxDegen = false;
  bool   UisoDegen = false;
  gp_Pnt MinApex, MaxApex;
  bool   HasSingularity = false;
  double uf1, uf2, vf1, vf2, fpar, lpar;
  BRepTools::UVBounds(Face, uf1, uf2, vf1, vf2);
  if (!(OffsetOutside && JoinType == GeomAbs_Arc)
      && (TheSurf->DynamicType() == STANDARD_TYPE(Geom_ConicalSurface)
          || TheSurf->DynamicType() == STANDARD_TYPE(Geom_OffsetSurface)))
  {
    NCollection_Sequence<TopoDS_Shape> DegEdges;
    TopExp_Explorer                    Explo(Face, TopAbs_EDGE);
    for (; Explo.More(); Explo.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());

      if (BRep_Tool::Degenerated(anEdge))
      {
        double                    aF, aL;
        occ::handle<Geom2d_Curve> c2d = BRep_Tool::CurveOnSurface(anEdge, Face, aF, aL);

        gp_Pnt2d aFPnt2d = c2d->Value(aF), aLPnt2d = c2d->Value(aL);

        gp_Pnt aFPnt = S->Value(aFPnt2d.X(), aFPnt2d.Y()),
               aLPnt = S->Value(aLPnt2d.X(), aLPnt2d.Y());

        //  aFPnt.SquareDistance(aLPnt) > Precision::SquareConfusion() -
        // is a sufficient condition of troubles: non-singular case, but edge is degenerated.
        // So, normal handling of degenerated edges is not applicable in case of non-singular point.
        if (aFPnt.SquareDistance(aLPnt) < Precision::SquareConfusion())
        {
          DegEdges.Append(anEdge);
        }
      }
    }
    if (!DegEdges.IsEmpty())
    {
      const double TolApex = 1.e-5;
      // define the iso of singularity (u or v)
      TopoDS_Edge               theDegEdge = TopoDS::Edge(DegEdges(1));
      occ::handle<Geom2d_Curve> aCurve = BRep_Tool::CurveOnSurface(theDegEdge, Face, fpar, lpar);
      gp_Pnt2d                  fp2d   = aCurve->Value(fpar);
      gp_Pnt2d                  lp2d   = aCurve->Value(lpar);
      if (std::abs(fp2d.X() - lp2d.X()) <= Precision::PConfusion())
        UisoDegen = true;

      if (DegEdges.Length() == 2)
      {
        if (UisoDegen)
        {
          UminDegen = true;
          UmaxDegen = true;
        }
        else
        {
          VminDegen = true;
          VmaxDegen = true;
        }
      }
      else // DegEdges.Length() == 1
      {
        if (UisoDegen)
        {
          if (std::abs(fp2d.X() - uf1) <= Precision::Confusion())
            UminDegen = true;
          else
            UmaxDegen = true;
        }
        else
        {
          if (std::abs(fp2d.Y() - vf1) <= Precision::Confusion())
            VminDegen = true;
          else
            VmaxDegen = true;
        }
      }
      if (TheSurf->DynamicType() == STANDARD_TYPE(Geom_ConicalSurface))
      {
        gp_Cone theCone = occ::down_cast<Geom_ConicalSurface>(TheSurf)->Cone();
        gp_Pnt  apex    = theCone.Apex();
        double  Uapex, Vapex;
        ElSLib::Parameters(theCone, apex, Uapex, Vapex);
        if (VminDegen)
        {
          TheSurf        = new Geom_RectangularTrimmedSurface(TheSurf, uf1, uf2, Vapex, vf2);
          MinApex        = apex;
          HasSingularity = true;
        }
        else if (VmaxDegen)
        {
          TheSurf        = new Geom_RectangularTrimmedSurface(TheSurf, uf1, uf2, vf1, Vapex);
          MaxApex        = apex;
          HasSingularity = true;
        }
      }
      else // TheSurf->DynamicType() == STANDARD_TYPE(Geom_OffsetSurface)
      {
        if (UminDegen)
        {
          occ::handle<Geom_Curve> uiso = TheSurf->UIso(uf1);
          if (BRepOffset_Tool::Gabarit(uiso) > TolApex)
          {
            occ::handle<Geom_Surface> BasisSurf =
              occ::down_cast<Geom_OffsetSurface>(TheSurf)->BasisSurface();
            gp_Pnt Papex, Pfirst, Pquart, Pmid;
            Papex                                   = BasisSurf->Value(uf1, vf1);
            Pfirst                                  = TheSurf->Value(uf1, vf1);
            Pquart                                  = TheSurf->Value(uf1, 0.75 * vf1 + 0.25 * vf2);
            Pmid                                    = TheSurf->Value(uf1, 0.5 * (vf1 + vf2));
            gp_Vec                    DirApex       = gp_Vec(Pfirst, Pquart) ^ gp_Vec(Pfirst, Pmid);
            occ::handle<Geom_Line>    LineApex      = new Geom_Line(Papex, DirApex);
            gp_Vec                    DirGeneratrix = BasisSurf->DN(uf1, vf1, 1, 0);
            occ::handle<Geom_Line>    LineGeneratrix = new Geom_Line(Pfirst, DirGeneratrix);
            GeomAPI_ExtremaCurveCurve theExtrema(LineGeneratrix, LineApex);
            gp_Pnt                    Pint1, Pint2;
            theExtrema.NearestPoints(Pint1, Pint2);
            double length = Pfirst.Distance(Pint1);
            if (OffsetOutside)
            {
              occ::handle<Geom_BoundedSurface> aSurf =
                new Geom_RectangularTrimmedSurface(TheSurf, uf1, uf2, vf1, vf2);
              GeomLib::ExtendSurfByLength(aSurf, length, 1, true, false);
              TheSurf = aSurf;
              double u1, u2, v1, v2;
              TheSurf->Bounds(u1, u2, v1, v2);
              MinApex = TheSurf->Value(u1, vf1);
            }
            else
            {
              occ::handle<Geom_Curve>     viso = TheSurf->VIso(vf1);
              GeomAPI_ProjectPointOnCurve Projector(Pint1, viso);
              double                      NewFirstU = Projector.LowerDistanceParameter();
              TheSurf = new Geom_RectangularTrimmedSurface(TheSurf, NewFirstU, uf2, vf1, vf2);
              MinApex = TheSurf->Value(NewFirstU, vf1);
            }
            HasSingularity = true;
          }
        } // end of if (UminDegen)
        if (UmaxDegen)
        {
          occ::handle<Geom_Curve> uiso = TheSurf->UIso(uf2);
          if (BRepOffset_Tool::Gabarit(uiso) > TolApex)
          {
            occ::handle<Geom_Surface> BasisSurf =
              occ::down_cast<Geom_OffsetSurface>(TheSurf)->BasisSurface();
            gp_Pnt Papex, Pfirst, Pquart, Pmid;
            Papex                                   = BasisSurf->Value(uf2, vf1);
            Pfirst                                  = TheSurf->Value(uf2, vf1);
            Pquart                                  = TheSurf->Value(uf2, 0.75 * vf1 + 0.25 * vf2);
            Pmid                                    = TheSurf->Value(uf2, 0.5 * (vf1 + vf2));
            gp_Vec                    DirApex       = gp_Vec(Pfirst, Pquart) ^ gp_Vec(Pfirst, Pmid);
            occ::handle<Geom_Line>    LineApex      = new Geom_Line(Papex, DirApex);
            gp_Vec                    DirGeneratrix = BasisSurf->DN(uf2, vf1, 1, 0);
            occ::handle<Geom_Line>    LineGeneratrix = new Geom_Line(Pfirst, DirGeneratrix);
            GeomAPI_ExtremaCurveCurve theExtrema(LineGeneratrix, LineApex);
            gp_Pnt                    Pint1, Pint2;
            theExtrema.NearestPoints(Pint1, Pint2);
            double length = Pfirst.Distance(Pint1);
            if (OffsetOutside)
            {
              occ::handle<Geom_BoundedSurface> aSurf =
                new Geom_RectangularTrimmedSurface(TheSurf, uf1, uf2, vf1, vf2);
              GeomLib::ExtendSurfByLength(aSurf, length, 1, true, true);
              TheSurf = aSurf;
              double u1, u2, v1, v2;
              TheSurf->Bounds(u1, u2, v1, v2);
              MaxApex = TheSurf->Value(u2, vf1);
            }
            else
            {
              occ::handle<Geom_Curve>     viso = TheSurf->VIso(vf1);
              GeomAPI_ProjectPointOnCurve Projector(Pint1, viso);
              double                      NewLastU = Projector.LowerDistanceParameter();
              TheSurf = new Geom_RectangularTrimmedSurface(TheSurf, uf1, NewLastU, vf1, vf2);
              MaxApex = TheSurf->Value(NewLastU, vf1);
            }
            HasSingularity = true;
          }
        } // end of if (UmaxDegen)
        if (VminDegen)
        {
          occ::handle<Geom_Curve> viso = TheSurf->VIso(vf1);
          if (BRepOffset_Tool::Gabarit(viso) > TolApex)
          {
            occ::handle<Geom_Surface> BasisSurf =
              occ::down_cast<Geom_OffsetSurface>(TheSurf)->BasisSurface();
            gp_Pnt Papex, Pfirst, Pquart, Pmid;
            Papex                                   = BasisSurf->Value(uf1, vf1);
            Pfirst                                  = TheSurf->Value(uf1, vf1);
            Pquart                                  = TheSurf->Value(0.75 * uf1 + 0.25 * uf2, vf1);
            Pmid                                    = TheSurf->Value(0.5 * (uf1 + uf2), vf1);
            gp_Vec                    DirApex       = gp_Vec(Pfirst, Pquart) ^ gp_Vec(Pfirst, Pmid);
            occ::handle<Geom_Line>    LineApex      = new Geom_Line(Papex, DirApex);
            gp_Vec                    DirGeneratrix = BasisSurf->DN(uf1, vf1, 0, 1);
            occ::handle<Geom_Line>    LineGeneratrix = new Geom_Line(Pfirst, DirGeneratrix);
            GeomAPI_ExtremaCurveCurve theExtrema(LineGeneratrix, LineApex);
            gp_Pnt                    Pint1, Pint2;
            theExtrema.NearestPoints(Pint1, Pint2);
            double length = Pfirst.Distance(Pint1);
            if (OffsetOutside)
            {
              occ::handle<Geom_BoundedSurface> aSurf =
                new Geom_RectangularTrimmedSurface(TheSurf, uf1, uf2, vf1, vf2);
              GeomLib::ExtendSurfByLength(aSurf, length, 1, false, false);
              TheSurf = aSurf;
              double u1, u2, v1, v2;
              TheSurf->Bounds(u1, u2, v1, v2);
              MinApex = TheSurf->Value(uf1, v1);
            }
            else
            {
              occ::handle<Geom_Curve>     uiso = TheSurf->UIso(uf1);
              GeomAPI_ProjectPointOnCurve Projector(Pint1, uiso);
              double                      NewFirstV = Projector.LowerDistanceParameter();
              TheSurf = new Geom_RectangularTrimmedSurface(TheSurf, uf1, uf2, NewFirstV, vf2);
              MinApex = TheSurf->Value(uf1, NewFirstV);
              // TheSurf = new Geom_RectangularTrimmedSurface(TheSurf, uf1, uf2, vf1+length, vf2);
              // MinApex = TheSurf->Value( uf1, vf1+length );
            }
            HasSingularity = true;
          }
        } // end of if (VminDegen)
        if (VmaxDegen)
        {
          occ::handle<Geom_Curve> viso = TheSurf->VIso(vf2);
          if (BRepOffset_Tool::Gabarit(viso) > TolApex)
          {
            occ::handle<Geom_Surface> BasisSurf =
              occ::down_cast<Geom_OffsetSurface>(TheSurf)->BasisSurface();
            gp_Pnt Papex, Pfirst, Pquart, Pmid;
            Papex                                   = BasisSurf->Value(uf1, vf2);
            Pfirst                                  = TheSurf->Value(uf1, vf2);
            Pquart                                  = TheSurf->Value(0.75 * uf1 + 0.25 * uf2, vf2);
            Pmid                                    = TheSurf->Value(0.5 * (uf1 + uf2), vf2);
            gp_Vec                    DirApex       = gp_Vec(Pfirst, Pquart) ^ gp_Vec(Pfirst, Pmid);
            occ::handle<Geom_Line>    LineApex      = new Geom_Line(Papex, DirApex);
            gp_Vec                    DirGeneratrix = BasisSurf->DN(uf1, vf2, 0, 1);
            occ::handle<Geom_Line>    LineGeneratrix = new Geom_Line(Pfirst, DirGeneratrix);
            GeomAPI_ExtremaCurveCurve theExtrema(LineGeneratrix, LineApex);
            gp_Pnt                    Pint1, Pint2;
            theExtrema.NearestPoints(Pint1, Pint2);
            double length = Pfirst.Distance(Pint1);
            if (OffsetOutside)
            {
              occ::handle<Geom_BoundedSurface> aSurf =
                new Geom_RectangularTrimmedSurface(TheSurf, uf1, uf2, vf1, vf2);
              GeomLib::ExtendSurfByLength(aSurf, length, 1, false, true);
              TheSurf = aSurf;
              double u1, u2, v1, v2;
              TheSurf->Bounds(u1, u2, v1, v2);
              MaxApex = TheSurf->Value(uf1, v2);
            }
            else
            {
              occ::handle<Geom_Curve>     uiso = TheSurf->UIso(uf1);
              GeomAPI_ProjectPointOnCurve Projector(Pint1, uiso);
              double                      NewLastV = Projector.LowerDistanceParameter();
              TheSurf = new Geom_RectangularTrimmedSurface(TheSurf, uf1, uf2, vf1, NewLastV);
              MaxApex = TheSurf->Value(uf1, NewLastV);
              // TheSurf = new Geom_RectangularTrimmedSurface(TheSurf, uf1, uf2, vf1, vf2-length);
              // MaxApex = TheSurf->Value( uf1, vf2-length );
            }
            HasSingularity = true;
          }
        } // end of if (VmaxDegen)
      } // end of else (case of Geom_OffsetSurface)
    } // end of if (!DegEdges.IsEmpty())
  } // end of processing offsets of faces with possible degenerated edges

  // find the PCurves of the edges of <Faces>

  BRep_Builder myBuilder;
  myBuilder.MakeFace(myFace);
  if (!IsTransformed)
    myBuilder.UpdateFace(myFace, TheSurf, L, BRep_Tool::Tolerance(Face));
  else
    myBuilder.UpdateFace(myFace, TheSurf, TopLoc_Location(), BRep_Tool::Tolerance(Face));

  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> MapSS;

  // mise a jour de la map sur les vertex deja crees
  TopoDS_Shape aLocalShapeOriented = Face.Oriented(TopAbs_FORWARD);
  TopoDS_Face  CurFace             = TopoDS::Face(aLocalShapeOriented);
  //  TopoDS_Face CurFace = TopoDS::Face(Face.Oriented(TopAbs_FORWARD));

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> VonDegen;
  double                                                 u1, u2, v1, v2;
  TheSurf->Bounds(u1, u2, v1, v2);

  TopExp_Explorer exp(CurFace, TopAbs_EDGE);
  for (; exp.More(); exp.Next())
  {
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
    TopoDS_Vertex      V1, V2, OV1, OV2;
    TopExp::Vertices(E, V1, V2);
    if (HasSingularity && BRep_Tool::Degenerated(E))
      VonDegen.Add(V1);
    if (Created.IsBound(E))
    {
      const TopoDS_Edge& OE = TopoDS::Edge(Created(E));
      TopExp::Vertices(OE, OV1, OV2);
      if (!MapSS.IsBound(V1))
        MapSS.Bind(V1, OV1);
      if (!MapSS.IsBound(V2))
        MapSS.Bind(V2, OV2);
    }
    if (Created.IsBound(V1))
    {
      if (!MapSS.IsBound(V1))
        MapSS.Bind(V1, Created(V1));
    }
    if (Created.IsBound(V2))
    {
      if (!MapSS.IsBound(V2))
        MapSS.Bind(V2, Created(V2));
    }
  }

  TopExp_Explorer expw(CurFace, TopAbs_WIRE);
  for (; expw.More(); expw.Next())
  {
    const TopoDS_Wire& W = TopoDS::Wire(expw.Current());
    TopExp_Explorer    expe(W.Oriented(TopAbs_FORWARD), TopAbs_EDGE);
    TopoDS_Wire        OW;
    myBuilder.MakeWire(OW);
    for (; expe.More(); expe.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(expe.Current());
      TopoDS_Vertex      V1, V2;
      TopExp::Vertices(E, V1, V2);
      gp_Pnt2d                  P2d1, P2d2;
      gp_Pnt                    P1, P2;
      double                    vstart, vend;
      double                    f, l;
      occ::handle<Geom2d_Curve> C2d = BRep_Tool::CurveOnSurface(E, CurFace, f, l);
      TopoDS_Edge               OE;
      if (MapSS.IsBound(E) && !VonDegen.Contains(V1) && !VonDegen.Contains(V2))
      { // c`est un edge de couture
        OE                                    = TopoDS::Edge(MapSS(E));
        TopoDS_Shape              aLocalShape = E.Reversed();
        occ::handle<Geom2d_Curve> C2d_1 =
          BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalShape), CurFace, f, l);
        //	occ::handle<Geom2d_Curve> C2d_1 =
        //	  BRep_Tool::CurveOnSurface(TopoDS::Edge(E.Reversed()),CurFace,f,l);
        if (E.Orientation() == TopAbs_FORWARD)
          UpdateEdge(OE, C2d, C2d_1, myFace, BRep_Tool::Tolerance(E));
        else
          UpdateEdge(OE, C2d_1, C2d, myFace, BRep_Tool::Tolerance(E));
        myBuilder.Range(OE, f, l);
      }
      else
      {
        TopoDS_Shape aLocalShape = E.Oriented(TopAbs_FORWARD);
        TopoDS_Edge  Eforward    = TopoDS::Edge(aLocalShape);
        P2d1                     = C2d->Value(BRep_Tool::Parameter(V1, Eforward, CurFace));
        P2d2                     = C2d->Value(BRep_Tool::Parameter(V2, Eforward, CurFace));
        if (VonDegen.Contains(V1))
        {
          if (std::abs(P2d1.Y() - vf1) <= Precision::Confusion())
          {
            P1     = MinApex;
            vstart = v1;
          }
          else
          {
            P1     = MaxApex;
            vstart = v2;
          }
        }
        else
        {
          TheSurf->D0(P2d1.X(), P2d1.Y(), P1);
          if (!L.IsIdentity() && !IsTransformed)
            P1.Transform(L.Transformation());
          vstart = P2d1.Y();
        }
        if (VonDegen.Contains(V2))
        {
          if (std::abs(P2d2.Y() - vf1) <= Precision::Confusion())
          {
            P2   = MinApex;
            vend = v1;
          }
          else
          {
            P2   = MaxApex;
            vend = v2;
          }
        }
        else
        {
          TheSurf->D0(P2d2.X(), P2d2.Y(), P2);
          if (!L.IsIdentity() && !IsTransformed)
            P2.Transform(L.Transformation());
          vend = P2d2.Y();
        }
        // E a-t-il ume image dans la Map des Created ?
        if (Created.IsBound(E))
        {
          OE = TopoDS::Edge(Created(E));
        }
        else if (MapSS.IsBound(E)) // seam edge
          OE = TopoDS::Edge(MapSS(E));
        else
        {
          myBuilder.MakeEdge(OE);
          TopoDS_Vertex OV1, OV2;
          if (MapSS.IsBound(V1))
          {
            OV1 = TopoDS::Vertex(MapSS(V1));
          }
          else
          {
            myBuilder.MakeVertex(OV1);
            myBuilder.UpdateVertex(OV1, P1, BRep_Tool::Tolerance(V1));
            MapSS.Bind(V1, OV1);
          }
          if (MapSS.IsBound(V2))
          {
            OV2 = TopoDS::Vertex(MapSS(V2));
          }
          else
          {
            myBuilder.MakeVertex(OV2);
            myBuilder.UpdateVertex(OV2, P2, BRep_Tool::Tolerance(V2));
            MapSS.Bind(V2, OV2);
          }
          myBuilder.Add(OE, OV1.Oriented(V1.Orientation()));
          myBuilder.Add(OE, OV2.Oriented(V2.Orientation()));
          if (BRep_Tool::Degenerated(E))
          {
            myBuilder.Degenerated(OE, true);
            /*
    #ifdef OCCT_DEBUG
            gp_Pnt        P1,P2;
            gp_Pnt2d      P2d;
            P2d = C2d->Value(f); TheSurf->D0(P2d.X(),P2d.Y(),P1);
            P2d = C2d->Value(l); TheSurf->D0(P2d.X(),P2d.Y(),P2);
            double Tol = BRep_Tool::Tolerance(V1);
            if (!P1.IsEqual(P2,Tol)) {
              std::cout <<"BRepOffset_Offset : E degenerated -> OE not degenerated"<<std::endl;
            }
    #endif
                */
          }
        }
        if (VonDegen.Contains(V1) || VonDegen.Contains(V2))
        {
          if (VonDegen.Contains(V1))
            P2d1.SetY(vstart);
          if (VonDegen.Contains(V2))
            P2d2.SetY(vend);
          C2d = new Geom2d_Line(P2d1, gp_Vec2d(P2d1, P2d2));
          f   = 0.;
          l   = P2d1.Distance(P2d2);
          if (MapSS.IsBound(E)) // seam edge
          {
            occ::handle<Geom2d_Curve> C2d_1 = BRep_Tool::CurveOnSurface(OE, myFace, f, l);
            if (E.Orientation() == TopAbs_FORWARD)
              UpdateEdge(OE, C2d, C2d_1, myFace, BRep_Tool::Tolerance(E));
            else
              UpdateEdge(OE, C2d_1, C2d, myFace, BRep_Tool::Tolerance(E));
          }
          else
            UpdateEdge(OE, C2d, myFace, BRep_Tool::Tolerance(E));
          // myBuilder.Range(OE,f,l);
          myBuilder.Range(OE, myFace, f, l);
          if (!BRep_Tool::Degenerated(E) && TheSurf->IsUClosed())
          {
            TopoDS_Shape              aLocalShapeReversedE = E.Reversed();
            occ::handle<Geom2d_Curve> C2d_1 =
              BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalShapeReversedE), CurFace, f, l);
            P2d1 = C2d_1->Value(BRep_Tool::Parameter(V1, E, CurFace));
            P2d2 = C2d_1->Value(BRep_Tool::Parameter(V2, E, CurFace));
            if (VonDegen.Contains(V1))
              P2d1.SetY(vstart);
            if (VonDegen.Contains(V2))
              P2d2.SetY(vend);
            C2d_1 = new Geom2d_Line(P2d1, gp_Vec2d(P2d1, P2d2));
            if (E.Orientation() == TopAbs_FORWARD)
              UpdateEdge(OE, C2d, C2d_1, myFace, BRep_Tool::Tolerance(E));
            else
              UpdateEdge(OE, C2d_1, C2d, myFace, BRep_Tool::Tolerance(E));
          }
          /*
          if (!BRep_Tool::Degenerated(E))
            {
          occ::handle<Geom_Line> theLine = new Geom_Line( P1, gp_Vec(P1, P2) );
          myBuilder.UpdateEdge( OE, theLine, BRep_Tool::Tolerance(E) );
            }
          */
        }
        else
        {
          UpdateEdge(OE, C2d, myFace, BRep_Tool::Tolerance(E));
          myBuilder.Range(OE, f, l);
          // ComputeCurve3d(OE,C2d,TheSurf,L,BRep_Tool::Tolerance(E));
        }
        if (!BRep_Tool::Degenerated(OE))
          ComputeCurve3d(OE, C2d, TheSurf, L, BRep_Tool::Tolerance(E));
        MapSS.Bind(E, OE);
      }
      myBuilder.Add(OW, OE.Oriented(E.Orientation()));
    }
    myBuilder.Add(myFace, OW.Oriented(W.Orientation()));
  }

  myFace.Orientation(Face.Orientation());

  BRepTools::Update(myFace);
}

//=================================================================================================

void BRepOffset_Offset::Init(const TopoDS_Edge&  Path,
                             const TopoDS_Edge&  Edge1,
                             const TopoDS_Edge&  Edge2,
                             const double        Offset,
                             const bool          Polynomial,
                             const double        Tol,
                             const GeomAbs_Shape Conti)
{
  TopoDS_Edge FirstEdge, LastEdge;
  Init(Path, Edge1, Edge2, Offset, FirstEdge, LastEdge, Polynomial, Tol, Conti);
}

//=================================================================================================

void BRepOffset_Offset::Init(const TopoDS_Edge&  Path,
                             const TopoDS_Edge&  Edge1,
                             const TopoDS_Edge&  Edge2,
                             const double        Offset,
                             const TopoDS_Edge&  FirstEdge,
                             const TopoDS_Edge&  LastEdge,
                             const bool          Polynomial,
                             const double        Tol,
                             const GeomAbs_Shape Conti)
{
  bool C1Denerated = false;
  bool C2Denerated = false;
  myStatus         = BRepOffset_Good;
  myShape          = Path;

  TopLoc_Location Loc;
  double          f[3], l[3];

  occ::handle<Geom_Curve> CP = BRep_Tool::Curve(Path, Loc, f[0], l[0]);
  CP                         = new Geom_TrimmedCurve(CP, f[0], l[0]);
  CP->Transform(Loc.Transformation());
  occ::handle<GeomAdaptor_Curve> HCP = new GeomAdaptor_Curve(CP);

  occ::handle<Geom_Curve> C1 = BRep_Tool::Curve(Edge1, Loc, f[1], l[1]);

  occ::handle<Adaptor3d_Curve> HEdge1;
  bool                         C1is3D = true;
  if (C1.IsNull())
  {
    C1is3D = false;
    occ::handle<Geom2d_Curve> C12d;
    occ::handle<Geom_Surface> S1;
    BRep_Tool::CurveOnSurface(Edge1, C12d, S1, Loc, f[1], l[1]);
    S1   = occ::down_cast<Geom_Surface>(S1->Transformed(Loc.Transformation()));
    C12d = new Geom2d_TrimmedCurve(C12d, f[1], l[1]);
    occ::handle<GeomAdaptor_Surface> HS1 = new GeomAdaptor_Surface(S1);
    occ::handle<Geom2dAdaptor_Curve> HC1 = new Geom2dAdaptor_Curve(C12d);
    Adaptor3d_CurveOnSurface         Cons(HC1, HS1);
    HEdge1 = new Adaptor3d_CurveOnSurface(Cons);
  }
  else
  {
    C1 = new Geom_TrimmedCurve(C1, f[1], l[1]);
    C1->Transform(Loc.Transformation());
    HEdge1 = new GeomAdaptor_Curve(C1);
    GeomAdaptor_Curve AC1(C1);
    if (AC1.GetType() == GeomAbs_Circle)
    {
      C1Denerated = (AC1.Circle().Radius() < Precision::Confusion());
    }
  }

  occ::handle<Geom_Curve> C2 = BRep_Tool::Curve(Edge2, Loc, f[2], l[2]);

  occ::handle<Adaptor3d_Curve> HEdge2;
  bool                         C2is3D = true;
  if (C2.IsNull())
  {
    C2is3D = false;
    occ::handle<Geom2d_Curve> C12d;
    occ::handle<Geom_Surface> S1;
    BRep_Tool::CurveOnSurface(Edge2, C12d, S1, Loc, f[2], l[2]);
    S1   = occ::down_cast<Geom_Surface>(S1->Transformed(Loc.Transformation()));
    C12d = new Geom2d_TrimmedCurve(C12d, f[2], l[2]);
    occ::handle<GeomAdaptor_Surface> HS1 = new GeomAdaptor_Surface(S1);
    occ::handle<Geom2dAdaptor_Curve> HC1 = new Geom2dAdaptor_Curve(C12d);
    Adaptor3d_CurveOnSurface         Cons(HC1, HS1);
    HEdge2 = new Adaptor3d_CurveOnSurface(Cons);
  }
  else
  {
    C2 = new Geom_TrimmedCurve(C2, f[2], l[2]);
    C2->Transform(Loc.Transformation());
    HEdge2 = new GeomAdaptor_Curve(C2);
    GeomAdaptor_Curve AC2(C2);
    if (AC2.GetType() == GeomAbs_Circle)
    {
      C2Denerated = (AC2.Circle().Radius() < Precision::Confusion());
    }
  }

  // Calcul du tuyau
  GeomFill_Pipe Pipe(HCP, HEdge1, HEdge2, std::abs(Offset));
  Pipe.Perform(Tol, Polynomial, Conti);
  if (!Pipe.IsDone())
    throw Standard_ConstructionError("GeomFill_Pipe : Cannot make a surface");
  double ErrorPipe = Pipe.ErrorOnSurf();

  occ::handle<Geom_Surface> S      = Pipe.Surface();
  bool                      ExchUV = Pipe.ExchangeUV();
  double                    f1, l1, f2, l2;
  S->Bounds(f1, l1, f2, l2);

  // Perform the face
  double       PathTol = BRep_Tool::Tolerance(Path);
  double       TheTol;
  BRep_Builder myBuilder;
  myBuilder.MakeFace(myFace);
  TopLoc_Location Id;
  myBuilder.UpdateFace(myFace, S, Id, PathTol);

  // update de Edge1. (Rem : has already a 3d curve)
  double                    U, U1, U2;
  occ::handle<Geom2d_Curve> PC;
  if (ExchUV)
  {
    PC = new Geom2d_Line(gp_Pnt2d(0, f2), gp_Dir2d(gp_Dir2d::D::X));
    U1 = f1;
    U2 = l1;
    if (!C1is3D)
      C1 = S->VIso(f2);
  }
  else
  {
    PC = new Geom2d_Line(gp_Pnt2d(f1, 0), gp_Dir2d(gp_Dir2d::D::Y));
    U1 = f2;
    U2 = l2;
    if (!C1is3D)
      C1 = S->UIso(f1);
  }

  occ::handle<Geom_Curve> Dummy;
  if (!C1is3D)
    UpdateEdge(Edge1, C1, Id, BRep_Tool::Tolerance(Edge1));
  else if (C1Denerated)
  {
    UpdateEdge(Edge1, Dummy, Id, BRep_Tool::Tolerance(Edge1));
    myBuilder.Degenerated(Edge1, true);
  }

  TheTol = std::max(PathTol, BRep_Tool::Tolerance(Edge1) + ErrorPipe);
  UpdateEdge(Edge1, PC, myFace, TheTol);

  // mise a same range de la nouvelle pcurve.
  if (!C1is3D && !C1Denerated)
  {
    myBuilder.SameRange(Edge1, false);
    myBuilder.Range(Edge1, U1, U2, true);
  }
  myBuilder.Range(Edge1, myFace, U1, U2);
  BRepLib::SameRange(Edge1);

  // mise a sameparameter pour les KPart
  if (ErrorPipe == 0)
  {
    TheTol = std::max(TheTol, Tol);
    myBuilder.SameParameter(Edge1, false);
    BRepLib::SameParameter(Edge1, TheTol);
  }

  // Update de edge2. (Rem : has already a 3d curve)
  if (ExchUV)
  {
    PC = new Geom2d_Line(gp_Pnt2d(0, l2), gp_Dir2d(gp_Dir2d::D::X));
    U1 = f1;
    U2 = l1;
    if (!C2is3D)
      C2 = S->VIso(l2);
  }
  else
  {
    PC = new Geom2d_Line(gp_Pnt2d(l1, 0), gp_Dir2d(gp_Dir2d::D::Y));
    U1 = f2;
    U2 = l2;
    if (!C2is3D)
      C2 = S->UIso(l1);
  }

  if (!C2is3D)
    UpdateEdge(Edge2, C2, Id, BRep_Tool::Tolerance(Edge2));
  else if (C2Denerated)
  {
    UpdateEdge(Edge2, Dummy, Id, BRep_Tool::Tolerance(Edge2));
    myBuilder.Degenerated(Edge2, true);
  }

  TheTol = std::max(PathTol, BRep_Tool::Tolerance(Edge2) + ErrorPipe);
  UpdateEdge(Edge2, PC, myFace, TheTol);

  // mise a same range de la nouvelle pcurve.
  myBuilder.SameRange(Edge2, false);
  if (!C2is3D && !C2Denerated)
    myBuilder.Range(Edge2, U1, U2, true);
  myBuilder.Range(Edge2, myFace, U1, U2);
  BRepLib::SameRange(Edge2);

  // mise a sameparameter pour les KPart
  if (ErrorPipe == 0)
  {
    TheTol = std::max(TheTol, Tol);
    myBuilder.SameParameter(Edge2, false);
    BRepLib::SameParameter(Edge2, TheTol);
  }

  TopoDS_Edge Edge3, Edge4;
  // eval edge3
  TopoDS_Vertex V1f, V1l, V2f, V2l;
  TopExp::Vertices(Path, V1f, V1l);
  bool IsClosed = (V1f.IsSame(V1l));

  TopExp::Vertices(Edge1, V1f, V1l);
  TopExp::Vertices(Edge2, V2f, V2l);

  bool StartDegenerated = (V1f.IsSame(V2f));
  bool EndDegenerated   = (V1l.IsSame(V2l));

  bool E3rev = false;
  bool E4rev = false;

  TopoDS_Vertex VVf, VVl;
  if (FirstEdge.IsNull())
  {
    myBuilder.MakeEdge(Edge3);
    myBuilder.Add(Edge3, V1f.Oriented(TopAbs_FORWARD));
    myBuilder.Add(Edge3, V2f.Oriented(TopAbs_REVERSED));
  }
  else
  {
    TopoDS_Shape aLocalEdge = FirstEdge.Oriented(TopAbs_FORWARD);
    Edge3                   = TopoDS::Edge(aLocalEdge);
    //    Edge3 = TopoDS::Edge(FirstEdge.Oriented(TopAbs_FORWARD));
    TopExp::Vertices(Edge3, VVf, VVl);
#ifdef OCCT_DEBUG
    // si firstedge n est pas nul, il faut que les vertex soient partages
    if (!VVf.IsSame(V1f) && !VVf.IsSame(V2f))
    {
      std::cout << "Attention Vertex non partages !!!!!!" << std::endl;
    }
#endif
    if (!VVf.IsSame(V1f) && !VVf.IsSame(V2f))
    {
      // On fait vraisemblablement des conneries !!
      // On cree un autre edge, on appelle le Sewing apres.
      myBuilder.MakeEdge(Edge3);
      myBuilder.Add(Edge3, V1f.Oriented(TopAbs_FORWARD));
      myBuilder.Add(Edge3, V2f.Oriented(TopAbs_REVERSED));
    }
    else if (!VVf.IsSame(V1f))
    {
      Edge3.Reverse();
      E3rev = true;
    }
  }

  if (IsClosed)
    Edge4 = Edge3;

  constexpr double TolApp = Precision::Approximation();

  occ::handle<Geom2d_Line> L1, L2;
  if (IsClosed)
  {
    if (ExchUV)
    {
      // rem : si ExchUv, il faut  reverser le Wire.
      // donc l'edge Forward dans la face sera E4 : d'ou L1 et L2
      L2 = new Geom2d_Line(gp_Pnt2d(f1, 0), gp_Dir2d(gp_Dir2d::D::Y));
      L1 = new Geom2d_Line(gp_Pnt2d(l1, 0), gp_Dir2d(gp_Dir2d::D::Y));
      U1 = f2;
      U2 = l2;
    }
    else
    {
      L1 = new Geom2d_Line(gp_Pnt2d(0, f2), gp_Dir2d(gp_Dir2d::D::X));
      L2 = new Geom2d_Line(gp_Pnt2d(0, l2), gp_Dir2d(gp_Dir2d::D::X));
      U1 = f1;
      U2 = l1;
    }
    if (E3rev)
    {
      L1->Reverse();
      L2->Reverse();
      U  = -U1;
      U1 = -U2;
      U2 = U;
    }
    UpdateEdge(Edge3, L1, L2, myFace, PathTol);
    myBuilder.Range(Edge3, myFace, U1, U2);
    if (StartDegenerated)
      myBuilder.Degenerated(Edge3, true);
    else if (FirstEdge.IsNull()) // then the 3d curve has not been yet computed
      ComputeCurve3d(Edge3, L1, S, Id, TolApp);
  }
  else
  {
    if (LastEdge.IsNull())
    {
      myBuilder.MakeEdge(Edge4);
      myBuilder.Add(Edge4, V1l.Oriented(TopAbs_FORWARD));
      myBuilder.Add(Edge4, V2l.Oriented(TopAbs_REVERSED));
    }
    else
    {
      TopoDS_Shape aLocalEdge = LastEdge.Oriented(TopAbs_FORWARD);
      Edge4                   = TopoDS::Edge(aLocalEdge);
      //      Edge4 = TopoDS::Edge(LastEdge.Oriented(TopAbs_FORWARD));
      TopExp::Vertices(Edge4, VVf, VVl);
#ifdef OCCT_DEBUG
      // si lastedge n est pas nul, il faut que les vertex soient partages
      if (!VVf.IsSame(V1l) && !VVf.IsSame(V2l))
      {
        std::cout << "Attention Vertex non partages !!!!!!" << std::endl;
      }
#endif
      if (!VVf.IsSame(V1l) && !VVf.IsSame(V2l))
      {
        // On fait vraisemblablement des conneries !!
        // On cree un autre edge, on appelle le Sewing apres.
        myBuilder.MakeEdge(Edge4);
        myBuilder.Add(Edge4, V1l.Oriented(TopAbs_FORWARD));
        myBuilder.Add(Edge4, V2l.Oriented(TopAbs_REVERSED));
      }
      else if (!VVf.IsSame(V1l))
      {
        Edge4.Reverse();
        E4rev = true;
      }
    }

    if (ExchUV)
    {
      L1 = new Geom2d_Line(gp_Pnt2d(f1, 0), gp_Dir2d(gp_Dir2d::D::Y));
      U1 = f2;
      U2 = l2;
    }
    else
    {
      L1 = new Geom2d_Line(gp_Pnt2d(0, f2), gp_Dir2d(gp_Dir2d::D::X));
      U1 = f1;
      U2 = l1;
    }
    if (E3rev)
    {
      L1->Reverse();
      U  = -U1;
      U1 = -U2;
      U2 = U;
    }
    UpdateEdge(Edge3, L1, myFace, PathTol);
    myBuilder.Range(Edge3, myFace, U1, U2);
    if (StartDegenerated)
      myBuilder.Degenerated(Edge3, true);
    else if (FirstEdge.IsNull()) // then the 3d curve has not been yet computed
      ComputeCurve3d(Edge3, L1, S, Id, TolApp);

    if (ExchUV)
    {
      L2 = new Geom2d_Line(gp_Pnt2d(l1, 0), gp_Dir2d(gp_Dir2d::D::Y));
      U1 = f2;
      U2 = l2;
    }
    else
    {
      L2 = new Geom2d_Line(gp_Pnt2d(0, l2), gp_Dir2d(gp_Dir2d::D::X));
      U1 = f1;
      U2 = l1;
    }
    if (E4rev)
    {
      L2->Reverse();
      U  = -U1;
      U1 = -U2;
      U2 = U;
    }
    UpdateEdge(Edge4, L2, myFace, PathTol);
    myBuilder.Range(Edge4, myFace, U1, U2);
    if (EndDegenerated)
      myBuilder.Degenerated(Edge4, true);
    else if (LastEdge.IsNull()) // then the 3d curve has not been yet computed
      ComputeCurve3d(Edge4, L2, S, Id, TolApp);
  }

  // SameParameter ??
  if (!FirstEdge.IsNull() && !StartDegenerated)
  {
    BRepLib::BuildCurve3d(Edge3, PathTol);
    myBuilder.SameRange(Edge3, false);
    myBuilder.SameParameter(Edge3, false);
    BRepLib::SameParameter(Edge3, Tol);
  }
  if (!LastEdge.IsNull() && !EndDegenerated)
  {
    BRepLib::BuildCurve3d(Edge4, PathTol);
    myBuilder.SameRange(Edge4, false);
    myBuilder.SameParameter(Edge4, false);
    BRepLib::SameParameter(Edge4, Tol);
  }

  TopoDS_Wire W;
  myBuilder.MakeWire(W);

  myBuilder.Add(W, Edge1.Oriented(TopAbs_REVERSED));
  myBuilder.Add(W, Edge2.Oriented(TopAbs_FORWARD));
  myBuilder.Add(W, Edge4.Reversed());
  myBuilder.Add(W, Edge3);

  if (ExchUV)
  {
    W.Reverse();
  }

  myBuilder.Add(myFace, W);
  if (ExchUV)
    myFace.Reverse();

  BRepTools::Update(myFace);

  if (Edge1.Orientation() == TopAbs_REVERSED)
    myFace.Reverse();
}

//=================================================================================================

void BRepOffset_Offset::Init(const TopoDS_Vertex&                  Vertex,
                             const NCollection_List<TopoDS_Shape>& LEdge,
                             const double                          Offset,
                             const bool                            Polynomial,
                             const double                          TolApp,
                             const GeomAbs_Shape                   Conti)
{
  myStatus = BRepOffset_Good;
  myShape  = Vertex;

  // evaluate the Ax3 of the Sphere
  // find 3 different vertices in LEdge
  NCollection_List<TopoDS_Shape>::Iterator it;
  TopoDS_Vertex                            V1, V2, V3, V4;

#ifdef OCCT_DEBUG
  char* name = new char[100];
  if (Affich)
  {
    NbOFFSET++;

    Sprintf(name, "VOnSph_%d", NbOFFSET);
  #ifdef DRAW
    DBRep::Set(name, Vertex);
  #endif
    int NbEdges = 1;
    for (it.Initialize(LEdge); it.More(); it.Next())
    {
      Sprintf(name, "EOnSph_%d_%d", NbOFFSET, NbEdges++);
  #ifdef DRAW
      const TopoDS_Shape& CurE = it.Value();
      DBRep::Set(name, CurE);
  #endif
    }
  }
#endif

  gp_Pnt Origin = BRep_Tool::Pnt(Vertex);

  //// Find the axis of the sphere to exclude
  //// degenerated and seam edges from the face under construction
  BRepLib_MakeWire MW;
  MW.Add(LEdge);
  TopoDS_Wire theWire = MW.Wire();

  ShapeFix_Shape Fixer(theWire);
  Fixer.Perform();
  theWire = TopoDS::Wire(Fixer.Shape());

  GProp_GProps GlobalProps;
  BRepGProp::LinearProperties(theWire, GlobalProps);
  gp_Pnt BaryCenter = GlobalProps.CentreOfMass();
  gp_Vec Xdir(BaryCenter, Origin);

  gp_Pnt FarestCorner = GetFarestCorner(theWire);
  gp_Pln thePlane     = gce_MakePln(Origin, BaryCenter, FarestCorner);
  gp_Dir Vdir         = thePlane.Axis().Direction();

  gp_Ax3 Axis(Origin, Vdir, Xdir);

  occ::handle<Geom_Surface> S = new Geom_SphericalSurface(Axis, std::abs(Offset));

  double f, l, Tol = BRep_Tool::Tolerance(Vertex);

  TopLoc_Location Loc;
  BRep_Builder    myBuilder;
  myBuilder.MakeFace(myFace);
  occ::handle<Geom_Surface> SS = S;

  // En polynomial, calcul de la surface par F(u,v).
  // Pas de changement de parametre, donc ProjLib sur la Sphere
  // reste OK.
  if (Polynomial)
  {
    GeomConvert_ApproxSurface Approx(S, TolApp, Conti, Conti, 10, 10, 10, 1);
    if (Approx.IsDone())
    {
      SS = Approx.Surface();
    }
  }

  myBuilder.UpdateFace(myFace, SS, Loc, Tol);

  TopoDS_Wire W;
  myBuilder.MakeWire(W);

#ifdef DRAW
  // POP pour NT
  //  char name[100];
  if (Affich)
  {
    Sprintf(name, "SPHERE_%d", NbOFFSET);
    DrawTrSurf::Set(name, S);
  }
  int CO = 1;
#endif

  for (it.Initialize(LEdge); it.More(); it.Next())
  {
    TopoDS_Edge E = TopoDS::Edge(it.Value());

    occ::handle<Geom_Curve> C = BRep_Tool::Curve(E, Loc, f, l);
    if (C.IsNull())
    {
      BRepLib::BuildCurve3d(E, BRep_Tool::Tolerance(E));
      C = BRep_Tool::Curve(E, Loc, f, l);
    }
    C = new Geom_TrimmedCurve(C, f, l);
    C->Transform(Loc.Transformation());

#ifdef DRAW
    if (Affich)
    {
      Sprintf(name, "CURVE_%d_%d", NbOFFSET, CO);
      DrawTrSurf::Set(name, C);
      CO++;
    }
#endif

    occ::handle<Geom2d_Curve> PCurve = GeomProjLib::Curve2d(C, S);
    // check if the first point of PCurve in is the canonical boundaries
    // of the sphere. Else move it.
    // the transformation is : U` = U + PI + 2 k  PI
    //                         V` = +/- PI + 2 k` PI
    gp_Pnt2d P2d        = PCurve->Value(f);
    bool     IsToAdjust = false;
    if (P2d.Y() < -M_PI / 2.)
    {
      IsToAdjust = true;
      PCurve->Mirror(gp_Ax2d(gp_Pnt2d(0., -M_PI / 2.), gp::DX2d()));
    }
    else if (P2d.Y() > M_PI / 2.)
    {
      IsToAdjust = true;
      PCurve->Mirror(gp_Ax2d(gp_Pnt2d(0., M_PI / 2.), gp::DX2d()));
    }
    if (IsToAdjust)
    {
      // set the u firstpoint in [0,2*pi]
      gp_Vec2d Tr(M_PI, 0.);
      if (P2d.X() > M_PI)
        Tr.Reverse();
      PCurve->Translate(Tr);
    }

    UpdateEdge(E, PCurve, myFace, Tol);
    myBuilder.Range(E, myFace, f, l);
    myBuilder.Add(W, E);
  }
  if (Offset < 0.)
  {
    myBuilder.Add(myFace, W.Oriented(TopAbs_REVERSED));
    myFace.Reverse();
  }
  else
  {
    myBuilder.Add(myFace, W);
  }

  BRepTools::Update(myFace);
}

//=================================================================================================

void BRepOffset_Offset::Init(const TopoDS_Edge& Edge, const double Offset)
{
  myShape         = Edge;
  double myOffset = std::abs(Offset);

  double          f, l;
  TopLoc_Location Loc;

  occ::handle<Geom_Curve> CP = BRep_Tool::Curve(Edge, Loc, f, l);
  CP                         = new Geom_TrimmedCurve(CP, f, l);
  CP->Transform(Loc.Transformation());

  GeomFill_Pipe Pipe(CP, myOffset);
  Pipe.Perform();
  if (!Pipe.IsDone())
    throw Standard_ConstructionError("GeomFill_Pipe : Cannot make a surface");

  BRepLib_MakeFace MF(Pipe.Surface(), Precision::Confusion());
  myFace = MF.Face();

  if (Offset < 0.)
    myFace.Reverse();
}

//=================================================================================================

const TopoDS_Face& BRepOffset_Offset::Face() const
{
  return myFace;
}

//=================================================================================================

TopoDS_Shape BRepOffset_Offset::Generated(const TopoDS_Shape& Shape) const
{
  TopoDS_Shape aShape;

  switch (myShape.ShapeType())
  {
    case TopAbs_FACE: {
      TopExp_Explorer exp(myShape.Oriented(TopAbs_FORWARD), TopAbs_EDGE);
      TopExp_Explorer expo(myFace.Oriented(TopAbs_FORWARD), TopAbs_EDGE);
      for (; exp.More() && expo.More(); exp.Next(), expo.Next())
      {
        if (Shape.IsSame(exp.Current()))
        {
          if (myShape.Orientation() == TopAbs_REVERSED)
            aShape = expo.Current().Reversed();
          else
            aShape = expo.Current();
          break;
        }
      }
    }
    break;

    case TopAbs_EDGE:
      // have generate a pipe.
      {
        TopoDS_Vertex V1, V2;
        TopExp::Vertices(TopoDS::Edge(myShape), V1, V2);

        TopExp_Explorer expf(myFace.Oriented(TopAbs_FORWARD), TopAbs_WIRE);
        TopExp_Explorer expo(expf.Current().Oriented(TopAbs_FORWARD), TopAbs_EDGE);
        expo.Next();
        expo.Next();

        if (V2.IsSame(Shape))
        {
          if (expf.Current().Orientation() == TopAbs_REVERSED)
            aShape = expo.Current().Reversed();
          else
            aShape = expo.Current();
        }
        else
        {
          expo.Next();
          if (expf.Current().Orientation() == TopAbs_REVERSED)
            aShape = expo.Current().Reversed();
          else
            aShape = expo.Current();
        }
        if (myFace.Orientation() == TopAbs_REVERSED)
          aShape.Reverse();
      }
      break;
    default:
      break;
  }

  return aShape;
}

//=================================================================================================

BRepOffset_Status BRepOffset_Offset::Status() const
{
  return myStatus;
}
