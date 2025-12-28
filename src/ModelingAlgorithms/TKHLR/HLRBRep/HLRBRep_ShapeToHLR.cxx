// Created on: 1992-08-27
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef No_Exception
// #define No_Exception
#endif

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <HLRBRep_Data.hxx>
#include <HLRBRep_ShapeToHLR.hxx>
#include <HLRTopoBRep_OutLiner.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Map.hxx>

//=================================================================================================

occ::handle<HLRBRep_Data> HLRBRep_ShapeToHLR::Load(
  const occ::handle<HLRTopoBRep_OutLiner>&                                         S,
  const HLRAlgo_Projector&                                                         P,
  NCollection_DataMap<TopoDS_Shape, BRepTopAdaptor_Tool, TopTools_ShapeMapHasher>& MST,
  const int                                                                        nbIso)
{
  S->Fill(P, MST, nbIso);

  HLRTopoBRep_Data&                                             TopDS = S->DataStructure();
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> FM;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> EM;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    VerticesToEdges;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    EdgesToFaces;

  TopExp_Explorer exshell, exface;

  for (exshell.Init(S->OutLinedShape(), TopAbs_SHELL); exshell.More(); exshell.Next())
  { // faces in a shell

    for (exface.Init(exshell.Current(), TopAbs_FACE); exface.More(); exface.Next())
    {
      if (!FM.Contains(exface.Current()))
        FM.Add(exface.Current());
    }
  }

  for (exface.Init(S->OutLinedShape(), TopAbs_FACE, TopAbs_SHELL); exface.More(); exface.Next())
  { // faces not in a shell
    if (!FM.Contains(exface.Current()))
      FM.Add(exface.Current());
  }

  TopExp::MapShapes(S->OutLinedShape(), TopAbs_EDGE, EM);

  int i;
  int nbEdge = EM.Extent();

  for (i = 1; i <= nbEdge; i++) // vertices back to edges
    TopExp::MapShapesAndAncestors(EM(i), TopAbs_VERTEX, TopAbs_EDGE, VerticesToEdges);

  int nbVert = VerticesToEdges.Extent();
  int nbFace = FM.Extent();

  TopoDS_Vertex                            VF, VL;
  NCollection_List<TopoDS_Shape>::Iterator itn;
  int                                      i1, i2;
  bool                                     o1, o2;
  bool                                     c1, c2;
  double                                   pf, pl;
  float                                    tf, tl;

  // Create the data structure
  occ::handle<HLRBRep_Data> DS = new HLRBRep_Data(nbVert, nbEdge, nbFace);
  HLRBRep_EdgeData*         ed = nullptr;
  if (nbEdge != 0)
    ed = &(DS->EDataArray().ChangeValue(1));
  //  ed++;

  for (i = 1; i <= nbFace; i++)
  { // test of Double edges
    TopExp::MapShapesAndAncestors(FM(i), TopAbs_EDGE, TopAbs_FACE, EdgesToFaces);
  }

  for (i = 1; i <= nbEdge; i++)
  { // load the Edges
    const TopoDS_Edge& Edg = TopoDS::Edge(EM(i));
    TopExp::Vertices(Edg, VF, VL);
    BRep_Tool::Range(Edg, pf, pl);
    bool reg1 = false;
    bool regn = false;
    int  inde = EdgesToFaces.FindIndex(Edg);
    if (inde > 0)
    {
      if (EdgesToFaces(inde).Extent() == 2)
      {
        itn                   = EdgesToFaces(inde);
        const TopoDS_Face& F1 = TopoDS::Face(itn.Value());
        itn.Next();
        const TopoDS_Face& F2 = TopoDS::Face(itn.Value());
        GeomAbs_Shape      rg = BRep_Tool::Continuity(Edg, F1, F2);
        reg1                  = rg >= GeomAbs_G1;
        regn                  = rg >= GeomAbs_G2;
      }
    }

    if (VF.IsNull())
    {
      i1 = 0;
      o1 = false;
      c1 = false;
      pf = RealFirst();
      tf = (float)Epsilon(pf);
    }
    else
    {
      i1 = VerticesToEdges.FindIndex(VF);
      o1 = TopDS.IsOutV(VF);
      c1 = TopDS.IsIntV(VF);
      tf = (float)BRep_Tool::Tolerance(VF);
    }

    if (VL.IsNull())
    {
      i2 = 0;
      o2 = false;
      c2 = false;
      pl = RealLast();
      tl = (float)Epsilon(pl);
    }
    else
    {
      i2 = VerticesToEdges.FindIndex(VL);
      o2 = TopDS.IsOutV(VL);
      c2 = TopDS.IsIntV(VL);
      tl = (float)BRep_Tool::Tolerance(VL);
    }

    ed->Set(reg1, regn, Edg, i1, i2, o1, o2, c1, c2, pf, tf, pl, tl);
    DS->EdgeMap().Add(Edg);
    ed++;
  }

  ExploreShape(S, DS, FM, EM);
  return DS;
}

