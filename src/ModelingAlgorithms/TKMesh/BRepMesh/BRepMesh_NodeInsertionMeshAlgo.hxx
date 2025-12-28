// Created on: 2016-07-07
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

#ifndef _BRepMesh_NodeInsertionMeshAlgo_HeaderFile
#define _BRepMesh_NodeInsertionMeshAlgo_HeaderFile

#include <BRepMesh_Classifier.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_PCurve.hxx>
#include <BRepMesh_Vertex.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>
#include <Standard_ErrorHandler.hxx>
#include <BRepMesh_Delaun.hxx>

//! Extends base meshing algo in order to enable possibility
//! of addition of free vertices into the mesh.
template <class RangeSplitter, class BaseAlgo>
class BRepMesh_NodeInsertionMeshAlgo : public BaseAlgo
{
public:
  //! Constructor.
  BRepMesh_NodeInsertionMeshAlgo() {}

  //! Destructor.
  virtual ~BRepMesh_NodeInsertionMeshAlgo() {}

  //! Performs processing of the given face.
  virtual void Perform(const IMeshData::IFaceHandle& theDFace,
                       const IMeshTools_Parameters&  theParameters,
                       const Message_ProgressRange&  theRange) override
  {
    myRangeSplitter.Reset(theDFace, theParameters);
    myClassifier = new BRepMesh_Classifier;
    if (!theRange.More())
    {
      return;
    }
    BaseAlgo::Perform(theDFace, theParameters, theRange);
    myClassifier.Nullify();
  }

protected:
  typedef NCollection_Shared<NCollection_Sequence<const gp_Pnt2d*>> SequenceOfPnt2d;

  //! Performs initialization of data structure using existing model data.
  virtual bool initDataStructure() override
  {
    occ::handle<NCollection_IncAllocator> aTmpAlloc = new NCollection_IncAllocator;

    const IMeshData::IFaceHandle&                    aDFace = this->getDFace();
    NCollection_Array1<occ::handle<SequenceOfPnt2d>> aWires(0, aDFace->WiresNb() - 1);
    for (int aWireIt = 0; aWireIt < aDFace->WiresNb(); ++aWireIt)
    {
      const IMeshData::IWireHandle& aDWire = aDFace->GetWire(aWireIt);
      if (aDWire->IsSet(IMeshData_SelfIntersectingWire)
          || (aDWire->IsSet(IMeshData_OpenWire) && aWireIt != 0))
      {
        continue;
      }

      aWires(aWireIt) = collectWirePoints(aDWire, aTmpAlloc);
    }

    myRangeSplitter.AdjustRange();
    if (!myRangeSplitter.IsValid())
    {
      aDFace->SetStatus(IMeshData_Failure);
      return false;
    }

    const std::pair<double, double>& aDelta    = myRangeSplitter.GetDelta();
    const std::pair<double, double>& aTolUV    = myRangeSplitter.GetToleranceUV();
    const double                     uCellSize = 14.0 * aTolUV.first;
    const double                     vCellSize = 14.0 * aTolUV.second;

    this->getStructure()->Data()->SetCellSize(uCellSize / aDelta.first, vCellSize / aDelta.second);
    this->getStructure()->Data()->SetTolerance(aTolUV.first / aDelta.first,
                                               aTolUV.second / aDelta.second);

    for (int aWireIt = 0; aWireIt < aDFace->WiresNb(); ++aWireIt)
    {
      const occ::handle<SequenceOfPnt2d>& aWire = aWires(aWireIt);
      if (!aWire.IsNull() && !aWire->IsEmpty())
      {
        myClassifier->RegisterWire(*aWire,
                                   aTolUV,
                                   myRangeSplitter.GetRangeU(),
                                   myRangeSplitter.GetRangeV());
      }
    }

    if (this->getParameters().InternalVerticesMode)
    {
      insertInternalVertices();
    }

    return BaseAlgo::initDataStructure();
  }

