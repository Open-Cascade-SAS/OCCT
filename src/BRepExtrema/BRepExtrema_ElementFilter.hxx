// Created on: 2015-05-07
// Created by: Denis BOGOLEPOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _BRepExtrema_ElementFilter_HeaderFile
#define _BRepExtrema_ElementFilter_HeaderFile

#include <Standard_TypeDef.hxx>

//! Filtering tool used to detect if two given mesh elements
//! should be tested for overlapping/intersection or not.
struct BRepExtrema_ElementFilter
{
  //! Result of filtering function.
  enum FilterResult
  {
    NoCheck,
    Overlap,
    DoCheck
  };

  //! Releases resources of element filter.
  virtual ~BRepExtrema_ElementFilter()
  {
    //
  }

  //! Checks if two mesh elements should be tested for overlapping/intersection
  //! (used for detection correct/incorrect cases of shared edges and vertices).
  virtual FilterResult PreCheckElements (const Standard_Integer /*theIndex1*/,
                                         const Standard_Integer /*theIndex2*/)
  {
    return DoCheck;
  }
};

#endif // _BRepExtrema_ElementFilter_HeaderFile