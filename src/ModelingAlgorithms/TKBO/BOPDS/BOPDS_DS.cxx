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

#include <BOPDS_DS.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_MapOfPave.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_VectorOfPave.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <Geom_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Tools.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Precision.hxx>
#include <Standard_Assert.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopTools_MapOfShape.hxx>
#include <algorithm>
//

static void TotalShapes(const TopoDS_Shape& aS, Standard_Integer& aNbS, TopTools_MapOfShape& aMS);

static Standard_Real ComputeParameter(const TopoDS_Vertex& aV, const TopoDS_Edge& aE);

//=================================================================================================

BOPDS_DS::BOPDS_DS()
    : myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
      myArguments(myAllocator),
      myRanges(0, myAllocator),
      myLines(0, myAllocator),
      myMapShapeIndex(100, myAllocator),
      myPaveBlocksPool(0, myAllocator),
      myMapPBCB(100, myAllocator),
      myFaceInfoPool(0, myAllocator),
      myShapesSD(100, myAllocator),
      myMapVE(100, myAllocator),
      myInterfTB(100, myAllocator),
      myInterfVV(0, myAllocator),
      myInterfVE(0, myAllocator),
      myInterfVF(0, myAllocator),
      myInterfEE(0, myAllocator),
      myInterfEF(0, myAllocator),
      myInterfFF(0, myAllocator),
      myInterfVZ(0, myAllocator),
      myInterfEZ(0, myAllocator),
      myInterfFZ(0, myAllocator),
      myInterfZZ(0, myAllocator),
      myInterfered(100, myAllocator)
{
  myNbShapes       = 0;
  myNbSourceShapes = 0;
}

//=================================================================================================

BOPDS_DS::BOPDS_DS(const Handle(NCollection_BaseAllocator)& theAllocator)
    : myAllocator(theAllocator),
      myArguments(myAllocator),
      myRanges(0, myAllocator),
      myLines(0, myAllocator),
      myMapShapeIndex(100, myAllocator),
      myPaveBlocksPool(0, myAllocator),
      myMapPBCB(100, myAllocator),
      myFaceInfoPool(0, myAllocator),
      myShapesSD(100, myAllocator),
      myMapVE(100, myAllocator),
      myInterfTB(100, myAllocator),
      myInterfVV(0, myAllocator),
      myInterfVE(0, myAllocator),
      myInterfVF(0, myAllocator),
      myInterfEE(0, myAllocator),
      myInterfEF(0, myAllocator),
      myInterfFF(0, myAllocator),
      myInterfVZ(0, myAllocator),
      myInterfEZ(0, myAllocator),
      myInterfFZ(0, myAllocator),
      myInterfZZ(0, myAllocator),
      myInterfered(100, myAllocator)
{
  myNbShapes       = 0;
  myNbSourceShapes = 0;
}

//=================================================================================================

BOPDS_DS::~BOPDS_DS()
{
  Clear();
}

//=================================================================================================

void BOPDS_DS::Clear()
{
  myNbShapes       = 0;
  myNbSourceShapes = 0;
  //
  myArguments.Clear();
  myRanges.Clear();
  myLines.Clear();
  myMapShapeIndex.Clear();
  myPaveBlocksPool.Clear();
  myFaceInfoPool.Clear();
  myShapesSD.Clear();
  myMapVE.Clear();
  myMapPBCB.Clear();
  myInterfTB.Clear();
  myInterfVV.Clear();
  myInterfVE.Clear();
  myInterfVF.Clear();
  myInterfEE.Clear();
  myInterfEF.Clear();
  myInterfFF.Clear();
  myInterfVZ.Clear();
  myInterfEZ.Clear();
  myInterfFZ.Clear();
  myInterfZZ.Clear();
  myInterfered.Clear();
}

//=================================================================================================

void BOPDS_DS::SetArguments(const TopTools_ListOfShape& theLS)
{
  myArguments = theLS;
}

//=================================================================================================

const TopTools_ListOfShape& BOPDS_DS::Arguments() const
{
  return myArguments;
}

//=================================================================================================

const Handle(NCollection_BaseAllocator)& BOPDS_DS::Allocator() const
{
  return myAllocator;
}

//=================================================================================================

Standard_Integer BOPDS_DS::NbShapes() const
{
  return myLines.Size();
}

//=================================================================================================

Standard_Integer BOPDS_DS::NbSourceShapes() const
{
  return myNbSourceShapes;
}

//=================================================================================================

Standard_Integer BOPDS_DS::NbRanges() const
{
  return myRanges.Size();
}

//=================================================================================================

const BOPDS_IndexRange& BOPDS_DS::Range(const Standard_Integer theI) const
{
  return myRanges(theI);
}

//=================================================================================================

Standard_Integer BOPDS_DS::Rank(const Standard_Integer theI) const
{
  Standard_Integer i, aNb, iErr;
  //
  iErr = -1;
  aNb  = NbRanges();
  for (i = 0; i < aNb; ++i)
  {
    const BOPDS_IndexRange& aR = Range(i);
    if (aR.Contains(theI))
    {
      return i;
    }
  }
  return iErr;
}

//=================================================================================================

Standard_Boolean BOPDS_DS::IsNewShape(const Standard_Integer theI) const
{
  return theI >= NbSourceShapes();
}

//=================================================================================================

Standard_Integer BOPDS_DS::Append(const BOPDS_ShapeInfo& theSI)
{
  Standard_Integer iX;
  //
  myLines.Appended() = theSI;
  iX                 = myLines.Length() - 1;
  myMapShapeIndex.Bind(theSI.Shape(), iX);
  //
  return iX;
}

//=================================================================================================

Standard_Integer BOPDS_DS::Append(const TopoDS_Shape& theS)
{
  Standard_Integer iX;
  //
  myLines.Appended().SetShape(theS);
  iX = myLines.Length() - 1;
  myMapShapeIndex.Bind(theS, iX);
  return iX;
}

//=================================================================================================

const BOPDS_ShapeInfo& BOPDS_DS::ShapeInfo(const Standard_Integer theI) const
{
  return myLines(theI);
}

//=================================================================================================

BOPDS_ShapeInfo& BOPDS_DS::ChangeShapeInfo(const Standard_Integer theI)
{
  BOPDS_ShapeInfo* pSI;
  //
  const BOPDS_ShapeInfo& aSI = ShapeInfo(theI);
  pSI                        = (BOPDS_ShapeInfo*)&aSI;
  return *pSI;
}

//=================================================================================================

const TopoDS_Shape& BOPDS_DS::Shape(const Standard_Integer theI) const
{

  const TopoDS_Shape& aS = ShapeInfo(theI).Shape();
  return aS;
}

//=================================================================================================

Standard_Integer BOPDS_DS::Index(const TopoDS_Shape& theS) const
{
  Standard_Integer iRet;
  //
  iRet = -1;
  if (myMapShapeIndex.IsBound(theS))
  {
    iRet = myMapShapeIndex.Find(theS);
  }
  return iRet;
}

//=================================================================================================

