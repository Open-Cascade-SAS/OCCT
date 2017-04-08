// Created on: 1991-09-27
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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


#include <BRepPrim_Builder.hxx>
#include <BRepPrim_Direction.hxx>
#include <BRepPrim_GWedge.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <gp_Ax2.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#define NBFACES 6
#define NBWIRES 6
#define NBEDGES 12
#define NBVERTICES 8

static const Standard_Integer num[6] = {0,1,2,3,4,5};
static const Standard_Integer val[6] = {0,4,0,2,0,1};
static const Standard_Integer tab[6][6] = {{-1,-1, 0, 1, 8, 9},
					   {-1,-1, 2, 3,10,11},
					   { 0, 2,-1,-1, 4, 5},
					   { 1, 3,-1,-1, 6, 7},
					   { 8,10, 4, 6,-1,-1},
					   { 9,11, 5, 7,-1,-1}};

//=======================================================================
//function : BRepPrim_Wedge_NumDir1
//purpose  : when giving a direction return the range of the face
//=======================================================================

static Standard_Integer BRepPrim_Wedge_NumDir1
  (const BRepPrim_Direction d1) { return num[d1]; }

//=======================================================================
//function : BRepPrim_Wedge_NumDir2
//purpose  : when giving two directions return the range of the edge
//=======================================================================

static Standard_Integer BRepPrim_Wedge_NumDir2
  (const BRepPrim_Direction d1,
   const BRepPrim_Direction d2)
{
  Standard_Integer i1 = BRepPrim_Wedge_NumDir1(d1);
  Standard_Integer i2 = BRepPrim_Wedge_NumDir1(d2);
  if ( i1/2 == i2/2 ) throw Standard_DomainError();
  return tab[i1][i2];
}

//=======================================================================
//function : BRepPrim_Wedge_NumDir3
//purpose  : when giving three directions return the range of the vertex
//=======================================================================

static Standard_Integer BRepPrim_Wedge_NumDir3
  (const BRepPrim_Direction d1,
   const BRepPrim_Direction d2,
   const BRepPrim_Direction d3)
{
  Standard_Integer i1 = BRepPrim_Wedge_NumDir1(d1);
  Standard_Integer i2 = BRepPrim_Wedge_NumDir1(d2);
  Standard_Integer i3 = BRepPrim_Wedge_NumDir1(d3);
  if (( i1/2 == i2/2 ) ||
      ( i2/2 == i3/2 ) ||
      ( i3/2 == i1/2 )) throw Standard_DomainError();
  return val[i1]+val[i2]+val[i3];
}

//=======================================================================
//function : BRepPrim_Wedge_Check
//purpose  : raise Standard_DomainError if something was built
//=======================================================================

static void BRepPrim_Wedge_Check(const Standard_Boolean V[],
				   const Standard_Boolean E[],
				   const Standard_Boolean W[],
				   const Standard_Boolean F[])
{
  Standard_Integer i;
  for (i = 0; i < NBVERTICES; i++)
    if (V[i]) throw Standard_DomainError();
  for (i = 0; i < NBEDGES; i++)
    if (E[i]) throw Standard_DomainError();
  for (i = 0; i < NBWIRES; i++)
    if (W[i]) throw Standard_DomainError();
  for (i = 0; i < NBFACES; i++)
    if (F[i]) throw Standard_DomainError();
}

//=======================================================================
//function : BRepPrim_Wedge_Init
//purpose  : Set arrays to Standard_False
//=======================================================================

static void BRepPrim_Wedge_Init(Standard_Boolean& S,
				  Standard_Boolean V[],
				  Standard_Boolean E[],
				  Standard_Boolean W[],
				  Standard_Boolean F[])
{
  Standard_Integer i;
  S = Standard_False;
  for (i = 0; i < NBVERTICES; i++)
    V[i] = Standard_False;
  for (i = 0; i < NBEDGES; i++)
    E[i] = Standard_False;
  for (i = 0; i < NBWIRES; i++)
    W[i] = Standard_False;
  for (i = 0; i < NBFACES; i++)
    F[i] = Standard_False;
}

