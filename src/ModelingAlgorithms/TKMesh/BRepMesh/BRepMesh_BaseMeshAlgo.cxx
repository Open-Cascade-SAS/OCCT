// Created on: 2016-04-19
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

#include <BRepMesh_BaseMeshAlgo.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_PCurve.hxx>
#include <IMeshData_Curve.hxx>
#include <BRepMesh_Delaun.hxx>
#include <BRepMesh_ShapeTool.hxx>
#include <Standard_ErrorHandler.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_BaseMeshAlgo, IMeshTools_MeshAlgo)

//=================================================================================================

BRepMesh_BaseMeshAlgo::BRepMesh_BaseMeshAlgo() = default;

//=================================================================================================

BRepMesh_BaseMeshAlgo::~BRepMesh_BaseMeshAlgo() = default;

//=================================================================================================

void BRepMesh_BaseMeshAlgo::Perform(const IMeshData::IFaceHandle& theDFace,
                                    const IMeshTools_Parameters&  theParameters,
                                    const Message_ProgressRange&  theRange)
{
  try
  {
    OCC_CATCH_SIGNALS

    myDFace      = theDFace;
    myParameters = theParameters;
    myAllocator  = new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);
    myStructure  = new BRepMesh_DataStructureOfDelaun(myAllocator);
    myNodesMap   = new VectorOfPnt(256, myAllocator);
    myUsedNodes  = new DMapOfIntegerInteger(1, myAllocator);

    if (initDataStructure())
    {
      if (!theRange.More())
      {
        return;
      }
      generateMesh(theRange);
      commitSurfaceTriangulation();
    }
  }
  catch (Standard_Failure const& /*theException*/)
  {
  }

  myDFace.Nullify(); // Do not hold link to face.
  myStructure.Nullify();
  myNodesMap.Nullify();
  myUsedNodes.Nullify();
  myAllocator.Nullify();
}

//=================================================================================================

bool BRepMesh_BaseMeshAlgo::initDataStructure()
{
  for (int aWireIt = 0; aWireIt < myDFace->WiresNb(); ++aWireIt)
  {
    const IMeshData::IWireHandle& aDWire = myDFace->GetWire(aWireIt);
    if (aDWire->IsSet(IMeshData_SelfIntersectingWire))
    {
      // TODO: here we can add points of self-intersecting wire as fixed points
      // in order to keep consistency of nodes with adjacent faces.
      continue;
    }

    for (int aEdgeIt = 0; aEdgeIt < aDWire->EdgesNb(); ++aEdgeIt)
    {
      const IMeshData::IEdgeHandle    aDEdge         = aDWire->GetEdge(aEdgeIt);
      const IMeshData::ICurveHandle&  aCurve         = aDEdge->GetCurve();
      const IMeshData::ListOfInteger& aListOfPCurves = aDEdge->GetPCurves(myDFace.get());

      for (IMeshData::ListOfInteger::Iterator aPCurveIt(aListOfPCurves); aPCurveIt.More();
           aPCurveIt.Next())
      {
        const IMeshData::IPCurveHandle& aPCurve = aDEdge->GetPCurve(aPCurveIt.Value());
        const TopAbs_Orientation        aOri    = fixSeamEdgeOrientation(aDEdge, aPCurve);

        int       aPrevNodeIndex = -1;
        const int aLastPoint     = aPCurve->ParametersNb() - 1;
        for (int aPointIndex = 0; aPointIndex <= aLastPoint; ++aPointIndex)
        {
          const int aNodeIndex = registerNode(aCurve->GetPoint(aPointIndex),
                                              aPCurve->GetPoint(aPointIndex),
                                              BRepMesh_Frontier,
                                              false);

          aPCurve->GetIndex(aPointIndex) = aNodeIndex;

          if (aPrevNodeIndex != -1 && aPrevNodeIndex != aNodeIndex)
          {
            const int aLinksNb   = myStructure->NbLinks();
            const int aLinkIndex = addLinkToMesh(aPrevNodeIndex, aNodeIndex, aOri);
            if (aWireIt != 0 && aLinkIndex <= aLinksNb)
            {
              // Prevent holes around wire of zero area.
              BRepMesh_Edge& aLink = const_cast<BRepMesh_Edge&>(myStructure->GetLink(aLinkIndex));
              aLink.SetMovability(BRepMesh_Fixed);
            }
          }

          aPrevNodeIndex = aNodeIndex;
        }
      }
    }
  }

  return true;
}

//=================================================================================================

int BRepMesh_BaseMeshAlgo::registerNode(const gp_Pnt&                  thePoint,
                                        const gp_Pnt2d&                thePoint2d,
                                        const BRepMesh_DegreeOfFreedom theMovability,
                                        const bool                     isForceAdd)
{
  const int aNodeIndex =
    addNodeToStructure(thePoint2d, myNodesMap->Size(), theMovability, isForceAdd);

  if (aNodeIndex > myNodesMap->Size())
  {
    myNodesMap->Append(thePoint);
    myUsedNodes->Bind(aNodeIndex, aNodeIndex);
  }

  return aNodeIndex;
}

//=================================================================================================

int BRepMesh_BaseMeshAlgo::addNodeToStructure(const gp_Pnt2d&                thePoint,
                                              const int                      theLocation3d,
                                              const BRepMesh_DegreeOfFreedom theMovability,
                                              const bool                     isForceAdd)
{
  BRepMesh_Vertex aNode(thePoint.XY(), theLocation3d, theMovability);
  return myStructure->AddNode(aNode, isForceAdd);
}

