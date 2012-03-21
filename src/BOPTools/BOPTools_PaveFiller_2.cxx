// Created on: 2001-03-27
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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



#include <BOPTools_PaveFiller.ixx>

#include <TColStd_ListOfInteger.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_OnceExplorer.hxx>

#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_CommonBlock.hxx>

//=======================================================================
//                       A P I   F U N C T I O N S
//=======================================================================
// function: SplitsInFace 
// purpose: splits of edges from nF1 in nF2 
//=======================================================================
Standard_Integer BOPTools_PaveFiller::SplitsInFace(const Standard_Integer ,//for overriding
						     const Standard_Integer nF1,
						     const Standard_Integer nF2,
						     TColStd_ListOfInteger& aSplits)
{
  Standard_Integer nE1;
  TopAbs_ShapeEnum aT1, aT2;
  
  aT1=myDS->GetShapeType(nF1);
  aT2=myDS->GetShapeType(nF2);
  
  if (aT1!=TopAbs_FACE || aT2!=TopAbs_FACE) {
    return 1; // Type mismatch
  }
  
  BooleanOperations_OnceExplorer aExp(*myDS);
  aExp.Init(nF1, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    nE1=aExp.Current();
    SplitsInFace (nE1, nF2, aSplits);
  }
  return 0; //Ok
} 
//=======================================================================
// function: SplitsInFace
// purpose: splits of edge nE1 in aFace2 
//=======================================================================
  Standard_Integer BOPTools_PaveFiller::SplitsInFace(const Standard_Integer nE1,
						     const Standard_Integer nF2,
						     TColStd_ListOfInteger& aSplits)
{
  Standard_Integer nF1, nSp;
  TopAbs_ShapeEnum aT1, aT2;
  
  aT1=myDS->GetShapeType(nE1);
  aT2=myDS->GetShapeType(nF2);
  
  if (aT1!=TopAbs_EDGE || aT2!=TopAbs_FACE) {
    return 1; // Type mismatch
  }
  
  const BOPTools_ListOfCommonBlock& aLCB=myCommonBlockPool(myDS->RefEdge(nE1));
  BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
  for (; anItCB.More(); anItCB.Next()) {
    BOPTools_CommonBlock& aCB=anItCB.Value();
    BOPTools_PaveBlock& aPB1=aCB.PaveBlock1(nE1);

    nF1=aCB.Face();
    if (nF1==nF2) {
      nSp=aPB1.Edge();
      aSplits.Append(nSp);
    }
  }
  return 0; //Ok
} 

//=======================================================================
// function: SplitsOnEdge
// purpose:  splits of edge nE1 on nE2 
//=======================================================================
  Standard_Integer BOPTools_PaveFiller::SplitsOnEdge(const Standard_Integer nE1,
						     const Standard_Integer nE2,
						     TColStd_ListOfInteger& aSplits)
{
  Standard_Integer nE, nSp;
  TopAbs_ShapeEnum aT1, aT2;
  
  aT1=myDS->GetShapeType(nE1);
  aT2=myDS->GetShapeType(nE2);
  
  if (aT1!=TopAbs_EDGE || aT2!=TopAbs_EDGE) {
    return 1; // Type mismatch
  }
  
  const BOPTools_ListOfCommonBlock& aLCB=myCommonBlockPool(myDS->RefEdge(nE1));
  BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
  for (; anItCB.More(); anItCB.Next()) {
    BOPTools_CommonBlock& aCB=anItCB.Value();
    BOPTools_PaveBlock& aPB1=aCB.PaveBlock1(nE1);
    BOPTools_PaveBlock& aPB2=aCB.PaveBlock2(nE1);
    nE=aPB2.OriginalEdge();
    if (nE==nE2) {
      nSp=aPB1.Edge();
      aSplits.Append(nSp);
    }
  }
  return 0; //Ok
} 
//=======================================================================
// function: SplitsOnFace
// purpose:  splits of edge nE1 on face nF2 
//=======================================================================
  Standard_Integer BOPTools_PaveFiller::SplitsOnFace(const Standard_Integer nE1,
						     const Standard_Integer nF2,
						     TColStd_ListOfInteger& aSplits)
{
  Standard_Integer nE2, ip;
  TopAbs_ShapeEnum aT1, aT2;
  
  aT1=myDS->GetShapeType(nE1);
  aT2=myDS->GetShapeType(nF2);
  
  if (aT1!=TopAbs_EDGE || aT2!=TopAbs_FACE) {
    return 1; // Type mismatch
  }
  
  BooleanOperations_OnceExplorer aExp(*myDS);
  aExp.Init(nF2, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    nE2=aExp.Current();
    ip=SplitsOnEdge(nE1, nE2, aSplits);
    if (ip) {
      return ip;
    }
  }
  return 0; //Ok
} 
//=======================================================================
// function: SplitsOnFace
// purpose:  splits of edges from face nF1 on face nF2 
//=======================================================================
  Standard_Integer BOPTools_PaveFiller::SplitsOnFace(const Standard_Integer ,//for overriding
						     const Standard_Integer nF1,
						     const Standard_Integer nF2,
						     TColStd_ListOfInteger& aSplits)
{
  Standard_Integer nE1, ip;
  TopAbs_ShapeEnum aT1, aT2;
  
  aT1=myDS->GetShapeType(nF1);
  aT2=myDS->GetShapeType(nF2);
  
  if (aT1!=TopAbs_FACE || aT2!=TopAbs_FACE) {
    return 1; // Type mismatch
  }
  
  BooleanOperations_OnceExplorer aExp(*myDS);
  aExp.Init(nF1, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    nE1=aExp.Current();
    ip=SplitsOnFace(nE1, nF2, aSplits);
    if (ip) {
      return ip;
    }
  }
  return 0; //Ok
} 
///////////////////////////////////////////////////////////////////////////////////
//=======================================================================
// function: SplitsInFace 
// purpose: splits of edges from nF1 in nF2 
//=======================================================================
  Standard_Integer BOPTools_PaveFiller::SplitsInFace(const Standard_Integer ,//for overriding
						     const Standard_Integer nF1,
						     const Standard_Integer nF2,
						     BOPTools_ListOfPaveBlock& aLPB)
{
  Standard_Integer nE1;
  TopAbs_ShapeEnum aT1, aT2;
  
  aT1=myDS->GetShapeType(nF1);
  aT2=myDS->GetShapeType(nF2);
  
  if (aT1!=TopAbs_FACE || aT2!=TopAbs_FACE) {
    return 1; // Type mismatch
  }
  
  BooleanOperations_OnceExplorer aExp(*myDS);
  aExp.Init(nF1, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    nE1=aExp.Current();
    SplitsInFace (nE1, nF2, aLPB);
  }
  return 0; //Ok
} 
//=======================================================================
// function: SplitsInFace
// purpose: splits of edge nE1 in aFace2 
//=======================================================================
  Standard_Integer BOPTools_PaveFiller::SplitsInFace(const Standard_Integer nE1,
						     const Standard_Integer nF2,
						     BOPTools_ListOfPaveBlock& aLPB)
{
  Standard_Integer nF1;
  TopAbs_ShapeEnum aT1, aT2;
  
  aT1=myDS->GetShapeType(nE1);
  aT2=myDS->GetShapeType(nF2);
  
  if (aT1!=TopAbs_EDGE || aT2!=TopAbs_FACE) {
    return 1; // Type mismatch
  }
  
  const BOPTools_ListOfCommonBlock& aLCB=myCommonBlockPool(myDS->RefEdge(nE1));
  BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
  for (; anItCB.More(); anItCB.Next()) {
    BOPTools_CommonBlock& aCB=anItCB.Value();
    BOPTools_PaveBlock& aPB1=aCB.PaveBlock1(nE1);

    nF1=aCB.Face();
    if (nF1==nF2) {
      aLPB.Append(aPB1);
    }
  }
  return 0; //Ok
} 

