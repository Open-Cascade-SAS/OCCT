// Created on: 2016-02-19
// Created by: Kirill Gavrilov
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <Graphic3d_ShaderAttribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ShaderAttribute,Standard_Transient)

// =======================================================================
// function : Graphic3d_ShaderAttribute
// purpose  :
// =======================================================================
Graphic3d_ShaderAttribute::Graphic3d_ShaderAttribute (const TCollection_AsciiString& theName,
                                                      const int theLocation)
: myName (theName),
  myLocation (theLocation)
{
  //
}

// =======================================================================
// function : ~Graphic3d_ShaderAttribute
// purpose  :
// =======================================================================
Graphic3d_ShaderAttribute::~Graphic3d_ShaderAttribute()
{
  //
}
