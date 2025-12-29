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

#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Message_Msg.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeFix_FixSmallSolid.hxx>
#include <Standard_Type.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_FixSmallSolid, ShapeFix_Root)

//=================================================================================================

ShapeFix_FixSmallSolid::ShapeFix_FixSmallSolid()
    : myFixMode(0),
      myVolumeThreshold(Precision::Infinite()),
      myWidthFactorThreshold(Precision::Infinite())
{
}

//=======================================================================
// function : SetFixMode
// purpose  : Set the mode for applying fixes of small solids.
//=======================================================================
void ShapeFix_FixSmallSolid::SetFixMode(const int theMode)
{
  myFixMode = (theMode < 0 || theMode > 2) ? 0 : theMode;
}

//=======================================================================
// function : SetVolumeThreshold
// purpose  : Set or clear volume threshold for small solids
//=======================================================================
void ShapeFix_FixSmallSolid::SetVolumeThreshold(const double theThreshold)
{
  myVolumeThreshold = theThreshold >= 0.0 ? theThreshold : Precision::Infinite();
}

//=======================================================================
// function : SetWidthFactorThreshold
// purpose  : Set or clear width factor threshold for small solids
//=======================================================================
void ShapeFix_FixSmallSolid::SetWidthFactorThreshold(const double theThreshold)
{
  myWidthFactorThreshold = theThreshold >= 0.0 ? theThreshold : Precision::Infinite();
}

//=================================================================================================

// Check if an input shape is valid
static bool IsValidInput(const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
    return false;

  switch (theShape.ShapeType())
  {
    case TopAbs_COMPOUND:
    case TopAbs_COMPSOLID:
    case TopAbs_SOLID:
      return true;
    default:
      return false;
  }
}

//=======================================================================
// function : Remove
// purpose  : Remove small solids from the given shape
//=======================================================================
TopoDS_Shape ShapeFix_FixSmallSolid::Remove(const TopoDS_Shape&                    theShape,
                                            const occ::handle<ShapeBuild_ReShape>& theContext) const
{
  // Check if at least one smallness criterion is set and the shape is valid
  if (!IsThresholdsSet() || !IsValidInput(theShape))
    return theShape;

  // Find and remove all small solids
  TopExp_Explorer aSolidIter(theShape, TopAbs_SOLID);
  for (; aSolidIter.More(); aSolidIter.Next())
  {
    const TopoDS_Shape& aSolid = aSolidIter.Current();
    if (IsSmall(aSolid))
    {
      theContext->Remove(aSolid);
      SendWarning(aSolid, Message_Msg("ShapeFix.FixSmallSolid.MSG0"));
    }
  }

  // Return updated shape
  return theContext->Apply(theShape);
}

//=================================================================================================

// Calculate surface area of a shape
static double ShapeArea(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(theShape, aProps);
  return aProps.Mass();
}

//=================================================================================================

// Calculate volume of a shape
static double ShapeVolume(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}

//=================================================================================================

// Append an item to a list of shapes mapped to a shape
static void AddToMap(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                      theMap,
  const TopoDS_Shape& theKey,
  const TopoDS_Shape& theItem)
{
  NCollection_List<TopoDS_Shape>* aListPtr = theMap.ChangeSeek(theKey);
  if (aListPtr == nullptr)
  {
    NCollection_List<TopoDS_Shape> aList;
    aList.Append(theItem);
    theMap.Bind(theKey, aList);
  }
  else
    aListPtr->Append(theItem);
}

//=================================================================================================

// Append items to a list of shapes mapped to a shape
static void AddToMap(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                  theMap,
  const TopoDS_Shape&             theKey,
  NCollection_List<TopoDS_Shape>& theItems)
{
  if (theItems.IsEmpty())
    return;

  NCollection_List<TopoDS_Shape>* aListPtr = theMap.ChangeSeek(theKey);
  if (aListPtr == nullptr)
    theMap.Bind(theKey, theItems);
  else
    aListPtr->Append(theItems);
}

//=================================================================================================

// Map faces from a solid with their shells;
// unmap faces shared between two shells
static void MapFacesToShells(
  const TopoDS_Shape&                                                       theSolid,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theMap)
{
  TopoDS_Iterator aShellIter(theSolid);
  for (; aShellIter.More(); aShellIter.Next())
  {
    const TopoDS_Shape& aShell = aShellIter.Value();
    if (aShell.ShapeType() != TopAbs_SHELL)
      continue;

    TopoDS_Iterator aFaceIter(aShell);
    for (; aFaceIter.More(); aFaceIter.Next())
    {
      const TopoDS_Shape& aFace = aFaceIter.Value();
      if (aFace.ShapeType() != TopAbs_FACE)
        continue;

      if (!theMap.Bind(aFace, aShell))
        theMap.UnBind(aFace);
    }
  }
}