//=======================================================================
//function : BRepPrim_GWedge
//purpose  : build a box
//=======================================================================

BRepPrim_GWedge::BRepPrim_GWedge (const BRepPrim_Builder& B,
				    const gp_Ax2& Axes,
				    const Standard_Real dx,
				    const Standard_Real dy,
				    const Standard_Real dz) :
       myBuilder(B),
       myAxes(Axes),
       XMin(0),
       XMax(dx),
       YMin(0),
       YMax(dy),
       ZMin(0),
       ZMax(dz),
       Z2Min(0),
       Z2Max(dz),
       X2Min(0),
       X2Max(dx)
{
  for (Standard_Integer i = 0; i < NBFACES; i++) { myInfinite[i]=Standard_False; }
  if ( ( dx <= Precision::Confusion() ) ||
       ( dy <= Precision::Confusion() ) ||
       ( dz <= Precision::Confusion() ) )
    throw Standard_DomainError();
  BRepPrim_Wedge_Init(ShellBuilt,VerticesBuilt,EdgesBuilt,
			WiresBuilt,FacesBuilt);
}

//=======================================================================
//function : BRepPrim_GWedge
//purpose  : build a STEP wedge
//=======================================================================

BRepPrim_GWedge::BRepPrim_GWedge (const BRepPrim_Builder& B,
				    const gp_Ax2& Axes,
				    const Standard_Real dx,
				    const Standard_Real dy,
				    const Standard_Real dz,
				    const Standard_Real ltx) :
       myBuilder(B),
       myAxes(Axes),
       XMin(0),
       XMax(dx),
       YMin(0),
       YMax(dy),
       ZMin(0),
       ZMax(dz),
       Z2Min(0),
       Z2Max(dz),
       X2Min(0),
       X2Max(ltx)
{
  for (Standard_Integer i = 0; i < NBFACES; i++) { myInfinite[i]=Standard_False; }
  if ( ( dx <= Precision::Confusion() ) ||
       ( dy <= Precision::Confusion() ) ||
       ( dz <= Precision::Confusion() ) ||
       ( ltx < 0 ) )
    throw Standard_DomainError();
  BRepPrim_Wedge_Init(ShellBuilt,VerticesBuilt,EdgesBuilt,
			WiresBuilt,FacesBuilt);
}

//=======================================================================
//function : BRepPrim_GWedge
//purpose  : build a wedge by giving all the fields
//=======================================================================

BRepPrim_GWedge::BRepPrim_GWedge (const BRepPrim_Builder& B,
				    const gp_Ax2& Axes,
				    const Standard_Real xmin,
				    const Standard_Real ymin,
				    const Standard_Real zmin,
				    const Standard_Real z2min,
				    const Standard_Real x2min,
				    const Standard_Real xmax,
				    const Standard_Real ymax,
				    const Standard_Real zmax,
				    const Standard_Real z2max,
				    const Standard_Real x2max) :
       myBuilder(B),
       myAxes(Axes),
       XMin(xmin),
       XMax(xmax),
       YMin(ymin),
       YMax(ymax),
       ZMin(zmin),
       ZMax(zmax),
       Z2Min(z2min),
       Z2Max(z2max),
       X2Min(x2min),
       X2Max(x2max)
{
  for (Standard_Integer i = 0; i < NBFACES; i++) { myInfinite[i]=Standard_False; }
  if ( ( XMax-XMin <= Precision::Confusion() ) ||
       ( YMax-YMin <= Precision::Confusion() ) ||
       ( ZMax-ZMin <= Precision::Confusion() ) ||
       ( Z2Max-Z2Min < 0 ) ||
       ( X2Max-X2Min < 0 ) )
    throw Standard_DomainError();
  BRepPrim_Wedge_Init(ShellBuilt,VerticesBuilt,EdgesBuilt,
			WiresBuilt,FacesBuilt);
}

//=======================================================================
//function : Axes,
//           GetXMin, GetYMin, GetZMin, GetZ2Min, GetX2Min
//           GetXMax, GetYMax, GetZMax, GetZ2Max, GetX2Max
//purpose  : trivial
//=======================================================================

