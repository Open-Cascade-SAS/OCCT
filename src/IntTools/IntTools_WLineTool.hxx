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

#ifndef _IntTools_WLineTool_HeaderFile
#define _IntTools_WLineTool_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <IntPatch_WLine.hxx>
#include <IntPatch_SequenceOfLine.hxx>

class TopoDS_Face;
class GeomInt_LineConstructor;
class IntTools_Context;

//! IntTools_WLineTool provides set of static methods related to walking lines.
class IntTools_WLineTool
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static
    Standard_Boolean NotUseSurfacesForApprox(const TopoDS_Face& aF1,
                                             const TopoDS_Face& aF2,
                                             const Handle(IntPatch_WLine)& WL,
                                             const Standard_Integer ifprm,
                                             const Standard_Integer ilprm);

  Standard_EXPORT static
  Standard_Boolean DecompositionOfWLine(const Handle(IntPatch_WLine)& theWLine,
                                        const Handle(GeomAdaptor_Surface)&            theSurface1, 
                                        const Handle(GeomAdaptor_Surface)&            theSurface2,
                                        const TopoDS_Face&                             theFace1,
                                        const TopoDS_Face&                             theFace2,
                                        const GeomInt_LineConstructor&                 theLConstructor,
                                        const Standard_Boolean                         theAvoidLConstructor,
                                        const Standard_Real                            theTol,
                                        IntPatch_SequenceOfLine&                       theNewLines,
                                        const Handle(IntTools_Context)& );
};

#endif