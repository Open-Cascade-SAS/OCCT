// Copyright (c) 2011-2019 OPEN CASCADE SAS
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

#include <Graphic3d_Layer.hxx>

#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_CullingTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Layer, Standard_Transient)

//=================================================================================================

Graphic3d_Layer::Graphic3d_Layer(Graphic3d_ZLayerId                theId,
                                 const occ::handle<BVH_Builder3d>& theBuilder)
    : myNbStructures(0),
      myNbStructuresNotCulled(0),
      myLayerId(theId),
      myBVHPrimitivesTrsfPers(theBuilder),
      myBVHIsLeftChildQueuedFirst(true),
      myIsBVHPrimitivesNeedsReset(false)
{
  myIsBoundingBoxNeedsReset[0] = myIsBoundingBoxNeedsReset[1] = true;
}

//=================================================================================================

Graphic3d_Layer::~Graphic3d_Layer()
{
  //
}

//=================================================================================================

void Graphic3d_Layer::Add(const Graphic3d_CStructure* theStruct,
                          Graphic3d_DisplayPriority   thePriority,
                          bool                        isForChangePriority)
{
  const int anIndex = std::min(std::max(thePriority, Graphic3d_DisplayPriority_Bottom),
                               Graphic3d_DisplayPriority_Topmost);
  if (theStruct == nullptr)
  {
    return;
  }

  myArray[anIndex].Add(theStruct);
  if (theStruct->IsAlwaysRendered())
  {
    theStruct->MarkAsNotCulled();
    if (!isForChangePriority)
    {
      myAlwaysRenderedMap.Add(theStruct);
    }
  }
  else if (!isForChangePriority)
  {
    if (theStruct->TransformPersistence().IsNull())
    {
      myBVHPrimitives.Add(theStruct);
    }
    else
    {
      myBVHPrimitivesTrsfPers.Add(theStruct);
    }
  }
  ++myNbStructures;
}

//=================================================================================================

bool Graphic3d_Layer::Remove(const Graphic3d_CStructure* theStruct,
                             Graphic3d_DisplayPriority&  thePriority,
                             bool                        isForChangePriority)
{
  if (theStruct == nullptr)
  {
    thePriority = Graphic3d_DisplayPriority_INVALID;
    return false;
  }

  for (int aPriorityIter = Graphic3d_DisplayPriority_Bottom;
       aPriorityIter <= Graphic3d_DisplayPriority_Topmost;
       ++aPriorityIter)
  {
    NCollection_IndexedMap<const Graphic3d_CStructure*>& aStructures = myArray[aPriorityIter];
    const int                                            anIndex = aStructures.FindIndex(theStruct);
    if (anIndex == 0)
    {
      continue;
    }

    aStructures.Swap(anIndex, aStructures.Size());
    aStructures.RemoveLast();

    if (!isForChangePriority)
    {
      bool isAlwaysRend = theStruct->IsAlwaysRendered();
      if (!isAlwaysRend)
      {
        if (!myBVHPrimitives.Remove(theStruct))
        {
          if (!myBVHPrimitivesTrsfPers.Remove(theStruct))
          {
            isAlwaysRend = true;
          }
        }
      }
      if (isAlwaysRend)
      {
        const int anIndex2 = myAlwaysRenderedMap.FindIndex(theStruct);
        if (anIndex2 != 0)
        {
          myAlwaysRenderedMap.Swap(myAlwaysRenderedMap.Size(), anIndex2);
          myAlwaysRenderedMap.RemoveLast();
        }
      }
    }
    --myNbStructures;
    thePriority = (Graphic3d_DisplayPriority)aPriorityIter;
    return true;
  }

  thePriority = Graphic3d_DisplayPriority_INVALID;
  return false;
}

//=================================================================================================

void Graphic3d_Layer::InvalidateBVHData()
{
  myIsBVHPrimitivesNeedsReset = true;
}

//! Calculate a finite bounding box of infinite object as its middle point.
inline Graphic3d_BndBox3d centerOfinfiniteBndBox(const Graphic3d_BndBox3d& theBndBox)
{
  // bounding borders of infinite line has been calculated as own point in center of this line
  const NCollection_Vec3<double> aDiagVec = theBndBox.CornerMax() - theBndBox.CornerMin();
  return aDiagVec.SquareModulus() >= 500000.0 * 500000.0
           ? Graphic3d_BndBox3d((theBndBox.CornerMin() + theBndBox.CornerMax()) * 0.5)
           : Graphic3d_BndBox3d();
}