gp_Ax2 BRepPrim_GWedge::Axes     () const { return myAxes; }
Standard_Real   BRepPrim_GWedge::GetXMin  () const { return XMin;   }
Standard_Real   BRepPrim_GWedge::GetYMin  () const { return YMin;   }
Standard_Real   BRepPrim_GWedge::GetZMin  () const { return ZMin;   }
Standard_Real   BRepPrim_GWedge::GetZ2Min () const { return Z2Min;  }
Standard_Real   BRepPrim_GWedge::GetX2Min () const { return X2Min;  }
Standard_Real   BRepPrim_GWedge::GetXMax  () const { return XMax;   }
Standard_Real   BRepPrim_GWedge::GetYMax  () const { return YMax;   }
Standard_Real   BRepPrim_GWedge::GetZMax  () const { return ZMax;   }
Standard_Real   BRepPrim_GWedge::GetZ2Max () const { return Z2Max;  }
Standard_Real   BRepPrim_GWedge::GetX2Max () const { return X2Max;  }

//=======================================================================
//function : Open
//purpose  : trivial
//=======================================================================

void BRepPrim_GWedge::Open (const BRepPrim_Direction d1)
{
  BRepPrim_Wedge_Check(VerticesBuilt,EdgesBuilt,WiresBuilt,FacesBuilt);
  myInfinite[BRepPrim_Wedge_NumDir1(d1)] = Standard_True;
}

//=======================================================================
//function : Close
//purpose  : trivial
//=======================================================================

void BRepPrim_GWedge::Close (const BRepPrim_Direction d1)
{
  BRepPrim_Wedge_Check(VerticesBuilt,EdgesBuilt,WiresBuilt,FacesBuilt);
  myInfinite[BRepPrim_Wedge_NumDir1(d1)] = Standard_False;
}

//=======================================================================
//function : IsInfinite
//purpose  : true if it is open in the given direction
//=======================================================================

Standard_Boolean BRepPrim_GWedge::IsInfinite (const BRepPrim_Direction d1) const
{ return myInfinite[BRepPrim_Wedge_NumDir1(d1)]; }

//=======================================================================
//function : Shell
//purpose  : 
//=======================================================================

const TopoDS_Shell& BRepPrim_GWedge::Shell() {
  if (!ShellBuilt) {
    myBuilder.MakeShell(myShell);

    if (HasFace(BRepPrim_XMin))
      myBuilder.AddShellFace(myShell,Face(BRepPrim_XMin));
    if (HasFace(BRepPrim_XMax))
      myBuilder.AddShellFace(myShell,Face(BRepPrim_XMax));
    if (HasFace(BRepPrim_YMin))
      myBuilder.AddShellFace(myShell,Face(BRepPrim_YMin));
    if (HasFace(BRepPrim_YMax))
      myBuilder.AddShellFace(myShell,Face(BRepPrim_YMax));
    if (HasFace(BRepPrim_ZMin))
      myBuilder.AddShellFace(myShell,Face(BRepPrim_ZMin));
    if (HasFace(BRepPrim_ZMax))
      myBuilder.AddShellFace(myShell,Face(BRepPrim_ZMax));

    myShell.Closed (BRep_Tool::IsClosed (myShell));
    myBuilder.CompleteShell(myShell);
    ShellBuilt = Standard_True;
  }
  return myShell;
}

//=======================================================================
//function : HasFace 
//purpose  : true if the face exist in one direction
//=======================================================================

Standard_Boolean BRepPrim_GWedge::HasFace (const BRepPrim_Direction d1) const
{ 
  Standard_Boolean state = !myInfinite[BRepPrim_Wedge_NumDir1(d1)]; 
  if ( d1 == BRepPrim_YMax ) state = state && ( Z2Max != Z2Min )
                                             && ( X2Max != X2Min );
  return state;
}

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

