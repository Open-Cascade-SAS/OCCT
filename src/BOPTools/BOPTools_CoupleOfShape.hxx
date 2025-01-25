// Created by: Peter KURNEV
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

#ifndef BOPTools_CoupleOfShape_HeaderFile
#define BOPTools_CoupleOfShape_HeaderFile

#include <TopoDS_Shape.hxx>

//=======================================================================
// class :
// purpose  :
//=======================================================================
class BOPTools_CoupleOfShape
{
public:
  BOPTools_CoupleOfShape() {};
  //
  ~BOPTools_CoupleOfShape() {};

  //
  void SetShape1(const TopoDS_Shape& theShape) { myShape1 = theShape; }

  //
  const TopoDS_Shape& Shape1() const { return myShape1; }

  //
  void SetShape2(const TopoDS_Shape& theShape) { myShape2 = theShape; }

  //
  const TopoDS_Shape& Shape2() const { return myShape2; }

  //
protected:
  TopoDS_Shape myShape1;
  TopoDS_Shape myShape2;
};

#endif
