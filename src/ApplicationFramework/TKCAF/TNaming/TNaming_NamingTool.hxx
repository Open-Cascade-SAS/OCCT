// Created on: 2000-02-14
// Created by: Denis PASCAL
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _TNaming_NamingTool_HeaderFile
#define _TNaming_NamingTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_Label.hxx>
#include <NCollection_Map.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
class TNaming_NamedShape;
class TDF_Label;
class TopoDS_Shape;

class TNaming_NamingTool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void CurrentShape(const NCollection_Map<TDF_Label>&               Valid,
                                           const NCollection_Map<TDF_Label>&               Forbiden,
                                           const occ::handle<TNaming_NamedShape>& NS,
                                           NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       MS);

  Standard_EXPORT static void CurrentShapeFromShape(const NCollection_Map<TDF_Label>&         Valid,
                                                    const NCollection_Map<TDF_Label>&         Forbiden,
                                                    const TDF_Label&            Acces,
                                                    const TopoDS_Shape&         S,
                                                    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& MS);

  Standard_EXPORT static void BuildDescendants(const occ::handle<TNaming_NamedShape>& NS,
                                               NCollection_Map<TDF_Label>&                     Labels);

};

#endif // _TNaming_NamingTool_HeaderFile
