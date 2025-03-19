// Created on: 1994-11-08
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_EdgeInterferenceTool_HeaderFile
#define _TopOpeBRepDS_EdgeInterferenceTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopAbs_Orientation.hxx>
#include <Standard_Integer.hxx>
#include <TopTrans_CurveTransition.hxx>
class TopoDS_Shape;
class TopOpeBRepDS_Interference;
class TopOpeBRepDS_Point;

//! a tool computing complex transition on Edge.
class TopOpeBRepDS_EdgeInterferenceTool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepDS_EdgeInterferenceTool();

  Standard_EXPORT void Init(const TopoDS_Shape& E, const Handle(TopOpeBRepDS_Interference)& I);

  Standard_EXPORT void Add(const TopoDS_Shape&                      E,
                           const TopoDS_Shape&                      V,
                           const Handle(TopOpeBRepDS_Interference)& I);

  Standard_EXPORT void Add(const TopoDS_Shape&                      E,
                           const TopOpeBRepDS_Point&                P,
                           const Handle(TopOpeBRepDS_Interference)& I);

  Standard_EXPORT void Transition(const Handle(TopOpeBRepDS_Interference)& I) const;

protected:
private:
  TopAbs_Orientation       myEdgeOrientation;
  Standard_Integer         myEdgeOriented;
  TopTrans_CurveTransition myTool;
};

#endif // _TopOpeBRepDS_EdgeInterferenceTool_HeaderFile
