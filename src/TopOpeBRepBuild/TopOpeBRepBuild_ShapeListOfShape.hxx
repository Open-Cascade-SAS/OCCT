// Created on: 1995-06-12
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _TopOpeBRepBuild_ShapeListOfShape_HeaderFile
#define _TopOpeBRepBuild_ShapeListOfShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Shape.hxx>


//! represent shape + a list of shape
class TopOpeBRepBuild_ShapeListOfShape 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_ShapeListOfShape();
  
  Standard_EXPORT TopOpeBRepBuild_ShapeListOfShape(const TopoDS_Shape& S);
  
  Standard_EXPORT TopOpeBRepBuild_ShapeListOfShape(const TopoDS_Shape& S, const TopTools_ListOfShape& L);
  
  Standard_EXPORT const TopTools_ListOfShape& List() const;
  
  Standard_EXPORT TopTools_ListOfShape& ChangeList();
  
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  Standard_EXPORT TopoDS_Shape& ChangeShape();




protected:





private:



  TopTools_ListOfShape myList;
  TopoDS_Shape myShape;


};







#endif // _TopOpeBRepBuild_ShapeListOfShape_HeaderFile