gp_Pln BRepPrim_GWedge::Plane(const BRepPrim_Direction d1)
{

  Standard_Integer i = BRepPrim_Wedge_NumDir1(d1);

  gp_Dir D;
  gp_Vec VX = myAxes.XDirection();
  gp_Vec VY = myAxes.YDirection();
  gp_Vec VZ = myAxes.Direction();
  
  switch (i/2) {
    
  case 0 :
    D = myAxes.XDirection();
    break;
    
  case 1 :
    D = myAxes.YDirection();
    break;
    
  case 2 :
    D = myAxes.Direction();
    break;
    
  };
  Standard_Real X = 0., Y = 0., Z = 0.;
  
  switch (i) {
    
  case 0 :
    // XMin
    X = XMin;
    Y = YMin;
    Z = ZMin;
    if ( X2Min != XMin ) D = gp_Dir((YMax-YMin)*VX+(XMin-X2Min)*VY);
    break;
    
  case 1 :
    // XMax
    X = XMax;
    Y = YMin;
    Z = ZMin;
    if ( X2Max != XMax )  D = gp_Dir((YMax-YMin)*VX+(XMax-X2Max)*VY);
    break;
    
  case 2 :
    // YMin
    X = XMin;
    Y = YMin;
    Z = ZMin;
    break;
    
  case 3 :
    // YMax
    X = XMin;
    Y = YMax;
    Z = ZMin;
    break;
    
  case 4 :
    // ZMin
    X = XMin;
    Y = YMin;
    Z = ZMin;
    if ( Z2Min != ZMin ) D = gp_Dir((YMax-YMin)*VZ+(ZMin-Z2Min)*VY);
    break;
    
  case 5 :
    // ZMax
    X = XMin;
    Y = YMin;
    Z = ZMax;
    if ( Z2Max != ZMax )  D = gp_Dir((YMax-YMin)*VZ+(ZMax-Z2Max)*VY);
    break;
    
  };
  
  gp_Pnt P = myAxes.Location();
  P.Translate(X*gp_Vec(myAxes.XDirection()));
  P.Translate(Y*gp_Vec(myAxes.YDirection()));
  P.Translate(Z*gp_Vec(myAxes.Direction ()));
  gp_Pln plane(P,D);
  return plane;
}

//=======================================================================
//function : Face
//purpose  : the face in one direction
//=======================================================================

