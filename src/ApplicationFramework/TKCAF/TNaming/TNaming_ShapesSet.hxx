// Created on: 1997-01-09
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

#ifndef _TNaming_ShapesSet_HeaderFile
#define _TNaming_ShapesSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Shape;

class TNaming_ShapesSet
{
public:
  DEFINE_STANDARD_ALLOC

  TNaming_ShapesSet();

  Standard_EXPORT TNaming_ShapesSet(const TopoDS_Shape&    S,
                                    const TopAbs_ShapeEnum Type = TopAbs_SHAPE);

  //! Removes all Shapes
  void Clear();

  //! Adds the Shape <S>
  Standard_Boolean Add(const TopoDS_Shape& S);

  //! Returns True  if <S> is in <me>
  Standard_Boolean Contains(const TopoDS_Shape& S) const;

  //! Removes <S> in <me>.
  Standard_Boolean Remove(const TopoDS_Shape& S);

  //! Adds the shapes contained in <Shapes>.
  Standard_EXPORT void Add(const TNaming_ShapesSet& Shapes);

  //! Erases in <me> the shapes not
  //! contained in <Shapes>
  Standard_EXPORT void Filter(const TNaming_ShapesSet& Shapes);

  //! Removes in <me> the shapes contained in <Shapes>
  Standard_EXPORT void Remove(const TNaming_ShapesSet& Shapes);

  Standard_Boolean IsEmpty() const;

  Standard_Integer NbShapes() const;

  TopTools_MapOfShape& ChangeMap();

  const TopTools_MapOfShape& Map() const;

protected:
private:
  TopTools_MapOfShape myMap;
};

#include <TNaming_ShapesSet.lxx>

#endif // _TNaming_ShapesSet_HeaderFile
