// Created on: 1999-03-08
// Created by: Fabrice SERVANT
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Adaptor3d_HSurface.hxx>
#include <Bnd_Box.hxx>
#include <IntPolyh_Edge.hxx>
#include <IntPolyh_Point.hxx>
#include <IntPolyh_Triangle.hxx>

#include <stdio.h>
#define MyTolerance 10.0e-7
#define MyConfusionPrecision 10.0e-12
#define SquareMyConfusionPrecision 10.0e-24

static
  void GetInfoTA(const Standard_Integer numP1,
		 const Standard_Integer numP2,
		 const Standard_Integer numTA,
		 const IntPolyh_ArrayOfTriangles & TTriangles,
		 Standard_Integer & numP3b,
		 Standard_Integer & P3bIndex,
		 Standard_Integer & Edge2b,
		 Standard_Integer & Edge3b);
static
  void NewTriangle(const Standard_Integer P1,
		   const Standard_Integer P2,
		   const Standard_Integer P3,
		   IntPolyh_ArrayOfTriangles &TTriangles,
		   const Handle(Adaptor3d_HSurface)& MySurface,
		   IntPolyh_ArrayOfPoints &TPoints);
static
  void NewEdge(const Standard_Integer P1,
	       const Standard_Integer P2,
	       const Standard_Integer T1,
	       const Standard_Integer T2,
	     IntPolyh_ArrayOfEdges & TEdges); 
static
  void OldEdge(const Standard_Integer EdgeN,
	       const Standard_Integer NumTri,
	       const Standard_Integer NewTriNum,
	       IntPolyh_ArrayOfEdges & TEdges) ;

//=======================================================================
//function : ComputeDeflection
//purpose  : Computes the deflection of the triangle.
//           It is computed as a distance between triangles plane and
//           barycenter of the triangle in UV space.
//=======================================================================
Standard_Real
  IntPolyh_Triangle::ComputeDeflection(const Handle(Adaptor3d_HSurface)& theSurface,
                                       const IntPolyh_ArrayOfPoints& TPoints)
{
  myDeflection = 0.;
  //
  const IntPolyh_Point & P1 = TPoints[myPoints[0]];
  const IntPolyh_Point & P2 = TPoints[myPoints[1]];
  const IntPolyh_Point & P3 = TPoints[myPoints[2]];
  //
  {
    // check if the triangle is not degenerated - no more than one point
    // has a degenerated flag
    Standard_Integer iDeg = (P1.Degenerated() ? 1 : 0) +
                            (P2.Degenerated() ? 1 : 0) +
                            (P3.Degenerated() ? 1 : 0);
    if (iDeg > 1) {
      myIsDegenerated = Standard_True;
      return myDeflection;
    }
  }
  //
  // Plane of the triangle
  IntPolyh_Point NormaleTri;
  NormaleTri.Cross(P2-P1,P3-P1);
  Standard_Real SqNorm = NormaleTri.SquareModulus();
  if (SqNorm < SquareMyConfusionPrecision) {
    // The triangle is degenerated
    myIsDegenerated = Standard_True;
    return myDeflection;
  }
  //
  // Compute point on the surface
  Standard_Real Gu=(P1.U()+P2.U()+P3.U())/3.0;
  Standard_Real Gv=(P1.V()+P2.V()+P3.V())/3.0;
  gp_Pnt PtXYZ = theSurface->Value( Gu, Gv);
  // Point on the surface
  IntPolyh_Point BarycentreReel(PtXYZ.X(), PtXYZ.Y(), PtXYZ.Z(), Gu, Gv);
  // compute distance to plane
  NormaleTri = NormaleTri / sqrt(SqNorm);
  myDeflection = Abs(NormaleTri.Dot(BarycentreReel - P1));
  return myDeflection;
}

//=======================================================================
//function : GetNextTriangle
//purpose  : 
//=======================================================================
Standard_Integer
  IntPolyh_Triangle::GetNextTriangle(const Standard_Integer theTriangle,
                                     const Standard_Integer theEdgeNum,
                                     const IntPolyh_ArrayOfEdges& TEdges) const
{
  Standard_Integer aNextTriangle = -1;
  if (theEdgeNum < 1 || theEdgeNum > 3) {
    return aNextTriangle;
  }
  //
  const IntPolyh_Edge & anEdge = TEdges[myEdges[theEdgeNum-1]];
  aNextTriangle = ((anEdge.FirstTriangle() == theTriangle) ?
    anEdge.SecondTriangle() : anEdge.FirstTriangle());
  return aNextTriangle;
}

