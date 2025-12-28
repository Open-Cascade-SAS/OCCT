// Created on: 1996-01-11
// Created by: Jacques GOUSSARD
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

#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepTools.hxx>
#include <Extrema_ExtCC.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <LocOpe_WiresOnShape.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Map.hxx>
#include <Extrema_ExtCC2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <ShapeAnalysis_Edge.hxx>

#include <NCollection_Sequence.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <Extrema_ExtPS.hxx>

#include <BRepTopAdaptor_FClass2d.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>
#include <ShapeAnalysis.hxx>

IMPLEMENT_STANDARD_RTTIEXT(LocOpe_WiresOnShape, Standard_Transient)

static bool Project(const TopoDS_Vertex&,
                    const gp_Pnt2d&,
                    const TopoDS_Face&,
                    TopoDS_Edge&,
                    double&);

static double Project(const TopoDS_Vertex&, const TopoDS_Edge&);

static double Project(const TopoDS_Vertex&,
                      const gp_Pnt2d&,
                      const TopoDS_Edge&,
                      const TopoDS_Face&);

static void PutPCurve(const TopoDS_Edge&, const TopoDS_Face&);

static void PutPCurves(const TopoDS_Edge&, const TopoDS_Edge&, const TopoDS_Shape&);

static void FindInternalIntersections(const TopoDS_Edge&,
                                      const TopoDS_Face&,
                                      NCollection_IndexedDataMap<TopoDS_Shape,
                                                                 NCollection_List<TopoDS_Shape>,
                                                                 TopTools_ShapeMapHasher>&,
                                      bool&);

//=================================================================================================

LocOpe_WiresOnShape::LocOpe_WiresOnShape(const TopoDS_Shape& S)
    : myShape(S),
      myCheckInterior(true),
      myDone(false),
      myIndex(-1)
{
}

//=================================================================================================

void LocOpe_WiresOnShape::Init(const TopoDS_Shape& S)
{
  myShape         = S;
  myCheckInterior = true;
  myDone          = false;
  myMap.Clear();
  myMapEF.Clear();
}

//=================================================================================================

void LocOpe_WiresOnShape::Bind(const TopoDS_Wire& W, const TopoDS_Face& F)
{
  for (TopExp_Explorer exp(W, TopAbs_EDGE); exp.More(); exp.Next())
  {
    Bind(TopoDS::Edge(exp.Current()), F);
  }
}

//=================================================================================================

void LocOpe_WiresOnShape::Bind(const TopoDS_Compound& Comp, const TopoDS_Face& F)
{
  for (TopExp_Explorer exp(Comp, TopAbs_EDGE); exp.More(); exp.Next())
  {
    Bind(TopoDS::Edge(exp.Current()), F);
  }
  myFacesWithSection.Add(F);
}

//=================================================================================================

void LocOpe_WiresOnShape::Bind(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  //  if (!myMapEF.IsBound(E)) {
  if (!myMapEF.Contains(E))
  {
    //    for (TopExp_Explorer exp(F,TopAbs_EDGE);exp.More();exp.Next()) {
    TopExp_Explorer exp(F, TopAbs_EDGE);
    for (; exp.More(); exp.Next())
    {
      if (exp.Current().IsSame(E))
      {
        break;
      }
    }
    if (!exp.More())
    {
      //      myMapEF.Bind(E,F);
      myMapEF.Add(E, F);
    }
  }
  else
  {
    throw Standard_ConstructionError();
  }
}

//=================================================================================================

void LocOpe_WiresOnShape::Bind(const TopoDS_Edge& Ewir, const TopoDS_Edge& Efac)
{
  myMap.Bind(Ewir, Efac);
}

//=================================================================================================

