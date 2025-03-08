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

BRepMesh_BaseMeshAlgo::BRepMesh_BaseMeshAlgo() {}

//=================================================================================================

BRepMesh_BaseMeshAlgo::~BRepMesh_BaseMeshAlgo() {}

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

Standard_Boolean BRepMesh_BaseMeshAlgo::initDataStructure()
{
  for (Standard_Integer aWireIt = 0; aWireIt < myDFace->WiresNb(); ++aWireIt)
  {
    const IMeshData::IWireHandle& aDWire = myDFace->GetWire(aWireIt);
    if (aDWire->IsSet(IMeshData_SelfIntersectingWire))
    {
      // TODO: here we can add points of self-intersecting wire as fixed points
      // in order to keep consistency of nodes with adjacent faces.
      continue;
    }

    for (Standard_Integer aEdgeIt = 0; aEdgeIt < aDWire->EdgesNb(); ++aEdgeIt)
    {
      const IMeshData::IEdgeHandle    aDEdge = aDWire->GetEdge(aEdgeIt);
      const IMeshData::ICurveHandle&  aCurve = aDEdge->GetCurve();
      const IMeshData::IPCurveHandle& aPCurve =
        aDEdge->GetPCurve(myDFace.get(), aDWire->GetEdgeOrientation(aEdgeIt));

      const TopAbs_Orientation aOri = fixSeamEdgeOrientation(aDEdge, aPCurve);

      Standard_Integer       aPrevNodeIndex = -1;
      const Standard_Integer aLastPoint     = aPCurve->ParametersNb() - 1;
      for (Standard_Integer aPointIt = 0; aPointIt <= aLastPoint; ++aPointIt)
      {
        const Standard_Integer aNodeIndex =
          registerNode(aCurve->GetPoint(aPointIt),
                       aPCurve->GetPoint(aPointIt),
                       BRepMesh_Frontier,
                       Standard_False /*aPointIt > 0 && aPointIt < aLastPoint*/);

        aPCurve->GetIndex(aPointIt) = aNodeIndex;
        myUsedNodes->Bind(aNodeIndex, aNodeIndex);

        if (aPrevNodeIndex != -1 && aPrevNodeIndex != aNodeIndex)
        {
          const Standard_Integer aLinksNb   = myStructure->NbLinks();
          const Standard_Integer aLinkIndex = addLinkToMesh(aPrevNodeIndex, aNodeIndex, aOri);
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

  return Standard_True;
}

//=================================================================================================

Standard_Integer BRepMesh_BaseMeshAlgo::registerNode(const gp_Pnt&                  thePoint,
                                                     const gp_Pnt2d&                thePoint2d,
                                                     const BRepMesh_DegreeOfFreedom theMovability,
                                                     const Standard_Boolean         isForceAdd)
{
  const Standard_Integer aNodeIndex =
    addNodeToStructure(thePoint2d, myNodesMap->Size(), theMovability, isForceAdd);

  if (aNodeIndex > myNodesMap->Size())
  {
    myNodesMap->Append(thePoint);
  }

  return aNodeIndex;
}

//=================================================================================================

Standard_Integer BRepMesh_BaseMeshAlgo::addNodeToStructure(
  const gp_Pnt2d&                thePoint,
  const Standard_Integer         theLocation3d,
  const BRepMesh_DegreeOfFreedom theMovability,
  const Standard_Boolean         isForceAdd)
{
  BRepMesh_Vertex aNode(thePoint.XY(), theLocation3d, theMovability);
  return myStructure->AddNode(aNode, isForceAdd);
}

//=================================================================================================

Standard_Integer BRepMesh_BaseMeshAlgo::addLinkToMesh(const Standard_Integer   theFirstNodeId,
                                                      const Standard_Integer   theLastNodeId,
                                                      const TopAbs_Orientation theOrientation)
{
  Standard_Integer aLinkIndex;
  if (theOrientation == TopAbs_REVERSED)
    aLinkIndex =
      myStructure->AddLink(BRepMesh_Edge(theLastNodeId, theFirstNodeId, BRepMesh_Frontier));
  else if (theOrientation == TopAbs_INTERNAL)
    aLinkIndex = myStructure->AddLink(BRepMesh_Edge(theFirstNodeId, theLastNodeId, BRepMesh_Fixed));
  else
    aLinkIndex =
      myStructure->AddLink(BRepMesh_Edge(theFirstNodeId, theLastNodeId, BRepMesh_Frontier));

  return Abs(aLinkIndex);
}

//=================================================================================================

TopAbs_Orientation BRepMesh_BaseMeshAlgo::fixSeamEdgeOrientation(
  const IMeshData::IEdgeHandle&   theDEdge,
  const IMeshData::IPCurveHandle& thePCurve) const
{
  for (Standard_Integer aPCurveIt = 0; aPCurveIt < theDEdge->PCurvesNb(); ++aPCurveIt)
  {
    const IMeshData::IPCurveHandle& aPCurve = theDEdge->GetPCurve(aPCurveIt);
    if (aPCurve->GetFace() == myDFace && thePCurve != aPCurve)
    {
      // Simple check that another pcurve of seam edge does not coincide with reference one.
      const gp_Pnt2d& aPnt1_1 = thePCurve->GetPoint(0);
      const gp_Pnt2d& aPnt2_1 = thePCurve->GetPoint(thePCurve->ParametersNb() - 1);

      const gp_Pnt2d& aPnt1_2 = aPCurve->GetPoint(0);
      const gp_Pnt2d& aPnt2_2 = aPCurve->GetPoint(aPCurve->ParametersNb() - 1);

      const Standard_Real aSqDist1 =
        Min(aPnt1_1.SquareDistance(aPnt1_2), aPnt1_1.SquareDistance(aPnt2_2));
      const Standard_Real aSqDist2 =
        Min(aPnt2_1.SquareDistance(aPnt1_2), aPnt2_1.SquareDistance(aPnt2_2));
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
  Handle(Poly_Triangulation) aTriangulation = collectTriangles();
  if (aTriangulation.IsNull())
  {
    myDFace->SetStatus(IMeshData_Failure);
    return;
  }

  collectNodes(aTriangulation);

  BRepMesh_ShapeTool::AddInFace(myDFace->GetFace(), aTriangulation);
}

//=================================================================================================

Handle(Poly_Triangulation) BRepMesh_BaseMeshAlgo::collectTriangles()
{
  const IMeshData::MapOfInteger& aTriangles = myStructure->ElementsOfDomain();
  if (aTriangles.IsEmpty())
  {
    return Handle(Poly_Triangulation)();
  }

  Handle(Poly_Triangulation) aRes = new Poly_Triangulation();
  aRes->ResizeTriangles(aTriangles.Extent(), false);
  IMeshData::IteratorOfMapOfInteger aTriIt(aTriangles);
  for (Standard_Integer aTriangeId = 1; aTriIt.More(); aTriIt.Next(), ++aTriangeId)
  {
    const BRepMesh_Triangle& aCurElem = myStructure->GetElement(aTriIt.Key());

    Standard_Integer aNode[3];
    myStructure->ElementNodes(aCurElem, aNode);

    for (Standard_Integer i = 0; i < 3; ++i)
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

void BRepMesh_BaseMeshAlgo::collectNodes(const Handle(Poly_Triangulation)& theTriangulation)
{
  for (Standard_Integer i = 1; i <= myNodesMap->Size(); ++i)
  {
    if (myUsedNodes->IsBound(i))
    {
      const BRepMesh_Vertex& aVertex = myStructure->GetNode(i);

      const Standard_Integer aNodeIndex = myUsedNodes->Find(i);
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
