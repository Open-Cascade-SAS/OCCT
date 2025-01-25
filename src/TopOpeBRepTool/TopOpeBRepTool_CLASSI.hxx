// Created on: 1999-01-13
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

#ifndef _TopOpeBRepTool_CLASSI_HeaderFile
#define _TopOpeBRepTool_CLASSI_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Face.hxx>
#include <TopOpeBRepTool_IndexedDataMapOfShapeBox2d.hxx>
#include <TopOpeBRepTool_DataMapOfShapeface.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
class TopoDS_Shape;
class Bnd_Box2d;
class TopOpeBRepTool_face;

class TopOpeBRepTool_CLASSI
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepTool_CLASSI();

  Standard_EXPORT void Init2d(const TopoDS_Face& Fref);

  Standard_EXPORT Standard_Boolean HasInit2d() const;

  Standard_EXPORT Standard_Boolean Add2d(const TopoDS_Shape& S);

  Standard_EXPORT Standard_Boolean GetBox2d(const TopoDS_Shape& S, Bnd_Box2d& Box2d);

  Standard_EXPORT Standard_Integer ClassiBnd2d(const TopoDS_Shape&    S1,
                                               const TopoDS_Shape&    S2,
                                               const Standard_Real    tol,
                                               const Standard_Boolean checklarge);

  Standard_EXPORT Standard_Integer Classip2d(const TopoDS_Shape&    S1,
                                             const TopoDS_Shape&    S2,
                                             const Standard_Integer stabnd2d12);

  Standard_EXPORT Standard_Boolean Getface(const TopoDS_Shape& S, TopOpeBRepTool_face& fa) const;

  Standard_EXPORT Standard_Boolean Classilist(const TopTools_ListOfShape&         lS,
                                              TopTools_DataMapOfShapeListOfShape& mapgreasma);

protected:
private:
  TopoDS_Face                               myFref;
  TopOpeBRepTool_IndexedDataMapOfShapeBox2d mymapsbox2d;
  TopOpeBRepTool_DataMapOfShapeface         mymapsface;
};

#endif // _TopOpeBRepTool_CLASSI_HeaderFile
