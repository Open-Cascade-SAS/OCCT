// Created on: 1999-01-14
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

#ifndef _TopOpeBRepTool_face_HeaderFile
#define _TopOpeBRepTool_face_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>

class TopOpeBRepTool_face
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepTool_face();

  Standard_EXPORT Standard_Boolean Init(const TopoDS_Wire& W, const TopoDS_Face& Fref);

  Standard_EXPORT const TopoDS_Wire& W() const;

  Standard_EXPORT Standard_Boolean IsDone() const;

  Standard_EXPORT Standard_Boolean Finite() const;

  Standard_EXPORT const TopoDS_Face& Ffinite() const;

  Standard_EXPORT TopoDS_Face RealF() const;

protected:
private:
  TopoDS_Wire      myW;
  Standard_Boolean myfinite;
  TopoDS_Face      myFfinite;
};

#endif // _TopOpeBRepTool_face_HeaderFile
