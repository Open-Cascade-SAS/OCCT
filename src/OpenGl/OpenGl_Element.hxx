// Created on: 2011-08-05
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

#ifndef OpenGl_Element_Header
#define OpenGl_Element_Header

#include <OpenGl_RenderFilter.hxx>
#include <Standard_Type.hxx>

class OpenGl_Workspace;
class OpenGl_Context;

//! Base interface for drawable elements.
class OpenGl_Element
{
public:

  Standard_EXPORT OpenGl_Element();

  virtual void Render (const Handle(OpenGl_Workspace)& theWorkspace) const = 0;

  //! Release GPU resources.
  //! Pointer to the context is used because this method might be called
  //! when the context is already being destroyed and usage of a handle
  //! would be unsafe
  virtual void Release (OpenGl_Context* theContext) = 0;

  //! Pointer to the context is used because this method might be called
  //! when the context is already being destroyed and usage of a handle
  //! would be unsafe
  template <typename theResource_t>
  static void Destroy (OpenGl_Context* theContext,
                       theResource_t*& theElement)
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

  //! Return TRUE if primitive type generates shaded triangulation (to be used in filters).
  virtual Standard_Boolean IsFillDrawMode() const { return false; }

  //! Update parameters of the drawable elements.
  virtual void SynchronizeAspects() {}

protected:

  Standard_EXPORT virtual ~OpenGl_Element();

public:

  DEFINE_STANDARD_ALLOC

};

#endif // OpenGl_Element_Header
