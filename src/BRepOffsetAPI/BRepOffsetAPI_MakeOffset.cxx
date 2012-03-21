// Created on: 1995-09-18
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <BRepOffsetAPI_MakeOffset.ixx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <Extrema_ExtPS.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepFill_OffsetWire.hxx>
#include <BRepFill_ListIteratorOfListOfOffsetWire.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx> 
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Wire.hxx>

#include <StdFail_NotDone.hxx>

//=======================================================================
//function : BRepOffsetAPI_MakeOffset
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeOffset::BRepOffsetAPI_MakeOffset()
:myIsInitialized( Standard_False)
{
}


//=======================================================================
//function : BRepOffsetAPI_MakeOffset
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeOffset::BRepOffsetAPI_MakeOffset(const TopoDS_Face& Spine, 
				       const GeomAbs_JoinType Join)
{
  Init(Spine, Join);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeOffset::Init(const TopoDS_Face&     Spine,
			      const GeomAbs_JoinType Join)
{
  myFace          = Spine;
  myIsInitialized = Standard_True;
  myJoin          = Join;
  TopExp_Explorer exp;
  for (exp.Init(myFace,TopAbs_WIRE); exp.More();exp.Next()) {
    myWires.Append(exp.Current());
  }
}

//=======================================================================
//function : BRepOffsetAPI_MakeOffset
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeOffset::BRepOffsetAPI_MakeOffset(const TopoDS_Wire& Spine, 
					const GeomAbs_JoinType Join)
{
  myWires.Append(Spine);
  myIsInitialized = Standard_True;
  myJoin = Join;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeOffset::Init(const GeomAbs_JoinType Join)
{
  myJoin = Join;
}
 
//=======================================================================
//function : BRepOffsetAPI_MakeOffset
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeOffset::AddWire(const TopoDS_Wire& Spine)
					    
{
  myIsInitialized = Standard_True;
  myWires.Append(Spine);
}

//=======================================================================
//function : BuildDomain
//purpose  : 
//=======================================================================

static void BuildDomains(TopoDS_Face&               myFace,
			 TopTools_ListOfShape&      WorkWires,
			 BRepFill_ListOfOffsetWire& myAlgos,
			 GeomAbs_JoinType           myJoin,
			 Standard_Boolean           isPositive)
{
  BRepAlgo_FaceRestrictor  FR;
  TopoDS_Vertex            VF,VL;
  TopTools_ListOfShape     LOW;
  BRep_Builder             B;

  if (myFace.IsNull()) {
    myFace   = BRepBuilderAPI_MakeFace(TopoDS::Wire(WorkWires.First()),Standard_True);
    if (myFace.IsNull())
      StdFail_NotDone::Raise ("BRepOffsetAPI_MakeOffset : the wire is not planar");
  }
//  Modified by Sergey KHROMOV - Thu Apr 26 16:04:43 2001 Begin
  TopExp_Explorer anExp(myFace, TopAbs_WIRE);
  TopoDS_Shape    aWire1 = WorkWires.First();
  TopoDS_Shape    aWire2;
  if (anExp.More()) {
    aWire2 = anExp.Current();
    if ((aWire1.Orientation() == aWire2.Orientation() && isPositive) ||
	(aWire1.Orientation() == TopAbs::Complement(aWire2.Orientation()) && !isPositive)) {
      TopTools_ListOfShape LWires;
      TopTools_ListIteratorOfListOfShape itl;
      for (itl.Initialize(WorkWires); itl.More(); itl.Next()) {
	const TopoDS_Shape& W = itl.Value();
	LWires.Append(W.Reversed());
      }
      WorkWires = LWires;
    }
  }
//  Modified by Sergey KHROMOV - Thu Apr 26 16:04:44 2001 End
  FR.Init(myFace,Standard_True);
  //====================================================
  // Construction of faces limited by closed wires.
  //====================================================
  TopTools_ListIteratorOfListOfShape itl(WorkWires);
  for (; itl.More(); itl.Next()) {
    TopoDS_Wire& W = TopoDS::Wire(itl.Value());
    if (W.Closed()){
      FR.Add(W);
      continue;
    }
    TopExp::Vertices (W,VF,VL);
    if (VF.IsSame(VL)) {
      FR.Add(W);
    }
    else {
      LOW.Append(W);
    }
  }
  FR.Perform();
  if (!FR.IsDone()) {
    StdFail_NotDone::Raise ("BRepOffsetAPI_MakeOffset : Build Domains");
  }
  TopTools_ListOfShape Faces;
  for (; FR.More(); FR.Next()) {
    Faces.Append(FR.Current());
  }

  //===========================================
  // No closed wire => only one domain
  //===========================================
  if (Faces.IsEmpty()) {
    TopoDS_Shape aLocalShape = myFace.EmptyCopied();
    TopoDS_Face F = TopoDS::Face(aLocalShape);
//    TopoDS_Face F = TopoDS::Face(myFace.EmptyCopied());
    TopTools_ListIteratorOfListOfShape itW(LOW);
    for ( ; itW.More(); itW.Next()) {
      B.Add(F,itW.Value());
    }
    BRepFill_OffsetWire Algo(F, myJoin);
    myAlgos.Append(Algo);
    return;
  }
  
  //====================================================
  // Classification of open wires.
  //====================================================  
//  for (TopTools_ListIteratorOfListOfShape itF(Faces); itF.More(); itF.Next()) {
  TopTools_ListIteratorOfListOfShape itF;
  for (itF.Initialize(Faces) ; itF.More(); itF.Next()) {
    TopoDS_Face&          F = TopoDS::Face(itF.Value());
    BRepAdaptor_Surface   S(F,0);
    Standard_Real         Tol = BRep_Tool::Tolerance(F); 

    BRepTopAdaptor_FClass2d CL(F,Precision::Confusion());

    TopTools_ListIteratorOfListOfShape itW(LOW);
    while (itW.More()) {
      TopoDS_Wire& W = TopoDS::Wire(itW.Value());
      //=======================================================
      // Choice of a point on the wire. + projection on the face.
      //=======================================================
      TopExp_Explorer exp(W,TopAbs_VERTEX);
      TopoDS_Vertex   V  = TopoDS::Vertex(exp.Current());
      gp_Pnt2d        PV;
      gp_Pnt          P3d = BRep_Tool::Pnt(V);
      Extrema_ExtPS ExtPS (P3d,S,Tol,Tol);
      Standard_Real     Dist2Min = Precision::Infinite();
      Standard_Real     Found   = Standard_False;
      for (Standard_Integer ie = 1; ie <= ExtPS.NbExt(); ie++) {
	Standard_Real X,Y;
	if (ExtPS.SquareDistance(ie) < Dist2Min) {
	  Dist2Min = ExtPS.SquareDistance(ie);
	  Found   = Standard_True;
	  ExtPS.Point(ie).Parameter(X,Y);
	  PV.SetCoord(X,Y);
	}
      }
      if ( Found && (CL.Perform(PV) == TopAbs_IN)) {
	// The face that contains a wire is found and it is removed from the list
	B.Add(F,W);
	LOW.Remove(itW);
      }
      else {
	itW.Next();
      }
    }
  }
  //========================================
  // Creation of algorithms on each domain.
  //========================================
  for (itF.Initialize(Faces); itF.More(); itF.Next()) {
    BRepFill_OffsetWire Algo(TopoDS::Face(itF.Value()), myJoin);
    myAlgos.Append(Algo);
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeOffset::Perform(const Standard_Real Offset, 
				 const Standard_Real Alt)
{
  StdFail_NotDone_Raise_if ( !myIsInitialized,
			    "BRepOffsetAPI_MakeOffset : Perform without Init");

  Standard_Integer i = 1;
  BRepFill_ListIteratorOfListOfOffsetWire itOW;  
  TopoDS_Compound Res;
  BRep_Builder    B;
  B.MakeCompound (Res);

  myLastIsLeft = (Offset <= 0);
  
  if (Offset <= 0.) {
    if (myLeft.IsEmpty()) {
//  Modified by Sergey KHROMOV - Fri Apr 27 14:35:26 2001 Begin
      BuildDomains(myFace,myWires,myLeft,myJoin, Standard_False);
//  Modified by Sergey KHROMOV - Fri Apr 27 14:35:26 2001 End
    }
    for (itOW.Initialize(myLeft); itOW.More(); itOW.Next()) {
      BRepFill_OffsetWire& Algo = itOW.Value();
      Algo.Perform(Abs(Offset),Alt);
      if (Algo.IsDone() && !Algo.Shape().IsNull()) {
	B.Add(Res,Algo.Shape());
	if (i == 1) myShape = Algo.Shape();
	i++;
      }
    }
  }
  else {
    if (myRight.IsEmpty()) {
//  Modified by Sergey KHROMOV - Fri Apr 27 14:35:28 2001 Begin
      BuildDomains(myFace,myWires,myRight,myJoin, Standard_True);
//  Modified by Sergey KHROMOV - Fri Apr 27 14:35:35 2001 End
    }
    for (itOW.Initialize(myRight); itOW.More(); itOW.Next()) {
      BRepFill_OffsetWire& Algo = itOW.Value();
      Algo.Perform(Offset,Alt);
      if (Algo.IsDone() && !Algo.Shape().IsNull()) {
	B.Add(Res,Algo.Shape());
	if (i == 1) myShape = Algo.Shape();
	i++;
      }
    }
  }
  if (i > 2) myShape = Res;
  Done();
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeOffset::Build()
{
  Done();
}


//=======================================================================
//function : ShapesFromShape
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepOffsetAPI_MakeOffset::Generated
(const TopoDS_Shape& S)
{
  myGenerated.Clear();
  BRepFill_ListIteratorOfListOfOffsetWire itOW;
  BRepFill_ListOfOffsetWire* Algos;
  Algos= &myLeft;
  if (!myLastIsLeft) {
    Algos = &myRight;
  }
  for (itOW.Initialize(*Algos); itOW.More(); itOW.Next()) {
    BRepFill_OffsetWire& OW = itOW.Value();
    TopTools_ListOfShape L;
    L =  OW.GeneratedShapes(S.Oriented(TopAbs_FORWARD));
    myGenerated.Append(L);
    L =  OW.GeneratedShapes(S.Oriented(TopAbs_REVERSED));
    myGenerated.Append(L);
  }
  return myGenerated;
}
