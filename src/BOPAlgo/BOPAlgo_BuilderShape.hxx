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

#ifndef _BOPAlgo_BuilderShape_HeaderFile
#define _BOPAlgo_BuilderShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <Standard_Boolean.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPAlgo_Algo.hxx>
#include <BOPCol_BaseAllocator.hxx>
class TopoDS_Shape;


//! Root class for algorithms that has shape as result.<br>
//! The class provides the History mechanism, which allows
//! tracking the modification of the input shapes during
//! the operation.
class BOPAlgo_BuilderShape  : public BOPAlgo_Algo
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the result of algorithm
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  //! Returns the list of shapes generated from the
  //! shape theS.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& theS);
  
  //! Returns the list of shapes modified from the
  //! shape theS.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified (const TopoDS_Shape& theS);
  
  //! Returns true if the shape theS has been deleted.
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& theS);
  
  //! Returns true if the at least one shape(or subshape)
  //! of arguments has been deleted.
  Standard_EXPORT Standard_Boolean HasDeleted() const;
  
  //! Returns true if the at least one shape(or subshape)
  //! of arguments has generated shapes.
  Standard_EXPORT Standard_Boolean HasGenerated() const;
  
  //! Returns true if the at least one shape(or subshape)
  //! of arguments has modified shapes.
  Standard_EXPORT Standard_Boolean HasModified() const;
  
  Standard_EXPORT const BOPCol_IndexedDataMapOfShapeListOfShape& ImagesResult() const;




protected:

  
  Standard_EXPORT BOPAlgo_BuilderShape();
Standard_EXPORT virtual ~BOPAlgo_BuilderShape();
  
  Standard_EXPORT BOPAlgo_BuilderShape(const BOPCol_BaseAllocator& theAllocator);
  
  //! Prepare information for history support
  Standard_EXPORT virtual void PrepareHistory();


  TopoDS_Shape myShape;
  TopTools_ListOfShape myHistShapes;
  BOPCol_MapOfShape myMapShape;
  Standard_Boolean myHasDeleted;
  Standard_Boolean myHasGenerated;
  Standard_Boolean myHasModified;
  BOPCol_IndexedDataMapOfShapeListOfShape myImagesResult;
  Standard_Boolean myFlagHistory;


private:





};







#endif // _BOPAlgo_BuilderShape_HeaderFile
