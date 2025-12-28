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

#ifndef _TopOpeBRepDS_ListOfShapeOn1State_HeaderFile
#define _TopOpeBRepDS_ListOfShapeOn1State_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <Standard_Boolean.hxx>

//! represent a list of shape
class TopOpeBRepDS_ListOfShapeOn1State
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepDS_ListOfShapeOn1State();

  Standard_EXPORT const NCollection_List<TopoDS_Shape>& ListOnState() const;

  Standard_EXPORT NCollection_List<TopoDS_Shape>& ChangeListOnState();

  Standard_EXPORT bool IsSplit() const;

  Standard_EXPORT void Split(const bool B = true);

  Standard_EXPORT void Clear();

private:
  NCollection_List<TopoDS_Shape> myList;
  int                            mySplits;
};

#endif // _TopOpeBRepDS_ListOfShapeOn1State_HeaderFile
