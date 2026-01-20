// Created on: 1999-01-25
// Created by: Xuan PHAM PHU
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

#ifndef _TopOpeBRepDS_TOOL_HeaderFile
#define _TopOpeBRepDS_TOOL_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
class TopOpeBRepDS_HDataStructure;
class TopoDS_Edge;
class TopoDS_Shape;

class TopOpeBRepDS_TOOL
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static int EShareG(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                                  const TopoDS_Edge&                         E,
                                                  NCollection_List<TopoDS_Shape>&                      lEsd);

  Standard_EXPORT static bool ShareG(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                                 const int                     is1,
                                                 const int                     is2);

  Standard_EXPORT static bool GetEsd(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                                 const TopoDS_Shape&                        S,
                                                 const int                     ie,
                                                 int&                          iesd);

  Standard_EXPORT static bool ShareSplitON(
    const occ::handle<TopOpeBRepDS_HDataStructure>&            HDS,
    const NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>& MspON,
    const int                                i1,
    const int                                i2,
    TopoDS_Shape&                                         spON);

  Standard_EXPORT static bool GetConfig(
    const occ::handle<TopOpeBRepDS_HDataStructure>&            HDS,
    const NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>& MEspON,
    const int                                ie,
    const int                                iesd,
    int&                                     conf);

};

#endif // _TopOpeBRepDS_TOOL_HeaderFile
