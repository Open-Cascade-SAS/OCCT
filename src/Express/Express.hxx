// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#ifndef _Express_HeaderFile
#define _Express_HeaderFile

#include <Standard_Boolean.hxx>
#include <Standard_Type.hxx>
#include <Standard_OStream.hxx>

class Express_Schema;
class TCollection_AsciiString;

//! Provides data structures for representation of EXPRESS schema
//! (items, types, entities etc.)
//! and tools for generating XSTEP classes (HXX and CXX) from
//! items of the schema
class Express
{
public:

  DEFINE_STANDARD_ALLOC

  //! Returns (modifiable) handle to static schema object
  Standard_EXPORT static Handle(Express_Schema)& Schema();

  //! Writes standard copyright stamp (creation date/time, user, etc.)
  Standard_EXPORT static void WriteFileStamp (Standard_OStream& theOS);

  //! Writes standard comment for method in CXX file
  Standard_EXPORT static void WriteMethodStamp (Standard_OStream& theOS, const TCollection_AsciiString& theName);

  //! Converts item name from CASCADE to STEP style
  //! e.g. BoundedCurve -> bounded_curve
  Standard_EXPORT static TCollection_AsciiString ToStepName (const TCollection_AsciiString& theName);

  //! Converts item name from CASCADE to STEP style
  //! e.g. BoundedCurve -> bounded_curve
  Standard_EXPORT static TCollection_AsciiString EnumPrefix (const TCollection_AsciiString& theName);
};

#endif // _Express_HeaderFile
