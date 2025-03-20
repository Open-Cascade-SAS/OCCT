// Created on: 1993-10-22
// Created by: Laurent BOURESCHE
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

#ifndef _ChFiDS_Map_HeaderFile
#define _ChFiDS_Map_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Shape;

//! Encapsulation of IndexedDataMapOfShapeListOfShape.
class ChFiDS_Map
{
public:
  DEFINE_STANDARD_ALLOC

  //! Create an empty Map
  Standard_EXPORT ChFiDS_Map();

  //! Fills the map with the subshapes of type T1 as keys
  //! and the list of ancestors  of type T2 as items.
  Standard_EXPORT void Fill(const TopoDS_Shape&    S,
                            const TopAbs_ShapeEnum T1,
                            const TopAbs_ShapeEnum T2);

  Standard_EXPORT Standard_Boolean Contains(const TopoDS_Shape& S) const;

  Standard_EXPORT const TopTools_ListOfShape& FindFromKey(const TopoDS_Shape& S) const;

  const TopTools_ListOfShape& operator()(const TopoDS_Shape& S) const { return FindFromKey(S); }

  Standard_EXPORT const TopTools_ListOfShape& FindFromIndex(const Standard_Integer I) const;

  const TopTools_ListOfShape& operator()(const Standard_Integer I) const
  {
    return FindFromIndex(I);
  }

protected:
private:
  TopTools_IndexedDataMapOfShapeListOfShape myMap;
};

#endif // _ChFiDS_Map_HeaderFile
