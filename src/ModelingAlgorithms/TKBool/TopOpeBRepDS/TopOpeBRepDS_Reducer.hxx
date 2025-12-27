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

#ifndef _TopOpeBRepDS_Reducer_HeaderFile
#define _TopOpeBRepDS_Reducer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
class TopOpeBRepDS_HDataStructure;

//! reduce interferences of a data structure (HDS)
//! used in topological operations.
class TopOpeBRepDS_Reducer
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepDS_Reducer(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);

  Standard_EXPORT void ProcessFaceInterferences(
    const NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>& M);

  Standard_EXPORT void ProcessEdgeInterferences();

private:
  occ::handle<TopOpeBRepDS_HDataStructure> myHDS;
};

#endif // _TopOpeBRepDS_Reducer_HeaderFile