//! Return true if at least one vertex coordinate out of float range.
inline bool isInfiniteBndBox(const Graphic3d_BndBox3d& theBndBox)
{
  return std::abs(theBndBox.CornerMax().x()) >= ShortRealLast()
         || std::abs(theBndBox.CornerMax().y()) >= ShortRealLast()
         || std::abs(theBndBox.CornerMax().z()) >= ShortRealLast()
         || std::abs(theBndBox.CornerMin().x()) >= ShortRealLast()
         || std::abs(theBndBox.CornerMin().y()) >= ShortRealLast()
         || std::abs(theBndBox.CornerMin().z()) >= ShortRealLast();
}

//! Extend bounding box with another box.
static void addBox3dToBndBox(Bnd_Box& theResBox, const Graphic3d_BndBox3d& theBox)
{
  // skip too big boxes to prevent float overflow at camera parameters calculation
  if (theBox.IsValid() && !isInfiniteBndBox(theBox))
  {
    theResBox.Add(gp_Pnt(theBox.CornerMin().x(), theBox.CornerMin().y(), theBox.CornerMin().z()));
    theResBox.Add(gp_Pnt(theBox.CornerMax().x(), theBox.CornerMax().y(), theBox.CornerMax().z()));
  }
}

//=================================================================================================

Bnd_Box Graphic3d_Layer::BoundingBox(int                                  theViewId,
                                     const occ::handle<Graphic3d_Camera>& theCamera,
                                     int                                  theWindowWidth,
                                     int                                  theWindowHeight,
                                     bool theToIncludeAuxiliary) const
{
  updateBVH();

  const int                       aBoxId         = !theToIncludeAuxiliary ? 0 : 1;
  const NCollection_Mat4<double>& aProjectionMat = theCamera->ProjectionMatrix();
  const NCollection_Mat4<double>& aWorldViewMat  = theCamera->OrientationMatrix();
  if (myIsBoundingBoxNeedsReset[aBoxId])
  {
    // Recompute layer bounding box
    myBoundingBox[aBoxId].SetVoid();

    for (int aPriorIter = Graphic3d_DisplayPriority_Bottom;
         aPriorIter <= Graphic3d_DisplayPriority_Topmost;
         ++aPriorIter)
    {
      const NCollection_IndexedMap<const Graphic3d_CStructure*>& aStructures = myArray[aPriorIter];
      for (NCollection_IndexedMap<const Graphic3d_CStructure*>::Iterator aStructIter(aStructures);
           aStructIter.More();
           aStructIter.Next())
      {
        const Graphic3d_CStructure* aStructure = aStructIter.Value();
        if (!aStructure->IsVisible(theViewId))
        {
          continue;
        }

        // "FitAll" operation ignores object with transform persistence parameter
        // but adds transform persistence point in a bounding box of layer (only zoom pers.
        // objects).
        if (!aStructure->TransformPersistence().IsNull())
        {
          if (!theToIncludeAuxiliary && aStructure->TransformPersistence()->IsZoomOrRotate())
          {
            const gp_Pnt anAnchor = aStructure->TransformPersistence()->AnchorPoint();
            myBoundingBox[aBoxId].Add(anAnchor);
            continue;
          }
          // Panning and 2d persistence apply changes to projection or/and its translation
          // components. It makes them incompatible with z-fitting algorithm. Ignored by now.
          else if (!theToIncludeAuxiliary || aStructure->TransformPersistence()->IsTrihedronOr2d())
          {
            continue;
          }
        }

        if (!theToIncludeAuxiliary && aStructure->HasGroupTransformPersistence())
        {
          // add per-group transform-persistence point in a bounding box
          for (NCollection_Sequence<occ::handle<Graphic3d_Group>>::Iterator aGroupIter(
                 aStructure->Groups());
               aGroupIter.More();
               aGroupIter.Next())
          {
            const occ::handle<Graphic3d_Group>& aGroup = aGroupIter.Value();
            if (!aGroup->TransformPersistence().IsNull()
                && aGroup->TransformPersistence()->IsZoomOrRotate())
            {
              const gp_Pnt anAnchor = aGroup->TransformPersistence()->AnchorPoint();
              myBoundingBox[aBoxId].Add(anAnchor);
            }
          }
        }

        Graphic3d_BndBox3d aBox = aStructure->BoundingBox();
        if (!aBox.IsValid())
        {
          continue;
        }

        if (aStructure->IsInfinite && !theToIncludeAuxiliary)
        {
          // include center of infinite object
          aBox = centerOfinfiniteBndBox(aBox);
        }

        if (!aStructure->TransformPersistence().IsNull())
        {
          aStructure->TransformPersistence()->Apply(theCamera,
                                                    aProjectionMat,
                                                    aWorldViewMat,
                                                    theWindowWidth,
                                                    theWindowHeight,
                                                    aBox);
        }
        addBox3dToBndBox(myBoundingBox[aBoxId], aBox);
      }
    }

    myIsBoundingBoxNeedsReset[aBoxId] = false;
  }

  Bnd_Box aResBox = myBoundingBox[aBoxId];
  if (!theToIncludeAuxiliary || myAlwaysRenderedMap.IsEmpty())
  {
    return aResBox;
  }

  // add transformation-persistent objects which depend on camera position (and thus can not be
  // cached) for operations like Z-fit
  for (NCollection_IndexedMap<const Graphic3d_CStructure*>::Iterator aStructIter(
         myAlwaysRenderedMap);
       aStructIter.More();
       aStructIter.Next())
  {
    const Graphic3d_CStructure* aStructure = aStructIter.Value();
    if (!aStructure->IsVisible(theViewId))
    {
      continue;
    }

    // handle per-group transformation persistence specifically
    if (aStructure->HasGroupTransformPersistence())
    {
      for (NCollection_Sequence<occ::handle<Graphic3d_Group>>::Iterator aGroupIter(
             aStructure->Groups());
           aGroupIter.More();
           aGroupIter.Next())
      {
        const occ::handle<Graphic3d_Group>& aGroup = aGroupIter.Value();
        const Graphic3d_BndBox4f&           aBoxF  = aGroup->BoundingBox();
        if (aGroup->TransformPersistence().IsNull() || !aBoxF.IsValid())
        {
          continue;
        }

        Graphic3d_BndBox3d aBoxCopy(NCollection_Vec3<double>(aBoxF.CornerMin().xyz()),
                                    NCollection_Vec3<double>(aBoxF.CornerMax().xyz()));
        aGroup->TransformPersistence()->Apply(theCamera,
                                              aProjectionMat,
                                              aWorldViewMat,
                                              theWindowWidth,
                                              theWindowHeight,
                                              aBoxCopy);
        addBox3dToBndBox(aResBox, aBoxCopy);
      }
    }

    const Graphic3d_BndBox3d& aStructBox = aStructure->BoundingBox();
    if (!aStructBox.IsValid() || aStructure->TransformPersistence().IsNull()
        || !aStructure->TransformPersistence()->IsTrihedronOr2d())
    {
      continue;
    }

    Graphic3d_BndBox3d aBoxCopy = aStructBox;
    aStructure->TransformPersistence()
      ->Apply(theCamera, aProjectionMat, aWorldViewMat, theWindowWidth, theWindowHeight, aBoxCopy);
    addBox3dToBndBox(aResBox, aBoxCopy);
  }

  return aResBox;
}

