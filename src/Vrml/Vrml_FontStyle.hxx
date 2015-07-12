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

#ifndef _Vrml_FontStyle_HeaderFile
#define _Vrml_FontStyle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Vrml_FontStyleFamily.hxx>
#include <Vrml_FontStyleStyle.hxx>
#include <Standard_OStream.hxx>


//! defines a FontStyle node of VRML of properties of geometry
//! and its appearance.
//! The  size  field  specifies  the  height  (in  object  space  units)
//! of  glyphs  rendered  and  determines  the  vertical  spacing  of
//! adjacent  lines  of  text.
class Vrml_FontStyle 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Vrml_FontStyle(const Standard_Real aSize = 10, const Vrml_FontStyleFamily aFamily = Vrml_SERIF, const Vrml_FontStyleStyle aStyle = Vrml_NONE);
  
  Standard_EXPORT void SetSize (const Standard_Real aSize);
  
  Standard_EXPORT Standard_Real Size() const;
  
  Standard_EXPORT void SetFamily (const Vrml_FontStyleFamily aFamily);
  
  Standard_EXPORT Vrml_FontStyleFamily Family() const;
  
  Standard_EXPORT void SetStyle (const Vrml_FontStyleStyle aStyle);
  
  Standard_EXPORT Vrml_FontStyleStyle Style() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




protected:





private:



  Standard_Real mySize;
  Vrml_FontStyleFamily myFamily;
  Vrml_FontStyleStyle myStyle;


};







#endif // _Vrml_FontStyle_HeaderFile