//=======================================================================
//function : LinkEdges2Triangle
//purpose  : 
//=======================================================================
void IntPolyh_Triangle::LinkEdges2Triangle(const IntPolyh_ArrayOfEdges& TEdges,
                                           const Standard_Integer theEdge1,
                                           const Standard_Integer theEdge2,
                                           const Standard_Integer theEdge3)
{
  if (theEdge1 < 0 || theEdge2 < 0 || theEdge3 < 0) {
    return;
  }
  //
  myEdges[0] = theEdge1;
  myEdges[1] = theEdge2;
  myEdges[2] = theEdge3;
  //
  myEdgesOrientations[0] = ((TEdges[myEdges[0]].FirstPoint() == myPoints[0]) ? 1 : -1);
  myEdgesOrientations[1] = ((TEdges[myEdges[1]].FirstPoint() == myPoints[1]) ? 1 : -1);
  myEdgesOrientations[2] = ((TEdges[myEdges[2]].FirstPoint() == myPoints[2]) ? 1 : -1);
}

//=======================================================================
//function : GetInfoTA
//purpose  : 
//=======================================================================
void GetInfoTA(const Standard_Integer numP1,
	       const Standard_Integer numP2,
	       const Standard_Integer numTA,
	       const IntPolyh_ArrayOfTriangles & TTriangles,
	       Standard_Integer & numP3b,
	       Standard_Integer & P3bIndex,
	       Standard_Integer & Edge2b,
	       Standard_Integer & Edge3b)
{
  /// On veut savoir quel est le troisieme point du triangle
  /// adjacent (TriAdj) et quel sont les edges partant de ce point
  const IntPolyh_Triangle & TriAdj=TTriangles[numTA];
  Standard_Integer P1b=TriAdj.FirstPoint();
  Standard_Integer P2b=TriAdj.SecondPoint();
  Standard_Integer P3b=TriAdj.ThirdPoint();

  if ( (P1b!=numP1)&&(P1b!=numP2) ) { 
    numP3b=P1b; 
    P3bIndex=1;
    if (P2b==numP1) {
      ///P1bP2b==numP3bnumP1:Edge3b donc dans ce cas
     Edge3b=TriAdj.FirstEdge();
     /// Donc P1bP3b==numP3bnumP2:Edge2b
     Edge2b=TriAdj.ThirdEdge();
   }
    else {
     Edge2b=TriAdj.FirstEdge();
     Edge3b=TriAdj.ThirdEdge();
    }
  }
  else if( (P2b!=numP1)&&(P2b!=numP2) ) { 
    numP3b=P2b; 
    P3bIndex=2;
    if (P1b==numP1) {
      ///P2bP1b==numP3bnumP1:Edge3b donc dans ce cas
     Edge3b=TriAdj.FirstEdge();
     /// Donc P2bP3b==numP3bnumP2:Edge2b
     Edge2b=TriAdj.SecondEdge();
   }
    else {
     Edge2b=TriAdj.FirstEdge();
     Edge3b=TriAdj.SecondEdge();
    }
  }
  else if( (P3b!=numP1)&&(P3b!=numP2) ) { 
    numP3b=P3b; 
    P3bIndex=3; 
    if (P2b==numP1) {
      ///P3bP2b==numP3bnumP1:Edge3b donc dans ce cas
     Edge3b=TriAdj.SecondEdge();
     /// Donc P3bP1b==numP3bnumP2:Edge2b
     Edge2b=TriAdj.ThirdEdge();
   }
    else {
     Edge2b=TriAdj.SecondEdge();
     Edge3b=TriAdj.ThirdEdge();
    }
  }      
}

//=======================================================================
//function : NewTriangle
//purpose  : 
//=======================================================================
void NewTriangle(const Standard_Integer P1,
		 const Standard_Integer P2,
		 const Standard_Integer P3,
		 IntPolyh_ArrayOfTriangles &TTriangles,
		 const Handle(Adaptor3d_HSurface)& MySurface,
		 IntPolyh_ArrayOfPoints &TPoints) {
  const Standard_Integer FinTT = TTriangles.NbItems();
  TTriangles[FinTT].SetFirstPoint(P1);
  TTriangles[FinTT].SetSecondPoint(P2);
  TTriangles[FinTT].SetThirdPoint(P3);
  TTriangles[FinTT].ComputeDeflection(MySurface, TPoints);
  TTriangles.IncrementNbItems();
}

