// Created on: 2014-03-31
// Created by: Danila ULYANOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <OpenGl_Layer.hxx>

#include <OpenGl_BVHTreeSelector.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>
#include <Graphic3d_GraphicDriver.hxx>

// =======================================================================
// function : OpenGl_Layer
// purpose  :
// =======================================================================
OpenGl_Layer::OpenGl_Layer (const Standard_Integer theNbPriorities)
: myArray                     (0, theNbPriorities - 1),
  myNbStructures              (0),
  myBVHIsLeftChildQueuedFirst (Standard_True),
  myIsBVHPrimitivesNeedsReset (Standard_False)
{
  myIsBoundingBoxNeedsReset[0] = myIsBoundingBoxNeedsReset[1] = true;
}

// =======================================================================
// function : ~OpenGl_Layer
// purpose  :
// =======================================================================
OpenGl_Layer::~OpenGl_Layer()
{
  //
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void OpenGl_Layer::Add (const OpenGl_Structure* theStruct,
                        const Standard_Integer  thePriority,
                        Standard_Boolean        isForChangePriority)
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
    if (theStruct->TransformPersistence.Flags == Graphic3d_TMF_None)
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
bool OpenGl_Layer::Remove (const OpenGl_Structure* theStruct,
                           Standard_Integer&       thePriority,
                           Standard_Boolean        isForChangePriority)
{
  if (theStruct == NULL)
  {
    thePriority = -1;
    return false;
  }

  const Standard_Integer aNbPriorities = myArray.Length();
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    OpenGl_IndexedMapOfStructure& aStructures = myArray (aPriorityIter);

    const Standard_Integer anIndex = aStructures.FindIndex (theStruct);
    if (anIndex != 0)
    {
      aStructures.Swap (anIndex, aStructures.Size());
      aStructures.RemoveLast();

      if (!isForChangePriority)
      {
        if (theStruct->IsAlwaysRendered())
        {
          const Standard_Integer anIndex2 = myAlwaysRenderedMap.FindIndex (theStruct);
          if (anIndex2 != 0)
          {
            myAlwaysRenderedMap.Swap (myAlwaysRenderedMap.Size(), anIndex2);
            myAlwaysRenderedMap.RemoveLast();
          }
        }
        else
        {
          if (!myBVHPrimitives.Remove (theStruct))
          {
            myBVHPrimitivesTrsfPers.Remove (theStruct);
          }
        }
      }
      --myNbStructures;
      thePriority = aPriorityIter;
      return true;
    }
  }

  thePriority = -1;
  return false;
}

// =======================================================================
// function : InvalidateBVHData
// purpose  :
// =======================================================================
void OpenGl_Layer::InvalidateBVHData() const
{
  myIsBVHPrimitivesNeedsReset = Standard_True;
}

//! Calculate a finite bounding box of infinite object as its middle point.
inline Graphic3d_BndBox4f centerOfinfiniteBndBox (const Graphic3d_BndBox4f& theBndBox)
{
  // bounding borders of infinite line has been calculated as own point in center of this line
  const Graphic3d_Vec4 aDiagVec = theBndBox.CornerMax() - theBndBox.CornerMin();
  return aDiagVec.xyz().SquareModulus() >= 500000.0f * 500000.0f
       ? Graphic3d_BndBox4f ((theBndBox.CornerMin() + theBndBox.CornerMax()) * 0.5f)
       : Graphic3d_BndBox4f();
}

