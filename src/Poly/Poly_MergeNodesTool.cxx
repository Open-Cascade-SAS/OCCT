// Copyright (c) 2015-2021 OPEN CASCADE SAS
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

#include <Poly_MergeNodesTool.hxx>

#include <NCollection_IncAllocator.hxx>
#include <Standard_CStringHasher.hxx>

#include <algorithm>

namespace
{
//! Returns initial number of buckets for the map.
static int initialNbBuckets(int theNbFacets)
{
  return theNbFacets > 0
           ? theNbFacets * 2 // consider ratio 1:2 (NbTriangles:MergedNodes) as expected
           : 995329;         // default initial value for mesh of unknown size
}
} // namespace

IMPLEMENT_STANDARD_RTTIEXT(Poly_MergeNodesTool, Standard_Transient)

//! Map node.
class Poly_MergeNodesTool::MergedNodesMap::DataMapNode : public NCollection_TListNode<int>
{
public:
  //! Constructor.
  DataMapNode(const NCollection_Vec3<float>& thePos,
              const NCollection_Vec3<float>& theNorm,
              int                            theItem,
              NCollection_ListNode*          theNext)
      : NCollection_TListNode<int>(theItem, theNext),
        myKey(thePos, theNorm)
  {
  }

  //! Key.
  const Poly_MergeNodesTool::Vec3AndNormal& Key() const { return myKey; }

  //! Static deleter to be passed to BaseMap
  static void delNode(NCollection_ListNode* theNode, Handle(NCollection_BaseAllocator)& theAl)
  {
    ((DataMapNode*)theNode)->~DataMapNode();
    theAl->Free(theNode);
  }

private:
  Poly_MergeNodesTool::Vec3AndNormal myKey;
};

// =======================================================================
// function : MergedNodesMap
// purpose  :
// =======================================================================
Poly_MergeNodesTool::MergedNodesMap::MergedNodesMap(const int theNbBuckets)
    : NCollection_BaseMap(theNbBuckets, true, new NCollection_IncAllocator()),
      myTolerance(0.0f),
      myInvTol(0.0f),
      myAngle(1.0f),
      myAngleCos(0.0f),
      myToMergeOpposite(false)
{
  //
}

// =======================================================================
// function : MergedNodesMap::SetMergeTolerance
// purpose  :
// =======================================================================
void Poly_MergeNodesTool::MergedNodesMap::SetMergeTolerance(double theTolerance)
{
  myTolerance = (float)theTolerance;
  myInvTol    = 0.0f;
  if (myTolerance > 0.0f)
  {
    myInvTol = float(1.0 / theTolerance);
  }
}

// =======================================================================
// function : MergedNodesMap::hashCode
// purpose  :
// =======================================================================
inline size_t Poly_MergeNodesTool::MergedNodesMap::vec3iHashCode(
  const Poly_MergeNodesTool::MergedNodesMap::CellVec3i& theVec,
  const int                                             theUpper)
{
  // copied from NCollection_CellFilter
  constexpr uint64_t aShiftBits = (CHAR_BIT * sizeof(int64_t) - 1) / 3;
  uint64_t           aHashCode  = 0;
  aHashCode                     = (aHashCode << aShiftBits) ^ theVec[0];
  aHashCode                     = (aHashCode << aShiftBits) ^ theVec[1];
  aHashCode                     = (aHashCode << aShiftBits) ^ theVec[2];
  return aHashCode % theUpper + 1;
}

// =======================================================================
// function : MergedNodesMap::hashCode
// purpose  :
// =======================================================================
inline size_t Poly_MergeNodesTool::MergedNodesMap::hashCode(const NCollection_Vec3<float>& thePos,
                                                            const NCollection_Vec3<float>& theNorm,
                                                            const int theUpper) const
{
  (void)theNorm;
  if (myInvTol <= 0.0f)
  {
    // compute DJB2 hash of a string
    const size_t              aLength   = sizeof(NCollection_Vec3<float>);
    unsigned int              aHashCode = 0;
    const Standard_Character* c         = (Standard_CString)&thePos;
    for (size_t i = 0; i < aLength; ++i, ++c)
    {
      aHashCode = ((aHashCode << 5) + aHashCode) ^ (*c);
    }
    return aHashCode % theUpper + 1;
  }

  const CellVec3i anIndex = vec3ToCell(thePos);
  return vec3iHashCode(anIndex, theUpper);
}

