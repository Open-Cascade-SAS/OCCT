// Created on: 2013-09-26
// Created by: Dmitry BOBYLEV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_StencilTest.hxx>

OpenGl_StencilTest::OpenGl_StencilTest()
{
  //
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_StencilTest::Render (const Handle(OpenGl_Workspace)&) const
{
  if (myIsEnabled)
  {
    glEnable (GL_STENCIL_TEST);
    glStencilFunc (GL_NOTEQUAL, 1, 0xFF);
  }
  else
  {
    glDisable (GL_STENCIL_TEST);
  }
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_StencilTest::Release (const Handle(OpenGl_Context)&)
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