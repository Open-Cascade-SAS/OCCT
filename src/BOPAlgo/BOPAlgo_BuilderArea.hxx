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

#ifndef _BOPAlgo_BuilderArea_HeaderFile
#define _BOPAlgo_BuilderArea_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_IndexedMapOfOrientedShape.hxx>
#include <BOPAlgo_Algo.hxx>
#include <BOPCol_BaseAllocator.hxx>
class IntTools_Context;


//! The root class for algorithms to build
//! faces/solids from set of edges/faces
class BOPAlgo_BuilderArea  : public BOPAlgo_Algo
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT void SetContext (const Handle(IntTools_Context)& theContext);
  
  Standard_EXPORT const BOPCol_ListOfShape& Shapes() const;
Standard_EXPORT void SetShapes(const BOPCol_ListOfShape& theLS);
  
  Standard_EXPORT const BOPCol_ListOfShape& Loops() const;
  
  Standard_EXPORT const BOPCol_ListOfShape& Areas() const;




protected:

  
  Standard_EXPORT BOPAlgo_BuilderArea();
Standard_EXPORT virtual ~BOPAlgo_BuilderArea();
  
  Standard_EXPORT BOPAlgo_BuilderArea(const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT virtual void PerformShapesToAvoid() = 0;
  
  Standard_EXPORT virtual void PerformLoops() = 0;
  
  Standard_EXPORT virtual void PerformAreas() = 0;
  
  Standard_EXPORT virtual void PerformInternalShapes() = 0;


  Handle(IntTools_Context) myContext;
  BOPCol_ListOfShape myShapes;
  BOPCol_ListOfShape myLoops;
  BOPCol_ListOfShape myLoopsInternal;
  BOPCol_ListOfShape myAreas;
  BOPCol_IndexedMapOfOrientedShape myShapesToAvoid;


private:





};







#endif // _BOPAlgo_BuilderArea_HeaderFile
