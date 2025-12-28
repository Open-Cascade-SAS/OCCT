// Created on: 1996-09-04
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

#ifndef _LocOpe_Pipe_HeaderFile
#define _LocOpe_Pipe_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepFill_Pipe.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Pnt.hxx>
class TopoDS_Wire;
class Geom_Curve;

//! Defines a pipe (near from Pipe from BRepFill),
//! with modifications provided for the Pipe feature.
class LocOpe_Pipe
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT LocOpe_Pipe(const TopoDS_Wire& Spine, const TopoDS_Shape& Profile);

  const TopoDS_Shape& Spine() const;

  const TopoDS_Shape& Profile() const;

  const TopoDS_Shape& FirstShape() const;

  const TopoDS_Shape& LastShape() const;

  Standard_EXPORT const TopoDS_Shape& Shape() const;

  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Shapes(const TopoDS_Shape& S);

  Standard_EXPORT const NCollection_Sequence<occ::handle<Geom_Curve>>& Curves(
    const NCollection_Sequence<gp_Pnt>& Spt);

  Standard_EXPORT occ::handle<Geom_Curve> BarycCurve();

private:
  BRepFill_Pipe                                                                              myPipe;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myMap;
  TopoDS_Shape                                                                               myRes;
  NCollection_List<TopoDS_Shape>                myGShap;
  NCollection_Sequence<occ::handle<Geom_Curve>> myCrvs;
  TopoDS_Shape                                  myFirstShape;
  TopoDS_Shape                                  myLastShape;
};

#include <LocOpe_Pipe.lxx>

#endif // _LocOpe_Pipe_HeaderFile
