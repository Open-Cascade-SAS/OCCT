// Created on: 1995-02-08
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomInt_LineTool_HeaderFile
#define _GeomInt_LineTool_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <GeomInt_LineConstructor.hxx>
#include <IntPatch_SequenceOfLine.hxx>

class IntPatch_Point;
class IntPatch_WLine;

class GeomInt_LineTool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static Standard_Integer NbVertex(const Handle(IntPatch_Line)& L);

  Standard_EXPORT static const IntPatch_Point& Vertex(const Handle(IntPatch_Line)& L,
                                                      const Standard_Integer       I);

  Standard_EXPORT static Standard_Real FirstParameter(const Handle(IntPatch_Line)& L);

  Standard_EXPORT static Standard_Real LastParameter(const Handle(IntPatch_Line)& L);

  Standard_EXPORT static Standard_Boolean DecompositionOfWLine(
    const Handle(IntPatch_WLine)&      theWLine,
    const Handle(GeomAdaptor_Surface)& theSurface1,
    const Handle(GeomAdaptor_Surface)& theSurface2,
    const Standard_Real                aTolSum,
    const GeomInt_LineConstructor&     theLConstructor,
    IntPatch_SequenceOfLine&           theNewLines);

protected:
private:
};

#endif // _GeomInt_LineTool_HeaderFile
