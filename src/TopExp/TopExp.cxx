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
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : MapShapes
//purpose  : 
//=======================================================================
void TopExp::MapShapes(const TopoDS_Shape& S,
		       const TopAbs_ShapeEnum T,
		       TopTools_IndexedMapOfShape& M)
{
  TopExp_Explorer Ex(S,T);
  while (Ex.More()) {
    M.Add(Ex.Current());
    Ex.Next();
  }
}

//=======================================================================
//function : MapShapes
//purpose  : 
//=======================================================================

void TopExp::MapShapes(const TopoDS_Shape& S,
		       TopTools_IndexedMapOfShape& M)
{
  M.Add(S);
  TopoDS_Iterator It(S);
  while (It.More()) {
    MapShapes(It.Value(),M);
    It.Next();
  }
}

//=======================================================================
//function : MapShapesAndAncestors
//purpose  : 
//=======================================================================

void  TopExp::MapShapesAndAncestors
  (const TopoDS_Shape& S, 
   const TopAbs_ShapeEnum TS, 
   const TopAbs_ShapeEnum TA, 
   TopTools_IndexedDataMapOfShapeListOfShape& M)
{
  TopTools_ListOfShape empty;
  
  // visit ancestors
  TopExp_Explorer exa(S,TA);
  while (exa.More()) {
    // visit shapes
    const TopoDS_Shape& anc = exa.Current();
    TopExp_Explorer exs(anc,TS);
    while (exs.More()) {
      Standard_Integer index = M.FindIndex(exs.Current());
      if (index == 0) index = M.Add(exs.Current(),empty);
      M(index).Append(anc);
      exs.Next();
    }
    exa.Next();
  }
  
  // visit shapes not under ancestors
  TopExp_Explorer ex(S,TS,TA);
  while (ex.More()) {
    Standard_Integer index = M.FindIndex(ex.Current());
    if (index == 0) index = M.Add(ex.Current(),empty);
    ex.Next();
  }
}

//=======================================================================
//function : MapShapesAndUniqueAncestors
//purpose  : 
//=======================================================================

void  TopExp::MapShapesAndUniqueAncestors
  (const TopoDS_Shape& S, 
   const TopAbs_ShapeEnum TS, 
   const TopAbs_ShapeEnum TA, 
   TopTools_IndexedDataMapOfShapeListOfShape& M,
   const Standard_Boolean useOrientation)
{
  TopTools_ListOfShape empty;
  
  // visit ancestors
  TopExp_Explorer exa(S,TA);
  while (exa.More())
  {
    // visit shapes
    const TopoDS_Shape& anc = exa.Current();
    TopExp_Explorer exs(anc,TS);
    while (exs.More())
    {
      Standard_Integer index = M.FindIndex(exs.Current());
      if (index == 0)
        index = M.Add(exs.Current(),empty);
      TopTools_ListOfShape& aList = M(index);
      // check if anc already exists in a list
      TopTools_ListIteratorOfListOfShape it(aList);
      for (; it.More(); it.Next())
        if (useOrientation? anc.IsEqual(it.Value()) : anc.IsSame(it.Value()))
          break;
      if (!it.More())
        aList.Append(anc);
      exs.Next();
    }
    exa.Next();
  }

  // visit shapes not under ancestors
  TopExp_Explorer ex(S,TS,TA);
  while (ex.More())
  {
    Standard_Integer index = M.FindIndex(ex.Current());
    if (index == 0) 
      M.Add(ex.Current(),empty);
    ex.Next();
  }
}

//=======================================================================
//function : FirstVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex  TopExp::FirstVertex(const TopoDS_Edge& E,
				   const Standard_Boolean CumOri)
{
  TopoDS_Iterator ite(E,CumOri);
  while (ite.More()) {
    if (ite.Value().Orientation() == TopAbs_FORWARD) 
      return TopoDS::Vertex(ite.Value());
    ite.Next();
  }
  return TopoDS_Vertex();
}


//=======================================================================
//function : LastVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex  TopExp::LastVertex(const TopoDS_Edge& E,
				  const Standard_Boolean CumOri)
{
  TopoDS_Iterator ite(E,CumOri);
  while (ite.More()) {
    if (ite.Value().Orientation() == TopAbs_REVERSED) 
      return TopoDS::Vertex(ite.Value());
    ite.Next();
  }
  return TopoDS_Vertex();
}


