// Created on: 1997-05-06
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

#ifndef _TNaming_IteratorOnShapesSet_HeaderFile
#define _TNaming_IteratorOnShapesSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <Standard_Boolean.hxx>
class Standard_NoMoreObject;
class Standard_NoSuchObject;
class TNaming_ShapesSet;
class TopoDS_Shape;

class TNaming_IteratorOnShapesSet
{
public:
  DEFINE_STANDARD_ALLOC

  TNaming_IteratorOnShapesSet();

  TNaming_IteratorOnShapesSet(const TNaming_ShapesSet& S);

  //! Initialize the iteration
  void Init(const TNaming_ShapesSet& S);

  //! Returns True if there is a current Item in
  //! the iteration.
  Standard_Boolean More() const;

  //! Move to the next Item
  void Next();

  const TopoDS_Shape& Value() const;

protected:
private:
  TopTools_MapIteratorOfMapOfShape myIt;
};

#include <TNaming_IteratorOnShapesSet.lxx>

#endif // _TNaming_IteratorOnShapesSet_HeaderFile
