// Created by: Peter KURNEV
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


#include <BOPCol_MapOfShape.hxx>
#include <BOPTools_Set.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

static 
  Standard_Integer NormalizedIds(const Standard_Integer aId,
                                 const Standard_Integer aDiv);

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPTools_Set::BOPTools_Set() 
:
  myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
  myShapes(myAllocator)
{
  myNbShapes=0;
  mySum=0;
  myUpper=432123;
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPTools_Set::BOPTools_Set
  (const Handle(NCollection_BaseAllocator)& theAllocator) 
:
  myAllocator(theAllocator),
  myShapes(myAllocator)
{ 
  myNbShapes=0;
  mySum=0;  
  myUpper=432123;
}
//=======================================================================
//function :~ 
//purpose  : 
//=======================================================================
BOPTools_Set::~BOPTools_Set()
{
  Clear();
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BOPTools_Set::Clear()
{ 
  myNbShapes=0;
  mySum=0;
  myShapes.Clear();
}
//=======================================================================
//function : NbShapes
//purpose  : 
//=======================================================================
Standard_Integer BOPTools_Set::NbShapes()const
{
  return myNbShapes;
}
//=======================================================================
//function :Assign
//purpose  : 
//=======================================================================
BOPTools_Set& BOPTools_Set::Assign(const BOPTools_Set& theOther)
{ 
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  myShape=theOther.myShape;
  myNbShapes=theOther.myNbShapes;
  mySum=theOther.mySum;
  myUpper=theOther.myUpper;
  myAllocator=theOther.myAllocator;
  //
  myShapes.Clear();
  aIt.Initialize(theOther.myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value();
    myShapes.Append(aSx);
  }
  return *this;
}
//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
const TopoDS_Shape& BOPTools_Set::Shape()const
{
  return myShape;
}
//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================
Standard_Integer BOPTools_Set::HashCode
  (const Standard_Integer theUpper)const
{
  return ::HashCode(mySum, theUpper);
}
//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_Set::IsEqual
  (const BOPTools_Set& theOther)const
{
  Standard_Boolean bRet;
  //
  bRet=Standard_False;
  //
  if (theOther.myNbShapes!=myNbShapes) {
    return bRet;
  }
  //
  BOPCol_MapOfShape aM1;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  aIt.Initialize(myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx1=aIt.Value();
    aM1.Add(aSx1);
  }
  //
  aIt.Initialize(theOther.myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx2=aIt.Value();
    if (!aM1.Contains(aSx2)) {
      return bRet;
    }
  }
  //
  return !bRet;
}
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BOPTools_Set::Add(const TopoDS_Shape& theS,
                       const TopAbs_ShapeEnum theType)
{
  Standard_Integer aId, aIdN;
  TopAbs_Orientation aOr;
  TopExp_Explorer aExp;
  //
  myShape=theS;
  myShapes.Clear();
  myNbShapes=0;
  mySum=0;
  //
  aExp.Init(theS, theType);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aSx=aExp.Current();
    if (theType==TopAbs_EDGE) {
      const TopoDS_Edge& aEx=*((TopoDS_Edge*)&aSx);
      if (BRep_Tool::Degenerated(aEx)) {
        continue;
      }
    }
    //
    aOr=aSx.Orientation();
    if (aOr==TopAbs_INTERNAL) {
      TopoDS_Shape aSy;
      //
      aSy=aSx;
      //
      aSy.Orientation(TopAbs_FORWARD);
      myShapes.Append(aSy);
      //
      aSy.Orientation(TopAbs_REVERSED);
      myShapes.Append(aSy);
    }
    else {
      myShapes.Append(aSx);
    }
  }
  //
  myNbShapes=myShapes.Extent();
  if (!myNbShapes) {
    return;
  }
  // 
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  aIt.Initialize(myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value();
    aId=aSx.HashCode(myUpper);
    aIdN=NormalizedIds(aId, myNbShapes);
    mySum+=aIdN;
  }
}
//=======================================================================
// function: NormalizedIds
// purpose : 
//=======================================================================
Standard_Integer NormalizedIds(const Standard_Integer aId,
                               const Standard_Integer aDiv)
{
  Standard_Integer aMax, aTresh, aIdRet;
  //
  aIdRet=aId;
  aMax=::IntegerLast();
  aTresh=aMax/aDiv;
  if (aId>aTresh) {
    aIdRet=aId%aTresh;
  }
  return aIdRet;
}
