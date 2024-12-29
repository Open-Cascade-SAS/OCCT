// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2024 OPEN CASCADE SAS
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

#include <Standard_Type.hxx>

//============================================================================
Standard_Boolean Standard_Type::SubType (const Handle(Standard_Type)& theOther) const
{
  const Standard_Type* aTypeIter = this;
  while (aTypeIter != nullptr)
  {
    if (*theOther == *aTypeIter)
    {
      return true;
    }
    aTypeIter = aTypeIter->Parent();
  }
  return false;
}

//============================================================================

Standard_Boolean Standard_Type::SubType (const Standard_CString theName) const
{
  const Standard_Type* aTypeIter = this;
  while (aTypeIter != nullptr)
  {
    if (IsEqual(theName, aTypeIter->Name()))
    {
      return true;
    }
    aTypeIter = aTypeIter->Parent();
  }
  return false;
}

// ------------------------------------------------------------------
// Print (me; s: in out OStream) returns OStream;
// ------------------------------------------------------------------
void Standard_Type::Print (Standard_OStream& AStream) const
{
  AStream << std::hex << (Standard_Address)this << " : " << std::dec << myName ;
}