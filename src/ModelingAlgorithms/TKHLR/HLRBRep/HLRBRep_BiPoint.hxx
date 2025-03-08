// Created on: 1992-08-21
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRBRep_BiPoint_HeaderFile
#define _HLRBRep_BiPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
class TopoDS_Shape;
class gp_Pnt;

//! Contains the colors of a shape.
class HLRBRep_BiPoint
{
public:
  DEFINE_STANDARD_ALLOC

  HLRBRep_BiPoint()
      : myRg1Line(false),
        myRgNLine(false),
        myOutLine(false),
        myIntLine(false)
  {
  }

  HLRBRep_BiPoint(const Standard_Real    x1,
                  const Standard_Real    y1,
                  const Standard_Real    z1,
                  const Standard_Real    x2,
                  const Standard_Real    y2,
                  const Standard_Real    z2,
                  const TopoDS_Shape&    S,
                  const Standard_Boolean reg1,
                  const Standard_Boolean regn,
                  const Standard_Boolean outl,
                  const Standard_Boolean intl)
      : myP1(x1, y1, z1),
        myP2(x2, y2, z2),
        myShape(S),
        myRg1Line(reg1),
        myRgNLine(regn),
        myOutLine(outl),
        myIntLine(intl)
  {
  }

  const gp_Pnt& P1() const { return myP1; }

  const gp_Pnt& P2() const { return myP2; }

  const TopoDS_Shape& Shape() const { return myShape; }

  void Shape(const TopoDS_Shape& S) { myShape = S; }

  Standard_Boolean Rg1Line() const { return myRg1Line; }

  void Rg1Line(const Standard_Boolean B) { myRg1Line = B; }

  Standard_Boolean RgNLine() const { return myRgNLine; }

  void RgNLine(const Standard_Boolean B) { myRgNLine = B; }

  Standard_Boolean OutLine() const { return myOutLine; }

  void OutLine(const Standard_Boolean B) { myOutLine = B; }

  Standard_Boolean IntLine() const { return myIntLine; }

  void IntLine(const Standard_Boolean B) { myIntLine = B; }

private:
  gp_Pnt           myP1;
  gp_Pnt           myP2;
  TopoDS_Shape     myShape;
  Standard_Boolean myRg1Line;
  Standard_Boolean myRgNLine;
  Standard_Boolean myOutLine;
  Standard_Boolean myIntLine;
};

#endif // _HLRBRep_BiPoint_HeaderFile