// =======================================================================
// function : MergedNodesMap::vec3AreEqual
// purpose  :
// =======================================================================
inline bool Poly_MergeNodesTool::MergedNodesMap::vec3AreEqual(
  const NCollection_Vec3<float>& theKey1,
  const NCollection_Vec3<float>& theKey2) const
{
  if (myInvTol <= 0.0f)
  {
    return theKey1.IsEqual(theKey2);
  }

  /// tolerance should be smaller than triangle size to avoid artifacts
  // const CellVec3i anIndex1 = vec3ToCell (theKey1);
  // const CellVec3i anIndex2 = vec3ToCell (theKey2);
  // return anIndex1.IsEqual (anIndex2);

  float aVal = theKey1.x() - theKey2.x();
  if (aVal < 0)
  {
    aVal = -aVal;
  }
  if (aVal > myTolerance)
  {
    return false;
  }
  aVal = theKey1.y() - theKey2.y();
  if (aVal < 0)
  {
    aVal = -aVal;
  }
  if (aVal > myTolerance)
  {
    return false;
  }
  aVal = theKey1.z() - theKey2.z();
  if (aVal < 0)
  {
    aVal = -aVal;
  }
  if (aVal > myTolerance)
  {
    return false;
  }
  return true;
}

// =======================================================================
// function : MergedNodesMap::isEqual
// purpose  :
// =======================================================================
inline bool Poly_MergeNodesTool::MergedNodesMap::isEqual(const Vec3AndNormal&           theKey1,
                                                         const NCollection_Vec3<float>& thePos2,
                                                         const NCollection_Vec3<float>& theNorm2,
                                                         bool& theIsOpposite) const
{
  if (!vec3AreEqual(theKey1.Pos, thePos2))
  {
    return false;
  }

  const float aCosinus = theKey1.Norm.Dot(theNorm2);
  if (aCosinus >= myAngleCos)
  {
    // theIsOpposite = false;
    return true;
  }
  else if (myToMergeOpposite && aCosinus <= -myAngleCos)
  {
    theIsOpposite = true;
    return true;
  }
  return false;
}

// =======================================================================
// function : MergedNodesMap::Bind
// purpose  :
// =======================================================================
inline bool Poly_MergeNodesTool::MergedNodesMap::Bind(int&                           theIndex,
                                                      bool&                          theIsOpposite,
                                                      const NCollection_Vec3<float>& thePos,
                                                      const NCollection_Vec3<float>& theNorm)
{
  if (Resizable())
  {
    ReSize(Extent());
  }

  DataMapNode** aData = (DataMapNode**)myData1;
  const size_t  aHash = hashCode(thePos, theNorm, NbBuckets());
  for (DataMapNode* aNodeIter = aData[aHash]; aNodeIter != NULL;
       aNodeIter              = (DataMapNode*)aNodeIter->Next())
  {
    if (isEqual(aNodeIter->Key(), thePos, theNorm, theIsOpposite))
    {
      theIndex = aNodeIter->ChangeValue();
      return false;
    }
  }
  if (myInvTol > 0.0f)
  {
    static const CellVec3i THE_NEIGHBRS[26] = {
      CellVec3i(-1, 0, 0),  CellVec3i(1, 0, 0),  CellVec3i(0, -1, 0),   CellVec3i(0, 1, 0),
      CellVec3i(0, 0, -1),  CellVec3i(0, 0, 1),  CellVec3i(-1, -1, 0),  CellVec3i(1, -1, 0),
      CellVec3i(1, 1, 0),   CellVec3i(-1, 1, 0), CellVec3i(0, -1, -1),  CellVec3i(0, 1, -1),
      CellVec3i(0, 1, 1),   CellVec3i(0, -1, 1), CellVec3i(-1, 0, -1),  CellVec3i(1, 0, -1),
      CellVec3i(1, 0, 1),   CellVec3i(-1, 0, 1), CellVec3i(-1, -1, -1), CellVec3i(1, -1, -1),
      CellVec3i(-1, 1, -1), CellVec3i(1, 1, -1), CellVec3i(-1, -1, 1),  CellVec3i(1, -1, 1),
      CellVec3i(-1, 1, 1),  CellVec3i(1, 1, 1)};
    const CellVec3i anIndexCnt = vec3ToCell(thePos);
    for (int aNeigIter = 0; aNeigIter < 26; ++aNeigIter)
    {
      const CellVec3i anIndex = anIndexCnt + THE_NEIGHBRS[aNeigIter];
      const size_t    aHashEx = vec3iHashCode(anIndex, NbBuckets());
      for (DataMapNode* aNodeIter = aData[aHashEx]; aNodeIter != NULL;
           aNodeIter              = (DataMapNode*)aNodeIter->Next())
      {
        if (isEqual(aNodeIter->Key(), thePos, theNorm, theIsOpposite))
        {
          theIndex = aNodeIter->ChangeValue();
          return false;
        }
      }
    }
  }
  // theIsOpposite = false;
  aData[aHash] = new (this->myAllocator) DataMapNode(thePos, theNorm, theIndex, aData[aHash]);
  Increment();
  return true;
}

