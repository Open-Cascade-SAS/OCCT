// Created on: 1993-01-28
// Created by: Laurent BUCHARD
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

#ifndef _IntPatch_PrmPrmIntersection_T3Bits_HeaderFile
#define _IntPatch_PrmPrmIntersection_T3Bits_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IntPatch_PrmPrmIntersection_T3Bits
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntPatch_PrmPrmIntersection_T3Bits(const Standard_Integer size);

  Standard_EXPORT ~IntPatch_PrmPrmIntersection_T3Bits();

  void Add(const Standard_Integer t) { p[t >> 5] |= (1 << (((unsigned int)t) & 31)); }

  Standard_Integer Val(const Standard_Integer t) const
  {
    return (p[t >> 5] & (1 << (((unsigned int)t) & 31)));
  }

  void Raz(const Standard_Integer t) { p[t >> 5] &= ~(1 << (((unsigned int)t) & 31)); }

  Standard_EXPORT void ResetAnd();

  Standard_EXPORT Standard_Integer And(IntPatch_PrmPrmIntersection_T3Bits& Oth,
                                       Standard_Integer&                   indiceprecedent);

private:
  Standard_Integer* p;
  Standard_Integer  Isize;
};

#endif // _IntPatch_PrmPrmIntersection_T3Bits_HeaderFile
