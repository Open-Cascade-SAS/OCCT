// Created: 2017-12-28
//
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <BRepMesh_Triangulator.hxx>
#include <Precision.hxx>
#include <ProjLib.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <BRepMesh_Delaun.hxx>

#ifdef CHECK_PERF
  #include <MoniTool_Timer.hxx>
  #include <MoniTool_TimerSentry.hxx>
#endif

namespace
{
//=================================================================================================

inline int wireNodesNb(const NCollection_List<NCollection_Sequence<int>>& theWires)
{
  int                                      nbNodes = 0;
  NCollection_List<NCollection_Sequence<int>>::Iterator itW(theWires);
  for (int i = 1; itW.More(); itW.Next(), i++)
  {
    nbNodes += itW.Value().Length();
  }

  return nbNodes;
}

//=================================================================================================

inline void appendTriangle(const int                        theNode1,
                           const int                        theNode2,
                           const int                        theNode3,
                           const NCollection_Sequence<int>& theW,
                           NCollection_List<Poly_Triangle>& thePolyTriangles)
{
  const Poly_Triangle aT1(theW(theNode1) + 1, theW(theNode2) + 1, theW(theNode3) + 1);
  thePolyTriangles.Append(aT1);
}
} // namespace

//=================================================================================================

occ::handle<Poly_Triangulation> BRepMesh_Triangulator::ToPolyTriangulation(
  const NCollection_Array1<gp_Pnt>&              theNodes,
  const NCollection_List<Poly_Triangle>& thePolyTriangles)
{
  NCollection_Array1<Poly_Triangle>                     aTriangles(1, thePolyTriangles.Extent());
  NCollection_List<Poly_Triangle>::Iterator itT(thePolyTriangles);
  for (int i = 1; itT.More(); itT.Next(), i++)
  {
    aTriangles.SetValue(i, itT.Value());
  }

  return new Poly_Triangulation(theNodes, aTriangles);
}

//=================================================================================================

BRepMesh_Triangulator::BRepMesh_Triangulator(
  const NCollection_Vector<gp_XYZ>&                  theXYZs,
  const NCollection_List<NCollection_Sequence<int>>& theWires,
  const gp_Dir&                                      theNorm)
    : myXYZs(theXYZs),
      myWires(theWires),
      myPlane(gp::Origin(), theNorm)
{
}

//=================================================================================================

bool BRepMesh_Triangulator::Perform(NCollection_List<Poly_Triangle>& thePolyTriangles)
{
#ifdef CHECK_PERF
  MoniTool_TimerSentry MTS01("BRepMesh_Triangulator::Perform");
#endif

  if (myMess.IsNull())
  {
    myMess = Message::DefaultMessenger();
  }

  if (myWires.Extent() == 1)
  {
    const NCollection_Sequence<int>& aTmpWire = myWires.First();
    if (aTmpWire.Length() < 5)
    {
      // prepare triangles from simple wire (3 or 4 points)
      addTriange34(aTmpWire, thePolyTriangles);
      return true;
    }
  }

  if (!prepareMeshStructure())
  {
    return false;
  }

  if (!triangulate(thePolyTriangles))
  {
    return false;
  }

#ifdef CHECK_PERF
  MTS01.Stop();
#endif

  return true;
}

//=======================================================================
// function : addTriange34
// purpose  : auxiliary for makeTrianglesUsingBRepMesh
//=======================================================================
void BRepMesh_Triangulator::addTriange34(const NCollection_Sequence<int>& theW,
                                         NCollection_List<Poly_Triangle>& thePolyTriangles)
{
  // prepare triangles from simple wire (3 or 4 points)
  if (theW.Length() == 3)
  {
    Poly_Triangle aT(theW(1) + 1, theW(2) + 1, theW(3) + 1);
    thePolyTriangles.Append(aT);
  }
  else if (theW.Length() == 4)
  {
    double    d13   = (myXYZs.Value(theW(1)) - myXYZs.Value(theW(3))).SquareModulus();
    double    d24   = (myXYZs.Value(theW(2)) - myXYZs.Value(theW(4))).SquareModulus();
    bool use13 = true;
    if (d24 < d13)
    {
      // additional check for inner corner
      static int aPivotNodes[4] = {2, 4, 1, 3};
      use13                     = checkCondition(aPivotNodes, theW);
    }
    else
    {
      static int aPivotNodes[4] = {1, 3, 2, 4};
      use13                     = !checkCondition(aPivotNodes, theW);
    }

    if (use13)
    {
      appendTriangle(1, 2, 3, theW, thePolyTriangles);
      appendTriangle(3, 4, 1, theW, thePolyTriangles);
    }
    else
    {
      appendTriangle(1, 2, 4, theW, thePolyTriangles);
      appendTriangle(2, 3, 4, theW, thePolyTriangles);
    }
  }
}

