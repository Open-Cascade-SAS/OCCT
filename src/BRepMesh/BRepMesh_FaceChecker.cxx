// Created on: 2016-07-04
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_FaceChecker.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_Edge.hxx>
#include <OSD_Parallel.hxx>
#include <BRepMesh_GeomTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_FaceChecker, Standard_Transient)

namespace
{
  const Standard_Real MaxTangentAngle = 5. * M_PI / 180.;

  //! Functor to be used to fill segments and bounding box tree in parallel.
  class SegmentsFiller
  {
  public:
    //! Constructor.
    SegmentsFiller(const IMeshData::IFaceHandle&                    theDFace,
                   Handle(BRepMesh_FaceChecker::ArrayOfSegments)&   theWiresSegments,
                   Handle(BRepMesh_FaceChecker::ArrayOfBndBoxTree)& theWiresBndBoxTree)
      : myDFace(theDFace),
        myWiresSegments(theWiresSegments),
        myWiresBndBoxTree(theWiresBndBoxTree)
    {
      myWiresSegments   = new BRepMesh_FaceChecker::ArrayOfSegments   (0, myDFace->WiresNb() - 1);
      myWiresBndBoxTree = new BRepMesh_FaceChecker::ArrayOfBndBoxTree (0, myDFace->WiresNb() - 1);
    }

    //! Performs initialization of wire with the given index.
    void operator()(const Standard_Integer theWireIndex) const
    {
      const IMeshData::IWireHandle& aDWire = myDFace->GetWire(theWireIndex);

      Handle(NCollection_IncAllocator) aTmpAlloc1 = new NCollection_IncAllocator();

      Handle(BRepMesh_FaceChecker::Segments) aSegments = 
        new BRepMesh_FaceChecker::Segments(aDWire->EdgesNb(), aTmpAlloc1);
      Handle(IMeshData::BndBox2dTree) aBndBoxTree = new IMeshData::BndBox2dTree(aTmpAlloc1);

      myWiresSegments  ->ChangeValue(theWireIndex) = aSegments;
      myWiresBndBoxTree->ChangeValue(theWireIndex) = aBndBoxTree;

      Handle(NCollection_IncAllocator) aTmpAlloc2 = new NCollection_IncAllocator();
      IMeshData::BndBox2dTreeFiller aBndBoxTreeFiller(*aBndBoxTree, aTmpAlloc2);

      for (Standard_Integer aEdgeIt = 0; aEdgeIt < aDWire->EdgesNb(); ++aEdgeIt)
      {
        // TODO: check 2d wire for consistency.

        const IMeshData::IEdgePtr&      aDEdge  = aDWire->GetEdge(aEdgeIt);
        const IMeshData::IPCurveHandle& aPCurve = aDEdge->GetPCurve(myDFace.get(), aDWire->GetEdgeOrientation(aEdgeIt));

        for (Standard_Integer aPointIt = 1; aPointIt < aPCurve->ParametersNb(); ++aPointIt)
        {
          gp_Pnt2d& aPnt1 = aPCurve->GetPoint(aPointIt - 1);
          gp_Pnt2d& aPnt2 = aPCurve->GetPoint(aPointIt);

          Bnd_Box2d aBox;
          aBox.Add(aPnt1);
          aBox.Add(aPnt2);
          aBox.Enlarge(Precision::Confusion());

          aBndBoxTreeFiller.Add(aSegments->Size(), aBox);
          aSegments->Append(BRepMesh_FaceChecker::Segment(aDEdge, &aPnt1, &aPnt2));
        }
      }

      aBndBoxTreeFiller.Fill();
    }

  private:

    SegmentsFiller (const SegmentsFiller& theOther);

    void operator=(const SegmentsFiller& theOther);

  private:

    const IMeshData::IFaceHandle&                    myDFace;
    Handle(BRepMesh_FaceChecker::ArrayOfSegments)&   myWiresSegments;
    Handle(BRepMesh_FaceChecker::ArrayOfBndBoxTree)& myWiresBndBoxTree;
  };