void LocOpe_WiresOnShape::BindAll()
{
  if (myDone)
  {
    return;
  }
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> theMap;

  // Detection des vertex a projeter ou a "binder" avec des vertex existants
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>           mapV;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator ite(myMap);
  TopExp_Explorer                                                                    exp, exp2;
  for (; ite.More(); ite.Next())
  {
    const TopoDS_Edge& eref = TopoDS::Edge(ite.Key());
    const TopoDS_Edge& eimg = TopoDS::Edge(ite.Value());

    PutPCurves(eref, eimg, myShape);

    for (exp.Init(eref, TopAbs_VERTEX); exp.More(); exp.Next())
    {
      const TopoDS_Vertex& vtx = TopoDS::Vertex(exp.Current());
      if (!theMap.Contains(vtx))
      { // pas deja traite
        for (exp2.Init(eimg, TopAbs_VERTEX); exp2.More(); exp2.Next())
        {
          const TopoDS_Vertex& vtx2 = TopoDS::Vertex(exp2.Current());
          if (vtx2.IsSame(vtx))
          {
            break;
          }
          else if (BRepTools::Compare(vtx, vtx2))
          {
            mapV.Bind(vtx, vtx2);
            break;
          }
        }
        if (!exp2.More())
        {
          mapV.Bind(vtx, eimg);
        }
        theMap.Add(vtx);
      }
    }
  }

  for (ite.Initialize(mapV); ite.More(); ite.Next())
  {
    myMap.Bind(ite.Key(), ite.Value());
  }

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                         Splits;
  int                                                    Ind;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> anOverlappedEdges;
  for (Ind = 1; Ind <= myMapEF.Extent(); Ind++)
  {
    const TopoDS_Edge& edg = TopoDS::Edge(myMapEF.FindKey(Ind));
    const TopoDS_Face& fac = TopoDS::Face(myMapEF(Ind));

    PutPCurve(edg, fac);
    double                    pf, pl;
    occ::handle<Geom2d_Curve> aPCurve = BRep_Tool::CurveOnSurface(edg, fac, pf, pl);
    if (aPCurve.IsNull())
      continue;

    if (myCheckInterior)
    {
      bool isOverlapped = false;
      FindInternalIntersections(edg, fac, Splits, isOverlapped);
      if (isOverlapped)
        anOverlappedEdges.Add(edg);
    }
  }

  for (Ind = 1; Ind <= Splits.Extent(); Ind++)
  {
    TopoDS_Shape anEdge = Splits.FindKey(Ind);
    if (anOverlappedEdges.Contains(anEdge))
      continue;
    TopoDS_Shape aFace = myMapEF.FindFromKey(anEdge);
    // Remove "anEdge" from "myMapEF"
    myMapEF.RemoveKey(anEdge);
    NCollection_List<TopoDS_Shape>::Iterator itl(Splits(Ind));
    for (; itl.More(); itl.Next())
      myMapEF.Add(itl.Value(), aFace);
  }

  NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher> aVertParam;

  for (Ind = 1; Ind <= myMapEF.Extent(); Ind++)
  {
    const TopoDS_Edge& edg = TopoDS::Edge(myMapEF.FindKey(Ind));
    const TopoDS_Face& fac = TopoDS::Face(myMapEF(Ind));
    // JAG 02.02.96 : On verifie les pcurves...

    // PutPCurve(edg,fac);

    for (exp.Init(edg, TopAbs_VERTEX); exp.More(); exp.Next())
    {
      const TopoDS_Vertex& vtx = TopoDS::Vertex(exp.Current());
      if (theMap.Contains(vtx))
      {
        continue;
      }

      double              vtx_param = BRep_Tool::Parameter(vtx, edg);
      BRepAdaptor_Curve2d BAcurve2d(edg, fac);

      gp_Pnt2d p2d =
        (!BAcurve2d.Curve().IsNull() ? BAcurve2d.Value(vtx_param)
                                     : gp_Pnt2d(Precision::Infinite(), Precision::Infinite()));

      TopoDS_Edge Epro;
      double      prm         = Precision::Infinite();
      bool        isProjected = myMap.IsBound(vtx);

      // if vertex was already projected on the current edge on the previous face
      // it is necessary to check tolerance of the vertex in the 2D space on the current
      // face without projection and update tolerance of vertex if it is necessary
      if (isProjected)
      {
        TopoDS_Shape aSh = myMap.Find(vtx);
        if (aSh.ShapeType() != TopAbs_EDGE)
          continue;
        Epro = TopoDS::Edge(myMap.Find(vtx));
        if (aVertParam.IsBound(vtx))
          prm = aVertParam.Find(vtx);
      }
      bool ok = Project(vtx, p2d, fac, Epro, prm);
      if (ok && !isProjected)
      {

        for (exp2.Init(Epro, TopAbs_VERTEX); exp2.More(); exp2.Next())
        {
          const TopoDS_Vertex& vtx2 = TopoDS::Vertex(exp2.Current());
          if (vtx2.IsSame(vtx))
          {
            myMap.Bind(vtx, vtx2);
            theMap.Add(vtx);
            break;
          }
          else if (BRepTools::Compare(vtx, vtx2))
          {
            double aF1, aL1;
            BRep_Tool::Range(Epro, fac, aF1, aL1);
            if (!BRep_Tool::Degenerated(Epro)
                && (std::abs(prm - aF1) <= Precision::PConfusion()
                    || std::abs(prm - aL1) <= Precision::PConfusion()))
            {
              myMap.Bind(vtx, vtx2);
              theMap.Add(vtx);
              break;
            }
          }
        }
        if (!exp2.More())
        {
          myMap.Bind(vtx, Epro);
          aVertParam.Bind(vtx, prm);
        }
      }
    }
  }

  //  Modified by Sergey KHROMOV - Mon Feb 12 16:26:50 2001 Begin
  for (ite.Initialize(myMap); ite.More(); ite.Next())
    if ((ite.Key()).ShapeType() == TopAbs_EDGE)
      myMapEF.Add(ite.Key(), ite.Value());
  //  Modified by Sergey KHROMOV - Mon Feb 12 16:26:52 2001 End

  myDone = true;
}

//=================================================================================================

void LocOpe_WiresOnShape::InitEdgeIterator()
{
  BindAll();
  //  myIt.Initialize(myMapEF);
  myIndex = 1;
}

//=================================================================================================

bool LocOpe_WiresOnShape::MoreEdge()
{
  //  return myIt.More();
  return (myIndex <= myMapEF.Extent());
}

//=================================================================================================

TopoDS_Edge LocOpe_WiresOnShape::Edge()
{
  //  return TopoDS::Edge(myIt.Key());
  return TopoDS::Edge(myMapEF.FindKey(myIndex));
}

//=================================================================================================

TopoDS_Face LocOpe_WiresOnShape::OnFace()
{
  //  return TopoDS::Face(myIt.Value());
  return TopoDS::Face(myMapEF(myIndex));
}

//=================================================================================================

bool LocOpe_WiresOnShape::OnEdge(TopoDS_Edge& E)
{
  //  if (myMap.IsBound(myIt.Key())) {
  if (myMap.IsBound(myMapEF.FindKey(myIndex)))
  {
    //    E = TopoDS::Edge(myMap(myIt.Key()));
    E = TopoDS::Edge(myMap(myMapEF.FindKey(myIndex)));
    return true;
  }
  return false;
}

//=================================================================================================

void LocOpe_WiresOnShape::NextEdge()
{
  //  myIt.Next();
  myIndex++;
}

//=================================================================================================

bool LocOpe_WiresOnShape::OnVertex(const TopoDS_Vertex& Vw, TopoDS_Vertex& Vs)
{
  if (myMap.IsBound(Vw))
  {
    if (myMap(Vw).ShapeType() == TopAbs_VERTEX)
    {
      Vs = TopoDS::Vertex(myMap(Vw));
      return true;
    }
    return false;
  }
  return false;
}

//=================================================================================================

bool LocOpe_WiresOnShape::OnEdge(const TopoDS_Vertex& V, TopoDS_Edge& Ed, double& prm)
{
  if (!myMap.IsBound(V) || myMap(V).ShapeType() == TopAbs_VERTEX)
  {
    return false;
  }

  Ed  = TopoDS::Edge(myMap(V));
  prm = Project(V, Ed);
  return true;
}

//=================================================================================================

bool LocOpe_WiresOnShape::OnEdge(const TopoDS_Vertex& V,
                                 const TopoDS_Edge&   EdgeFrom,
                                 TopoDS_Edge&         Ed,
                                 double&              prm)
{
  if (!myMap.IsBound(V) || myMap(V).ShapeType() == TopAbs_VERTEX)
  {
    return false;
  }

  Ed = TopoDS::Edge(myMap(V));
  if (!myMapEF.Contains(EdgeFrom))
    return false;

  TopoDS_Shape            aShape = myMapEF.FindFromKey(EdgeFrom);
  double                  aF, aL;
  occ::handle<Geom_Curve> aC = BRep_Tool::Curve(Ed, aF, aL);
  if (aC.IsNull() && aShape.ShapeType() == TopAbs_FACE)
  {

    TopoDS_Face         aFace     = TopoDS::Face(aShape);
    double              vtx_param = BRep_Tool::Parameter(V, EdgeFrom);
    BRepAdaptor_Curve2d BAcurve2d(EdgeFrom, aFace);
    gp_Pnt2d            p2d = BAcurve2d.Value(vtx_param);

    prm = Project(V, p2d, Ed, aFace);
  }
  else
    prm = Project(V, TopoDS::Edge(Ed));

  return true;
}

