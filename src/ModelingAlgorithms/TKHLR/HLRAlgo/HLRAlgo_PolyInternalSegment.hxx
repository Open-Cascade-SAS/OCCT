// Created on: 1996-12-03
// Created by: Christophe MARION
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

#ifndef _HLRAlgo_PolyInternalSegment_HeaderFile
#define _HLRAlgo_PolyInternalSegment_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>

//! to Update OutLines.
struct HLRAlgo_PolyInternalSegment
{
  DEFINE_STANDARD_ALLOC

  Standard_Integer LstSg1, LstSg2, NxtSg1, NxtSg2, Conex1, Conex2;
};

#endif // _HLRAlgo_PolyInternalSegment_HeaderFile
