// File:	Poly_Connect.cxx
// Created:	Mon Mar  6 16:30:09 1995
// Author:	Laurent PAINNOT
//		<lpa@metrox>


#include <Poly_Connect.ixx>
#include <Poly_Triangle.hxx>

#include <Standard.hxx>


//=======================================================================
//function : Poly_Connect
//purpose  : 
//=======================================================================
// this structure records one of the edges starting from a node

//typedef struct polyedge {
struct polyedge {
    polyedge* next;             // the next edge in the list
    Standard_Integer nd;    // the second node of the edge
    Standard_Integer nt[2]; // the two adjacent triangles
    Standard_Integer nn[2]; // the two adjacent nodes
    void* operator new(size_t aSize) 
      {return (void*)(Standard::Allocate(aSize));}
//    void  operator delete(void* aNode, size_t aSize) {
    void  operator delete(void* aNode) {
      Standard_Address anAdress = (Standard_Address)aNode;
      Standard::Free(anAdress);
    }
  };

Poly_Connect::Poly_Connect(const Handle(Poly_Triangulation)& T) :
    myTriangulation(T),
    myTriangles(1,T->NbNodes()),
    myAdjacents(1,6*T->NbTriangles())
{
  myTriangles.Init(0);
  myAdjacents.Init(0);
  Standard_Integer nbNodes     = myTriangulation->NbNodes();
  Standard_Integer nbTriangles = myTriangulation->NbTriangles();

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
//function : Triangles
//purpose  : 
//=======================================================================

void Poly_Connect::Triangles(const Standard_Integer T, 
			     Standard_Integer& t1, 
			     Standard_Integer& t2, 
			     Standard_Integer& t3) const 
{
  Standard_Integer index = 6*(T-1);
  t1 = myAdjacents(index+1);
  t2 = myAdjacents(index+2);
  t3 = myAdjacents(index+3);
}

//=======================================================================
//function : Nodes
//purpose  : 
//=======================================================================

void Poly_Connect::Nodes(const Standard_Integer T, 
			 Standard_Integer& n1, 
			 Standard_Integer& n2, 
			 Standard_Integer& n3) const 
{
  Standard_Integer index = 6*(T-1);
  n1 = myAdjacents(index+4);
  n2 = myAdjacents(index+5);
  n3 = myAdjacents(index+6);
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

  Standard_Integer i, no[3];
  const Poly_Array1OfTriangle& triangles = myTriangulation->Triangles();
  triangles(myfirst).Get(no[0], no[1], no[2]);
  for (i = 0; i < 3; i++)
    if (no[i] == mynode) break;
  myothernode = no[(i+2)%3];
  mysense = Standard_True;
  mymore = Standard_True;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void Poly_Connect::Next()
{
  Standard_Integer i, j;
  static Standard_Integer n[3];
  static Standard_Integer t[3];
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


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Integer Poly_Connect::Value() const
{
  return mytr;
}
