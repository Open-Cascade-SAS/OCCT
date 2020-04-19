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

// =======================================================================
// function : Graphic3d_Layer
// purpose  :
// =======================================================================
Graphic3d_Layer::Graphic3d_Layer (Graphic3d_ZLayerId theId,
                                  Standard_Integer theNbPriorities,
                                  const Handle(Select3D_BVHBuilder3d)& theBuilder)
: myArray                     (0, theNbPriorities - 1),
  myNbStructures              (0),
  myNbStructuresNotCulled     (0),
  myLayerId                   (theId),
  myBVHPrimitivesTrsfPers     (theBuilder),
  myBVHIsLeftChildQueuedFirst (Standard_True),
  myIsBVHPrimitivesNeedsReset (Standard_False)
{
  myIsBoundingBoxNeedsReset[0] = myIsBoundingBoxNeedsReset[1] = true;
}

// =======================================================================
// function : ~Graphic3d_Layer
// purpose  :
// =======================================================================
Graphic3d_Layer::~Graphic3d_Layer()
{
  //
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void Graphic3d_Layer::Add (const Graphic3d_CStructure* theStruct,
                           Standard_Integer thePriority,
                           Standard_Boolean isForChangePriority)
{
  const Standard_Integer anIndex = Min (Max (thePriority, 0), myArray.Length() - 1);
  if (theStruct == NULL)
  {
    return;
  }

  myArray (anIndex).Add (theStruct);
  if (theStruct->IsAlwaysRendered())
  {
    theStruct->MarkAsNotCulled();
    if (!isForChangePriority)
    {
      myAlwaysRenderedMap.Add (theStruct);
    }
  }
  else if (!isForChangePriority)
  {
    if (theStruct->TransformPersistence().IsNull())
    {
      myBVHPrimitives.Add (theStruct);
    }
    else
    {
      myBVHPrimitivesTrsfPers.Add (theStruct);
    }
  }
  ++myNbStructures;
}

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
bool Graphic3d_Layer::Remove (const Graphic3d_CStructure* theStruct,
                              Standard_Integer& thePriority,
                              Standard_Boolean isForChangePriority)
{
  if (theStruct == NULL)
  {
    thePriority = -1;
    return false;
  }

  const Standard_Integer aNbPriorities = myArray.Length();
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    Graphic3d_IndexedMapOfStructure& aStructures = myArray (aPriorityIter);
    const Standard_Integer anIndex = aStructures.FindIndex (theStruct);
    if (anIndex == 0)
    {
      continue;
    }

    aStructures.Swap (anIndex, aStructures.Size());
    aStructures.RemoveLast();

    if (!isForChangePriority)
    {
      Standard_Boolean isAlwaysRend = theStruct->IsAlwaysRendered();
      if (!isAlwaysRend)
      {
        if (!myBVHPrimitives.Remove (theStruct))
        {
          if (!myBVHPrimitivesTrsfPers.Remove (theStruct))
          {
            isAlwaysRend = Standard_True;
          }
        }
      }
      if (isAlwaysRend)
      {
        const Standard_Integer anIndex2 = myAlwaysRenderedMap.FindIndex (theStruct);
        if (anIndex2 != 0)
        {
          myAlwaysRenderedMap.Swap (myAlwaysRenderedMap.Size(), anIndex2);
          myAlwaysRenderedMap.RemoveLast();
        }
      }
    }
    --myNbStructures;
    thePriority = aPriorityIter;
    return true;
  }

  thePriority = -1;
  return false;
}

// =======================================================================
// function : InvalidateBVHData
// purpose  :
// =======================================================================
void Graphic3d_Layer::InvalidateBVHData()
{
  myIsBVHPrimitivesNeedsReset = Standard_True;
}

