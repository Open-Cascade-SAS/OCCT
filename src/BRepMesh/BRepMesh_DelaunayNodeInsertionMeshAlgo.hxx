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
template<class RangeSplitter, class BaseAlgo>
class BRepMesh_DelaunayNodeInsertionMeshAlgo : public BRepMesh_NodeInsertionMeshAlgo<RangeSplitter, BaseAlgo>
{
private:
  // Typedef for OCCT RTTI
  typedef BRepMesh_NodeInsertionMeshAlgo<RangeSplitter, BaseAlgo> InsertionBaseClass;

public:

  //! Constructor.
  BRepMesh_DelaunayNodeInsertionMeshAlgo()
    : myIsPreProcessSurfaceNodes (Standard_False)
  {
  }

  //! Destructor.
  virtual ~BRepMesh_DelaunayNodeInsertionMeshAlgo()
  {
  }

  //! Returns PreProcessSurfaceNodes flag. 
  Standard_Boolean IsPreProcessSurfaceNodes () const
  {
    return myIsPreProcessSurfaceNodes;
  }

  //! Sets PreProcessSurfaceNodes flag.
  //! If TRUE, registers surface nodes before generation of base mesh.
  //! If FALSE, inserts surface nodes after generation of base mesh. 
  void SetPreProcessSurfaceNodes (const Standard_Boolean isPreProcessSurfaceNodes)
  {
    myIsPreProcessSurfaceNodes = isPreProcessSurfaceNodes;
  }

protected:

  //! Performs initialization of data structure using existing model data.
  virtual Standard_Boolean initDataStructure() Standard_OVERRIDE
  {
    if (!InsertionBaseClass::initDataStructure())
    {
      return Standard_False;
    }

    if (myIsPreProcessSurfaceNodes)
    {
      const Handle(IMeshData::ListOfPnt2d) aSurfaceNodes =
        this->getRangeSplitter().GenerateSurfaceNodes(this->getParameters());

      registerSurfaceNodes (aSurfaceNodes);
    }

    return Standard_True;
  }

  //! Returns size of cell to be used by acceleration circles grid structure.
  virtual std::pair<Standard_Integer, Standard_Integer> getCellsCount (const Standard_Integer theVerticesNb) Standard_OVERRIDE
  {
    return BRepMesh_GeomTool::CellsCount (this->getDFace()->GetSurface(), theVerticesNb,
                                          this->getDFace()->GetDeflection(),
                                          &this->getRangeSplitter());
  }

  //! Performs processing of generated mesh. Generates surface nodes and inserts them into structure.
  virtual void postProcessMesh (BRepMesh_Delaun& theMesher,
                                const Message_ProgressRange& theRange) Standard_OVERRIDE
  {
    if (!theRange.More())
    {
      return;
    }
    InsertionBaseClass::postProcessMesh (theMesher, Message_ProgressRange()); // shouldn't be range passed here?

    if (!myIsPreProcessSurfaceNodes)
    {
      const Handle(IMeshData::ListOfPnt2d) aSurfaceNodes =
        this->getRangeSplitter().GenerateSurfaceNodes(this->getParameters());

      insertNodes(aSurfaceNodes, theMesher, theRange);
    }
  }

  //! Inserts nodes into mesh.
  Standard_Boolean insertNodes(
    const Handle(IMeshData::ListOfPnt2d)& theNodes,
    BRepMesh_Delaun&                      theMesher,
    const Message_ProgressRange&          theRange)
  {
    if (theNodes.IsNull() || theNodes->IsEmpty())
    {
      return Standard_False;
    }

    IMeshData::VectorOfInteger aVertexIndexes(theNodes->Size(), this->getAllocator());
    IMeshData::ListOfPnt2d::Iterator aNodesIt(*theNodes);
    for (Standard_Integer aNodeIt = 1; aNodesIt.More(); aNodesIt.Next(), ++aNodeIt)
    {
      const gp_Pnt2d& aPnt2d = aNodesIt.Value();
      if (this->getClassifier()->Perform(aPnt2d) == TopAbs_IN)
      {
        aVertexIndexes.Append(this->registerNode(this->getRangeSplitter().Point(aPnt2d),
                                                 aPnt2d, BRepMesh_Free, Standard_False));
      }
    }

    theMesher.AddVertices (aVertexIndexes, theRange);
    if (!theRange.More())
    {
      return Standard_False;
    }
    return !aVertexIndexes.IsEmpty();
  }

private:
  
  //! Registers surface nodes in data structure.
  Standard_Boolean registerSurfaceNodes(
    const Handle(IMeshData::ListOfPnt2d)& theNodes)
  {
    if (theNodes.IsNull() || theNodes->IsEmpty())
    {
      return Standard_False;
    }

    Standard_Boolean isAdded = Standard_False;
    IMeshData::ListOfPnt2d::Iterator aNodesIt(*theNodes);
    for (Standard_Integer aNodeIt = 1; aNodesIt.More(); aNodesIt.Next(), ++aNodeIt)
    {
      const gp_Pnt2d& aPnt2d = aNodesIt.Value();
      if (this->getClassifier()->Perform(aPnt2d) == TopAbs_IN)
      {
        isAdded = Standard_True;
        this->registerNode(this->getRangeSplitter().Point(aPnt2d),
                           aPnt2d, BRepMesh_Free, Standard_False);
      }
    }

    return isAdded;
  }

private:

  Standard_Boolean myIsPreProcessSurfaceNodes;
};

#endif
