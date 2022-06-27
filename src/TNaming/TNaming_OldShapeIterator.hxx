// Created on: 1996-12-16
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _TNaming_OldShapeIterator_HeaderFile
#define _TNaming_OldShapeIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TNaming_PtrNode.hxx>
#include <Standard_Integer.hxx>
class Standard_NoMoreObject;
class Standard_NoSuchObject;
class TNaming_Tool;
class TNaming_Localizer;
class TNaming_Naming;
class TopoDS_Shape;
class TNaming_UsedShapes;
class TDF_Label;
class TNaming_Iterator;
class TNaming_NamedShape;


//! Iterates on all the ascendants of a shape
class TNaming_OldShapeIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TNaming_OldShapeIterator(const TopoDS_Shape& aShape, const Standard_Integer Transaction, const TDF_Label& access);
  
  Standard_EXPORT TNaming_OldShapeIterator(const TopoDS_Shape& aShape, const TDF_Label& access);
  
  //! Iterates from the current Shape in <anIterator>
  Standard_EXPORT TNaming_OldShapeIterator(const TNaming_OldShapeIterator& anIterator);
  
  //! Iterates from the current Shape in <anIterator>
  Standard_EXPORT TNaming_OldShapeIterator(const TNaming_Iterator& anIterator);
  
    Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT TDF_Label Label() const;
  
  Standard_EXPORT Handle(TNaming_NamedShape) NamedShape() const;
  
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  //! True if the  new  shape is a modification  (split,
  //! fuse,etc...) of the old shape.
  Standard_EXPORT Standard_Boolean IsModification() const;


friend class TNaming_Tool;
friend class TNaming_Localizer;
friend class TNaming_Naming;


protected:





private:

  
  Standard_EXPORT TNaming_OldShapeIterator(const TopoDS_Shape& aShape, const Standard_Integer Transaction, const Handle(TNaming_UsedShapes)& Shapes);
  
  Standard_EXPORT TNaming_OldShapeIterator(const TopoDS_Shape& aShape, const Handle(TNaming_UsedShapes)& Shapes);


  TNaming_PtrNode myNode;
  Standard_Integer myTrans;


};


#include <TNaming_OldShapeIterator.lxx>





#endif // _TNaming_OldShapeIterator_HeaderFile
