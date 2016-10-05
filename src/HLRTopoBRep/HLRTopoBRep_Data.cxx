// Created on: 1994-10-24
// Created by: Christophe MARION
// Copyright (c) 1994-1999 Matra Datavision
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


#include <HLRTopoBRep_Data.hxx>
#include <HLRTopoBRep_ListOfVData.hxx>
#include <HLRTopoBRep_VData.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : HLRTopoBRep_Data
//purpose  : 
//=======================================================================
HLRTopoBRep_Data::HLRTopoBRep_Data ()
{
  Clear();
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void HLRTopoBRep_Data::Clear ()
{
  myOldS.Clear();
  mySplE.Clear();
  myData.Clear();
  myOutV.Clear();
  myIntV.Clear();
  myEdgesVertices.Clear();
}

//=======================================================================
//function : Clean
//purpose  : 
//=======================================================================

void HLRTopoBRep_Data::Clean ()
{
}

//=======================================================================
//function : EdgeHasSplE
//purpose  : 
//=======================================================================

Standard_Boolean 
HLRTopoBRep_Data::EdgeHasSplE (const TopoDS_Edge& E) const 
{
  if (!mySplE.IsBound(E)) return Standard_False;
  return !mySplE(E).IsEmpty();
}

//=======================================================================
//function : FaceHasIntL
//purpose  : 
//=======================================================================

Standard_Boolean 
HLRTopoBRep_Data::FaceHasIntL (const TopoDS_Face& F) const 
{
  if (!myData.IsBound(F)) return Standard_False;
  return !myData(F).FaceIntL().IsEmpty();
}

//=======================================================================
//function : FaceHasOutL
//purpose  : 
//=======================================================================

Standard_Boolean 
HLRTopoBRep_Data::FaceHasOutL (const TopoDS_Face& F) const 
{
  if (!myData.IsBound(F)) return Standard_False;
  return !myData(F).FaceOutL().IsEmpty();
}

//=======================================================================
//function : FaceHasIsoL
//purpose  : 
//=======================================================================

Standard_Boolean 
HLRTopoBRep_Data::FaceHasIsoL (const TopoDS_Face& F) const 
{
  if (!myData.IsBound(F)) return Standard_False;
  return !myData(F).FaceIsoL().IsEmpty();
}

//=======================================================================
//function : IsSplEEdgeEdge
//purpose  : 
//=======================================================================

Standard_Boolean 
HLRTopoBRep_Data::IsSplEEdgeEdge (const TopoDS_Edge& E1,
				  const TopoDS_Edge& E2) const 
{
  Standard_Boolean found = Standard_False;
  if (EdgeHasSplE(E1)) {
    
    TopTools_ListIteratorOfListOfShape itS;
    for (itS.Initialize(EdgeSplE(E1));
	 itS.More() && !found;
	 itS.Next())
      found= itS.Value().IsSame(E2);
  }
  else
    found = E1.IsSame(E2);
  return found;
}

//=======================================================================
//function : IsIntLFaceEdge
//purpose  : 
//=======================================================================

Standard_Boolean 
HLRTopoBRep_Data::IsIntLFaceEdge (const TopoDS_Face& F,
				  const TopoDS_Edge& E) const 
{
  Standard_Boolean found = Standard_False;
  if (FaceHasIntL(F)) {

    TopTools_ListIteratorOfListOfShape itE;
    for(itE.Initialize(FaceIntL(F));
	itE.More() && !found;
	itE.Next()) {
      found = IsSplEEdgeEdge(TopoDS::Edge(itE.Value()),E);
    }
  }
  return found;
}

//=======================================================================
//function : IsOutLFaceEdge
//purpose  : 
//=======================================================================

Standard_Boolean 
HLRTopoBRep_Data::IsOutLFaceEdge (const TopoDS_Face& F,
				  const TopoDS_Edge& E) const 
{
  Standard_Boolean found = Standard_False;
  if (FaceHasOutL(F)) {

    TopTools_ListIteratorOfListOfShape itE;
    for(itE.Initialize(FaceOutL(F));
	itE.More() && !found;
	itE.Next()) {
      found = IsSplEEdgeEdge(TopoDS::Edge(itE.Value()),E);
    }
  }
  return found;
}

//=======================================================================
//function : IsIsoLFaceEdge
//purpose  : 
//=======================================================================

Standard_Boolean 
HLRTopoBRep_Data::IsIsoLFaceEdge (const TopoDS_Face& F,
				  const TopoDS_Edge& E) const 
{
  Standard_Boolean found = Standard_False;
  if (FaceHasIsoL(F)) {

    TopTools_ListIteratorOfListOfShape itE;
    for(itE.Initialize(FaceIsoL(F));
	itE.More() && !found;
	itE.Next()) {
      found = IsSplEEdgeEdge(TopoDS::Edge(itE.Value()),E);
    }
  }
  return found;
}

//=======================================================================
//function : NewSOldS
//purpose  : 
//=======================================================================

TopoDS_Shape HLRTopoBRep_Data::NewSOldS (const TopoDS_Shape& NewS) const 
{
  if (myOldS.IsBound(NewS)) return myOldS(NewS);
  else                      return NewS;
}

//=======================================================================
//function : AddOldS
//purpose  : 
//=======================================================================

void HLRTopoBRep_Data::AddOldS (const TopoDS_Shape& NewS,
				const TopoDS_Shape& OldS)
{
  if (!myOldS.IsBound(NewS))
    myOldS.Bind(NewS,OldS);
}

//=======================================================================
//function : AddSplE
//purpose  : 
//=======================================================================

TopTools_ListOfShape & HLRTopoBRep_Data::AddSplE (const TopoDS_Edge& E)
{
  if (!mySplE.IsBound(E)) {
    TopTools_ListOfShape empty;
    mySplE.Bind(E,empty);
  }
  return mySplE(E);
}

//=======================================================================
//function : AddIntL
//purpose  : 
//=======================================================================

TopTools_ListOfShape & HLRTopoBRep_Data::AddIntL (const TopoDS_Face& F)
{
  if (!myData.IsBound(F)) {
    HLRTopoBRep_FaceData theData;
    myData.Bind(F, theData);
  }
  return myData(F).AddIntL();
}

//=======================================================================
//function : AddOutL
//purpose  : 
//=======================================================================

TopTools_ListOfShape & HLRTopoBRep_Data::AddOutL (const TopoDS_Face& F)
{
  if (!myData.IsBound(F)) {
    HLRTopoBRep_FaceData theData;
    myData.Bind(F, theData);
  }
  return myData(F).AddOutL();
}

//=======================================================================
//function : AddIsoL
//purpose  : 
//=======================================================================

TopTools_ListOfShape & HLRTopoBRep_Data::AddIsoL (const TopoDS_Face& F)
{
  if (!myData.IsBound(F)) {
    HLRTopoBRep_FaceData theData;
    myData.Bind(F, theData);
  }
  return myData(F).AddIsoL();
}

//=======================================================================
//function : InitEdge
//purpose  : 
//=======================================================================

void HLRTopoBRep_Data::InitEdge ()
{
  myEIterator.Initialize(myEdgesVertices);

  while (myEIterator.More() && myEIterator.Value().IsEmpty())
    myEIterator.Next();
}

//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================

void HLRTopoBRep_Data::NextEdge ()
{
  myEIterator.Next();

  while (myEIterator.More() && myEIterator.Value().IsEmpty())
    myEIterator.Next();
}

//=======================================================================
//function : InitVertex
//purpose  : 
//=======================================================================

void HLRTopoBRep_Data::InitVertex (const TopoDS_Edge& E)
{
  if (!myEdgesVertices.IsBound(E)) {
    HLRTopoBRep_ListOfVData empty;
    myEdgesVertices.Bind(E,empty);
  }
  HLRTopoBRep_ListOfVData& L = myEdgesVertices(E);
  myVList = &L;
  myVIterator.Initialize(L);
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex & HLRTopoBRep_Data::Vertex () const 
{
  return TopoDS::Vertex(myVIterator.Value().Vertex());
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real HLRTopoBRep_Data::Parameter() const 
{
  return myVIterator.Value().Parameter();
}

//=======================================================================
//function : InsertBefore
//purpose  : 
//=======================================================================

void HLRTopoBRep_Data::InsertBefore (const TopoDS_Vertex& V,
				     const Standard_Real P)
{
  HLRTopoBRep_VData VD(P,V);
  myVList->InsertBefore(VD,myVIterator);
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

void HLRTopoBRep_Data::Append (const TopoDS_Vertex& V, 
			       const Standard_Real P)
{
  HLRTopoBRep_VData VD(P,V);
  myVList->Append(VD);
}

