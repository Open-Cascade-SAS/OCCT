// Created on: 2005-10-05
// Created by: Mikhail KLOKOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_BaseRangeSample_HeaderFile
#define _IntTools_BaseRangeSample_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

//! base class for range index management
class IntTools_BaseRangeSample 
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntTools_BaseRangeSample();

  Standard_EXPORT IntTools_BaseRangeSample(const Standard_Integer theDepth);

  void SetDepth (const Standard_Integer theDepth) { myDepth = theDepth; }

  Standard_Integer GetDepth() const { return myDepth; }

private:

  Standard_Integer myDepth;

};

#endif // _IntTools_BaseRangeSample_HeaderFile