  //! Selector.
  //! Used to identify segments with overlapped bounding boxes.
  class BndBox2dTreeSelector : public IMeshData::BndBox2dTree::Selector
  {
  public:
    //! Constructor.
    BndBox2dTreeSelector(const Standard_Real theTolerance)
      : myMaxLoopSize(M_PI * theTolerance * theTolerance),
        mySelfSegmentIndex(-1),
        mySegment(0),
        myIndices(256, new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE))
    {
    }

    //! Sets working set of segments.
    void SetSegments(const Handle(BRepMesh_FaceChecker::Segments)& theSegments)
    {
      mySegments = theSegments;
    }

    //! Resets current selector.
    void Reset(const BRepMesh_FaceChecker::Segment* theSegment,
               const Standard_Integer               theSelfSegmentIndex)
    {
      myIndices.Clear();

      mySelfSegmentIndex = theSelfSegmentIndex;
      mySegment = theSegment;

      myBox.SetVoid();
      myBox.Add(*mySegment->Point1);
      myBox.Add(*mySegment->Point2);
      myBox.Enlarge(Precision::Confusion());
    }

    //! Indicates should the given box be rejected or not.
    virtual Standard_Boolean Reject(const Bnd_Box2d& theBox) const
    {
      return myBox.IsOut(theBox);
    }

    //! Accepts segment with the given index in case if it fits conditions.
    virtual Standard_Boolean Accept(const Standard_Integer& theSegmentIndex)
    {
      const BRepMesh_FaceChecker::Segment& aSegment = mySegments->Value(theSegmentIndex);

      gp_Pnt2d aIntPnt;
      const BRepMesh_GeomTool::IntFlag aIntStatus = BRepMesh_GeomTool::IntSegSeg(
        mySegment->Point1->XY(), mySegment->Point2->XY(),
        aSegment.Point1->XY(), aSegment.Point2->XY(),
        Standard_False, Standard_False, aIntPnt);

      if (aIntStatus == BRepMesh_GeomTool::Cross)
      {
        const Standard_Real aAngle = gp_Vec2d(mySegment->Point1->XY(), mySegment->Point2->XY()).Angle(
                                     gp_Vec2d(aSegment.Point1->XY(), aSegment.Point2->XY()));

        if (Abs(aAngle) < MaxTangentAngle)
        {
          return Standard_False;
        }

        if (mySelfSegmentIndex != -1)
        {
          gp_XY aPrevVec;
          Standard_Real aSumS = 0.;
          const gp_XY& aRefPnt = aIntPnt.Coord();
          for (Standard_Integer i = mySelfSegmentIndex; i < theSegmentIndex; ++i)
          {
            const BRepMesh_FaceChecker::Segment& aCurrSegment = mySegments->Value(i);
            gp_XY aCurVec = aCurrSegment.Point2->XY() - aRefPnt;

            if (aCurVec.SquareModulus() < gp::Resolution())
              continue;

            if (aPrevVec.SquareModulus() > gp::Resolution())
              aSumS += aPrevVec ^ aCurVec;

            aPrevVec = aCurVec;
          }

          if (Abs(aSumS / 2.) < myMaxLoopSize)
          {
            return Standard_False;
          }
        }

        myIndices.Append(theSegmentIndex);
        return Standard_True;
      }

      return Standard_False;
    }

    //! Returns indices of intersecting segments.
    const IMeshData::VectorOfInteger& Indices() const
    {
      return myIndices;
    }

  private:

    Standard_Real                          myMaxLoopSize;
    Standard_Integer                       mySelfSegmentIndex;
    Handle(BRepMesh_FaceChecker::Segments) mySegments;
    const BRepMesh_FaceChecker::Segment*   mySegment;
    Bnd_Box2d                              myBox;
    IMeshData::VectorOfInteger             myIndices;
  };
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_FaceChecker::BRepMesh_FaceChecker(
  const IMeshData::IFaceHandle& theFace,
  const IMeshTools_Parameters&  theParameters)
  : myDFace(theFace),
    myParameters(theParameters)
{
}

