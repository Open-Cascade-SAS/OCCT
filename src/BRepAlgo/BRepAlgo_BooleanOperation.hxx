// Created on: 1993-10-14
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepAlgo_BooleanOperation_HeaderFile
#define _BRepAlgo_BooleanOperation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <TopAbs_State.hxx>
#include <TopTools_ListOfShape.hxx>
class TopOpeBRepBuild_HBuilder;
class TopoDS_Shape;


//! The abstract class BooleanOperation is the root
//! class of Boolean operations.
//! A BooleanOperation object stores the two shapes in
//! preparation for the Boolean operation specified in
//! one of the classes inheriting from this one. These include:
//! - Common
//! - Cut
//! - Fuse
//! - Section.
class BRepAlgo_BooleanOperation  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT virtual ~BRepAlgo_BooleanOperation();
  
  Standard_EXPORT void PerformDS();
  
  Standard_EXPORT void Perform (const TopAbs_State St1, const TopAbs_State St2);
  
  Standard_EXPORT Handle(TopOpeBRepBuild_HBuilder) Builder() const;
  
  //! Returns the first shape involved in this Boolean operation.
  Standard_EXPORT const TopoDS_Shape& Shape1() const;
  
  //! Returns the second shape involved in this Boolean operation.
  Standard_EXPORT const TopoDS_Shape& Shape2() const;
  
  //! Returns the list  of shapes modified from the shape
  //! <S>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified (const TopoDS_Shape& S) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& S) Standard_OVERRIDE;




protected:

  
  //! Prepares the operations for S1 and S2.
  Standard_EXPORT BRepAlgo_BooleanOperation(const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  Standard_EXPORT void BuilderCanWork (const Standard_Boolean B);
  
  Standard_EXPORT Standard_Boolean BuilderCanWork() const;
  
  Standard_EXPORT virtual void InitParameters();


  Handle(TopOpeBRepBuild_HBuilder) myHBuilder;
  TopoDS_Shape myS1;
  TopoDS_Shape myS2;


private:



  TopTools_MapOfShape myMap;
  Standard_Boolean myBuilderCanWork;
  TopTools_DataMapOfShapeShape topToSew;


};







#endif // _BRepAlgo_BooleanOperation_HeaderFile
