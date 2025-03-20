// Created on: 1998-12-04
// Created by: DUSUZEAU Louis
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Resource_LexicalCompare_HeaderFile
#define _Resource_LexicalCompare_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class TCollection_AsciiString;

class Resource_LexicalCompare
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Resource_LexicalCompare();

  //! Returns True if <Left> is lower than <Right>.
  Standard_EXPORT Standard_Boolean IsLower(const TCollection_AsciiString& Left,
                                           const TCollection_AsciiString& Right) const;

protected:
private:
};

#endif // _Resource_LexicalCompare_HeaderFile
