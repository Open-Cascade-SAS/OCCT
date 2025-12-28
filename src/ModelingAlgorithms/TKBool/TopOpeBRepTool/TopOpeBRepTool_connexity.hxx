// Created on: 1998-12-09
// Created by: Xuan PHAM PHU
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_connexity_HeaderFile
#define _TopOpeBRepTool_connexity_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Array1.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <Standard_Boolean.hxx>

class TopOpeBRepTool_connexity
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepTool_connexity();

  Standard_EXPORT TopOpeBRepTool_connexity(const TopoDS_Shape& Key);

  Standard_EXPORT void SetKey(const TopoDS_Shape& Key);

  Standard_EXPORT const TopoDS_Shape& Key() const;

  Standard_EXPORT int Item(const int OriKey,
                                        NCollection_List<TopoDS_Shape>&  Item) const;

  Standard_EXPORT int AllItems(NCollection_List<TopoDS_Shape>& Item) const;

  Standard_EXPORT void AddItem(const int OriKey, const NCollection_List<TopoDS_Shape>& Item);

  Standard_EXPORT void AddItem(const int OriKey, const TopoDS_Shape& Item);

  Standard_EXPORT bool RemoveItem(const int OriKey,
                                              const TopoDS_Shape&    Item);

  Standard_EXPORT bool RemoveItem(const TopoDS_Shape& Item);

  Standard_EXPORT NCollection_List<TopoDS_Shape>& ChangeItem(const int OriKey);

  Standard_EXPORT bool IsMultiple() const;

  Standard_EXPORT bool IsFaulty() const;

  Standard_EXPORT int IsInternal(NCollection_List<TopoDS_Shape>& Item) const;

private:
  TopoDS_Shape                 theKey;
  NCollection_Array1<NCollection_List<TopoDS_Shape>> theItems;
};

#endif // _TopOpeBRepTool_connexity_HeaderFile
