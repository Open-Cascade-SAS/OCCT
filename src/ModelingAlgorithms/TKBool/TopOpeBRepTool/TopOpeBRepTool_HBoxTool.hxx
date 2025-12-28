// Created on: 1993-07-08
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_HBoxTool_HeaderFile
#define _TopOpeBRepTool_HBoxTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Bnd_Box.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Standard_Transient.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Shape;
class Bnd_Box;

class TopOpeBRepTool_HBoxTool : public Standard_Transient
{

public:
  Standard_EXPORT TopOpeBRepTool_HBoxTool();

  Standard_EXPORT void Clear();

  Standard_EXPORT void AddBoxes(const TopoDS_Shape&    S,
                                const TopAbs_ShapeEnum TS,
                                const TopAbs_ShapeEnum TA = TopAbs_SHAPE);

  Standard_EXPORT void AddBox(const TopoDS_Shape& S);

  Standard_EXPORT static void ComputeBox(const TopoDS_Shape& S, Bnd_Box& B);

  Standard_EXPORT static void ComputeBoxOnVertices(const TopoDS_Shape& S, Bnd_Box& B);

  Standard_EXPORT static void DumpB(const Bnd_Box& B);

  Standard_EXPORT const Bnd_Box& Box(const TopoDS_Shape& S);

  Standard_EXPORT const Bnd_Box& Box(const int I) const;

  Standard_EXPORT bool HasBox(const TopoDS_Shape& S) const;

  Standard_EXPORT const TopoDS_Shape& Shape(const int I) const;

  Standard_EXPORT int Index(const TopoDS_Shape& S) const;

  Standard_EXPORT int Extent() const;

  Standard_EXPORT NCollection_IndexedDataMap<TopoDS_Shape, Bnd_Box>& ChangeIMS();

  Standard_EXPORT const NCollection_IndexedDataMap<TopoDS_Shape, Bnd_Box>& IMS() const;

  DEFINE_STANDARD_RTTIEXT(TopOpeBRepTool_HBoxTool, Standard_Transient)

private:
  NCollection_IndexedDataMap<TopoDS_Shape, Bnd_Box> myIMS;
};

#endif // _TopOpeBRepTool_HBoxTool_HeaderFile
