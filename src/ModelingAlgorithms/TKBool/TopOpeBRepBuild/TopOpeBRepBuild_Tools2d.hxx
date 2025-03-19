// Created on: 1999-11-29
// Created by: Peter KURNEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TopOpeBRepBuild_Tools2d_HeaderFile
#define _TopOpeBRepBuild_Tools2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Wire;

class TopOpeBRepBuild_Tools2d
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void MakeMapOfShapeVertexInfo(
    const TopoDS_Wire&                               aWire,
    TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo& aMap);

  Standard_EXPORT static void DumpMapOfShapeVertexInfo(
    const TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo& aMap);

  Standard_EXPORT static void Path(const TopoDS_Wire& aWire, TopTools_ListOfShape& aResList);

protected:
private:
};

#endif // _TopOpeBRepBuild_Tools2d_HeaderFile
