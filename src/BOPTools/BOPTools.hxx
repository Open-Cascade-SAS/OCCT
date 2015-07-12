// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _BOPTools_HeaderFile
#define _BOPTools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_IndexedMapOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
class TopoDS_Shape;
class BOPTools_ShapeSet;
class BOPTools_EdgeSet;
class BOPTools_AlgoTools;
class BOPTools_Set;
class BOPTools_SetMapHasher;
class BOPTools_AlgoTools2D;
class BOPTools_AlgoTools3D;



class BOPTools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void MapShapes (const TopoDS_Shape& S, BOPCol_MapOfShape& M);
  
  Standard_EXPORT static void MapShapes (const TopoDS_Shape& S, BOPCol_IndexedMapOfShape& M);
  
  Standard_EXPORT static void MapShapes (const TopoDS_Shape& S, const TopAbs_ShapeEnum T, BOPCol_IndexedMapOfShape& M);
  
  Standard_EXPORT static void MapShapesAndAncestors (const TopoDS_Shape& S, const TopAbs_ShapeEnum TS, const TopAbs_ShapeEnum TA, BOPCol_IndexedDataMapOfShapeListOfShape& M);




protected:





private:




friend class BOPTools_ShapeSet;
friend class BOPTools_EdgeSet;
friend class BOPTools_AlgoTools;
friend class BOPTools_Set;
friend class BOPTools_SetMapHasher;
friend class BOPTools_AlgoTools2D;
friend class BOPTools_AlgoTools3D;

};







#endif // _BOPTools_HeaderFile
