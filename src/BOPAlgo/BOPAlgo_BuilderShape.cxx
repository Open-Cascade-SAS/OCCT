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


#include <BOPAlgo_BuilderShape.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPAlgo_BuilderShape::BOPAlgo_BuilderShape()
:
  BOPAlgo_Algo()
{
  myHasDeleted=Standard_False;
  myHasGenerated=Standard_False;
  myHasModified=Standard_False;
  myFlagHistory=Standard_False;
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPAlgo_BuilderShape::BOPAlgo_BuilderShape(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Algo(theAllocator)
{
  myHasDeleted=Standard_False;
  myHasGenerated=Standard_False;
  myHasModified=Standard_False;
  myFlagHistory=Standard_False;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  BOPAlgo_BuilderShape::~BOPAlgo_BuilderShape()
{
}
//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BOPAlgo_BuilderShape::Shape() const
{
  return myShape;
}
//
//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& BOPAlgo_BuilderShape::Generated(const TopoDS_Shape& )
{
  myHistShapes.Clear();
  return myHistShapes;
}
//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& BOPAlgo_BuilderShape::Modified(const TopoDS_Shape& )
{
  myHistShapes.Clear();
  return myHistShapes;
}
//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_BuilderShape::IsDeleted(const TopoDS_Shape& theS)
{
  Standard_Boolean bRet;
  //
  bRet=!myMapShape.Contains(theS);
  return bRet;  
}
//=======================================================================
//function : HasDeleted
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_BuilderShape::HasDeleted()const
{
  return myHasDeleted;
}
//=======================================================================
//function : HasGenerated
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_BuilderShape::HasGenerated()const
{
  return myHasGenerated;
}
//=======================================================================
//function : HasModified
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_BuilderShape::HasModified()const
{
  return myHasModified;
}
//=======================================================================
//function : PrepareHistory
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderShape::PrepareHistory()
{
  myHistShapes.Clear();
  myMapShape.Clear();
  myImagesResult.Clear();
  myHasDeleted=Standard_False;
  myHasGenerated=Standard_False;
  myHasModified=Standard_False;
  myFlagHistory=Standard_False;
}

//=======================================================================
//function : ImagesResult
//purpose  : 
//=======================================================================
  const TopTools_IndexedDataMapOfShapeListOfShape& 
    BOPAlgo_BuilderShape::ImagesResult()const
{
  return myImagesResult;
}