const TopoDS_Face& BRepPrim_GWedge::Face 
       (const BRepPrim_Direction d1)
{

  Standard_Integer i = BRepPrim_Wedge_NumDir1(d1);

  if (!FacesBuilt[i]) {
    gp_Pln P = Plane(d1);
    myBuilder.MakeFace(myFaces[i],P);
    if (HasWire(d1)) myBuilder.AddFaceWire(myFaces[i],Wire(d1));
    if ( i%2 == 0 ) myBuilder.ReverseFace(myFaces[i]);

    // pcurves

    BRepPrim_Direction dd1 = BRepPrim_ZMin, dd2 = BRepPrim_YMax, 
    dd3 = BRepPrim_ZMax,dd4 = BRepPrim_YMin;

    switch (i/2) {
      
    case 0 :
      // XMin XMax
      dd1 = BRepPrim_ZMin;
      dd2 = BRepPrim_YMax;
      dd3 = BRepPrim_ZMax;
      dd4 = BRepPrim_YMin;
      break;
      
    case 1 :
      // YMin YMax
      dd1 = BRepPrim_XMin;
      dd2 = BRepPrim_ZMax;
      dd3 = BRepPrim_XMax;
      dd4 = BRepPrim_ZMin;
      break;
      
    case 2 :
      // ZMin ZMax
      dd1 = BRepPrim_YMin;
      dd2 = BRepPrim_XMax;
      dd3 = BRepPrim_YMax;
      dd4 = BRepPrim_XMin;
      break;
      
    };

    gp_Lin L;
    gp_Dir DX = P.XAxis().Direction();
    gp_Dir DY = P.YAxis().Direction();
    Standard_Real U,V,DU,DV;
    if (HasEdge(d1,dd4)) {
      L = Line(d1,dd4);
    ElSLib::Parameters(P,L.Location(),U,V);
      DU = L.Direction() * DX;
      DV = L.Direction() * DY;
      myBuilder.SetPCurve(myEdges[BRepPrim_Wedge_NumDir2(d1,dd4)],
			  myFaces[i],
			  gp_Lin2d(gp_Pnt2d(U,V),gp_Dir2d(DU,DV)));
    }
    if (HasEdge(d1,dd3)) {
      L = Line(d1,dd3);
    ElSLib::Parameters(P,L.Location(),U,V);
      DU = L.Direction() * DX;
      DV = L.Direction() * DY;
      myBuilder.SetPCurve(myEdges[BRepPrim_Wedge_NumDir2(d1,dd3)],
			  myFaces[i],
			  gp_Lin2d(gp_Pnt2d(U,V),gp_Dir2d(DU,DV)));
    }

    if (HasEdge(d1,dd2)) {
      L = Line(d1,dd2);
    ElSLib::Parameters(P,L.Location(),U,V);
      DU = L.Direction() * DX;
      DV = L.Direction() * DY;
      myBuilder.SetPCurve(myEdges[BRepPrim_Wedge_NumDir2(d1,dd2)],
			  myFaces[i],
			  gp_Lin2d(gp_Pnt2d(U,V),gp_Dir2d(DU,DV)));
    }

    if (HasEdge(d1,dd1)) {
      L = Line(d1,dd1);
    ElSLib::Parameters(P,L.Location(),U,V);
      DU = L.Direction() * DX;
      DV = L.Direction() * DY;
      myBuilder.SetPCurve(myEdges[BRepPrim_Wedge_NumDir2(d1,dd1)],
			  myFaces[i],
			  gp_Lin2d(gp_Pnt2d(U,V),gp_Dir2d(DU,DV)));
    }

   
    myBuilder.CompleteFace(myFaces[i]);
    FacesBuilt[i] = Standard_True;
  }
  
  return myFaces[i];
  
}

//=======================================================================
//function : HasWire
//purpose  : trivial
//=======================================================================

Standard_Boolean BRepPrim_GWedge::HasWire (const BRepPrim_Direction d1) const
{
  Standard_Integer i = BRepPrim_Wedge_NumDir1(d1);

  if (myInfinite[i]) return Standard_False;
  BRepPrim_Direction dd1 = BRepPrim_XMin,dd2 = BRepPrim_YMax,dd3 = BRepPrim_XMax ,dd4 = BRepPrim_ZMin;

  switch (i/2) {
    
  case 0 :
    // XMin XMax
    dd1 = BRepPrim_ZMin;
    dd2 = BRepPrim_YMax;
    dd3 = BRepPrim_ZMax;
    dd4 = BRepPrim_YMin;
    break;
    
  case 1 :
    // YMin YMax
    dd1 = BRepPrim_XMin;
    dd2 = BRepPrim_ZMax;
    dd3 = BRepPrim_XMax;
    dd4 = BRepPrim_ZMin;
    break;
    
  case 2 :
    // ZMin ZMax
    dd1 = BRepPrim_YMin;
    dd2 = BRepPrim_XMax;
    dd3 = BRepPrim_YMax;
    dd4 = BRepPrim_XMin;
    break;
#ifndef OCCT_DEBUG
  default:
    break;
#endif
  };

  return HasEdge(d1,dd1)||HasEdge(d1,dd2)||HasEdge(d1,dd3)||HasEdge(d1,dd4);

}

//=======================================================================
//function : Wire
//purpose  : trivial
//=======================================================================

