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

#ifndef _Graphic3d_ShaderAttribute_HeaderFile
#define _Graphic3d_ShaderAttribute_HeaderFile

#include <Standard_Transient.hxx>
#include <NCollection_DefineAlloc.hxx>
#include <TCollection_AsciiString.hxx>

//! Describes custom vertex shader attribute.
class Graphic3d_ShaderAttribute : public Standard_Transient
{
public:

  //! Creates new attribute.
  Standard_EXPORT Graphic3d_ShaderAttribute (const TCollection_AsciiString& theName,
                                             const int theLocation);

  //! Destructor.
  Standard_EXPORT virtual ~Graphic3d_ShaderAttribute();

  //! Returns name of shader variable.
  const TCollection_AsciiString& Name() const
  {
    return myName;
  }

  //! Returns attribute location to be bound on GLSL program linkage stage.
  int Location() const
  {
    return myLocation;
  }

protected:

  TCollection_AsciiString myName;     //!< attribute name
  int                     myLocation; //!< attribute location

public:

  DEFINE_STANDARD_RTTIEXT(Graphic3d_ShaderAttribute,Standard_Transient)

};

DEFINE_STANDARD_HANDLE (Graphic3d_ShaderAttribute, Standard_Transient)

#endif // _Graphic3d_ShaderAttribute_HeaderFile