//=================================================================================================

void HLRBRep_ShapeToHLR::ExploreFace(
  const occ::handle<HLRTopoBRep_OutLiner>&                             S,
  const occ::handle<HLRBRep_Data>&                                     DS,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& FM,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& EM,
  int&                                                                 i,
  const TopoDS_Face&                                                   F,
  const bool                                                           closed)
{
  i++;
  TopExp_Explorer    Ex1, Ex2;
  HLRTopoBRep_Data&  TopDS   = S->DataStructure();
  TopAbs_Orientation orient  = FM(i).Orientation();
  TopoDS_Face        theFace = TopoDS::Face(FM(i));
  theFace.Orientation(TopAbs_FORWARD);
  HLRBRep_FaceData& fd = DS->FDataArray().ChangeValue(i);

  int nw = 0;

  for (Ex1.Init(theFace, TopAbs_WIRE); Ex1.More(); Ex1.Next())
    nw++;

  fd.Set(theFace, orient, closed, nw);
  nw = 0;

  for (Ex1.Init(theFace, TopAbs_WIRE); Ex1.More(); Ex1.Next())
  {
    nw++;
    int ne = 0;

    for (Ex2.Init(Ex1.Current(), TopAbs_EDGE); Ex2.More(); Ex2.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(Ex2.Current());
      if (!BRep_Tool::Degenerated(anEdge))
        ne++;
    }

    fd.SetWire(nw, ne);
    ne = 0;

    for (Ex2.Init(Ex1.Current(), TopAbs_EDGE); Ex2.More(); Ex2.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(Ex2.Current());
      if (BRep_Tool::Degenerated(E))
        continue;
      ne++;
      int                ie        = EM.FindIndex(E);
      TopAbs_Orientation anOrientE = E.Orientation();
      bool               Int       = TopDS.IsIntLFaceEdge(F, E);
      bool               Iso       = TopDS.IsIsoLFaceEdge(F, E);
      bool               Out       = TopDS.IsOutLFaceEdge(F, E);
      bool               Dbl       = BRepTools::IsReallyClosed(TopoDS::Edge(E), theFace);
      fd.SetWEdge(nw, ne, ie, anOrientE, Out, Int, Dbl, Iso);
    }
  }
  DS->FaceMap().Add(theFace);
}

//=================================================================================================

void HLRBRep_ShapeToHLR::ExploreShape(
  const occ::handle<HLRTopoBRep_OutLiner>&                             S,
  const occ::handle<HLRBRep_Data>&                                     DS,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& FM,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& EM)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> ShapeMap;
  TopExp_Explorer                                        exshell, exface, exedge;
  int                                                    i = 0;

  for (exshell.Init(S->OriginalShape(), TopAbs_SHELL); exshell.More(); exshell.Next())
  { // faces in a shell (open or close)

    bool closed = exshell.Current().Closed();

    if (!closed)
    {
      int  ie;
      int  nbEdge = EM.Extent();
      int* flag   = new int[nbEdge + 1];

      for (ie = 1; ie <= nbEdge; ie++)
        flag[ie] = 0;

      for (exedge.Init(exshell.Current(), TopAbs_EDGE); exedge.More(); exedge.Next())
      {
        const TopoDS_Edge& E      = TopoDS::Edge(exedge.Current());
        ie                        = EM.FindIndex(E);
        TopAbs_Orientation orient = E.Orientation();
        if (!BRep_Tool::Degenerated(E))
        {
          if (orient == TopAbs_FORWARD)
            flag[ie] += 1;
          else if (orient == TopAbs_REVERSED)
            flag[ie] -= 1;
        }
      }
      closed = true;

      for (ie = 1; ie <= nbEdge && closed; ie++)
        closed = (flag[ie] == 0);
      delete[] flag;
      flag = nullptr;
    }

    for (exface.Init(exshell.Current(), TopAbs_FACE); exface.More(); exface.Next())
    {
      if (ShapeMap.Add(exface.Current()))
      {
        ExploreFace(S, DS, FM, EM, i, TopoDS::Face(exface.Current()), closed);
      }
    }
  }

  for (exface.Init(S->OriginalShape(), TopAbs_FACE, TopAbs_SHELL); exface.More(); exface.Next())
  { // faces not in a shell
    if (ShapeMap.Add(exface.Current()))
    {
      ExploreFace(S, DS, FM, EM, i, TopoDS::Face(exface.Current()), false);
    }
  }
}
