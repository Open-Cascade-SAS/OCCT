// Created:	Fri Nov 22 13:32:26 2002
// Author:	Andrey BETENEV
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

#include <Express_Reference.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Reference, Express_Item)

//=================================================================================================

Express_Reference::Express_Reference(const Standard_CString                         theName,
                                     const Handle(TColStd_HSequenceOfHAsciiString)& theTypes)
    : Express_Item(theName)
{
  myTypes = theTypes;
}

//=================================================================================================

Standard_Boolean Express_Reference::GenerateClass() const
{
  return Standard_False;
}

//=================================================================================================

void Express_Reference::PropagateUse() const {}
