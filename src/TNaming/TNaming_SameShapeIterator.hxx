// Created on: 1997-04-24
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TNaming_SameShapeIterator_HeaderFile
#define _TNaming_SameShapeIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TNaming_PtrNode.hxx>
#include <Standard_Boolean.hxx>
class Standard_NoMoreObject;
class Standard_NoSuchObject;
class TNaming_Tool;
class TopoDS_Shape;
class TNaming_UsedShapes;
class TDF_Label;


//! To iterate on   all  the label which contained  a
//! given shape.
class TNaming_SameShapeIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TNaming_SameShapeIterator(const TopoDS_Shape& aShape, const TDF_Label& access);
  
    Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT TDF_Label Label() const;


friend class TNaming_Tool;


protected:





private:

  
  Standard_EXPORT TNaming_SameShapeIterator(const TopoDS_Shape& aShape, const Handle(TNaming_UsedShapes)& Shapes);


  TNaming_PtrNode myNode;
  Standard_Boolean myIsNew;


};


#include <TNaming_SameShapeIterator.lxx>





#endif // _TNaming_SameShapeIterator_HeaderFile
