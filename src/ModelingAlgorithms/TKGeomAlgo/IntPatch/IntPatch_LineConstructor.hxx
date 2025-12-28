// Created on: 1996-11-07
// Created by: Laurent BUCHARD
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

#ifndef _IntPatch_LineConstructor_HeaderFile
#define _IntPatch_LineConstructor_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <IntPatch_Line.hxx>
#include <NCollection_Sequence.hxx>

class Adaptor3d_TopolTool;

//! The intersections algorithms compute the intersection
//! on two surfaces and return the intersections lines as
//! IntPatch_Line.
class IntPatch_LineConstructor
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntPatch_LineConstructor(const int mode);

  Standard_EXPORT void Perform(const NCollection_Sequence<occ::handle<IntPatch_Line>>& SL,
                               const occ::handle<IntPatch_Line>&                       L,
                               const occ::handle<Adaptor3d_Surface>&                   S1,
                               const occ::handle<Adaptor3d_TopolTool>&                 D1,
                               const occ::handle<Adaptor3d_Surface>&                   S2,
                               const occ::handle<Adaptor3d_TopolTool>&                 D2,
                               const double                                            Tol);

  Standard_EXPORT int NbLines() const;

  Standard_EXPORT occ::handle<IntPatch_Line> Line(const int index) const;

private:
  NCollection_Sequence<occ::handle<IntPatch_Line>> slin;
};

#endif // _IntPatch_LineConstructor_HeaderFile