//! Calculate a finite bounding box of infinite object as its middle point.
inline Graphic3d_BndBox3d centerOfinfiniteBndBox (const Graphic3d_BndBox3d& theBndBox)
{
  // bounding borders of infinite line has been calculated as own point in center of this line
  const Graphic3d_Vec3d aDiagVec = theBndBox.CornerMax() - theBndBox.CornerMin();
  return aDiagVec.SquareModulus() >= 500000.0 * 500000.0
       ? Graphic3d_BndBox3d ((theBndBox.CornerMin() + theBndBox.CornerMax()) * 0.5)
       : Graphic3d_BndBox3d();
}

//! Return true if at least one vertex coordinate out of float range.
inline bool isInfiniteBndBox (const Graphic3d_BndBox3d& theBndBox)
{
  return Abs (theBndBox.CornerMax().x()) >= ShortRealLast()
      || Abs (theBndBox.CornerMax().y()) >= ShortRealLast()
      || Abs (theBndBox.CornerMax().z()) >= ShortRealLast()
      || Abs (theBndBox.CornerMin().x()) >= ShortRealLast()
      || Abs (theBndBox.CornerMin().y()) >= ShortRealLast()
      || Abs (theBndBox.CornerMin().z()) >= ShortRealLast();
}

// =======================================================================
// function : BoundingBox
// purpose  :
// =======================================================================
Bnd_Box Graphic3d_Layer::BoundingBox (Standard_Integer theViewId,
                                      const Handle(Graphic3d_Camera)& theCamera,
                                      Standard_Integer theWindowWidth,
                                      Standard_Integer theWindowHeight,
                                      Standard_Boolean theToIncludeAuxiliary) const
{
  updateBVH();

  const Standard_Integer aBoxId = !theToIncludeAuxiliary ? 0 : 1;
  const Graphic3d_Mat4d& aProjectionMat = theCamera->ProjectionMatrix();
  const Graphic3d_Mat4d& aWorldViewMat  = theCamera->OrientationMatrix();
  if (myIsBoundingBoxNeedsReset[aBoxId])
  {
    // Recompute layer bounding box
    myBoundingBox[aBoxId].SetVoid();

    for (Graphic3d_ArrayOfIndexedMapOfStructure::Iterator aMapIter (myArray); aMapIter.More(); aMapIter.Next())
    {
      const Graphic3d_IndexedMapOfStructure& aStructures = aMapIter.Value();
      for (Graphic3d_IndexedMapOfStructure::Iterator aStructIter (aStructures); aStructIter.More(); aStructIter.Next())
      {
        const Graphic3d_CStructure* aStructure = aStructIter.Value();
        if (!aStructure->IsVisible (theViewId))
        {
          continue;
        }

        // "FitAll" operation ignores object with transform persistence parameter
        // but adds transform persistence point in a bounding box of layer (only zoom pers. objects).
        if (!aStructure->TransformPersistence().IsNull())
        {
          if (!theToIncludeAuxiliary
            && aStructure->TransformPersistence()->IsZoomOrRotate())
          {
            const gp_Pnt anAnchor = aStructure->TransformPersistence()->AnchorPoint();
            myBoundingBox[aBoxId].Add (anAnchor);
            continue;
          }
          // Panning and 2d persistence apply changes to projection or/and its translation components.
          // It makes them incompatible with z-fitting algorithm. Ignored by now.
          else if (!theToIncludeAuxiliary
                 || aStructure->TransformPersistence()->IsTrihedronOr2d())
          {
            continue;
          }
        }

        Graphic3d_BndBox3d aBox = aStructure->BoundingBox();
        if (!aBox.IsValid())
        {
          continue;
        }

        if (aStructure->IsInfinite
        && !theToIncludeAuxiliary)
        {
          // include center of infinite object
          aBox = centerOfinfiniteBndBox (aBox);
        }

        if (!aStructure->TransformPersistence().IsNull())
        {
          aStructure->TransformPersistence()->Apply (theCamera, aProjectionMat, aWorldViewMat, theWindowWidth, theWindowHeight, aBox);
        }

        // skip too big boxes to prevent float overflow at camera parameters calculation
        if (aBox.IsValid()
        && !isInfiniteBndBox (aBox))
        {
          myBoundingBox[aBoxId].Add (gp_Pnt (aBox.CornerMin().x(), aBox.CornerMin().y(), aBox.CornerMin().z()));
          myBoundingBox[aBoxId].Add (gp_Pnt (aBox.CornerMax().x(), aBox.CornerMax().y(), aBox.CornerMax().z()));
        }
      }
    }

    myIsBoundingBoxNeedsReset[aBoxId] = false;
  }

  Bnd_Box aResBox = myBoundingBox[aBoxId];
  if (!theToIncludeAuxiliary
    || myAlwaysRenderedMap.IsEmpty())
  {
    return aResBox;
  }

  // add transformation-persistent objects which depend on camera position (and thus can not be cached) for operations like Z-fit
  for (NCollection_IndexedMap<const Graphic3d_CStructure*>::Iterator aStructIter (myAlwaysRenderedMap); aStructIter.More(); aStructIter.Next())
  {
    const Graphic3d_CStructure* aStructure = aStructIter.Value();
    if (!aStructure->IsVisible (theViewId))
    {
      continue;
    }
    else if (aStructure->TransformPersistence().IsNull()
         || !aStructure->TransformPersistence()->IsTrihedronOr2d())
    {
      continue;
    }

    Graphic3d_BndBox3d aBox = aStructure->BoundingBox();
    if (!aBox.IsValid())
    {
      continue;
    }

    aStructure->TransformPersistence()->Apply (theCamera, aProjectionMat, aWorldViewMat, theWindowWidth, theWindowHeight, aBox);
    if (aBox.IsValid()
    && !isInfiniteBndBox (aBox))
    {
      aResBox.Add (gp_Pnt (aBox.CornerMin().x(), aBox.CornerMin().y(), aBox.CornerMin().z()));
      aResBox.Add (gp_Pnt (aBox.CornerMax().x(), aBox.CornerMax().y(), aBox.CornerMax().z()));
    }
  }

  return aResBox;
}

