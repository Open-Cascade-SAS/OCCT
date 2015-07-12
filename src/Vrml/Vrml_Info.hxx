// Created on: 1997-02-10
// Created by: Alexander BRIVIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Vrml_Info_HeaderFile
#define _Vrml_Info_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_OStream.hxx>
class TCollection_AsciiString;


//! defines a Info node of VRML specifying properties of geometry
//! and its appearance.
//! It  is  used  to  store  information  in  the  scene  graph,
//! Typically  for  application-specific  purposes,  copyright  messages,
//! or  other  strings.
class Vrml_Info 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Vrml_Info(const TCollection_AsciiString& aString = "<Undefined info>");
  
  Standard_EXPORT void SetString (const TCollection_AsciiString& aString);
  
  Standard_EXPORT TCollection_AsciiString String() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




protected:





private:



  TCollection_AsciiString myString;


};







#endif // _Vrml_Info_HeaderFile