const TopoDS_Wire& BRepPrim_GWedge::Wire
       (const BRepPrim_Direction d1)
{
  Standard_Integer i = BRepPrim_Wedge_NumDir1(d1);

  BRepPrim_Direction dd1 = BRepPrim_XMin,dd2 = BRepPrim_YMax,dd3 = BRepPrim_XMax ,dd4 = BRepPrim_ZMin;

  if (!WiresBuilt[i]) {

    switch (i/2) {
      
    case 0 :
      // XMin XMax
      dd1 = BRepPrim_ZMin;
      dd2 = BRepPrim_YMax;
      dd3 = BRepPrim_ZMax;
      dd4 = BRepPrim_YMin;
      break;
      
    case 1 :
      // YMin YMax
      dd1 = BRepPrim_XMin;
      dd2 = BRepPrim_ZMax;
      dd3 = BRepPrim_XMax;
      dd4 = BRepPrim_ZMin;
      break;
      
    case 2 :
      // ZMin ZMax
      dd1 = BRepPrim_YMin;
      dd2 = BRepPrim_XMax;
      dd3 = BRepPrim_YMax;
      dd4 = BRepPrim_XMin;
      break;
    default:
      break;
    };

    myBuilder.MakeWire(myWires[i]);
  
    if (HasEdge(d1,dd4))
      myBuilder.AddWireEdge(myWires[i],Edge(d1,dd4),Standard_False);
    if (HasEdge(d1,dd3))
      myBuilder.AddWireEdge(myWires[i],Edge(d1,dd3),Standard_False);
    if (HasEdge(d1,dd2))
      myBuilder.AddWireEdge(myWires[i],Edge(d1,dd2),Standard_True );
    if (HasEdge(d1,dd1))
      myBuilder.AddWireEdge(myWires[i],Edge(d1,dd1),Standard_True );
   
    myBuilder.CompleteWire(myWires[i]);
    WiresBuilt[i] = Standard_True;
  }
   
  return myWires[i];

}

//=======================================================================
//function : HasEdge
//purpose  : trivial
//=======================================================================

Standard_Boolean BRepPrim_GWedge::HasEdge (const BRepPrim_Direction d1,
				   const BRepPrim_Direction d2) const
{
  Standard_Boolean state = !(myInfinite[BRepPrim_Wedge_NumDir1(d1)] ||
	            myInfinite[BRepPrim_Wedge_NumDir1(d2)]);
  Standard_Integer i = BRepPrim_Wedge_NumDir2(d1,d2);
  if      ( i == 6 || i == 7 ) state = state && ( X2Max != X2Min );
  else if ( i == 1 || i == 3 ) state = state && ( Z2Max != Z2Min );
  return state;
}

//=======================================================================
//function : Line
//purpose  : trivial
//=======================================================================

gp_Lin BRepPrim_GWedge::Line
  (const BRepPrim_Direction d1,
   const BRepPrim_Direction d2)
{
  if (!HasEdge(d1,d2)) throw Standard_DomainError();

  Standard_Integer i = BRepPrim_Wedge_NumDir2(d1,d2);

  Standard_Real X =0., Y =0., Z =0.;

  gp_Dir D;
  gp_Vec VX = myAxes.XDirection();
  gp_Vec VY = myAxes.YDirection();
  gp_Vec VZ = myAxes.Direction();

  switch (i/4) {

  case 0 :
    D = myAxes.Direction();
    break;

  case 1 :
    D = myAxes.XDirection();
    break;

  case 2 :
    D = myAxes.YDirection();
    break;

  };

  switch (i) {

  case 0 :
    // XMin YMin
    X = XMin;
    Y = YMin;
    Z = ZMin;
    break;

  case 1 :
    // XMin YMax
    X = X2Min;
    Y = YMax;
    Z = Z2Min;
    break;

  case 2 :
    // XMax YMin
    X = XMax;
    Y = YMin;
    Z = ZMin;
    break;

  case 3 :
    // XMax YMax
    X = X2Max;
    Y = YMax;
    Z = Z2Min;
    break;

  case 4 :
    // YMin ZMin
    X = XMin;
    Y = YMin;
    Z = ZMin;
    break;

  case 5 :
    // YMin ZMax
    X = XMin;
    Y = YMin;
    Z = ZMax;
    break;

  case 6 :
    // YMax ZMin
    X = X2Min;
    Y = YMax;
    Z = Z2Min;
    break;

  case 7 :
    // YMax ZMax
    X = X2Min;
    Y = YMax;
    Z = Z2Max;
    break;

  case 8 :
    // ZMin XMin
    X = XMin;
    Y = YMin;
    Z = ZMin;
    if ( (XMin != X2Min) || (ZMin != Z2Min) )
    {
      D = gp_Vec( (X2Min-XMin)*VX + (YMax-YMin)*VY + (Z2Min-ZMin)*VZ);
    }
    break;

  case 9 :
    // ZMax XMin
    X = XMin;
    Y = YMin;
    Z = ZMax;
    if ( (XMin != X2Min) || (ZMax != Z2Max) )
    {
      D = gp_Vec( (X2Min-XMin)*VX + (YMax-YMin)*VY + (Z2Max-ZMax)*VZ);
    }
    break;

  case 10 :
    // ZMin XMax
    X = XMax;
    Y = YMin;
    Z = ZMin;
    if ( (XMax != X2Max) || (ZMin != Z2Min) )
    {
      D = gp_Vec( (X2Max-XMax)*VX + (YMax-YMin)*VY + (Z2Min-ZMin)*VZ);
    }
    break;

  case 11 :
    // ZMax XMax
    X = XMax;
    Y = YMin;
    Z = ZMax;
    if ( (XMax != X2Max) || (ZMax != Z2Max) )
    {
      D = gp_Vec( (X2Max-XMax)*VX + (YMax-YMin)*VY + (Z2Max-ZMax)*VZ);
    }
    break;

  }

  gp_Pnt P = myAxes.Location();
  P.Translate(X*gp_Vec(myAxes.XDirection()));
  P.Translate(Y*gp_Vec(myAxes.YDirection()));
  P.Translate(Z*gp_Vec(myAxes.Direction ()));
  return gp_Lin(gp_Ax1(P,D));

}

