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

//:q0 abv 12.03.99: mat-a.stp: protection against no pcurves in SwapSeam()
//    abv 28.04.99  S4137: added method Add(WireData), method SetLast fixed
//    abv 05.05.99  S4174: protection against INTERNAL/EXTERNAL edges

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_Curve.hxx>
#include <ShapeExtend_WireData.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeExtend_WireData,Standard_Transient)

//=======================================================================
//function : ShapeExtend_WireData
//purpose  : 
//=======================================================================
ShapeExtend_WireData::ShapeExtend_WireData()
{
  Clear();  
}

//=======================================================================
//function : ShapeExtend_WireData
//purpose  : 
//=======================================================================

ShapeExtend_WireData::ShapeExtend_WireData (const TopoDS_Wire& wire,
					    const Standard_Boolean chained,
                                            const Standard_Boolean theManifold)
{
  Init ( wire, chained ,theManifold);  
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::Init (const Handle(ShapeExtend_WireData)& other) 
{
  Clear();
  Standard_Integer i, nb = other->NbEdges();
  for (i = 1; i <= nb; i++) Add ( other->Edge(i) );
  nb = other->NbNonManifoldEdges();
  for (i = 1; i <= nb; i++) Add ( other->NonmanifoldEdge(i) );
  myManifoldMode = other->ManifoldMode();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_WireData::Init (const TopoDS_Wire& wire,
					     const Standard_Boolean chained,
                                             const Standard_Boolean theManifold) 
{
  Clear();
  myManifoldMode = theManifold;
  Standard_Boolean OK = Standard_True;
  TopoDS_Vertex Vlast;
  for ( TopoDS_Iterator it(wire); it.More(); it.Next() ) {
    TopoDS_Edge E = TopoDS::Edge ( it.Value() );

    // protect against INTERNAL/EXTERNAL edges
    if ( (E.Orientation() != TopAbs_REVERSED &&
	 E.Orientation() != TopAbs_FORWARD)) {
      myNonmanifoldEdges->Append(E);
      continue;
    }
    
    TopoDS_Vertex V1, V2;
    for ( TopoDS_Iterator itv(E); itv.More(); itv.Next() ) {
      TopoDS_Vertex V = TopoDS::Vertex ( itv.Value() );
      if ( V.Orientation() == TopAbs_FORWARD ) V1 = V;
      else if ( V.Orientation() == TopAbs_REVERSED ) V2 = V;
    }

    // chainage? Si pas bon et chained False on repart sur WireExplorer
    if ( ! Vlast.IsNull() && ! Vlast.IsSame ( V1 ) && theManifold) {
      OK = Standard_False;
      if ( ! chained ) break;
    }
    Vlast = V2;
    if(wire.Orientation() == TopAbs_REVERSED)
      myEdges->Prepend( E );
    else
      myEdges->Append ( E );
  }

  if(!myManifoldMode) {
    Standard_Integer nb = myNonmanifoldEdges->Length();
    Standard_Integer i =1;
    for( ; i <= nb; i++)
      myEdges->Append(myNonmanifoldEdges->Value(i));
    myNonmanifoldEdges->Clear();
  }
//    refaire chainage ?  Par WireExplorer
  if ( OK || chained ) return OK;

  Clear();
  for ( BRepTools_WireExplorer we(wire); we.More(); we.Next() ) 
    myEdges->Append ( TopoDS::Edge ( we.Current() ) );

  return OK;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::Clear() 
{
  myEdges = new TopTools_HSequenceOfShape();
  myNonmanifoldEdges = new TopTools_HSequenceOfShape;
  mySeamF = mySeamR = -1;  
  mySeams.Nullify();
  myManifoldMode = Standard_True;
}

//=======================================================================
//function : ComputeSeams
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::ComputeSeams (const Standard_Boolean enforce) 
{
  if (mySeamF >= 0 && !enforce) return;

  mySeams = new TColStd_HSequenceOfInteger();
  mySeamF = mySeamR = 0;
  TopoDS_Shape S;
  Standard_Integer i, nb = NbEdges();
  TopTools_IndexedMapOfShape ME;
  Standard_Integer* SE = new Standard_Integer [nb+1];

  //  deux passes : d abord on mappe les Edges REVERSED
  //  Pour chacune, on note aussi son RANG dans la liste
  for (i = 1; i <= nb; i ++) {
    S = Edge(i);
    if (S.Orientation() == TopAbs_REVERSED) {
      Standard_Integer num = ME.Add (S);
      SE[num] = i;
    }
  }
  
  //  ensuite on voit les Edges FORWARD qui y seraient deja -> on note leur n0
  //  c-a-d le n0 de la directe ET de la reverse
  for (i = 1; i <= nb; i ++) {
    S = Edge(i);
    if (S.Orientation() == TopAbs_REVERSED) continue;
    Standard_Integer num = ME.FindIndex (S);
    if (num <= 0) continue;
    if (mySeamF == 0) { mySeamF = i; mySeamR = SE[num]; }
    else { mySeams->Append(i); mySeams->Append( SE[num] ); }
  }

  delete [] SE;  // ne pas oublier !!
}

//=======================================================================
//function : SetLast
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::SetLast (const Standard_Integer num) 
{
  if (num == 0) return;
  Standard_Integer i, nb = NbEdges();
  for (i = nb; i > num; i--) {
    TopoDS_Edge edge = TopoDS::Edge ( myEdges->Value(nb) );
    myEdges->Remove (nb);
    myEdges->InsertBefore (1, edge);
  }
  mySeamF = -1;
}

//=======================================================================
//function : SetDegeneratedLast
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::SetDegeneratedLast() 
{
  Standard_Integer i, nb = NbEdges();
  for (i = 1; i <= nb; i ++) {
    if ( BRep_Tool::Degenerated ( Edge(i) ) ) {
      SetLast ( i );
      return;
    }
  }
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::Add (const TopoDS_Edge& edge,
			       const Standard_Integer atnum) 
{
  if(edge.Orientation()!= TopAbs_REVERSED &&
	 edge.Orientation() != TopAbs_FORWARD && myManifoldMode) {
    myNonmanifoldEdges->Append(edge);
    return;
  }
    
  if (edge.IsNull()) return;
  if (atnum == 0) {
    myEdges->Append (edge);
  } 
  else {
    myEdges->InsertBefore (atnum,edge);
  }
  mySeamF = -1;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::Add (const TopoDS_Wire& wire,
				const Standard_Integer atnum) 
{
  if (wire.IsNull()) return;
  Standard_Integer n = atnum;
  TopTools_SequenceOfShape aNMEdges;
  for (TopoDS_Iterator it(wire); it.More(); it.Next()) {
    TopoDS_Edge edge = TopoDS::Edge (it.Value());
    if(edge.Orientation()!= TopAbs_REVERSED &&
	 edge.Orientation() != TopAbs_FORWARD) {
      if(myManifoldMode)
        myNonmanifoldEdges->Append(edge);
      else
	aNMEdges.Append(edge);
      continue;
    }
    if (n == 0) {
      myEdges->Append (edge);
    } else {
      myEdges->InsertBefore (n,edge);
      n++;
    }
  }
  Standard_Integer i =1, nb = aNMEdges.Length();
  for( ; i <= nb ; i++)
    myEdges->Append(aNMEdges.Value(i));
  mySeamF = -1;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::Add (const Handle(ShapeExtend_WireData) &wire,
				const Standard_Integer atnum) 
{
  if ( wire.IsNull() ) return;
  TopTools_SequenceOfShape aNMEdges;
  Standard_Integer n = atnum;
  Standard_Integer i=1;
  for (; i <= wire->NbEdges(); i++ ) {
    TopoDS_Edge aE =  wire->Edge(i);
    if(aE.Orientation() == TopAbs_INTERNAL || 
       aE.Orientation() == TopAbs_EXTERNAL) {
      aNMEdges.Append(aE);
      continue;
    }
      
    if (n == 0 ) {
      myEdges->Append ( wire->Edge(i) );
    } 
    else {
      myEdges->InsertBefore ( n, wire->Edge(i) );
      n++;
    }
  }
  
  //non-manifold edges for non-manifold wire shoud be added at end
  for (i=1; i <=aNMEdges.Length(); i++)
    myEdges->Append(aNMEdges.Value(i));
  
  for (i=1; i <= wire->NbNonManifoldEdges(); i++) {
    if( myManifoldMode)
      myNonmanifoldEdges->Append(wire->NonmanifoldEdge(i));
    else {
      if (n == 0) 
        myEdges->Append ( wire->Edge(i) );
    
      else {
        myEdges->InsertBefore ( n, wire->Edge(i) );
        n++;
      }
    }
  }
  
  mySeamF = -1;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::Add (const TopoDS_Shape& shape,
			       const Standard_Integer atnum) 
{
  if      (shape.ShapeType() == TopAbs_EDGE) Add (TopoDS::Edge (shape), atnum);
  else if (shape.ShapeType() == TopAbs_WIRE) Add (TopoDS::Wire (shape), atnum);
}

//=======================================================================
//function : AddOriented
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::AddOriented (const TopoDS_Edge& edge,
				       const Standard_Integer mode) 
{
  if (edge.IsNull() || mode < 0) return;
  TopoDS_Edge E = edge;
  if (mode == 1 || mode == 3) E.Reverse();
  Add (E, mode/2);  // mode = 0,1 -> 0  mode = 2,3 -> 1
}

//=======================================================================
//function : AddOriented
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::AddOriented (const TopoDS_Wire& wire,
				       const Standard_Integer mode) 
{
  if (wire.IsNull() || mode < 0) return;
  TopoDS_Wire W = wire;
  if (mode == 1 || mode == 3) W.Reverse();
  Add (W, mode/2);  // mode = 0,1 -> 0  mode = 2,3 -> 1
}

void ShapeExtend_WireData::AddOriented (const TopoDS_Shape& shape,
					const Standard_Integer mode) 
{
  if      (shape.ShapeType() == TopAbs_EDGE) AddOriented (TopoDS::Edge (shape), mode);
  else if (shape.ShapeType() == TopAbs_WIRE) AddOriented (TopoDS::Wire (shape), mode);
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::Remove (const Standard_Integer num) 
{
 
  myEdges->Remove ( num > 0 ? num : NbEdges() );
 
  mySeamF = -1;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void ShapeExtend_WireData::Set (const TopoDS_Edge& edge,
			       const Standard_Integer num) 
{
  if(edge.Orientation()!= TopAbs_REVERSED &&
	 edge.Orientation() != TopAbs_FORWARD && myManifoldMode) {
    if(num <= myNonmanifoldEdges->Length())
      myNonmanifoldEdges->SetValue(num,edge);
    else
      myNonmanifoldEdges->Append(edge);
  }
  
  else
    myEdges->SetValue ( ( num > 0 ? num : NbEdges() ), edge);
  mySeamF = -1;
}

//=======================================================================
//function : Reverse
//purpose  : reverse order of edges in the wire
//=======================================================================

void ShapeExtend_WireData::Reverse () 
{
  Standard_Integer i, nb = NbEdges();

  // inverser les edges + les permuter pour inverser le wire
  for (i = 1; i <= nb/2; i ++) {
    TopoDS_Shape S1 = myEdges->Value(i);      S1.Reverse();
    TopoDS_Shape S2 = myEdges->Value(nb+1-i); S2.Reverse();
    myEdges->SetValue (i,      S2);
    myEdges->SetValue (nb+1-i, S1);
  }
  //  nb d edges impair : inverser aussi l edge du milieu (rang inchange)
  if ( nb % 2 ) {    //  test impair
    i = (nb+1)/2;
    TopoDS_Shape SI = myEdges->Value(i);      SI.Reverse();
    myEdges->SetValue (i, SI);
  }
  mySeamF = -1;
}

//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

//  Fonction auxiliaire SwapSeam pour inverser
static void SwapSeam (const TopoDS_Shape& S, const TopoDS_Face& F)
{
  TopoDS_Edge E = TopoDS::Edge (S);
  if ( E.IsNull() || F.IsNull() ) return;
  if ( E.Orientation() == TopAbs_REVERSED ) return;  // ne le faire qu une fois !

  TopoDS_Face theface = F;  
  theface.Orientation(TopAbs_FORWARD);
  
//:S4136  Standard_Real Tol = BRep_Tool::Tolerance(theface);
  Handle(Geom2d_Curve) c2df,c2dr;
  Standard_Real uff,ulf,ufr,ulr;

  // d abord FWD puis REV
  c2df = BRep_Tool::CurveOnSurface (E,theface,uff,ulf);
  E.Orientation (TopAbs_REVERSED);
  c2dr = BRep_Tool::CurveOnSurface (E,theface,ufr,ulr);
  if ( c2df.IsNull() || c2dr.IsNull() ) return; //:q0
  // On permute
  E.Orientation (TopAbs_FORWARD);
  BRep_Builder B;
  B.UpdateEdge (E,c2dr,c2df,theface,0.); //:S4136: Tol
  B.Range (E,theface,uff,ulf);
}

void ShapeExtend_WireData::Reverse (const TopoDS_Face &face) 
{
  Reverse();
  if ( face.IsNull() ) return;

  //  ATTENTION aux coutures
  //  Une edge de couture est presente deux fois, FWD et REV
  //  Les inverser revient a permuter leur role ... donc ne rien faire
  //  Il faut donc aussi permuter leurs pcurves
  ComputeSeams(Standard_True);
  if (mySeamF > 0) SwapSeam (myEdges->Value(mySeamF),face);
  if (mySeamR > 0) SwapSeam (myEdges->Value(mySeamR),face);
  Standard_Integer nb = (mySeams.IsNull() ? 0 : mySeams->Length());
  for ( Standard_Integer i = 1; i <= nb; i ++) {
    SwapSeam (myEdges->Value(mySeams->Value(i)),face);
  }
  mySeamF = -1;
}

//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

Standard_Integer ShapeExtend_WireData::NbEdges() const
{
  return myEdges->Length();
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

TopoDS_Edge ShapeExtend_WireData::Edge (const Standard_Integer num) const
{
  if (num < 0) {
    TopoDS_Edge E = Edge (-num);
    E.Reverse();
    return E;
  }
  return TopoDS::Edge ( myEdges->Value ( num ) );
}
//=======================================================================
//function : NbNonManifoldEdges
//purpose  : 
//=======================================================================

Standard_Integer ShapeExtend_WireData::NbNonManifoldEdges() const
{
  return myNonmanifoldEdges->Length();
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

TopoDS_Edge ShapeExtend_WireData::NonmanifoldEdge (const Standard_Integer num) const
{
  TopoDS_Edge E;
  if (num < 0) 
    return E;
  
  return TopoDS::Edge ( myNonmanifoldEdges->Value ( num ) );
}
//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer ShapeExtend_WireData::Index (const TopoDS_Edge& edge)
{
  for (Standard_Integer i = 1; i <= NbEdges(); i++)
    if (Edge (i).IsSame (edge) && (Edge(i).Orientation() == edge.Orientation() || !IsSeam(i)))
      return i;
  return 0;
}

//=======================================================================
//function : IsSeam
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_WireData::IsSeam (const Standard_Integer num) 
{
  if (mySeamF < 0) ComputeSeams();
  if (mySeamF == 0) return Standard_False;
  
  if (num == mySeamF || num == mySeamR) return Standard_True;
//  Pas suffisant : on regarde dans la liste
  Standard_Integer i, nb = mySeams->Length();
  for (i = 1; i <= nb; i ++) {
    if (num == mySeams->Value(i)) return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Make
//purpose  : 
//=======================================================================

TopoDS_Wire ShapeExtend_WireData::Wire() const
{
  TopoDS_Wire W;
  BRep_Builder B;
  B.MakeWire (W); 
  Standard_Integer i, nb = NbEdges();
  Standard_Boolean ismanifold = Standard_True;
  for (i = 1; i <= nb; i ++) {
    TopoDS_Edge aE =  Edge(i);
    if (aE.Orientation() != TopAbs_FORWARD &&
	aE.Orientation() != TopAbs_REVERSED)
      ismanifold = Standard_False;
    B.Add (W, aE);
  }
  if(ismanifold) {
    TopoDS_Vertex vf, vl;
    TopExp::Vertices (W, vf, vl);
    if (!vf.IsNull() && !vl.IsNull() && vf.IsSame (vl)) W.Closed (Standard_True);
  }
  if(myManifoldMode) {
    nb = NbNonManifoldEdges();
    for (i = 1; i <= nb; i ++) B.Add (W, NonmanifoldEdge(i));
  }
  return W;
}

//=======================================================================
//function : APIMake
//purpose  : 
//=======================================================================

TopoDS_Wire ShapeExtend_WireData::WireAPIMake() const
{
  TopoDS_Wire W;
  BRepBuilderAPI_MakeWire MW;
  Standard_Integer i, nb = NbEdges();
  for (i = 1; i <= nb; i ++)  MW.Add (Edge(i));
  if(myManifoldMode) {
    nb = NbNonManifoldEdges();
    for (i = 1; i <= nb; i ++) MW.Add (NonmanifoldEdge(i));
  }
  if ( MW.IsDone() ) W = MW.Wire();
  return W;
}

//=======================================================================
//function : NonmanifoldEdges
//purpose  : 
//=======================================================================
Handle(TopTools_HSequenceOfShape) ShapeExtend_WireData::NonmanifoldEdges() const
{
  return myNonmanifoldEdges;
}

//=======================================================================
//function : ManifoldMode
//purpose  : 
//=======================================================================
Standard_Boolean&  ShapeExtend_WireData::ManifoldMode()
{
  return myManifoldMode;
}
