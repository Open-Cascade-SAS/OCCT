// Created on: 1996-12-17
// Created by: Yves FRICAUD
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

#ifndef _TNaming_RefShape_HeaderFile
#define _TNaming_RefShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TNaming_PtrNode.hxx>
class TopoDS_Shape;
class TDF_Label;
class TNaming_NamedShape;

class TNaming_RefShape
{
public:
  DEFINE_STANDARD_ALLOC

  TNaming_RefShape();

  TNaming_RefShape(const TopoDS_Shape& S);

  void Shape(const TopoDS_Shape& S);

  void FirstUse(const TNaming_PtrNode& aPtr);

  TNaming_PtrNode FirstUse() const;

  const TopoDS_Shape& Shape() const;

  Standard_EXPORT TDF_Label Label() const;

  Standard_EXPORT Handle(TNaming_NamedShape) NamedShape() const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

protected:
private:
  TopoDS_Shape    myShape;
  TNaming_PtrNode myFirstUse;
};

#include <TNaming_RefShape.lxx>

#endif // _TNaming_RefShape_HeaderFile
