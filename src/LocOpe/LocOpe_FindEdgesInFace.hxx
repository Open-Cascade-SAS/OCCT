// Created on: 1996-02-15
// Created by: Jacques GOUSSARD
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

#ifndef _LocOpe_FindEdgesInFace_HeaderFile
#define _LocOpe_FindEdgesInFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
class TopoDS_Edge;

class LocOpe_FindEdgesInFace
{
public:
  DEFINE_STANDARD_ALLOC

  LocOpe_FindEdgesInFace();

  LocOpe_FindEdgesInFace(const TopoDS_Shape& S, const TopoDS_Face& F);

  Standard_EXPORT void Set(const TopoDS_Shape& S, const TopoDS_Face& F);

  void Init();

  Standard_Boolean More() const;

  const TopoDS_Edge& Edge() const;

  void Next();

protected:
private:
  TopoDS_Shape                       myShape;
  TopoDS_Face                        myFace;
  TopTools_ListOfShape               myList;
  TopTools_ListIteratorOfListOfShape myIt;
};

#include <LocOpe_FindEdgesInFace.lxx>

#endif // _LocOpe_FindEdgesInFace_HeaderFile