//=================================================================================================

double Graphic3d_Layer::considerZoomPersistenceObjects(
  int                                  theViewId,
  const occ::handle<Graphic3d_Camera>& theCamera,
  int                                  theWindowWidth,
  int                                  theWindowHeight) const
{
  if (NbOfTransformPersistenceObjects() == 0)
  {
    return 1.0;
  }

  const NCollection_Mat4<double>& aProjectionMat = theCamera->ProjectionMatrix();
  const NCollection_Mat4<double>& aWorldViewMat  = theCamera->OrientationMatrix();
  double                          aMaxCoef       = -std::numeric_limits<double>::max();

  for (int aPriorIter = Graphic3d_DisplayPriority_Bottom;
       aPriorIter <= Graphic3d_DisplayPriority_Topmost;
       ++aPriorIter)
  {
    const NCollection_IndexedMap<const Graphic3d_CStructure*>& aStructures = myArray[aPriorIter];
    for (NCollection_IndexedMap<const Graphic3d_CStructure*>::Iterator aStructIter(aStructures);
         aStructIter.More();
         aStructIter.Next())
    {
      const Graphic3d_CStructure* aStructure = aStructIter.Value();
      if (!aStructure->IsVisible(theViewId) || aStructure->TransformPersistence().IsNull()
          || !aStructure->TransformPersistence()->IsZoomOrRotate())
      {
        continue;
      }

      Graphic3d_BndBox3d aBox = aStructure->BoundingBox();
      if (!aBox.IsValid())
      {
        continue;
      }

      aStructure->TransformPersistence()
        ->Apply(theCamera, aProjectionMat, aWorldViewMat, theWindowWidth, theWindowHeight, aBox);

      const BVH_Vec3d& aCornerMin       = aBox.CornerMin();
      const BVH_Vec3d& aCornerMax       = aBox.CornerMax();
      const int        aNbOfPoints      = 8;
      const gp_Pnt aPoints[aNbOfPoints] = {gp_Pnt(aCornerMin.x(), aCornerMin.y(), aCornerMin.z()),
                                           gp_Pnt(aCornerMin.x(), aCornerMin.y(), aCornerMax.z()),
                                           gp_Pnt(aCornerMin.x(), aCornerMax.y(), aCornerMin.z()),
                                           gp_Pnt(aCornerMin.x(), aCornerMax.y(), aCornerMax.z()),
                                           gp_Pnt(aCornerMax.x(), aCornerMin.y(), aCornerMin.z()),
                                           gp_Pnt(aCornerMax.x(), aCornerMin.y(), aCornerMax.z()),
                                           gp_Pnt(aCornerMax.x(), aCornerMax.y(), aCornerMin.z()),
                                           gp_Pnt(aCornerMax.x(), aCornerMax.y(), aCornerMax.z())};
      gp_Pnt       aConvertedPoints[aNbOfPoints];
      double       aConvertedMinX = std::numeric_limits<double>::max();
      double       aConvertedMaxX = -std::numeric_limits<double>::max();
      double       aConvertedMinY = std::numeric_limits<double>::max();
      double       aConvertedMaxY = -std::numeric_limits<double>::max();
      for (int anIdx = 0; anIdx < aNbOfPoints; ++anIdx)
      {
        aConvertedPoints[anIdx] = theCamera->Project(aPoints[anIdx]);

        aConvertedMinX = std::min(aConvertedMinX, aConvertedPoints[anIdx].X());
        aConvertedMaxX = std::max(aConvertedMaxX, aConvertedPoints[anIdx].X());

        aConvertedMinY = std::min(aConvertedMinY, aConvertedPoints[anIdx].Y());
        aConvertedMaxY = std::max(aConvertedMaxY, aConvertedPoints[anIdx].Y());
      }

      const bool isBigObject =
        (std::abs(aConvertedMaxX - aConvertedMinX)
         > 2.0) // width  of zoom pers. object greater than width  of window
                // clang-format off
                                         || (std::abs (aConvertedMaxY - aConvertedMinY) > 2.0); // height of zoom pers. object greater than height of window
                // clang-format on
      const bool isAlreadyInScreen = (aConvertedMinX > -1.0 && aConvertedMinX < 1.0)
                                     && (aConvertedMaxX > -1.0 && aConvertedMaxX < 1.0)
                                     && (aConvertedMinY > -1.0 && aConvertedMinY < 1.0)
                                     && (aConvertedMaxY > -1.0 && aConvertedMaxY < 1.0);
      if (isBigObject || isAlreadyInScreen)
      {
        continue;
      }

      const gp_Pnt aTPPoint          = aStructure->TransformPersistence()->AnchorPoint();
      gp_Pnt       aConvertedTPPoint = theCamera->Project(aTPPoint);
      aConvertedTPPoint.SetZ(0.0);

      if (aConvertedTPPoint.Coord().Modulus() < Precision::Confusion())
      {
        continue;
      }

      double aShiftX = 0.0;
      if (aConvertedMinX < -1.0)
      {
        aShiftX =
          ((aConvertedMaxX < -1.0) ? (-(1.0 + aConvertedMaxX) + (aConvertedMaxX - aConvertedMinX))
                                   : -(1.0 + aConvertedMinX));
      }
      else if (aConvertedMaxX > 1.0)
      {
        aShiftX =
          ((aConvertedMinX > 1.0) ? ((aConvertedMinX - 1.0) + (aConvertedMaxX - aConvertedMinX))
                                  : (aConvertedMaxX - 1.0));
      }

      double aShiftY = 0.0;
      if (aConvertedMinY < -1.0)
      {
        aShiftY =
          ((aConvertedMaxY < -1.0) ? (-(1.0 + aConvertedMaxY) + (aConvertedMaxY - aConvertedMinY))
                                   : -(1.0 + aConvertedMinY));
      }
      else if (aConvertedMaxY > 1.0)
      {
        aShiftY =
          ((aConvertedMinY > 1.0) ? ((aConvertedMinY - 1.0) + (aConvertedMaxY - aConvertedMinY))
                                  : (aConvertedMaxY - 1.0));
      }

      const double aDifX = std::abs(aConvertedTPPoint.X()) - aShiftX;
      const double aDifY = std::abs(aConvertedTPPoint.Y()) - aShiftY;
      if (aDifX > Precision::Confusion())
      {
        aMaxCoef = std::max(aMaxCoef, std::abs(aConvertedTPPoint.X()) / aDifX);
      }
      if (aDifY > Precision::Confusion())
      {
        aMaxCoef = std::max(aMaxCoef, std::abs(aConvertedTPPoint.Y()) / aDifY);
      }
    }
  }

  return (aMaxCoef > 0.0) ? aMaxCoef : 1.0;
}

