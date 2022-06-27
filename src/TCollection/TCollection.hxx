// Created on: 1992-10-13
// Created by: Ramin BARRETO
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

#ifndef _TCollection_HeaderFile
#define _TCollection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

//! The package <TCollection> provides the services for the
//! transient basic data structures.
class TCollection 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Returns a  prime number greater than  <I> suitable
  //! to dimension a Map.  When  <I> becomes great there
  //! is  a  limit on  the  result (today  the  limit is
  //! around 1 000 000). This is not a limit of the number of
  //! items but a limit in the number  of buckets.  i.e.
  //! there will be more collisions  in  the map.
  Standard_EXPORT static Standard_Integer NextPrimeForMap (const Standard_Integer I);

};

#endif // _TCollection_HeaderFile