//=======================================================================
// function : checkCondition
// purpose  : auxiliary for addTriange34
//=======================================================================
bool BRepMesh_Triangulator::checkCondition(const int (&theNodes)[4],
                                                       const NCollection_Sequence<int>& theW)
{
  const gp_XYZ aV0 = myXYZs.Value(theW(theNodes[1])) - myXYZs.Value(theW(theNodes[0]));
  const gp_XYZ aV1 = myXYZs.Value(theW(theNodes[2])) - myXYZs.Value(theW(theNodes[0]));
  const gp_XYZ aV2 = myXYZs.Value(theW(theNodes[3])) - myXYZs.Value(theW(theNodes[0]));

  const gp_XYZ aCross1 = aV0.Crossed(aV1);
  const gp_XYZ aCross2 = aV0.Crossed(aV2);
  return (aCross1.SquareModulus() < Precision::SquareConfusion()
          || aCross2.SquareModulus() < Precision::SquareConfusion()
          || gp_Dir(aCross1).IsEqual(gp_Dir(aCross2), 0.01));
}

//=================================================================================================

bool BRepMesh_Triangulator::prepareMeshStructure()
{
  myIndices       = new IMeshData::VectorOfInteger(wireNodesNb(myWires));
  myMeshStructure = new BRepMesh_DataStructureOfDelaun(new NCollection_IncAllocator);

  // fill this structure created BRepMesh_Vertexes using 2d points received
  // by projection initial 3d point on plane.
  try
  {
    int                                      aNumNode = 0;
    NCollection_List<NCollection_Sequence<int>>::Iterator itW(myWires);
    for (int i = 1; itW.More(); itW.Next(), i++)
    {
      const NCollection_Sequence<int>& aW = itW.Value();
      for (int nn = 1; nn <= aW.Length(); ++nn, ++aNumNode)
      {
        const gp_Pnt2d         aP2d = ProjLib::Project(myPlane, gp_Pnt(myXYZs(aW(nn))));
        const BRepMesh_Vertex  aVertex(aP2d.XY(), aNumNode, BRepMesh_Frontier);
        const int nnn = myMeshStructure->AddNode(aVertex);
        myIndices->SetValue(aNumNode, nnn);
        myTmpMap.Bind(aNumNode + 1, aW(nn) + 1);
      }

      const int aOffset = aNumNode - aW.Length();
      for (int nn = 1; nn <= aW.Length(); ++nn)
      {
        const BRepMesh_Edge anEdge(myIndices->Value(aOffset + (nn - 1)),
                                   myIndices->Value(aOffset + (nn % aW.Length())),
                                   BRepMesh_Frontier);
        myMeshStructure->AddLink(anEdge);
      }
    }
  }
  catch (...)
  {
    myMess->Send("makeTrianglesUsingBRepMesh: Exception raised during filling of "
                 "BRepMesh_DataStructureOfDelaun",
                 Message_Fail);
    return false;
  }

  return true;
}

//=================================================================================================

bool BRepMesh_Triangulator::triangulate(
  NCollection_List<Poly_Triangle>& thePolyTriangles)
{
  try
  {
    // perform triangulation used created aMeshStructure
    BRepMesh_Delaun                aTriangulation(myMeshStructure, *myIndices);
    const IMeshData::MapOfInteger& aTriangles = myMeshStructure->ElementsOfDomain();
    if (aTriangles.Extent() < 1)
    {
      return false;
    }

    // prepare Poly_Triangles from result triangles and add to returned list
    for (IMeshData::IteratorOfMapOfInteger aTriIter(aTriangles); aTriIter.More(); aTriIter.Next())
    {
      const int   aTriangleId = aTriIter.Key();
      const BRepMesh_Triangle& aTriangle   = myMeshStructure->GetElement(aTriangleId);
      if (aTriangle.Movability() == BRepMesh_Deleted)
      {
        continue;
      }
      int aTri2d[3];
      myMeshStructure->ElementNodes(aTriangle, aTri2d);

      const Poly_Triangle aT(myTmpMap.Find(aTri2d[0]),
                             myTmpMap.Find(aTri2d[1]),
                             myTmpMap.Find(aTri2d[2]));
      thePolyTriangles.Append(aT);
    }
  }
  catch (Standard_Failure const& aFailure)
  {
    TCollection_AsciiString aStr(
      "makeTrianglesUsingBRepMesh: Exception raised during polygon triangulation: ");
    aStr.AssignCat(aFailure.GetMessageString());
    myMess->Send(aStr.ToCString(), Message_Fail);
    return false;
  }

  return true;
}