//=================================================================================================

// Find an outer shell having greatest sum area of
// all faces shared with the solid
static bool FindMostSharedShell(
  const TopoDS_Shape& theSolid,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&
                                  theMapFacesToOuterShells,
  TopoDS_Shape&                   theMostSharedOuterShell,
  TopoDS_Shape&                   theMostSharedSolidShell,
  NCollection_List<TopoDS_Shape>& theOtherSolidShells)
{
  NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher> aSharedAreas;
  double                                                             aMaxSharedArea = 0.0;
  const TopoDS_Shape* aMostSharedOuterShellPtr                                      = nullptr;
  const TopoDS_Shape* aMostSharedSolidShellPtr                                      = nullptr;

  // check every shell in the solid for faces shared with outer shells
  TopoDS_Iterator aShellIter(theSolid);
  for (; aShellIter.More(); aShellIter.Next())
  {
    const TopoDS_Shape& aSolidShell = aShellIter.Value();
    if (aSolidShell.ShapeType() != TopAbs_SHELL)
      continue;

    theOtherSolidShells.Append(aSolidShell);

    TopoDS_Iterator aFaceIter(aSolidShell);
    for (; aFaceIter.More(); aFaceIter.Next())
    {
      const TopoDS_Shape& aFace = aFaceIter.Value();
      if (aFace.ShapeType() != TopAbs_FACE)
        continue;

      // find an outer shell that shares the current face
      const TopoDS_Shape* anOuterShellPtr = theMapFacesToOuterShells.Seek(aFace);
      if (anOuterShellPtr == nullptr)
        continue;
      const TopoDS_Shape& anOuterShell = *anOuterShellPtr;

      // add the face area to the sum shared area for the outer shell
      double  anArea         = ShapeArea(aFace);
      double* aSharedAreaPtr = aSharedAreas.ChangeSeek(anOuterShell);
      if (aSharedAreaPtr == nullptr)
        aSharedAreas.Bind(anOuterShell, anArea);
      else
        anArea = (*aSharedAreaPtr) += anArea;

      // if this outer shell currently has maximum shared area,
      // remember it and the current solid's shell
      if (aMaxSharedArea < anArea)
      {
        aMaxSharedArea           = anArea;
        aMostSharedOuterShellPtr = &anOuterShell;
        aMostSharedSolidShellPtr = &aSolidShell;
      }
    }
  }

  // return nothing if no adjanced outer shells were found
  if (aMostSharedSolidShellPtr == nullptr)
    return false;

  // compose return values
  theMostSharedOuterShell = *aMostSharedOuterShellPtr;
  theMostSharedSolidShell = *aMostSharedSolidShellPtr;

  // remove the most shared solid's shell from the returned list of its other shells
  NCollection_List<TopoDS_Shape>::Iterator anOtherShellIter(theOtherSolidShells);
  while (!anOtherShellIter.Value().IsSame(theMostSharedSolidShell))
    anOtherShellIter.Next();
  theOtherSolidShells.Remove(anOtherShellIter);

  return true;
}

//=================================================================================================