//! Return true if at least one vertex coordinate out of float range.
inline bool isInfiniteBndBox (const Graphic3d_BndBox4f& theBndBox)
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
Graphic3d_BndBox4f OpenGl_Layer::BoundingBox (const Standard_Integer          theViewId,
                                              const Handle(Graphic3d_Camera)& theCamera,
                                              const Standard_Integer          theWindowWidth,
                                              const Standard_Integer          theWindowHeight,
                                              const Standard_Boolean          theToIncludeAuxiliary) const
{
  const Standard_Integer aBoxId = !theToIncludeAuxiliary ? 0 : 1;
  const Graphic3d_Mat4& aProjectionMat = theCamera->ProjectionMatrixF();
  const Graphic3d_Mat4& aWorldViewMat  = theCamera->OrientationMatrixF();
  if (myIsBoundingBoxNeedsReset[aBoxId])
  {
    // Recompute layer bounding box
    myBoundingBox[aBoxId].Clear();

    const Standard_Integer aNbPriorities = myArray.Length();
    for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
    {
      const OpenGl_IndexedMapOfStructure& aStructures = myArray (aPriorityIter);
      for (Standard_Integer aStructIdx = 1; aStructIdx <= aStructures.Size(); ++aStructIdx)
      {
        const OpenGl_Structure* aStructure = aStructures.FindKey (aStructIdx);
        if (!aStructure->IsVisible (theViewId))
        {
          continue;
        }

        // "FitAll" operation ignores object with transform persistence parameter
        // but adds transform persistence point in a bounding box of layer (only zoom pers. objects).
        if (aStructure->TransformPersistence.Flags != Graphic3d_TMF_None)
        {
          if (!theToIncludeAuxiliary
           && (aStructure->TransformPersistence.Flags & Graphic3d_TMF_ZoomPers) != 0)
          {
            BVH_Vec4f aTPPoint (static_cast<float> (aStructure->TransformPersistence.Point.x()),
                                static_cast<float> (aStructure->TransformPersistence.Point.y()),
                                static_cast<float> (aStructure->TransformPersistence.Point.z()),
                                1.0f);

            myBoundingBox[aBoxId].Combine (aTPPoint);
            continue;
          }
          // Panning and 2d persistence apply changes to projection or/and its translation components.
          // It makes them incompatible with z-fitting algorithm. Ignored by now.
          else if (!theToIncludeAuxiliary
                || (aStructure->TransformPersistence.Flags & (Graphic3d_TMF_2d | Graphic3d_TMF_PanPers | Graphic3d_TMF_TriedronPers)) != 0)
          {
            continue;
          }
        }

        Graphic3d_BndBox4f aBox = aStructure->BoundingBox();
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

        if (aStructure->TransformPersistence.Flags != Graphic3d_TMF_None)
        {
          aStructure->TransformPersistence.Apply (theCamera,
                                                  aProjectionMat,
                                                  aWorldViewMat,
                                                  theWindowWidth,
                                                  theWindowHeight,
                                                  aBox);
        }

        // skip too big boxes to prevent float overflow at camera parameters calculation
        if (!isInfiniteBndBox (aBox))
        {
          myBoundingBox[aBoxId].Combine (aBox);
        }
      }
    }

    myIsBoundingBoxNeedsReset[aBoxId] = false;
  }

  if (!theToIncludeAuxiliary
    || myAlwaysRenderedMap.IsEmpty())
  {
    return myBoundingBox[aBoxId];
  }

  // add transformation-persistent objects which depend on camera position (and thus can not be cached) for operations like Z-fit
  Graphic3d_BndBox4f aResBox = myBoundingBox[aBoxId];
  for (NCollection_IndexedMap<const OpenGl_Structure*>::Iterator aStructIter (myAlwaysRenderedMap); aStructIter.More(); aStructIter.Next())
  {
    const OpenGl_Structure* aStructure = aStructIter.Value();
    if (!aStructure->IsVisible (theViewId))
    {
      continue;
    }
    else if ((aStructure->TransformPersistence.Flags & Graphic3d_TMF_TriedronPers) == 0)
    {
      continue;
    }

    Graphic3d_BndBox4f aBox = aStructure->BoundingBox();
    if (!aBox.IsValid())
    {
      continue;
    }

    aStructure->TransformPersistence.Apply (theCamera,
                                            aProjectionMat,
                                            aWorldViewMat,
                                            theWindowWidth,
                                            theWindowHeight,
                                            aBox);
    if (!isInfiniteBndBox (aBox))
    {
      aResBox.Combine (aBox);
    }
  }

  return aResBox;
}