void BOPDS_DS::Init(const Standard_Real theFuzz)
{
  Standard_Integer                    i1, i2, j, aI, aNb, aNbS, aNbE, aNbSx;
  Standard_Integer                    n1, n2, n3, nV, nW, nE, aNbF;
  Standard_Real                       aTol, aTolAdd;
  TopAbs_ShapeEnum                    aTS;
  TopoDS_Iterator                     aItS;
  TColStd_ListIteratorOfListOfInteger aIt1, aIt2, aIt3;
  TopTools_ListIteratorOfListOfShape  aIt;
  BOPDS_IndexRange                    aR;
  Handle(NCollection_BaseAllocator)   aAllocator;
  TopTools_MapOfShape                 aMS;
  //
  // 1 Append Source Shapes
  aNb = myArguments.Extent();
  if (!aNb)
  {
    return;
  }
  //
  myRanges.SetIncrement(aNb);
  //
  aNbS = 0;
  aIt.Initialize(myArguments);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aSx = aIt.Value();
    //
    aNbSx = 0;
    TotalShapes(aSx, aNbSx, aMS);
    //
    aNbS = aNbS + aNbSx;
  }
  aMS.Clear();
  //
  myLines.SetIncrement(2 * aNbS);
  //-----------------------------------------------------scope_1 f
  aAllocator = NCollection_BaseAllocator::CommonBaseAllocator();
  //
  //
  i1 = 0;
  i2 = 0;
  aIt.Initialize(myArguments);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();
    if (myMapShapeIndex.IsBound(aS))
    {
      continue;
    }
    aI = Append(aS);
    //
    InitShape(aI, aS);
    //
    i2 = NbShapes() - 1;
    aR.SetIndices(i1, i2);
    myRanges.Append(aR);
    i1 = i2 + 1;
  }
  //
  aTolAdd          = Max(theFuzz, Precision::Confusion()) * 0.5;
  myNbSourceShapes = NbShapes();
  //
  // 2 Bounding Boxes
  //
  // 2.1 Vertex
  for (j = 0; j < myNbSourceShapes; ++j)
  {
    BOPDS_ShapeInfo& aSI = ChangeShapeInfo(j);
    //
    const TopoDS_Shape& aS = aSI.Shape();
    //
    aTS = aSI.ShapeType();
    //
    if (aTS == TopAbs_VERTEX)
    {
      Bnd_Box&             aBox = aSI.ChangeBox();
      const TopoDS_Vertex& aV   = *((TopoDS_Vertex*)&aS);
      const gp_Pnt&        aP   = BRep_Tool::Pnt(aV);
      aTol                      = BRep_Tool::Tolerance(aV);
      aBox.SetGap(aTol + aTolAdd);
      aBox.Add(aP);
    }
  }
  // 2.2 Edge
  aNbE = 0;
  for (j = 0; j < myNbSourceShapes; ++j)
  {
    BOPDS_ShapeInfo& aSI = ChangeShapeInfo(j);
    //
    aTS = aSI.ShapeType();
    if (aTS == TopAbs_EDGE)
    {
      const TopoDS_Shape& aS = aSI.Shape();
      const TopoDS_Edge&  aE = *((TopoDS_Edge*)&aS);
      aTol                   = BRep_Tool::Tolerance(aE);
      //
      if (!BRep_Tool::Degenerated(aE))
      {
        Standard_Boolean   bInf1, bInf2;
        Standard_Integer   aIx;
        Standard_Real      aT1, aT2;
        gp_Pnt             aPx;
        Handle(Geom_Curve) aC3D;
        TopoDS_Vertex      aVx;
        TopoDS_Edge        aEx;
        BRep_Builder       aBB;
        BOPDS_ShapeInfo    aSIx;
        //
        TColStd_ListOfInteger& aLI = aSI.ChangeSubShapes();
        //
        aEx = aE;
        aEx.Orientation(TopAbs_FORWARD);
        //
        aC3D  = BRep_Tool::Curve(aEx, aT1, aT2);
        bInf1 = Precision::IsNegativeInfinite(aT1);
        bInf2 = Precision::IsPositiveInfinite(aT2);
        //
        if (bInf1)
        {
          aC3D->D0(aT1, aPx);
          aBB.MakeVertex(aVx, aPx, aTol);
          aVx.Orientation(TopAbs_FORWARD);
          //
          aSIx.SetShape(aVx);
          aSIx.SetShapeType(TopAbs_VERTEX);
          aSIx.SetFlag(1); // infinite flag
          //
          aIx = Append(aSIx);
          aLI.Append(aIx);
        }
        if (bInf2)
        {
          aC3D->D0(aT2, aPx);
          aBB.MakeVertex(aVx, aPx, aTol);
          aVx.Orientation(TopAbs_REVERSED);
          //
          aSIx.SetShape(aVx);
          aSIx.SetShapeType(TopAbs_VERTEX);
          aSIx.SetFlag(1); // infinite flag
          //
          aIx = Append(aSIx);
          aLI.Append(aIx);
        }
      }
      else
      {
        aSI.SetFlag(j);
      }
      //
      Bnd_Box& aBox = aSI.ChangeBox();
      BRepBndLib::Add(aE, aBox);
      //
      const TColStd_ListOfInteger& aLV = aSI.SubShapes();
      aIt1.Initialize(aLV);
      for (; aIt1.More(); aIt1.Next())
      {
        nV                    = aIt1.Value();
        BOPDS_ShapeInfo& aSIV = ChangeShapeInfo(nV);
        Bnd_Box&         aBx  = aSIV.ChangeBox();
        aBox.Add(aBx);
      }
      aBox.SetGap(aBox.GetGap() + aTolAdd);
      ++aNbE;
    }
  }
  // 2.3 Face
  TColStd_MapOfInteger              aMI(100, aAllocator);
  TColStd_MapIteratorOfMapOfInteger aItMI;
  //
  aNbF = 0;
  for (j = 0; j < myNbSourceShapes; ++j)
  {
    BOPDS_ShapeInfo& aSI = ChangeShapeInfo(j);
    //
    aTS = aSI.ShapeType();
    if (aTS == TopAbs_FACE)
    {
      const TopoDS_Shape& aS = aSI.Shape();
      //
      Bnd_Box& aBox = aSI.ChangeBox();
      BRepBndLib::Add(aS, aBox);
      //
      TColStd_ListOfInteger& aLW = aSI.ChangeSubShapes();
      aIt1.Initialize(aLW);
      for (; aIt1.More(); aIt1.Next())
      {
        nW                    = aIt1.Value();
        BOPDS_ShapeInfo& aSIW = ChangeShapeInfo(nW);
        //
        const TColStd_ListOfInteger& aLE = aSIW.SubShapes();
        aIt2.Initialize(aLE);
        for (; aIt2.More(); aIt2.Next())
        {
          nE                    = aIt2.Value();
          BOPDS_ShapeInfo& aSIE = ChangeShapeInfo(nE);
          Bnd_Box&         aBx  = aSIE.ChangeBox();
          aBox.Add(aBx);
          aMI.Add(nE);
          //
          const TopoDS_Edge& aE = *(TopoDS_Edge*)(&aSIE.Shape());
          if (BRep_Tool::Degenerated(aE))
          {
            aSIE.SetFlag(j);
          }
          //
          const TColStd_ListOfInteger& aLV = aSIE.SubShapes();
          aIt3.Initialize(aLV);
          for (; aIt3.More(); aIt3.Next())
          {
            nV = aIt3.Value();
            aMI.Add(nV);
          }
        }
      } // for (; aIt1.More(); aIt1.Next()) {
      //
      // pure internal vertices on the face
      aItS.Initialize(aS);
      for (; aItS.More(); aItS.Next())
      {
        const TopoDS_Shape& aSx = aItS.Value();
        if (aSx.ShapeType() == TopAbs_VERTEX)
        {
          nV = Index(aSx);
          aMI.Add(nV);
        }
      }
      //
      //
      // For a Face: change wires for BRep sub-shapes
      aLW.Clear();
      aItMI.Initialize(aMI);
      for (; aItMI.More(); aItMI.Next())
      {
        nV = aItMI.Value();
        aLW.Append(nV);
      }
      aMI.Clear();
      aBox.SetGap(aBox.GetGap() + aTolAdd);
      ++aNbF;
    } // if (aTS==TopAbs_FACE) {
  } // for (j=0; j<myNbSourceShapes; ++j) {
  //
  // For the check mode we need to compute the bounding box for solid.
  // Otherwise, it will be computed on the building stage
  Standard_Boolean bCheckMode = (myArguments.Extent() == 1);
  if (bCheckMode)
  {
    // 2.4 Solids
    for (j = 0; j < myNbSourceShapes; ++j)
    {
      BOPDS_ShapeInfo& aSI = ChangeShapeInfo(j);
      //
      aTS = aSI.ShapeType();
      if (aTS != TopAbs_SOLID)
      {
        continue;
      }
      Bnd_Box& aBox = aSI.ChangeBox();
      BuildBndBoxSolid(j, aBox);
      //
      //
      // update sub-shapes by BRep comprising ones
      aMI.Clear();
      TColStd_ListOfInteger& aLI1 = aSI.ChangeSubShapes();
      //
      aIt1.Initialize(aLI1);
      for (; aIt1.More(); aIt1.Next())
      {
        n1                    = aIt1.Value();
        BOPDS_ShapeInfo& aSI1 = ChangeShapeInfo(n1);
        if (aSI1.ShapeType() != TopAbs_SHELL)
        {
          continue;
        }
        //
        const TColStd_ListOfInteger& aLI2 = aSI1.SubShapes();
        aIt2.Initialize(aLI2);
        for (; aIt2.More(); aIt2.Next())
        {
          n2                    = aIt2.Value();
          BOPDS_ShapeInfo& aSI2 = ChangeShapeInfo(n2);
          if (aSI2.ShapeType() != TopAbs_FACE)
          {
            continue;
          }
          //
          aMI.Add(n2);
          //
          const TColStd_ListOfInteger& aLI3 = aSI2.SubShapes();
          aIt3.Initialize(aLI3);
          for (; aIt3.More(); aIt3.Next())
          {
            n3 = aIt3.Value();
            aMI.Add(n3);
          }
        }
      }
      //
      aLI1.Clear();
      aItMI.Initialize(aMI);
      for (; aItMI.More(); aItMI.Next())
      {
        n1 = aItMI.Value();
        aLI1.Append(n1);
      }
      aMI.Clear();
    } // for (j=0; j<myNbSourceShapes; ++j) {
  }
  //
  aMI.Clear();
  //-----------------------------------------------------
  //
  // Prepare Vertex-Edge connection map
  for (nE = 0; nE < myNbSourceShapes; ++nE)
  {
    BOPDS_ShapeInfo& aSI = ChangeShapeInfo(nE);
    if (aSI.ShapeType() != TopAbs_EDGE)
      continue;

    const TColStd_ListOfInteger& aLV = aSI.SubShapes();
    aIt1.Initialize(aLV);
    for (; aIt1.More(); aIt1.Next())
    {
      nV                         = aIt1.Value();
      TColStd_ListOfInteger* pLE = myMapVE.ChangeSeek(nV);
      if (!pLE)
      {
        pLE = myMapVE.Bound(nV, TColStd_ListOfInteger(myAllocator));
        pLE->Append(nE);
      }
      else
      {
        // provide uniqueness of the edges in the list
        for (aIt2.Initialize(*pLE); aIt2.More(); aIt2.Next())
        {
          if (aIt2.Value() == nE)
            break;
        }
        if (!aIt2.More())
          pLE->Append(nE);
      }
    }
  }
  //-----------------------------------------------------scope_1 t
  // 3 myPaveBlocksPool
  // 4. myFaceInfoPool
  myPaveBlocksPool.SetIncrement(aNbE);
  myFaceInfoPool.SetIncrement(aNbF);
}

