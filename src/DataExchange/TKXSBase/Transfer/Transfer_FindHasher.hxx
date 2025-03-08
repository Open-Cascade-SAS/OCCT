// Created on: 1994-11-04
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Transfer_FindHasher_HeaderFile
#define _Transfer_FindHasher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Transfer_Finder.hxx>
#include <Standard_Handle.hxx>

//! FindHasher defines HashCode for Finder, which is : ask a
//! Finder its HashCode !  Because this is the Finder itself which
//! brings the HashCode for its Key
//!
//! This class complies to the template given in TCollection by
//! MapHasher itself
class Transfer_FindHasher
{
public:
  size_t operator()(const Handle(Transfer_Finder)& theFinder) const
  {
    return theFinder->GetHashCode();
  }

  //! Returns True if two keys are the same.
  //! The test does not work on the Finders themselves but by
  //! calling their methods Equates
  bool operator()(const Handle(Transfer_Finder)& theK1, const Handle(Transfer_Finder)& theK2) const
  {
    if (theK1.IsNull())
      return false;
    return theK1->Equates(theK2);
  }
};

#endif // _Transfer_FindHasher_HeaderFile
