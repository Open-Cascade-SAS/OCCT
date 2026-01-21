// Created on: 1992-04-06
// Created by: Christian CAILLET
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

#ifndef _IGESData_DirPart_HeaderFile
#define _IGESData_DirPart_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IGESData_IGESType;

//! literal/numeric description of an entity's directory section, taken from file
class IGESData_DirPart
{
public:
  DEFINE_STANDARD_ALLOC

  //! creates an empty DirPart, ready to be filled by Init
  Standard_EXPORT IGESData_DirPart();

  //! fills DirPart with consistent data read from file
  Standard_EXPORT void Init(const int         i1,
                            const int         i2,
                            const int         i3,
                            const int         i4,
                            const int         i5,
                            const int         i6,
                            const int         i7,
                            const int         i8,
                            const int         i9,
                            const int         i19,
                            const int         i11,
                            const int         i12,
                            const int         i13,
                            const int         i14,
                            const int         i15,
                            const int         i16,
                            const int         i17,
                            const char* const res1,
                            const char* const res2,
                            const char* const label,
                            const char* const subscript);

  //! returns values recorded in DirPart
  //! (content of cstrings are modified)
  Standard_EXPORT void Values(int&  i1,
                              int&  i2,
                              int&  i3,
                              int&  i4,
                              int&  i5,
                              int&  i6,
                              int&  i7,
                              int&  i8,
                              int&  i9,
                              int&  i19,
                              int&  i11,
                              int&  i12,
                              int&  i13,
                              int&  i14,
                              int&  i15,
                              int&  i16,
                              int&  i17,
                              char* res1,
                              char* res2,
                              char* label,
                              char* subscript) const;

  //! returns "type" and "form" info, used to recognize the entity
  Standard_EXPORT IGESData_IGESType Type() const;

private:
  int  thevals[17];
  char theres1[10];
  char theres2[10];
  char thelabl[10];
  char thesubs[10];
};

#endif // _IGESData_DirPart_HeaderFile
