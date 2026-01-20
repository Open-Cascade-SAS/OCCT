// Created on: 1992-05-13
// Created by: NW,JPB,CAL
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

#ifndef _Aspect_GenId_HeaderFile
#define _Aspect_GenId_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <Standard_Boolean.hxx>

//! This class permits the creation and control of integer identifiers.
class Aspect_GenId
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an available set of identifiers with the lower bound 0 and the upper bound INT_MAX
  //! / 2.
  Standard_EXPORT Aspect_GenId();

  //! Creates an available set of identifiers with specified range.
  //! Raises IdentDefinitionError if theUpper is less than theLow.
  Standard_EXPORT Aspect_GenId(const int theLow, const int theUpper);

  //! Free all identifiers - make the whole range available again.
  Standard_EXPORT void Free();

  //! Free specified identifier. Warning - method has no protection against double-freeing!
  Standard_EXPORT void Free(const int theId);

  //! Returns true if there are available identifiers in range.
  bool HasFree() const { return myFreeCount > 0 || myFreeIds.Extent() > 0; }

  //! Returns the number of available identifiers.
  int Available() const { return myFreeCount + myFreeIds.Extent(); }

  //! Returns the lower identifier in range.
  int Lower() const { return myLowerBound; }

  //! Returns the next available identifier.
  //! Warning: Raises IdentDefinitionError if all identifiers are busy.
  Standard_EXPORT int Next();

  //! Generates the next available identifier.
  //! @param[out] theId  generated identifier
  //! @return FALSE if all identifiers are busy.
  Standard_EXPORT bool Next(int& theId);

  //! Returns the upper identifier in range.
  int Upper() const { return myUpperBound; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

private:
  int      myFreeCount;
  int      myLength;
  int      myLowerBound;
  int      myUpperBound;
  NCollection_List<int> myFreeIds;
};

#endif // _Aspect_GenId_HeaderFile
