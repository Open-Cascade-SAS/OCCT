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

#ifndef _TNaming_CopyShape_HeaderFile
#define _TNaming_CopyShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
class TopoDS_Shape;
class TNaming_TranslateTool;
class TopLoc_Location;

class TNaming_CopyShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Makes  copy  a  set  of  shape(s),  using the  aMap
  Standard_EXPORT static void CopyTool(const TopoDS_Shape&                         aShape,
                                       TColStd_IndexedDataMapOfTransientTransient& aMap,
                                       TopoDS_Shape&                               aResult);

  //! Translates  a  Transient  shape(s)  to  Transient
  Standard_EXPORT static void Translate(const TopoDS_Shape&                         aShape,
                                        TColStd_IndexedDataMapOfTransientTransient& aMap,
                                        TopoDS_Shape&                               aResult,
                                        const Handle(TNaming_TranslateTool)&        TrTool);

  //! Translates a Topological  Location  to an  other  Top.
  //! Location
  Standard_EXPORT static TopLoc_Location Translate(
    const TopLoc_Location&                      L,
    TColStd_IndexedDataMapOfTransientTransient& aMap);

protected:
private:
};

#endif // _TNaming_CopyShape_HeaderFile
