// Created on: 2013-09-26
// Created by: Dmitry BOBYLEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_StencilTest.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Workspace.hxx>
#include <Standard_Dump.hxx>

OpenGl_StencilTest::OpenGl_StencilTest()
{
  //
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_StencilTest::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
  if (myIsEnabled)
  {
    aCtx->core11fwd->glEnable (GL_STENCIL_TEST);
    aCtx->core11fwd->glStencilFunc (GL_NOTEQUAL, 1, 0xFF);
  }
  else
  {
    aCtx->core11fwd->glDisable (GL_STENCIL_TEST);
  }
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_StencilTest::Release (OpenGl_Context*)
{
  //
}

// =======================================================================
// function : SetStencilTestState
// purpose  :
// =======================================================================
void OpenGl_StencilTest::SetOptions (const Standard_Boolean theIsEnabled)
{
  myIsEnabled = theIsEnabled;
}

// =======================================================================
// function : ~OpenGl_StencilTest
// purpose  :
// =======================================================================
OpenGl_StencilTest::~OpenGl_StencilTest()
{
  //
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void OpenGl_StencilTest::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN (theOStream, OpenGl_StencilTest)

  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, OpenGl_Element)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsEnabled)
}
