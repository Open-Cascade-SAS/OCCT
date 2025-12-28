// Created on: 1993-01-19
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

#define No_Standard_NoMoreObject
#define No_Standard_NoSuchObject
#define No_Standard_TypeMismatch

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

//=================================================================================================

void TopExp::MapShapes(const TopoDS_Shape&                                            S,
                       const TopAbs_ShapeEnum                                         T,
                       NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& M)
{
  TopExp_Explorer Ex(S, T);
  while (Ex.More())
  {
    M.Add(Ex.Current());
    Ex.Next();
  }
}

//=================================================================================================

void TopExp::MapShapes(const TopoDS_Shape&                                            S,
                       NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& M,
                       const bool                                                     cumOri,
                       const bool                                                     cumLoc)
{
  M.Add(S);
  TopoDS_Iterator It(S, cumOri, cumLoc);
  while (It.More())
  {
    MapShapes(It.Value(), M);
    It.Next();
  }
}

//=================================================================================================

void TopExp::MapShapes(const TopoDS_Shape&                                     S,
                       NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& M,
                       const bool                                              cumOri,
                       const bool                                              cumLoc)
{
  M.Add(S);
  TopoDS_Iterator It(S, cumOri, cumLoc);
  for (; It.More(); It.Next())
    MapShapes(It.Value(), M);
}

//=================================================================================================

void TopExp::MapShapesAndAncestors(
  const TopoDS_Shape&    S,
  const TopAbs_ShapeEnum TS,
  const TopAbs_ShapeEnum TA,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    M)
{
  NCollection_List<TopoDS_Shape> empty;

  // visit ancestors
  TopExp_Explorer exa(S, TA);
  while (exa.More())
  {
    // visit shapes
    const TopoDS_Shape& anc = exa.Current();
    TopExp_Explorer     exs(anc, TS);
    while (exs.More())
    {
      int index = M.FindIndex(exs.Current());
      if (index == 0)
        index = M.Add(exs.Current(), empty);
      M(index).Append(anc);
      exs.Next();
    }
    exa.Next();
  }

  // visit shapes not under ancestors
  TopExp_Explorer ex(S, TS, TA);
  while (ex.More())
  {
    int index = M.FindIndex(ex.Current());
    if (index == 0)
      index = M.Add(ex.Current(), empty);
    ex.Next();
  }
}

//=================================================================================================

void TopExp::MapShapesAndUniqueAncestors(
  const TopoDS_Shape&    S,
  const TopAbs_ShapeEnum TS,
  const TopAbs_ShapeEnum TA,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
             M,
  const bool useOrientation)
{
  NCollection_List<TopoDS_Shape> empty;

  // visit ancestors
  TopExp_Explorer exa(S, TA);
  while (exa.More())
  {
    // visit shapes
    const TopoDS_Shape& anc = exa.Current();
    TopExp_Explorer     exs(anc, TS);
    while (exs.More())
    {
      int index = M.FindIndex(exs.Current());
      if (index == 0)
        index = M.Add(exs.Current(), empty);
      NCollection_List<TopoDS_Shape>& aList = M(index);
      // check if anc already exists in a list
      NCollection_List<TopoDS_Shape>::Iterator it(aList);
      for (; it.More(); it.Next())
        if (useOrientation ? anc.IsEqual(it.Value()) : anc.IsSame(it.Value()))
          break;
      if (!it.More())
        aList.Append(anc);
      exs.Next();
    }
    exa.Next();
  }

  // visit shapes not under ancestors
  TopExp_Explorer ex(S, TS, TA);
  while (ex.More())
  {
    int index = M.FindIndex(ex.Current());
    if (index == 0)
      M.Add(ex.Current(), empty);
    ex.Next();
  }
}

//=================================================================================================

TopoDS_Vertex TopExp::FirstVertex(const TopoDS_Edge& E, const bool CumOri)
{
  TopoDS_Iterator ite(E, CumOri);
  while (ite.More())
  {
    if (ite.Value().Orientation() == TopAbs_FORWARD)
      return TopoDS::Vertex(ite.Value());
    ite.Next();
  }
  return TopoDS_Vertex();
}

//=================================================================================================

