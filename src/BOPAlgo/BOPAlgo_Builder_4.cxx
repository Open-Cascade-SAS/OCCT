// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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


#include <BOPAlgo_Builder.hxx>
#include <BOPDS_DS.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <IntTools_Context.hxx>
#include <TopExp.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BOPAlgo_Builder::Generated
  (const TopoDS_Shape& theS)
{
  // The rules for Generated shapes are these:
  // 1. The EDGE may be generated from the FACES as an intersection edge;
  // 2. The VERTEX may be generated from the EDGES and FACES as an intersection vertex.
  //
  // The list of generated elements will contain only those which are contained
  // in the result of the operation.

  myHistShapes.Clear();

  if (!myHasGenerated)
    return myHistShapes;

  if (theS.IsNull())
    return myHistShapes;

  // Only EDGES and FACES should be considered
  TopAbs_ShapeEnum aType = theS.ShapeType();
  if (aType != TopAbs_EDGE && aType != TopAbs_FACE)
    // Wrong type
    return myHistShapes;

  // Check that DS contains the shape, i.e. it is from the arguments of the operation
  Standard_Integer nS = myDS->Index(theS);
  if (nS < 0)
    // Unknown shape
    return myHistShapes;

  // Check that the shape has participated in any intersections
  const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(nS);
  if (!aSI.HasReference())
    // Untouched shape
    return myHistShapes;

  // Analyze all types of Interferences which can produce
  // new vertices - Edge/Edge and Edge/Face
  BOPDS_VectorOfInterfEE& aEEs = myDS->InterfEE();
  BOPDS_VectorOfInterfEF& aEFs = myDS->InterfEF();

  // Fence map to avoid duplicates in the list of Generated;
  TColStd_MapOfInteger aMFence;

  // Analyze each interference and find those in which the given shape has participated

  // No need to analyze Edge/Edge interferences for the shapes of type FACE
  Standard_Boolean isFace = (aType == TopAbs_FACE);

  for (Standard_Integer k = (isFace ? 1 : 0); k < 2; ++k)
  {
    Standard_Integer aNbLines = !k ? aEEs.Length() : aEFs.Length();
    for (Standard_Integer i = 0; i < aNbLines; ++i)
    {
      BOPDS_Interf *aInt = !k ? (BOPDS_Interf*)(&aEEs(i)) : (BOPDS_Interf*)(&aEFs(i));
      if (!aInt->HasIndexNew())
        // No new vertices created
        continue;

      if (!aInt->Contains(nS))
        continue;

      Standard_Integer nVNew = aInt->IndexNew();
      myDS->HasShapeSD(nVNew, nVNew);
      if (!aMFence.Add(nVNew))
        continue;

      // Get the new vertex
      const TopoDS_Shape& aVNew = myDS->Shape(nVNew);

      // Check that the result shape contains vertex
      if (myMapShape.Contains(aVNew))
        // Save the vertex as generated
        myHistShapes.Append(aVNew);
    }
  }

  if (!isFace)
    return myHistShapes;

  // For the FACE it is also necessary to collect all
  // section elements created in FACE/FACE interferences.
  // This information is available in the FaceInfo structure.
  const BOPDS_FaceInfo& aFI = myDS->FaceInfo(nS);

  // Section edges of the face
  const BOPDS_IndexedMapOfPaveBlock& aMPBSc = aFI.PaveBlocksSc();
  // Save section edges contained in the result shape
  Standard_Integer aNb = aMPBSc.Extent();
  for (Standard_Integer i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aENew = myDS->Shape(aMPBSc(i)->Edge());
    if (myMapShape.Contains(aENew))
      myHistShapes.Append(aENew);
  }

  // Section vertices of the face
  const TColStd_MapOfInteger& aMVSc = aFI.VerticesSc();
  // Save section vertices contained in the result shape
  TColStd_MapOfInteger::Iterator aItM(aMVSc);
  for (; aItM.More(); aItM.Next())
  {
    const TopoDS_Shape& aVNew = myDS->Shape(aItM.Value());
    if (myMapShape.Contains(aVNew))
      myHistShapes.Append(aVNew);
  }

  return myHistShapes;
}
//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BOPAlgo_Builder::Modified
  (const TopoDS_Shape& theS)
{
  myHistShapes.Clear();

  if (!myHasModified)
    // No modified elements
    return myHistShapes;

  const TopTools_ListOfShape* pLSp = myImagesResult.Seek(theS);
  if (!pLSp || pLSp->IsEmpty())
    // No track in the result -> no modified
    return myHistShapes;

  // For modification check if the shape is not linked to itself
  if (pLSp->Extent() == 1)
  {
    if (theS.IsSame(pLSp->First()) && !myImages.IsBound(theS))
      // Shape is not modified
      return myHistShapes;
  }

  // Iterate on all splits and save them with proper orientation into the result list
  TopTools_ListIteratorOfListOfShape aIt(*pLSp);
  for (; aIt.More(); aIt.Next())
  {
    TopoDS_Shape aSp = aIt.Value();
    // Use the orientation of the input shape
    TopAbs_ShapeEnum aType = aSp.ShapeType();
    if (aType == TopAbs_VERTEX || aType == TopAbs_SOLID)
      aSp.Orientation(theS.Orientation());
    else if (BOPTools_AlgoTools::IsSplitToReverse(aSp, theS, myContext))
      aSp.Reverse();

    myHistShapes.Append(aSp);
  }

  return myHistShapes;
}
//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_Builder::IsDeleted(const TopoDS_Shape& theS)
{
  // The shape is considered as Deleted if it has participated in the
  // operation and the result shape does not contain the shape itself
  // and none of its splits.

  if (!myHasDeleted)
    // Non of the shapes have been deleted during the operation
    return Standard_False;

  const TopTools_ListOfShape *pImages = myImagesResult.Seek(theS);
  if (!pImages)
    // No track about the shape, i.e. the shape has not participated
    // in operation -> Not deleted
    return Standard_False;

  // Check if any parts of the shape has been kept in the result
  return pImages->IsEmpty();
}
//=======================================================================
//function : LocModified
//purpose  : 
//=======================================================================
const TopTools_ListOfShape* BOPAlgo_Builder::LocModified(const TopoDS_Shape& theS)
{
  return myImages.Seek(theS);
}
//=======================================================================
//function : PrepareHistory
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::PrepareHistory()
{
  if (!myFlagHistory)
  {
    // Clearing
    BOPAlgo_BuilderShape::PrepareHistory();
    return;
  }

  // Clearing from previous operations
  BOPAlgo_BuilderShape::PrepareHistory();
  myFlagHistory = Standard_True;

  // Map the result shape
  TopExp::MapShapes(myShape, myMapShape);

  // Among all input shapes find those that have any trace in the result
  // and save them into myImagesResult map with connection to parts
  // kept in the result shape. If the input shape has no trace in the
  // result shape, link it to the empty list in myImagesResult meaning
  // that the shape has been removed.
  //
  // Also, set the proper values to the history flags:
  // - myHasDeleted for Deleted shapes;
  // - myHasModified for Modified shapes;
  // - myHasGenerated for Generated shapes.
  Standard_Integer aNbS = myDS->NbSourceShapes();
  for (Standard_Integer i = 0; i < aNbS; ++i)
  {
    const TopoDS_Shape& aS = myDS->Shape(i);

    // History information is only available for the shapes of type
    // VERTEX, EDGE, FACE and SOLID. Skip all shapes of different type.
    TopAbs_ShapeEnum aType = aS.ShapeType();
    if (!(aType == TopAbs_VERTEX ||
          aType == TopAbs_EDGE   ||
          aType == TopAbs_FACE   ||
          aType == TopAbs_SOLID))
      continue;

    // Track the modification of the shape
    TopTools_ListOfShape* pImages = &myImagesResult(myImagesResult.Add(aS, TopTools_ListOfShape()));

    // Check if the shape has any splits
    const TopTools_ListOfShape* pLSp = LocModified(aS);
    if (!pLSp)
    {
      // No splits, check if the result shape contains the shape itself
      if (myMapShape.Contains(aS))
        // Shape has passed into result without modifications -> link the shape to itself
        pImages->Append(aS);
      else
        // No trace of the shape in the result -> Deleted element is found
        myHasDeleted = Standard_True;
    }
    else
    {
      // Find all splits of the shape which are kept in the result
      TopTools_ListIteratorOfListOfShape aIt(*pLSp);
      for (; aIt.More(); aIt.Next())
      {
        const TopoDS_Shape& aSp = aIt.Value();

        // Check if the result shape contains the split
        if (myMapShape.Contains(aSp))
        {
          // Link the shape to the split
          pImages->Append(aSp);
        }
      }

      if (!pImages->IsEmpty())
        // Modified element is found
        myHasModified = Standard_True;
      else
        // Deleted element is found
        myHasDeleted = Standard_True;
    }

    // Until first found, check if the shape has Generated elements
    if (!myHasGenerated)
    {
      // Temporarily set the HasGenerated flag to TRUE to look for the shapes generated from aS.
      // Otherwise, the method Generated will always be returning an empty list, assuming that the
      // operation has no generated elements at all.
      myHasGenerated = Standard_True;
      myHasGenerated = (Generated(aS).Extent() > 0);
    }
  }
}