  //! Adds the given 2d point to mesh data structure.
  //! Returns index of node in the structure.
  virtual int addNodeToStructure(const gp_Pnt2d&                thePoint,
                                 const int                      theLocation3d,
                                 const BRepMesh_DegreeOfFreedom theMovability,
                                 const bool                     isForceAdd) override
  {
    return BaseAlgo::addNodeToStructure(myRangeSplitter.Scale(thePoint, true),
                                        theLocation3d,
                                        theMovability,
                                        isForceAdd);
  }

  //! Returns 2d point associated to the given vertex.
  virtual gp_Pnt2d getNodePoint2d(const BRepMesh_Vertex& theVertex) const override
  {
    return myRangeSplitter.Scale(theVertex.Coord(), false);
  }

  //! Returns range splitter.
  const RangeSplitter& getRangeSplitter() const { return myRangeSplitter; }

  //! Returns classifier.
  const occ::handle<BRepMesh_Classifier>& getClassifier() const { return myClassifier; }

private:
  //! Creates collection of points representing discrete wire.
  occ::handle<SequenceOfPnt2d> collectWirePoints(
    const IMeshData::IWireHandle&                theDWire,
    const occ::handle<NCollection_IncAllocator>& theAllocator)
  {
    occ::handle<SequenceOfPnt2d> aWirePoints = new SequenceOfPnt2d(theAllocator);
    for (int aEdgeIt = 0; aEdgeIt < theDWire->EdgesNb(); ++aEdgeIt)
    {
      const IMeshData::IEdgeHandle    aDEdge = theDWire->GetEdge(aEdgeIt);
      const IMeshData::IPCurveHandle& aPCurve =
        aDEdge->GetPCurve(this->getDFace().get(), theDWire->GetEdgeOrientation(aEdgeIt));

      int aPointIt, aEndIndex, aInc;
      if (aPCurve->IsForward())
      {
        // For an infinite cylinder (for example)
        // aPCurve->ParametersNb() == 0

        aEndIndex = aPCurve->ParametersNb() - 1;
        aPointIt  = (std::min)(0, aEndIndex);
        aInc      = 1;
      }
      else
      {
        // For an infinite cylinder (for example)
        // aPCurve->ParametersNb() == 0

        aPointIt  = aPCurve->ParametersNb() - 1;
        aEndIndex = (std::min)(0, aPointIt);
        aInc      = -1;
      }

      // For an infinite cylinder (for example)
      // this cycle will not be executed.
      for (; aPointIt != aEndIndex; aPointIt += aInc)
      {
        const gp_Pnt2d& aPnt2d = aPCurve->GetPoint(aPointIt);
        aWirePoints->Append(&aPnt2d);
        myRangeSplitter.AddPoint(aPnt2d);
      }
    }

    return aWirePoints;
  }

  //! Iterates over internal vertices of a face and
  //! creates corresponding nodes in data structure.
  void insertInternalVertices()
  {
    TopExp_Explorer aExplorer(this->getDFace()->GetFace(), TopAbs_VERTEX, TopAbs_EDGE);
    for (; aExplorer.More(); aExplorer.Next())
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(aExplorer.Current());
      if (aVertex.Orientation() != TopAbs_INTERNAL)
      {
        continue;
      }

      insertInternalVertex(aVertex);
    }
  }

  //! Inserts the given vertex into mesh.
  void insertInternalVertex(const TopoDS_Vertex& theVertex)
  {
    try
    {
      OCC_CATCH_SIGNALS

      gp_Pnt2d aPnt2d = BRep_Tool::Parameters(theVertex, this->getDFace()->GetFace());
      // check UV values for internal vertices
      if (myClassifier->Perform(aPnt2d) != TopAbs_IN)
        return;

      this->registerNode(BRep_Tool::Pnt(theVertex), aPnt2d, BRepMesh_Fixed, false);
    }
    catch (Standard_Failure const&)
    {
    }
  }

private:
  RangeSplitter                    myRangeSplitter;
  occ::handle<BRepMesh_Classifier> myClassifier;
};

#endif