//=======================================================================
//function : Edge
//purpose  : trivial
//=======================================================================

const TopoDS_Edge& BRepPrim_GWedge::Edge
       (const BRepPrim_Direction d1,
	const BRepPrim_Direction d2)
{
  if (!HasEdge(d1,d2)) throw Standard_DomainError();

  Standard_Integer i = BRepPrim_Wedge_NumDir2(d1,d2);

  if (!EdgesBuilt[i]) {

    BRepPrim_Direction dd1 = BRepPrim_XMin ,dd2 = BRepPrim_XMax;

    switch (i/4) {
    
    case 0 :
      dd1 = BRepPrim_ZMin;
      dd2 = BRepPrim_ZMax;
      break;
    
    case 1 :
      dd1 = BRepPrim_XMin;
      dd2 = BRepPrim_XMax;
      break;
    
    case 2 :
      dd1 = BRepPrim_YMin;
      dd2 = BRepPrim_YMax;
      break;

    default:
      break;
    };
    
    gp_Lin L = Line(d1,d2);
    myBuilder.MakeEdge(myEdges[i],L);
    
    if (HasVertex(d1,d2,dd2)) {
      myBuilder.AddEdgeVertex(myEdges[i],Vertex(d1,d2,dd2),
			      ElCLib::Parameter(L,Point(d1,d2,dd2)),
			      Standard_False);
    }
    if (HasVertex(d1,d2,dd1)) {
      myBuilder.AddEdgeVertex(myEdges[i],Vertex(d1,d2,dd1),
			      ElCLib::Parameter(L,Point(d1,d2,dd1)),
			      Standard_True );
    }
    
    if ( Z2Max == Z2Min ) {
      if ( i == 6 ) {
	myEdges[7]    = myEdges[6];
	EdgesBuilt[7] = Standard_True;
      }
      else if ( i == 7 ) {
	myEdges[6]    = myEdges[7];
	EdgesBuilt[6] = Standard_True;
      }
    }
    if ( X2Max == X2Min ) {
      if ( i == 1 ) {
	myEdges[3]    = myEdges[1];
	EdgesBuilt[3] = Standard_True;
      }
      else if ( i == 3 ) {
	myEdges[1]    = myEdges[3];
	EdgesBuilt[1] = Standard_True;
      }
    }

    myBuilder.CompleteEdge(myEdges[i]);
    EdgesBuilt[i] = Standard_True;
  }

  return myEdges[i];

}