// =======================================================================
// function : considerZoomPersistenceObjects
// purpose  :
// =======================================================================
Standard_Real Graphic3d_Layer::considerZoomPersistenceObjects (Standard_Integer theViewId,
                                                               const Handle(Graphic3d_Camera)& theCamera,
                                                               Standard_Integer theWindowWidth,
                                                               Standard_Integer theWindowHeight) const
{
  if (NbOfTransformPersistenceObjects() == 0)
  {
    return 1.0;
  }

  const Graphic3d_Mat4d& aProjectionMat = theCamera->ProjectionMatrix();
  const Graphic3d_Mat4d& aWorldViewMat  = theCamera->OrientationMatrix();
  Standard_Real          aMaxCoef       = -std::numeric_limits<double>::max();

  for (Graphic3d_ArrayOfIndexedMapOfStructure::Iterator aMapIter (myArray); aMapIter.More(); aMapIter.Next())
  {
    const Graphic3d_IndexedMapOfStructure& aStructures = aMapIter.Value();
    for (Graphic3d_IndexedMapOfStructure::Iterator aStructIter (aStructures); aStructIter.More(); aStructIter.Next())
    {
      const Graphic3d_CStructure* aStructure = aStructIter.Value();
      if (!aStructure->IsVisible (theViewId)
       ||  aStructure->TransformPersistence().IsNull()
       || !aStructure->TransformPersistence()->IsZoomOrRotate())
      {
        continue;
      }

      Graphic3d_BndBox3d aBox = aStructure->BoundingBox();
      if (!aBox.IsValid())
      {
        continue;
      }

      aStructure->TransformPersistence()->Apply (theCamera, aProjectionMat, aWorldViewMat, theWindowWidth, theWindowHeight, aBox);

      const BVH_Vec3d&       aCornerMin           = aBox.CornerMin();
      const BVH_Vec3d&       aCornerMax           = aBox.CornerMax();
      const Standard_Integer aNbOfPoints          = 8;
      const gp_Pnt           aPoints[aNbOfPoints] = { gp_Pnt (aCornerMin.x(), aCornerMin.y(), aCornerMin.z()),
                                                      gp_Pnt (aCornerMin.x(), aCornerMin.y(), aCornerMax.z()),
                                                      gp_Pnt (aCornerMin.x(), aCornerMax.y(), aCornerMin.z()),
                                                      gp_Pnt (aCornerMin.x(), aCornerMax.y(), aCornerMax.z()),
                                                      gp_Pnt (aCornerMax.x(), aCornerMin.y(), aCornerMin.z()),
                                                      gp_Pnt (aCornerMax.x(), aCornerMin.y(), aCornerMax.z()),
                                                      gp_Pnt (aCornerMax.x(), aCornerMax.y(), aCornerMin.z()),
                                                      gp_Pnt (aCornerMax.x(), aCornerMax.y(), aCornerMax.z()) };
      gp_Pnt aConvertedPoints[aNbOfPoints];
      Standard_Real aConvertedMinX =  std::numeric_limits<double>::max();
      Standard_Real aConvertedMaxX = -std::numeric_limits<double>::max();
      Standard_Real aConvertedMinY =  std::numeric_limits<double>::max();
      Standard_Real aConvertedMaxY = -std::numeric_limits<double>::max();
      for (Standard_Integer anIdx = 0; anIdx < aNbOfPoints; ++anIdx)
      {
        aConvertedPoints[anIdx] = theCamera->Project (aPoints[anIdx]);

        aConvertedMinX          = Min (aConvertedMinX, aConvertedPoints[anIdx].X());
        aConvertedMaxX          = Max (aConvertedMaxX, aConvertedPoints[anIdx].X());

        aConvertedMinY          = Min (aConvertedMinY, aConvertedPoints[anIdx].Y());
        aConvertedMaxY          = Max (aConvertedMaxY, aConvertedPoints[anIdx].Y());
      }

      const Standard_Boolean isBigObject  = (Abs (aConvertedMaxX - aConvertedMinX) > 2.0)  // width  of zoom pers. object greater than width  of window
                                         || (Abs (aConvertedMaxY - aConvertedMinY) > 2.0); // height of zoom pers. object greater than height of window
      const Standard_Boolean isAlreadyInScreen = (aConvertedMinX > -1.0 && aConvertedMinX < 1.0)
                                              && (aConvertedMaxX > -1.0 && aConvertedMaxX < 1.0)
                                              && (aConvertedMinY > -1.0 && aConvertedMinY < 1.0)
                                              && (aConvertedMaxY > -1.0 && aConvertedMaxY < 1.0);
      if (isBigObject || isAlreadyInScreen)
      {
        continue;
      }

      const gp_Pnt aTPPoint = aStructure->TransformPersistence()->AnchorPoint();
      gp_Pnt aConvertedTPPoint = theCamera->Project (aTPPoint);
      aConvertedTPPoint.SetZ (0.0);

      if (aConvertedTPPoint.Coord().Modulus() < Precision::Confusion())
      {
        continue;
      }

      Standard_Real aShiftX = 0.0;
      if (aConvertedMinX < -1.0)
      {
        aShiftX = ((aConvertedMaxX < -1.0) ? (-(1.0 + aConvertedMaxX) + (aConvertedMaxX - aConvertedMinX)) : -(1.0 + aConvertedMinX));
      }
      else if (aConvertedMaxX > 1.0)
      {
        aShiftX = ((aConvertedMinX > 1.0) ? ((aConvertedMinX - 1.0) + (aConvertedMaxX - aConvertedMinX)) : (aConvertedMaxX - 1.0));
      }

      Standard_Real aShiftY = 0.0;
      if (aConvertedMinY < -1.0)
      {
        aShiftY = ((aConvertedMaxY < -1.0) ? (-(1.0 + aConvertedMaxY) + (aConvertedMaxY - aConvertedMinY)) : -(1.0 + aConvertedMinY));
      }
      else if (aConvertedMaxY > 1.0)
      {
        aShiftY = ((aConvertedMinY > 1.0) ? ((aConvertedMinY - 1.0) + (aConvertedMaxY - aConvertedMinY)) : (aConvertedMaxY - 1.0));
      }

      const Standard_Real aDifX = Abs (aConvertedTPPoint.X()) - aShiftX;
      const Standard_Real aDifY = Abs (aConvertedTPPoint.Y()) - aShiftY;
      if (aDifX > Precision::Confusion())
      {
        aMaxCoef = Max (aMaxCoef, Abs (aConvertedTPPoint.X()) / aDifX);
      }
      if (aDifY > Precision::Confusion())
      {
        aMaxCoef = Max (aMaxCoef, Abs (aConvertedTPPoint.Y()) / aDifY);
      }
    }
  }

  return (aMaxCoef > 0.0) ? aMaxCoef : 1.0;
}