//=================================================================================================

void Graphic3d_Layer::updateBVH() const
{
  if (!myIsBVHPrimitivesNeedsReset)
  {
    return;
  }

  myBVHPrimitives.Clear();
  myBVHPrimitivesTrsfPers.Clear();
  myAlwaysRenderedMap.Clear();
  myIsBVHPrimitivesNeedsReset = false;
  for (int aPriorIter = Graphic3d_DisplayPriority_Bottom;
       aPriorIter <= Graphic3d_DisplayPriority_Topmost;
       ++aPriorIter)
  {
    const NCollection_IndexedMap<const Graphic3d_CStructure*>& aStructures = myArray[aPriorIter];
    for (NCollection_IndexedMap<const Graphic3d_CStructure*>::Iterator aStructIter(aStructures);
         aStructIter.More();
         aStructIter.Next())
    {
      const Graphic3d_CStructure* aStruct = aStructIter.Value();
      if (aStruct->IsAlwaysRendered())
      {
        aStruct->MarkAsNotCulled();
        myAlwaysRenderedMap.Add(aStruct);
      }
      else if (aStruct->TransformPersistence().IsNull())
      {
        myBVHPrimitives.Add(aStruct);
      }
      else
      {
        myBVHPrimitivesTrsfPers.Add(aStruct);
      }
    }
  }
}

