// Created on: 2012-06-27
// Created by: Dmitry BOBYLEV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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
