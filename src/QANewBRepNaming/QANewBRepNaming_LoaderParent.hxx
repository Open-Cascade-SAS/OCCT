// Created on: 2003-05-21
// Created by: Open CASCADE Support
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _QANewBRepNaming_LoaderParent_HeaderFile
#define _QANewBRepNaming_LoaderParent_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
class TopoDS_Shape;
class TNaming_Builder;



class QANewBRepNaming_LoaderParent 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void LoadGeneratedDangleShapes (const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum GeneratedFrom, TNaming_Builder& GenBuider);
  
  Standard_EXPORT static Standard_Boolean GetDangleShapes (const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum GeneratedFrom, TopTools_DataMapOfShapeShape& Dangles);




protected:





private:





};







#endif // _QANewBRepNaming_LoaderParent_HeaderFile
