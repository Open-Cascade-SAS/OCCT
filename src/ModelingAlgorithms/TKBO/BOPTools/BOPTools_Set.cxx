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

#include <BOPTools_Set.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

static size_t NormalizedIds(const size_t aId, const int aDiv);

//=================================================================================================

BOPTools_Set::BOPTools_Set()
    : myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
      myShapes(256, myAllocator)
{
  myNbShapes = 0;
  mySum      = 0;
  myUpper    = 432123;
}

//=================================================================================================

BOPTools_Set::BOPTools_Set(const occ::handle<NCollection_BaseAllocator>& theAllocator)
    : myAllocator(theAllocator),
      myShapes(256, myAllocator)
{
  myNbShapes = 0;
  mySum      = 0;
  myUpper    = 432123;
}

//=================================================================================================

BOPTools_Set::BOPTools_Set(const BOPTools_Set& theOther)
    : myAllocator(theOther.myAllocator),
      myShapes(theOther.myShapes),
      myShape(theOther.myShape),
      myNbShapes(theOther.myNbShapes),
      mySum(theOther.mySum),
      myUpper(theOther.myUpper)
{
}

//=================================================================================================

BOPTools_Set::~BOPTools_Set()
{
  Clear();
}

//=================================================================================================

void BOPTools_Set::Clear()
{
  myNbShapes = 0;
  mySum      = 0;
  myShapes.Clear();
}

//=================================================================================================

int BOPTools_Set::NbShapes() const
{
  return myNbShapes;
}

//=================================================================================================

BOPTools_Set& BOPTools_Set::Assign(const BOPTools_Set& theOther)
{
  myShape     = theOther.myShape;
  myNbShapes  = theOther.myNbShapes;
  mySum       = theOther.mySum;
  myUpper     = theOther.myUpper;
  myAllocator = theOther.myAllocator;
  myShapes    = theOther.myShapes;
  return *this;
}

//=================================================================================================

const TopoDS_Shape& BOPTools_Set::Shape() const
{
  return myShape;
}

//=================================================================================================

bool BOPTools_Set::IsEqual(const BOPTools_Set& theOther) const
{
  if (theOther.myNbShapes != myNbShapes)
  {
    return false;
  }
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aM1;
  //
  for (int i = 0; i < myShapes.Size(); ++i)
  {
    aM1.Add(myShapes(i));
  }
  //
  for (int i = 0; i < theOther.myShapes.Size(); ++i)
  {
    if (!aM1.Contains(theOther.myShapes(i)))
    {
      return false;
    }
  }
  //
  return true;
}

//=================================================================================================

void BOPTools_Set::Add(const TopoDS_Shape& theS, const TopAbs_ShapeEnum theType)
{
  size_t             aId, aIdN;
  TopAbs_Orientation aOr;
  TopExp_Explorer    aExp;
  //
  myShape = theS;
  myShapes.Clear();
  myNbShapes = 0;
  mySum      = 0;
  //
  aExp.Init(theS, theType);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aSx = aExp.Current();
    if (theType == TopAbs_EDGE)
    {
      const TopoDS_Edge& aEx = TopoDS::Edge(aSx);
      if (BRep_Tool::Degenerated(aEx))
      {
        continue;
      }
    }
    //
    aOr = aSx.Orientation();
    if (aOr == TopAbs_INTERNAL)
    {
      TopoDS_Shape aSy;
      //
      aSy = aSx;
      //
      aSy.Orientation(TopAbs_FORWARD);
      myShapes.Append(aSy);
      //
      aSy.Orientation(TopAbs_REVERSED);
      myShapes.Append(aSy);
    }
    else
    {
      myShapes.Append(aSx);
    }
  }
  //
  myNbShapes = myShapes.Size();
  if (!myNbShapes)
  {
    return;
  }
  //
  for (int i = 0; i < myShapes.Size(); ++i)
  {
    const TopoDS_Shape& aSx = myShapes(i);
    aId                     = TopTools_ShapeMapHasher{}(aSx) % myUpper + 1;
    aIdN                    = NormalizedIds(aId, myNbShapes);
    mySum += aIdN;
  }
}

//=================================================================================================

size_t NormalizedIds(const size_t aId, const int aDiv)
{
  size_t aMax, aTresh, aIdRet;
  //
  aIdRet = aId;
  aMax   = SIZE_MAX;
  aTresh = aMax / aDiv;
  if (aId > aTresh)
  {
    aIdRet = aId % aTresh;
  }
  return aIdRet;
}