//=================================================================================================

bool Project(const TopoDS_Vertex& V,
             const gp_Pnt2d&      p2d,
             const TopoDS_Face&   F,
             TopoDS_Edge&         theEdge,
             double&              param)
{
  double aTolV = BRep_Tool::Tolerance(V);
  double dmin  = (theEdge.IsNull() ? RealLast() : aTolV * aTolV);

  bool valret = false;

  occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(F);

  if (theEdge.IsNull())
  {
    gp_Pnt toproj(BRep_Tool::Pnt(V));
    for (TopExp_Explorer exp(F.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp.More(); exp.Next())
    {
      const TopoDS_Edge&      edg = TopoDS::Edge(exp.Current());
      double                  f, l;
      occ::handle<Geom_Curve> C        = BRep_Tool::Curve(edg, f, l);
      double                  aCurDist = Precision::Infinite();
      double                  aCurPar  = Precision::Infinite();
      if (!C.IsNull())
      {
        aCurPar = Project(V, edg);
        if (Precision::IsInfinite(aCurPar))
          continue;
        gp_Pnt aCurPBound;
        C->D0(aCurPar, aCurPBound);
        aCurDist = aCurPBound.SquareDistance(toproj);
      }

      else if (!Precision::IsInfinite(p2d.X()))
      {
        // Geom2dAPI_ProjectPointOnCurve proj;
        occ::handle<Geom2d_Curve> aC2d = BRep_Tool::CurveOnSurface(edg, F, f, l);
        if (aC2d.IsNull())
          continue;

        aCurPar = Project(V, p2d, edg, F);
        if (Precision::IsInfinite(aCurPar))
          continue;
        occ::handle<Geom2d_Curve> PC = BRep_Tool::CurveOnSurface(edg, F, f, l);
        gp_Pnt2d                  aPProj;
        PC->D0(aCurPar, aPProj);
        gp_Pnt aCurPBound;
        aSurf->D0(aPProj.X(), aPProj.Y(), aCurPBound);
        aCurDist = aCurPBound.SquareDistance(toproj);
      }

      if (aCurDist < dmin)
      {
        theEdge = edg;
        theEdge.Orientation(edg.Orientation());
        dmin  = aCurDist;
        param = aCurPar;
      }
    }
    if (theEdge.IsNull())
      return false;
  }
  else if (Precision::IsInfinite(param))
  {
    double                  f, l;
    occ::handle<Geom_Curve> C = BRep_Tool::Curve(theEdge, f, l);
    param                     = (!C.IsNull() ? Project(V, theEdge) : Project(V, p2d, theEdge, F));
  }

  double ttol = aTolV + BRep_Tool::Tolerance(theEdge);
  if (dmin <= ttol * ttol)
  {
    valret = true;
    GeomAdaptor_Surface adSurf(aSurf);

    double anUResolution = adSurf.UResolution(1.);
    double aVResolution  = adSurf.UResolution(1.);

    double                    f, l;
    occ::handle<Geom2d_Curve> aCrvBound = BRep_Tool::CurveOnSurface(theEdge, F, f, l);
    if (!aCrvBound.IsNull())
    {
      gp_Pnt2d aPBound2d;
      aCrvBound->D0(param, aPBound2d);

      // distance in 2D space recomputed in the 3D space in order to tolerance of vertex
      // cover gap in 2D space. For consistency with check of the validity in the  BRepCheck_Wire
      double dumax = 0.01 * (adSurf.LastUParameter() - adSurf.FirstUParameter());
      double dvmax = 0.01 * (adSurf.LastVParameter() - adSurf.FirstVParameter());

      gp_Pnt2d aPcur = p2d;
      double   dumin = std::abs(aPcur.X() - aPBound2d.X());
      double   dvmin = std::abs(aPcur.Y() - aPBound2d.Y());
      if (dumin > dumax && adSurf.IsUPeriodic())
      {
        double aX1 = aPBound2d.X();
        double aShift =
          ShapeAnalysis::AdjustToPeriod(aX1, adSurf.FirstUParameter(), adSurf.LastUParameter());
        aX1 += aShift;
        aPBound2d.SetX(aX1);
        double aX2 = p2d.X();
        aShift =
          ShapeAnalysis::AdjustToPeriod(aX2, adSurf.FirstUParameter(), adSurf.LastUParameter());
        aX2 += aShift;
        dumin = std::abs(aX2 - aX1);
        if (dumin > dumax && (std::abs(dumin - adSurf.UPeriod()) < Precision::PConfusion()))
        {
          aX2   = aX1;
          dumin = 0.;
        }
        aPcur.SetX(aX2);
      }

      if (dvmin > dvmax && adSurf.IsVPeriodic())
      {
        double aY1 = aPBound2d.Y();
        double aShift =
          ShapeAnalysis::AdjustToPeriod(aY1, adSurf.FirstVParameter(), adSurf.LastVParameter());
        aY1 += aShift;
        aPBound2d.SetY(aY1);
        double aY2 = p2d.Y();
        aShift =
          ShapeAnalysis::AdjustToPeriod(aY2, adSurf.FirstVParameter(), adSurf.LastVParameter());
        aY2 += aShift;
        dvmin = std::abs(aY1 - aY2);
        if (dvmin > dvmax && (std::abs(dvmin - adSurf.VPeriod()) < Precision::Confusion()))
        {
          aY2   = aY1;
          dvmin = 0.;
        }
        aPcur.SetY(aY2);
      }
      double aDist3d = aTolV;
      if ((dumin > dumax) || (dvmin > dvmax))
      {

        dumax          = adSurf.UResolution(aTolV);
        dvmax          = adSurf.VResolution(aTolV);
        double aTol2d  = 2. * std::max(dumax, dvmax);
        double aDist2d = std::max(dumin, dvmin);

        if (aDist2d > aTol2d)
        {
          double aDist3d1 = aDist2d / std::max(anUResolution, aVResolution);
          if (aDist3d1 > aDist3d)
            aDist3d = aDist3d1;
        }
      }

      // added check by 3D the same as in the BRepCheck_Wire::SelfIntersect
      gp_Pnt aPBound;
      aSurf->D0(aPBound2d.X(), aPBound2d.Y(), aPBound);
      gp_Pnt aPV2d;
      aSurf->D0(p2d.X(), p2d.Y(), aPV2d);
      double aDistPoints_3D = aPV2d.SquareDistance(aPBound);
      double aMaxDist       = std::max(aDistPoints_3D, aDist3d * aDist3d);

      BRep_Builder B;
      if (aTolV * aTolV < aMaxDist)
      {
        double aNewTol = sqrt(aMaxDist);
        B.UpdateVertex(V, aNewTol);
      }
    }
  }
#ifdef OCCT_DEBUG_MESH
  else
  {
    std::cout << "LocOpe_WiresOnShape::Project --> le vertex projete est a une ";
    std::cout << "distance / la face = " << dmin << " superieure a la tolerance = " << ttol
              << std::endl;
  }
#endif
  return valret;
}

//=================================================================================================

double Project(const TopoDS_Vertex& V, const TopoDS_Edge& theEdge)
{
  occ::handle<Geom_Curve> C;
  TopLoc_Location         Loc;
  double                  f, l;

  gp_Pnt                      toproj(BRep_Tool::Pnt(V));
  GeomAPI_ProjectPointOnCurve proj;

  C = BRep_Tool::Curve(theEdge, Loc, f, l);
  if (!Loc.IsIdentity())
  {
    occ::handle<Geom_Geometry> GG = C->Transformed(Loc.Transformation());
    C                             = occ::down_cast<Geom_Curve>(GG);
  }
  proj.Init(toproj, C, f, l);

  return (proj.NbPoints() > 0 ? proj.LowerDistanceParameter() : Precision::Infinite());
}

//=================================================================================================

double Project(const TopoDS_Vertex&,
               const gp_Pnt2d&    p2d,
               const TopoDS_Edge& theEdge,
               const TopoDS_Face& theFace)
{

  occ::handle<Geom2d_Curve> PC;

  double f, l;

  Geom2dAPI_ProjectPointOnCurve proj;

  PC = BRep_Tool::CurveOnSurface(theEdge, theFace, f, l);

  proj.Init(p2d, PC, f, l);

  return proj.NbPoints() > 0 ? proj.LowerDistanceParameter() : Precision::Infinite();
}

//=================================================================================================

void PutPCurve(const TopoDS_Edge& Edg, const TopoDS_Face& Fac)
{
  BRep_Builder    B;
  TopLoc_Location LocFac;

  occ::handle<Geom_Surface>  S    = BRep_Tool::Surface(Fac, LocFac);
  occ::handle<Standard_Type> styp = S->DynamicType();

  if (styp == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    S    = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
    styp = S->DynamicType();
  }

  if (styp == STANDARD_TYPE(Geom_Plane))
  {
    return;
  }

  double Umin, Umax, Vmin, Vmax;
  BRepTools::UVBounds(Fac, Umin, Umax, Vmin, Vmax);

  double f, l;

  occ::handle<Geom2d_Curve> aC2d = BRep_Tool::CurveOnSurface(Edg, Fac, f, l);
  if (!aC2d.IsNull())
  {
    gp_Pnt2d p2d;
    aC2d->D0((f + l) * 0.5, p2d);
    bool IsIn = true;
    if ((p2d.X() < Umin - Precision::PConfusion()) || (p2d.X() > Umax + Precision::PConfusion()))
      IsIn = false;
    if ((p2d.Y() < Vmin - Precision::PConfusion()) || (p2d.Y() > Vmax + Precision::PConfusion()))
      IsIn = false;

    if (IsIn)
      return;
  }

  TopLoc_Location         Loc;
  occ::handle<Geom_Curve> C = BRep_Tool::Curve(Edg, Loc, f, l);
  if (!Loc.IsIdentity())
  {
    occ::handle<Geom_Geometry> GG = C->Transformed(Loc.Transformation());
    C                             = occ::down_cast<Geom_Curve>(GG);
  }

  if (C->DynamicType() != STANDARD_TYPE(Geom_TrimmedCurve))
  {
    C = new Geom_TrimmedCurve(C, f, l);
  }

  S = BRep_Tool::Surface(Fac);

  double        TolFirst = -1, TolLast = -1;
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(Edg, V1, V2);
  if (!V1.IsNull())
    TolFirst = BRep_Tool::Tolerance(V1);
  if (!V2.IsNull())
    TolLast = BRep_Tool::Tolerance(V2);

  constexpr double                     tol2d = Precision::Confusion();
  occ::handle<Geom2d_Curve>            C2d;
  ShapeConstruct_ProjectCurveOnSurface aToolProj;
  aToolProj.Init(S, tol2d);

  aToolProj.Perform(C, f, l, C2d, TolFirst, TolLast);
  if (C2d.IsNull())
  {
    return;
  }

  gp_Pnt2d pf(C2d->Value(f));
  gp_Pnt2d pl(C2d->Value(l));
  gp_Pnt   PF, PL;
  S->D0(pf.X(), pf.Y(), PF);
  S->D0(pl.X(), pl.Y(), PL);
  if (Edg.Orientation() == TopAbs_REVERSED)
  {
    V1 = TopExp::LastVertex(Edg);
    V1.Reverse();
  }
  else
  {
    V1 = TopExp::FirstVertex(Edg);
  }
  if (Edg.Orientation() == TopAbs_REVERSED)
  {
    V2 = TopExp::FirstVertex(Edg);
    V2.Reverse();
  }
  else
  {
    V2 = TopExp::LastVertex(Edg);
  }

  if (!V1.IsNull() && V2.IsNull())
  {
    // Handling of internal vertices
    double old1 = BRep_Tool::Tolerance(V1);
    double old2 = BRep_Tool::Tolerance(V2);
    gp_Pnt pnt1 = BRep_Tool::Pnt(V1);
    gp_Pnt pnt2 = BRep_Tool::Pnt(V2);
    double tol1 = pnt1.Distance(PF);
    double tol2 = pnt2.Distance(PL);
    B.UpdateVertex(V1, std::max(old1, tol1));
    B.UpdateVertex(V2, std::max(old2, tol2));
  }

  if (S->IsUPeriodic())
  {
    double           up      = S->UPeriod();
    constexpr double tolu    = Precision::PConfusion(); // Epsilon(up);
    int              nbtra   = 0;
    double           theUmin = std::min(pf.X(), pl.X());
    double           theUmax = std::max(pf.X(), pl.X());

    if (theUmin < Umin - tolu)
    {
      while (theUmin < Umin - tolu)
      {
        theUmin += up;
        nbtra++;
      }
    }
    else if (theUmax > Umax + tolu)
    {
      while (theUmax > Umax + tolu)
      {
        theUmax -= up;
        nbtra--;
      }
    }

    if (nbtra != 0)
    {
      C2d->Translate(gp_Vec2d(nbtra * up, 0.));
    }
  }

  if (S->IsVPeriodic())
  {
    double           vp      = S->VPeriod();
    constexpr double tolv    = Precision::PConfusion(); // Epsilon(vp);
    int              nbtra   = 0;
    double           theVmin = std::min(pf.Y(), pl.Y());
    double           theVmax = std::max(pf.Y(), pl.Y());

    if (theVmin < Vmin - tolv)
    {
      while (theVmin < Vmin - tolv)
      {
        theVmin += vp;
        theVmax += vp;
        nbtra++;
      }
    }
    else if (theVmax > Vmax + tolv)
    {
      while (theVmax > Vmax + tolv)
      {
        theVmax -= vp;
        theVmin -= vp;
        nbtra--;
      }
    }

    if (nbtra != 0)
    {
      C2d->Translate(gp_Vec2d(0., nbtra * vp));
    }
  }
  B.UpdateEdge(Edg, C2d, Fac, BRep_Tool::Tolerance(Edg));

  B.SameParameter(Edg, false);
  BRepLib::SameParameter(Edg, tol2d);
}

//=================================================================================================

void PutPCurves(const TopoDS_Edge& Efrom, const TopoDS_Edge& Eto, const TopoDS_Shape& myShape)
{

  NCollection_List<TopoDS_Shape> Lfaces;
  TopExp_Explorer                exp, exp2;

  for (exp.Init(myShape, TopAbs_FACE); exp.More(); exp.Next())
  {
    for (exp2.Init(exp.Current(), TopAbs_EDGE); exp2.More(); exp2.Next())
    {
      if (exp2.Current().IsSame(Eto))
      {
        Lfaces.Append(exp.Current());
      }
    }
  }

  if (Lfaces.Extent() != 1 && Lfaces.Extent() != 2)
  {
    throw Standard_ConstructionError();
  }

  // soit bord libre, soit connexite entre 2 faces, eventuellement edge closed

  if (Lfaces.Extent() == 1)
  {
    return; // sera fait par PutPCurve.... on l`espere
  }

  BRep_Builder               B;
  occ::handle<Geom_Surface>  S;
  occ::handle<Standard_Type> styp;
  occ::handle<Geom_Curve>    C;
  double                     Umin, Umax, Vmin, Vmax;
  double                     f, l;
  TopLoc_Location            Loc, LocFac;

  if (!Lfaces.First().IsSame(Lfaces.Last()))
  {
    NCollection_List<TopoDS_Shape>::Iterator itl(Lfaces);
    for (; itl.More(); itl.Next())
    {
      const TopoDS_Face& Fac = TopoDS::Face(itl.Value());

      if (!BRep_Tool::CurveOnSurface(Efrom, Fac, f, l).IsNull())
      {
        continue;
      }
      S    = BRep_Tool::Surface(Fac, LocFac);
      styp = S->DynamicType();
      if (styp == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
      {
        S    = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
        styp = S->DynamicType();
      }
      if (styp == STANDARD_TYPE(Geom_Plane))
      {
        continue;
      }

      BRepTools::UVBounds(Fac, Umin, Umax, Vmin, Vmax);
      C = BRep_Tool::Curve(Efrom, Loc, f, l);
      if (!Loc.IsIdentity())
      {
        occ::handle<Geom_Geometry> GG = C->Transformed(Loc.Transformation());
        C                             = occ::down_cast<Geom_Curve>(GG);
      }

      if (C->DynamicType() != STANDARD_TYPE(Geom_TrimmedCurve))
      {
        C = new Geom_TrimmedCurve(C, f, l);
      }

      S = BRep_Tool::Surface(Fac);

      // Compute the tol2d
      double              tol3d = std::max(BRep_Tool::Tolerance(Efrom), BRep_Tool::Tolerance(Fac));
      GeomAdaptor_Surface Gas(S, Umin, Umax, Vmin, Vmax);
      double              TolU  = Gas.UResolution(tol3d);
      double              TolV  = Gas.VResolution(tol3d);
      double              tol2d = std::max(TolU, TolV);

      occ::handle<Geom2d_Curve> C2d = GeomProjLib::Curve2d(C, S, Umin, Umax, Vmin, Vmax, tol2d);
      if (C2d.IsNull())
        return;

      gp_Pnt2d pf(C2d->Value(f));
      gp_Pnt2d pl(C2d->Value(l));

      if (S->IsUPeriodic())
      {
        double           up      = S->UPeriod();
        constexpr double tolu    = Precision::PConfusion(); // Epsilon(up);
        int              nbtra   = 0;
        double           theUmin = std::min(pf.X(), pl.X());
        double           theUmax = std::max(pf.X(), pl.X());

        if (theUmin < Umin - tolu)
        {
          while (theUmin < Umin - tolu)
          {
            theUmin += up;
            theUmax += up;
            nbtra++;
          }
        }
        else if (theUmax > Umax + tolu)
        {
          while (theUmax > Umax + tolu)
          {
            theUmax -= up;
            theUmin -= up;
            nbtra--;
          }
        }
        /*
            if (theUmin > Umax+tolu) {
              while (theUmin > Umax+tolu) {
                theUmin -= up;
                nbtra--;
              }
            }
            else if (theUmax < Umin-tolu) {
              while (theUmax < Umin-tolu) {
                theUmax += up;
                nbtra++;
              }
            }
        */
        if (nbtra != 0)
        {
          C2d->Translate(gp_Vec2d(nbtra * up, 0.));
        }
      }

      if (S->IsVPeriodic())
      {
        double           vp      = S->VPeriod();
        constexpr double tolv    = Precision::PConfusion(); // Epsilon(vp);
        int              nbtra   = 0;
        double           theVmin = std::min(pf.Y(), pl.Y());
        double           theVmax = std::max(pf.Y(), pl.Y());

        if (theVmin < Vmin - tolv)
        {
          while (theVmin < Vmin - tolv)
          {
            theVmin += vp;
            theVmax += vp;
            nbtra++;
          }
        }
        else if (theVmax > Vmax + tolv)
        {
          while (theVmax > Vmax + tolv)
          {
            theVmax -= vp;
            theVmin -= vp;
            nbtra--;
          }
        }
        /*
            if (theVmin > Vmax+tolv) {
              while (theVmin > Vmax+tolv) {
                theVmin -= vp;
                nbtra--;
              }
            }
            else if (theVmax < Vmin-tolv) {
              while (theVmax < Vmin-tolv) {
                theVmax += vp;
                nbtra++;
              }
            }
        */
        if (nbtra != 0)
        {
          C2d->Translate(gp_Vec2d(0., nbtra * vp));
        }
      }
      B.UpdateEdge(Efrom, C2d, Fac, BRep_Tool::Tolerance(Efrom));
    }
  }

  else
  {
    const TopoDS_Face& Fac = TopoDS::Face(Lfaces.First());
    if (!BRep_Tool::IsClosed(Eto, Fac))
    {
      throw Standard_ConstructionError();
    }

    TopoDS_Shape              aLocalE = Efrom.Oriented(TopAbs_FORWARD);
    TopoDS_Shape              aLocalF = Fac.Oriented(TopAbs_FORWARD);
    occ::handle<Geom2d_Curve> c2dff =
      BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalE), TopoDS::Face(aLocalF), f, l);

    //    occ::handle<Geom2d_Curve> c2dff =
    //      BRep_Tool::CurveOnSurface(TopoDS::Edge(Efrom.Oriented(TopAbs_FORWARD)),
    //				TopoDS::Face(Fac.Oriented(TopAbs_FORWARD)),
    //				f,l);

    aLocalE = Efrom.Oriented(TopAbs_REVERSED);
    aLocalF = Fac.Oriented(TopAbs_FORWARD);
    occ::handle<Geom2d_Curve> c2dfr =
      BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalE), TopoDS::Face(aLocalF), f, l);
    //    occ::handle<Geom2d_Curve> c2dfr =
    //      BRep_Tool::CurveOnSurface(TopoDS::Edge(Efrom.Oriented(TopAbs_REVERSED)),
    //				TopoDS::Face(Fac.Oriented(TopAbs_FORWARD)),
    //				f,l);

    aLocalE = Eto.Oriented(TopAbs_FORWARD);
    aLocalF = Fac.Oriented(TopAbs_FORWARD);
    occ::handle<Geom2d_Curve> c2dtf =
      BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalE), TopoDS::Face(aLocalF), f, l);

    //    occ::handle<Geom2d_Curve> c2dtf =
    //      BRep_Tool::CurveOnSurface(TopoDS::Edge(Eto.Oriented(TopAbs_FORWARD)),
    //				TopoDS::Face(Fac.Oriented(TopAbs_FORWARD)),
    //				f,l);
    aLocalE = Eto.Oriented(TopAbs_REVERSED);
    aLocalF = Fac.Oriented(TopAbs_FORWARD);
    occ::handle<Geom2d_Curve> c2dtr =
      BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalE), TopoDS::Face(aLocalF), f, l);
    //    occ::handle<Geom2d_Curve> c2dtr =
    //      BRep_Tool::CurveOnSurface(TopoDS::Edge(Eto.Oriented(TopAbs_REVERSED)),
    //				TopoDS::Face(Fac.Oriented(TopAbs_FORWARD)),
    //				f,l);

    gp_Pnt2d ptf(c2dtf->Value(f)); // sur courbe frw
    gp_Pnt2d ptr(c2dtr->Value(f)); // sur courbe rev

    bool isoU = (std::abs(ptf.Y() - ptr.Y()) < Epsilon(ptf.X())); // meme V

    // Efrom et Eto dans le meme sens???

    C = BRep_Tool::Curve(Efrom, Loc, f, l);
    if (!Loc.IsIdentity())
    {
      occ::handle<Geom_Geometry> GG = C->Transformed(Loc.Transformation());
      C                             = occ::down_cast<Geom_Curve>(GG);
    }

    gp_Pnt pt;
    gp_Vec d1f, d1t;

    C->D1(f, pt, d1f);

    TopoDS_Vertex       FirstVertex = TopExp::FirstVertex(Efrom);
    double              vtx_param   = BRep_Tool::Parameter(FirstVertex, Efrom);
    BRepAdaptor_Curve2d BAcurve2d(Efrom, Fac);
    gp_Pnt2d            p2d = BAcurve2d.Value(vtx_param);

    double prmproj = Project(TopExp::FirstVertex(Efrom), p2d, Eto, Fac);

    C = BRep_Tool::Curve(Eto, Loc, f, l);
    if (!Loc.IsIdentity())
    {
      occ::handle<Geom_Geometry> GG = C->Transformed(Loc.Transformation());
      C                             = occ::down_cast<Geom_Curve>(GG);
    }

    C->D1(prmproj, pt, d1t);

    double SameOri = (d1t.Dot(d1f) > 0.);

    if (c2dff.IsNull() && c2dfr.IsNull())
    {
      S    = BRep_Tool::Surface(Fac);
      styp = S->DynamicType();
      if (styp == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
      {
        S    = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
        styp = S->DynamicType();
      }

      C = BRep_Tool::Curve(Efrom, Loc, f, l);
      if (!Loc.IsIdentity())
      {
        occ::handle<Geom_Geometry> GG = C->Transformed(Loc.Transformation());
        C                             = occ::down_cast<Geom_Curve>(GG);
      }

      if (C->DynamicType() != STANDARD_TYPE(Geom_TrimmedCurve))
      {
        C = new Geom_TrimmedCurve(C, f, l);
      }

      // Compute the tol2d
      BRepTools::UVBounds(Fac, Umin, Umax, Vmin, Vmax);

      double              tol3d = std::max(BRep_Tool::Tolerance(Efrom), BRep_Tool::Tolerance(Fac));
      GeomAdaptor_Surface Gas(S, Umin, Umax, Vmin, Vmax);
      double              TolU  = Gas.UResolution(tol3d);
      double              TolV  = Gas.VResolution(tol3d);
      double              tol2d = std::max(TolU, TolV);

      occ::handle<Geom2d_Curve> C2d = GeomProjLib::Curve2d(C, S, Umin, Umax, Vmin, Vmax, tol2d);
      c2dff                         = C2d;
      c2dfr                         = C2d;
    }
    else if (c2dfr.IsNull())
    {
      c2dfr = c2dff;
    }
    else if (c2dff.IsNull())
    {
      c2dff = c2dfr;
    }

    BRep_Tool::Range(Efrom, f, l);

    gp_Pnt2d p2f = c2dff->Value(f);
    gp_Pnt2d p2r = c2dfr->Value(f);

    if (isoU)
    {
      if (SameOri)
      {
        if (std::abs(ptf.X() - p2f.X()) > Epsilon(ptf.X()))
        {
          c2dff = occ::down_cast<Geom2d_Curve>(c2dff->Translated(gp_Vec2d(ptf.X() - p2f.X(), 0.)));
        }
        if (std::abs(ptr.X() - p2r.X()) > Epsilon(ptr.X()))
        {
          c2dfr = occ::down_cast<Geom2d_Curve>(c2dfr->Translated(gp_Vec2d(ptr.X() - p2r.X(), 0.)));
        }
      }
      else
      {
        if (std::abs(ptr.X() - p2f.X()) > Epsilon(ptr.X()))
        {
          c2dff = occ::down_cast<Geom2d_Curve>(c2dff->Translated(gp_Vec2d(ptr.X() - p2f.X(), 0.)));
        }

        if (std::abs(ptf.X() - p2r.X()) > Epsilon(ptf.X()))
        {
          c2dfr = occ::down_cast<Geom2d_Curve>(c2dfr->Translated(gp_Vec2d(ptf.X() - p2r.X(), 0.)));
        }
      }

      // on est bien en U, recalage si periodique en V a faire
    }

    else
    { // !isoU soit isoV
      if (SameOri)
      {
        if (std::abs(ptf.Y() - p2f.Y()) > Epsilon(ptf.Y()))
        {
          c2dff = occ::down_cast<Geom2d_Curve>(c2dff->Translated(gp_Vec2d(0., ptf.Y() - p2f.Y())));
        }
        if (std::abs(ptr.Y() - p2r.Y()) > Epsilon(ptr.Y()))
        {
          c2dfr = occ::down_cast<Geom2d_Curve>(c2dfr->Translated(gp_Vec2d(0., ptr.Y() - p2r.Y())));
        }
      }
      else
      {
        if (std::abs(ptr.Y() - p2f.Y()) > Epsilon(ptr.Y()))
        {
          c2dff = occ::down_cast<Geom2d_Curve>(c2dff->Translated(gp_Vec2d(0., ptr.Y() - p2f.Y())));
        }
        if (std::abs(ptf.Y() - p2r.Y()) > Epsilon(ptf.Y()))
        {
          c2dfr = occ::down_cast<Geom2d_Curve>(c2dfr->Translated(gp_Vec2d(0., ptf.Y() - p2r.Y())));
        }
      }
      // on est bien en V, recalage si periodique en U a faire
    }
    B.UpdateEdge(Efrom, c2dff, c2dfr, Fac, BRep_Tool::Tolerance(Efrom));
  }
}