//=======================================================================
//function : NewEdge
//purpose  : 
//=======================================================================
void NewEdge(const Standard_Integer P1,
	     const Standard_Integer P2,
	     const Standard_Integer T1,
	     const Standard_Integer T2,
	     IntPolyh_ArrayOfEdges & TEdges)
{

  const Standard_Integer FinTE = TEdges.NbItems();

  TEdges[FinTE].SetFirstPoint(P1);
  TEdges[FinTE].SetSecondPoint(P2);
  TEdges[FinTE].SetFirstTriangle(T1);
  TEdges[FinTE].SetSecondTriangle(T2);
  TEdges.IncrementNbItems();
}

//=======================================================================
//function : OldEdge
//purpose  : 
//=======================================================================
void OldEdge(const Standard_Integer EdgeN,
	     const Standard_Integer NumTri,
	     const Standard_Integer NewTriNum,
	     IntPolyh_ArrayOfEdges & TEdges) 
{
  if(TEdges[EdgeN].FirstTriangle()==NumTri){
    TEdges[EdgeN].SetFirstTriangle(NewTriNum);
  }
  else{
    TEdges[EdgeN].SetSecondTriangle(NewTriNum);
  }
}

//=======================================================================
//function : MiddleRefinement
//purpose  : 
//=======================================================================
void IntPolyh_Triangle::MiddleRefinement(const Standard_Integer NumTri,
                                         const Handle(Adaptor3d_HSurface)& MySurface,
                                         IntPolyh_ArrayOfPoints &TPoints,
                                         IntPolyh_ArrayOfTriangles &TTriangles,
                                         IntPolyh_ArrayOfEdges & TEdges)
{

  Standard_Integer FinTE = TEdges.NbItems();
  Standard_Integer FinTT = TTriangles.NbItems();

  // Refinement of the mesh by the middle of the largest dimensions
  Standard_Integer numP1 = FirstPoint();
  Standard_Integer numP2 = SecondPoint();
  Standard_Integer numP3 = ThirdPoint();

  const IntPolyh_Point& P1 = TPoints[numP1];
  const IntPolyh_Point& P2 = TPoints[numP2];
  const IntPolyh_Point& P3 = TPoints[numP3];

  // compute the largest dimension
  Standard_Real L12 = P1.SquareDistance(P2);
  Standard_Real L23 = P2.SquareDistance(P3);
  Standard_Real L31 = P3.SquareDistance(P1);

  if ((L12>L23) && (L12>L31)) {
    const Standard_Integer FinTP = TPoints.NbItems();
    (TPoints[FinTP]).Middle( MySurface,P1, P2);
    
    ///les nouveaux triangles
    Standard_Integer T1,T2,T3,T4;

    T1=FinTT;
    NewTriangle(numP2,numP3,FinTP,TTriangles,MySurface,TPoints);
    FinTT++;
    T2=FinTT;;
    NewTriangle(numP3,numP1,FinTP,TTriangles,MySurface,TPoints);
    FinTT++;

    ///***AFFINAGE DU TRIANGLE ADJACENT***

    Standard_Integer numTA = GetNextTriangle(NumTri,1,TEdges);

    if (numTA>=0) {
      Standard_Integer numP3b = -1;
      Standard_Integer P3bIndex = -1;

      Standard_Integer Edge2b = -1;
      Standard_Integer Edge3b = -1;
      
      GetInfoTA(numP1,numP2,numTA,TTriangles,numP3b,P3bIndex,Edge2b,Edge3b);
      
      T3=FinTT;
      NewTriangle(numP2,numP3b,FinTP,TTriangles,MySurface,TPoints);
      FinTT++;
      T4=FinTT;
      NewTriangle(numP3b,numP1,FinTP,TTriangles,MySurface,TPoints);

      ///On cree les nouveaux edges
      Standard_Integer E1,E2,E3,E4;
      
      E1=FinTE;
      NewEdge(numP1,FinTP,T2,T4,TEdges);
      FinTE++;
      E2=FinTE;
      NewEdge(FinTP,numP2,T1,T3,TEdges);
      FinTE++;
      E3=FinTE;
      NewEdge(FinTP,numP3,T1,T2,TEdges);
      FinTE++;
      E4=FinTE;
      NewEdge(FinTP,numP3b,T3,T4,TEdges);

      ///On met a jour les anciens edges
      OldEdge(myEdges[1],NumTri,T1,TEdges);
      OldEdge(myEdges[2],NumTri,T2,TEdges);
      OldEdge(Edge2b,numTA,T3,TEdges);
      OldEdge(Edge3b,numTA,T4,TEdges);
      
      /// On remplit les nouveaux triangles avec les edges
      TTriangles[T1].LinkEdges2Triangle(TEdges,myEdges[1],E3,E2);
      TTriangles[T2].LinkEdges2Triangle(TEdges,myEdges[2],E1,E3);
      TTriangles[T3].LinkEdges2Triangle(TEdges,Edge2b,E4,E2);
      TTriangles[T4].LinkEdges2Triangle(TEdges,Edge3b,E1,E4);

      ///On tue le triangle adjacent
      TTriangles[numTA].SetDeflection(-1.0);
      TTriangles[numTA].SetIntersectionPossible(Standard_False);

    }
    else { ///seulement deux nouveaux triangles
      //on cree les nouveaux edges avec T1 et T2
      Standard_Integer E1,E2,E3;
      
      E1=FinTE;
      NewEdge(numP1,FinTP,T2,-1,TEdges);
      FinTE++;
      E2=FinTE;
      NewEdge(FinTP,numP2,T1,-1,TEdges);
      FinTE++;
      E3=FinTE;
      NewEdge(FinTP,numP3,T1,T2,TEdges);

      ///On met a jour les anciens edges
      OldEdge(myEdges[1],NumTri,T1,TEdges);
      OldEdge(myEdges[2],NumTri,T2,TEdges);
      
      /// On remplit les nouveaux triangles avec les edges
      TTriangles[T1].LinkEdges2Triangle(TEdges,myEdges[1],E3,E2);
      TTriangles[T2].LinkEdges2Triangle(TEdges,myEdges[2],E1,E3);
    }
  }
  
  else if ((L23>L31) && (L23>L12)){
    const Standard_Integer FinTP = TPoints.NbItems();
    (TPoints[FinTP]).Middle(MySurface, P2,P3);

    ///les nouveaux triangles
    Standard_Integer T1,T2,T3,T4;    
    
    T1=FinTT;
    NewTriangle(numP1,numP2,FinTP,TTriangles,MySurface,TPoints);
    FinTT++;
    T2=FinTT;
    NewTriangle(numP3,numP1,FinTP,TTriangles,MySurface,TPoints);
    FinTT++;
    
    ///*RAFFINAGE DU TRIANGLE ADJACENT***

    Standard_Integer numTA = GetNextTriangle(NumTri,2,TEdges);

    if (numTA>=0) {
      Standard_Integer numP1b=-1;
      Standard_Integer P1bIndex = -1;

      Standard_Integer Edge1b = -1;
      Standard_Integer Edge3b = -1;

      GetInfoTA(numP2,numP3,numTA,TTriangles,numP1b,P1bIndex,Edge3b,Edge1b);     

      T3=FinTT;
      NewTriangle(numP2,numP1b,FinTP,TTriangles,MySurface,TPoints);
      FinTT++;
      T4=FinTT;
      NewTriangle(numP1b,numP3,FinTP,TTriangles,MySurface,TPoints);

      ///Nouveaux Edges
      Standard_Integer E1,E2,E3,E4;

      E1=FinTE;
      NewEdge(numP2,FinTP,T1,T3,TEdges);
      FinTE++;
      E2=FinTE;
      NewEdge(FinTP,numP3,T2,T4,TEdges);
      FinTE++;
      E3=FinTE;
      NewEdge(FinTP,numP1,T1,T2,TEdges);
      FinTE++;
      E4=FinTE;
      NewEdge(FinTP,numP1b,T3,T4,TEdges);

      ///On met a jour les anciens edges
      OldEdge(myEdges[0],NumTri,T1,TEdges);
      OldEdge(myEdges[2],NumTri,T2,TEdges);
      OldEdge(Edge1b,numTA,T3,TEdges);
      OldEdge(Edge3b,numTA,T4,TEdges);
      
      /// On remplit les nouveaux triangles avec les edges
      TTriangles[T1].LinkEdges2Triangle(TEdges,myEdges[0],E1,E3);
      TTriangles[T2].LinkEdges2Triangle(TEdges,myEdges[2],E3,E2);
      TTriangles[T3].LinkEdges2Triangle(TEdges,Edge1b,E4,E1);
      TTriangles[T4].LinkEdges2Triangle(TEdges,Edge3b,E2,E4);

      ///On tue le triangle adjacent
      TTriangles[numTA].SetDeflection(-1.0);
      TTriangles[numTA].SetIntersectionPossible(Standard_False);
    }
    else { ///seulement deux nouveaux triangles
      ///Nouveaux Edges
      Standard_Integer E1,E2,E3;

      E1=FinTE;
      NewEdge(numP2,FinTP,T1,-1,TEdges);
      FinTE++;
      E2=FinTE;
      NewEdge(FinTP,numP3,T2,-1,TEdges);
      FinTE++;
      E3=FinTE;
      NewEdge(FinTP,numP1,T1,T2,TEdges);

      ///On met a jour les anciens edges
      OldEdge(myEdges[0],NumTri,T1,TEdges);
      OldEdge(myEdges[2],NumTri,T2,TEdges);
      
      /// On remplit les nouveaux triangles avec les edges
      TTriangles[T1].LinkEdges2Triangle(TEdges,myEdges[0],E1,E3);
      TTriangles[T2].LinkEdges2Triangle(TEdges,myEdges[2],E3,E2);
    }
  }
    else {
    const Standard_Integer FinTP = TPoints.NbItems();
    (TPoints[FinTP]).Middle(MySurface, P3,P1);

    Standard_Integer T1,T2,T3,T4;
    
    T1=FinTT;
    NewTriangle(numP1,numP2,FinTP,TTriangles,MySurface,TPoints);
    FinTT++;
    T2=FinTT;
    NewTriangle(numP2,numP3,FinTP,TTriangles,MySurface,TPoints);
    FinTT++;
    
    ///*RAFFINAGE DU TRIANGLE ADJACENT***

    Standard_Integer numTA = GetNextTriangle(NumTri,3,TEdges);

    if (numTA>=0) {

      Standard_Integer numP2b = -1;
      Standard_Integer P2bIndex = -1;
      
      Standard_Integer Edge1b = -1;
      Standard_Integer Edge2b = -1;
     
      GetInfoTA(numP3,numP1,numTA,TTriangles,numP2b,P2bIndex,Edge1b,Edge2b);

      T3=FinTT;
      NewTriangle(numP1,numP2b,FinTP,TTriangles,MySurface,TPoints);
      FinTT++;
      T4=FinTT;
      NewTriangle(numP2b,numP3,FinTP,TTriangles,MySurface,TPoints);

      ///Nouveaux Edges
      Standard_Integer E1,E2,E3,E4;

      E1=FinTE;
      NewEdge(numP2,FinTP,T1,T2,TEdges);
      FinTE++;
      E2=FinTE;
      NewEdge(FinTP,numP3,T2,T4,TEdges);
      FinTE++;
      E3=FinTE;
      NewEdge(FinTP,numP2b,T4,T3,TEdges);
      FinTE++;
      E4=FinTE;
      NewEdge(FinTP,numP1,T1,T3,TEdges);

      ///On met a jour les anciens edges
      OldEdge(myEdges[0],NumTri,T1,TEdges);
      OldEdge(myEdges[1],NumTri,T2,TEdges);
      OldEdge(Edge1b,numTA,T3,TEdges);
      OldEdge(Edge2b,numTA,T4,TEdges);
      
      /// On remplit les nouveaux triangles avec les edges
      TTriangles[T1].LinkEdges2Triangle(TEdges,myEdges[0],E1,E4);
      TTriangles[T2].LinkEdges2Triangle(TEdges,myEdges[1],E2,E1);
      TTriangles[T3].LinkEdges2Triangle(TEdges,Edge1b,E3,E4);
      TTriangles[T4].LinkEdges2Triangle(TEdges,Edge2b,E2,E3);

      ///On tue le triangle adjacent
      TTriangles[numTA].SetDeflection(-1.0);
      TTriangles[numTA].SetIntersectionPossible(Standard_False);
    }
    else { ///seulement deux nouveaux triangles
      ///Nouveaux Edges
      Standard_Integer E1,E2,E4;

      E1=FinTE;
      NewEdge(numP2,FinTP,T1,T2,TEdges);
      FinTE++;
      E2=FinTE;
      NewEdge(FinTP,numP3,T2,-1,TEdges);
      FinTE++;
      E4=FinTE;
      NewEdge(FinTP,numP1,T1,-1,TEdges);

      ///On met a jour les anciens edges
      OldEdge(myEdges[0],NumTri,T1,TEdges);
      OldEdge(myEdges[1],NumTri,T2,TEdges);

      /// On remplit les nouveaux triangles avec les edges
      TTriangles[T1].LinkEdges2Triangle(TEdges,myEdges[0],E1,E4);
      TTriangles[T2].LinkEdges2Triangle(TEdges,myEdges[1],E2,E1);
    }
  }
  TPoints.IncrementNbItems();

  // make the triangle obsolete
  myDeflection = -1.0;
  myIsIntersectionPossible = Standard_False;
}

