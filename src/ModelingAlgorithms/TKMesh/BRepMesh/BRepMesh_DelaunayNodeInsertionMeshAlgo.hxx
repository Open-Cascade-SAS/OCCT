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
#include <BSplCLib.hxx>
#include <Geom_BSplineSurface.hxx>
#include <NCollection_LinearVector.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <algorithm>
#include <tuple>

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
  ~BRepMesh_DelaunayNodeInsertionMeshAlgo() override = default;

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
  //! Surface point with its unscaled parameters.
  struct SurfacePoint
  {
    gp_Pnt2d Parameter; //!< Surface parameters.
    gp_Pnt   Point;     //!< Evaluated surface point.
  };

  //! Performs initialization of data structure using existing model data.
  bool initDataStructure() override
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
  std::pair<int, int> getCellsCount(const int theVerticesNb) override
  {
    return BRepMesh_GeomTool::CellsCount(this->getDFace()->GetSurface(),
                                         theVerticesNb,
                                         this->getDFace()->GetDeflection(),
                                         &this->getRangeSplitter());
  }

  //! Performs processing of generated mesh. Generates surface nodes and inserts them into
  //! structure.
  void postProcessMesh(BRepMesh_Delaun& theMesher, const Message_ProgressRange& theRange) override
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

    IMeshData::VectorOfInteger aVertexIndexes(theNodes->Size(), this->getAllocator());
    if (this->getDFace()->GetSurface()->GetType() == GeomAbs_BSplineSurface)
    {
      registerSplineNodes(theNodes, aVertexIndexes);
    }
    else
    {
      for (const gp_Pnt2d& aPnt2d : *theNodes)
      {
        if (this->getClassifier()->Perform(aPnt2d) == TopAbs_IN)
        {
          aVertexIndexes.Append(this->registerNode(this->getRangeSplitter().Point(aPnt2d),
                                                   aPnt2d,
                                                   BRepMesh_Free,
                                                   false));
        }
      }
    }

    return insertVertices(aVertexIndexes, theMesher, theRange);
  }

  //! Inserts already evaluated surface points in their original order.
  bool insertNodes(const NCollection_LinearVector<SurfacePoint>& theNodes,
                   BRepMesh_Delaun&                              theMesher,
                   const Message_ProgressRange&                  theRange)
  {
    if (theNodes.IsEmpty())
    {
      return false;
    }

    IMeshData::VectorOfInteger aVertexIndexes(theNodes.Size(), this->getAllocator());
    for (const SurfacePoint& aNode : theNodes)
    {
      if (this->getClassifier()->Perform(aNode.Parameter) == TopAbs_IN)
      {
        aVertexIndexes.Append(
          this->registerNode(aNode.Point, aNode.Parameter, BRepMesh_Free, false));
      }
    }
    return insertVertices(aVertexIndexes, theMesher, theRange);
  }

private:
  //! Adds registered vertices and reports whether the mesh grew.
  bool insertVertices(IMeshData::VectorOfInteger&  theVertexIndexes,
                      BRepMesh_Delaun&             theMesher,
                      const Message_ProgressRange& theRange)
  {
    const int aPreviousTriangles = this->getStructure()->ElementsOfDomain().Extent();
    theMesher.AddVertices(theVertexIndexes, theRange);
    if (!theRange.More())
    {
      return false;
    }
    return this->getStructure()->ElementsOfDomain().Extent() > aPreviousTriangles;
  }

  //! Evaluates each knot patch together, retaining the node insertion order.
  void registerSplineNodes(const Handle(IMeshData::ListOfPnt2d)& theNodes,
                           IMeshData::VectorOfInteger&           theVertexIndexes)
  {
    struct SurfaceNode
    {
      gp_Pnt2d Parameter; //!< Unscaled surface parameters.
      gp_Pnt   Point;     //!< Evaluated surface point.
      size_t   Index;     //!< Original insertion order.
      int      USpan;     //!< U knot span.
      int      VSpan;     //!< V knot span.
    };

    const occ::handle<Geom_BSplineSurface> aSpline =
      this->getDFace()->GetSurface()->AdaptorSurfaceOriginal().BSpline();
    NCollection_LinearVector<SurfaceNode> aNodes(theNodes->Size());
    for (const gp_Pnt2d& aParameter : *theNodes)
    {
      if (this->getClassifier()->Perform(aParameter) != TopAbs_IN)
      {
        continue;
      }
      SurfaceNode aNode{aParameter, gp_Pnt(), aNodes.Size(), 0, 0};
      double      aParameterInPeriod;
      BSplCLib::LocateParameter(aSpline->UDegree(),
                                aSpline->UKnotSequence(),
                                BSplCLib::NoMults(),
                                aParameter.X(),
                                aSpline->IsUPeriodic(),
                                aNode.USpan,
                                aParameterInPeriod);
      BSplCLib::LocateParameter(aSpline->VDegree(),
                                aSpline->VKnotSequence(),
                                BSplCLib::NoMults(),
                                aParameter.Y(),
                                aSpline->IsVPeriodic(),
                                aNode.VSpan,
                                aParameterInPeriod);
      aNodes.Append(aNode);
    }
    std::sort(aNodes.begin(),
              aNodes.end(),
              [](const SurfaceNode& theLeft, const SurfaceNode& theRight) {
                return std::tie(theLeft.USpan, theLeft.VSpan, theLeft.Index)
                       < std::tie(theRight.USpan, theRight.VSpan, theRight.Index);
              });
    for (SurfaceNode& aNode : aNodes)
    {
      aNode.Point = this->getRangeSplitter().Point(aNode.Parameter);
    }
    std::sort(aNodes.begin(),
              aNodes.end(),
              [](const SurfaceNode& theLeft, const SurfaceNode& theRight) {
                return theLeft.Index < theRight.Index;
              });
    for (const SurfaceNode& aNode : aNodes)
    {
      theVertexIndexes.Append(
        this->registerNode(aNode.Point, aNode.Parameter, BRepMesh_Free, false));
    }
  }

  //! Registers surface nodes in data structure.
  bool registerSurfaceNodes(const Handle(IMeshData::ListOfPnt2d)& theNodes)
  {
    if (theNodes.IsNull() || theNodes->IsEmpty())
    {
      return false;
    }

    bool isAdded = false;
    for (const gp_Pnt2d& aPnt2d : *theNodes)
    {
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
