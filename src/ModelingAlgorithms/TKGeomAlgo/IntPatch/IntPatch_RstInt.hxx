// Created on: 1993-05-07
// Created by: Jacques GOUSSARD
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

#ifndef _IntPatch_RstInt_HeaderFile
#define _IntPatch_RstInt_HeaderFile

#include <Adaptor3d_Surface.hxx>

class IntPatch_Line;
class Adaptor3d_TopolTool;

//! trouver les points d intersection entre la ligne de
//! cheminement et les arcs de restriction
class IntPatch_RstInt
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void PutVertexOnLine(const Handle(IntPatch_Line)&       L,
                                              const Handle(Adaptor3d_Surface)&   Surf,
                                              const Handle(Adaptor3d_TopolTool)& Domain,
                                              const Handle(Adaptor3d_Surface)&   OtherSurf,
                                              const Standard_Boolean             OnFirst,
                                              const Standard_Real                Tol);
};

#endif // _IntPatch_RstInt_HeaderFile