//=======================================================================
//function : MultipleMiddleRefinement
//purpose  : 
//=======================================================================
void IntPolyh_Triangle::MultipleMiddleRefinement(const Standard_Real theRefineCriterion,
                                                 const Bnd_Box& theBox,
                                                 const Standard_Integer theTriangleNumber,
                                                 const Handle(Adaptor3d_HSurface)& theSurface,
                                                 IntPolyh_ArrayOfPoints& TPoints,
                                                 IntPolyh_ArrayOfTriangles& TTriangles,
                                                 IntPolyh_ArrayOfEdges& TEdges)
{
  // Number of triangles before refinement of current triangle
  const Standard_Integer FinTTInit = TTriangles.NbItems();
  // Split the current triangle
  MiddleRefinement(theTriangleNumber, theSurface, TPoints, TTriangles, TEdges);
  // Refine the new triangles
  for (Standard_Integer i = FinTTInit; i < TTriangles.NbItems(); ++i) {
    IntPolyh_Triangle& aTriangle = TTriangles[i];
    if(theBox.IsOut(aTriangle.BoundingBox(TPoints))) {
      aTriangle.SetIntersectionPossible(Standard_False);
    }
    else if (aTriangle.Deflection() > theRefineCriterion) {
      aTriangle.MiddleRefinement(i, theSurface, TPoints, TTriangles, TEdges);
    }
  }
}