// =======================================================================
// function : MergedNodesMap::ReSize
// purpose  :
// =======================================================================
inline void Poly_MergeNodesTool::MergedNodesMap::ReSize(const int theSize)
{
  NCollection_ListNode** aNewData   = NULL;
  NCollection_ListNode** aDummy     = NULL;
  int                    aNbNewBuck = 0;
  if (BeginResize(theSize, aNbNewBuck, aNewData, aDummy))
  {
    if (DataMapNode** anOldData = (DataMapNode**)myData1)
    {
      for (int anOldBuckIter = 0; anOldBuckIter <= NbBuckets(); ++anOldBuckIter)
      {
        for (DataMapNode* anOldNodeIter = anOldData[anOldBuckIter]; anOldNodeIter != NULL;)
        {
          const size_t aNewHash  = hashCode(anOldNodeIter->Key(), aNbNewBuck);
          DataMapNode* aNextNode = (DataMapNode*)anOldNodeIter->Next();
          anOldNodeIter->Next()  = aNewData[aNewHash];
          aNewData[aNewHash]     = anOldNodeIter;
          anOldNodeIter          = aNextNode;
        }
      }
    }
    EndResize(theSize, aNbNewBuck, aNewData, aDummy);
  }
}

// =======================================================================
// function : Poly_MergeNodesTool
// purpose  :
// =======================================================================
Poly_MergeNodesTool::Poly_MergeNodesTool(const double theSmoothAngle,
                                         const double theMergeTolerance,
                                         const int    theNbFacets)
    : myPolyData(new Poly_Triangulation()),
      myNodeIndexMap(
        (theSmoothAngle > 0.0 || theMergeTolerance > 0.0) ? initialNbBuckets(theNbFacets) : 1),
      myNodeInds(0, 0, 0, -1),
      myTriNormal(0.0f, 0.0f, 1.0f),
      myUnitFactor(1.0),
      myNbNodes(0),
      myNbElems(0),
      myNbDegenElems(0),
      myNbMergedElems(0),
      myToDropDegenerative(true),
      myToMergeElems(false)
{
  SetMergeAngle(theSmoothAngle);
  SetMergeTolerance(theMergeTolerance);
}

// =======================================================================
// function : AddElement
// purpose  :
// =======================================================================
void Poly_MergeNodesTool::AddElement(const gp_XYZ* theElemNodes, int theNbNodes)
{
  if (theNbNodes != 3 && theNbNodes != 4)
  {
    throw Standard_ProgramError("Poly_MergeNodesTool::AddElement() - Internal error");
  }

  myPlaces[0] = theElemNodes[0];
  myPlaces[1] = theElemNodes[1];
  myPlaces[2] = theElemNodes[2];
  if (theNbNodes == 4)
  {
    myPlaces[3] = theElemNodes[3];
  }
  PushLastElement(theNbNodes);
}

