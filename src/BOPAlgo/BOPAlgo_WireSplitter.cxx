// Created by: Peter KURNEV
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
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

#include <BOPAlgo_WireEdgeSet.hxx>
#include <BOPAlgo_WireSplitter.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_Parallel.hxx>
#include <NCollection_Vector.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListOfShape.hxx>

//=================================================================================================

BOPAlgo_WireSplitter::BOPAlgo_WireSplitter()
    : BOPAlgo_Algo(),
      myWES(NULL),
      myLCB(myAllocator)
{
}

//=================================================================================================

BOPAlgo_WireSplitter::BOPAlgo_WireSplitter(const Handle(NCollection_BaseAllocator)& theAllocator)
    : BOPAlgo_Algo(theAllocator),
      myWES(NULL),
      myLCB(myAllocator)
{
}

//=================================================================================================

BOPAlgo_WireSplitter::~BOPAlgo_WireSplitter() {}

//=================================================================================================

void BOPAlgo_WireSplitter::SetWES(const BOPAlgo_WireEdgeSet& theWES)
{
  myWES = (BOPAlgo_WireEdgeSet*)&theWES;
}

//=================================================================================================

BOPAlgo_WireEdgeSet& BOPAlgo_WireSplitter::WES()
{
  return *myWES;
}

//=================================================================================================

void BOPAlgo_WireSplitter::SetContext(const Handle(IntTools_Context)& theContext)
{
  myContext = theContext;
}

//=================================================================================================

const Handle(IntTools_Context)& BOPAlgo_WireSplitter::Context()
{
  return myContext;
}

//=================================================================================================

void BOPAlgo_WireSplitter::CheckData()
{
  if (!myWES)
  {
    AddError(new BOPAlgo_AlertNullInputShapes);
    return;
  }
}

//=================================================================================================

void BOPAlgo_WireSplitter::Perform(const Message_ProgressRange& theRange)
{
  GetReport()->Clear();
  Message_ProgressScope aPS(theRange, "Building wires", 1);
  //
  CheckData();
  if (HasErrors())
  {
    return;
  }
  //
  // create a context
  if (myContext.IsNull())
  {
    myContext = new IntTools_Context;
  }
  //
  BOPTools_AlgoTools::MakeConnexityBlocks(myWES->StartElements(),
                                          TopAbs_VERTEX,
                                          TopAbs_EDGE,
                                          myLCB);
  if (UserBreak(aPS))
  {
    return;
  }

  MakeWires(aPS.Next());
}

/////////////////////////////////////////////////////////////////////////
class BOPAlgo_WS_ConnexityBlock
{
public:
  BOPAlgo_WS_ConnexityBlock() {};
  ~BOPAlgo_WS_ConnexityBlock() {};

  void SetFace(const TopoDS_Face& theF) { myFace = theF; }

  const TopoDS_Face& Face() const { return myFace; }

  void SetConnexityBlock(const BOPTools_ConnexityBlock& theCB) { myCB = theCB; }

  const BOPTools_ConnexityBlock& ConnexityBlock() const { return myCB; }

  void SetContext(const Handle(IntTools_Context)& aContext) { myContext = aContext; }

  //
  const Handle(IntTools_Context)& Context() const { return myContext; }

  //
  void SetProgressRange(const Message_ProgressRange& theRange) { myRange = theRange; }

  void Perform()
  {
    Message_ProgressScope aPS(myRange, NULL, 1);
    if (!aPS.More())
    {
      return;
    }
    BOPAlgo_WireSplitter::SplitBlock(myFace, myCB, myContext);
  }

protected:
  TopoDS_Face              myFace;
  BOPTools_ConnexityBlock  myCB;
  Handle(IntTools_Context) myContext;
  Message_ProgressRange    myRange;
};

typedef NCollection_Vector<BOPAlgo_WS_ConnexityBlock> BOPAlgo_VectorOfConnexityBlock;

//=================================================================================================

void BOPAlgo_WireSplitter::MakeWires(const Message_ProgressRange& theRange)
{
  Standard_Boolean                            bIsRegular;
  Standard_Integer                            aNbVCB, k;
  TopoDS_Wire                                 aW;
  BOPTools_ListIteratorOfListOfConnexityBlock aItCB;
  TopTools_ListIteratorOfListOfShape          aIt;
  BOPAlgo_VectorOfConnexityBlock              aVCB;
  //
  Message_ProgressScope aPSOuter(theRange, NULL, 1);
  //
  const TopoDS_Face& aF = myWES->Face();
  //
  aItCB.Initialize(myLCB);
  for (; aItCB.More(); aItCB.Next())
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }

    BOPTools_ConnexityBlock& aCB = aItCB.ChangeValue();
    bIsRegular                   = aCB.IsRegular();
    if (bIsRegular)
    {
      TopTools_ListOfShape& aLE = aCB.ChangeShapes();
      BOPAlgo_WireSplitter::MakeWire(aLE, aW);
      myWES->AddShape(aW);
    }
    else
    {
      BOPAlgo_WS_ConnexityBlock& aWSCB = aVCB.Appended();
      aWSCB.SetFace(aF);
      aWSCB.SetConnexityBlock(aCB);
    }
  }
  aNbVCB = aVCB.Length();
  Message_ProgressScope aPSParallel(aPSOuter.Next(), NULL, aNbVCB);
  for (Standard_Integer iW = 0; iW < aNbVCB; ++iW)
  {
    aVCB.ChangeValue(iW).SetProgressRange(aPSParallel.Next());
  }
  //===================================================
  BOPTools_Parallel::Perform(myRunParallel, aVCB, myContext);
  //===================================================
  for (k = 0; k < aNbVCB; ++k)
  {
    const BOPAlgo_WS_ConnexityBlock& aCB = aVCB(k);
    const TopTools_ListOfShape&      aLW = aCB.ConnexityBlock().Loops();
    aIt.Initialize(aLW);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aWx = aIt.Value();
      myWES->AddShape(aWx);
    }
  }
}
