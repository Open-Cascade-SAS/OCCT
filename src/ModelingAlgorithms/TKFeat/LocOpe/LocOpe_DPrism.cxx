// Created on: 1996-09-04
// Created by: Olga PILLOT
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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <LocOpe.hxx>
#include <LocOpe_BuildShape.hxx>
#include <LocOpe_DPrism.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

#ifdef OCCT_DEBUG
extern bool BRepFeat_GettraceFEAT();
#endif

#define NECHANT 7 // voir BRepFeat.cxx

//=================================================================================================

LocOpe_DPrism::LocOpe_DPrism(const TopoDS_Face&  Spine,
                             const double Height1,
                             const double Height2,
                             const double Angle)
    : mySpine(Spine)
{
  int i;

  myHeight        = Height1 + Height2;
  double y = Height1 * sin(Angle);
  double z = Height1 * cos(Angle);

  TopoDS_Vertex Vert2 = BRepLib_MakeVertex(gp_Pnt(0, y, z));

  double y1 = -Height2 * sin(Angle);
  double z1 = -Height2 * cos(Angle);

  TopoDS_Vertex Vert1 = BRepLib_MakeVertex(gp_Pnt(0, y1, z1));

  myProfile2 = BRepLib_MakeEdge(Vert1, Vert2);

  double Umin, Umax, Vmin, Vmax;
  Umax = 0.;
  Umin = 0.;
  Vmin = 0.;
  Vmax = 0.;

  BRepTools::UVBounds(Spine, Umin, Umax, Vmin, Vmax);
  double Deltay = std::max(Umax - Umin, Vmax - Vmin) + std::abs(y);
  Deltay *= 2;

  TopoDS_Vertex Vert3 = BRepLib_MakeVertex(gp_Pnt(0, y + Deltay, z));
  myProfile3          = BRepLib_MakeEdge(Vert2, Vert3);

  Umax = 0.;
  Umin = 0.;
  Vmin = 0.;
  Vmax = 0.;

  BRepTools::UVBounds(Spine, Umin, Umax, Vmin, Vmax);
  double Deltay1 = std::max(Umax - Umin, Vmax - Vmin) + std::abs(y1);
  Deltay1 *= 2;

  TopoDS_Vertex Vert4 = BRepLib_MakeVertex(gp_Pnt(0, y1 + Deltay1, z1));
  myProfile1          = BRepLib_MakeEdge(Vert4, Vert1);

  myProfile = BRepLib_MakeWire(myProfile1, myProfile2, myProfile3);

  myDPrism.Perform(mySpine, myProfile, gp::XOY());

  if (myDPrism.IsDone())
  {
    LocOpe_BuildShape    BS;
    BRep_Builder         B;
    TopoDS_Compound      C;
    TopoDS_Compound      D;
    NCollection_List<TopoDS_Shape> lfaces, lcomplete;

    B.MakeCompound(C);
    NCollection_List<TopoDS_Shape>::Iterator it;
    TopExp_Explorer                    ExpS(mySpine, TopAbs_EDGE);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                View;
    for (; ExpS.More(); ExpS.Next())
    {
      const TopoDS_Shape&         ES   = ExpS.Current();
      const NCollection_List<TopoDS_Shape>& lffs = myDPrism.GeneratedShapes(ES, myProfile1);
      for (it.Initialize(lffs); it.More(); it.Next())
      {
        if (View.Add(it.Value()))
          B.Add(C, it.Value());
      }
    }

    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> theMapEF;
    TopExp::MapShapesAndAncestors(C, TopAbs_EDGE, TopAbs_FACE, theMapEF);
    View.Clear();

    for (i = 1; i <= theMapEF.Extent(); i++)
    {
      if (theMapEF(i).Extent() == 1)
      {
        const TopoDS_Edge& edg = TopoDS::Edge(theMapEF.FindKey(i));
        const TopoDS_Face& fac = TopoDS::Face(theMapEF(i).First());
        if (View.Add(fac))
        {
          TopoDS_Shape aLocalShape = fac.EmptyCopied();
          TopoDS_Face  newFace(TopoDS::Face(aLocalShape));
          //	  TopoDS_Face newFace(TopoDS::Face(fac.EmptyCopied()));
          TopExp_Explorer exp;
          for (exp.Init(fac.Oriented(TopAbs_FORWARD), TopAbs_WIRE); exp.More(); exp.Next())
          {
            //	    for (TopExp_Explorer exp2(exp.Current(),TopAbs_EDGE);
            TopExp_Explorer exp2(exp.Current(), TopAbs_EDGE);
            for (; exp2.More(); exp2.Next())
            {
              if (exp2.Current().IsSame(edg))
              {
                B.Add(newFace, exp.Current());
                lfaces.Append(newFace);
                lcomplete.Append(newFace);
                break;
              }
            }
            if (exp2.More())
            {
              break;
            }
          }
        }
      }
    }

    BS.Perform(lfaces);
    myFirstShape = BS.Shape();

    B.MakeCompound(D);

    ExpS.ReInit();
    View.Clear();

    for (; ExpS.More(); ExpS.Next())
    {
      const TopoDS_Shape&         ES   = ExpS.Current();
      const NCollection_List<TopoDS_Shape>& lfls = myDPrism.GeneratedShapes(ES, myProfile3);
      for (it.Initialize(lfls); it.More(); it.Next())
      {
        if (View.Add(it.Value()))
          B.Add(D, it.Value());
      }
    }

    lfaces.Clear();
    theMapEF.Clear();
    TopExp::MapShapesAndAncestors(D, TopAbs_EDGE, TopAbs_FACE, theMapEF);
    View.Clear();

    for (i = 1; i <= theMapEF.Extent(); i++)
    {
      if (theMapEF(i).Extent() == 1)
      {
        const TopoDS_Edge& edg = TopoDS::Edge(theMapEF.FindKey(i));
        const TopoDS_Face& fac = TopoDS::Face(theMapEF(i).First());
        if (View.Add(fac))
        {
          TopoDS_Shape aLocalShape = fac.EmptyCopied();
          TopoDS_Face  newFace(TopoDS::Face(aLocalShape));
          //	  TopoDS_Face newFace(TopoDS::Face(fac.EmptyCopied()));
          TopExp_Explorer exp;
          for (exp.Init(fac.Oriented(TopAbs_FORWARD), TopAbs_WIRE); exp.More(); exp.Next())
          {
            //	    for (TopExp_Explorer exp2(exp.Current(),TopAbs_EDGE);
            TopExp_Explorer exp2(exp.Current(), TopAbs_EDGE);
            for (; exp2.More(); exp2.Next())
            {
              if (exp2.Current().IsSame(edg))
              {
                B.Add(newFace, exp.Current());
                lfaces.Append(newFace);
                lcomplete.Append(newFace);
                break;
              }
            }
            if (exp2.More())
            {
              break;
            }
          }
        }
      }
    }
    BS.Perform(lfaces);
    myLastShape = BS.Shape();

    View.Clear();

    for (ExpS.ReInit(); ExpS.More(); ExpS.Next())
    {
      const TopoDS_Shape&         ES   = ExpS.Current();
      const NCollection_List<TopoDS_Shape>& lffs = myDPrism.GeneratedShapes(ES, myProfile2);

      for (it.Initialize(lffs); it.More(); it.Next())
      {
        if (it.Value().ShapeType() == TopAbs_EDGE)
        {
          break;
        }
      }
      if (it.More())
      {
        TopoDS_Shape RemovedEdge = it.Value();
        TopoDS_Face  NewFace;
        TopoDS_Wire  NewWire;
        B.MakeWire(NewWire);
        TopAbs_Orientation Orref = TopAbs_FORWARD;
        TopExp_Explorer    exp;
        for (it.Initialize(lffs); it.More(); it.Next())
        {
          if (it.Value().ShapeType() == TopAbs_FACE)
          {
            exp.Init(it.Value().Oriented(TopAbs_FORWARD), TopAbs_WIRE);
            const TopoDS_Shape theWire = exp.Current();
            if (NewFace.IsNull())
            {
              occ::handle<Geom_Surface> S = BRep_Tool::Surface(TopoDS::Face(it.Value()));
              if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
              {
                S = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
              }
              if (S->DynamicType() != STANDARD_TYPE(Geom_Plane))
              {
                break;
              }

              B.MakeFace(NewFace, S, BRep_Tool::Tolerance(TopoDS::Face(it.Value())));
              NewFace.Orientation(TopAbs_FORWARD);
              Orref = theWire.Orientation();
              for (exp.Init(theWire.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp.More(); exp.Next())
              {
                if (!exp.Current().IsSame(RemovedEdge))
                {
                  B.Add(NewWire, exp.Current());
                }
              }
            }
            else
            {
              for (exp.Init(theWire.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp.More(); exp.Next())
              {
                if (!exp.Current().IsSame(RemovedEdge))
                {
                  if (theWire.Orientation() != Orref)
                  { // Les 2 faces planes ont des normales opposees
                    B.Add(NewWire, exp.Current());
                  }
                  else
                  {
                    B.Add(NewWire, exp.Current().Reversed());
                  }
                }
              }
            }
          }
        }
        if (!NewFace.IsNull())
        {
          B.Add(NewFace, NewWire.Oriented(Orref));
          lcomplete.Append(NewFace);
          NCollection_List<TopoDS_Shape> thelist;
          myMap.Bind(ES, thelist);
          myMap(ES).Append(NewFace);
        }
        else
        {
          for (it.Initialize(lffs); it.More(); it.Next())
          {
            if (View.Add(it.Value()) && it.Value().ShapeType() == TopAbs_FACE)
            {
              lcomplete.Append(it.Value());
            }
          }
        }
      }
      else
      {
        for (it.Initialize(lffs); it.More(); it.Next())
        {
          if (View.Add(it.Value()) && it.Value().ShapeType() == TopAbs_FACE)
          {
            lcomplete.Append(it.Value());
          }
        }
      }

      TopExp_Explorer ExpS2;
      for (ExpS2.Init(ES, TopAbs_VERTEX); ExpS2.More(); ExpS2.Next())
      {
        const NCollection_List<TopoDS_Shape>& ls2 = myDPrism.GeneratedShapes(ExpS2.Current(), myProfile2);
        for (it.Initialize(ls2); it.More(); it.Next())
        {
          if (View.Add(it.Value()) && it.Value().ShapeType() == TopAbs_FACE)
          {
            lcomplete.Append(it.Value());
          }
        }
      }
    }

    BS.Perform(lcomplete);
    myRes = BS.Shape();
    BRepLib::UpdateTolerances(myRes);
  }
}

//=================================================================================================

LocOpe_DPrism::LocOpe_DPrism(const TopoDS_Face&  Spine,
                             const double Height,
                             const double Angle)
    : mySpine(Spine)
{
  int i;
  myHeight        = Height;
  double y = Height * sin(Angle);
  double z = Height * cos(Angle);

  TopoDS_Vertex Vert1 = BRepLib_MakeVertex(gp_Pnt(0, 0, 0));
  TopoDS_Vertex Vert2 = BRepLib_MakeVertex(gp_Pnt(0, y, z));
  myProfile2          = BRepLib_MakeEdge(Vert1, Vert2);

  double Umin, Umax, Vmin, Vmax;
  BRepTools::UVBounds(Spine, Umin, Umax, Vmin, Vmax);
  double Deltay = std::max(Umax - Umin, Vmax - Vmin) + std::abs(y);
  Deltay *= 2;

  TopoDS_Vertex Vert3 = BRepLib_MakeVertex(gp_Pnt(0, y + Deltay, z));
  myProfile3          = BRepLib_MakeEdge(Vert2, Vert3);

  TopoDS_Vertex Vert4 = BRepLib_MakeVertex(gp_Pnt(0, Deltay, 0));
  myProfile1          = BRepLib_MakeEdge(Vert4, Vert1);

  myProfile = BRepLib_MakeWire(myProfile1, myProfile2, myProfile3);
  myDPrism.Perform(mySpine, myProfile, gp::XOY());

  if (myDPrism.IsDone())
  {
    LocOpe_BuildShape    BS;
    BRep_Builder         B;
    TopoDS_Compound      C;
    TopoDS_Compound      D;
    NCollection_List<TopoDS_Shape> lfaces, lcomplete;

    B.MakeCompound(C);
    NCollection_List<TopoDS_Shape>::Iterator it;
    TopExp_Explorer                    ExpS(mySpine, TopAbs_EDGE);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                View;
    for (; ExpS.More(); ExpS.Next())
    {
      const TopoDS_Shape&         ES   = ExpS.Current();
      const NCollection_List<TopoDS_Shape>& lffs = myDPrism.GeneratedShapes(ES, myProfile1);
      for (it.Initialize(lffs); it.More(); it.Next())
      {
        if (View.Add(it.Value()))
          B.Add(C, it.Value());
      }
    }

    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> theMapEF;
    TopExp::MapShapesAndAncestors(C, TopAbs_EDGE, TopAbs_FACE, theMapEF);
    View.Clear();

    for (i = 1; i <= theMapEF.Extent(); i++)
    {
      if (theMapEF(i).Extent() == 1)
      {
        const TopoDS_Edge& edg = TopoDS::Edge(theMapEF.FindKey(i));
        const TopoDS_Face& fac = TopoDS::Face(theMapEF(i).First());
        if (View.Add(fac))
        {
          TopoDS_Shape aLocalShape = fac.EmptyCopied();
          TopoDS_Face  newFace(TopoDS::Face(aLocalShape));
          //	  TopoDS_Face newFace(TopoDS::Face(fac.EmptyCopied()));
          TopExp_Explorer exp;
          for (exp.Init(fac.Oriented(TopAbs_FORWARD), TopAbs_WIRE); exp.More(); exp.Next())
          {
            //	    for (TopExp_Explorer exp2(exp.Current(),TopAbs_EDGE);
            TopExp_Explorer exp2(exp.Current(), TopAbs_EDGE);
            for (; exp2.More(); exp2.Next())
            {
              if (exp2.Current().IsSame(edg))
              {
                B.Add(newFace, exp.Current());
                lfaces.Append(newFace);
                lcomplete.Append(newFace);
                break;
              }
            }
            if (exp2.More())
            {
              break;
            }
          }
        }
      }
    }

    BS.Perform(lfaces);
    myFirstShape = BS.Shape();

    B.MakeCompound(D);

    ExpS.ReInit();
    View.Clear();

    for (; ExpS.More(); ExpS.Next())
    {
      const TopoDS_Shape&         ES   = ExpS.Current();
      const NCollection_List<TopoDS_Shape>& lfls = myDPrism.GeneratedShapes(ES, myProfile3);
      for (it.Initialize(lfls); it.More(); it.Next())
      {
        if (View.Add(it.Value()))
          B.Add(D, it.Value());
      }
    }

    lfaces.Clear();
    theMapEF.Clear();
    TopExp::MapShapesAndAncestors(D, TopAbs_EDGE, TopAbs_FACE, theMapEF);
    View.Clear();

    for (i = 1; i <= theMapEF.Extent(); i++)
    {
      if (theMapEF(i).Extent() == 1)
      {
        const TopoDS_Edge& edg = TopoDS::Edge(theMapEF.FindKey(i));
        const TopoDS_Face& fac = TopoDS::Face(theMapEF(i).First());
        if (View.Add(fac))
        {
          TopoDS_Shape aLocalShape = fac.EmptyCopied();
          TopoDS_Face  newFace(TopoDS::Face(aLocalShape));
          //	  TopoDS_Face newFace(TopoDS::Face(fac.EmptyCopied()));
          TopExp_Explorer exp;
          for (exp.Init(fac.Oriented(TopAbs_FORWARD), TopAbs_WIRE); exp.More(); exp.Next())
          {
            //	    for (TopExp_Explorer exp2(exp.Current(),TopAbs_EDGE);
            TopExp_Explorer exp2(exp.Current(), TopAbs_EDGE);
            for (; exp2.More(); exp2.Next())
            {
              if (exp2.Current().IsSame(edg))
              {
                B.Add(newFace, exp.Current());
                lfaces.Append(newFace);
                lcomplete.Append(newFace);
                break;
              }
            }
            if (exp2.More())
            {
              break;
            }
          }
        }
      }
    }
    BS.Perform(lfaces);
    myLastShape = BS.Shape();

    View.Clear();
    for (ExpS.ReInit(); ExpS.More(); ExpS.Next())
    {
      const TopoDS_Shape&         ES = ExpS.Current();
      const NCollection_List<TopoDS_Shape>& ls = myDPrism.GeneratedShapes(ES, myProfile2);
      for (it.Initialize(ls); it.More(); it.Next())
      {
        if (View.Add(it.Value()))
        {
          lcomplete.Append(it.Value());
        }
      }
      TopExp_Explorer ExpS2;
      for (ExpS2.Init(ES, TopAbs_VERTEX); ExpS2.More(); ExpS2.Next())
      {
        const NCollection_List<TopoDS_Shape>& ls2 = myDPrism.GeneratedShapes(ExpS2.Current(), myProfile2);
        for (it.Initialize(ls2); it.More(); it.Next())
        {
          if (View.Add(it.Value()) && it.Value().ShapeType() == TopAbs_FACE)
          {
            lcomplete.Append(it.Value());
          }
        }
      }
    }

    BS.Perform(lcomplete);
    myRes = BS.Shape();
    BRepLib::UpdateTolerances(myRes);
  }
}

//=================================================================================================

bool LocOpe_DPrism::IsDone() const
{
  return myDPrism.IsDone();
}

//=================================================================================================

const TopoDS_Shape& LocOpe_DPrism::Shape() const
{
  if (!myDPrism.IsDone())
  {
    throw StdFail_NotDone();
  }
  return myRes;
}

//=================================================================================================

const TopoDS_Shape& LocOpe_DPrism::Spine() const
{
  return mySpine;
}

//=================================================================================================

const TopoDS_Shape& LocOpe_DPrism::Profile() const
{
  return myProfile;
}

//=================================================================================================

const TopoDS_Shape& LocOpe_DPrism::FirstShape() const
{
  return myFirstShape;
}

//=================================================================================================

const TopoDS_Shape& LocOpe_DPrism::LastShape() const
{
  return myLastShape;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& LocOpe_DPrism::Shapes(const TopoDS_Shape& S) const
{
  if (!myDPrism.IsDone())
  {
    throw StdFail_NotDone();
  }
  if (myMap.IsBound(S))
  {
    return myMap(S);
  }
  else
  {
    return myDPrism.GeneratedShapes(S, myProfile2);
  }
}

//=================================================================================================

void LocOpe_DPrism::Curves(NCollection_Sequence<occ::handle<Geom_Curve>>& Scurves) const
{
  // Retrieves dy and dz with myProfile2
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(myProfile2, V1, V2);
  gp_Pnt        P1 = BRep_Tool::Pnt(V1);
  gp_Pnt        P2 = BRep_Tool::Pnt(V2);
  double dy = P2.Y() - P1.Y();
  double dz = P2.Z() - P1.Z();
  Scurves.Clear();
  occ::handle<Geom_Surface> S = BRep_Tool::Surface(mySpine);
  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    S = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
  }

  occ::handle<Geom_Plane> PP = occ::down_cast<Geom_Plane>(S);
  if (PP.IsNull())
  {
    throw Standard_ConstructionError();
  }

  gp_Pln P = PP->Pln();
  gp_Dir Normale(P.Axis().Direction());
  if (!P.Direct())
  {
    Normale.Reverse();
  }

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> theMap;
  TopExp_Explorer     exp(mySpine.Oriented(TopAbs_FORWARD), TopAbs_EDGE);
  TopLoc_Location     Loc;
  occ::handle<Geom_Curve>  C;
  double       f, l, prm;
  int    i;

  for (; exp.More(); exp.Next())
  {
    const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
    if (!theMap.Add(edg))
    {
      continue;
    }
    if (!BRep_Tool::Degenerated(edg))
    {
      C                = BRep_Tool::Curve(edg, Loc, f, l);
      C                = occ::down_cast<Geom_Curve>(C->Transformed(Loc.Transformation()));
      double u1 = -2 * std::abs(myHeight);
      double u2 = 2 * std::abs(myHeight);

      for (i = 0; i <= NECHANT; i++)
      {
        prm = ((NECHANT - i) * f + i * l) / NECHANT;
        gp_Pnt pt;
        gp_Vec d1;
        C->D1(prm, pt, d1);
        if (exp.Current().Orientation() == TopAbs_REVERSED)
        {
          d1.Reverse();
        }
        d1.Normalize();
        gp_Dir                    locy = Normale.Crossed(d1);
        gp_Vec                    ldir = dy * locy.XYZ() + dz * Normale.XYZ();
        gp_Lin                    lin(pt, ldir);
        occ::handle<Geom_Line>         Lin   = new Geom_Line(lin);
        occ::handle<Geom_TrimmedCurve> trlin = new Geom_TrimmedCurve(Lin, u1, u2, true);
        Scurves.Append(trlin);
      }
    }
  }
}

//=================================================================================================

occ::handle<Geom_Curve> LocOpe_DPrism::BarycCurve() const
{
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(myProfile2, V1, V2);
  gp_Pnt        P1 = BRep_Tool::Pnt(V1);
  gp_Pnt        P2 = BRep_Tool::Pnt(V2);
  double dz = P2.Z() - P1.Z();

  occ::handle<Geom_Surface> S = BRep_Tool::Surface(mySpine);
  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    S = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
  }

  occ::handle<Geom_Plane> PP = occ::down_cast<Geom_Plane>(S);
  if (PP.IsNull())
  {
    throw Standard_ConstructionError();
  }

  gp_Pln P = PP->Pln();
  gp_Dir Normale(P.Axis().Direction());
  if (!P.Direct())
  {
    Normale.Reverse();
  }
  if (mySpine.Orientation() == TopAbs_REVERSED)
  {
#ifdef OCCT_DEBUG
    bool trc = BRepFeat_GettraceFEAT();
    if (trc)
    {
      std::cout << "LocOpe_DPrism::BarycCurve()" << std::endl;
      std::cout << " Reversed Spine orientation" << std::endl;
    }
#endif
    //    Normale.Reverse();  //cts20871
  }
  gp_Vec Vec = dz * Normale.XYZ();

  gp_Pnt               bar(0., 0., 0.);
  NCollection_Sequence<gp_Pnt> spt;
  if (!myFirstShape.IsNull())
  {
    LocOpe::SampleEdges(myFirstShape, spt);
  }
  else
  {
    LocOpe::SampleEdges(mySpine, spt);
  }
  for (int jj = 1; jj <= spt.Length(); jj++)
  {
    const gp_Pnt& pvt = spt(jj);
    bar.ChangeCoord() += pvt.XYZ();
  }
  bar.ChangeCoord().Divide(spt.Length());
  gp_Ax1            newAx(bar, Vec);
  occ::handle<Geom_Line> theLin = new Geom_Line(newAx);
  return theLin;
}