//=======================================================================
// function: SplitsOnEdge
// purpose:  splits of edge nE1 on nE2 
//=======================================================================
  Standard_Integer BOPTools_PaveFiller::SplitsOnEdge(const Standard_Integer nE1,
						     const Standard_Integer nE2,
						     BOPTools_ListOfPaveBlock& aLPB)
{
  Standard_Integer nE;
  TopAbs_ShapeEnum aT1, aT2;
  
  aT1=myDS->GetShapeType(nE1);
  aT2=myDS->GetShapeType(nE2);
  
  if (aT1!=TopAbs_EDGE || aT2!=TopAbs_EDGE) {
    return 1; // Type mismatch
  }
  
  const BOPTools_ListOfCommonBlock& aLCB=myCommonBlockPool(myDS->RefEdge(nE1));
  BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
  for (; anItCB.More(); anItCB.Next()) {
    BOPTools_CommonBlock& aCB=anItCB.Value();
    //BOPTools_PaveBlock& aPB1=aCB.PaveBlock1(nE1);
    BOPTools_PaveBlock& aPB2=aCB.PaveBlock2(nE1);
    nE=aPB2.OriginalEdge();
    if (nE==nE2) {
      //modified by NIZNHY-PKV Tue Apr  4 16:59:24 2006f
      //aLPB.Append(aPB1);
      const BOPTools_PaveBlock& aPB1R=aCB.PaveBlock1();
      aLPB.Append(aPB1R);
      //modified by NIZNHY-PKV Tue Apr  4 16:59:28 2006t
    }
  }
  return 0; //Ok
} 
//=======================================================================
// function: SplitsOnFace
// purpose:  splits of edge nE1 on face nF2 
//=======================================================================
  Standard_Integer BOPTools_PaveFiller::SplitsOnFace(const Standard_Integer nE1,
						     const Standard_Integer nF2,
						     BOPTools_ListOfPaveBlock& aLPB)
{
  Standard_Integer nE2, ip;
  TopAbs_ShapeEnum aT1, aT2;
  
  aT1=myDS->GetShapeType(nE1);
  aT2=myDS->GetShapeType(nF2);
  
  if (aT1!=TopAbs_EDGE || aT2!=TopAbs_FACE) {
    return 1; // Type mismatch
  }
  
  BooleanOperations_OnceExplorer aExp(*myDS);
  aExp.Init(nF2, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    nE2=aExp.Current();
    ip=SplitsOnEdge(nE1, nE2, aLPB);
    if (ip) {
      return ip;
    }
  }
  return 0; //Ok
} 
//=======================================================================
// function: SplitsOnFace
// purpose:  splits of edges from face nF1 on face nF2 
//=======================================================================
  Standard_Integer BOPTools_PaveFiller::SplitsOnFace(const Standard_Integer ,//for overriding
						     const Standard_Integer nF1,
						     const Standard_Integer nF2,
						     BOPTools_ListOfPaveBlock& aLPB)
{
  Standard_Integer nE1, ip;
  TopAbs_ShapeEnum aT1, aT2;
  
  aT1=myDS->GetShapeType(nF1);
  aT2=myDS->GetShapeType(nF2);
  
  if (aT1!=TopAbs_FACE || aT2!=TopAbs_FACE) {
    return 1; // Type mismatch
  }
  
  BooleanOperations_OnceExplorer aExp(*myDS);
  aExp.Init(nF1, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    nE1=aExp.Current();
    ip=SplitsOnFace(nE1, nF2, aLPB);
    if (ip) {
      return ip;
    }
  }
  return 0; //Ok
} 