//=================================================================================================

void FindInternalIntersections(
  const TopoDS_Edge& theEdge,
  const TopoDS_Face& theFace,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
        Splits,
  bool& isOverlapped)
{
  constexpr double TolExt = Precision::PConfusion();
  int              i, j;

  BRepAdaptor_Surface          anAdSurf(theFace, false);
  NCollection_Sequence<double> SplitPars;

  TopoDS_Vertex theVertices[2];
  TopExp::Vertices(theEdge, theVertices[0], theVertices[1]);
  gp_Pnt thePnt[2];
  thePnt[0] = BRep_Tool::Pnt(theVertices[0]);
  thePnt[1] = BRep_Tool::Pnt(theVertices[1]);
  double aTolV[2];
  aTolV[0] = BRep_Tool::Tolerance(theVertices[0]);
  aTolV[1] = BRep_Tool::Tolerance(theVertices[1]);
  // clang-format off
  double ext = 16.; // = 4 * 4 - to avoid creating microedges, area around vertices is increased
                           // up to 4 vertex tolerance. Such approach is usual for other topological
                           // algorithms, for example, Boolean Operations.
  // clang-format on
  double aTolVExt[2] = {ext * aTolV[0] * aTolV[0], ext * aTolV[1] * aTolV[1]};

  BRepAdaptor_Curve2d thePCurve(theEdge, theFace);
  Bnd_Box2d           theBox;
  BndLib_Add2dCurve::Add(thePCurve, BRep_Tool::Tolerance(theEdge), theBox);

  double                         thePar[2];
  double                         aFpar, aLpar;
  const occ::handle<Geom_Curve>& theCurve = BRep_Tool::Curve(theEdge, thePar[0], thePar[1]);
  GeomAdaptor_Curve              theGAcurve(theCurve, thePar[0], thePar[1]);
  double aTolV2d[2]        = {theGAcurve.Resolution(aTolV[0]), theGAcurve.Resolution(aTolV[1])};
  aTolV2d[0]               = std::max(aTolV2d[0], Precision::PConfusion());
  aTolV2d[1]               = std::max(aTolV2d[1], Precision::PConfusion());
  double          aDistMax = Precision::Confusion() * Precision::Confusion();
  TopExp_Explorer Explo(theFace, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Edge&  anEdge = TopoDS::Edge(Explo.Current());
    BRepAdaptor_Curve2d aPCurve(anEdge, theFace);
    Bnd_Box2d           aBox;
    BndLib_Add2dCurve::Add(aPCurve, BRep_Tool::Tolerance(anEdge), aBox);
    if (theBox.IsOut(aBox))
      continue;

    const occ::handle<Geom_Curve>& aCurve = BRep_Tool::Curve(anEdge, aFpar, aLpar);
    GeomAdaptor_Curve              aGAcurve(aCurve, aFpar, aLpar);
    Extrema_ExtCC                  anExtrema(theGAcurve, aGAcurve, TolExt, TolExt);

    if (!anExtrema.IsDone() || !anExtrema.NbExt())
      continue;

    int    aNbExt   = anExtrema.NbExt();
    double MaxTol   = BRep_Tool::Tolerance(anEdge);
    double aMaxTol2 = MaxTol * MaxTol;
    if (anExtrema.IsParallel() && anExtrema.SquareDistance(1) <= aMaxTol2)
    {
      isOverlapped = true;
      return;
    }
    // Check extremity distances
    double dists[4];
    gp_Pnt aP11, aP12, aP21, aP22;
    anExtrema
      .TrimmedSquareDistances(dists[0], dists[1], dists[2], dists[3], aP11, aP12, aP21, aP22);
    for (i = 0; i < 4; ++i)
    {
      if (i < 2)
        j = 0;
      else
        j = 1;
      if (dists[i] < aTolVExt[j] / ext)
      {
        return;
      }
    }

    for (i = 1; i <= aNbExt; i++)
    {
      double aDist = anExtrema.SquareDistance(i);
      if (aDist > aMaxTol2)
        continue;

      Extrema_POnCurv aPOnC1, aPOnC2;
      anExtrema.Points(i, aPOnC1, aPOnC2);
      double theIntPar = aPOnC1.Parameter();
      double anIntPar  = aPOnC2.Parameter();
      for (j = 0; j < 2; j++) // try to find intersection on an extremity of "theEdge"
      {
        if (std::abs(theIntPar - thePar[j]) <= aTolV2d[j])
          break;
      }
      // intersection found in the middle of the edge
      if (j >= 2) // intersection is inside "theEdge" => split
      {
        gp_Pnt aPoint    = aCurve->Value(anIntPar);
        gp_Pnt aPointInt = theCurve->Value(theIntPar);

        if (aPointInt.SquareDistance(thePnt[0]) > aTolVExt[0]
            && aPointInt.SquareDistance(thePnt[1]) > aTolVExt[1]
            && aPoint.SquareDistance(thePnt[0]) > aTolVExt[0]
            && aPoint.SquareDistance(thePnt[1]) > aTolVExt[1])
        {
          SplitPars.Append(theIntPar);
          if (aDist > aDistMax)
            aDistMax = aDist;
        }
      }
    }
  }

  if (SplitPars.IsEmpty())
    return;

  // Sort
  for (i = 1; i < SplitPars.Length(); i++)
    for (j = i + 1; j <= SplitPars.Length(); j++)
      if (SplitPars(i) > SplitPars(j))
      {
        double Tmp   = SplitPars(i);
        SplitPars(i) = SplitPars(j);
        SplitPars(j) = Tmp;
      }
  // Remove repeating points
  i = 1;
  while (i < SplitPars.Length())
  {
    gp_Pnt Pnt1 = theCurve->Value(SplitPars(i));
    gp_Pnt Pnt2 = theCurve->Value(SplitPars(i + 1));
    if (Pnt1.SquareDistance(Pnt2) <= Precision::Confusion() * Precision::Confusion())
      SplitPars.Remove(i + 1);
    else
      i++;
  }

  // Split
  NCollection_List<TopoDS_Shape> NewEdges;
  BRep_Builder                   BB;

  TopoDS_Vertex FirstVertex = theVertices[0], LastVertex;
  double        FirstPar    = thePar[0], LastPar;
  for (i = 1; i <= SplitPars.Length() + 1; i++)
  {
    FirstVertex.Orientation(TopAbs_FORWARD);
    if (i <= SplitPars.Length())
    {
      LastPar          = SplitPars(i);
      gp_Pnt LastPoint = theCurve->Value(LastPar);
      LastVertex       = BRepLib_MakeVertex(LastPoint);
      BRep_Builder aB;
      aB.UpdateVertex(LastVertex, sqrt(aDistMax));
    }
    else
    {
      LastPar    = thePar[1];
      LastVertex = theVertices[1];
    }
    LastVertex.Orientation(TopAbs_REVERSED);

    TopoDS_Shape       aLocalShape = theEdge.EmptyCopied();
    TopAbs_Orientation anOrient    = aLocalShape.Orientation();
    aLocalShape.Orientation(TopAbs_FORWARD);
    TopoDS_Edge NewEdge = TopoDS::Edge(aLocalShape);
    BB.Range(NewEdge, FirstPar, LastPar);
    BB.Add(NewEdge, FirstVertex);
    BB.Add(NewEdge, LastVertex);
    NewEdge.Orientation(anOrient);
    ShapeAnalysis_Edge aSae;
    double             amaxdev = 0.;
    if (aSae.CheckSameParameter(NewEdge, theFace, amaxdev))
    {
      BRep_Builder aB;
      aB.UpdateEdge(NewEdge, amaxdev);
    }

    if (anOrient == TopAbs_FORWARD)
      NewEdges.Append(NewEdge);
    else
      NewEdges.Prepend(NewEdge);
    FirstVertex = LastVertex;
    FirstPar    = LastPar;
  }

  if (!NewEdges.IsEmpty())
    Splits.Add(theEdge, NewEdges);
}

