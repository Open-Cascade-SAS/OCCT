// Created on: 1993-07-16
// Created by: Remi LEQUETTE
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

#ifndef _TopTools_LocationSet_HeaderFile
#define _TopTools_LocationSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopLoc_IndexedMapOfLocation.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
class Message_ProgressIndicator;
class Standard_OutOfRange;
class TopLoc_Location;


//! The class LocationSet stores a set of location in
//! a relocatable state.
//!
//! It can be created from Locations.
//!
//! It can create Locations.
//!
//! It can be write and read from a stream.
class TopTools_LocationSet 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns an empty set of locations.
  Standard_EXPORT TopTools_LocationSet();
  
  //! Clears the content of the set.
  Standard_EXPORT void Clear();
  
  //! Incorporate a new Location in the  set and returns
  //! its index.
  Standard_EXPORT Standard_Integer Add (const TopLoc_Location& L);
  
  //! Returns the location of index <I>.
  Standard_EXPORT const TopLoc_Location& Location (const Standard_Integer I) const;
  
  //! Returns the index of <L>.
  Standard_EXPORT Standard_Integer Index (const TopLoc_Location& L) const;
  
  //! Dumps the content of me on the stream <OS>.
  Standard_EXPORT void Dump (Standard_OStream& OS) const;
  
  //! Writes the content of  me  on the stream <OS> in a
  //! format that can be read back by Read.
  Standard_EXPORT void Write (Standard_OStream& OS) const;
  
  //! Reads the content of me from the  stream  <IS>. me
  //! is first cleared.
  Standard_EXPORT void Read (Standard_IStream& IS);
  
  Standard_EXPORT void SetProgress (const Handle(Message_ProgressIndicator)& PR);
  
  Standard_EXPORT Handle(Message_ProgressIndicator) GetProgress() const;




protected:





private:



  TopLoc_IndexedMapOfLocation myMap;
  Handle(Message_ProgressIndicator) myProgress;


};







#endif // _TopTools_LocationSet_HeaderFile
