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

#ifndef _HLRBRep_BiPnt2D_HeaderFile
#define _HLRBRep_BiPnt2D_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Pnt2d.hxx>
#include <TopoDS_Shape.hxx>

//! Contains the colors of a shape.
class HLRBRep_BiPnt2D
{
public:
  DEFINE_STANDARD_ALLOC

  HLRBRep_BiPnt2D()
      : myRg1Line(false),
        myRgNLine(false),
        myOutLine(false),
        myIntLine(false)
  {
  }

  HLRBRep_BiPnt2D(const double        x1,
                  const double        y1,
                  const double        x2,
                  const double        y2,
                  const TopoDS_Shape& S,
                  const bool          reg1,
                  const bool          regn,
                  const bool          outl,
                  const bool          intl)
      : myP1(x1, y1),
        myP2(x2, y2),
        myShape(S),
        myRg1Line(reg1),
        myRgNLine(regn),
        myOutLine(outl),
        myIntLine(intl)
  {
  }

  HLRBRep_BiPnt2D(const gp_XY&        thePoint1,
                  const gp_XY&        thePoint2,
                  const TopoDS_Shape& S,
                  const bool          reg1,
                  const bool          regn,
                  const bool          outl,
                  const bool          intl)
      : myP1(thePoint1),
        myP2(thePoint2),
        myShape(S),
        myRg1Line(reg1),
        myRgNLine(regn),
        myOutLine(outl),
        myIntLine(intl)
  {
  }

  const gp_Pnt2d& P1() const { return myP1; }

  const gp_Pnt2d& P2() const { return myP2; }

  const TopoDS_Shape& Shape() const { return myShape; }

  void Shape(const TopoDS_Shape& S) { myShape = S; }

  bool Rg1Line() const { return myRg1Line; }

  void Rg1Line(const bool B) { myRg1Line = B; }

  bool RgNLine() const { return myRgNLine; }

  void RgNLine(const bool B) { myRgNLine = B; }

  bool OutLine() const { return myOutLine; }

  void OutLine(const bool B) { myOutLine = B; }

  bool IntLine() const { return myIntLine; }

  void IntLine(const bool B) { myIntLine = B; }

private:
  gp_Pnt2d     myP1;
  gp_Pnt2d     myP2;
  TopoDS_Shape myShape;
  bool         myRg1Line;
  bool         myRgNLine;
  bool         myOutLine;
  bool         myIntLine;
};

#endif // _HLRBRep_BiPnt2D_HeaderFile
