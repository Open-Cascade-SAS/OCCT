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
#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : LocGenerated
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BOPAlgo_Builder::LocGenerated
  (const TopoDS_Shape& theS)
{
  // The rules for Generated shapes are these:
  // 1. The EDGE may be generated from the FACES as an intersection edge;
  // 2. The VERTEX may be generated from the EDGES and FACES as an intersection vertex.
  //
  // The list of generated elements will contain only those which are contained
  // in the result of the operation.

  myHistShapes.Clear();

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
void BOPAlgo_Builder::PrepareHistory(const Message_ProgressRange& theRange)
{
  if (!HasHistory())
    return;

  // Initializing history tool
  myHistory = new BRepTools_History;

  // Map the result shape
  myMapShape.Clear();
  TopExp::MapShapes(myShape, myMapShape);

  // Among all input shapes find:
  // - Shapes that have been modified (split). Add the splits kept in the result
  //   shape as Modified from the shape;
  // - Shapes that have created new geometries (i.e. generated new shapes). Add
  //   the generated elements kept in the result shape as Generated from the shape;
  // - Shapes that have no trace in the result shape. Add them as Deleted
  //   during the operation.
  Standard_Integer aNbS = myDS->NbSourceShapes();
  Message_ProgressScope aPS(theRange, "Preparing history information", aNbS);
  for (Standard_Integer i = 0; i < aNbS; ++i, aPS.Next())
  {
    const TopoDS_Shape& aS = myDS->Shape(i);

    // Check if History information is available for this kind of shape.
    if (!BRepTools_History::IsSupportedType(aS))
      continue;

    if (UserBreak(aPS))
    {
      return;
    }

    Standard_Boolean isModified = Standard_False;

    // Check if the shape has any splits
    const TopTools_ListOfShape* pLSp = LocModified(aS);
    if (pLSp)
    {
      // Find all splits of the shape which are kept in the result
      TopTools_ListIteratorOfListOfShape aIt(*pLSp);
      for (; aIt.More(); aIt.Next())
      {
        TopoDS_Shape aSp = aIt.Value();
        // Check if the result shape contains the split
        if (myMapShape.Contains(aSp))
        {
          // Add modified shape with proper orientation
          TopAbs_ShapeEnum aType = aSp.ShapeType();
          if (aType == TopAbs_VERTEX || aType == TopAbs_SOLID)
            aSp.Orientation(aS.Orientation());
          else if (BOPTools_AlgoTools::IsSplitToReverse(aSp, aS, myContext))
            aSp.Reverse();

          myHistory->AddModified(aS, aSp);
          isModified = Standard_True;
        }
      }
    }

    // Check if the shape has Generated elements
    const TopTools_ListOfShape& aGenShapes = LocGenerated(aS);
    TopTools_ListIteratorOfListOfShape aIt(aGenShapes);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aG = aIt.Value();
      if (myMapShape.Contains(aG))
        myHistory->AddGenerated(aS, aG);
    }

    // Check if the shape has been deleted, i.e. it is not contained in the result
    // and has no Modified shapes.
    if (!isModified && !myMapShape.Contains(aS))
      myHistory->Remove(aS);
  }
}