//=======================================================================
//function : HasVertex
//purpose  : trivial
//=======================================================================

Standard_Boolean BRepPrim_GWedge::HasVertex
  (const BRepPrim_Direction d1,
   const BRepPrim_Direction d2,
   const BRepPrim_Direction d3) const
{ return !(myInfinite[BRepPrim_Wedge_NumDir1(d1)] ||
	   myInfinite[BRepPrim_Wedge_NumDir1(d2)] ||
	   myInfinite[BRepPrim_Wedge_NumDir1(d3)]); }

//=======================================================================
//function : Point
//purpose  : trivial
//=======================================================================

gp_Pnt  BRepPrim_GWedge::Point
  (const BRepPrim_Direction d1,
   const BRepPrim_Direction d2,
   const BRepPrim_Direction d3)
{
  if (!HasVertex(d1,d2,d3)) throw Standard_DomainError();

  Standard_Integer i = BRepPrim_Wedge_NumDir3(d1,d2,d3);

  Standard_Real X =0., Y =0., Z =0.;

  switch (i) {
    
  case 0 :
    X = XMin;
    Y = YMin;
    Z = ZMin;
    break;
    
  case 1 :
    X = XMin;
    Y = YMin;
    Z = ZMax;
    break;
    
  case 2 :
    X = X2Min;
    Y = YMax;
    Z = Z2Min;
    break;
    
  case 3 :
    X = X2Min;
    Y = YMax;
    Z = Z2Max;
    break;
    
  case 4 :
    X = XMax;
    Y = YMin;
    Z = ZMin;
    break;
    
  case 5 :
    X = XMax;
    Y = YMin;
    Z = ZMax;
    break;
    
  case 6 :
    X = X2Max;
    Y = YMax;
    Z = Z2Min;
    break;

  case 7 :
    X = X2Max;
    Y = YMax;
    Z = Z2Max;
    break;
    
  };

  gp_Pnt P = myAxes.Location();
  P.Translate(X*gp_Vec(myAxes.XDirection()));
  P.Translate(Y*gp_Vec(myAxes.YDirection()));
  P.Translate(Z*gp_Vec(myAxes.Direction ()));
  return P;
}

//=======================================================================
//function : Vertex
//purpose  : trivial
//=======================================================================

const TopoDS_Vertex& BRepPrim_GWedge::Vertex 
       (const BRepPrim_Direction d1,
	const BRepPrim_Direction d2,
	const BRepPrim_Direction d3)
{
  if (!HasVertex(d1,d2,d3)) throw Standard_DomainError();

  Standard_Integer i = BRepPrim_Wedge_NumDir3(d1,d2,d3);

  if (!VerticesBuilt[i]) {

    myBuilder.MakeVertex(myVertices[i],Point(d1,d2,d3));

    if ( Z2Max == Z2Min ) {
      if      ( i == 2 || i == 6 ) {
	myVertices[3] = myVertices[2];
	myVertices[7] = myVertices[6];
	VerticesBuilt[3] = Standard_True;
	VerticesBuilt[7] = Standard_True;
      }
      else if ( i == 3 || i == 7 ) {
	myVertices[2] = myVertices[3];
	myVertices[6] = myVertices[7];
	VerticesBuilt[2] = Standard_True;
	VerticesBuilt[6] = Standard_True;
      }
    }
    if ( X2Max == X2Min ) {
      if      ( i == 2 || i == 3 ) {
	myVertices[6] = myVertices[2];
	myVertices[7] = myVertices[3];
	VerticesBuilt[6] = Standard_True;
	VerticesBuilt[7] = Standard_True;
      }
      else if ( i == 6 || i == 7 ) {
	myVertices[2] = myVertices[6];
	myVertices[3] = myVertices[7];
	VerticesBuilt[2] = Standard_True;
	VerticesBuilt[3] = Standard_True;
      }
    }

    VerticesBuilt[i] = Standard_True;
  }

  return myVertices[i];

}