TopoDS_Vertex TopExp::LastVertex(const TopoDS_Edge& E, const bool CumOri)
{
  TopoDS_Iterator ite(E, CumOri);
  while (ite.More())
  {
    if (ite.Value().Orientation() == TopAbs_REVERSED)
      return TopoDS::Vertex(ite.Value());
    ite.Next();
  }
  return TopoDS_Vertex();
}

//=================================================================================================

void TopExp::Vertices(const TopoDS_Edge& E,
                      TopoDS_Vertex&     Vfirst,
                      TopoDS_Vertex&     Vlast,
                      const bool         CumOri)
{
  // minor optimization for case when Vfirst and Vlast are non-null:
  // at least for VC++ 10, it is faster if we use boolean flags than
  // if we nullify vertices at that point (see #27021)
  bool isFirstDefined = false;
  bool isLastDefined  = false;

  TopoDS_Iterator ite(E, CumOri);
  while (ite.More())
  {
    const TopoDS_Shape& aV = ite.Value();
    if (aV.Orientation() == TopAbs_FORWARD)
    {
      Vfirst         = TopoDS::Vertex(aV);
      isFirstDefined = true;
    }
    else if (aV.Orientation() == TopAbs_REVERSED)
    {
      Vlast         = TopoDS::Vertex(aV);
      isLastDefined = true;
    }
    ite.Next();
  }

  if (!isFirstDefined)
    Vfirst.Nullify();

  if (!isLastDefined)
    Vlast.Nullify();
}

//=================================================================================================

void TopExp::Vertices(const TopoDS_Wire& W, TopoDS_Vertex& Vfirst, TopoDS_Vertex& Vlast)
{
  Vfirst = Vlast = TopoDS_Vertex(); // nullify

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> vmap;
  TopoDS_Iterator                                        it(W);
  TopoDS_Vertex                                          V1, V2;

  while (it.More())
  {
    const TopoDS_Edge& E = TopoDS::Edge(it.Value());
    if (E.Orientation() == TopAbs_REVERSED)
      TopExp::Vertices(E, V2, V1);
    else
      TopExp::Vertices(E, V1, V2);
    // add or remove in the vertex map
    V1.Orientation(TopAbs_FORWARD);
    V2.Orientation(TopAbs_REVERSED);
    if (!vmap.Add(V1))
      vmap.Remove(V1);
    if (!vmap.Add(V2))
      vmap.Remove(V2);

    it.Next();
  }
  if (vmap.IsEmpty())
  { // closed
    TopoDS_Shape aLocalShape = V2.Oriented(TopAbs_FORWARD);
    Vfirst                   = TopoDS::Vertex(aLocalShape);
    aLocalShape              = V2.Oriented(TopAbs_REVERSED);
    Vlast                    = TopoDS::Vertex(aLocalShape);
    //    Vfirst = TopoDS::Vertex(V2.Oriented(TopAbs_FORWARD));
    //    Vlast  = TopoDS::Vertex(V2.Oriented(TopAbs_REVERSED));
  }
  else if (vmap.Extent() == 2)
  { // open
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator ite(vmap);

    while (ite.More() && ite.Key().Orientation() != TopAbs_FORWARD)
      ite.Next();
    if (ite.More())
      Vfirst = TopoDS::Vertex(ite.Key());
    ite.Initialize(vmap);
    while (ite.More() && ite.Key().Orientation() != TopAbs_REVERSED)
      ite.Next();
    if (ite.More())
      Vlast = TopoDS::Vertex(ite.Key());
  }
}

//=================================================================================================

bool TopExp::CommonVertex(const TopoDS_Edge& E1, const TopoDS_Edge& E2, TopoDS_Vertex& V)
{
  TopoDS_Vertex firstVertex1, lastVertex1, firstVertex2, lastVertex2;
  TopExp::Vertices(E1, firstVertex1, lastVertex1);
  TopExp::Vertices(E2, firstVertex2, lastVertex2);

  if (firstVertex1.IsSame(firstVertex2) || firstVertex1.IsSame(lastVertex2))
  {
    V = firstVertex1;
    return true;
  }
  if (lastVertex1.IsSame(firstVertex2) || lastVertex1.IsSame(lastVertex2))
  {
    V = lastVertex1;
    return true;
  }
  return false;
} // CommonVertex
