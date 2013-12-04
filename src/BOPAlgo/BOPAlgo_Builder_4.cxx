// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BOPAlgo_Builder.ixx>

#include <TopoDS_Iterator.hxx>

#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>

#include <BOPTools_AlgoTools.hxx>
#include <BOPTools.hxx>

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& BOPAlgo_Builder::Generated(const TopoDS_Shape& theS)
{
  Standard_Boolean bHasImage, bToReverse;
  TopAbs_ShapeEnum aType;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  myHistShapes.Clear();
  //
  if (theS.IsNull()) {
    return myHistShapes;
  }
  //
  bHasImage=myImages.IsBound(theS);
  if (!bHasImage) {
    return myHistShapes;
  }
  //
  aType=theS.ShapeType();
  //
  if (!(aType==TopAbs_EDGE   || aType==TopAbs_FACE || 
      aType==TopAbs_VERTEX || aType==TopAbs_SOLID)) {
    return myHistShapes;
  } 
  //
  //PrepareHistory();
  //
  const BOPCol_ListOfShape& aLSp=myImages.Find(theS);
  aIt.Initialize(aLSp);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSp=aIt.Value();
    if (myShapesSD.IsBound(aSp)) {
      if (myMapShape.Contains(aSp)) {
	TopoDS_Shape aSpR=myShapesSD.Find(aSp);
	//
	if (aType==TopAbs_VERTEX || aType==TopAbs_SOLID) {
	  aSpR.Orientation(theS.Orientation());
	}
	else {
	  bToReverse=BOPTools_AlgoTools::IsSplitToReverse(aSpR, theS, myContext);
	  if (bToReverse) {
	    aSpR.Reverse();
	  }
	}
	//
	  myHistShapes.Append(aSpR);
      }
    }
  }
  //
  return myHistShapes;
}
//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& BOPAlgo_Builder::Modified(const TopoDS_Shape& theS)
{
  Standard_Boolean bHasImage, bToReverse;
  TopAbs_ShapeEnum aType;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  myHistShapes.Clear();
  //
  if (theS.IsNull()) {
    return myHistShapes;
  }
  //
  bHasImage=myImages.IsBound(theS);
  if (!bHasImage) {
    return myHistShapes;
  }
  //
  aType=theS.ShapeType();
  //
  if (!(aType==TopAbs_EDGE   || aType==TopAbs_FACE || 
      aType==TopAbs_VERTEX || aType==TopAbs_SOLID)) {
    return myHistShapes;
  } 
  //
  //PrepareHistory();
  //
  const BOPCol_ListOfShape& aLSp=myImages.Find(theS);
  aIt.Initialize(aLSp);
  for (; aIt.More(); aIt.Next()) {
    TopoDS_Shape aSp=aIt.Value();
    if (!myShapesSD.IsBound(aSp)) {
      if (myMapShape.Contains(aSp)) {
	//
	if (aType==TopAbs_VERTEX || aType==TopAbs_SOLID) {
	  aSp.Orientation(theS.Orientation());
	}
	else {
	  bToReverse=BOPTools_AlgoTools::IsSplitToReverse(aSp, theS, myContext);
	  if (bToReverse) {
	    aSp.Reverse();
	  }
	}
	//
	myHistShapes.Append(aSp);
      }
    }
  }
  //
  return myHistShapes;
}
//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_Builder::IsDeleted(const TopoDS_Shape& theS)
{
  Standard_Boolean bRet, bHasImage, bContains;
  TopAbs_ShapeEnum aType;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  bRet=Standard_False;
  //
  if (theS.IsNull()) {
    return !bRet; //true
  }
  //
  aType=theS.ShapeType();
  if (!(aType==TopAbs_EDGE || aType==TopAbs_FACE || 
      aType==TopAbs_VERTEX || aType==TopAbs_SOLID)) {
    return !bRet;
  }
  //
  bHasImage=myImages.IsBound(theS);
  if (!bHasImage) {
    return !bRet; //true
  }
  //
  //PrepareHistory();
  //
  bContains=myMapShape.Contains(theS);
  if (bContains) {
    return bRet; //false
  }
  //
  const BOPCol_ListOfShape& aLSp=myImages.Find(theS);
  aIt.Initialize(aLSp);
  for (; aIt.More(); aIt.Next()) {
    TopoDS_Shape aSp=aIt.Value();
    //
    if (!myShapesSD.IsBound(aSp)) {
      if (myMapShape.Contains(aSp)) {
	return bRet; //false
      }
    }
    else {
      TopoDS_Shape aSpR=myShapesSD.Find(aSp);
      if (myMapShape.Contains(aSpR)) {
	return bRet; //false
      }
    }
  }
  return !bRet; // true
}
//=======================================================================
//function : PrepareHistory
//purpose  : 
//=======================================================================
  void BOPAlgo_Builder::PrepareHistory()
{
  if (!myFlagHistory) {
    return;
  }
  //
  if(myShape.IsNull()) {
    return;
  }
  //
  Standard_Boolean bHasImage, bContainsSD;
  TopAbs_ShapeEnum aType;
  BOPCol_MapOfShape aMS;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPCol_MapIteratorOfMapOfShape aItM;
  //
  // 1. Clearing 
  BOPAlgo_BuilderShape::PrepareHistory();
  //
  // 2. myMapShape - all shapes of result with theirs sub-shapes 
  BOPTools::MapShapes(myShape, myMapShape);
  //
  // 3. MS - all argument shapes with theirs sub-shapes
  aIt.Initialize(myArguments);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value();
    BOPTools::MapShapes(aSx, aMS);
  }
  //
  // 4. Treatment
  aItM.Initialize(aMS);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aSx=aItM.Key();
    aType=aSx.ShapeType();
    //
    // 4.1 .myImagesResult
    bHasImage=myImages.IsBound(aSx); 
    //
    BOPCol_ListOfShape aLSx;
    if (!bHasImage) {
      if (myMapShape.Contains(aSx)) {
	aLSx.Append(aSx);
	myImagesResult.Add(aSx, aLSx);
      }
    }
    else {
      const BOPCol_ListOfShape& aLSp=myImages.Find(aSx);
      aIt.Initialize(aLSp);
      for (; aIt.More(); aIt.Next()) {
	const TopoDS_Shape& aSp=aIt.Value();
	if (myMapShape.Contains(aSp)) {
	  aLSx.Append(aSp);
	}
      }
      myImagesResult.Add(aSx, aLSx);
    }
    // <- A
    //
    // 4.2 As it was 
    if (!myHasDeleted) {
      myHasDeleted=IsDeleted(aSx);
    }
    //
    if (!myHasGenerated || !myHasModified) {
      if (aType==TopAbs_EDGE   || aType==TopAbs_FACE || 
	  aType==TopAbs_VERTEX || aType==TopAbs_SOLID) {
	if (bHasImage) {
	  const BOPCol_ListOfShape& aLSp=myImages.Find(aSx);
	  aIt.Initialize(aLSp);
	  for (; aIt.More(); aIt.Next()) {
	    const TopoDS_Shape& aSp=aIt.Value();
	    //
	    if (myMapShape.Contains(aSp)) {
	      bContainsSD=myShapesSD.IsBound(aSp);
	      //
	      if (!myHasGenerated) {
		if (bContainsSD) {
		  myHasGenerated=Standard_True;
		}
	      }
	      if (!myHasModified) {
		if (!bContainsSD) {
		  myHasModified=Standard_True;
		}
	      }
	    } // if (myMapShape.Contains(aSp))
	  }
	}
      } 
    }
  }
  myFlagHistory=Standard_True;
}

// <- A
    /*
    BOPCol_ListOfShape aLSx;
    if (!bHasImage) {
      if (myMapShape.Contains(aSx)) {
	aLSx.Append(aSx);
	myImagesResult.Add(aSx, aLSx);
      }
    }
    else {
      const BOPCol_ListOfShape& aLSp=myImages.Find(aSx);
      aIt.Initialize(aLSp);
      for (; aIt.More(); aIt.Next()) {
	const TopoDS_Shape& aSp=aIt.Value();
	if (myMapShape.Contains(aSp)) {
	  aLSx.Append(aSp);
	}
      }
      myImagesResult.Add(aSx, aLSx);
    }
    */
