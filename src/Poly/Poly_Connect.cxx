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

#include <Poly_Connect.hxx>

#include <NCollection_IncAllocator.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>

// this structure records one of the edges starting from a node
struct polyedge
{
  polyedge* next;         // the next edge in the list
  Standard_Integer nt[2]; // the two adjacent triangles
  Standard_Integer nn[2]; // the two adjacent nodes
  Standard_Integer nd;    // the second node of the edge
  DEFINE_STANDARD_ALLOC
};

//=======================================================================
//function : Poly_Connect
//purpose  :
//=======================================================================
Poly_Connect::Poly_Connect()
: mytr    (0),
  myfirst (0),
  mynode  (0),
  myothernode (0),
  mysense (false),
  mymore  (false)
{
  //
}

//=======================================================================
//function : Poly_Connect
//purpose  :
//=======================================================================
Poly_Connect::Poly_Connect(const Handle(Poly_Triangulation)& theTriangulation)
: myTriangulation (theTriangulation),
  myTriangles (1, theTriangulation->NbNodes()),
  myAdjacents (1, 6 * theTriangulation->NbTriangles()),
  mytr    (0),
  myfirst (0),
  mynode  (0),
  myothernode (0),
  mysense (false),
  mymore  (false)
{
  Load (theTriangulation);
}