// Merge some shells to a base shell
static TopoDS_Shape MergeShells(
  const TopoDS_Shape&             theBaseShell,
  NCollection_List<TopoDS_Shape>& theShellsToMerge,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&
    theMapFacesToOuterShells,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&
    theMapNewFreeFacesToShells)
{
  // Create a new shell
  BRep_Builder aBuilder;
  TopoDS_Shape aNewShell = theBaseShell.EmptyCopied();

  // Sort the faces belogning to the merged shells:
  // - faces shared with the base shell:
  //     keep to remove from the base shell;
  // - faces shared with other outer shells, non-face elements:
  //     add to the new shell;
  // - faces not shared with any outer or any merged shell:
  //     keep to add to the new shell and to the new map.
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aRemoveFaces;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aNewFreeFaces;

  NCollection_List<TopoDS_Shape>::Iterator aShellIter(theShellsToMerge);
  for (; aShellIter.More(); aShellIter.Next())
  {
    TopoDS_Iterator aFaceIter(aShellIter.Value());
    for (; aFaceIter.More(); aFaceIter.Next())
    {
      const TopoDS_Shape& aFace = aFaceIter.Value();

      // non-face element in a shell - just add it to the new shell
      if (aFace.ShapeType() != TopAbs_FACE)
      {
        aBuilder.Add(aNewShell, aFace);
        continue;
      }

      // classify the face
      const TopoDS_Shape* anOuterShellPtr = theMapFacesToOuterShells.Seek(aFace);
      if (anOuterShellPtr != nullptr)
      {
        if (anOuterShellPtr->IsSame(theBaseShell))
          aRemoveFaces.Add(aFace); // face shared with the base shell
        else
          aBuilder.Add(aNewShell, aFace); // face shared with another outer shell
      }
      else
      {
        if (aNewFreeFaces.Contains(aFace))
          aNewFreeFaces.Remove(aFace); // face shared with another merged shell
        else
          aNewFreeFaces.Add(aFace); // face not shared
      }
    }
  }
  theShellsToMerge.Clear();

  // Add the kept faces from the merged shells to the new shell
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aNewFaceIter(aNewFreeFaces);
  for (; aNewFaceIter.More(); aNewFaceIter.Next())
  {
    const TopoDS_Shape& aFace = aNewFaceIter.Key();
    aBuilder.Add(aNewShell, aFace);
    theMapNewFreeFacesToShells.Bind(aFace, aNewShell);
  }
  aNewFreeFaces.Clear();

  // Add needed faces from the base shell to the new shell
  TopoDS_Iterator aBaseFaceIter(theBaseShell);
  for (; aBaseFaceIter.More(); aBaseFaceIter.Next())
  {
    const TopoDS_Shape& aFace = aBaseFaceIter.Value();
    if (!aRemoveFaces.Contains(aFace))
      aBuilder.Add(aNewShell, aFace);
  }

  // If there are no elements in the new shell, return null shape
  if (aNewShell.NbChildren() == 0)
    return TopoDS_Shape();

  return aNewShell;
}

//=================================================================================================

// Add some shells to a base shell
static TopoDS_Compound AddShells(const TopoDS_Shape&             theBaseShell,
                                 NCollection_List<TopoDS_Shape>& theShellsToAdd)
{
  // Create a compound
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Add the base shell to the compound
  if (!theBaseShell.IsNull())
    aBuilder.Add(aCompound, theBaseShell);

  // Add other shells to the compound
  NCollection_List<TopoDS_Shape>::Iterator aShellIter(theShellsToAdd);
  for (; aShellIter.More(); aShellIter.Next())
    aBuilder.Add(aCompound, aShellIter.Value());

  theShellsToAdd.Clear();

  return aCompound;
}

