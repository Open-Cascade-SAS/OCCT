// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BOPAlgo_BuilderShape.ixx>
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
  myHasDeleted=Standard_False;
  myHasGenerated=Standard_False;
  myHasModified=Standard_False;
  myFlagHistory=Standard_False;
}

//=======================================================================
//function : ImagesResult
//purpose  : 
//=======================================================================
  const BOPCol_IndexedDataMapOfShapeListOfShape& 
    BOPAlgo_BuilderShape::ImagesResult()const
{
  return myImagesResult;
}