//=================================================================================================

bool LocOpe_WiresOnShape::Add(const NCollection_Sequence<TopoDS_Shape>& theEdges)
{
  NCollection_Sequence<TopoDS_Shape> anEdges;
  int                                i = 1, nb = theEdges.Length();
  NCollection_Array1<Bnd_Box>        anEdgeBoxes(1, nb);
  for (; i <= nb; i++)
  {
    const TopoDS_Shape& aCurSplit = theEdges(i);
    TopExp_Explorer     anExpE(aCurSplit, TopAbs_EDGE);
    for (; anExpE.More(); anExpE.Next())
    {
      const TopoDS_Shape& aCurE = anExpE.Current();

      Bnd_Box aBoxE;
      BRepBndLib::AddClose(aCurE, aBoxE);
      if (aBoxE.IsVoid())
        continue;
      double aTolE = BRep_Tool::Tolerance(TopoDS::Edge(aCurE));
      aBoxE.SetGap(aTolE);
      anEdgeBoxes.SetValue(i, aBoxE);
      anEdges.Append(aCurE);
    }
  }
  TopExp_Explorer            anExpFaces(myShape, TopAbs_FACE);
  int                        numF = 1;
  TColStd_PackedMapOfInteger anUsedEdges;
  for (; anExpFaces.More(); anExpFaces.Next(), numF++)
  {
    const TopoDS_Face& aCurF = TopoDS::Face(anExpFaces.Current());
    Bnd_Box            aBoxF;
    BRepBndLib::Add(aCurF, aBoxF);
    if (aBoxF.IsVoid())
      continue;
    BRepAdaptor_Surface                         anAdF(aCurF, false);
    NCollection_Handle<BRepTopAdaptor_FClass2d> aCheckStateTool;

    i  = 1;
    nb = anEdgeBoxes.Length();
    for (; i <= nb; i++)
    {
      if (anUsedEdges.Contains(i))
        continue;

      if (aBoxF.IsOut(anEdgeBoxes(i)))
        continue;

      const TopoDS_Edge& aCurE = TopoDS::Edge(anEdges(i));

      double                  aF, aL;
      occ::handle<Geom_Curve> aC = BRep_Tool::Curve(aCurE, aF, aL);
      if (aC.IsNull())
      {
        anUsedEdges.Add(i);
        continue;
      }
      gp_Pnt        aP = aC->Value((aF + aL) * 0.5);
      Extrema_ExtPS anExtr(aP, anAdF, Precision::Confusion(), Precision::Confusion());

      if (!anExtr.IsDone() || !anExtr.NbExt())
        continue;
      double aTolE = BRep_Tool::Tolerance(TopoDS::Edge(aCurE));
      double aTol2 = (aTolE + Precision::Confusion()) * (aTolE + Precision::Confusion());
      int    n     = 1;
      for (; n <= anExtr.NbExt(); n++)
      {
        double aDist2 = anExtr.SquareDistance(n);
        if (aDist2 > aTol2)
          continue;
        const Extrema_POnSurf& aPS = anExtr.Point(n);
        double                 aU, aV;
        aPS.Parameter(aU, aV);

        if (aCheckStateTool.IsNull())
        {
          aCheckStateTool = new BRepTopAdaptor_FClass2d(aCurF, Precision::PConfusion());
        }
        if (aCheckStateTool->Perform(gp_Pnt2d(aU, aV)) == TopAbs_IN)
        {
          Bind(aCurE, aCurF);
          anUsedEdges.Add(i);
        }
      }
    }
  }
  return !anUsedEdges.IsEmpty();
}
