// Created on: 1996-12-23
// Created by: Alexander BRIVIN and Dmitry TARASOV
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Vrml_HeaderFile
#define _Vrml_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_OStream.hxx>
#include <Standard_CString.hxx>


//! Vrml package  implements the specification  of the
//! VRML ( Virtual  Reality Modeling Language ).  VRML
//! is a standard  language for describing interactive
//! 3-D objects and  worlds delivered across Internet.
//! Actual version of Vrml package have made for objects
//! of VRML version 1.0.
//! This package is used by VrmlConverter package.
//! The developer should  already be familiar with VRML
//! specification before using this package.
class Vrml 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Writes a header in anOStream (VRML file).
  //! Writes one line of commentary in  anOStream (VRML file).
  Standard_EXPORT static Standard_OStream& VrmlHeaderWriter (Standard_OStream& anOStream);
  
  Standard_EXPORT static Standard_OStream& CommentWriter (const Standard_CString aComment, Standard_OStream& anOStream);

};

#endif // _Vrml_HeaderFile