//=======================================================================
//function : Destructor
//purpose  : 
//=======================================================================
BRepMesh_FaceChecker::~BRepMesh_FaceChecker()
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_FaceChecker::Perform()
{
  myIntersectingEdges = new IMeshData::MapOfIEdgePtr;
  collectSegments();

  OSD_Parallel::For(0, myDFace->WiresNb(), *this, !isParallel());
  collectResult();

  myWiresBndBoxTree.Nullify();
  myWiresSegments.Nullify();
  myWiresIntersectingEdges.Nullify();
  return myIntersectingEdges->IsEmpty();
}

//=======================================================================
//function : collectSegments
//purpose  : 
//=======================================================================
void BRepMesh_FaceChecker::collectSegments()
{
  SegmentsFiller aSegmentsFiller(myDFace, myWiresSegments, myWiresBndBoxTree);
  OSD_Parallel::For(0, myDFace->WiresNb(), aSegmentsFiller, !isParallel());

  myWiresIntersectingEdges = new ArrayOfMapOfIEdgePtr(0, myDFace->WiresNb() - 1);
}

//=======================================================================
//function : perform
//purpose  : 
//=======================================================================
void BRepMesh_FaceChecker::perform(const Standard_Integer theWireIndex) const
{
  const Handle(Segments)&           aSegments1     = myWiresSegments->Value(theWireIndex);
  Handle(IMeshData::MapOfIEdgePtr)& aIntersections = myWiresIntersectingEdges->ChangeValue(theWireIndex);

  // TODO: Tolerance is set to twice value of face deflection in order to fit regressions.
  BndBox2dTreeSelector aSelector(2 * myDFace->GetDeflection());
  for (Standard_Integer aWireIt = theWireIndex; aWireIt < myDFace->WiresNb(); ++aWireIt)
  {
    const Handle(IMeshData::BndBox2dTree)& aBndBoxTree2 = myWiresBndBoxTree->Value(aWireIt);
    const Handle(Segments)&                aSegments2 = myWiresSegments->Value(aWireIt);

    aSelector.SetSegments(aSegments2);
    for (Standard_Integer aSegmentIt = 0; aSegmentIt < aSegments1->Size(); ++aSegmentIt)
    {
      const BRepMesh_FaceChecker::Segment& aSegment1 = aSegments1->Value(aSegmentIt);
      aSelector.Reset(&aSegment1, (aWireIt == theWireIndex) ? aSegmentIt : -1);
      if (aBndBoxTree2->Select(aSelector) != 0)
      {
        if (aIntersections.IsNull())
        {
          aIntersections = new IMeshData::MapOfIEdgePtr;
        }

        aIntersections->Add(aSegment1.EdgePtr);

        const IMeshData::VectorOfInteger& aSegments = aSelector.Indices();
        for (Standard_Integer aSelIt = 0; aSelIt < aSegments.Size(); ++aSelIt)
        {
          const BRepMesh_FaceChecker::Segment& aSegment2 = aSegments2->Value(aSegments(aSelIt));
          aIntersections->Add(aSegment2.EdgePtr);
        }
      }
    }
  }
}

//=======================================================================
//function : collectResult
//purpose  : 
//=======================================================================
void BRepMesh_FaceChecker::collectResult()
{
  for (Standard_Integer aWireIt = 0; aWireIt < myDFace->WiresNb(); ++aWireIt)
  {
    const Handle(IMeshData::MapOfIEdgePtr)& aEdges = myWiresIntersectingEdges->Value(aWireIt);
    if (!aEdges.IsNull())
    {
      myIntersectingEdges->Unite(*aEdges);
    }
  }
}
