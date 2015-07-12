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

#ifndef _BOPAlgo_BOP_HeaderFile
#define _BOPAlgo_BOP_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_Operation.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPAlgo_Builder.hxx>
#include <BOPCol_BaseAllocator.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Boolean.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
class TopoDS_Shape;
class BOPAlgo_PaveFiller;



class BOPAlgo_BOP  : public BOPAlgo_Builder
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT BOPAlgo_BOP();
Standard_EXPORT virtual ~BOPAlgo_BOP();
  
  Standard_EXPORT BOPAlgo_BOP(const BOPCol_BaseAllocator& theAllocator);
  
  //! Clears internal fields and arguments
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;
  
  //! Adds Tool argument of the operation
  Standard_EXPORT virtual void AddTool (const TopoDS_Shape& theShape);
  
  Standard_EXPORT virtual void SetTools (const BOPCol_ListOfShape& theShapes);
  
  Standard_EXPORT void SetOperation (const BOPAlgo_Operation theOperation);
  
  Standard_EXPORT BOPAlgo_Operation Operation() const;
  
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;




protected:

  
  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;
  
  //! Provides preparing actions
  Standard_EXPORT virtual void Prepare() Standard_OVERRIDE;
  
  //! Performs calculations using prepared Filler
  //! object <thePF>
  Standard_EXPORT virtual void PerformInternal1 (const BOPAlgo_PaveFiller& thePF) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void BuildResult (const TopAbs_ShapeEnum theType) Standard_OVERRIDE;
  
  Standard_EXPORT void BuildShape();
  
  Standard_EXPORT void BuildRC();
  
  Standard_EXPORT void BuildSolid();
  
  Standard_EXPORT Standard_Boolean IsBoundSplits (const TopoDS_Shape& theS, BOPCol_IndexedDataMapOfShapeListOfShape& theMEF);


  BOPAlgo_Operation myOperation;
  Standard_Integer myDims[2];
  TopoDS_Shape myRC;
  BOPCol_ListOfShape myTools;
  BOPCol_MapOfShape myMapTools;


private:





};







#endif // _BOPAlgo_BOP_HeaderFile
