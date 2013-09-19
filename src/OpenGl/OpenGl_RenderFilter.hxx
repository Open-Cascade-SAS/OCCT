// Created on: 2013-07-25
// Created by: Anton POLETAEV
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

#ifndef _OpenGl_RenderFilter_H__
#define _OpenGl_RenderFilter_H__

#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>

DEFINE_STANDARD_HANDLE (OpenGl_RenderFilter, Standard_Transient)

class OpenGl_Element;

//! Base class for defining element rendering filters.
//! This class can be used in pair with advance rendering passes, and for 
//! disabling rendering (setting up) graphical aspects.
class OpenGl_RenderFilter : public Standard_Transient
{
public:

  //! Checks whether the element can be rendered or not.
  //! @param theElement [in] the element to check.
  //! @return True if element can be rendered.
  virtual Standard_Boolean CanRender (const OpenGl_Element* theElement) = 0;

public:

  DEFINE_STANDARD_RTTI(OpenGl_RenderFilter)
};

#endif
