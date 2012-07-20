// Created on: 2012-06-27
// Created by: Dmitry BOBYLEV
// Copyright (c) 2012 OPEN CASCADE SAS
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

#include <TopTools_MutexForShapeProvider.hxx>

#include <Standard_Mutex.hxx>
#include <TopoDS_Iterator.hxx>


// macro to compare two types of shapes
#define SAMETYPE(x,y) ((x) == (y))
#define LESSCOMPLEX(x,y) ((x) > (y))

//=======================================================================
//function : TopTools_MutexForShapeProvider
//purpose  : 
//=======================================================================
TopTools_MutexForShapeProvider::TopTools_MutexForShapeProvider()
{
}


//=======================================================================
//function : ~TopTools_MutexForShapeProvider
//purpose  : 
//=======================================================================
TopTools_MutexForShapeProvider::~TopTools_MutexForShapeProvider()
{
  RemoveAllMutexes();
}

//=======================================================================
//function : CreateMutexesForSubShapes
//purpose  : 
//=======================================================================
void TopTools_MutexForShapeProvider::CreateMutexesForSubShapes(const TopoDS_Shape& theShape,
                                                               const TopAbs_ShapeEnum theType)
{
  if (LESSCOMPLEX(theShape.ShapeType(), theType))
    return;

  for(TopoDS_Iterator anIt(theShape); anIt.More(); anIt.Next())
  {
    const TopoDS_Shape& aShape = anIt.Value();
    if (LESSCOMPLEX(theType, aShape.ShapeType()))
    {
      CreateMutexesForSubShapes(aShape, theType);
    }
    else if (SAMETYPE(theType, aShape.ShapeType()))
    {
      CreateMutexForShape(aShape);
    }
  }
}

//=======================================================================
//function : CreateMutexForShape
//purpose  : 
//=======================================================================
void TopTools_MutexForShapeProvider::CreateMutexForShape(const TopoDS_Shape& theShape)
{
  if (!myMap.IsBound(theShape.TShape()))
  {
    Standard_Mutex* aMutex = new Standard_Mutex();
    myMap.Bind(theShape.TShape(), aMutex);
  }
}

//=======================================================================
//function : CreateMutexForShape
//purpose  : 
//=======================================================================
Standard_Mutex* TopTools_MutexForShapeProvider::GetMutex(const TopoDS_Shape& theShape) const
{
  if (myMap.IsBound(theShape.TShape()))
  {
    Standard_Mutex* aMutex = myMap.Find(theShape.TShape());
    return aMutex;
  }
  else
  {
    return NULL;
  }
}

//=======================================================================
//function : RemoveAllMutexes
//purpose  : 
//=======================================================================
void TopTools_MutexForShapeProvider::RemoveAllMutexes()
{
  for (NCollection_DataMap<TopoDS_Shape, Standard_Mutex *>::Iterator anIter;
         anIter.More(); anIter.Next())
  {
    delete anIter.Value();
  }
  myMap.Clear();
}