// =======================================================================
// function : updateBVH
// purpose  :
// =======================================================================
void Graphic3d_Layer::updateBVH() const
{
  if (!myIsBVHPrimitivesNeedsReset)
  {
    return;
  }

  myBVHPrimitives.Clear();
  myBVHPrimitivesTrsfPers.Clear();
  myAlwaysRenderedMap.Clear();
  myIsBVHPrimitivesNeedsReset = Standard_False;
  for (Graphic3d_ArrayOfIndexedMapOfStructure::Iterator aMapIter (myArray); aMapIter.More(); aMapIter.Next())
  {
    const Graphic3d_IndexedMapOfStructure& aStructures = aMapIter.Value();
    for (Graphic3d_IndexedMapOfStructure::Iterator aStructIter (aStructures); aStructIter.More(); aStructIter.Next())
    {
      const Graphic3d_CStructure* aStruct = aStructIter.Value();
      if (aStruct->IsAlwaysRendered())
      {
        aStruct->MarkAsNotCulled();
        myAlwaysRenderedMap.Add (aStruct);
      }
      else if (aStruct->TransformPersistence().IsNull())
      {
        myBVHPrimitives.Add (aStruct);
      }
      else
      {
        myBVHPrimitivesTrsfPers.Add (aStruct);
      }
    }
  }
}