namespace
{
//! This structure describes the node in BVH
struct NodeInStack
{
  NodeInStack(int theId = 0, bool theIsFullInside = false)
      : Id(theId),
        IsFullInside(theIsFullInside)
  {
  }

  int  Id;           //!< node identifier
  bool IsFullInside; //!< if the node is completely inside
};
} // namespace

//=================================================================================================

void Graphic3d_Layer::UpdateCulling(
  int                                             theViewId,
  const Graphic3d_CullingTool&                    theSelector,
  const Graphic3d_RenderingParams::FrustumCulling theFrustumCullingState)
{
  updateBVH();

  myNbStructuresNotCulled = myNbStructures;
  if (theFrustumCullingState != Graphic3d_RenderingParams::FrustumCulling_NoUpdate)
  {
    bool toTraverse = (theFrustumCullingState == Graphic3d_RenderingParams::FrustumCulling_On);
    for (NCollection_IndexedMap<const Graphic3d_CStructure*>::Iterator aStructIter(
           myBVHPrimitives.Structures());
         aStructIter.More();
         aStructIter.Next())
    {
      const Graphic3d_CStructure* aStruct = aStructIter.Value();
      aStruct->SetCulled(toTraverse);
    }
    for (NCollection_IndexedMap<const Graphic3d_CStructure*>::Iterator aStructIter(
           myBVHPrimitivesTrsfPers.Structures());
         aStructIter.More();
         aStructIter.Next())
    {
      const Graphic3d_CStructure* aStruct = aStructIter.Value();
      aStruct->SetCulled(toTraverse);
    }
  }

  if (theFrustumCullingState != Graphic3d_RenderingParams::FrustumCulling_On)
  {
    return;
  }
  if (myBVHPrimitives.Size() == 0 && myBVHPrimitivesTrsfPers.Size() == 0)
  {
    return;
  }

  myNbStructuresNotCulled = myAlwaysRenderedMap.Extent();
  Graphic3d_CullingTool::CullingContext aCullCtx;
  theSelector.SetCullingDistance(aCullCtx, myLayerSettings.CullingDistance());
  theSelector.SetCullingSize(aCullCtx, myLayerSettings.CullingSize());
  for (int aBVHTreeIdx = 0; aBVHTreeIdx < 2; ++aBVHTreeIdx)
  {
    const bool                               isTrsfPers = aBVHTreeIdx == 1;
    opencascade::handle<BVH_Tree<double, 3>> aBVHTree;
    if (isTrsfPers)
    {
      if (myBVHPrimitivesTrsfPers.Size() == 0)
        continue;

      const NCollection_Mat4<double>&     aProjection     = theSelector.ProjectionMatrix();
      const NCollection_Mat4<double>&     aWorldView      = theSelector.WorldViewMatrix();
      const Graphic3d_WorldViewProjState& aWVPState       = theSelector.WorldViewProjState();
      const int                           aViewportWidth  = theSelector.ViewportWidth();
      const int                           aViewportHeight = theSelector.ViewportHeight();

      aBVHTree = myBVHPrimitivesTrsfPers.BVH(theSelector.Camera(),
                                             aProjection,
                                             aWorldView,
                                             aViewportWidth,
                                             aViewportHeight,
                                             aWVPState);
    }
    else
    {
      if (myBVHPrimitives.Size() == 0)
        continue;

      aBVHTree = myBVHPrimitives.BVH();
    }

    const bool  toCheckFullInside = true;
    NodeInStack aNode(0, toCheckFullInside); // a root node
    if (theSelector.IsCulled(aCullCtx,
                             aBVHTree->MinPoint(0),
                             aBVHTree->MaxPoint(0),
                             toCheckFullInside ? &aNode.IsFullInside : nullptr))
    {
      continue;
    }

    NodeInStack aStack[BVH_Constants_MaxTreeDepth];
    int         aHead = -1;
    for (;;)
    {
      if (!aBVHTree->IsOuter(aNode.Id))
      {
        NodeInStack aLeft(aBVHTree->Child<0>(aNode.Id), toCheckFullInside);
        NodeInStack aRight(aBVHTree->Child<1>(aNode.Id), toCheckFullInside);
        bool        isLeftChildIn = true, isRightChildIn = true;
        if (aNode.IsFullInside)
        {
          // small size should be always checked
          isLeftChildIn  = !theSelector.IsTooSmall(aCullCtx,
                                                  aBVHTree->MinPoint(aLeft.Id),
                                                  aBVHTree->MaxPoint(aLeft.Id));
          isRightChildIn = !theSelector.IsTooSmall(aCullCtx,
                                                   aBVHTree->MinPoint(aRight.Id),
                                                   aBVHTree->MaxPoint(aRight.Id));
        }
        else
        {
          isLeftChildIn = !theSelector.IsCulled(aCullCtx,
                                                aBVHTree->MinPoint(aLeft.Id),
                                                aBVHTree->MaxPoint(aLeft.Id),
                                                toCheckFullInside ? &aLeft.IsFullInside : nullptr);
          if (!isLeftChildIn)
          {
            aLeft.IsFullInside = false;
          }

          isRightChildIn = !theSelector.IsCulled(aCullCtx,
                                                 aBVHTree->MinPoint(aRight.Id),
                                                 aBVHTree->MaxPoint(aRight.Id),
                                                 toCheckFullInside ? &aRight.IsFullInside : nullptr);
          if (!isRightChildIn)
          {
            aRight.IsFullInside = false;
          }
        }

        if (isLeftChildIn && isRightChildIn)
        {
          aNode                       = myBVHIsLeftChildQueuedFirst ? aLeft : aRight;
          aStack[++aHead]             = myBVHIsLeftChildQueuedFirst ? aRight : aLeft;
          myBVHIsLeftChildQueuedFirst = !myBVHIsLeftChildQueuedFirst;
        }
        else if (isLeftChildIn || isRightChildIn)
        {
          aNode = isLeftChildIn ? aLeft : aRight;
        }
        else
        {
          if (aHead < 0)
          {
            break;
          }

          aNode = aStack[aHead--];
        }
      }
      else
      {
        const int aStartIdx = aBVHTree->BegPrimitive(aNode.Id);
        const int anEndIdx  = aBVHTree->EndPrimitive(aNode.Id);
        for (int anIdx = aStartIdx; anIdx <= anEndIdx; ++anIdx)
        {
          const Graphic3d_CStructure* aStruct = isTrsfPers
                                                  ? myBVHPrimitivesTrsfPers.GetStructureById(anIdx)
                                                  : myBVHPrimitives.GetStructureById(anIdx);
          if (aStruct->IsVisible(theViewId))
          {
            aStruct->MarkAsNotCulled();
            ++myNbStructuresNotCulled;
          }
        }
        if (aHead < 0)
        {
          break;
        }

        aNode = aStack[aHead--];
      }
    }
  }
}

