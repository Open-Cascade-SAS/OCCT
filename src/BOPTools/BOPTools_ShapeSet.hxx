// Created by: Peter KURNEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BOPTools_ShapeSet_HeaderFile
#define _BOPTools_ShapeSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <BOPCol_MapOfOrientedShape.hxx>
#include <BOPCol_BaseAllocator.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Shape;
class TopoDS_Edge;


//! Implementation of some formal
//! opereations with a set of shapes
class BOPTools_ShapeSet 
{
public:

  DEFINE_STANDARD_ALLOC

  
    BOPTools_ShapeSet();
virtual ~BOPTools_ShapeSet();
  
    BOPTools_ShapeSet(const BOPCol_BaseAllocator& theAllocator);
  
    void SetShape (const TopoDS_Shape& theS);
  
    const TopoDS_Shape& Shape() const;
  
  Standard_EXPORT void Add (const BOPCol_ListOfShape& theLS);
  
    void Add (const TopoDS_Shape& theShape);
  
  Standard_EXPORT void Add (const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType);
  
    void AddEdge (const TopoDS_Edge& theEdge);
  
  Standard_EXPORT void AddEdges (const BOPCol_ListOfShape& theLS);
  
    void AddEdges (const TopoDS_Shape& theFace);
  
    void Subtract (const BOPTools_ShapeSet& theSet);
  void operator -= (const BOPTools_ShapeSet& theSet)
{
  Subtract(theSet);
}
  
    void Clear();
  
    void Get (BOPCol_ListOfShape& theLS) const;
  
    Standard_Boolean Contains (const BOPTools_ShapeSet& theSet) const;




protected:



  TopoDS_Shape myShape;
  BOPCol_MapOfOrientedShape myMap;


private:





};


#include <BOPTools_ShapeSet.lxx>





#endif // _BOPTools_ShapeSet_HeaderFile