TopoDS_Shape ShapeFix_FixSmallSolid::Merge(const TopoDS_Shape&                    theShape,
                                           const occ::handle<ShapeBuild_ReShape>& theContext) const
{
  // Check if at least one smallness criterion is set and the shape is valid
  if (!IsThresholdsSet() || !IsValidInput(theShape))
    return theShape;

  // Find all small solids and put them in a list;
  // Build a map of faces belonging to non-small solids
  // but not shared between two non-small solids
  NCollection_List<TopoDS_Shape>                                           aSmallSolids;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aMapFacesToShells;

  TopExp_Explorer aSolidIter(theShape, TopAbs_SOLID);
  for (; aSolidIter.More(); aSolidIter.Next())
  {
    const TopoDS_Shape& aSolid = aSolidIter.Current();
    if (IsSmall(aSolid))
      aSmallSolids.Append(aSolid);
    else
      MapFacesToShells(aSolid, aMapFacesToShells);
  }

  // Merge all small solids adjacent to at least one non-small one;
  // repeat this until no small solids remain or no new solids can be merged
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>  aNewMapFacesToShells;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>* aMapFacesToShellsPtr =
    &aMapFacesToShells;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>*
    aNewMapFacesToShellsPtr = &aNewMapFacesToShells;
  while (!aSmallSolids.IsEmpty())
  {
    // find small solids that may be merged on the current iteration;
    // compose their shells in lists associated with non-small solids' shells
    // which they should be merged to
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                             aShellsToMerge, aShellsToAdd;
    NCollection_List<TopoDS_Shape>::Iterator aSmallIter(aSmallSolids);
    while (aSmallIter.More())
    {
      const TopoDS_Shape& aSmallSolid = aSmallIter.Value();

      // find a non-small solid's shell having greatest sum area of
      // all faces shared with the current small solid
      TopoDS_Shape                   aNonSmallSolidShell;
      TopoDS_Shape                   anAdjacentShell;
      NCollection_List<TopoDS_Shape> aNotAdjacentShells;
      if (FindMostSharedShell(aSmallSolid,
                              *aMapFacesToShellsPtr,
                              aNonSmallSolidShell,
                              anAdjacentShell,
                              aNotAdjacentShells))
      {
        // add the small solid's shells to appropriate lists
        // associated with the selected non-small solid's shell
        AddToMap(aShellsToMerge, aNonSmallSolidShell, anAdjacentShell);
        AddToMap(aShellsToAdd, aNonSmallSolidShell, aNotAdjacentShells);

        // remove the small solid
        theContext->Remove(aSmallSolid);
        SendWarning(aSmallSolid, Message_Msg("ShapeFix.FixSmallSolid.MSG1"));

        aSmallSolids.Remove(aSmallIter);
      }
      else
        aSmallIter.Next();
    }

    // stop if no solids can be merged
    if (aShellsToMerge.IsEmpty())
      break;

    // update needed non-small solids' shells by
    // merging and adding the listed small solids' shells to them
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
      Iterator aShellIter(aShellsToMerge);
    for (; aShellIter.More(); aShellIter.Next())
    {
      // get the current non-small solid's shell
      // and corresponding small solids' shells
      const TopoDS_Shape&             aBaseShell = aShellIter.Key();
      NCollection_List<TopoDS_Shape>& aShellsToBeMerged =
        (NCollection_List<TopoDS_Shape>&)aShellIter.Value();
      NCollection_List<TopoDS_Shape>* aShellsToBeAddedPtr = aShellsToAdd.ChangeSeek(aBaseShell);

      // merge needed shells
      TopoDS_Shape aNewShell =
        MergeShells(aBaseShell, aShellsToBeMerged, *aMapFacesToShellsPtr, *aNewMapFacesToShellsPtr);

      // add new shells if needed
      if (aShellsToBeAddedPtr != nullptr)
        aNewShell = AddShells(aNewShell, *aShellsToBeAddedPtr);

      // replace the current non-small solid's shell with the new one(s)
      theContext->Replace(aBaseShell, aNewShell);
    }

    // clear the old faces map and start using the new one
    aMapFacesToShellsPtr->Clear();
    std::swap(aMapFacesToShellsPtr, aNewMapFacesToShellsPtr);
  }

  // Return updated shape
  return theContext->Apply(theShape);
}

//=======================================================================
// function : IsThresholdsSet
// purpose  : Check if at least one smallness criterion is set
//=======================================================================
bool ShapeFix_FixSmallSolid::IsThresholdsSet() const
{
  return (IsUsedVolumeThreshold() && myVolumeThreshold < Precision::Infinite())
         || (IsUsedWidthFactorThreshold() && myWidthFactorThreshold < Precision::Infinite());
}

//=======================================================================
// function : IsSmall
// purpose  : Check if a solid meets the smallness criteria
//=======================================================================
bool ShapeFix_FixSmallSolid::IsSmall(const TopoDS_Shape& theSolid) const
{
  // If the volume threshold is used and set, and the solid's volume exceeds
  // threshold value, consider the solid as not small
  double aVolume = ShapeVolume(theSolid);
  if (IsUsedVolumeThreshold() && aVolume > myVolumeThreshold)
    return false;

  // If the width factor threshold is used and set,
  // and the solid's width factor exceeds threshold value,
  // consider the solid as not small
  if (IsUsedWidthFactorThreshold() && myWidthFactorThreshold < Precision::Infinite())
  {
    double anArea = ShapeArea(theSolid);
    if (aVolume > myWidthFactorThreshold * anArea * 0.5)
      return false;
  }

  // Both thresholds are met - consider the solid as small
  return true;
}

//=======================================================================
// function : IsUsedWidthFactorThreshold
// purpose  : Check if width factor threshold criterion is used
//=======================================================================
bool ShapeFix_FixSmallSolid::IsUsedWidthFactorThreshold() const
{
  return myFixMode == 0 || myFixMode == 1;
}

//=======================================================================
// function : IsUsedVolumeThreshold
// purpose  : Check if volume threshold criterion is used
//=======================================================================
bool ShapeFix_FixSmallSolid::IsUsedVolumeThreshold() const
{
  return myFixMode == 0 || myFixMode == 2;
}