//=======================================================================
//function : Load
//purpose  :
//=======================================================================
void Poly_Connect::Load (const Handle(Poly_Triangulation)& theTriangulation)
{
  myTriangulation = theTriangulation;
  mytr = 0;
  myfirst = 0;
  mynode  = 0;
  myothernode = 0;
  mysense = false;
  mymore  = false;

  const Standard_Integer aNbNodes = myTriangulation->NbNodes();
  const Standard_Integer aNbTris  = myTriangulation->NbTriangles();
  {
    const Standard_Integer aNbAdjs = 6 * aNbTris;
    if (myTriangles.Size() != aNbNodes)
    {
      myTriangles.Resize (1, aNbNodes, Standard_False);
    }
    if (myAdjacents.Size() != aNbAdjs)
    {
      myAdjacents.Resize (1, aNbAdjs, Standard_False);
    }
  }

  myTriangles.Init(0);
  myAdjacents.Init(0);

  // We first build an array of the list of edges connected to the nodes
  // create an array to store the edges starting from the vertices
  NCollection_Array1<polyedge*> anEdges (1, aNbNodes);
  anEdges.Init (NULL);
  // use incremental allocator for small allocations
  Handle(NCollection_IncAllocator) anIncAlloc = new NCollection_IncAllocator();

  // loop on the triangles
  NCollection_Vec3<Standard_Integer> aTriNodes;
  NCollection_Vec2<Standard_Integer> anEdgeNodes;
  for (Standard_Integer aTriIter = 1; aTriIter <= aNbTris; ++aTriIter)
  {
    // get the nodes
    myTriangulation->Triangle (aTriIter).Get (aTriNodes[0], aTriNodes[1], aTriNodes[2]);

    // Update the myTriangles array
    myTriangles.SetValue (aTriNodes[0], aTriIter);
    myTriangles.SetValue (aTriNodes[1], aTriIter);
    myTriangles.SetValue (aTriNodes[2], aTriIter);

    // update the edge lists
    for (Standard_Integer aNodeInTri = 0; aNodeInTri < 3; ++aNodeInTri)
    {
      const Standard_Integer aNodeNext = (aNodeInTri + 1) % 3;  // the following node of the edge
      if (aTriNodes[aNodeInTri] < aTriNodes[aNodeNext])
      {
        anEdgeNodes[0] = aTriNodes[aNodeInTri];
        anEdgeNodes[1] = aTriNodes[aNodeNext];
      }
      else
      {
        anEdgeNodes[0] = aTriNodes[aNodeNext];
        anEdgeNodes[1] = aTriNodes[aNodeInTri];
      }

      // edge from node 0 to node 1 with node 0 < node 1
      // insert in the list of node 0
      polyedge* ced = anEdges[anEdgeNodes[0]];
      for (; ced != NULL; ced = ced->next)
      {
        // the edge already exists
        if (ced->nd == anEdgeNodes[1])
        {
          // just mark the adjacency if found
          ced->nt[1] = aTriIter;
          ced->nn[1] = aTriNodes[3 - aNodeInTri - aNodeNext];  // the third node
          break;
        }
      }

      if (ced == NULL)
      {
        // create the edge if not found
        ced = (polyedge* )anIncAlloc->Allocate (sizeof(polyedge));
        ced->next = anEdges[anEdgeNodes[0]];
        anEdges[anEdgeNodes[0]] = ced;
        ced->nd = anEdgeNodes[1];
        ced->nt[0] = aTriIter;
        ced->nn[0] = aTriNodes[3 - aNodeInTri - aNodeNext];  // the third node
        ced->nt[1] = 0;
        ced->nn[1] = 0;
      }
    }
  }

  // now complete the myAdjacents array
  Standard_Integer anAdjIndex = 1;
  for (Standard_Integer aTriIter = 1; aTriIter <= aNbTris; ++aTriIter)
  {
    // get the nodes
    myTriangulation->Triangle (aTriIter).Get (aTriNodes[0], aTriNodes[1], aTriNodes[2]);

    // for each edge in triangle
    for (Standard_Integer aNodeInTri = 0; aNodeInTri < 3; ++aNodeInTri)
    {
      const Standard_Integer aNodeNext = (aNodeInTri + 1) % 3;  // the following node of the edge
      if (aTriNodes[aNodeInTri] < aTriNodes[aNodeNext])
      {
        anEdgeNodes[0] = aTriNodes[aNodeInTri];
        anEdgeNodes[1] = aTriNodes[aNodeNext];
      }
      else
      {
        anEdgeNodes[0] = aTriNodes[aNodeNext];
        anEdgeNodes[1] = aTriNodes[aNodeInTri];
      }

      // edge from node 0 to node 1 with node 0 < node 1
      // find in the list of node 0
      const polyedge* ced = anEdges[anEdgeNodes[0]];
      while (ced->nd != anEdgeNodes[1])
      {
        ced = ced->next;
      }

      // Find the adjacent triangle
      const Standard_Integer l = ced->nt[0] == aTriIter ? 1 : 0;

      myAdjacents.SetValue (anAdjIndex,     ced->nt[l]);
      myAdjacents.SetValue (anAdjIndex + 3, ced->nn[l]);
      ++anAdjIndex;
    }
    anAdjIndex += 3;
  }

  // destroy the edges array - can be skipped when using NCollection_IncAllocator
  /*for (Standard_Integer aNodeIter = anEdges.Lower(); aNodeIter <= anEdges.Upper(); ++aNodeIter)
  {
    for (polyedge* anEdgeIter = anEdges[aNodeIter]; anEdgeIter != NULL;)
    {
      polyedge* aTmp = anEdgeIter->next;
      anIncAlloc->Free (anEdgeIter);
      anEdgeIter = aTmp;
    }
  }*/
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Poly_Connect::Initialize(const Standard_Integer N)
{
  mynode = N;
  myfirst = Triangle(N);
  mytr = myfirst;
  mysense = Standard_True;
  mymore = (myfirst != 0);
  if (mymore)
  {
    Standard_Integer i, no[3];
    const Poly_Array1OfTriangle& triangles = myTriangulation->Triangles();
    triangles(myfirst).Get(no[0], no[1], no[2]);
    for (i = 0; i < 3; i++)
      if (no[i] == mynode) break;
    myothernode = no[(i+2)%3];
  }
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void Poly_Connect::Next()
{
  Standard_Integer i, j;
  Standard_Integer n[3];
  Standard_Integer t[3];
  const Poly_Array1OfTriangle& triangles = myTriangulation->Triangles();
  Triangles(mytr, t[0], t[1], t[2]);
  if (mysense) {
    for (i = 0; i < 3; i++) {
      if (t[i] != 0) {
	triangles(t[i]).Get(n[0], n[1], n[2]);
	for (j = 0; j < 3; j++) {
	  if ((n[j] == mynode) && (n[(j+1)%3] == myothernode)) {
	    mytr = t[i];
	    myothernode = n[(j+2)%3];
	    mymore = (mytr != myfirst);
	    return;
	  }
	}
      }
    }
    // sinon, depart vers la gauche.
    triangles(myfirst).Get(n[0], n[1], n[2]);
    for (i = 0; i < 3; i++)
      if (n[i] == mynode) break;
    myothernode = n[(i+1)%3];
    mysense = Standard_False;
    mytr = myfirst;
    Triangles(mytr, t[0], t[1], t[2]);
  }
  if (!mysense) {
    for (i = 0; i < 3; i++) {
      if (t[i] != 0) {
	triangles(t[i]).Get(n[0], n[1], n[2]);
	for (j = 0; j < 3; j++) {
	  if ((n[j] == mynode) && (n[(j+2)%3] == myothernode)) {
	    mytr = t[i];
	    myothernode = n[(j+1)%3];
	    mymore = Standard_True;
	    return;
	  }
	}
      }
    }
  }
  mymore = Standard_False;
}