namespace
{
  //! This structure describes the node in BVH
  struct NodeInStack
  {
    NodeInStack (Standard_Integer theId = 0,
                 Standard_Boolean theIsFullInside = false) : Id (theId), IsFullInside (theIsFullInside) {}

    Standard_Integer Id;           //!< node identifier
    Standard_Boolean IsFullInside; //!< if the node is completely inside
  };
}

// =======================================================================
// function : UpdateCulling
// purpose  :
// =======================================================================
void Graphic3d_Layer::UpdateCulling (Standard_Integer theViewId,
                                     const Graphic3d_CullingTool& theSelector,
                                     const Graphic3d_RenderingParams::FrustumCulling theFrustumCullingState)
{
  updateBVH();

  myNbStructuresNotCulled = myNbStructures;
  if (theFrustumCullingState != Graphic3d_RenderingParams::FrustumCulling_NoUpdate)
  {
    Standard_Boolean toTraverse = (theFrustumCullingState == Graphic3d_RenderingParams::FrustumCulling_On);
    for (Graphic3d_IndexedMapOfStructure::Iterator aStructIter (myBVHPrimitives.Structures()); aStructIter.More(); aStructIter.Next())
    {
      const Graphic3d_CStructure* aStruct = aStructIter.Value();
      aStruct->SetCulled (toTraverse);
    }
    for (Graphic3d_IndexedMapOfStructure::Iterator aStructIter (myBVHPrimitivesTrsfPers.Structures()); aStructIter.More(); aStructIter.Next())
    {
      const Graphic3d_CStructure* aStruct = aStructIter.Value();
      aStruct->SetCulled (toTraverse);
    }
  }

  if (theFrustumCullingState != Graphic3d_RenderingParams::FrustumCulling_On)
  {
    return;
  }
  if (myBVHPrimitives        .Size() == 0
   && myBVHPrimitivesTrsfPers.Size() == 0)
  {
    return;
  }

  myNbStructuresNotCulled = myAlwaysRenderedMap.Extent();
  Graphic3d_CullingTool::CullingContext aCullCtx;
  theSelector.SetCullingDistance(aCullCtx, myLayerSettings.CullingDistance());
  theSelector.SetCullingSize    (aCullCtx, myLayerSettings.CullingSize());
  for (Standard_Integer aBVHTreeIdx = 0; aBVHTreeIdx < 2; ++aBVHTreeIdx)
  {
    const Standard_Boolean isTrsfPers = aBVHTreeIdx == 1;
    opencascade::handle<BVH_Tree<Standard_Real, 3> > aBVHTree;
    if (isTrsfPers)
    {
      if (myBVHPrimitivesTrsfPers.Size() == 0)
        continue;

      const Graphic3d_Mat4d& aProjection            = theSelector.ProjectionMatrix();
      const Graphic3d_Mat4d& aWorldView             = theSelector.WorldViewMatrix();
      const Graphic3d_WorldViewProjState& aWVPState = theSelector.WorldViewProjState();
      const Standard_Integer aViewportWidth         = theSelector.ViewportWidth();
      const Standard_Integer aViewportHeight        = theSelector.ViewportHeight();

      aBVHTree = myBVHPrimitivesTrsfPers.BVH (theSelector.Camera(), aProjection, aWorldView, aViewportWidth, aViewportHeight, aWVPState);
    }
    else
    {
      if (myBVHPrimitives.Size() == 0)
        continue;

      aBVHTree = myBVHPrimitives.BVH();
    }

    const bool toCheckFullInside = true;
    NodeInStack aNode (0, toCheckFullInside); // a root node
    if (theSelector.IsCulled (aCullCtx, aBVHTree->MinPoint (0), aBVHTree->MaxPoint (0), toCheckFullInside ? &aNode.IsFullInside : NULL))
    {
      continue;
    }

    NodeInStack aStack[BVH_Constants_MaxTreeDepth];
    Standard_Integer aHead = -1;
    for (;;)
    {
      if (!aBVHTree->IsOuter (aNode.Id))
      {
        NodeInStack aLeft (aBVHTree->Child<0> (aNode.Id), toCheckFullInside);
        NodeInStack aRight(aBVHTree->Child<1> (aNode.Id), toCheckFullInside);
        bool isLeftChildIn = true, isRightChildIn = true;
        if (aNode.IsFullInside)
        {
          // small size should be always checked
          isLeftChildIn  = !theSelector.IsTooSmall (aCullCtx, aBVHTree->MinPoint (aLeft.Id),  aBVHTree->MaxPoint (aLeft.Id));
          isRightChildIn = !theSelector.IsTooSmall (aCullCtx, aBVHTree->MinPoint (aRight.Id), aBVHTree->MaxPoint (aRight.Id));
        }
        else
        {
          isLeftChildIn = !theSelector.IsCulled (aCullCtx, aBVHTree->MinPoint (aLeft.Id),  aBVHTree->MaxPoint (aLeft.Id), toCheckFullInside ? &aLeft.IsFullInside : NULL);
          if (!isLeftChildIn)
          {
            aLeft.IsFullInside = false;
          }

          isRightChildIn = !theSelector.IsCulled (aCullCtx, aBVHTree->MinPoint (aRight.Id), aBVHTree->MaxPoint (aRight.Id), toCheckFullInside ? &aRight.IsFullInside : NULL);
          if (!isRightChildIn)
          {
            aRight.IsFullInside = false;
          }
        }

        if (isLeftChildIn
         && isRightChildIn)
        {
          aNode = myBVHIsLeftChildQueuedFirst ? aLeft : aRight;
          aStack[++aHead] = myBVHIsLeftChildQueuedFirst ? aRight : aLeft;
          myBVHIsLeftChildQueuedFirst = !myBVHIsLeftChildQueuedFirst;
        }
        else if (isLeftChildIn
              || isRightChildIn)
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
        const Standard_Integer aStartIdx = aBVHTree->BegPrimitive (aNode.Id);
        const Standard_Integer anEndIdx  = aBVHTree->EndPrimitive (aNode.Id);
        for (Standard_Integer anIdx = aStartIdx; anIdx <= anEndIdx; ++anIdx)
        {
          const Graphic3d_CStructure* aStruct = isTrsfPers
                                              ? myBVHPrimitivesTrsfPers.GetStructureById (anIdx)
                                              : myBVHPrimitives.GetStructureById (anIdx);
          if (aStruct->IsVisible (theViewId))
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

// =======================================================================
// function : Append
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_Layer::Append (const Graphic3d_Layer& theOther)
{
  // the source priority list shouldn't have more priorities
  const Standard_Integer aNbPriorities = theOther.NbPriorities();
  if (aNbPriorities > NbPriorities())
  {
    return Standard_False;
  }

  // add all structures to destination priority list
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    const Graphic3d_IndexedMapOfStructure& aStructures = theOther.myArray (aPriorityIter);
    for (Graphic3d_IndexedMapOfStructure::Iterator aStructIter (aStructures); aStructIter.More(); aStructIter.Next())
    {
      Add (aStructIter.Value(), aPriorityIter);
    }
  }

  return Standard_True;
}

//=======================================================================
//function : SetLayerSettings
//purpose  :
//=======================================================================
void Graphic3d_Layer::SetLayerSettings (const Graphic3d_ZLayerSettings& theSettings)
{
  const Standard_Boolean toUpdateTrsf = !myLayerSettings.Origin().IsEqual (theSettings.Origin(), gp::Resolution());
  myLayerSettings = theSettings;
  if (!toUpdateTrsf)
  {
    return;
  }

  for (Graphic3d_ArrayOfIndexedMapOfStructure::Iterator aMapIter (myArray); aMapIter.More(); aMapIter.Next())
  {
    Graphic3d_IndexedMapOfStructure& aStructures = aMapIter.ChangeValue();
    for (Graphic3d_IndexedMapOfStructure::Iterator aStructIter (aStructures); aStructIter.More(); aStructIter.Next())
    {
      Graphic3d_CStructure* aStructure = const_cast<Graphic3d_CStructure* >(aStructIter.Value());
      aStructure->updateLayerTransformation();
    }
  }
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Graphic3d_Layer::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, this)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myLayerId)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myNbStructures)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myNbStructuresNotCulled)

  const Standard_Integer aNbPriorities = myArray.Length();
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    const Graphic3d_IndexedMapOfStructure& aStructures = myArray (aPriorityIter);
    for (Graphic3d_IndexedMapOfStructure::Iterator aStructIter (aStructures); aStructIter.More(); aStructIter.Next())
    {
      const Graphic3d_CStructure* aStructure = aStructIter.Value();
      OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, aStructure)
    }
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myLayerSettings)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myBVHIsLeftChildQueuedFirst)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsBVHPrimitivesNeedsReset)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsBoundingBoxNeedsReset[0])
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsBoundingBoxNeedsReset[1])

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myBoundingBox[0])
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myBoundingBox[1])
}
