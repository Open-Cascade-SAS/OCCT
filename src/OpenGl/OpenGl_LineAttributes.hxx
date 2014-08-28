// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_LineAttributes_Header
#define _OpenGl_LineAttributes_Header

#include <OpenGl_Resource.hxx>

#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Font_FontAspect.hxx>

class OpenGl_Context;
class Handle(OpenGl_Context);

class OpenGl_LineAttributes : public OpenGl_Resource
{
public:

  OpenGl_LineAttributes();
  virtual ~OpenGl_LineAttributes();

  void Init (const Handle(OpenGl_Context)& theGlCtx);
  virtual void Release (OpenGl_Context* theGlCtx);

  void SetTypeOfLine (const Aspect_TypeOfLine theType) const;

  void SetTypeOfHatch (const int theType) const;

protected:

  unsigned int myLinestyleBase;
  unsigned int myPatternBase;

public:

  DEFINE_STANDARD_RTTI(OpenGl_LineAttributes)

};

DEFINE_STANDARD_HANDLE(OpenGl_LineAttributes, OpenGl_Resource)

#endif // _OpenGl_LineAttributes_Header