//=================================================================================================

bool Graphic3d_Layer::Append(const Graphic3d_Layer& theOther)
{
  // add all structures to destination priority list
  for (int aPriorityIter = Graphic3d_DisplayPriority_Bottom;
       aPriorityIter <= Graphic3d_DisplayPriority_Topmost;
       ++aPriorityIter)
  {
    const NCollection_IndexedMap<const Graphic3d_CStructure*>& aStructures =
      theOther.myArray[aPriorityIter];
    for (NCollection_IndexedMap<const Graphic3d_CStructure*>::Iterator aStructIter(aStructures);
         aStructIter.More();
         aStructIter.Next())
    {
      Add(aStructIter.Value(), (Graphic3d_DisplayPriority)aPriorityIter);
    }
  }

  return true;
}

//=================================================================================================

void Graphic3d_Layer::SetLayerSettings(const Graphic3d_ZLayerSettings& theSettings)
{
  const bool toUpdateTrsf =
    !myLayerSettings.Origin().IsEqual(theSettings.Origin(), gp::Resolution());
  myLayerSettings = theSettings;
  if (!toUpdateTrsf)
  {
    return;
  }

  for (int aPriorIter = Graphic3d_DisplayPriority_Bottom;
       aPriorIter <= Graphic3d_DisplayPriority_Topmost;
       ++aPriorIter)
  {
    NCollection_IndexedMap<const Graphic3d_CStructure*>& aStructures = myArray[aPriorIter];
    for (NCollection_IndexedMap<const Graphic3d_CStructure*>::Iterator aStructIter(aStructures);
         aStructIter.More();
         aStructIter.Next())
    {
      Graphic3d_CStructure* aStructure = const_cast<Graphic3d_CStructure*>(aStructIter.Value());
      aStructure->updateLayerTransformation();
    }
  }
}

//=================================================================================================

void Graphic3d_Layer::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, this)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myLayerId)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myNbStructures)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myNbStructuresNotCulled)

  for (int aPriorityIter = Graphic3d_DisplayPriority_Bottom;
       aPriorityIter <= Graphic3d_DisplayPriority_Topmost;
       ++aPriorityIter)
  {
    const NCollection_IndexedMap<const Graphic3d_CStructure*>& aStructures = myArray[aPriorityIter];
    for (NCollection_IndexedMap<const Graphic3d_CStructure*>::Iterator aStructIter(aStructures);
         aStructIter.More();
         aStructIter.Next())
    {
      const Graphic3d_CStructure* aStructure = aStructIter.Value();
      OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, aStructure)
    }
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myLayerSettings)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBVHIsLeftChildQueuedFirst)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsBVHPrimitivesNeedsReset)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsBoundingBoxNeedsReset[0])
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsBoundingBoxNeedsReset[1])

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBoundingBox[0])
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBoundingBox[1])
}
