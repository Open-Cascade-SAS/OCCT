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

#ifndef OpenGl_StencilTest_Header
#define OpenGl_StencilTest_Header

#include <OpenGl_Element.hxx>

class OpenGl_StencilTest : public OpenGl_Element
{
public:

  //! Default constructor
  OpenGl_StencilTest ();

  //! Render primitives to the window
  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;

  virtual void Release (const Handle(OpenGl_Context)& theContext);

  void SetOptions (const Standard_Boolean theIsEnabled);

protected:

  //! Destructor
  virtual ~OpenGl_StencilTest();

private:
  Standard_Boolean myIsEnabled;

public:

  DEFINE_STANDARD_ALLOC
};

#endif //OpenGl_StencilOptions_Header