//=================================================================================================

void BOPDS_DS::InitShape(const Standard_Integer aI, const TopoDS_Shape& aS)
{
  Standard_Integer                    aIx;
  TopoDS_Iterator                     aIt;
  TColStd_ListIteratorOfListOfInteger aIt1;
  //
  BOPDS_ShapeInfo& aSI = ChangeShapeInfo(aI);
  aSI.SetShapeType(aS.ShapeType());
  TColStd_ListOfInteger& aLI = aSI.ChangeSubShapes();
  //
  TColStd_MapOfInteger aM;
  //
  aIt1.Initialize(aLI);
  for (; aIt1.More(); aIt1.Next())
  {
    aM.Add(aIt1.Value());
  }
  //
  aIt.Initialize(aS);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape&     aSx = aIt.Value();
    const Standard_Integer* pIx = myMapShapeIndex.Seek(aSx);
    aIx                         = (pIx ? *pIx : Append(aSx));
    //
    InitShape(aIx, aSx);
    //
    if (aM.Add(aIx))
    {
      aLI.Append(aIx);
    }
  }
}

//=================================================================================================

Standard_Boolean BOPDS_DS::HasInterfShapeSubShapes(const Standard_Integer theI1,
                                                   const Standard_Integer theI2,
                                                   const Standard_Boolean theFlag) const
{
  Standard_Boolean                    bRet;
  Standard_Integer                    n2;
  TColStd_ListIteratorOfListOfInteger aIt;
  bRet = Standard_False;
  //
  const BOPDS_ShapeInfo&       aSI = ShapeInfo(theI2);
  const TColStd_ListOfInteger& aLI = aSI.SubShapes();
  aIt.Initialize(aLI);
  for (; aIt.More(); aIt.Next())
  {
    n2   = aIt.Value();
    bRet = HasInterf(theI1, n2);
    if (theFlag)
    {
      if (bRet)
      {
        break;
      }
    }
    else
    {
      if (!bRet)
      {
        break;
      }
    }
  }
  return bRet;
}

//=================================================================================================

Standard_Boolean BOPDS_DS::HasInterfSubShapes(const Standard_Integer theI1,
                                              const Standard_Integer theI2) const
{
  Standard_Boolean                    bRet;
  Standard_Integer                    n1;
  TColStd_ListIteratorOfListOfInteger aIt;
  bRet = Standard_False;
  //
  const BOPDS_ShapeInfo&       aSI = ShapeInfo(theI1);
  const TColStd_ListOfInteger& aLI = aSI.SubShapes();
  aIt.Initialize(aLI);
  for (; aIt.More(); aIt.Next())
  {
    n1   = aIt.Value();
    bRet = HasInterfShapeSubShapes(n1, theI2);
    if (bRet)
    {
      break;
    }
  }
  return bRet;
}

//
// PaveBlocks
//=================================================================================================

const BOPDS_VectorOfListOfPaveBlock& BOPDS_DS::PaveBlocksPool() const
{
  return myPaveBlocksPool;
}

//=================================================================================================

BOPDS_VectorOfListOfPaveBlock& BOPDS_DS::ChangePaveBlocksPool()
{
  return myPaveBlocksPool;
}

//=================================================================================================

Standard_Boolean BOPDS_DS::HasPaveBlocks(const Standard_Integer theI) const
{
  return ShapeInfo(theI).HasReference();
}

//=================================================================================================

const BOPDS_ListOfPaveBlock& BOPDS_DS::PaveBlocks(const Standard_Integer theI) const
{
  static BOPDS_ListOfPaveBlock sLPB;
  Standard_Integer             aRef;
  //
  if (HasPaveBlocks(theI))
  {
    aRef                              = ShapeInfo(theI).Reference();
    const BOPDS_ListOfPaveBlock& aLPB = myPaveBlocksPool(aRef);
    return aLPB;
  }
  return sLPB;
}

//=================================================================================================

BOPDS_ListOfPaveBlock& BOPDS_DS::ChangePaveBlocks(const Standard_Integer theI)
{
  Standard_Boolean bHasReference;
  Standard_Integer aRef;
  //
  BOPDS_ShapeInfo& aSI = ChangeShapeInfo(theI);
  bHasReference        = aSI.HasReference();
  if (!bHasReference)
  {
    InitPaveBlocks(theI);
  }
  //
  aRef = aSI.Reference();
  return myPaveBlocksPool(aRef);
}

//=================================================================================================

void BOPDS_DS::InitPaveBlocks(const Standard_Integer theI)
{
  Standard_Integer                    nV = 0, iRef, aNbV, nVSD;
  Standard_Real                       aT;
  TopAbs_Orientation                  aOrE;
  TopoDS_Vertex                       aV;
  TColStd_ListIteratorOfListOfInteger aIt;
  BOPDS_Pave                          aPave;
  Handle(BOPDS_PaveBlock)             aPB;
  //
  BOPDS_ShapeInfo&   aSI = ChangeShapeInfo(theI);
  const TopoDS_Edge& aE  = *(TopoDS_Edge*)(&aSI.Shape());
  aOrE                   = aE.Orientation();
  //
  const TColStd_ListOfInteger& aLV = aSI.SubShapes();
  aNbV                             = aLV.Extent();
  if (!aNbV)
  {
    return;
  }
  //
  aPB = new BOPDS_PaveBlock;
  aPB->SetOriginalEdge(theI);
  //
  if (aOrE != TopAbs_INTERNAL)
  {
    aIt.Initialize(aLV);
    for (; aIt.More(); aIt.Next())
    {
      nV = aIt.Value();
      //
      const BOPDS_ShapeInfo& aSIV = ShapeInfo(nV);
      aV                          = *(TopoDS_Vertex*)(&aSIV.Shape());
      if (aSIV.HasFlag())
      {
        aT = ComputeParameter(aV, aE);
      }
      else
      {
        aT = BRep_Tool::Parameter(aV, aE);
      }
      //
      if (HasShapeSD(nV, nVSD))
      {
        nV = nVSD;
      }
      aPave.SetIndex(nV);
      aPave.SetParameter(aT);
      if (aSI.HasFlag())
        // for a degenerated edge append pave unconditionally
        aPB->AppendExtPave1(aPave);
      else
        aPB->AppendExtPave(aPave);
    }
    //
    if (aNbV == 1)
    {
      aV.Reverse();
      aT = BRep_Tool::Parameter(aV, aE);
      aPave.SetIndex(nV);
      aPave.SetParameter(aT);
      aPB->AppendExtPave1(aPave);
    }
  }
  //
  else
  {
    TopoDS_Iterator aItE;
    //
    aItE.Initialize(aE, Standard_False, Standard_True);
    for (; aItE.More(); aItE.Next())
    {
      aV = *((TopoDS_Vertex*)&aItE.Value());
      nV = Index(aV);
      //
      const BOPDS_ShapeInfo& aSIV = ShapeInfo(nV);
      if (aSIV.HasFlag())
      {
        aT = ComputeParameter(aV, aE);
      }
      else
      {
        aT = BRep_Tool::Parameter(aV, aE);
      }
      //
      if (HasShapeSD(nV, nVSD))
      {
        nV = nVSD;
      }
      aPave.SetIndex(nV);
      aPave.SetParameter(aT);
      aPB->AppendExtPave1(aPave);
    }
  }
  //
  BOPDS_ListOfPaveBlock& aLPB = myPaveBlocksPool.Appended();
  iRef                        = myPaveBlocksPool.Length() - 1;
  //
  aPB->Update(aLPB, Standard_False);
  aSI.SetReference(iRef);
}

//=================================================================================================

void BOPDS_DS::UpdatePaveBlocks()
{
  Standard_Integer                    i, aNbPBP;
  BOPDS_ListOfPaveBlock               aLPBN(myAllocator);
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  //
  BOPDS_VectorOfListOfPaveBlock& aPBP = myPaveBlocksPool;
  //
  aNbPBP = aPBP.Size();
  for (i = 0; i < aNbPBP; ++i)
  {
    BOPDS_ListOfPaveBlock& aLPB = aPBP(i);
    //
    aItPB.Initialize(aLPB);
    for (; aItPB.More();)
    {
      Handle(BOPDS_PaveBlock)& aPB = aItPB.ChangeValue();
      //
      if (!aPB->IsToUpdate())
      {
        aItPB.Next();
        continue;
      }
      //
      aLPBN.Clear();
      aPB->Update(aLPBN);
      //
      aLPB.Remove(aItPB);
      //
      aLPB.Append(aLPBN);
    } // for (; aItPB.More(); aItPB.Next()) {
  } // for (i=0; i<aNbPBP; ++i) {
}