//=================================================================================================

int BRepMesh_BaseMeshAlgo::addLinkToMesh(const int                theFirstNodeId,
                                         const int                theLastNodeId,
                                         const TopAbs_Orientation theOrientation)
{
  int aLinkIndex;
  if (theOrientation == TopAbs_REVERSED)
    aLinkIndex =
      myStructure->AddLink(BRepMesh_Edge(theLastNodeId, theFirstNodeId, BRepMesh_Frontier));
  else if (theOrientation == TopAbs_INTERNAL)
    aLinkIndex = myStructure->AddLink(BRepMesh_Edge(theFirstNodeId, theLastNodeId, BRepMesh_Fixed));
  else
    aLinkIndex =
      myStructure->AddLink(BRepMesh_Edge(theFirstNodeId, theLastNodeId, BRepMesh_Frontier));

  return std::abs(aLinkIndex);
}

//=================================================================================================

TopAbs_Orientation BRepMesh_BaseMeshAlgo::fixSeamEdgeOrientation(
  const IMeshData::IEdgeHandle&   theDEdge,
  const IMeshData::IPCurveHandle& thePCurve) const
{
  for (int aPCurveIt = 0; aPCurveIt < theDEdge->PCurvesNb(); ++aPCurveIt)
  {
    const IMeshData::IPCurveHandle& aPCurve = theDEdge->GetPCurve(aPCurveIt);
    if (aPCurve->GetFace() == myDFace && thePCurve != aPCurve)
    {
      // Simple check that another pcurve of seam edge does not coincide with reference one.
      const gp_Pnt2d& aPnt1_1 = thePCurve->GetPoint(0);
      const gp_Pnt2d& aPnt2_1 = thePCurve->GetPoint(thePCurve->ParametersNb() - 1);

      const gp_Pnt2d& aPnt1_2 = aPCurve->GetPoint(0);
      const gp_Pnt2d& aPnt2_2 = aPCurve->GetPoint(aPCurve->ParametersNb() - 1);

      const double aSqDist1 =
        std::min(aPnt1_1.SquareDistance(aPnt1_2), aPnt1_1.SquareDistance(aPnt2_2));
      const double aSqDist2 =
        std::min(aPnt2_1.SquareDistance(aPnt1_2), aPnt2_1.SquareDistance(aPnt2_2));
      if (aSqDist1 < Precision::SquareConfusion() && aSqDist2 < Precision::SquareConfusion())
      {
        return TopAbs_INTERNAL;
      }
    }
  }

  return thePCurve->GetOrientation();
}

//=================================================================================================

void BRepMesh_BaseMeshAlgo::commitSurfaceTriangulation()
{
  occ::handle<Poly_Triangulation> aTriangulation = collectTriangles();
  if (aTriangulation.IsNull())
  {
    myDFace->SetStatus(IMeshData_Failure);
    return;
  }

  collectNodes(aTriangulation);

  BRepMesh_ShapeTool::AddInFace(myDFace->GetFace(), aTriangulation);
}

//=================================================================================================

occ::handle<Poly_Triangulation> BRepMesh_BaseMeshAlgo::collectTriangles()
{
  const IMeshData::MapOfInteger& aTriangles = myStructure->ElementsOfDomain();
  if (aTriangles.IsEmpty())
  {
    return occ::handle<Poly_Triangulation>();
  }

  occ::handle<Poly_Triangulation> aRes = new Poly_Triangulation();
  aRes->ResizeTriangles(aTriangles.Extent(), false);
  IMeshData::IteratorOfMapOfInteger aTriIt(aTriangles);
  for (int aTriangeId = 1; aTriIt.More(); aTriIt.Next(), ++aTriangeId)
  {
    const BRepMesh_Triangle& aCurElem = myStructure->GetElement(aTriIt.Key());

    int aNode[3];
    myStructure->ElementNodes(aCurElem, aNode);

    for (int i = 0; i < 3; ++i)
    {
      if (!myUsedNodes->IsBound(aNode[i]))
      {
        myUsedNodes->Bind(aNode[i], myUsedNodes->Size() + 1);
      }

      aNode[i] = myUsedNodes->Find(aNode[i]);
    }

    aRes->SetTriangle(aTriangeId, Poly_Triangle(aNode[0], aNode[1], aNode[2]));
  }
  aRes->ResizeNodes(myUsedNodes->Extent(), false);
  aRes->AddUVNodes();
  return aRes;
}

//=================================================================================================

void BRepMesh_BaseMeshAlgo::collectNodes(const occ::handle<Poly_Triangulation>& theTriangulation)
{
  for (int i = 1; i <= myNodesMap->Size(); ++i)
  {
    if (myUsedNodes->IsBound(i))
    {
      const BRepMesh_Vertex& aVertex = myStructure->GetNode(i);

      const int aNodeIndex = myUsedNodes->Find(i);
      theTriangulation->SetNode(aNodeIndex, myNodesMap->Value(aVertex.Location3d()));
      theTriangulation->SetUVNode(aNodeIndex, getNodePoint2d(aVertex));
    }
  }
}

//=================================================================================================

gp_Pnt2d BRepMesh_BaseMeshAlgo::getNodePoint2d(const BRepMesh_Vertex& theVertex) const
{
  return theVertex.Coord();
}
