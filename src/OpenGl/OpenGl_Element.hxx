// Created on: 2011-08-05
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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

#ifndef OpenGl_Element_Header
#define OpenGl_Element_Header

#include <Handle_OpenGl_Context.hxx>
#include <Handle_OpenGl_Workspace.hxx>
#include <OpenGl_RenderFilter.hxx>

//! Base interface for drawable elements.
class OpenGl_Element
{
public:

  Standard_EXPORT OpenGl_Element();

  virtual void Render (const Handle(OpenGl_Workspace)& theWorkspace) const = 0;

  //! Release GPU resources.
  virtual void Release (const Handle(OpenGl_Context)& theContext) = 0;

  template <typename theResource_t>
  static void Destroy (const Handle(OpenGl_Context)& theContext,
                       theResource_t*&               theElement)
  {
    if (theElement == NULL)
    {
      return;
    }

    theElement->Release (theContext);
    OpenGl_Element* anElement = theElement;
    delete anElement;
    theElement = NULL;
  }

public:

  //! Render element if it passes the filtering procedure. This method should
  //! be used for elements which can be used in scope of rendering algorithms.
  //! E.g. elements of groups during recursive rendering.
  //! If render filter is null, pure rendering is performed.
  //! @param theWorkspace [in] the rendering workspace.
  //! @param theFilter [in] the rendering filter to check whether the element
  //! should be rendered or not.
  //! @return True if element passes the filering check and is rendered.
  inline Standard_Boolean
    RenderFiltered (const Handle(OpenGl_Workspace)& theWorkspace,
                    const Handle(OpenGl_RenderFilter)& theFilter) const
  {
    if (!theFilter.IsNull() && !theFilter->CanRender (this))
    {
      return Standard_False;
    }

    Render (theWorkspace);

    return Standard_True;
  }

protected:

  Standard_EXPORT virtual ~OpenGl_Element();

public:

  DEFINE_STANDARD_ALLOC

};

#endif // OpenGl_Element_Header
