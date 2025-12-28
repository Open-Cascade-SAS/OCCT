// Created on: 1995-03-06
// Created by: Laurent PAINNOT
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

#ifndef _Poly_Connect_HeaderFile
#define _Poly_Connect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Poly_Triangulation;

//! Provides an algorithm to explore, inside a triangulation, the
//! adjacency data for a node or a triangle.
//! Adjacency data for a node consists of triangles which
//! contain the node.
//! Adjacency data for a triangle consists of:
//! -   the 3 adjacent triangles which share an edge of the triangle,
//! -   and the 3 nodes which are the other nodes of these adjacent triangles.
//! Example
//! Inside a triangulation, a triangle T
//! has nodes n1, n2 and n3.
//! It has adjacent triangles AT1, AT2 and AT3 where:
//! - AT1 shares the nodes n2 and n3,
//! - AT2 shares the nodes n3 and n1,
//! - AT3 shares the nodes n1 and n2.
//! It has adjacent nodes an1, an2 and an3 where:
//! - an1 is the third node of AT1,
//! - an2 is the third node of AT2,
//! - an3 is the third node of AT3.
//! So triangle AT1 is composed of nodes n2, n3 and an1.
//! There are two ways of using this algorithm.
//! -   From a given node you can look for one triangle that
//! passes through the node, then look for the triangles
//! adjacent to this triangle, then the adjacent nodes. You
//! can thus explore the triangulation step by step (functions
//! Triangle, Triangles and Nodes).
//! -   From a given node you can look for all the triangles
//! that pass through the node (iteration method, using the
//! functions Initialize, More, Next and Value).
//! A Connect object can be seen as a tool which analyzes a
//! triangulation and translates it into a series of triangles. By
//! doing this, it provides an interface with other tools and
//! applications working on basic triangles, and which do not
//! work directly with a Poly_Triangulation.
class Poly_Connect
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an uninitialized algorithm.
  Standard_EXPORT Poly_Connect();

  //! Constructs an algorithm to explore the adjacency data of
  //! nodes or triangles for the triangulation T.
  Standard_EXPORT Poly_Connect(const occ::handle<Poly_Triangulation>& theTriangulation);

  //! Initialize the algorithm to explore the adjacency data of
  //! nodes or triangles for the triangulation theTriangulation.
  Standard_EXPORT void Load(const occ::handle<Poly_Triangulation>& theTriangulation);

  //! Returns the triangulation analyzed by this tool.
  const occ::handle<Poly_Triangulation>& Triangulation() const { return myTriangulation; }

  //! Returns the index of a triangle containing the node at
  //! index N in the nodes table specific to the triangulation analyzed by this tool
  int Triangle(const int N) const { return myTriangles(N); }

  //! Returns in t1, t2 and t3, the indices of the 3 triangles
  //! adjacent to the triangle at index T in the triangles table
  //! specific to the triangulation analyzed by this tool.
  //! Warning
  //! Null indices are returned when there are fewer than 3
  //! adjacent triangles.
  void Triangles(const int T,
                 int&      t1,
                 int&      t2,
                 int&      t3) const
  {
    int index = 6 * (T - 1);
    t1                     = myAdjacents(index + 1);
    t2                     = myAdjacents(index + 2);
    t3                     = myAdjacents(index + 3);
  }

  //! Returns, in n1, n2 and n3, the indices of the 3 nodes
  //! adjacent to the triangle referenced at index T in the
  //! triangles table specific to the triangulation analyzed by this tool.
  //! Warning
  //! Null indices are returned when there are fewer than 3 adjacent nodes.
  void Nodes(const int T,
             int&      n1,
             int&      n2,
             int&      n3) const
  {
    int index = 6 * (T - 1);
    n1                     = myAdjacents(index + 4);
    n2                     = myAdjacents(index + 5);
    n3                     = myAdjacents(index + 6);
  }

public:
  //! Initializes an iterator to search for all the triangles
  //! containing the node referenced at index N in the nodes
  //! table, for the triangulation analyzed by this tool.
  //! The iterator is managed by the following functions:
  //! -   More, which checks if there are still elements in the iterator
  //! -   Next, which positions the iterator on the next element
  //! -   Value, which returns the current element.
  //! The use of such an iterator provides direct access to the
  //! triangles around a particular node, i.e. it avoids iterating on
  //! all the component triangles of a triangulation.
  //! Example
  //! Poly_Connect C(Tr);
  //! for
  //! (C.Initialize(n1);C.More();C.Next())
  //! {
  //! t = C.Value();
  //! }
  Standard_EXPORT void Initialize(const int N);

  //! Returns true if there is another element in the iterator
  //! defined with the function Initialize (i.e. if there is another
  //! triangle containing the given node).
  bool More() const { return mymore; }

  //! Advances the iterator defined with the function Initialize to
  //! access the next triangle.
  //! Note: There is no action if the iterator is empty (i.e. if the
  //! function More returns false).-
  Standard_EXPORT void Next();

  //! Returns the index of the current triangle to which the
  //! iterator, defined with the function Initialize, points. This is
  //! an index in the triangles table specific to the triangulation
  //! analyzed by this tool
  int Value() const { return mytr; }

private:
  occ::handle<Poly_Triangulation> myTriangulation;
  NCollection_Array1<int>    myTriangles;
  NCollection_Array1<int>    myAdjacents;
  int           mytr;
  int           myfirst;
  int           mynode;
  int           myothernode;
  bool           mysense;
  bool           mymore;
  TColStd_PackedMapOfInteger myPassedTr;
};

#endif // _Poly_Connect_HeaderFile
