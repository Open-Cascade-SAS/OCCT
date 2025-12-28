// Created on: 1998-04-27
// Created by: Roman LYGIN
// Copyright (c) 1998-1999 Matra Datavision
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

// Modified:	Thu Sep 17 12:27:58 1998
// 25.12.98 pdn transmission from BRepTools_Sewing to BRepBuilderAPI_Sewing
// szv#4 S4163
// 11.01.00 svv #1: porting on DEC

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_BoxBndTree.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeAnalysis_Shell.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_Vertex.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <ShapeExtend_WireData.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

// ied_modif_for_compil_Nov-19-1998
//=================================================================================================

ShapeAnalysis_FreeBounds::ShapeAnalysis_FreeBounds() {}

//=================================================================================================

ShapeAnalysis_FreeBounds::ShapeAnalysis_FreeBounds(const TopoDS_Shape&    shape,
                                                   const double    toler,
                                                   const bool splitclosed,
                                                   const bool splitopen)
    : myTolerance(toler),
      myShared(false),
      mySplitClosed(splitclosed),
      mySplitOpen(splitopen)
{
  BRepBuilderAPI_Sewing Sew(toler, false, false);
  for (TopoDS_Iterator S(shape); S.More(); S.Next())
    Sew.Add(S.Value());
  Sew.Perform();
  //
  // Extract free edges.
  //
  int                  nbedge = Sew.NbFreeEdges();
  occ::handle<NCollection_HSequence<TopoDS_Shape>> edges  = new NCollection_HSequence<TopoDS_Shape>;
  occ::handle<NCollection_HSequence<TopoDS_Shape>> wires;
  TopoDS_Edge                       anEdge;
  for (int iedge = 1; iedge <= nbedge; iedge++)
  {
    anEdge = TopoDS::Edge(Sew.FreeEdge(iedge));
    if (!BRep_Tool::Degenerated(anEdge))
      edges->Append(anEdge);
  }
  //
  // Chainage.
  //
  ConnectEdgesToWires(edges, toler, false, wires);
  DispatchWires(wires, myWires, myEdges);
  SplitWires();

  return;
}

//=================================================================================================