//=======================================================================
//function : SetEdgeAndOrientation
//purpose  : 
//=======================================================================
void IntPolyh_Triangle::SetEdgeAndOrientation(const IntPolyh_Edge& theEdge,
                                              const Standard_Integer theEdgeIndex)
{
  // Points on the edge - pe1, pe2
  Standard_Integer pe1 = theEdge.FirstPoint(), pe2 = theEdge.SecondPoint();
  //
  // We have points on the triangle - p1, p2 and p3;
  // And points on the edge - pe1, pe2;
  // By comparing these points we should define which
  // edge it is for the triangle and its orientation on it:
  // e1 = p1->p2;
  // e2 = p2->p3;
  // e3 = p3->p1;
  // In case the order of points on the edge is forward,
  // the orientation is positive, otherwise it is negative.

  for (Standard_Integer i = 0, i1 = 1; i < 3; ++i, ++i1) {
    if (i1 > 2) {
      i1 = 0;
    }
    //
    if (pe1 == myPoints[i] && pe2 == myPoints[i1]) {
      myEdges[i] = theEdgeIndex;
      myEdgesOrientations[i] = 1;
      break;
    }
    if (pe1 == myPoints[i1] && pe2 == myPoints[i]) {
      myEdges[i] = theEdgeIndex;
      myEdgesOrientations[i] = -1;
      break;
    }
  }
}

