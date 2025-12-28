// Created on: 1997-05-27
// Created by: Jacques GOUSSARD
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

//  Modified by skv - Mon May 31 12:58:34 2004 OCC5865

#include <BRep_Builder.hxx>
#include <LocOpe_BuildWires.hxx>
#include <LocOpe_WiresOnShape.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>

static int FindFirstEdge(const NCollection_IndexedDataMap<TopoDS_Shape,
                                                          NCollection_List<TopoDS_Shape>,
                                                          TopTools_ShapeMapHasher>&,
                         const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&);

//=================================================================================================

LocOpe_BuildWires::LocOpe_BuildWires()
    : myDone(false)
{
}

//=================================================================================================

//  Modified by skv - Mon May 31 12:58:27 2004 OCC5865 Begin
LocOpe_BuildWires::LocOpe_BuildWires(const NCollection_List<TopoDS_Shape>&   L,
                                     const occ::handle<LocOpe_WiresOnShape>& PW)
{
  Perform(L, PW);
}

//  Modified by skv - Mon May 31 12:58:28 2004 OCC5865 End

//=================================================================================================

//  Modified by skv - Mon May 31 12:59:09 2004 OCC5865 Begin
void LocOpe_BuildWires::Perform(const NCollection_List<TopoDS_Shape>&   L,
                                const occ::handle<LocOpe_WiresOnShape>& PW)
{
  //  Modified by skv - Mon May 31 12:59:10 2004 OCC5865 End
  myDone = false;
  myRes.Clear();

  BRep_Builder    B;
  TopoDS_Compound C;
  B.MakeCompound(C);

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> theMap;
  NCollection_List<TopoDS_Shape>::Iterator               itl(L);
  for (; itl.More(); itl.Next())
  {
    const TopoDS_Shape& edg = itl.Value();
    if (theMap.Add(edg) && edg.ShapeType() == TopAbs_EDGE)
    {
      B.Add(C, edg.Oriented(TopAbs_FORWARD)); // orientation importante pour
      // appel a TopExp::Vertices
    }
  }

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    theMapVE;
  TopExp::MapShapesAndAncestors(C, TopAbs_VERTEX, TopAbs_EDGE, theMapVE);

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> Bords;
  //  for (int i = 1; i <= theMapVE.Extent(); i++) {
  int i;

  for (i = 1; i <= theMapVE.Extent(); i++)
  {
    //  Modified by skv - Mon May 31 13:07:50 2004 OCC5865 Begin
    //     if (theMapVE(i).Extent() == 1) {
    TopoDS_Vertex vtx = TopoDS::Vertex(theMapVE.FindKey(i));
    TopoDS_Edge   etmp;
    TopoDS_Vertex aV_border;
    double        partmp;
    if (theMapVE(i).Extent() == 1
        || (PW->OnVertex(vtx, aV_border) || PW->OnEdge(vtx, etmp, partmp)))
    {
      Bords.Add(vtx);
      //  Modified by skv - Mon May 31 13:07:50 2004 OCC5865 End
    }
  }

  while ((i = FindFirstEdge(theMapVE, Bords)) <= theMapVE.Extent())
  {
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mapE;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>        mapV;
    const TopoDS_Edge& edgf = TopoDS::Edge(theMapVE(i).First());

    TopoDS_Vertex VF, VL;
    TopExp::Vertices(edgf, VF, VL);

    if (Bords.Contains(VL) && !Bords.Contains(VF))
    {
      mapE.Add(edgf.Oriented(TopAbs_REVERSED));
      TopoDS_Vertex temp = VF;
      VF                 = VL;
      VL                 = temp;
    }
    else
    {
      mapE.Add(edgf.Oriented(TopAbs_FORWARD));
    }
    mapV.Add(VF);

    while (!(mapV.Contains(VL) || Bords.Contains(VL)))
    {
      int                                      ind = theMapVE.FindIndex(VL);
      NCollection_List<TopoDS_Shape>::Iterator anIterl(theMapVE(ind));
      for (; anIterl.More(); anIterl.Next())
      {
        if (!mapE.Contains(anIterl.Value()))
        {
          break;
        }
      }

      if (!anIterl.More())
      {
        throw Standard_ConstructionError();
      }
      const TopoDS_Edge& theEdge = TopoDS::Edge(anIterl.Value());
      TopoDS_Vertex      Vf, Vl;
      TopExp::Vertices(theEdge, Vf, Vl);
      mapV.Add(VL);
      if (Vf.IsSame(VL))
      {
        mapE.Add(theEdge.Oriented(TopAbs_FORWARD));
        VL = Vl;
      }
      else
      { // on doit avoir Vl == VL
        mapE.Add(theEdge.Oriented(TopAbs_REVERSED));
        VL = Vf;
      }
    }

    TopoDS_Wire newWire;
    B.MakeWire(newWire);

    if (mapV.Contains(VL))
    { // on sort avec une boucle a recreer
      TopoDS_Vertex Vf;
      //      for (int j = 1; j<= mapE.Extent(); j++) {
      int j;
      for (j = 1; j <= mapE.Extent(); j++)
      {
        const TopoDS_Edge& edg = TopoDS::Edge(mapE(j));
        if (edg.Orientation() == TopAbs_FORWARD)
        {
          Vf = TopExp::FirstVertex(edg);
        }
        else
        {
          Vf = TopExp::LastVertex(edg);
        }
        if (Vf.IsSame(VL))
        {
          break;
        }
        mapV.Remove(Vf);
      }
      for (; j <= mapE.Extent(); j++)
      {
        B.Add(newWire, mapE(j));
      }
      newWire.Closed(true);
    }
    else
    { // on sort sur un bord : wire ouvert...
      mapV.Add(VL);
      for (int j = 1; j <= mapE.Extent(); j++)
      {
        B.Add(newWire, mapE(j));
      }
      newWire.Closed(false);
    }

    myRes.Append(newWire);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itm;
    for (itm.Initialize(mapV); itm.More(); itm.Next())
    {
      const TopoDS_Vertex& vtx = TopoDS::Vertex(itm.Key());
      Bords.Add(vtx);
      int ind = theMapVE.FindIndex(vtx);
      itl.Initialize(theMapVE(ind));
      while (itl.More())
      {
        if (mapE.Contains(itl.Value()))
        {
          theMapVE(ind).Remove(itl);
        }
        else
        {
          itl.Next();
        }
      }
    }
  }

  myDone = true;
}

//=================================================================================================

bool LocOpe_BuildWires::IsDone() const
{
  return myDone;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& LocOpe_BuildWires::Result() const
{
  if (!myDone)
  {
    throw StdFail_NotDone();
  }
  return myRes;
}

//=================================================================================================

static int FindFirstEdge(const NCollection_IndexedDataMap<TopoDS_Shape,
                                                          NCollection_List<TopoDS_Shape>,
                                                          TopTools_ShapeMapHasher>&    theMapVE,
                         const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theBord)
{
  int i = 1;

  for (; i <= theMapVE.Extent(); i++)
  {
    if (theMapVE(i).Extent() > 0)
    {
      break;
    }
  }

  if (i > theMapVE.Extent())
  {
    return i;
  }

  int goodi = i;
  for (; i <= theMapVE.Extent(); i++)
  {
    const TopoDS_Shape& vtx = theMapVE.FindKey(i);
    if (theMapVE(i).Extent() > 0 && theBord.Contains(vtx))
    {
      goodi = i;
      break;
    }
  }
  return goodi;
}