//=======================================================================
//function : Vertices
//purpose  : 
//=======================================================================

void  TopExp::Vertices(const TopoDS_Edge& E,
		       TopoDS_Vertex& Vfirst,
		       TopoDS_Vertex& Vlast,
		       const Standard_Boolean CumOri)
{
  // minor optimization for case when Vfirst and Vlast are non-null:
  // at least for VC++ 10, it is faster if we use boolean flags than 
  // if we nullify vertices at that point (see #27021)
  Standard_Boolean isFirstDefined = Standard_False;
  Standard_Boolean isLastDefined = Standard_False;
  
  TopoDS_Iterator ite(E, CumOri);
  while (ite.More()) {
    const TopoDS_Shape& aV = ite.Value();
    if (aV.Orientation() == TopAbs_FORWARD)
    {
      Vfirst = TopoDS::Vertex (aV);
      isFirstDefined = Standard_True;
    }
    else if (aV.Orientation() == TopAbs_REVERSED)
    {
      Vlast = TopoDS::Vertex (aV);
      isLastDefined = Standard_True;
    }
    ite.Next();
  }

  if (!isFirstDefined)
    Vfirst.Nullify();

  if (!isLastDefined)
    Vlast.Nullify();
}


//=======================================================================
//function : Vertices
//purpose  : 
//=======================================================================

void  TopExp::Vertices(const TopoDS_Wire& W,
		       TopoDS_Vertex& Vfirst,
		       TopoDS_Vertex& Vlast)
{
  Vfirst = Vlast = TopoDS_Vertex(); // nullify

  TopTools_MapOfShape vmap;
  TopoDS_Iterator it(W);
  TopoDS_Vertex   V1,V2;

  while (it.More()) {
    const TopoDS_Edge& E = TopoDS::Edge(it.Value());
    if (E.Orientation() == TopAbs_REVERSED)
      TopExp::Vertices(E,V2,V1);
    else
      TopExp::Vertices(E,V1,V2);
    // add or remove in the vertex map
    V1.Orientation(TopAbs_FORWARD);
    V2.Orientation(TopAbs_REVERSED);
    if (!vmap.Add(V1)) vmap.Remove(V1);
    if (!vmap.Add(V2)) vmap.Remove(V2);

    it.Next();
  }
  if (vmap.IsEmpty()) { // closed
    TopoDS_Shape aLocalShape = V2.Oriented(TopAbs_FORWARD);
    Vfirst = TopoDS::Vertex(aLocalShape);
    aLocalShape = V2.Oriented(TopAbs_REVERSED);
    Vlast  = TopoDS::Vertex(aLocalShape);
//    Vfirst = TopoDS::Vertex(V2.Oriented(TopAbs_FORWARD));
//    Vlast  = TopoDS::Vertex(V2.Oriented(TopAbs_REVERSED));
  }
  else if (vmap.Extent() == 2) { //open
    TopTools_MapIteratorOfMapOfShape ite(vmap);
    
    while (ite.More() && ite.Key().Orientation() != TopAbs_FORWARD) 
      ite.Next();
    if (ite.More()) Vfirst = TopoDS::Vertex(ite.Key());
    ite.Initialize(vmap);
    while (ite.More() && ite.Key().Orientation() != TopAbs_REVERSED) 
      ite.Next();
    if (ite.More()) Vlast = TopoDS::Vertex(ite.Key());
  }
}


//=======================================================================
//function : CommonVertex
//purpose  : 
//=======================================================================
Standard_Boolean TopExp::CommonVertex(const TopoDS_Edge& E1, 
				      const TopoDS_Edge& E2,
				      TopoDS_Vertex& V)
{
  TopoDS_Vertex firstVertex1, lastVertex1, firstVertex2, lastVertex2;
  TopExp::Vertices(E1, firstVertex1, lastVertex1);
  TopExp::Vertices(E2, firstVertex2, lastVertex2);

  if (firstVertex1.IsSame(firstVertex2) || 
      firstVertex1.IsSame(lastVertex2)) {
    V = firstVertex1;
    return Standard_True;
  }
  if (lastVertex1.IsSame(firstVertex2) ||
	   lastVertex1.IsSame(lastVertex2)) {
    V = lastVertex1;
    return Standard_True;
  }
  return Standard_False;
} // CommonVertex