ShapeAnalysis_FreeBounds::ShapeAnalysis_FreeBounds(const TopoDS_Shape&    shape,
                                                   const bool splitclosed,
                                                   const bool splitopen,
                                                   const bool checkinternaledges)
    : myTolerance(0.),
      myShared(true),
      mySplitClosed(splitclosed),
      mySplitOpen(splitopen)
{
  TopoDS_Shell aTmpShell;
  BRep_Builder aB;
  aB.MakeShell(aTmpShell);
  for (TopExp_Explorer aExpFace(shape, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
    aB.Add(aTmpShell, aExpFace.Current());

  ShapeAnalysis_Shell sas;
  sas.CheckOrientedShells(aTmpShell, true, checkinternaledges);

  if (sas.HasFreeEdges())
  {
    ShapeExtend_Explorer              see;
    occ::handle<NCollection_HSequence<TopoDS_Shape>> edges = see.SeqFromCompound(sas.FreeEdges(), false);

    occ::handle<NCollection_HSequence<TopoDS_Shape>> wires;
    ConnectEdgesToWires(edges, Precision::Confusion(), true, wires);
    DispatchWires(wires, myWires, myEdges);
    SplitWires();
  }
}

//=================================================================================================

void ShapeAnalysis_FreeBounds::ConnectEdgesToWires(occ::handle<NCollection_HSequence<TopoDS_Shape>>& edges,
                                                   const double                toler,
                                                   const bool             shared,
                                                   occ::handle<NCollection_HSequence<TopoDS_Shape>>& wires)
{
  occ::handle<NCollection_HSequence<TopoDS_Shape>> iwires = new NCollection_HSequence<TopoDS_Shape>;
  BRep_Builder                      B;

  int i; // svv #1
  for (i = 1; i <= edges->Length(); i++)
  {
    TopoDS_Wire wire;
    B.MakeWire(wire);
    B.Add(wire, edges->Value(i));
    iwires->Append(wire);
  }

  ConnectWiresToWires(iwires, toler, shared, wires);

  for (i = 1; i <= edges->Length(); i++)
    if (iwires->Value(i).Orientation() == TopAbs_REVERSED)
      edges->ChangeValue(i).Reverse();
}

//=================================================================================================

void ShapeAnalysis_FreeBounds::ConnectWiresToWires(occ::handle<NCollection_HSequence<TopoDS_Shape>>& iwires,
                                                   const double                toler,
                                                   const bool             shared,
                                                   occ::handle<NCollection_HSequence<TopoDS_Shape>>& owires)
{
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> map;
  ConnectWiresToWires(iwires, toler, shared, owires, map);
}

//=================================================================================================

void ShapeAnalysis_FreeBounds::ConnectWiresToWires(occ::handle<NCollection_HSequence<TopoDS_Shape>>& iwires,
                                                   const double                toler,
                                                   const bool             shared,
                                                   occ::handle<NCollection_HSequence<TopoDS_Shape>>& owires,
                                                   NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&      vertices)
{
  if (iwires.IsNull() || !iwires->Length())
    return;
  occ::handle<NCollection_HArray1<TopoDS_Shape>> arrwires = new NCollection_HArray1<TopoDS_Shape>(1, iwires->Length());
  // amv
  int i;
  for (i = 1; i <= arrwires->Length(); i++)
    arrwires->SetValue(i, iwires->Value(i));
  owires                  = new NCollection_HSequence<TopoDS_Shape>;
  double tolerance = std::max(toler, Precision::Confusion());

  occ::handle<ShapeExtend_WireData> sewd = new ShapeExtend_WireData(TopoDS::Wire(arrwires->Value(1)));

  bool isUsedManifoldMode = true;

  if ((sewd->NbEdges() < 1) && (sewd->NbNonManifoldEdges() > 0))
  {
    isUsedManifoldMode = false;
    sewd =
      new ShapeExtend_WireData(TopoDS::Wire(arrwires->Value(1)), true, isUsedManifoldMode);
  }

  occ::handle<ShapeAnalysis_Wire> saw = new ShapeAnalysis_Wire;
  saw->Load(sewd);
  saw->SetPrecision(tolerance);

  NCollection_UBTree<int, Bnd_Box>                            aBBTree;
  NCollection_UBTreeFiller<int, Bnd_Box> aTreeFiller(aBBTree);
  ShapeAnalysis_BoxBndTreeSelector                    aSel(arrwires, shared);
  aSel.LoadList(1);

  for (int inbW = 2; inbW <= arrwires->Length(); inbW++)
  {
    TopoDS_Wire   trW = TopoDS::Wire(arrwires->Value(inbW));
    Bnd_Box       aBox;
    TopoDS_Vertex trV1, trV2;
    ShapeAnalysis::FindBounds(trW, trV1, trV2);
    gp_Pnt trP1 = BRep_Tool::Pnt(trV1);
    gp_Pnt trP2 = BRep_Tool::Pnt(trV2);
    aBox.Set(trP1);
    aBox.Add(trP2);
    aBox.SetGap(tolerance);
    aTreeFiller.Add(inbW, aBox);
  }

  aTreeFiller.Fill();
  int nsel;

  ShapeAnalysis_Edge sae; // szv#4:S4163:12Mar99 moved
  bool   done = false;

  while (!done)
  {
    bool found = false, tail = false, direct = false;
    int lwire = 0;
    aSel.SetStop();
    Bnd_Box       FVBox, LVBox;
    TopoDS_Vertex Vf, Vl;
    Vf = sae.FirstVertex(sewd->Edge(1));
    Vl = sae.LastVertex(sewd->Edge(sewd->NbEdges()));

    gp_Pnt pf, pl;
    pf = BRep_Tool::Pnt(Vf);
    pl = BRep_Tool::Pnt(Vl);
    FVBox.Set(pf);
    FVBox.SetGap(tolerance);
    LVBox.Set(pl);
    LVBox.SetGap(tolerance);

    aSel.DefineBoxes(FVBox, LVBox);

    if (shared)
      aSel.DefineVertexes(Vf, Vl);
    else
    {
      aSel.DefinePnt(pf, pl);
      aSel.SetTolerance(tolerance);
    }

    nsel = aBBTree.Select(aSel);

    if (nsel != 0 && !aSel.LastCheckStatus(ShapeExtend_FAIL))
    {
      found  = true;
      lwire  = aSel.GetNb();
      tail   = aSel.LastCheckStatus(ShapeExtend_DONE1) || aSel.LastCheckStatus(ShapeExtend_DONE2);
      direct = aSel.LastCheckStatus(ShapeExtend_DONE1) || aSel.LastCheckStatus(ShapeExtend_DONE3);
      aSel.LoadList(lwire);
    }

    if (found)
    {
      if (!direct)
        arrwires->ChangeValue(lwire).Reverse();

      TopoDS_Wire                  aCurW = TopoDS::Wire(arrwires->Value(lwire));
      occ::handle<ShapeExtend_WireData> acurwd =
        new ShapeExtend_WireData(TopoDS::Wire(arrwires->Value(lwire)),
                                 true,
                                 isUsedManifoldMode);
      if (!acurwd->NbEdges())
        continue;
      sewd->Add(acurwd, (tail ? 0 : 1));
    }
    else
    {
      // making wire
      // 1.providing connection (see ShapeFix_Wire::FixConnected())
      // int i; // svv #1
      for (/*int*/ i = 1; i <= saw->NbEdges(); i++)
      {
        if (saw->CheckConnected(i))
        {
          int n2 = i;
          int n1 = (n2 > 1 ? n2 - 1 : saw->NbEdges());
          TopoDS_Edge      E1 = sewd->Edge(n1);
          TopoDS_Edge      E2 = sewd->Edge(n2);

          TopoDS_Vertex Vprev, Vfol, V; // connection vertex
          Vprev = sae.LastVertex(E1);
          Vfol  = sae.FirstVertex(E2);

          if (saw->LastCheckStatus(ShapeExtend_DONE1)) // absolutely confused
            V = Vprev;
          else
          {
            ShapeBuild_Vertex sbv;
            V = sbv.CombineVertex(Vprev, Vfol);
          }
          vertices.Bind(Vprev, V);
          vertices.Bind(Vfol, V);

          // replace vertices to a new one
          ShapeBuild_Edge sbe;
          if (saw->NbEdges() < 2)
            sewd->Set(sbe.CopyReplaceVertices(E2, V, V), n2);
          else
          {
            sewd->Set(sbe.CopyReplaceVertices(E2, V, TopoDS_Vertex()), n2);
            if (!saw->LastCheckStatus(ShapeExtend_DONE1))
              sewd->Set(sbe.CopyReplaceVertices(E1, TopoDS_Vertex(), V), n1);
          }
        }
      }

      // 2.making wire
      TopoDS_Wire wire = sewd->Wire();
      if (isUsedManifoldMode)
      {
        if (!saw->CheckConnected(1) && saw->LastCheckStatus(ShapeExtend_OK))
          wire.Closed(true);
      }
      else
      {
        // Try to check connection by number of free vertices
        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> vmap;
        TopoDS_Iterator     it(wire);

        for (; it.More(); it.Next())
        {
          const TopoDS_Shape& E = it.Value();
          TopoDS_Iterator     ite(E, false, true);
          for (; ite.More(); ite.Next())
          {
            const TopoDS_Shape& V = ite.Value();
            if (V.Orientation() == TopAbs_FORWARD || V.Orientation() == TopAbs_REVERSED)
            {
              // add or remove in the vertex map
              if (!vmap.Add(V))
                vmap.Remove(V);
            }
          }
        }
        if (vmap.IsEmpty())
        {
          wire.Closed(true);
        }
      }

      owires->Append(wire);
      sewd->Clear();
      sewd->ManifoldMode() = isUsedManifoldMode;

      // Recherche de la premier edge non traitee pour un autre wire.
      // Searching for first edge for next wire
      lwire = -1;
      for (/*int*/ i = 1; i <= arrwires->Length(); i++)
      {
        if (!aSel.ContWire(i))
        {
          lwire = i; // szv#4:S4163:12Mar99 optimized
          sewd->Add(TopoDS::Wire(arrwires->Value(lwire)));
          aSel.LoadList(lwire);

          if (sewd->NbEdges() > 0)
            break;
          sewd->Clear();
        }
      }

      if (lwire == -1)
        done = 1;
    }
  }

  for (/*int*/ i = 1; i <= iwires->Length(); i++)
  {
    iwires->SetValue(i, arrwires->Value(i));
  }
}

static void SplitWire(const TopoDS_Wire&                 wire,
                      const double                toler,
                      const bool             shared,
                      occ::handle<NCollection_HSequence<TopoDS_Shape>>& closed,
                      occ::handle<NCollection_HSequence<TopoDS_Shape>>& open)
{
  closed                  = new NCollection_HSequence<TopoDS_Shape>;
  open                    = new NCollection_HSequence<TopoDS_Shape>;
  double tolerance = std::max(toler, Precision::Confusion());

  BRep_Builder       B;
  ShapeAnalysis_Edge sae;

  occ::handle<ShapeExtend_WireData> sewd    = new ShapeExtend_WireData(wire);
  int             nbedges = sewd->NbEdges();

  // ConnectedEdgeSequence - list of indices of connected edges to build a wire
  NCollection_Sequence<int> ces;
  // statuses - array of flags describing the edge:
  // 0-free, 1-in CES, 2-already in wire,
  // 3-no closed wire can be produced starting at this edge
  NCollection_Array1<int> statuses(1, nbedges);
  statuses.Init(0);

  // building closed wires
  int i; // svv #1
  for (i = 1; i <= nbedges; i++)
    if (statuses.Value(i) == 0)
    {
      ces.Append(i);
      statuses.SetValue(i, 1); // putting into CES
      bool SearchBackward = true;

      for (;;)
      {
        bool found;
        TopoDS_Edge      edge;
        TopoDS_Vertex    lvertex;
        gp_Pnt           lpoint;

        // searching for connection in ces
        if (SearchBackward)
        {
          SearchBackward = false;
          found          = false;
          edge           = sewd->Edge(ces.Last());
          lvertex        = sae.LastVertex(edge);
          lpoint         = BRep_Tool::Pnt(lvertex);
          int j; // svv #1
          for (j = ces.Length(); (j >= 1) && !found; j--)
          {
            TopoDS_Vertex fv = sae.FirstVertex(sewd->Edge(ces.Value(j)));
            gp_Pnt        fp = BRep_Tool::Pnt(fv);
            if ((shared && lvertex.IsSame(fv)) || (!shared && lpoint.IsEqual(fp, tolerance)))
              found = true;
          }

          if (found)
          {
            j++; // because of decreasing last iteration
            // making closed wire
            TopoDS_Wire wire1;
            B.MakeWire(wire1);
            for (int cesindex = j; cesindex <= ces.Length(); cesindex++)
            {
              B.Add(wire1, sewd->Edge(ces.Value(cesindex)));
              statuses.SetValue(ces.Value(cesindex), 2);
            }
            wire1.Closed(true);
            closed->Append(wire1);
            ces.Remove(j, ces.Length());
            if (ces.IsEmpty())
              break;
          }
        }

        // searching for connection among free edges
        found   = false;
        edge    = sewd->Edge(ces.Last());
        lvertex = sae.LastVertex(edge);
        lpoint  = BRep_Tool::Pnt(lvertex);
        int j; // svv #1
        for (j = 1; (j <= nbedges) && !found; j++)
          if (statuses.Value(j) == 0)
          {
            TopoDS_Vertex fv = sae.FirstVertex(sewd->Edge(j));
            gp_Pnt        fp = BRep_Tool::Pnt(fv);
            if ((shared && lvertex.IsSame(fv)) || (!shared && lpoint.IsEqual(fp, tolerance)))
              found = true;
          }

        if (found)
        {
          j--; // because of last iteration
          ces.Append(j);
          statuses.SetValue(j, 1); // putting into CES
          SearchBackward = true;
          continue;
        }

        // no edges found - mark the branch as open (use status 3)
        statuses.SetValue(ces.Last(), 3);
        ces.Remove(ces.Length());
        if (ces.IsEmpty())
          break;
      }
    }

  // building open wires
  occ::handle<NCollection_HSequence<TopoDS_Shape>> edges = new NCollection_HSequence<TopoDS_Shape>;
  for (i = 1; i <= nbedges; i++)
    if (statuses.Value(i) != 2)
      edges->Append(sewd->Edge(i));

  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(edges, toler, shared, open);
}

void ShapeAnalysis_FreeBounds::SplitWires(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& wires,
                                          const double                      toler,
                                          const bool                   shared,
                                          occ::handle<NCollection_HSequence<TopoDS_Shape>>&       closed,
                                          occ::handle<NCollection_HSequence<TopoDS_Shape>>&       open)
{
  closed = new NCollection_HSequence<TopoDS_Shape>;
  open   = new NCollection_HSequence<TopoDS_Shape>;

  for (int i = 1; i <= wires->Length(); i++)
  {
    occ::handle<NCollection_HSequence<TopoDS_Shape>> tmpclosed, tmpopen;
    SplitWire(TopoDS::Wire(wires->Value(i)), toler, shared, tmpclosed, tmpopen);
    closed->Append(tmpclosed);
    open->Append(tmpopen);
  }
}

//=================================================================================================

void ShapeAnalysis_FreeBounds::DispatchWires(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& wires,
                                             TopoDS_Compound&                         closed,
                                             TopoDS_Compound&                         open)
{
  BRep_Builder B;
  if (closed.IsNull())
    B.MakeCompound(closed);
  if (open.IsNull())
    B.MakeCompound(open);
  if (wires.IsNull())
    return;

  for (int iw = 1; iw <= wires->Length(); iw++)
    if (wires->Value(iw).Closed())
      B.Add(closed, wires->Value(iw));
    else
      B.Add(open, wires->Value(iw));
}

//=======================================================================
// function : SplitWires
// purpose  : Splits compounds of closed (myWires) and open (myEdges) wires
//           into small closed wires according to fields mySplitClosed and
//           mySplitOpen and rebuilds compounds
//=======================================================================

void ShapeAnalysis_FreeBounds::SplitWires()
{
  if (!mySplitClosed && !mySplitOpen)
    return; // nothing to do

  ShapeExtend_Explorer              see;
  occ::handle<NCollection_HSequence<TopoDS_Shape>> closedwires, cw1, cw2, openwires, ow1, ow2;
  closedwires = see.SeqFromCompound(myWires, false);
  openwires   = see.SeqFromCompound(myEdges, false);

  if (mySplitClosed)
    SplitWires(closedwires, myTolerance, myShared, cw1, ow1);
  else
  {
    cw1 = closedwires;
    ow1 = new NCollection_HSequence<TopoDS_Shape>;
  }

  if (mySplitOpen)
    SplitWires(openwires, myTolerance, myShared, cw2, ow2);
  else
  {
    cw2 = new NCollection_HSequence<TopoDS_Shape>;
    ow2 = openwires;
  }

  closedwires = cw1;
  closedwires->Append(cw2);
  openwires = ow1;
  openwires->Append(ow2);

  // szv#4:S4163:12Mar99 SGI warns
  TopoDS_Shape compWires = see.CompoundFromSeq(closedwires);
  TopoDS_Shape compEdges = see.CompoundFromSeq(openwires);
  myWires                = TopoDS::Compound(compWires);
  myEdges                = TopoDS::Compound(compEdges);
}
