// Created on: 1995-09-01
// Created by: Yves FRICAUD
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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopOpeBRepBuild_WireToFace.hxx>

//=================================================================================================

BRepAlgo_FaceRestrictor::BRepAlgo_FaceRestrictor() = default;

//=================================================================================================

void BRepAlgo_FaceRestrictor::Init(const TopoDS_Face& F,
                                   const bool         Proj,
                                   const bool         CorrectionOrientation)
{
  myFace       = F;
  modeProj     = Proj;
  myCorrection = CorrectionOrientation;
}

//=================================================================================================

void BRepAlgo_FaceRestrictor::Add(TopoDS_Wire& W)
{
  wires.Append(W);
}

//=================================================================================================

void BRepAlgo_FaceRestrictor::Clear()
{
  wires.Clear();
  faces.Clear();
}

//=================================================================================================

static bool ChangePCurve(TopoDS_Edge& E, const occ::handle<Geom_Surface>& S, TopLoc_Location& L)
{
  BRep_Builder              BB;
  occ::handle<Geom_Surface> SE;
  occ::handle<Geom2d_Curve> C2;
  TopLoc_Location           LE;
  double                    f, l;

  BRep_Tool::CurveOnSurface(E, C2, SE, LE, f, l, 1);
  if (!C2.IsNull())
    BB.UpdateEdge(E, C2, S, L, Precision::Confusion());
  return (C2.IsNull());
}

//=================================================================================================

static bool ProjCurve3d(TopoDS_Edge& E, const occ::handle<Geom_Surface>& S, TopLoc_Location& L)
{
  BRep_Builder            BB;
  TopLoc_Location         LE;
  double                  f, l;
  occ::handle<Geom_Curve> C = BRep_Tool::Curve(E, LE, f, l);
  if (C.IsNull())
  {
    return false;
  }
  occ::handle<Geom_TrimmedCurve> CT = new Geom_TrimmedCurve(C, f, l);

  TopLoc_Location LL = L.Inverted().Multiplied(LE);
  CT->Transform(LL.Transformation());

  occ::handle<Geom2d_Curve> C2 = GeomProjLib::Curve2d(CT, S);
  BB.UpdateEdge(E, C2, S, L, Precision::Confusion());
  return true;
}

//=================================================================================================

void BRepAlgo_FaceRestrictor::Perform()
{

  if (myCorrection)
  {
    PerformWithCorrection();
    return;
  }

  myDone = false;
  NCollection_List<TopoDS_Shape>::Iterator it(wires);

  //--------------------------------------------------------------------
  // return geometry of the reference face.
  //--------------------------------------------------------------------
  TopLoc_Location                  L;
  const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(myFace, L);

  //-----------------------------------------------------------------------
  // test if edges are on S. otherwise  add S to the first pcurve.
  // or projection of the edge on F.
  //----------------------------------------------------------------------
  TopExp_Explorer Exp;
  //  BRep_Builder    BB;
  double f, l;

  TopOpeBRepBuild_WireToFace WTF;

  for (; it.More(); it.Next())
  {
    // update the surface on edges.
    const TopoDS_Wire& W = TopoDS::Wire(it.Value());

    for (Exp.Init(W, TopAbs_EDGE); Exp.More(); Exp.Next())
    {

      TopoDS_Edge               E  = TopoDS::Edge(Exp.Current());
      occ::handle<Geom2d_Curve> C2 = BRep_Tool::CurveOnSurface(E, S, L, f, l);

      if (C2.IsNull())
      {
        // no pcurve on the reference surface.
        if (modeProj)
        {
          // Projection of the 3D curve on surface.
          if (!ProjCurve3d(E, S, L))
          {
            return;
          }
        }
        else
        {
          // return the first pcurve glued on <S>
          bool YaPCurve = ChangePCurve(E, S, L);
          if (!YaPCurve)
          {
            if (!ProjCurve3d(E, S, L))
            {
              return;
            }
          }
        }
      }
    }
    WTF.AddWire(W);
  }

  WTF.MakeFaces(myFace, faces);

  myDone = true;
}

//=================================================================================================

bool BRepAlgo_FaceRestrictor::IsDone() const
{
  return myDone;
}

//=================================================================================================

bool BRepAlgo_FaceRestrictor::More() const
{
  return (!faces.IsEmpty());
}