//=================================================================================================

void BOPDS_DS::UpdatePaveBlock(const Handle(BOPDS_PaveBlock)& thePB)
{
  if (!thePB->IsToUpdate())
  {
    return;
  }
  //
  Standard_Integer                    nE, iRef;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB, aItPBN;
  BOPDS_ListOfPaveBlock               aLPBN(myAllocator);
  Handle(BOPDS_PaveBlock)             aPB;
  //
  BOPDS_VectorOfListOfPaveBlock& aPBP = myPaveBlocksPool;
  //
  nE                          = thePB->OriginalEdge();
  BOPDS_ShapeInfo& aSI        = ChangeShapeInfo(nE);
  iRef                        = aSI.Reference();
  BOPDS_ListOfPaveBlock& aLPB = aPBP(iRef);
  //
  aItPB.Initialize(aLPB);
  for (; aItPB.More(); aItPB.Next())
  {
    aPB = aItPB.ChangeValue();
    if (aPB == thePB)
    {
      aPB->Update(aLPBN);
      aLPB.Append(aLPBN);
      aLPB.Remove(aItPB);
      break;
    }
  }
}

//=================================================================================================

void BOPDS_DS::UpdateCommonBlock(const Handle(BOPDS_CommonBlock)& theCB,
                                 const Standard_Real              theFuzz)
{
  Standard_Integer                                                 nE, iRef, n1, n2;
  BOPDS_ListIteratorOfListOfPaveBlock                              aItPB, aItPBCB, aItPBN;
  BOPDS_ListOfPaveBlock                                            aLPBN;
  NCollection_DataMap<BOPDS_Pair, BOPDS_ListOfPaveBlock>           aMPKLPB;
  NCollection_DataMap<BOPDS_Pair, BOPDS_ListOfPaveBlock>::Iterator aItMPKLPB;
  Handle(BOPDS_PaveBlock)                                          aPB;
  Handle(BOPDS_CommonBlock)                                        aCBx;
  BOPDS_Pair                                                       aPK;
  //
  const BOPDS_ListOfPaveBlock& aLPBCB = theCB->PaveBlocks();
  if (!aLPBCB.First()->IsToUpdate())
  {
    return;
  }
  //
  const TColStd_ListOfInteger& aLF = theCB->Faces();
  //
  BOPDS_VectorOfListOfPaveBlock& aPBP = myPaveBlocksPool;
  //
  aItPBCB.Initialize(aLPBCB);
  for (; aItPBCB.More(); aItPBCB.Next())
  {
    const Handle(BOPDS_PaveBlock)& aPBCB = aItPBCB.ChangeValue();
    //
    nE                          = aPBCB->OriginalEdge();
    iRef                        = ChangeShapeInfo(nE).Reference();
    BOPDS_ListOfPaveBlock& aLPB = aPBP(iRef);
    //
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next())
    {
      aPB = aItPB.ChangeValue();
      if (aPB == aPBCB)
      {
        //
        aLPBN.Clear();
        aPB->Update(aLPBN);
        //
        aItPBN.Initialize(aLPBN);
        for (; aItPBN.More(); aItPBN.Next())
        {
          Handle(BOPDS_PaveBlock)& aPBN = aItPBN.ChangeValue();
          aLPB.Append(aPBN);
          //
          aPBN->Indices(n1, n2);
          aPK.SetIndices(n1, n2);
          if (aMPKLPB.IsBound(aPK))
          {
            BOPDS_ListOfPaveBlock& aLPBx = aMPKLPB.ChangeFind(aPK);
            aLPBx.Append(aPBN);
          }
          else
          {
            BOPDS_ListOfPaveBlock aLPBx;
            aLPBx.Append(aPBN);
            aMPKLPB.Bind(aPK, aLPBx);
          }
        }
        aLPB.Remove(aItPB);
        break;
      }
    }
  }
  //
  aItMPKLPB.Initialize(aMPKLPB);
  for (; aItMPKLPB.More(); aItMPKLPB.Next())
  {
    BOPDS_ListOfPaveBlock& aLPBx = aItMPKLPB.ChangeValue();
    //
    while (aLPBx.Extent())
    {
      Standard_Boolean      bCoinside;
      BOPDS_ListOfPaveBlock aLPBxN;
      //
      aItPB.Initialize(aLPBx);
      for (; aItPB.More();)
      {
        const Handle(BOPDS_PaveBlock)& aPBx = aItPB.Value();
        if (aLPBxN.Extent())
        {
          const Handle(BOPDS_PaveBlock)& aPBCx = aLPBxN.First();
          bCoinside                            = CheckCoincidence(aPBx, aPBCx, theFuzz);
          if (bCoinside)
          {
            aLPBxN.Append(aPBx);
            aLPBx.Remove(aItPB);
            continue;
          } // if (bCoinside) {
        } // if (aLPBxN.Extent()) {
        else
        {
          aLPBxN.Append(aPBx);
          aLPBx.Remove(aItPB);
          continue;
        }
        aItPB.Next();
      } // for(; aItPB.More(); ) {
      //
      aCBx = new BOPDS_CommonBlock;
      aCBx->SetPaveBlocks(aLPBxN);
      aCBx->SetFaces(aLF);
      //
      aItPB.Initialize(aLPBxN);
      for (; aItPB.More(); aItPB.Next())
      {
        aPB = aItPB.ChangeValue();
        SetCommonBlock(aPB, aCBx);
      }
    }
  }
}

//=================================================================================================

Handle(BOPDS_PaveBlock) BOPDS_DS::RealPaveBlock(const Handle(BOPDS_PaveBlock)& thePB) const
{
  if (IsCommonBlock(thePB))
  {
    const Handle(BOPDS_CommonBlock)& aCB = CommonBlock(thePB);
    const Handle(BOPDS_PaveBlock)&   aPB = aCB->PaveBlock1();
    return aPB;
  }
  return thePB;
}

//=================================================================================================

