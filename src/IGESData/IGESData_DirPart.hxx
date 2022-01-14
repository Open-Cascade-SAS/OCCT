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
  Standard_EXPORT void Init (const Standard_Integer i1, const Standard_Integer i2, const Standard_Integer i3, const Standard_Integer i4, const Standard_Integer i5, const Standard_Integer i6, const Standard_Integer i7, const Standard_Integer i8, const Standard_Integer i9, const Standard_Integer i19, const Standard_Integer i11, const Standard_Integer i12, const Standard_Integer i13, const Standard_Integer i14, const Standard_Integer i15, const Standard_Integer i16, const Standard_Integer i17, const Standard_CString res1, const Standard_CString res2, const Standard_CString label, const Standard_CString subscript);

  //! returns values recorded in DirPart
  //! (content of cstrings are modified)
  Standard_EXPORT void Values (Standard_Integer& i1, Standard_Integer& i2, Standard_Integer& i3, Standard_Integer& i4, Standard_Integer& i5, Standard_Integer& i6, Standard_Integer& i7, Standard_Integer& i8, Standard_Integer& i9, Standard_Integer& i19, Standard_Integer& i11, Standard_Integer& i12, Standard_Integer& i13, Standard_Integer& i14, Standard_Integer& i15, Standard_Integer& i16, Standard_Integer& i17, const Standard_CString res1, const Standard_CString res2, const Standard_CString label, const Standard_CString subscript) const;
  
  //! returns "type" and "form" info, used to recognize the entity
  Standard_EXPORT IGESData_IGESType Type() const;

private:

  Standard_Integer thevals[17];
  Standard_Character theres1[10];
  Standard_Character theres2[10];
  Standard_Character thelabl[10];
  Standard_Character thesubs[10];

};

#endif // _IGESData_DirPart_HeaderFile