//=======================================================================
//function : BoundingBox
//purpose  : 
//=======================================================================
const Bnd_Box& IntPolyh_Triangle::BoundingBox(const IntPolyh_ArrayOfPoints& thePoints)
{
  if (myBox.IsVoid()) {
    const IntPolyh_Point& aP1 = thePoints[myPoints[0]];
    const IntPolyh_Point& aP2 = thePoints[myPoints[1]];
    const IntPolyh_Point& aP3 = thePoints[myPoints[2]];
    myBox.Add(gp_Pnt(aP1.X(), aP1.Y(), aP1.Z()));
    myBox.Add(gp_Pnt(aP2.X(), aP2.Y(), aP2.Z()));
    myBox.Add(gp_Pnt(aP3.X(), aP3.Y(), aP3.Z()));
    myBox.SetGap(myDeflection + Precision::Confusion());
  }
  return myBox;
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void IntPolyh_Triangle::Dump (const Standard_Integer i) const
{ 
  printf("\nTriangle(%3d) : Points %5d %5d %5d Edges %5d %5d %5d deflection: %8f "
         "intersection possible %8d  intersection: %5d\n",
         i, myPoints[0], myPoints[1], myPoints[2],
         myEdges[0], myEdges[1], myEdges[2],
         myDeflection, (myIsIntersectionPossible ? 1 : 0), (myHasIntersection ? 1 : 0));
}
