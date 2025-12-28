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

#ifndef _BRepMesh_DelaunayNodeInsertionMeshAlgo_HeaderFile
#define _BRepMesh_DelaunayNodeInsertionMeshAlgo_HeaderFile

#include <BRepMesh_NodeInsertionMeshAlgo.hxx>
#include <BRepMesh_GeomTool.hxx>

//! Extends base Delaunay meshing algo in order to enable possibility
//! of addition of free vertices and internal nodes into the mesh.
template <class RangeSplitter, class BaseAlgo>
class BRepMesh_DelaunayNodeInsertionMeshAlgo
    : public BRepMesh_NodeInsertionMeshAlgo<RangeSplitter, BaseAlgo>
{
private:
  // Typedef for OCCT RTTI
  typedef BRepMesh_NodeInsertionMeshAlgo<RangeSplitter, BaseAlgo> InsertionBaseClass;

public:
  //! Constructor.
  BRepMesh_DelaunayNodeInsertionMeshAlgo()
      : myIsPreProcessSurfaceNodes(false)
  {
  }

  //! Destructor.
  virtual ~BRepMesh_DelaunayNodeInsertionMeshAlgo() {}

  //! Returns PreProcessSurfaceNodes flag.
  bool IsPreProcessSurfaceNodes() const { return myIsPreProcessSurfaceNodes; }

  //! Sets PreProcessSurfaceNodes flag.
  //! If TRUE, registers surface nodes before generation of base mesh.
  //! If FALSE, inserts surface nodes after generation of base mesh.
  void SetPreProcessSurfaceNodes(const bool isPreProcessSurfaceNodes)
  {
    myIsPreProcessSurfaceNodes = isPreProcessSurfaceNodes;
  }

protected:
  //! Performs initialization of data structure using existing model data.
  virtual bool initDataStructure() override
  {
    if (!InsertionBaseClass::initDataStructure())
    {
      return false;
    }

    if (myIsPreProcessSurfaceNodes)
    {
      const Handle(IMeshData::ListOfPnt2d) aSurfaceNodes =
        this->getRangeSplitter().GenerateSurfaceNodes(this->getParameters());

      registerSurfaceNodes(aSurfaceNodes);
    }

    return true;
  }

  //! Returns size of cell to be used by acceleration circles grid structure.
  virtual std::pair<int, int> getCellsCount(const int theVerticesNb) override
  {
    return BRepMesh_GeomTool::CellsCount(this->getDFace()->GetSurface(),
                                         theVerticesNb,
                                         this->getDFace()->GetDeflection(),
                                         &this->getRangeSplitter());
  }

  //! Performs processing of generated mesh. Generates surface nodes and inserts them into
  //! structure.
  virtual void postProcessMesh(BRepMesh_Delaun&             theMesher,
                               const Message_ProgressRange& theRange) override
  {
    if (!theRange.More())
    {
      return;
    }
    // clang-format off
    InsertionBaseClass::postProcessMesh (theMesher, Message_ProgressRange()); // shouldn't be range passed here?
    // clang-format on

    if (!myIsPreProcessSurfaceNodes)
    {
      const Handle(IMeshData::ListOfPnt2d) aSurfaceNodes =
        this->getRangeSplitter().GenerateSurfaceNodes(this->getParameters());

      insertNodes(aSurfaceNodes, theMesher, theRange);
    }
  }

  //! Inserts nodes into mesh.
  bool insertNodes(const Handle(IMeshData::ListOfPnt2d)& theNodes,
                   BRepMesh_Delaun&                      theMesher,
                   const Message_ProgressRange&          theRange)
  {
    if (theNodes.IsNull() || theNodes->IsEmpty())
    {
      return false;
    }

    IMeshData::VectorOfInteger       aVertexIndexes(theNodes->Size(), this->getAllocator());
    IMeshData::ListOfPnt2d::Iterator aNodesIt(*theNodes);
    for (int aNodeIt = 1; aNodesIt.More(); aNodesIt.Next(), ++aNodeIt)
    {
      const gp_Pnt2d& aPnt2d = aNodesIt.Value();
      if (this->getClassifier()->Perform(aPnt2d) == TopAbs_IN)
      {
        aVertexIndexes.Append(
          this->registerNode(this->getRangeSplitter().Point(aPnt2d), aPnt2d, BRepMesh_Free, false));
      }
    }

    theMesher.AddVertices(aVertexIndexes, theRange);
    if (!theRange.More())
    {
      return false;
    }
    return !aVertexIndexes.IsEmpty();
  }

private:
  //! Registers surface nodes in data structure.
  bool registerSurfaceNodes(const Handle(IMeshData::ListOfPnt2d)& theNodes)
  {
    if (theNodes.IsNull() || theNodes->IsEmpty())
    {
      return false;
    }

    bool                             isAdded = false;
    IMeshData::ListOfPnt2d::Iterator aNodesIt(*theNodes);
    for (int aNodeIt = 1; aNodesIt.More(); aNodesIt.Next(), ++aNodeIt)
    {
      const gp_Pnt2d& aPnt2d = aNodesIt.Value();
      if (this->getClassifier()->Perform(aPnt2d) == TopAbs_IN)
      {
        isAdded = true;
        this->registerNode(this->getRangeSplitter().Point(aPnt2d), aPnt2d, BRepMesh_Free, false);
      }
    }

    return isAdded;
  }

private:
  bool myIsPreProcessSurfaceNodes;
};

#endif