Standard_Boolean BOPDS_DS::IsCommonBlockOnEdge(const Handle(BOPDS_PaveBlock)& thePB) const
{
  if (IsCommonBlock(thePB))
  {
    const Handle(BOPDS_CommonBlock)& aCB = CommonBlock(thePB);
    return aCB->PaveBlocks().Extent() > 1;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean BOPDS_DS::IsCommonBlock(const Handle(BOPDS_PaveBlock)& thePB) const
{
  return myMapPBCB.IsBound(thePB);
}

//=================================================================================================

Handle(BOPDS_CommonBlock) BOPDS_DS::CommonBlock(const Handle(BOPDS_PaveBlock)& thePB) const
{
  return (IsCommonBlock(thePB) ? myMapPBCB.Find(thePB) : NULL);
}

//=================================================================================================

void BOPDS_DS::SetCommonBlock(const Handle(BOPDS_PaveBlock)&   thePB,
                              const Handle(BOPDS_CommonBlock)& theCB)
{
  if (IsCommonBlock(thePB))
  {
    Handle(BOPDS_CommonBlock)& aCB = myMapPBCB.ChangeFind(thePB);
    aCB                            = theCB;
  }
  else
  {
    myMapPBCB.Bind(thePB, theCB);
  }
}

//
// FaceInfo
//

//=================================================================================================

const BOPDS_VectorOfFaceInfo& BOPDS_DS::FaceInfoPool() const
{
  return myFaceInfoPool;
}

//=================================================================================================

Standard_Boolean BOPDS_DS::HasFaceInfo(const Standard_Integer theI) const
{
  return ShapeInfo(theI).HasReference();
}

//=================================================================================================

const BOPDS_FaceInfo& BOPDS_DS::FaceInfo(const Standard_Integer theI) const
{
  static BOPDS_FaceInfo sFI;
  Standard_Integer      aRef;
  //
  if (HasFaceInfo(theI))
  {
    aRef                      = ShapeInfo(theI).Reference();
    const BOPDS_FaceInfo& aFI = myFaceInfoPool(aRef);
    return aFI;
  }
  return sFI;
}

//=================================================================================================

BOPDS_FaceInfo& BOPDS_DS::ChangeFaceInfo(const Standard_Integer theI)
{
  Standard_Boolean bHasReference;
  Standard_Integer aRef;
  BOPDS_FaceInfo*  pFI;
  //
  BOPDS_ShapeInfo& aSI = ChangeShapeInfo(theI);
  bHasReference        = aSI.HasReference();
  if (!bHasReference)
  {
    InitFaceInfo(theI);
  }
  //
  aRef                      = aSI.Reference();
  const BOPDS_FaceInfo& aFI = myFaceInfoPool(aRef);
  pFI                       = (BOPDS_FaceInfo*)&aFI;
  return *pFI;
}

//=================================================================================================

void BOPDS_DS::InitFaceInfo(const Standard_Integer theI)
{
  Standard_Integer iRef;
  //
  BOPDS_ShapeInfo& aSI = ChangeShapeInfo(theI);
  BOPDS_FaceInfo&  aFI = myFaceInfoPool.Appended();
  iRef                 = myFaceInfoPool.Length() - 1;
  aSI.SetReference(iRef);
  //
  aFI.SetIndex(theI);
  InitFaceInfoIn(theI);
  UpdateFaceInfoOn(theI);
}

//=================================================================================================

void BOPDS_DS::InitFaceInfoIn(const Standard_Integer theI)
{
  BOPDS_ShapeInfo& aSI = ChangeShapeInfo(theI);
  if (aSI.HasReference())
  {
    BOPDS_FaceInfo&     aFI = myFaceInfoPool(aSI.Reference());
    const TopoDS_Shape& aF  = Shape(theI);
    for (TopoDS_Iterator itS(aF); itS.More(); itS.Next())
    {
      const TopoDS_Shape& aV = itS.Value();
      if (aV.ShapeType() == TopAbs_VERTEX)
      {
        Standard_Integer nV = Index(aV);
        HasShapeSD(nV, nV);
        aFI.ChangeVerticesIn().Add(nV);
      }
    }
  }
}

//=================================================================================================

void BOPDS_DS::UpdateFaceInfoIn(const Standard_Integer theI)
{
  Standard_Integer iRef;
  //
  BOPDS_ShapeInfo& aSI = ChangeShapeInfo(theI);
  if (aSI.HasReference())
  {
    iRef                = aSI.Reference();
    BOPDS_FaceInfo& aFI = myFaceInfoPool(iRef);
    //
    BOPDS_IndexedMapOfPaveBlock& aMPBIn = aFI.ChangePaveBlocksIn();
    TColStd_MapOfInteger&        aMVIn  = aFI.ChangeVerticesIn();
    aMPBIn.Clear();
    aMVIn.Clear();
    FaceInfoIn(theI, aMPBIn, aMVIn);
  }
}

//=================================================================================================

void BOPDS_DS::UpdateFaceInfoOn(const Standard_Integer theI)
{
  Standard_Integer iRef;
  //
  BOPDS_ShapeInfo& aSI = ChangeShapeInfo(theI);
  if (aSI.HasReference())
  {
    iRef                = aSI.Reference();
    BOPDS_FaceInfo& aFI = myFaceInfoPool(iRef);
    //
    BOPDS_IndexedMapOfPaveBlock& aMPBOn = aFI.ChangePaveBlocksOn();
    TColStd_MapOfInteger&        aMVOn  = aFI.ChangeVerticesOn();
    aMPBOn.Clear();
    aMVOn.Clear();
    FaceInfoOn(theI, aMPBOn, aMVOn);
  }
}

//=================================================================================================

void BOPDS_DS::FaceInfoOn(const Standard_Integer       theF,
                          BOPDS_IndexedMapOfPaveBlock& theMPB,
                          TColStd_MapOfInteger&        theMI)
{
  Standard_Integer                    nS, nSD, nV1, nV2;
  TColStd_ListIteratorOfListOfInteger aIt;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  //
  const BOPDS_ShapeInfo&       aSI = ShapeInfo(theF);
  const TColStd_ListOfInteger& aLI = aSI.SubShapes();
  aIt.Initialize(aLI);
  for (; aIt.More(); aIt.Next())
  {
    nS                          = aIt.Value();
    const BOPDS_ShapeInfo& aSIE = ShapeInfo(nS);
    if (aSIE.ShapeType() == TopAbs_EDGE)
    {
      const BOPDS_ListOfPaveBlock& aLPB = PaveBlocks(nS);
      aItPB.Initialize(aLPB);
      for (; aItPB.More(); aItPB.Next())
      {
        const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
        aPB->Indices(nV1, nV2);
        theMI.Add(nV1);
        theMI.Add(nV2);
        Handle(BOPDS_PaveBlock) aPBR = RealPaveBlock(aPB);
        theMPB.Add(aPBR);
      }
    } // if (aSIE.ShapeType()==TopAbs_EDGE)
    else
    {
      // nE is TopAbs_VERTEX
      if (HasShapeSD(nS, nSD))
      {
        nS = nSD;
      }
      theMI.Add(nS);
    }
  }
}

//=================================================================================================

void BOPDS_DS::FaceInfoIn(const Standard_Integer       theF,
                          BOPDS_IndexedMapOfPaveBlock& theMPB,
                          TColStd_MapOfInteger&        theMI)
{
  Standard_Integer                    i, aNbVF, aNbEF, nV, nE, nVSD;
  TopoDS_Iterator                     aItS;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  //
  // 1. Pure internal vertices on the face
  const TopoDS_Shape& aF = Shape(theF);
  aItS.Initialize(aF);
  for (; aItS.More(); aItS.Next())
  {
    const TopoDS_Shape& aSx = aItS.Value();
    if (aSx.ShapeType() == TopAbs_VERTEX)
    {
      nV = Index(aSx);
      if (HasShapeSD(nV, nVSD))
      {
        nV = nVSD;
      }
      theMI.Add(nV);
    }
  }
  //
  // 2. aVFs
  BOPDS_VectorOfInterfVF& aVFs = InterfVF();
  aNbVF                        = aVFs.Length();
  for (i = 0; i < aNbVF; ++i)
  {
    BOPDS_InterfVF& aVF = aVFs(i);
    if (aVF.Contains(theF))
    {
      nV = aVF.OppositeIndex(theF);
      if (HasShapeSD(nV, nVSD))
      {
        nV = nVSD;
      }
      theMI.Add(nV);
    }
  }
  //
  // 3. aEFs
  BOPDS_VectorOfInterfEF& aEFs = InterfEF();
  aNbEF                        = aEFs.Length();
  for (i = 0; i < aNbEF; ++i)
  {
    BOPDS_InterfEF& aEF = aEFs(i);
    if (aEF.Contains(theF))
    {
      if (aEF.HasIndexNew(nV))
      {
        if (HasShapeSD(nV, nVSD))
        {
          nV = nVSD;
        }
        theMI.Add(nV);
      }
      else
      {
        nE                                = aEF.OppositeIndex(theF);
        const BOPDS_ListOfPaveBlock& aLPB = PaveBlocks(nE);
        aItPB.Initialize(aLPB);
        for (; aItPB.More(); aItPB.Next())
        {
          const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
          if (IsCommonBlock(aPB))
          {
            const Handle(BOPDS_CommonBlock)& aCB = CommonBlock(aPB);
            if (aCB->Contains(theF))
            {
              const Handle(BOPDS_PaveBlock)& aPB1 = aCB->PaveBlock1();
              theMPB.Add(aPB1);
            }
          }
        } // for (; aItPB.More(); aItPB.Next()) {
      } // else {
    } // if(aEF.Contains(theF)) {
  } // for (i=0; i<aNbEF; ++i) {
}

//=================================================================================================

void BOPDS_DS::UpdateFaceInfoIn(const TColStd_MapOfInteger& theFaces)
{
  for (TColStd_MapOfInteger::Iterator itM(theFaces); itM.More(); itM.Next())
  {
    const Standard_Integer nF  = itM.Value();
    BOPDS_ShapeInfo&       aSI = ChangeShapeInfo(nF);
    if (!aSI.HasReference())
    {
      myFaceInfoPool.Appended().SetIndex(nF);
      aSI.SetReference(myFaceInfoPool.Length() - 1);
    }
    BOPDS_FaceInfo& aFI = myFaceInfoPool(aSI.Reference());
    aFI.ChangePaveBlocksIn().Clear();
    aFI.ChangeVerticesIn().Clear();

    // 1. Add pure internal vertices on the face
    InitFaceInfoIn(nF);
  }

  // 2. Analyze Vertex-Face interferences
  BOPDS_VectorOfInterfVF& aVFs  = InterfVF();
  const Standard_Integer  aNbVF = aVFs.Length();
  for (Standard_Integer iVF = 0; iVF < aNbVF; ++iVF)
  {
    BOPDS_InterfVF&        aVF = aVFs(iVF);
    const Standard_Integer nF  = aVF.Index2();
    if (theFaces.Contains(nF))
    {
      Standard_Integer nV = aVF.Index1();
      HasShapeSD(nV, nV);
      myFaceInfoPool(ShapeInfo(nF).Reference()).ChangeVerticesIn().Add(nV);
    }
  }
  //
  // 3. Analyze Edge-Face interferences
  BOPDS_VectorOfInterfEF& aEFs  = InterfEF();
  const Standard_Integer  aNbEF = aEFs.Length();
  for (Standard_Integer iEF = 0; iEF < aNbEF; ++iEF)
  {
    BOPDS_InterfEF&        aEF = aEFs(iEF);
    const Standard_Integer nF  = aEF.Index2();
    if (theFaces.Contains(nF))
    {
      BOPDS_FaceInfo&  aFI = myFaceInfoPool(ShapeInfo(nF).Reference());
      Standard_Integer nVNew;
      if (aEF.HasIndexNew(nVNew))
      {
        HasShapeSD(nVNew, nVNew);
        aFI.ChangeVerticesIn().Add(nVNew);
      }
      else
      {
        const Standard_Integer       nE   = aEF.Index1();
        const BOPDS_ListOfPaveBlock& aLPB = PaveBlocks(nE);
        for (BOPDS_ListOfPaveBlock::Iterator itPB(aLPB); itPB.More(); itPB.Next())
        {
          const Handle(BOPDS_PaveBlock)&   aPB = itPB.Value();
          const Handle(BOPDS_CommonBlock)& aCB = CommonBlock(aPB);
          if (!aCB.IsNull())
          {
            if (aCB->Contains(nF))
            {
              const Handle(BOPDS_PaveBlock)& aPBR = aCB->PaveBlock1();
              aFI.ChangePaveBlocksIn().Add(aPBR);
            }
          }
        }
      }
    }
  }
}

//=================================================================================================

void BOPDS_DS::UpdateFaceInfoOn(const TColStd_MapOfInteger& theFaces)
{
  for (TColStd_MapOfInteger::Iterator itM(theFaces); itM.More(); itM.Next())
  {
    const Standard_Integer nF  = itM.Value();
    BOPDS_ShapeInfo&       aSI = ChangeShapeInfo(nF);
    if (!aSI.HasReference())
    {
      myFaceInfoPool.Appended().SetIndex(nF);
      aSI.SetReference(myFaceInfoPool.Length() - 1);
    }
    BOPDS_FaceInfo& aFI = myFaceInfoPool(aSI.Reference());
    aFI.ChangePaveBlocksOn().Clear();
    aFI.ChangeVerticesOn().Clear();

    FaceInfoOn(nF, aFI.ChangePaveBlocksOn(), aFI.ChangeVerticesOn());
  }
}

//=================================================================================================

void BOPDS_DS::RefineFaceInfoOn()
{
  Standard_Integer            i, aNb, nF, aNbPB, j;
  BOPDS_IndexedMapOfPaveBlock aMPB;
  //
  aNb = myFaceInfoPool.Length();
  for (i = 0; i < aNb; ++i)
  {
    BOPDS_FaceInfo& aFI = myFaceInfoPool(i);
    nF                  = aFI.Index();
    UpdateFaceInfoOn(nF);
    BOPDS_IndexedMapOfPaveBlock& aMPBOn = aFI.ChangePaveBlocksOn();
    //
    aMPB.Clear();
    aMPB.Assign(aMPBOn);
    aMPBOn.Clear();
    //
    aNbPB = aMPB.Extent();
    for (j = 1; j <= aNbPB; ++j)
    {
      const Handle(BOPDS_PaveBlock)& aPB = aMPB(j);
      if (aPB->HasEdge())
      {
        aMPBOn.Add(aPB);
      }
    }
  }
}

//=================================================================================================

void BOPDS_DS::RefineFaceInfoIn()
{
  for (Standard_Integer i = 0; i < myNbSourceShapes; ++i)
  {
    const BOPDS_ShapeInfo& aSI = ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE)
      continue;

    if (!aSI.HasReference())
      continue;

    BOPDS_FaceInfo& aFI = ChangeFaceInfo(i);

    const BOPDS_IndexedMapOfPaveBlock& aMPBOn = aFI.PaveBlocksOn();
    BOPDS_IndexedMapOfPaveBlock&       aMPBIn = aFI.ChangePaveBlocksIn();

    if (aMPBIn.IsEmpty() || aMPBOn.IsEmpty())
      continue;

    BOPDS_IndexedMapOfPaveBlock aMPBInNew;

    const Standard_Integer aNbPBIn = aMPBIn.Extent();
    for (Standard_Integer j = 1; j <= aNbPBIn; ++j)
    {
      if (!aMPBOn.Contains(aMPBIn(j)))
        aMPBInNew.Add(aMPBIn(j));
    }

    if (aMPBInNew.Extent() < aNbPBIn)
      aMPBIn = aMPBInNew;
  }
}

//=================================================================================================

void BOPDS_DS::AloneVertices(const Standard_Integer theI, TColStd_ListOfInteger& theLI) const
{
  if (HasFaceInfo(theI))
  {
    //
    Standard_Integer                  i, j, nV1, nV2, nV, aNbPB;
    TColStd_MapIteratorOfMapOfInteger aItMI;
    //
    TColStd_MapOfInteger aMI(100, myAllocator);
    //
    const BOPDS_FaceInfo& aFI = FaceInfo(theI);
    //
    for (i = 0; i < 2; ++i)
    {
      const BOPDS_IndexedMapOfPaveBlock& aMPB = (!i) ? aFI.PaveBlocksIn() : aFI.PaveBlocksSc();
      aNbPB                                   = aMPB.Extent();
      for (j = 1; j <= aNbPB; ++j)
      {
        const Handle(BOPDS_PaveBlock)& aPB = aMPB(j);
        aPB->Indices(nV1, nV2);
        aMI.Add(nV1);
        aMI.Add(nV2);
      }
    }
    //
    for (i = 0; i < 2; ++i)
    {
      const TColStd_MapOfInteger& aMIV = (!i) ? aFI.VerticesIn() : aFI.VerticesSc();
      aItMI.Initialize(aMIV);
      for (; aItMI.More(); aItMI.Next())
      {
        nV = aItMI.Value();
        if (nV >= 0)
        {
          if (aMI.Add(nV))
          {
            theLI.Append(nV);
          }
        }
      }
    }
  }
}

//=================================================================================================

void BOPDS_DS::SubShapesOnIn(const Standard_Integer       theNF1,
                             const Standard_Integer       theNF2,
                             TColStd_MapOfInteger&        theMVOnIn,
                             TColStd_MapOfInteger&        theMVCommon,
                             BOPDS_IndexedMapOfPaveBlock& thePBOnIn,
                             BOPDS_MapOfPaveBlock&        theCommonPB) const
{
  Standard_Integer                  i, j, nV, nV1, nV2, aNbPB;
  TColStd_MapIteratorOfMapOfInteger aIt;
  BOPDS_IndexedMapOfPaveBlock       pMPB[4];
  //
  const BOPDS_FaceInfo& aFI1 = FaceInfo(theNF1);
  const BOPDS_FaceInfo& aFI2 = FaceInfo(theNF2);
  //
  pMPB[0] = aFI1.PaveBlocksOn();
  pMPB[1] = aFI1.PaveBlocksIn();
  pMPB[2] = aFI2.PaveBlocksOn();
  pMPB[3] = aFI2.PaveBlocksIn();
  //
  for (i = 0; i < 4; ++i)
  {
    aNbPB = pMPB[i].Extent();
    for (j = 1; j <= aNbPB; ++j)
    {
      const Handle(BOPDS_PaveBlock)& aPB = pMPB[i](j);
      thePBOnIn.Add(aPB);
      aPB->Indices(nV1, nV2);

      theMVOnIn.Add(nV1);
      theMVOnIn.Add(nV2);

      if (i < 2)
      {
        if (pMPB[2].Contains(aPB) || pMPB[3].Contains(aPB))
        {
          theCommonPB.Add(aPB);
          theMVCommon.Add(nV1);
          theMVCommon.Add(nV2);
        }
      }
    }
  }
  //
  const TColStd_MapOfInteger& aMVOn1 = aFI1.VerticesOn();
  const TColStd_MapOfInteger& aMVIn1 = aFI1.VerticesIn();
  const TColStd_MapOfInteger& aMVOn2 = aFI2.VerticesOn();
  const TColStd_MapOfInteger& aMVIn2 = aFI2.VerticesIn();
  //
  for (i = 0; i < 2; ++i)
  {
    const TColStd_MapOfInteger& aMV1 = (!i) ? aMVOn1 : aMVIn1;
    aIt.Initialize(aMV1);
    for (; aIt.More(); aIt.Next())
    {
      nV = aIt.Value();
      if (aMVOn2.Contains(nV) || aMVIn2.Contains(nV))
      {
        theMVOnIn.Add(nV);

        // Vertex taken from the 1st face is in the 2nd one.
        theMVCommon.Add(nV);
      }
    }
  }
}

//=================================================================================================

void BOPDS_DS::SharedEdges(const Standard_Integer                   nF1,
                           const Standard_Integer                   nF2,
                           TColStd_ListOfInteger&                   theLI,
                           const Handle(NCollection_BaseAllocator)& aAllocator)
{
  Standard_Integer                    nE, nSp;
  TColStd_ListIteratorOfListOfInteger aItLI;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  TColStd_MapOfInteger                aMI(100, aAllocator);
  //
  const BOPDS_ShapeInfo&       aSI1 = ShapeInfo(nF1);
  const TColStd_ListOfInteger& aLI1 = aSI1.SubShapes();
  aItLI.Initialize(aLI1);
  for (; aItLI.More(); aItLI.Next())
  {
    nE                          = aItLI.Value();
    const BOPDS_ShapeInfo& aSIE = ChangeShapeInfo(nE);
    if (aSIE.ShapeType() == TopAbs_EDGE)
    {
      const BOPDS_ListOfPaveBlock& aLPB = PaveBlocks(nE);
      if (aLPB.IsEmpty())
      {
        aMI.Add(nE);
      }
      else
      {
        aItLPB.Initialize(aLPB);
        for (; aItLPB.More(); aItLPB.Next())
        {
          const Handle(BOPDS_PaveBlock) aPB = RealPaveBlock(aItLPB.Value());
          nSp                               = aPB->Edge();
          aMI.Add(nSp);
        }
      }
    }
  }
  //
  const BOPDS_ShapeInfo&       aSI2 = ShapeInfo(nF2);
  const TColStd_ListOfInteger& aLI2 = aSI2.SubShapes();
  aItLI.Initialize(aLI2);
  for (; aItLI.More(); aItLI.Next())
  {
    nE                          = aItLI.Value();
    const BOPDS_ShapeInfo& aSIE = ChangeShapeInfo(nE);
    if (aSIE.ShapeType() == TopAbs_EDGE)
    {
      const BOPDS_ListOfPaveBlock& aLPB = PaveBlocks(nE);
      if (aLPB.IsEmpty())
      {
        if (aMI.Contains(nE))
        {
          theLI.Append(nE);
        }
      }
      else
      {
        aItLPB.Initialize(aLPB);
        for (; aItLPB.More(); aItLPB.Next())
        {
          const Handle(BOPDS_PaveBlock) aPB = RealPaveBlock(aItLPB.Value());
          nSp                               = aPB->Edge();
          if (aMI.Contains(nSp))
          {
            theLI.Append(nSp);
          }
        }
      }
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// same domain shapes
//
//=================================================================================================

TColStd_DataMapOfIntegerInteger& BOPDS_DS::ShapesSD()
{
  return myShapesSD;
}

//=================================================================================================

void BOPDS_DS::AddShapeSD(const Standard_Integer theIndex, const Standard_Integer theIndexSD)
{
  if (theIndex != theIndexSD)
    myShapesSD.Bind(theIndex, theIndexSD);
}

//=================================================================================================

Standard_Boolean BOPDS_DS::HasShapeSD(const Standard_Integer theIndex,
                                      Standard_Integer&      theIndexSD) const
{
  Standard_Boolean        bHasSD = Standard_False;
  const Standard_Integer* pSD    = myShapesSD.Seek(theIndex);
  while (pSD)
  {
    theIndexSD = *pSD;
    bHasSD     = Standard_True;
    pSD        = myShapesSD.Seek(theIndexSD);
  }
  return bHasSD;
}

//=================================================================================================

void BOPDS_DS::Dump() const
{
  Standard_Integer i, aNb, aNbSS;
  //
  printf(" *** DS ***\n");
  aNb = NbRanges();
  printf(" Ranges:%d\n", aNb);
  for (i = 0; i < aNb; ++i)
  {
    const BOPDS_IndexRange& aR = Range(i);
    aR.Dump();
    printf("\n");
  }
  //
  aNbSS = NbSourceShapes();
  printf(" Shapes:%d\n", aNbSS);
  aNb = NbShapes();
  for (i = 0; i < aNb; ++i)
  {
    const BOPDS_ShapeInfo& aSI = ShapeInfo(i);
    printf(" %d :", i);
    aSI.Dump();
    printf("\n");
    if (i == aNbSS - 1)
    {
      printf(" ****** adds\n");
    }
  }
  printf(" ******\n");
}

//=================================================================================================

Standard_Boolean BOPDS_DS::CheckCoincidence(const Handle(BOPDS_PaveBlock)& aPB1,
                                            const Handle(BOPDS_PaveBlock)& aPB2,
                                            const Standard_Real            theFuzz)
{
  Standard_Boolean bRet;
  Standard_Integer nE1, nE2, aNbPoints;
  Standard_Real    aT11, aT12, aT21, aT22, aT1m, aD, aTol, aT2x;
  gp_Pnt           aP1m;
  //
  bRet = Standard_False;
  //
  aPB1->Range(aT11, aT12);
  aT1m                   = IntTools_Tools::IntermediatePoint(aT11, aT12);
  nE1                    = aPB1->OriginalEdge();
  const TopoDS_Edge& aE1 = (*(TopoDS_Edge*)(&Shape(nE1)));
  BOPTools_AlgoTools::PointOnEdge(aE1, aT1m, aP1m);
  //
  aPB2->Range(aT21, aT22);
  nE2                    = aPB2->OriginalEdge();
  const TopoDS_Edge& aE2 = (*(TopoDS_Edge*)(&Shape(nE2)));
  //
  Standard_Real               f, l;
  Handle(Geom_Curve)          aC2 = BRep_Tool::Curve(aE2, f, l);
  GeomAPI_ProjectPointOnCurve aPPC;
  aPPC.Init(aC2, f, l);
  aPPC.Perform(aP1m);
  aNbPoints = aPPC.NbPoints();
  if (aNbPoints)
  {
    aD = aPPC.LowerDistance();
    //
    aTol = BRep_Tool::MaxTolerance(aE1, TopAbs_VERTEX);
    aTol =
      aTol + BRep_Tool::MaxTolerance(aE2, TopAbs_VERTEX) + Max(theFuzz, Precision::Confusion());
    if (aD < aTol)
    {
      aT2x = aPPC.LowerDistanceParameter();
      if (aT2x > aT21 && aT2x < aT22)
      {
        return !bRet;
      }
    }
  }
  return bRet;
}

//=================================================================================================

Standard_Boolean BOPDS_DS::IsSubShape(const Standard_Integer theI1, const Standard_Integer theI2)
{
  Standard_Boolean bRet;
  Standard_Integer nS;
  bRet = Standard_False;
  //
  TColStd_ListIteratorOfListOfInteger aItLI;
  //
  const BOPDS_ShapeInfo&       aSI = ShapeInfo(theI2);
  const TColStd_ListOfInteger& aLI = aSI.SubShapes();
  aItLI.Initialize(aLI);
  for (; aItLI.More(); aItLI.Next())
  {
    nS = aItLI.Value();
    if (nS == theI1)
    {
      bRet = Standard_True;
      break;
    }
  }

  return bRet;
}

//=================================================================================================

void BOPDS_DS::Paves(const Standard_Integer theEdge, BOPDS_ListOfPave& theLP)
{
  Standard_Integer                    aNb, i;
  BOPDS_ListIteratorOfListOfPaveBlock aIt;
  BOPDS_MapOfPave                     aMP;
  //
  const BOPDS_ListOfPaveBlock& aLPB = PaveBlocks(theEdge);
  aNb                               = aLPB.Extent() + 1;
  if (aNb == 1)
  {
    return;
  }
  //
  BOPDS_VectorOfPave pPaves(1, aNb);
  //
  i = 1;
  aIt.Initialize(aLPB);
  for (; aIt.More(); aIt.Next())
  {
    const Handle(BOPDS_PaveBlock)& aPB    = aIt.Value();
    const BOPDS_Pave&              aPave1 = aPB->Pave1();
    const BOPDS_Pave&              aPave2 = aPB->Pave2();
    //
    if (aMP.Add(aPave1))
    {
      pPaves(i) = aPave1;
      ++i;
    }
    //
    if (aMP.Add(aPave2))
    {
      pPaves(i) = aPave2;
      ++i;
    }
  }
  //
  Standard_ASSERT_VOID(aNb == aMP.Extent(), "Abnormal number of paves");
  //
  std::sort(pPaves.begin(), pPaves.end());
  //
  for (i = 1; i <= aNb; ++i)
  {
    theLP.Append(pPaves(i));
  }
}

//=================================================================================================

void TotalShapes(const TopoDS_Shape& aS, Standard_Integer& aNbS, TopTools_MapOfShape& aMS)
{
  if (aMS.Add(aS))
  {
    TopoDS_Iterator aIt;
    ++aNbS;
    aIt.Initialize(aS);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aSx = aIt.Value();
      TotalShapes(aSx, aNbS, aMS);
    }
  }
}

//=================================================================================================

Standard_Real ComputeParameter(const TopoDS_Vertex& aV, const TopoDS_Edge& aE)
{
  Standard_Real      aT1, aT2, aTRet, aTolE2, aD2;
  gp_Pnt             aPC, aPV;
  Handle(Geom_Curve) aC3D;
  TopoDS_Edge        aEE;
  //
  aEE = aE;
  aEE.Orientation(TopAbs_FORWARD);
  //
  aTRet = 0.;
  //
  aTolE2 = BRep_Tool::Tolerance(aE);
  aTolE2 = aTolE2 * aTolE2;
  //
  aPV = BRep_Tool::Pnt(aV);
  //
  aC3D = BRep_Tool::Curve(aEE, aT1, aT2);
  //
  aC3D->D0(aT1, aPC);
  aD2 = aPC.SquareDistance(aPV);
  if (aD2 < aTolE2)
  {
    aTRet = aT1;
  }
  //
  aC3D->D0(aT2, aPC);
  aD2 = aPC.SquareDistance(aPV);
  if (aD2 < aTolE2)
  {
    aTRet = aT2;
  }
  //
  return aTRet;
}

//=================================================================================================

void BOPDS_DS::BuildBndBoxSolid(const Standard_Integer theIndex,
                                Bnd_Box&               aBoxS,
                                const Standard_Boolean theCheckInverted)
{
  Standard_Boolean                    bIsOpenBox, bIsInverted;
  Standard_Integer                    nSh, nFc;
  Standard_Real                       aTolS, aTolFc;
  TColStd_ListIteratorOfListOfInteger aItLI, aItLI1;
  //
  const BOPDS_ShapeInfo& aSI    = ShapeInfo(theIndex);
  const TopoDS_Shape&    aS     = aSI.Shape();
  const TopoDS_Solid&    aSolid = (*(TopoDS_Solid*)(&aS));
  //
  bIsOpenBox = Standard_False;
  //
  aTolS                              = 0.;
  const TColStd_ListOfInteger& aLISh = aSI.SubShapes();
  aItLI.Initialize(aLISh);
  for (; aItLI.More(); aItLI.Next())
  {
    nSh                          = aItLI.Value();
    const BOPDS_ShapeInfo& aSISh = ShapeInfo(nSh);
    if (aSISh.ShapeType() != TopAbs_SHELL)
    {
      continue;
    }
    //
    const TColStd_ListOfInteger& aLIFc = aSISh.SubShapes();
    aItLI1.Initialize(aLIFc);
    for (; aItLI1.More(); aItLI1.Next())
    {
      nFc                          = aItLI1.Value();
      const BOPDS_ShapeInfo& aSIFc = ShapeInfo(nFc);
      if (aSIFc.ShapeType() != TopAbs_FACE)
      {
        continue;
      }
      //
      const Bnd_Box& aBFc = aSIFc.Box();
      aBoxS.Add(aBFc);
      //
      if (!bIsOpenBox)
      {
        bIsOpenBox = (aBFc.IsOpenXmin() || aBFc.IsOpenXmax() || aBFc.IsOpenYmin()
                      || aBFc.IsOpenYmax() || aBFc.IsOpenZmin() || aBFc.IsOpenZmax());
        if (bIsOpenBox)
        {
          break;
        }
      }
      //
      const TopoDS_Face& aFc = *((TopoDS_Face*)&aSIFc.Shape());
      aTolFc                 = BRep_Tool::Tolerance(aFc);
      if (aTolFc > aTolS)
      {
        aTolS = aTolFc;
      }
    } // for (; aItLI1.More(); aItLI1.Next()) {
    if (bIsOpenBox)
    {
      break;
    }
    //
    const TopoDS_Shell& aSh = *((TopoDS_Shell*)&aSISh.Shape());
    bIsOpenBox              = BOPTools_AlgoTools::IsOpenShell(aSh);
    if (bIsOpenBox)
    {
      break;
    }
  } // for (; aItLI.More(); aItLI.Next()) {
  //
  if (bIsOpenBox)
  {
    aBoxS.SetWhole();
  }
  else if (theCheckInverted)
  {
    bIsInverted = BOPTools_AlgoTools::IsInvertedSolid(aSolid);
    if (bIsInverted)
    {
      aBoxS.SetWhole();
    }
  }
}

//=================================================================================================

void BOPDS_DS::UpdatePaveBlocksWithSDVertices()
{
  Standard_Integer                    i, aNbPBP;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  //
  BOPDS_VectorOfListOfPaveBlock& aPBP = myPaveBlocksPool;
  //
  aNbPBP = aPBP.Size();
  for (i = 0; i < aNbPBP; ++i)
  {
    BOPDS_ListOfPaveBlock& aLPB = aPBP(i);
    //
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next())
    {
      Handle(BOPDS_PaveBlock)& aPB = aItPB.ChangeValue();
      UpdatePaveBlockWithSDVertices(aPB);
    } // for (; aItPB.More(); aItPB.Next()) {
  } // for (i = 0; i < aNbPBP; ++i) {
}

//=================================================================================================

void BOPDS_DS::UpdatePaveBlockWithSDVertices(const Handle(BOPDS_PaveBlock)& thePB)
{
  Standard_Integer nV1, nV2;
  BOPDS_Pave       aPave1, aPave2;
  //
  aPave1 = thePB->Pave1();
  aPave2 = thePB->Pave2();
  //
  nV1 = aPave1.Index();
  nV2 = aPave2.Index();
  //
  if (HasShapeSD(nV1, nV1))
  {
    aPave1.SetIndex(nV1);
    thePB->SetPave1(aPave1);
  }
  //
  if (HasShapeSD(nV2, nV2))
  {
    aPave2.SetIndex(nV2);
    thePB->SetPave2(aPave2);
  }
}

//=================================================================================================

void BOPDS_DS::UpdateCommonBlockWithSDVertices(const Handle(BOPDS_CommonBlock)& theCB)
{
  const BOPDS_ListOfPaveBlock&        aLPB = theCB->PaveBlocks();
  BOPDS_ListIteratorOfListOfPaveBlock aItPB(aLPB);
  for (; aItPB.More(); aItPB.Next())
  {
    const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
    UpdatePaveBlockWithSDVertices(aPB);
  }
}

//=================================================================================================

void BOPDS_DS::InitPaveBlocksForVertex(const Standard_Integer theNV)
{
  const TColStd_ListOfInteger* pLE = myMapVE.Seek(theNV);
  if (!pLE)
    return;

  TColStd_ListIteratorOfListOfInteger aItLE(*pLE);
  for (; aItLE.More(); aItLE.Next())
    ChangePaveBlocks(aItLE.Value());
}

//=================================================================================================

void BOPDS_DS::ReleasePaveBlocks()
{
  // It is necessary to remove the reference to PaveBlocks for the untouched
  // edges to avoid creation of the same images for them.
  // Pave blocks for this reference should be cleared.
  // This will allow to differ the small edges, for which it is
  // impossible to even build a pave block from the normal edges for which the
  // pave block have been created, but stayed untouched.
  // The small edge, for which no pave blocks have been created,
  // should be avoided in the result, thus the reference to empty list
  // of pave blocks will stay to mark the edge as Deleted.

  BOPDS_VectorOfListOfPaveBlock& aPBP   = ChangePaveBlocksPool();
  Standard_Integer               aNbPBP = aPBP.Length();
  if (!aNbPBP)
  {
    return;
  }
  //
  for (Standard_Integer i = 0; i < aNbPBP; ++i)
  {
    BOPDS_ListOfPaveBlock& aLPB = aPBP(i);
    if (aLPB.Extent() == 1)
    {
      const Handle(BOPDS_PaveBlock)& aPB = aLPB.First();
      if (!IsCommonBlock(aPB))
      {
        Standard_Integer nV1, nV2;
        aPB->Indices(nV1, nV2);
        if (!IsNewShape(nV1) && !IsNewShape(nV2))
        {
          // Both vertices are original, thus the PB is untouched.
          // Remove reference for the original edge
          Standard_Integer nE = aPB->OriginalEdge();
          if (nE >= 0)
          {
            ChangeShapeInfo(nE).SetReference(-1);
          }
          // Clear contents of the list
          aLPB.Clear();
        }
      }
    }
  }
}

//=================================================================================================

Standard_Boolean BOPDS_DS::IsValidShrunkData(const Handle(BOPDS_PaveBlock)& thePB)
{
  if (!thePB->HasShrunkData())
    return Standard_False;

  // Compare the distances from the bounds of the shrunk range to the vertices
  // with the tolerance values of vertices

  // Shrunk range
  Standard_Real    aTS[2];
  Bnd_Box          aBox;
  Standard_Boolean bIsSplit;
  //
  thePB->ShrunkData(aTS[0], aTS[1], aBox, bIsSplit);
  //
  // Vertices
  Standard_Integer nV[2];
  thePB->Indices(nV[0], nV[1]);
  //
  const TopoDS_Edge& aE = TopoDS::Edge(Shape(thePB->OriginalEdge()));
  BRepAdaptor_Curve  aBAC(aE);
  //
  Standard_Real anEps = BRep_Tool::Tolerance(aE) * 0.01;
  //
  for (Standard_Integer i = 0; i < 2; ++i)
  {
    const TopoDS_Vertex& aV   = TopoDS::Vertex(Shape(nV[i]));
    Standard_Real        aTol = BRep_Tool::Tolerance(aV) + Precision::Confusion();
    // Bounding point
    gp_Pnt aP = BRep_Tool::Pnt(aV);
    //
    // Point on the end of shrunk range
    gp_Pnt aPS = aBAC.Value(aTS[i]);
    //
    Standard_Real aDist = aP.Distance(aPS);
    if (aTol - aDist > anEps)
    {
      return Standard_False;
    }
  }
  return Standard_True;
}