//=================================================================================================

void BRepAlgo_FaceRestrictor::Next()
{
  faces.RemoveFirst();
}

//=================================================================================================

TopoDS_Face BRepAlgo_FaceRestrictor::Current() const
{
  return (TopoDS::Face(faces.First()));
}

//=================================================================================================

static bool IsClosed(const TopoDS_Wire& W)

{
  if (W.Closed())
    return true;
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(W, V1, V2);
  return (V1.IsSame(V2));
}

//=================================================================================================

static bool IsInside(const TopoDS_Wire& wir,
                     const TopoDS_Face& F,
                     BRepTopAdaptor_FClass2d& /*FClass2d*/)
{
  TopExp_Explorer exp;
  exp.Init(wir, TopAbs_EDGE);
  if (exp.More())
  {
    const TopoDS_Edge&        edg = TopoDS::Edge(exp.Current());
    double                    f, l;
    occ::handle<Geom2d_Curve> C2d = BRep_Tool::CurveOnSurface(edg, F, f, l);
    double                    prm;

    if (!Precision::IsNegativeInfinite(f) && !Precision::IsPositiveInfinite(l))
    {
      prm = (f + l) / 2.;
    }
    else
    {
      if (Precision::IsNegativeInfinite(f) && Precision::IsPositiveInfinite(l))
      {
        prm = 0.;
      }
      else if (Precision::IsNegativeInfinite(f))
      {
        prm = l - 1.;
      }
      else
      {
        prm = f + 1.;
      }
    }

    gp_Pnt2d                pt2d(C2d->Value(prm));
    BRepTopAdaptor_FClass2d FClass2d(F, Precision::PConfusion());
    TopAbs_State            st2 = FClass2d.Perform(pt2d, false);
    return (st2 == TopAbs_IN);
  }
  return false;
}

//=================================================================================================

static void Store(
  const TopoDS_Wire& W2,
  const TopoDS_Wire& W1,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    keyIsIn,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    keyContains)
{
  if (!keyIsIn.IsBound(W2))
  {
    NCollection_List<TopoDS_Shape> empty;
    keyIsIn.Bind(W2, empty);
  }
  keyIsIn(W2).Append(W1);
  if (!keyContains.IsBound(W1))
  {
    NCollection_List<TopoDS_Shape> empty;
    keyContains.Bind(W1, empty);
  }
  keyContains(W1).Append(W2);
}

//=================================================================================================

static void BuildFaceIn(
  TopoDS_Face&       F,
  const TopoDS_Wire& W,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    KeyContains,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                  KeyIsIn,
  TopAbs_Orientation              Orientation,
  NCollection_List<TopoDS_Shape>& Faces)
{
  BRep_Builder B;

  if (!KeyContains.IsBound(W) || KeyContains(W).IsEmpty())
    return;

  // Removal of W in KeyIsIn.
  //  for (NCollection_List<TopoDS_Shape>::Iterator it(KeyContains(W)); it.More(); it.Next()) {

  NCollection_List<TopoDS_Shape>::Iterator it;
  for (it.Initialize(KeyContains(W)); it.More(); it.Next())
  {
    const TopoDS_Wire&                       WI = TopoDS::Wire(it.Value());
    NCollection_List<TopoDS_Shape>&          L2 = KeyIsIn(WI);
    NCollection_List<TopoDS_Shape>::Iterator it2;
    for (it2.Initialize(L2); it2.More(); it2.Next())
    {
      if (it2.Value().IsSame(W))
      {
        L2.Remove(it2);
        break;
      }
    }
  }

  NCollection_List<TopoDS_Shape> WireExt;

  for (it.Initialize(KeyContains(W)); it.More(); it.Next())
  {
    const TopoDS_Wire&              WI = TopoDS::Wire(it.Value());
    NCollection_List<TopoDS_Shape>& L2 = KeyIsIn(WI);

    if (L2.IsEmpty())
    {
      WireExt.Append(WI);
    }
  }

  for (it.Initialize(WireExt); it.More(); it.Next())
  {
    const TopoDS_Wire&              WI = TopoDS::Wire(it.Value());
    NCollection_List<TopoDS_Shape>& L2 = KeyIsIn(WI);
    if (L2.IsEmpty())
    {
      if (Orientation == TopAbs_FORWARD)
      {
        TopoDS_Wire NWI(WI);
        NWI.Reverse();
        // TopoDS_Wire NWI = TopoDS::Wire(WI.Reversed());
        B.Add(F, NWI);
        BuildFaceIn(F, WI, KeyContains, KeyIsIn, TopAbs_REVERSED, Faces);
      }
      else
      {
        TopoDS_Shape aLocalShape = Faces.First().EmptyCopied();
        TopoDS_Face  NF          = TopoDS::Face(aLocalShape);
        // TopoDS_Face NF = TopoDS::Face(Faces.First().EmptyCopied());
        B.Add(NF, WI);
        Faces.Append(NF);
        BuildFaceIn(NF, WI, KeyContains, KeyIsIn, TopAbs_FORWARD, Faces);
      }
    }
  }
}

