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

#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>

// this structure records one of the edges starting from a node
struct polyedge
{
  polyedge* next;         // the next edge in the list
  Standard_Integer nd;    // the second node of the edge
  Standard_Integer nt[2]; // the two adjacent triangles
  Standard_Integer nn[2]; // the two adjacent nodes
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

  const Standard_Integer nbNodes     = myTriangulation->NbNodes();
  const Standard_Integer nbTriangles = myTriangulation->NbTriangles();
  {
    const Standard_Integer aNbAdjs = 6 * nbTriangles;
    if (myTriangles.Size() != nbNodes)
    {
      myTriangles.Resize (1, nbNodes, Standard_False);
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
  Standard_Integer i;
  // the last node is not used because edges are stored at the lower node index
  polyedge** edges = new polyedge*[nbNodes];
  for (i = 0; i < nbNodes; i++) edges[i] = 0;


  // loop on the triangles
  Standard_Integer j,k,n[3],n1,n2;
  const Poly_Array1OfTriangle& triangles = myTriangulation->Triangles();

  for (i = 1; i <= nbTriangles; i++) {

    // get the nodes
    triangles(i).Get(n[0],n[1],n[2]);

    // Update the myTriangles array
    myTriangles(n[0]) = i;
    myTriangles(n[1]) = i;
    myTriangles(n[2]) = i;

    // update the edge lists
    for (j = 0; j < 3; j++) {
      k = (j+1) % 3;  // the following node of the edge
      if (n[j] <= n[k]) {
	n1 = n[j];
	n2 = n[k];
      }
      else {
	n1 = n[k];
	n2 = n[j];
      }

      // edge from n1 to n2 with n1 < n2
      // insert in the list of n1

      polyedge* ced = edges[n1];
      while (ced != 0) {
	// the edge already exists
	if (ced->nd == n2)
	  break;
	else
	  ced = ced->next;
      }

      if (ced == 0) {
	// create the edge if not found
	ced = new polyedge;
	ced->next = edges[n1];
	edges[n1] = ced;
	ced->nd = n2;
	ced->nt[0] = i;
	ced->nn[0] = n[3-j-k];  // the third node
	ced->nt[1] = 0;
	ced->nn[1] = 0;
      }
      else {
	// just mark the adjacency if found
	ced->nt[1] = i;
	ced->nn[1] = n[3-j-k];  // the third node
      }
    }
  }


  // now complete the myAdjacents array
  
  Standard_Integer index = 1;
  for (i = 1; i <= nbTriangles; i++) {
    
    // get the nodes
    triangles(i).Get(n[0],n[1],n[2]);

    // fore each edge
    for (j = 0; j < 3; j++) {
      k = (j+1) % 3;  // the following node of the edge
      if (n[j] <= n[k]) {
	n1 = n[j];
	n2 = n[k];
      }
      else {
	n1 = n[k];
	n2 = n[j];
      }

      // edge from n1 to n2 with n1 < n2
      // find in the list of n1

      polyedge* ced = edges[n1];
      while (ced->nd != n2)
	ced = ced->next;

      // Find the adjacent triangle
      Standard_Integer l = 0;
      if (ced->nt[0] == i) l = 1;
      
      myAdjacents(index)   = ced->nt[l];
      myAdjacents(index+3) = ced->nn[l];
      index++;
    }
    index += 3;
  }

  // destroy the edges array
  for (i = 0; i < nbNodes; i++) {
    polyedge* ced = edges[i];
    while (ced != 0) {
      polyedge* tmp = ced->next;
      delete ced;
      ced = tmp;
    }
  }
  delete [] edges;
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