// =======================================================================
// function : PushLastElement
// purpose  :
// =======================================================================
void Poly_MergeNodesTool::PushLastElement(int theNbNodes)
{
  if (theNbNodes != 3 && theNbNodes != 4)
  {
    throw Standard_ProgramError("Poly_MergeNodesTool::PushLastElement() - Internal error");
  }

  bool isOpposite = false;
  myNodeInds[3]   = -1;
  if (myNodeIndexMap.HasMergeAngle() || myNodeIndexMap.HasMergeTolerance())
  {
    if (!myNodeIndexMap.ToMergeAnyAngle())
    {
      myTriNormal = computeTriNormal();
    }

    pushNodeCheck(isOpposite, 0);
    pushNodeCheck(isOpposite, 1);
    pushNodeCheck(isOpposite, 2);
    if (theNbNodes == 4)
    {
      pushNodeCheck(isOpposite, 3);
    }
  }
  else
  {
    pushNodeNoMerge(0);
    pushNodeNoMerge(1);
    pushNodeNoMerge(2);
    if (theNbNodes == 4)
    {
      pushNodeNoMerge(3);
    }
  }

  if (myToDropDegenerative)
  {
    // warning - removing degenerate elements may produce unused nodes
    if (myNodeInds[0] == myNodeInds[1] || myNodeInds[0] == myNodeInds[2]
        || myNodeInds[1] == myNodeInds[2])
    {
      if (theNbNodes == 4)
      {
        //
      }
      else
      {
        ++myNbDegenElems;
        return;
      }
    }
  }

  if (myToMergeElems)
  {
    NCollection_Vec4<int> aSorted = myNodeInds;
    std::sort(aSorted.ChangeData(), aSorted.ChangeData() + theNbNodes);
    if (!myElemMap.Add(aSorted))
    {
      ++myNbMergedElems;
      return;
    }
  }

  ++myNbElems;
  if (!myPolyData.IsNull())
  {
    if (myPolyData->NbTriangles() < myNbElems)
    {
      myPolyData->ResizeTriangles(myNbElems * 2, true);
    }
    myPolyData->SetTriangle(myNbElems,
                            Poly_Triangle(myNodeInds[0] + 1, myNodeInds[1] + 1, myNodeInds[2] + 1));
    if (theNbNodes == 4)
    {
      ++myNbElems;
      if (myPolyData->NbTriangles() < myNbElems)
      {
        myPolyData->ResizeTriangles(myNbElems * 2, true);
      }
      myPolyData->SetTriangle(
        myNbElems,
        Poly_Triangle(myNodeInds[0] + 1, myNodeInds[2] + 1, myNodeInds[3] + 1));
    }
  }
}

// =======================================================================
// function : AddTriangulation
// purpose  :
// =======================================================================
void Poly_MergeNodesTool::AddTriangulation(const Handle(Poly_Triangulation)& theTris,
                                           const gp_Trsf&                    theTrsf,
                                           const Standard_Boolean            theToReverse)
{
  if (theTris.IsNull())
  {
    return;
  }

  if (!myPolyData.IsNull() && myPolyData->NbNodes() == 0)
  {
    // preallocate optimistically
    myPolyData->SetDoublePrecision(theTris->IsDoublePrecision());
    myPolyData->ResizeNodes(theTris->NbNodes(), false);
    myPolyData->ResizeTriangles(theTris->NbTriangles(), false);
  }

  for (int anElemIter = 1; anElemIter <= theTris->NbTriangles(); ++anElemIter)
  {
    Poly_Triangle anElem = theTris->Triangle(anElemIter);
    if (theToReverse)
    {
      anElem = Poly_Triangle(anElem.Value(1), anElem.Value(3), anElem.Value(2));
    }
    for (int aTriNodeIter = 0; aTriNodeIter < 3; ++aTriNodeIter)
    {
      const gp_Pnt aNode     = theTris->Node(anElem.Value(aTriNodeIter + 1)).Transformed(theTrsf);
      myPlaces[aTriNodeIter] = aNode.XYZ();
    }
    PushLastTriangle();
  }
}

// =======================================================================
// function : Result
// purpose  :
// =======================================================================
Handle(Poly_Triangulation) Poly_MergeNodesTool::Result()
{
  if (myPolyData.IsNull())
  {
    return Handle(Poly_Triangulation)();
  }

  // compress data
  myPolyData->ResizeNodes(myNbNodes, true);
  myPolyData->ResizeTriangles(myNbElems, true);
  return myPolyData;
}

// =======================================================================
// function : MergeNodes
// purpose  :
// =======================================================================
Handle(Poly_Triangulation) Poly_MergeNodesTool::MergeNodes(
  const Handle(Poly_Triangulation)& theTris,
  const gp_Trsf&                    theTrsf,
  const Standard_Boolean            theToReverse,
  const double                      theSmoothAngle,
  const double                      theMergeTolerance,
  const bool                        theToForce)
{
  if (theTris.IsNull() || theTris->NbNodes() < 3 || theTris->NbTriangles() < 1)
  {
    return Handle(Poly_Triangulation)();
  }

  Poly_MergeNodesTool aMergeTool(theSmoothAngle, theMergeTolerance, theTris->NbTriangles());
  aMergeTool.AddTriangulation(theTris, theTrsf, theToReverse);
  if (!theToForce && aMergeTool.NbNodes() == theTris->NbNodes()
      && aMergeTool.NbElements() == theTris->NbTriangles())
  {
    return Handle(Poly_Triangulation)();
  }
  return aMergeTool.Result();
}