//=================================================================================================

void BRepAlgo_FaceRestrictor::PerformWithCorrection()
{
  BRep_Builder B;

  myDone = false;
  NCollection_List<TopoDS_Shape>::Iterator it(wires);
  //---------------------------------------------------------
  // Reorientation of all closed wires to the left.
  //---------------------------------------------------------
  for (; it.More(); it.Next())
  {
    TopoDS_Wire& W           = TopoDS::Wire(it.ChangeValue());
    TopoDS_Shape aLocalShape = myFace.EmptyCopied();
    TopoDS_Face  NF          = TopoDS::Face(aLocalShape);
    //    TopoDS_Face  NF = TopoDS::Face(myFace.EmptyCopied());
    NF.Orientation(TopAbs_FORWARD);
    B.Add(NF, W);

    if (IsClosed(W))
    {
      BRepTopAdaptor_FClass2d FClass2d(NF, Precision::PConfusion());
      if (FClass2d.PerformInfinitePoint() != TopAbs_OUT)
      {
        W.Reverse();
      }
    }
  }
  //---------------------------------------------------------
  // Classification of wires ones compared to the others.
  //---------------------------------------------------------
  for (it.Initialize(wires); it.More(); it.Next())
  {
    const TopoDS_Wire&                       W1 = TopoDS::Wire(it.Value());
    NCollection_List<TopoDS_Shape>::Iterator it2(wires);

    if (IsClosed(W1))
    {
      TopoDS_Shape aLocalShape = myFace.EmptyCopied();
      TopoDS_Face  NF          = TopoDS::Face(aLocalShape);
      //      TopoDS_Face  NF = TopoDS::Face(myFace.EmptyCopied());
      NF.Orientation(TopAbs_FORWARD);
      B.Add(NF, W1);

      BRepTopAdaptor_FClass2d FClass2d(NF, Precision::PConfusion());
      while (it2.More())
      {
        const TopoDS_Wire& W2 = TopoDS::Wire(it2.Value());
        if (!W1.IsSame(W2) && IsInside(W2, NF, FClass2d))
        {
          Store(W2, W1, keyIsIn, keyContains);
        }
        it2.Next();
      }
    }
  }
  NCollection_List<TopoDS_Shape> WireExt;

  for (it.Initialize(wires); it.More(); it.Next())
  {
    const TopoDS_Wire& W = TopoDS::Wire(it.Value());
    if (!keyIsIn.IsBound(W) || keyIsIn(W).IsEmpty())
    {
      WireExt.Append(W);
    }
  }

  for (it.Initialize(WireExt); it.More(); it.Next())
  {
    const TopoDS_Wire& W = TopoDS::Wire(it.Value());
    if (!keyIsIn.IsBound(W) || keyIsIn(W).IsEmpty())
    {
      TopoDS_Shape aLocalShape = myFace.EmptyCopied();
      TopoDS_Face  NewFace     = TopoDS::Face(aLocalShape);
      //      TopoDS_Face NewFace = TopoDS::Face(myFace.EmptyCopied());
      NewFace.Orientation(TopAbs_FORWARD);
      B.Add(NewFace, W);
      faces.Append(NewFace);
      //--------------------------------------------
      // Construction of a face by exterior wire.
      //--------------------------------------------
      BuildFaceIn(NewFace, W, keyContains, keyIsIn, TopAbs_FORWARD, faces);
    }
  }
  myDone = true;
}