// =======================================================================
// function : considerZoomPersistenceObjects
// purpose  :
// =======================================================================
Standard_Real OpenGl_Layer::considerZoomPersistenceObjects (const Standard_Integer          theViewId,
                                                            const Handle(Graphic3d_Camera)& theCamera,
                                                            Standard_Integer                theWindowWidth,
                                                            Standard_Integer                theWindowHeight) const
{
  if (NbOfTransformPersistenceObjects() == 0)
  {
    return 1.0;
  }

  const Standard_Integer aNbPriorities  = myArray.Length();
  const Graphic3d_Mat4&  aProjectionMat = theCamera->ProjectionMatrixF();
  const Graphic3d_Mat4&  aWorldViewMat  = theCamera->OrientationMatrixF();
  Standard_Real          aMaxCoef       = -std::numeric_limits<double>::max();

  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    const OpenGl_IndexedMapOfStructure& aStructures = myArray (aPriorityIter);
    for (Standard_Integer aStructIdx = 1; aStructIdx <= aStructures.Size(); ++aStructIdx)
    {
      OpenGl_Structure* aStructure = const_cast<OpenGl_Structure*> (aStructures.FindKey (aStructIdx));
      if (!aStructure->IsVisible (theViewId)
       || (aStructure->TransformPersistence.Flags & Graphic3d_TMF_ZoomPers) == 0)
      {
        continue;
      }

      Graphic3d_BndBox4f aBox = aStructure->BoundingBox();
      if (!aBox.IsValid())
      {
        continue;
      }

      aStructure->TransformPersistence.Apply (theCamera, aProjectionMat, aWorldViewMat, theWindowWidth, theWindowHeight, aBox);

      const BVH_Vec4f&       aCornerMin           = aBox.CornerMin();
      const BVH_Vec4f&       aCornerMax           = aBox.CornerMax();
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

      const gp_Pnt aTPPoint (aStructure->TransformPersistence.Point.x(),
                             aStructure->TransformPersistence.Point.y(),
                             aStructure->TransformPersistence.Point.z());
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
// function : renderAll
// purpose  :
// =======================================================================
void OpenGl_Layer::renderAll (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Standard_Integer aNbPriorities = myArray.Length();
  const Standard_Integer aViewId       = theWorkspace->View()->Identification();
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    const OpenGl_IndexedMapOfStructure& aStructures = myArray (aPriorityIter);
    for (Standard_Integer aStructIdx = 1; aStructIdx <= aStructures.Size(); ++aStructIdx)
    {
      const OpenGl_Structure* aStruct = aStructures.FindKey (aStructIdx);
      if (!aStruct->IsVisible())
      {
        continue;
      }
      else if (!aStruct->ViewAffinity.IsNull()
            && !aStruct->ViewAffinity->IsVisible (aViewId))
      {
        continue;
      }

      aStruct->Render (theWorkspace);
    }
  }
}

// =======================================================================
// function : updateBVH
// purpose  :
// =======================================================================
void OpenGl_Layer::updateBVH() const
{
  if (!myIsBVHPrimitivesNeedsReset)
  {
    return;
  }

  myBVHPrimitives.Clear();
  myBVHPrimitivesTrsfPers.Clear();
  myIsBVHPrimitivesNeedsReset = Standard_False;
  for (Standard_Integer aPriorityIdx = 0, aNbPriorities = myArray.Length(); aPriorityIdx < aNbPriorities; ++aPriorityIdx)
  {
    for (OpenGl_IndexedMapOfStructure::Iterator aStructIter (myArray (aPriorityIdx)); aStructIter.More(); aStructIter.Next())
    {
      const OpenGl_Structure* aStruct = aStructIter.Value();
      if (aStruct->IsAlwaysRendered())
      {
        continue;
      }

      if (aStruct->TransformPersistence.Flags == Graphic3d_TMF_None)
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

// =======================================================================
// function : renderTraverse
// purpose  :
// =======================================================================
void OpenGl_Layer::renderTraverse (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  updateBVH();

  OpenGl_BVHTreeSelector& aSelector = theWorkspace->View()->BVHTreeSelector();
  traverse (aSelector);

  const Standard_Integer aNbPriorities = myArray.Length();
  const Standard_Integer aViewId       = theWorkspace->View()->Identification();
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    const OpenGl_IndexedMapOfStructure& aStructures = myArray (aPriorityIter);
    for (Standard_Integer aStructIdx = 1; aStructIdx <= aStructures.Size(); ++aStructIdx)
    {
      const OpenGl_Structure* aStruct = aStructures.FindKey (aStructIdx);
      if (aStruct->IsCulled()
      || !aStruct->IsVisible (aViewId))
      {
        continue;
      }

      aStruct->Render (theWorkspace);
      aStruct->ResetCullingStatus();
    }
  }
}

// =======================================================================
// function : traverse
// purpose  :
// =======================================================================
void OpenGl_Layer::traverse (OpenGl_BVHTreeSelector& theSelector) const
{
  // handle a case when all objects are infinite
  if (myBVHPrimitives        .Size() == 0
   && myBVHPrimitivesTrsfPers.Size() == 0)
    return;

  theSelector.CacheClipPtsProjections();

  NCollection_Handle<BVH_Tree<Standard_ShortReal, 4> > aBVHTree;

  for (Standard_Integer aBVHTreeIdx = 0; aBVHTreeIdx < 2; ++aBVHTreeIdx)
  {
    const Standard_Boolean isTrsfPers = aBVHTreeIdx == 1;
    if (isTrsfPers)
    {
      if (myBVHPrimitivesTrsfPers.Size() == 0)
        continue;

      const OpenGl_Mat4& aProjection                = theSelector.ProjectionMatrix();
      const OpenGl_Mat4& aWorldView                 = theSelector.WorldViewMatrix();
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

    Standard_Integer aNode = 0; // a root node

    if (!theSelector.Intersect (aBVHTree->MinPoint (0),
                                aBVHTree->MaxPoint (0)))
    {
      continue;
    }

    Standard_Integer aStack[32];
    Standard_Integer aHead = -1;
    for (;;)
    {
      if (!aBVHTree->IsOuter (aNode))
      {
        const Standard_Integer aLeftChildIdx  = aBVHTree->Child<0> (aNode);
        const Standard_Integer aRightChildIdx = aBVHTree->Child<1> (aNode);
        const Standard_Boolean isLeftChildIn  = theSelector.Intersect (aBVHTree->MinPoint (aLeftChildIdx),
                                                                       aBVHTree->MaxPoint (aLeftChildIdx));
        const Standard_Boolean isRightChildIn = theSelector.Intersect (aBVHTree->MinPoint (aRightChildIdx),
                                                                       aBVHTree->MaxPoint (aRightChildIdx));
        if (isLeftChildIn
         && isRightChildIn)
        {
          aNode = myBVHIsLeftChildQueuedFirst ? aLeftChildIdx : aRightChildIdx;
          aStack[++aHead] = myBVHIsLeftChildQueuedFirst ? aRightChildIdx : aLeftChildIdx;
          myBVHIsLeftChildQueuedFirst = !myBVHIsLeftChildQueuedFirst;
        }
        else if (isLeftChildIn
              || isRightChildIn)
        {
          aNode = isLeftChildIn ? aLeftChildIdx : aRightChildIdx;
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
        Standard_Integer aIdx = aBVHTree->BegPrimitive (aNode);
        const OpenGl_Structure* aStruct =
          isTrsfPers ? myBVHPrimitivesTrsfPers.GetStructureById (aIdx)
                     : myBVHPrimitives.GetStructureById (aIdx);
        aStruct->MarkAsNotCulled();
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
Standard_Boolean OpenGl_Layer::Append (const OpenGl_Layer& theOther)
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
    const OpenGl_IndexedMapOfStructure& aStructures = theOther.myArray (aPriorityIter);
    for (Standard_Integer aStructIdx = 1; aStructIdx <= aStructures.Size(); ++aStructIdx)
    {
      Add (aStructures.FindKey (aStructIdx), aPriorityIter);
    }
  }

  return Standard_True;
}

//=======================================================================
//function : Render
//purpose  :
//=======================================================================
void OpenGl_Layer::Render (const Handle(OpenGl_Workspace)&   theWorkspace,
                           const OpenGl_GlobalLayerSettings& theDefaultSettings) const
{
  Graphic3d_PolygonOffset anAppliedOffsetParams = theWorkspace->AppliedPolygonOffset();

  // separate depth buffers
  if (IsSettingEnabled (Graphic3d_ZLayerDepthClear))
  {
    glClear (GL_DEPTH_BUFFER_BIT);
  }

  // handle depth test
  if (IsSettingEnabled (Graphic3d_ZLayerDepthTest))
  {
    // assuming depth test is enabled by default
    glDepthFunc (theDefaultSettings.DepthFunc);
  }
  else
  {
    glDepthFunc (GL_ALWAYS);
  }

  // save environment texture
  Handle(OpenGl_Texture) anEnvironmentTexture = theWorkspace->EnvironmentTexture();
  if (!myLayerSettings.UseEnvironmentTexture)
  {
    theWorkspace->SetEnvironmentTexture (Handle(OpenGl_Texture)());
  }

  // handle depth offset
  if (IsSettingEnabled (Graphic3d_ZLayerDepthOffset))
  {
    Graphic3d_PolygonOffset aLayerPolygonOffset;
    aLayerPolygonOffset.Mode   = Aspect_POM_Fill;
    aLayerPolygonOffset.Factor = myLayerSettings.DepthOffsetFactor;
    aLayerPolygonOffset.Units  = myLayerSettings.DepthOffsetUnits;
    theWorkspace->SetPolygonOffset (aLayerPolygonOffset);
  }
  else
  {
    theWorkspace->SetPolygonOffset (anAppliedOffsetParams);
  }

  // handle depth write
  theWorkspace->UseDepthWrite() = IsSettingEnabled (Graphic3d_ZLayerDepthWrite);
  glDepthMask (theWorkspace->UseDepthWrite() ? GL_TRUE : GL_FALSE);

  // render priority list
  theWorkspace->IsCullingEnabled() ? renderTraverse (theWorkspace) : renderAll (theWorkspace);

  // always restore polygon offset between layers rendering
  theWorkspace->SetPolygonOffset (anAppliedOffsetParams);

  // restore environment texture
  if (!myLayerSettings.UseEnvironmentTexture)
  {
    theWorkspace->SetEnvironmentTexture (anEnvironmentTexture);
  }
}
